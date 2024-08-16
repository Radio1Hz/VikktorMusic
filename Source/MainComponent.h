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

class MainComponent : public AudioAppComponent, KeyListener , public ChangeListener, public MultiTimer, public ValueTree::Listener
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
	void timeSignatureChanged();
	void ReadSamplesToAudioSampleBufferImage();
	
	void changeListenerCallback(ChangeBroadcaster* source);
	void playButtonClicked();
	void stopButtonClicked();
	void setupProjectTree();
	void updateToggleStateAudio(Button* button);
	void updateToggleStateSettings(Button* button);
	void updateToggleStateVisualize(Button* button);
	void updateToggleStateSavePlayedAudio(Button* button);

	static String displayProgress(double currentPositionInSeconds, double totalLengthInSeconds);
	static String displayLocalTime(double currentPositionInSeconds, double totalLengthInSeconds);

	void prepareToPlay(int	samplesPerBlockExpected, double	sampleRate);
	void releaseResources();
	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill);

	void paint(Graphics& g) override;
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
	TextButton playButton;
	TextButton stopButton;
	TextButton loadButton;
	TextButton newButton;
	Label tempoLabel;
	TextEditor tempoText;
	Label keySignatureLabel;
	TextEditor keySignatureNumeratorText;
	Label keySignatureSeparatorLabel;
	TextEditor keySignatureDenominatorText;

	Label timeLabel;
	ToggleButton audioOnToggleButton;
	ToggleButton audioSettingsToggleButton;
	ToggleButton audioVisualizeToggleButton;
	DialogWindow newDialogWindow;
	ToggleButton savePlayedAudioSamplesToggleButton;
	AudioDeviceSelectorComponent audioSetupComp;
	bool savePlayedAudioSamples = true;
	// Inherited via MultiTimer
	void timerCallback(int timerID) override;
	void changeState(TransportState newState);

	AudioFormatManager formatManager;
	std::unique_ptr<FileChooser> fileChooser;
	
	// Internal Buffer related 

	AudioBuffer<float> internalAudioBuffer;
	Image internalBufferSamplesImage0;
	Image internalBufferSamplesImage1;
	Image audioSampleBlockImage;
	Array<float> audioSampleBufferCopy;
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
	Random random;

	void valueTreePropertyChanged(ValueTree& treeWhosePropertyHasChanged, const Identifier& property) override;
	void valueTreeChildAdded(ValueTree& parentTree, ValueTree& childWhichHasBeenAdded) override;
	void valueTreeChildRemoved(ValueTree & parentTree, ValueTree & childWhichHasBeenRemoved, int indexFromWhichChildWasRemoved) override;
	void valueTreeChildOrderChanged(ValueTree & parentTreeWhoseChildrenHaveMoved, int oldIndex, int newIndex) override;
	void valueTreeParentChanged(ValueTree & treeWhoseParentHasChanged) override;
	void valueTreeRedirected(ValueTree & treeWhichHasBeenChanged) override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
