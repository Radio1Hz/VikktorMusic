#include "MainComponent.h"
#include "DebugComponent.h"

//==============================================================================


MainComponent::MainComponent() : state(TransportState::Stopped)
{
	// Make sure you set the size of the component after
	// you add any child components.
	setSize(800, 600);
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

	timeLabel.setText(juce::String(juce::RelativeTime::seconds(0.0).getDescription()), NotificationType::dontSendNotification);
	timeLabel.setJustificationType(Justification::centredRight);

	addAndMakeVisible(logSpaceComponent, -1);
	addAndMakeVisible(audioToggleButton, -1);
	addAndMakeVisible(&playButton);
	addAndMakeVisible(&stopButton);
	addAndMakeVisible(timeLabel);
	//addAndMakeVisible(debugComponent, -1);

	auto file = juce::File(juce::String(CharPointer_UTF8("C:\\Data\\Samples\\Ashtar.wav")));

	if (file != juce::File{})                                                // [9]
	{
		auto* reader = formatManager.createReaderFor(file);                 // [10]

		if (reader != nullptr)
		{
			auto newSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);   // [11]
			transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);       // [12]
			playButton.setEnabled(true);                                                      // [13]
			readerSource.reset(newSource.release());                                          // [14]
		}
	}

	startTimer(100);
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
	//const auto currentDevice = deviceManager.getAvailableDeviceTypes();
	juce::String message;

	message << "Preparing to play audio...\n";
	message << " samplesPerBlockExpected = " << samplesPerBlockExpected << "\n";
	message << " sampleRate = " << sampleRate;
	currentSampleRate = sampleRate;
	currentSamplesPerBlock = samplesPerBlockExpected;
	
	transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
	// ---------------------------- buffer Init -----------------------------------

	//internalAudioBuffer = juce::AudioBuffer<float>(2, currentSampleRate * numSeconds);
	//internalAudioBuffer.clear(0, 0, currentSampleRate * numSeconds); //left
	//internalAudioBuffer.clear(1, 0, currentSampleRate * numSeconds); //right

	//for (int i = 0; i < numSeconds; i++)
	//{
	//	internalAudioBuffer.addSample(0, i * currentSampleRate, 0.90f);
	//	internalAudioBuffer.addSample(1, i * currentSampleRate, 0.90f);
	//}

	//auto file = juce::File("C:\\Data\\Samples\\mixkit-arcade-retro-game-over-213.wav");
	
	//internalAudioBuffer = getAudioBufferFromFile(file);

	//std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
	//if (reader)
	//{
	//	juce::AudioBuffer<float> tempBuffer;
	//	auto duration = (float)reader->lengthInSamples / reader->sampleRate;
	//	reader->readSamples(& tempBuffer, 2, 0, internalAudioBuffer.getNumSamples());
	//	//internalAudioBuffer = juce::AudioBuffer<float>(2, currentSampleRate * duration);
	//}

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
	rectMain.translate(60.0f, (float)getHeight() - 30.0f);
	g.drawText("Sample Rate: " + String(currentSampleRate) + ", Samples per block: " + String(currentSamplesPerBlock), rectMain, Justification::left);
}

void MainComponent::resized()
{
	// This is called when the MainContentComponent is resized.
	// If you add any child components, this is where you should
	// update their positions.
	logSpaceComponent.setSize(getWidth(), getHeight() - 30);
	debugComponent.setBounds(0, 0, getWidth(), 30);
	audioToggleButton.setBounds(0, getHeight() - 30, 60, 30);
	stopButton.setBounds(60, getHeight() - 60, 60, 30);
	playButton.setBounds(0, getHeight() - 60, 60, 30);
	timeLabel.setBounds(120, getHeight() - 60, 120, 30);
}
