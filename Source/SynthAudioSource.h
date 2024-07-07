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
#include "SineWaveVoice.h"
#include "SineWaveSound.h"

using namespace juce;


class SynthAudioSource : public juce::AudioSource
{

public:

    SynthAudioSource()
    {
        for (auto i = 0; i < 2; ++i)
        {
            synth.addVoice(new SineWaveVoice());
        }

        synth.addSound(new SineWaveSound());
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

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        bufferToFill.clearActiveBufferRegion();

        juce::MidiBuffer incomingMidi;
        synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, bufferToFill.startSample, bufferToFill.numSamples);
    }

    juce::Synthesiser synth;
};
