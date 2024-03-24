#include "MainComponent.h"
#include "DebugComponent.h"

//==============================================================================


MainComponent::MainComponent() : state(TransportState::Stopped)
{
	// Make sure you set the size of the component after
	// you add any child components.
	setOpaque(true);
	
	formatManager.registerBasicFormats();
	transportSource.addChangeListener(this);
	setAudioChannels(0, 2); // no inputs, two outputs

	audioToggleButton.setButtonText("Audio On");
	audioToggleButton.onClick = [this] { updateToggleState(&audioToggleButton); };
	
	if (!audioOn)
	{
		audioToggleButton.triggerClick();
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

	addAndMakeVisible(logSpaceComponent, -1);
	addAndMakeVisible(audioToggleButton);
	addAndMakeVisible(&playButton);
	addAndMakeVisible(&stopButton);
	addAndMakeVisible(&loadButton);
	addAndMakeVisible(timeLabel);
	//addAndMakeVisible(debugComponent, -1);
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
			ReadSamplesToImage();
		}
	});
}

void MainComponent::ReadSamplesToImage()
{

	int numberOfSamples = internalAudioBuffer.getNumSamples();
	Graphics g0(sampleBufferImage0);
	Graphics g1(sampleBufferImage1);
	g0.fillAll(Colours::black);
	g1.fillAll(Colours::black);
	int sampleIndex = 0;
	int prevSampleIndex = 0;

	// Draw signal in images Channel 0
	for (int i = 1; i < sampleBufferImage0.getWidth(); i++)
	{
		
		sampleIndex = (int)((i*1.0f / sampleBufferImage0.getWidth()*1.0f) * numberOfSamples);
		int sampleRange = sampleIndex - prevSampleIndex;
		auto valueRange = internalAudioBuffer.findMinMax(0, prevSampleIndex, sampleRange);
		g0.setColour(Colours::darkgrey);
		g0.drawLine((float)i, (0.5f - valueRange.getStart()/2) * sampleBufferImage0.getHeight(), (float)i, (0.5f - valueRange.getEnd()/2) * sampleBufferImage0.getHeight(), 1.0f);
		prevSampleIndex = sampleIndex;
	}

	prevSampleIndex = 0;

	if (internalAudioBuffer.getNumChannels() > 1)
	{
		// Draw signal in images Channel 1
		for (int i = 1; i < sampleBufferImage1.getWidth(); i++)
		{
			sampleIndex = (int)((i * 1.0f / sampleBufferImage1.getWidth() * 1.0f) * numberOfSamples);
			int sampleRange = sampleIndex - prevSampleIndex;
			auto valueRange = internalAudioBuffer.findMinMax(1, prevSampleIndex, sampleRange);
			g1.setColour(Colours::darkgrey);
			g1.drawLine((float)i, (0.5f - valueRange.getStart()/2) * sampleBufferImage1.getHeight(), (float)i, (0.5f - valueRange.getEnd()/2) * sampleBufferImage1.getHeight(), 1.0f);
			prevSampleIndex = sampleIndex;

		}

	}
	repaint();
}

MainComponent::~MainComponent()
{
	shutdownAudio();
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
	repaint();
}

void MainComponent::updateToggleState(juce::Button* button)
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

void MainComponent::setAudioOn()
{
	if (!audioOn)
	{
		audioOn = true;
	}
}

void MainComponent::setAudioOff()
{
	// This shuts down the audio device and clears the audio source.
	if (audioOn)
	{
		audioOn = false;
	}
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	juce::String message;

	currentSampleRate = sampleRate;
	currentSamplesPerBlock = samplesPerBlockExpected;
	
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
	if (audioOn)
	{
		transportSource.getNextAudioBlock(bufferToFill);
	}
	else
	{
		transportSource.getNextAudioBlock(bufferToFill);
		bufferToFill.clearActiveBufferRegion();
	}
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
	g.drawText("Sample Rate: " + String(currentSampleRate) + ", Samples per block: " + String(currentSamplesPerBlock), rectMain, Justification::left);
	int signalImagesOffsetFromBottom = 60;
	Rectangle<float> sampleBufferImageRect = Rectangle<float>((float)getWidth(), 30.0f);
	sampleBufferImageRect.translate(0, (float)getHeight() - signalImagesOffsetFromBottom);
	g.setColour(juce::Colours::darkgrey);
	g.drawImage(sampleBufferImage0, sampleBufferImageRect);
	g.drawRect(sampleBufferImageRect);
	sampleBufferImageRect.translate(0, -30);
	g.drawImage(sampleBufferImage1, sampleBufferImageRect);
	g.drawRect(sampleBufferImageRect);

	g.setColour(juce::Colours::lightyellow);

	double total_length = transportSource.getLengthInSeconds();

	if (total_length)
	{
		double current_pos = transportSource.getCurrentPosition();
		double percent = current_pos / total_length;

		juce::Point<float> topLeft0 = sampleBufferImageRect.getTopLeft();
		juce::Point<float> bottomRight1 = sampleBufferImageRect.getBottomRight();

		g.fillRect(sampleBufferImageRect.getWidth() * (float)percent, topLeft0.getY(), 1.0f, 2*sampleBufferImageRect.getHeight());
	}
}

void MainComponent::resized()
{
	// This is called when the MainContentComponent is resized.
	// If you add any child components, this is where you should
	// update their positions.
	int controlsAreaOffsetFromBottom = 120; // 60 px from the bottom of the screen
	
	logSpaceComponent.setSize(getWidth(), getHeight() - controlsAreaOffsetFromBottom);
	debugComponent.setBounds(0, 0, getWidth(), 30);
	audioToggleButton.setBounds(0, getHeight() - controlsAreaOffsetFromBottom, 60, 30);
	stopButton.setBounds(180, getHeight() - controlsAreaOffsetFromBottom, 60, 30);
	loadButton.setBounds(60, getHeight() - controlsAreaOffsetFromBottom, 60, 30);
	playButton.setBounds(120, getHeight() - controlsAreaOffsetFromBottom, 60, 30);
	timeLabel.setBounds(240, getHeight() - controlsAreaOffsetFromBottom, 120, 30);
	sampleBufferImage0 = juce::Image(juce::Image::RGB, getWidth(), 30, true);
	sampleBufferImage1 = juce::Image(juce::Image::RGB, getWidth(), 30, true);
	sampleBufferImage0.clear(sampleBufferImage0.getBounds(), juce::Colours::black);
	sampleBufferImage1.clear(sampleBufferImage1.getBounds(), juce::Colours::black);
	if (internalAudioBuffer.getNumSamples())
	{
		ReadSamplesToImage();
	}
	
}
