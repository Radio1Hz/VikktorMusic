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
//==============================================================================

class MIDITimelineComponent : public BaseComponent, public CommunicationAgent, private AsyncUpdater
{
public:
	MIDITimelineComponent();
	MIDITimelineComponent(int numMeasures, int synthID);
	void clearMatrix();
	~MIDITimelineComponent() override;

	void paint(juce::Graphics&) override;
	void resized() override;
	void changeListenerCallback(ChangeBroadcaster* source) override;
	void handleAsyncUpdate() override;
	void shiftDragEvent(const juce::MouseEvent& event) override;
	void shiftMouseDownEvent(const juce::MouseEvent& event) override;
	void shiftMouseUpEvent(const juce::MouseEvent& event) override;
	void mouseDoubleClickEvent(const juce::MouseEvent& event) override;
	void triggerRepaint();
	
	void playMIDI();
	void stopMIDI();
	void processMidi();
	void allNotesOff();
	void init();

private:
	void loadMIDI();
	void setComponentSize();
	void initMenu();
	void populateSelectionMatrix();
	std::unique_ptr<juce::dsp::Matrix<int>> selectionMatrix;
	void clearTimeline();
	void repaintMatrixImage();
	void processSelection();
	void analyzeContextInSelection();
	void defineAllContextsPerMeasures();
	void operationOnSelection01();
	void saveAudioBufferToDisk();
	void saveMIDIFileToDisk();
	float minCellWidth = 10.0f;
	int selectedCellStart = -1;
	int selectedCellEnd = 0;
	bool selectionInProgress = false;
	void MIDITimelineComponent::scanPlugins();
	MusicMath musicMath;
	Image matrixImage;
	void drawMIDIEvents(Rectangle<float> trackRect, int trackIndex, Graphics& g);
	unique_ptr<FileChooser> fileChooser;
	unique_ptr<MidiFile> midiFile;
	OwnedArray<MidiMessageSequence> midiTracks;
	OwnedArray<SynthAudioSource> synths;
	OwnedArray<MarkovMatrixComponent> measureMatrices;
	vector<std::vector<NoteEventDesc>> noteEventMatrix;
	vector<float> noteProbabilities;
	vector<vector<ContextDesc>> contextPerMeasureVector;
	
	String projectPath = "";
	String projectName = "";
	int viewMode = 1; // 0 - MIDI, 1 - Matrix
	int noteRangeStart = 36;
	int noteRangeEnd = 84;
	int noteRangeSize = 0;
	int counter = 0;
	int currentTimeUnit = 0;
	float timelineHeightRatio = 0.075f;
	short defaultTicksPerQuarterNote = 196;
	int samplesPerBlockExpectedInt = 0;
	double sampleRateInt = 0;
	bool isPlaying = false;
	int samplesElapsedSincePlay = 0;
	int numMeasures = 0;
	int synthID = 0;
	int numTimeUnitsInMeasure = 0;
	float numQuartersPerMeasure = 0;
	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
	void releaseResources() override;
	bool initializationIsComplete = false;
	juce::AudioDeviceManager audioDeviceManager;
	juce::AudioPluginFormatManager formatManager;
	std::unique_ptr<juce::AudioPluginInstance> pluginInstance;
	juce::AudioBuffer<float> audioBuffer;
	 
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDITimelineComponent)
};
