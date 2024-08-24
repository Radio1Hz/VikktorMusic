/*
  ==============================================================================

	SampleWaveVoice.h
	Created: 23 Aug 2024 7:31:28pm
	Author:  viktor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
using namespace juce;
using namespace std;

class SampleWaveVoice : public SynthesiserVoice
{
public:
	SampleWaveVoice(map<int, String>& samplesPerMIDIN, int sampleR);
	~SampleWaveVoice() override;
	map<int, String>& samplesPerMIDINote;
	unique_ptr<map<int, AudioSampleBuffer>> buffersPerMIDINote;
	// Inherited via SynthesiserVoice
	bool canPlaySound(SynthesiserSound*) override;
	void startNote(int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition) override;
	void stopNote(float velocity, bool allowTailOff) override;
	void pitchWheelMoved(int newPitchWheelValue) override;
	void controllerMoved(int controllerNumber, int newControllerValue) override;
	void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override;
	void loadSamples();
	AudioFormatManager formatManager;
	int sampleRate = 0;
	bool isPlaying = false;
	int playingMIDINoteNumber = 0;
	float playingNoteVelocity = 0.0f;
	int bufferSampleCounter = 0;
private:
	int synthID = 0;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SampleWaveVoice)
};