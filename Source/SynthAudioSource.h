/*
  ==============================================================================

	SynthComponent.h
	Created: 2 Jun 2024 3:19:21pm
	Author:  viktor

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "BaseComponent.h"
#include "CommunicationAgent.h"
#include "SyntheticWaveVoice.h"
#include "SampleWaveVoice.h"
#include "BaseWaveSound.h"

using namespace juce;


class SynthAudioSource : public AudioSource
{

public:

	SynthAudioSource(int numVoices, int synthID, int sampleRate)
	{
		for (auto i = 0; i < numVoices; ++i)
		{
			switch (synthID)
			{
			case 0:
			{
				synth.addVoice(new SyntheticWaveVoice(synthID));
				break;
			}
			case 1:
			{
				map<int, String> samplesPerMIDINote;
				// C - Kick
				samplesPerMIDINote.insert(pair<int, String>(60, "C:\\Data\\Samples\\Aftermath-Drum-Kit\\Aftermath-Drum-Kit\\808 kick 8.wav"));
				// D - Snare
				samplesPerMIDINote.insert(pair<int, String>(62, "C:\\Data\\Samples\\Real Drums Vol. 1\\Real Drums Vol. 1\\Snare\\RD_S_4.wav"));
				// E - HiHat
				samplesPerMIDINote.insert(pair<int, String>(64, "C:\\Data\\Samples\\Aftermath-Drum-Kit\\Aftermath-Drum-Kit\\DDE HiHat 1.wav"));

				synth.addVoice(new SampleWaveVoice(samplesPerMIDINote, sampleRate));
				
				break;
			}
			default:
				break;
			}
			
		}

		synth.addSound(new BaseWaveSound());
	}

	~SynthAudioSource()
	{
		synth.clearVoices();
		synth.clearSounds();
	}

	void setUsingSineWaveSound()
	{
		synth.clearSounds();
	}

	void prepareToPlay(int /*samplesPerBlockExpected*/, double sampleRate) override
	{
		synth.setCurrentPlaybackSampleRate(sampleRate); // [3]
	}

	void releaseResources() override {};

	void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override
	{
		bufferToFill.clearActiveBufferRegion();

		MidiBuffer incomingMidi;
		synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, bufferToFill.startSample, bufferToFill.numSamples);
	}

	Synthesiser synth;
};
