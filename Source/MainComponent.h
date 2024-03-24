#pragma once

#include <JuceHeader.h>
#include "DebugComponent.h"
#include "TimerComponent.h"
#include "LogSpaceComponent.h"

//==============================================================================
/*
	This component lives inside our window, and this is where you should put all
	your controls and content.
*/


class MainComponent : public juce::AudioAppComponent, juce::KeyListener , public juce::ChangeListener, public juce::Timer
{
public:
	//==============================================================================
	MainComponent();
	void chooseFile();
	void ReadSamplesToImage();
	~MainComponent() override;

	void changeListenerCallback(juce::ChangeBroadcaster* source);
	void playButtonClicked();
	void stopButtonClicked();
	void updateToggleState(juce::Button* button);

	/*
	AudioAppComponent::setAudioChannels(): We must call this to register the number of input and output channels we need. Typically, we do this in our constructor. In turn, this function triggers the start of audio processing in our application.
	AudioAppComponent::shutdownAudio(): We must call this to shutdown the audio system. Typically, we do this in our destructor.
	AudioAppComponent::prepareToPlay() : This is called just before audio processing starts.
	AudioAppComponent::releaseResources() : This is called when audio processing has finished.
	AudioAppComponent::getNextAudioBlock() : This is called each time the audio hardware needs a new block of audio data.
	*/

	void prepareToPlay(int	samplesPerBlockExpected, double	sampleRate);
	void releaseResources();
	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill);

	//==============================================================================
	void paint(juce::Graphics& g) override;
	void resized() override;
	bool MainComponent::keyPressed(const KeyPress& key, Component* originatingComponent) override;
	void setAudioOn();
	void setAudioOff();
	LogSpaceComponent logSpaceComponent;
	ToggleButton audioToggleButton;
	juce::TextButton playButton;
	juce::TextButton stopButton;
	juce::TextButton loadButton;
	juce::Label timeLabel;

	enum TransportState
	{
		Stopped,
		Starting,
		Playing,
		Pausing,
		Paused,
		Stopping
	};

private:

	// Inherited via Timer
	void timerCallback() override;
	void changeState(TransportState newState);
	juce::AudioFormatManager formatManager;
	juce::AudioBuffer<float> internalAudioBuffer;
	std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
	juce::AudioTransportSource transportSource;
	
	juce::Image sampleBufferImage0;
	juce::Image sampleBufferImage1;

	TransportState state;
	std::unique_ptr<FileChooser> fileChooser;
	
	int currentSamplesPerBlock = 0;
	bool audioOn = false;
	double currentSampleRate = 0.0;
	juce::Random random;
	DebugComponent debugComponent;
	
	int currentSamplePlaying = 0;
	int numSeconds = 10;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
	
};
