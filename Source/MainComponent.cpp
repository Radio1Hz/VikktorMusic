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
	false) // hide advanced options)
{
	// Make sure you set the size of the component after
	// you add any child components.
	setOpaque(true);
	
	formatManager.registerBasicFormats();
	transportSource.addChangeListener(this);
	setAudioChannels(0, 2); // no inputs, two outputs

	audioOnToggleButton.setButtonText("Audio On");
	audioOnToggleButton.onClick = [this] { updateToggleStateAudio(&audioOnToggleButton); };

	audioSettingsToggleButton.setButtonText("Settings");
	audioSettingsToggleButton.onClick = [this] { updateToggleStateSettings(&audioSettingsToggleButton); };
	
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

	timeLabel.setText(juce::String(juce::RelativeTime::seconds(0.0).getDescription()), NotificationType::dontSendNotification);
	timeLabel.setJustificationType(Justification::centredRight);

	addAndMakeVisible(logSpaceComponent);
	addAndMakeVisible(audioOnToggleButton);
	addAndMakeVisible(&playButton);
	addAndMakeVisible(&stopButton);
	addAndMakeVisible(&loadButton);
	addAndMakeVisible(timeLabel);
	addAndMakeVisible(audioSetupComp);
	addAndMakeVisible(debugComponent);
	addAndMakeVisible(audioSettingsToggleButton);
	audioSetupComp.setVisible(false);
	setSize(800, 600);
	startTimer(100);
}

void MainComponent::chooseFile()
{
	fileChooser = std::make_unique<FileChooser>("Please select the .wav file you want to load...", File("C:\\Data\\Samples"), "*.wav");

	auto folderChooserFlags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles;

	fileChooser->launchAsync(folderChooserFlags, [this](const FileChooser& chooser)
	{
		File file(chooser.getResult());

		if (file != juce::File{})
		{
			AudioFormatReader* reader = formatManager.createReaderFor(file);
			WavAudioFormat wavFormat;
			if (reader != nullptr)
			{
				internalAudioBuffer.setSize(reader->numChannels, (int)reader->lengthInSamples);

				auto memoryMappedReader = wavFormat.createMemoryMappedReader(file);
				memoryMappedReader->mapEntireFile();

				memoryMappedReader->read(internalAudioBuffer.getArrayOfWritePointers(), reader->numChannels, 0, (int)reader->lengthInSamples);
				auto newSource = std::make_unique<juce::AudioFormatReaderSource>(memoryMappedReader, true);
				transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
				
				playButton.setEnabled(true);
				readerSource.reset(newSource.release());
				delete reader;
			}
			this->internalBufferTotalLengthInSeconds = transportSource.getLengthInSeconds();
			ReadSamplesToImage();
		}
	});
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
		
		sampleIndex = (int)((i*1.0f / internalBufferSamplesImage0.getWidth()*1.0f) * numberOfSamples);
		int sampleRange = sampleIndex - prevSampleIndex;
		auto valueRange = internalAudioBuffer.findMinMax(0, prevSampleIndex, sampleRange);
		g0.setColour(Colours::darkgrey);
		g0.drawLine((float)i, (0.5f - valueRange.getStart()/2) * internalBufferSamplesImage0.getHeight(), (float)i, (0.5f - valueRange.getEnd()/2) * internalBufferSamplesImage0.getHeight(), 1.0f);
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
			g1.drawLine((float)i, (0.5f - valueRange.getStart()/2) * internalBufferSamplesImage1.getHeight(), (float)i, (0.5f - valueRange.getEnd()/2) * internalBufferSamplesImage1.getHeight(), 1.0f);
			prevSampleIndex = sampleIndex;
		}
	}
	repaint();
}

MainComponent::~MainComponent()
{
	stopTimer();
	shutdownAudio();
	transportSource.setSource(nullptr);
	removeAllChildren();
	deleteAllChildren();
}

void MainComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
	if (source == &transportSource)
	{
		if (transportSource.isPlaying())
			changeState(Playing);
		else if ((state == Stopping) || (state == Playing))
			changeState(Stopped);
		else if (Pausing == state)
			changeState(Paused);
	}
}

void MainComponent::playButtonClicked()
{
	if ((state == Stopped) || (state == Paused))
		changeState(Starting);
	else if (state == Playing)
		changeState(Pausing);
}

void MainComponent::stopButtonClicked()
{
	if (state == Paused)
		changeState(Stopped);
	else
		changeState(Stopping);
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
			transportSource.setPosition(0.0);
			internalBufferCurrentPositionInSeconds = 0.0;
			internalBufferPointerRasterX = 0;
			repaint();
			break;

		case Starting:
			transportSource.start();
			break;

		case Playing:
			playButton.setButtonText("Pause");
			stopButton.setButtonText("Stop");
			stopButton.setEnabled(true);
			break;

		case Pausing:
			transportSource.stop();
			break;

		case Paused:
			playButton.setButtonText("Resume");
			stopButton.setButtonText("Reset");
			break;

		case Stopping:
			transportSource.stop();
			break;
		}
	}
}
void MainComponent::timerCallback()
{
	timeLabel.setText(juce::String(juce::RelativeTime::seconds(transportSource.getCurrentPosition()).getDescription()), NotificationType::dontSendNotification);
	if (transportSource.isPlaying() && internalBufferTotalLengthInSeconds > 0.0)
	{
		double percent = internalBufferCurrentPositionInSeconds / internalBufferTotalLengthInSeconds;
		int newRasterX = (int)((double)internalBufferSamplesImage0.getWidth() * percent);
		if (newRasterX != internalBufferPointerRasterX)
		{
			internalBufferPointerRasterX = newRasterX;
			repaint();
		}
	}
	double cpuUsage = deviceManager.getCpuUsage();
	debugComponent.setInformation("CPU: " + juce::String(cpuUsage*100.00) + "%");
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
	juce::String message;

	internalBufferSampleRate = sampleRate;
	internalBufferSamplesPerBlock = samplesPerBlockExpected;
	
	transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
	transportSource.setLooping(true);

	//-----------------------------------------------------------------------------

	juce::Logger::getCurrentLogger()->writeToLog(message);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
	if (readerSource.get() == nullptr)
	{
		bufferToFill.clearActiveBufferRegion();
		return;
	}

	if (internalBufferAudioOn)
	{
		transportSource.getNextAudioBlock(bufferToFill);
	}
	else
	{
		transportSource.getNextAudioBlock(bufferToFill);
		bufferToFill.clearActiveBufferRegion();
	}

	internalBufferCurrentPositionInSeconds = transportSource.getCurrentPosition();
}

void MainComponent::releaseResources()
{
	juce::Logger::getCurrentLogger()->writeToLog("Releasing audio resources");
	transportSource.releaseResources();
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
	g.drawText("Sample Rate: " + String(internalBufferSampleRate) + ", Samples per block: " + String(internalBufferSamplesPerBlock), rectMain, Justification::left);
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

}

void MainComponent::resized()
{
	// This is called when the MainContentComponent is resized.
	// If you add any child components, this is where you should
	// update their positions.
	int controlsAreaOffsetFromBottom = 120;

	debugComponent.setBounds(0, 0, getWidth(), 30);
	logSpaceComponent.setBounds(0, 30, getWidth(), getHeight() - controlsAreaOffsetFromBottom-30);
	audioSettingsToggleButton.setBounds(getWidth()-80, getHeight() - controlsAreaOffsetFromBottom, 80, 30);
	audioOnToggleButton.setBounds(0, getHeight() - controlsAreaOffsetFromBottom, 60, 30);
	stopButton.setBounds(180, getHeight() - controlsAreaOffsetFromBottom, 60, 30);
	loadButton.setBounds(60, getHeight() - controlsAreaOffsetFromBottom, 60, 30);
	playButton.setBounds(120, getHeight() - controlsAreaOffsetFromBottom, 60, 30);
	timeLabel.setBounds(240, getHeight() - controlsAreaOffsetFromBottom, 120, 30);

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
