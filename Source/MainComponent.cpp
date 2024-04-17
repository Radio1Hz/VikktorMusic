#include "MainComponent.h"
#include "DebugComponent.h"

//==============================================================================


MainComponent::MainComponent() : state(TransportState::Stopped),
audioSetupComp(
	deviceManager,
	0,     // minimum input channels
	256,   // maximum input channels
	0,     // minimum output channels
	256,   // maximum output channels
	false, // ability to select midi inputs
	false, // ability to select midi output device
	false, // treat channels as stereo pairs
	false), // hide advanced options)
	newDialogWindow(juce::String("New Project"), juce::Colours::black, true, false)
{
	// Make sure you set the size of the component after
	// you add any child components.
	setOpaque(true);

	formatManager.registerBasicFormats();
	setAudioChannels(0, 2); // no inputs, two outputs

	audioOnToggleButton.setButtonText("Audio On");
	audioOnToggleButton.onClick = [this] { updateToggleStateAudio(&audioOnToggleButton); };

	audioSettingsToggleButton.setButtonText("Settings");
	audioSettingsToggleButton.onClick = [this] { updateToggleStateSettings(&audioSettingsToggleButton); };

	audioVisualizeToggleButton.setButtonText("Visualize");
	audioVisualizeToggleButton.onClick = [this] { updateToggleStateVisualize(&audioVisualizeToggleButton); };

	if (!internalBufferAudioOn)
	{
		audioOnToggleButton.triggerClick();
	}

	playButton.setButtonText("Play");
	playButton.onClick = [this] { playButtonClicked(); };
	playButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
	playButton.setEnabled(false);

	stopButton.setButtonText("Stop");
	stopButton.onClick = [this] { stopButtonClicked(); };
	stopButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
	stopButton.setEnabled(false);

	loadButton.setButtonText("Load");
	loadButton.onClick = [this] { chooseFile(); };
	loadButton.setColour(juce::TextButton::buttonColourId, juce::Colours::blueviolet);
	loadButton.setEnabled(true);

	newButton.setButtonText("New");
	newButton.onClick = [this] { newButtonClicked(); };
	newButton.setColour(juce::TextButton::buttonColourId, juce::Colours::orangered);
	newButton.setEnabled(true);

	timeLabel.setText(displayProgress(0.0, 0.0), NotificationType::dontSendNotification);
	timeLabel.setJustificationType(Justification::centredRight);

	addAndMakeVisible(logSpaceComponent);
	addAndMakeVisible(audioOnToggleButton);
	addAndMakeVisible(&playButton);
	addAndMakeVisible(&stopButton);
	addAndMakeVisible(&loadButton);
	addAndMakeVisible(&newButton);
	addAndMakeVisible(timeLabel);
	addAndMakeVisible(audioSetupComp);
	addAndMakeVisible(debugComponent);
	addAndMakeVisible(audioSettingsToggleButton);
	addAndMakeVisible(audioVisualizeToggleButton);

	audioSetupComp.setVisible(false);
	audioSampleBlockImage = juce::Image(juce::Image::RGB, 240, 30, true);
	audioSampleBufferCopy.clear();

	//projectTree.addListener(this);

	//setupProjectTree();

	setSize(800, 600);
	startTimer(TimerType::CPUTimer, 250);
	startTimer(TimerType::SampleVisualization, 100);
	startTimer(TimerType::TimePositionTimer, 100);
}

void MainComponent::chooseFile()
{
	shutdownAudio();
	playButton.setEnabled(false);
	fileChooser = std::make_unique<FileChooser>("Please select the .wav file you want to load...", File("C:\\Data\\Samples"), "*.wav");

	auto folderChooserFlags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles;

	fileChooser->launchAsync(folderChooserFlags, [this](const FileChooser& chooser)
		{
			File file(chooser.getResult());

			if (file != juce::File{})
			{
				std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
				if (reader != nullptr)
				{
					if (reader->sampleRate != internalBufferSampleRate)
					{
						AudioSampleBuffer tempInBuffer, tempOutBuffer;
						tempInBuffer.setSize(reader->numChannels, (int)reader->lengthInSamples);
						reader->read(tempInBuffer.getArrayOfWritePointers(), reader->numChannels, 0, (int)reader->lengthInSamples);

						tempOutBuffer.setSize(reader->numChannels, (int)((internalBufferSampleRate / reader->sampleRate) * reader->lengthInSamples));
						this->internalBufferTotalLengthInSeconds = (float)reader->lengthInSamples / reader->sampleRate;
						internalAudioBuffer.setSize(reader->numChannels, tempOutBuffer.getNumSamples());

						LagrangeInterpolator resampler = LagrangeInterpolator();
						for (unsigned int i = 0; i < reader->numChannels; i++)
						{
							resampler.process(reader->sampleRate / internalBufferSampleRate, tempInBuffer.getReadPointer(i), tempOutBuffer.getWritePointer(i), tempOutBuffer.getNumSamples());
							internalAudioBuffer.copyFrom(i, 0, tempOutBuffer.getReadPointer(i), tempOutBuffer.getNumSamples());
						}
					}
					else
					{
						this->internalBufferTotalLengthInSeconds = (float)reader->lengthInSamples / reader->sampleRate;
						internalAudioBuffer.setSize(reader->numChannels, (int)reader->lengthInSamples);
						reader->read(&internalAudioBuffer, 0, (int)reader->lengthInSamples, 0, true, true);
					}

					setAudioChannels(0, (int)reader->numChannels);
					playButton.setEnabled(true);
				}
				this->internalBufferCurrentPositionInSeconds = 0.0;
				this->internalBufferCurrentSamplePlaying = 0;

				ReadSamplesToImage();
			}
		});
}

void MainComponent::newButtonClicked()
{
	shutdownAudio();
	playButton.setEnabled(false);
	setNewProject(120, 10);
}


void MainComponent::setNewProject(int bpm, int measures)
{
	double newLengthInSeconds = (60.0/bpm) * 4 * measures;
	internalAudioBuffer.setSize(2, (int)(internalBufferSampleRate * newLengthInSeconds), false, true, false);

	internalBufferTotalLengthInSeconds = newLengthInSeconds;
	internalBufferCurrentPositionInSeconds = 0.0;
	internalBufferCurrentSamplePlaying = 0;
	setAudioChannels(0, 2);
	playButton.setEnabled(true);
	ReadSamplesToImage();
}

void MainComponent::ReadSamplesToImage()
{
	int numberOfSamples = internalAudioBuffer.getNumSamples();
	Graphics g0(internalBufferSamplesImage0);
	Graphics g1(internalBufferSamplesImage1);
	g0.fillAll(Colours::black);
	g1.fillAll(Colours::black);
	int sampleIndex = 0;
	int prevSampleIndex = 0;

	// Draw signal in images Channel 0
	for (int i = 1; i < internalBufferSamplesImage0.getWidth(); i++)
	{
		sampleIndex = (int)((i * 1.0f / internalBufferSamplesImage0.getWidth() * 1.0f) * numberOfSamples);
		int sampleRange = sampleIndex - prevSampleIndex;
		auto valueRange = internalAudioBuffer.findMinMax(0, prevSampleIndex, sampleRange);
		g0.setColour(Colours::darkgrey);
		g0.drawLine((float)i, (0.5f - valueRange.getStart() / 2) * internalBufferSamplesImage0.getHeight(), (float)i, (0.5f - valueRange.getEnd() / 2) * internalBufferSamplesImage0.getHeight(), 1.0f);
		prevSampleIndex = sampleIndex;
	}

	prevSampleIndex = 0;

	if (internalAudioBuffer.getNumChannels() > 1)
	{
		// Draw signal in images Channel 1
		for (int i = 1; i < internalBufferSamplesImage1.getWidth(); i++)
		{
			sampleIndex = (int)((i * 1.0f / internalBufferSamplesImage1.getWidth() * 1.0f) * numberOfSamples);
			int sampleRange = sampleIndex - prevSampleIndex;
			auto valueRange = internalAudioBuffer.findMinMax(1, prevSampleIndex, sampleRange);
			g1.setColour(Colours::darkgrey);
			g1.drawLine((float)i, (0.5f - valueRange.getStart() / 2) * internalBufferSamplesImage1.getHeight(), (float)i, (0.5f - valueRange.getEnd() / 2) * internalBufferSamplesImage1.getHeight(), 1.0f);
			prevSampleIndex = sampleIndex;
		}
	}
	repaint();
}


void MainComponent::ReadSamplesToAudioSampleBufferImage()
{
	Graphics g(audioSampleBlockImage);
	g.fillAll(Colours::darkred);
	g.setColour(Colours::lightgrey);
	int prevSampleIndex = 0;
	Point<float> prevPoint(0.0f, (float)audioSampleBlockImage.getHeight() / 2);

	for (int i = 1; i < audioSampleBlockImage.getWidth(); i++)
	{
		int sampleIndex = (int)(((float)i / (float)audioSampleBlockImage.getWidth()) * audioSampleBufferCopy.size());
		float sample = audioSampleBufferCopy[sampleIndex];

		Point<float> nextPoint((float)i, (0.5f - sample / 2) * audioSampleBlockImage.getHeight());
		g.drawLine(Line<float>(prevPoint, nextPoint));
		prevPoint.setX(nextPoint.getX());
		prevPoint.setY(nextPoint.getY());
		prevSampleIndex = sampleIndex;
	}
}

MainComponent::~MainComponent()
{
	shutdownAudio();
	removeAllChildren();
	deleteAllChildren();
}

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* /*source*/)
{

}

void MainComponent::playButtonClicked()
{
	if ((state == Stopped) || (state == Paused))
		changeState(Playing);
	else if (state == Playing)
		changeState(Paused);
}

void MainComponent::stopButtonClicked()
{
	changeState(Stopped);
}

void MainComponent::setupProjectTree()
{
	ValueTree projectInfo("projectInfo");
	projectInfo.setProperty("name", var("Untitled document"), nullptr);
	projectInfo.setProperty("bpm", var(120), nullptr);
	projectTree.addChild(projectInfo, -1, nullptr);

	FileOutputStream stream = FileOutputStream(File("C:\\Data\\VikktorMusic\\proj.proj"));
	projectTree.writeToStream(stream);
	stream.flush();
}

void MainComponent::changeState(TransportState newState)
{
	if (state != newState)
	{
		state = newState;

		switch (state)
		{
		case Stopped:
			playButton.setButtonText("Play");
			stopButton.setButtonText("Stop");
			stopButton.setEnabled(false);
			internalBufferCurrentPositionInSeconds = 0.0;
			internalBufferCurrentSamplePlaying = 0;
			internalBufferPointerRasterX = 0;
			repaint();
			break;

		case Playing:
			playButton.setButtonText("Pause");
			stopButton.setButtonText("Stop");
			stopButton.setEnabled(true);
			break;

		case Paused:
			playButton.setButtonText("Resume");
			stopButton.setButtonText("Reset");
			break;
		}
	}
}

void MainComponent::valueTreePropertyChanged(ValueTree& /*treeWhosePropertyHasChanged*/, const Identifier& /*property*/)
{
}

void MainComponent::valueTreeChildAdded(ValueTree& /*parentTree*/, ValueTree& /*childWhichHasBeenAdded*/)
{
}

void MainComponent::valueTreeChildRemoved(ValueTree& /*parentTree*/, ValueTree& /*childWhichHasBeenRemoved*/, int /*indexFromWhichChildWasRemoved*/)
{
}

void MainComponent::valueTreeChildOrderChanged(ValueTree& /*parentTreeWhoseChildrenHaveMoved*/, int /*oldIndex*/, int /*newIndex*/)
{
}

void MainComponent::valueTreeParentChanged(ValueTree& /*treeWhoseParentHasChanged*/)
{
}

void MainComponent::valueTreeRedirected(ValueTree& /*treeWhichHasBeenChanged*/)
{
}

void MainComponent::timerCallback(int timerID)
{
	if (timerID == (int)TimerType::CPUTimer)
	{
		double cpuUsage = deviceManager.getCpuUsage();
		float bufferLengthms = 1000.0f * (internalBufferSamplesPerBlock / (float)internalBufferSampleRate);
		bufferLengthms = std::round(bufferLengthms * 10) / 10;
		debugComponent.setInformation("CPU: " + juce::String((int)(cpuUsage * 100.00)) + "% Audio Block processed in " + juce::String(audioBlockProcessedTimeInMilliseconds) + " / " + juce::String(bufferLengthms) + " ms");
	}

	if (timerID == (int)TimerType::TimePositionTimer)
	{
		timeLabel.setText(displayProgress(internalBufferCurrentPositionInSeconds, internalBufferTotalLengthInSeconds), NotificationType::dontSendNotification);
		if (internalBufferTotalLengthInSeconds > 0.0)
		{
			double percent = internalBufferCurrentPositionInSeconds / internalBufferTotalLengthInSeconds;
			int newRasterX = (int)((double)internalBufferSamplesImage0.getWidth() * percent);
			if (newRasterX != internalBufferPointerRasterX)
			{
				internalBufferPointerRasterX = newRasterX;
				if (internalBufferCurrentSamplePlaying == 0)
				{
					changeState(Stopped);
				}
			}

		}
	}

	if (timerID == (int)TimerType::SampleVisualization)
	{
		if (audioSampleBufferCopyNumSamples >= audioSampleBufferCopy.size())
		{
			ReadSamplesToAudioSampleBufferImage();
			audioSampleBufferCopyNumSamples = 0;
		}
	}
	repaint();

}

void MainComponent::updateToggleStateAudio(juce::Button* button)
{
	bool stateOn = button->getToggleState();
	if (stateOn)
	{
		setAudioOn();
	}
	else
	{
		setAudioOff();
	}
}

void MainComponent::updateToggleStateSettings(juce::Button* button)
{
	audioSetupComp.setVisible(button->getToggleState());
}

void MainComponent::updateToggleStateVisualize(juce::Button* button)
{
	this->shouldVisualize = button->getToggleState();
	repaint();
}

juce::String MainComponent::displayProgress(double currentPositionInSeconds, double totalLengthInSeconds)
{
	RelativeTime currentTime = RelativeTime(currentPositionInSeconds);
	RelativeTime totalTime = RelativeTime(totalLengthInSeconds);

	int currentSec = 100 + (int)currentPositionInSeconds % 60;
	int currentMin = 100 + (int)currentTime.inMinutes() % 60;
	int currentHr = 100 + (int)currentTime.inHours();

	int totalSec = 100 + (int)totalLengthInSeconds % 60;
	int totalMin = 100 + (int)totalTime.inMinutes() % 60;
	int totalHr = 100 + (int)totalTime.inHours();

	return juce::String(currentHr).substring(1) + ":" + juce::String(currentMin).substring(1) + ":" + juce::String(currentSec).substring(1) + " / " + juce::String(totalHr).substring(1) + ":" + juce::String(totalMin).substring(1) + ":" + juce::String(totalSec).substring(1);
}

void MainComponent::setAudioOn()
{
	if (!internalBufferAudioOn)
	{
		internalBufferAudioOn = true;
	}
}

void MainComponent::setAudioOff()
{
	// This shuts down the audio device and clears the audio source.
	if (internalBufferAudioOn)
	{
		internalBufferAudioOn = false;
	}
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	internalBufferSampleRate = sampleRate;
	internalBufferSamplesPerBlock = samplesPerBlockExpected;
	audioSampleBufferCopy.resize(internalBufferSamplesPerBlock * 5);
	repaint();
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
	int64 counterHiRes = (int64)Time::getMillisecondCounterHiRes();
	if (state != Stopped && state != Paused)
	{
		if (internalAudioBuffer.getNumSamples() > 0 && internalAudioBuffer.getNumChannels() > 0)
		{
			auto numOutputChannels = bufferToFill.buffer->getNumChannels();

			auto outputSamplesRemaining = bufferToFill.numSamples;
			auto outputSamplesOffset = bufferToFill.startSample;

			while (outputSamplesRemaining > 0)
			{
				auto bufferSamplesRemaining = internalAudioBuffer.getNumSamples() - internalBufferCurrentSamplePlaying;                // [10]
				auto samplesThisTime = juce::jmin(outputSamplesRemaining, bufferSamplesRemaining);

				for (auto channel = 0; channel < numOutputChannels; channel++)
				{
					if (!internalBufferAudioOn)
					{
						bufferToFill.clearActiveBufferRegion();
					}
					else
					{
						bufferToFill.buffer->copyFrom(channel, outputSamplesOffset, internalAudioBuffer, channel, internalBufferCurrentSamplePlaying, samplesThisTime);                                //  [12.5]
					}

					if (shouldVisualize && channel == 0 && audioSampleBufferCopyNumSamples < audioSampleBufferCopy.size())
					{
						for (int i = 0; i < samplesThisTime; i++)
						{
							audioSampleBufferCopy.set(audioSampleBufferCopyNumSamples++, bufferToFill.buffer->getSample(channel, bufferToFill.startSample + i));
							if (audioSampleBufferCopyNumSamples == audioSampleBufferCopy.size())
							{
								break;
							}
						}
					}
				}

				outputSamplesRemaining -= samplesThisTime;
				outputSamplesOffset += samplesThisTime;
				internalBufferCurrentSamplePlaying += samplesThisTime;
				internalBufferCurrentPositionInSeconds = (double)internalBufferCurrentSamplePlaying / internalBufferSampleRate;

				if (internalBufferCurrentSamplePlaying >= internalAudioBuffer.getNumSamples())
				{
					internalBufferCurrentSamplePlaying = 0;
					state = Stopped;
					audioSampleBufferCopy.clear();
				}
			}
		}
	}

	audioBlockProcessedTimeInMilliseconds = (int64)Time::getMillisecondCounterHiRes() - counterHiRes;

}

void MainComponent::releaseResources()
{
	juce::Logger::getCurrentLogger()->writeToLog("Releasing audio resources");
}

bool MainComponent::keyPressed(const KeyPress& /*key*/, Component* /*originatingComponent*/)
{
	return true;
}

//==============================================================================
void MainComponent::paint(juce::Graphics& g)
{
	// (Our component is opaque, so we must completely fill the background with a solid colour)
	g.fillAll(juce::Colours::black);
	Rectangle<float> rectMain = Rectangle<float>((float)getWidth(), 30.0f);
	g.setColour(Colour::fromRGB(255, 255, 255));
	rectMain.translate(0.0f, (float)getHeight() - 30.0f);
	float bufferLengthms = 1000.0f * (internalBufferSamplesPerBlock / (float)internalBufferSampleRate);
	bufferLengthms = std::round(bufferLengthms * 10) / 10;
	g.drawText("Sample Rate: " + String(internalBufferSampleRate) + ", Buffer Size: " + String(internalBufferSamplesPerBlock) + " (" + String(bufferLengthms) + " ms)", rectMain, Justification::left);
	int signalImagesOffsetFromBottom = 60;
	Rectangle<float> sampleBufferImageRect = Rectangle<float>((float)getWidth(), 30.0f);
	sampleBufferImageRect.translate(0, (float)getHeight() - signalImagesOffsetFromBottom);
	g.setColour(juce::Colours::darkgrey);
	g.drawImage(internalBufferSamplesImage0, sampleBufferImageRect);
	g.drawRect(sampleBufferImageRect);
	sampleBufferImageRect.translate(0, -30);
	g.drawImage(internalBufferSamplesImage1, sampleBufferImageRect);
	g.drawRect(sampleBufferImageRect);
	g.setColour(juce::Colours::lightyellow);

	juce::Point<int> topLeft0 = sampleBufferImageRect.getTopLeft().toInt();
	juce::Point<int> bottomRight1 = sampleBufferImageRect.getBottomRight().toInt();

	g.fillRect(internalBufferPointerRasterX, topLeft0.getY(), 1, 2 * (int)sampleBufferImageRect.getHeight());
	if (this->shouldVisualize)
	{
		Rectangle<float> audioSampleBlockImageRect = Rectangle<float>((float)audioSampleBlockImage.getWidth(), (float)audioSampleBlockImage.getHeight());
		audioSampleBlockImageRect.translate((float)getWidth() - audioSampleBlockImage.getWidth(), (float)getHeight() - audioSampleBlockImage.getHeight());
		g.drawImage(audioSampleBlockImage, audioSampleBlockImageRect);
	}
}

void MainComponent::resized()
{
	// This is called when the MainContentComponent is resized.
	// If you add any child components, this is where you should
	// update their positions.
	int controlsAreaOffsetFromBottom = 120;

	debugComponent.setBounds(0, 0, getWidth(), 30);
	logSpaceComponent.setBounds(0, 30, getWidth(), getHeight() - controlsAreaOffsetFromBottom - 30);
	audioSettingsToggleButton.setBounds(getWidth() - 80, getHeight() - controlsAreaOffsetFromBottom, 80, 30);
	audioVisualizeToggleButton.setBounds(getWidth() - 160, getHeight() - controlsAreaOffsetFromBottom, 80, 30);
	timeLabel.setBounds(getWidth() - 360, getHeight() - controlsAreaOffsetFromBottom, 160, 30);
	audioOnToggleButton.setBounds(0, getHeight() - controlsAreaOffsetFromBottom, 60, 30);
	loadButton.setBounds(60, getHeight() - controlsAreaOffsetFromBottom, 60, 30);
	playButton.setBounds(120, getHeight() - controlsAreaOffsetFromBottom, 60, 30);
	stopButton.setBounds(180, getHeight() - controlsAreaOffsetFromBottom, 60, 30);
	newButton.setBounds(240, getHeight() - controlsAreaOffsetFromBottom, 60, 30);

	internalBufferSamplesImage0 = juce::Image(juce::Image::RGB, getWidth(), 30, true);
	internalBufferSamplesImage1 = juce::Image(juce::Image::RGB, getWidth(), 30, true);

	internalBufferSamplesImage0.clear(internalBufferSamplesImage0.getBounds(), juce::Colours::black);
	internalBufferSamplesImage1.clear(internalBufferSamplesImage1.getBounds(), juce::Colours::black);

	if (internalAudioBuffer.getNumSamples())
	{
		ReadSamplesToImage();
	}

	if (getWidth() > 400)
	{
		audioSetupComp.setBounds(getWidth() - 400, getHeight() - controlsAreaOffsetFromBottom - 300, 400, 300);
	}
	else
	{
		audioSetupComp.setVisible(false);
	}
}
