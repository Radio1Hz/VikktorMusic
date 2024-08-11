#pragma once

#include <JuceHeader.h>
#include "DebugComponent.h"
#include "TimerComponent.h"
#include "LogSpaceComponent.h"
#include "NewProjectComponent.h"

//========================================================================================================================
/*
	This component lives inside our window, and this is where you should put all
	your controls and content.
*/

class MainComponent : public juce::AudioAppComponent, juce::KeyListener , public juce::ChangeListener, public juce::MultiTimer, public juce::ValueTree::Listener
{
public:
	//====================================================================================================================
	MainComponent();
	~MainComponent() override;

	void chooseFile();
	void newButtonClicked();
	void setNewProject(int bpm, int measures);
	void ReadSamplesToImage();
	void tempoTextChanged();
	void tempoChanged();
	void ReadSamplesToAudioSampleBufferImage();
	
	void changeListenerCallback(juce::ChangeBroadcaster* source);
	void playButtonClicked();
	void stopButtonClicked();
	void setupProjectTree();
	void updateToggleStateAudio(juce::Button* button);
	void updateToggleStateSettings(juce::Button* button);
	void updateToggleStateVisualize(juce::Button* button);

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
		TimePositionTimer,
		SampleVisualization
	};

	enum TransportState
	{
		Stopped,
		Playing,
		Paused
	};
	Identifier projectMainNode = Identifier("projectHead");
	ValueTree projectTree = ValueTree(projectMainNode);

private:
	LogSpaceComponent logSpaceComponent;
	DebugComponent debugComponent;
	NewProjectComponent newProjectComponent;
	juce::TextButton playButton;
	juce::TextButton stopButton;
	juce::TextButton loadButton;
	juce::TextButton newButton;
	juce::TextEditor tempoText;
	juce::Label timeLabel;
	juce::Label tempoLabel;
	juce::ToggleButton audioOnToggleButton;
	juce::ToggleButton audioSettingsToggleButton;
	juce::ToggleButton audioVisualizeToggleButton;
	juce::DialogWindow newDialogWindow;
	AudioDeviceSelectorComponent audioSetupComp;

	// Inherited via MultiTimer
	void timerCallback(int timerID) override;
	void changeState(TransportState newState);

	juce::AudioFormatManager formatManager;
	std::unique_ptr<FileChooser> fileChooser;
	
	// Internal Buffer related 

	juce::AudioBuffer<float> internalAudioBuffer;
	juce::Image internalBufferSamplesImage0;
	juce::Image internalBufferSamplesImage1;
	juce::Image audioSampleBlockImage;
	juce::Array<float> audioSampleBufferCopy;
	int audioSampleBufferCopyNumSamples = 0;
	int64 audioBlockProcessedTimeInMilliseconds = 0;
	double internalBufferTotalLengthInSeconds = 0.0;
	double internalBufferCurrentPositionInSeconds = 0.0;
	int internalBufferSamplesPerBlock = 0;
	double internalBufferSampleRate = 0.0;
	bool internalBufferAudioOn = false;
	int internalBufferCurrentSamplePlaying = 0;
	int internalBufferPointerRasterX = 0;
	bool shouldVisualize = false;
	TransportState state;
	juce::Random random;

	void valueTreePropertyChanged(ValueTree& treeWhosePropertyHasChanged, const Identifier& property) override;
	void valueTreeChildAdded(ValueTree& parentTree, ValueTree& childWhichHasBeenAdded) override;
	void valueTreeChildRemoved(ValueTree & parentTree, ValueTree & childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) override;
	void valueTreeChildOrderChanged(ValueTree & parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex) override;
	void valueTreeParentChanged(ValueTree & treeWhoseParentHasChanged) override;
	void valueTreeRedirected(ValueTree & treeWhichHasBeenChanged) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
