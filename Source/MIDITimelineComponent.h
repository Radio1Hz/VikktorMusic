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
//==============================================================================

class MIDITimelineComponent : public BaseComponent, public CommunicationAgent, private AsyncUpdater
{
public:
	MIDITimelineComponent();
	MIDITimelineComponent(int numMeasures);
	void clearMatrix();
	~MIDITimelineComponent() override;

	void paint(juce::Graphics&) override;
	void resized() override;
	void changeListenerCallback(ChangeBroadcaster* source) override;
	void handleAsyncUpdate() override;
	void triggerRepaint();
	void playMIDI();
	void stopMIDI();
	void processMidi();

private:
	void loadMIDI();
	void setComponentSize();
	void initMenu();
	void clearTimeline();
	void repaintMatrixImage();
	void init();

	Image matrixImage;
	void drawMIDIEvents(Rectangle<float> trackRect, int trackIndex, Graphics& g);
	std::unique_ptr<FileChooser> fileChooser;
	std::unique_ptr<MidiFile> midiFile;
	OwnedArray<MidiMessageSequence> midiTracks;
	OwnedArray<SynthAudioSource> synths;
	OwnedArray<MarkovMatrixComponent> measureMatrices;
	std::vector<std::vector<NoteEventDesc>> noteEventMatrix;
	std::vector<float> noteProbabilities;
	String projectPath = "";
	String projectName = "";
	MusicMath musicMath;
	int viewMode = 1; // 0 - MIDI, 1 - Matrix
	int noteRangeStart = 36;
	int noteRangeEnd = 96;
	int noteRangeSize = 0;
	int counter = 0;
	int currentTimeUnit = -1;
	float timelineHeightRatio = 0.075f;
	int samplesPerBlockExpectedInt = 0;
	double sampleRateInt = 0;
	bool isPlaying = false;
	int samplesElapsedSincePlay = 0;
	int numMeasures = 0;
	int numTimeUnitsInMeasure = 0;
	float numQuartersPerMeasure = 0;
	void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
	void releaseResources() override;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDITimelineComponent)
};
