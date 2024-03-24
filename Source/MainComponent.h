#pragma once

#include <JuceHeader.h>
#include "DebugComponent.h"
#include "TimerComponent.h"
#include "LogSpaceComponent.h"

//========================================================================================================================
/*
	This component lives inside our window, and this is where you should put all
	your controls and content.
*/


class MainComponent : public juce::AudioAppComponent, juce::KeyListener , public juce::ChangeListener, public juce::MultiTimer
{
public:
	//====================================================================================================================
	MainComponent();
	~MainComponent() override;

	void chooseFile();
	void ReadSamplesToImage();
	
	void changeListenerCallback(juce::ChangeBroadcaster* source);
	void playButtonClicked();
	void stopButtonClicked();
	void updateToggleStateAudio(juce::Button* button);
	void updateToggleStateSettings(juce::Button* button);

	static juce::String displayProgress(double currentPositionInSeconds, double totalLengthInSeconds);

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

	void paint(juce::Graphics& g) override;
	void resized() override;
	bool MainComponent::keyPressed(const KeyPress& key, Component* originatingComponent) override;
	void setAudioOn();
	void setAudioOff();

	enum TimerType
	{
		CPUTimer,
		TimePositionTimer
	};

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

	LogSpaceComponent logSpaceComponent;
	DebugComponent debugComponent;
	juce::TextButton playButton;
	juce::TextButton stopButton;
	juce::TextButton loadButton;
	juce::Label timeLabel;
	juce::ToggleButton audioOnToggleButton;
	juce::ToggleButton audioSettingsToggleButton;

	AudioDeviceSelectorComponent audioSetupComp;

	// Inherited via MultiTimer
	void timerCallback(int timerID) override;
	
	void changeState(TransportState newState);

	juce::AudioFormatManager formatManager;
	juce::AudioTransportSource transportSource;

	std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
	std::unique_ptr<FileChooser> fileChooser;
	
	// Internal Buffer related 

	juce::AudioBuffer<float> internalAudioBuffer;
	juce::Image internalBufferSamplesImage0;
	juce::Image internalBufferSamplesImage1;
	double internalBufferTotalLengthInSeconds = 0.0;
	double internalBufferCurrentPositionInSeconds = 0.0;
	int internalBufferSamplesPerBlock = 0;
	double internalBufferSampleRate = 0.0;
	bool internalBufferAudioOn = false;
	int internalBufferCurrentSamplePlaying = 0;
	int internalBufferPointerRasterX = 0;

	TransportState state;
	juce::Random random;
	
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
