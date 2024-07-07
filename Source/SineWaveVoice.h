/*
  ==============================================================================

    SineWaveVoice.h
    Created: 30 Jun 2024 7:16:26pm
    Author:  viktor

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
using namespace juce;

class SineWaveVoice : public SynthesiserVoice
{
public:
    SineWaveVoice();
    ~SineWaveVoice() override;

    // Inherited via SynthesiserVoice
    bool canPlaySound(SynthesiserSound*) override;
    void startNote(int midiNoteNumber, float velocity, SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override;

private:
    double currentAngle = 0.0, angleDelta = 0.0, level = 0.0, tailOff = 0.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SineWaveVoice)
};