/*
  ==============================================================================

	MIDITimeline.h
	Created: 17 Apr 2024 5:37:02pm
	Author:  viktor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "BaseComponent.h"
#include "CommunicationAgent.h"
#include "MusicMath.h"
#include "MarkovMatrixComponent.h"
#include "SynthAudioSource.h"

using namespace juce;
using namespace dsp;
using namespace std;
//==============================================================================

class MIDITimelineComponent : public BaseComponent, public CommunicationAgent, private AsyncUpdater
{
public:
	MIDITimelineComponent();
	MIDITimelineComponent(int numMeasures, int synthID);
	~MIDITimelineComponent() override;
	void paint(Graphics&) override;
	void resized() override;
	void changeListenerCallback(ChangeBroadcaster* source) override;
	void handleAsyncUpdate() override;
	void shiftDragEvent(const MouseEvent& event) override;
	void shiftMouseDownEvent(const MouseEvent& event) override;
	void shiftMouseUpEvent(const MouseEvent& event) override;
	void mouseDoubleClickEvent(const MouseEvent& event) override;
	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
	void releaseResources() override;

	void playMIDI();
	void stopMIDI();
	void processMidi();
	void allNotesOff();
	void init();
	void triggerRepaint();

private:
	//Methods
	void clearMatrix();
	void loadMIDI();
	void setComponentSize();
	void initMenu();
	void populateSelectionMatrix();
	void clearTimeline();
	void repaintMatrixImage();
	void processSelection();
	void analyzeContextInSelection();
	void defineAllContextsPerMeasures();
	void operationOnSelection01();
	void saveAudioBufferToDisk();
	void saveMIDIFileToDisk();
	void saveSelectionAsMIDIFile();
	void drawMIDIEvents(Rectangle<float> trackRect, int trackIndex, Graphics& g);
	void MIDITimelineComponent::scanPlugins();

	//Selection related
	unique_ptr<Matrix<NoteEventDesc>> selectionMatrix;
	int selectedCellStart = -1;
	int selectedCellEnd = 0;
	bool selectionInProgress = false;

	//Matrix 
	vector<vector<NoteEventDesc>> noteEventMatrix;
	vector<float> noteProbabilities;
	vector<vector<ContextDesc>> contextPerMeasureVector;

	//Graphics
	float minCellWidth = 10.0f;
	Image matrixImage;
	int viewMode = 1; // 0 - MIDI, 1 - Matrix
	float timelineHeightRatio = 0.075f;

	//MusicMath
	MusicMath musicMath;
	int noteRangeStart = 36;
	int noteRangeEnd = 92;
	int noteRangeSize = 0;
	int currentTimeUnit = 0;
	short defaultTicksPerQuarterNote = 196;
	int numMeasures = 0;
	int numTimeUnitsInMeasure = 0;
	float numQuartersPerMeasure = 0;

	//MIDI
	unique_ptr<FileChooser> fileChooser;
	unique_ptr<MidiFile> midiFile;
	OwnedArray<MidiMessageSequence> midiTracks;
	unique_ptr<MidiOutput> midiOutput;
	int defaultMIDIChannel = 1;

	//Synth
	OwnedArray<SynthAudioSource> synths;
	OwnedArray<MarkovMatrixComponent> measureMatrices;

	//Project
	String projectPath = "";
	String projectName = "";
	
	//AudioBuffer
	int samplesPerBlockExpectedInt = 0;
	double sampleRateInt = 0;
	bool isPlaying = false;
	int samplesElapsedSincePlay = 0;
	AudioBuffer<float> audioBuffer;
	AudioDeviceManager audioDeviceManager;
	AudioPluginFormatManager formatManager;

	//Other
	int counter = 0;
	int synthID = 0;
	bool initializationIsComplete = false;
	
	//Plugin
	unique_ptr<AudioPluginInstance> pluginInstance;
	 
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDITimelineComponent)
};
