/*
  ==============================================================================

    SineWaveSound.h
    Created: 7 Jul 2024 1:53:36pm
    Author:  viktor

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
using namespace juce;

struct BaseWaveSound : public juce::SynthesiserSound
{
    BaseWaveSound() {}

    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};