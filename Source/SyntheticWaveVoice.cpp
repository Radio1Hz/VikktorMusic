/*
  ==============================================================================

	SineWaveVoice.cpp
	Created: 30 Jun 2024 7:16:26pm
	Author:  viktor

  ==============================================================================
*/

#include "SyntheticWaveVoice.h"
#include "BaseWaveSound.h"

SyntheticWaveVoice::SyntheticWaveVoice()
{
}

SyntheticWaveVoice::SyntheticWaveVoice(int sID)
{
    this->synthID = sID;
}

SyntheticWaveVoice::~SyntheticWaveVoice()
{
}

bool SyntheticWaveVoice::canPlaySound(SynthesiserSound* sound)
{
	return dynamic_cast<BaseWaveSound*> (sound) != nullptr;
}

void SyntheticWaveVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound* /*sound*/, int /*currentPitchWheelPosition*/)
{
    currentAngle = 0.0;
    level = velocity * 0.15;
    tailOff = 0.0;

    auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    auto cyclesPerSample = cyclesPerSecond / getSampleRate();

    angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
}

void SyntheticWaveVoice::stopNote(float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        if (tailOff == 0.0)
            tailOff = 1.0;
    }
    else
    {
        clearCurrentNote();
        angleDelta = 0.0;
    }
}

void SyntheticWaveVoice::pitchWheelMoved(int /*newPitchWheelValue*/)
{
}

void SyntheticWaveVoice::controllerMoved(int /*controllerNumber*/, int /*newControllerValue*/)
{
}

void SyntheticWaveVoice::renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
    float currentSample = 0.0f;
    if (angleDelta != 0.0)
    {
        if (tailOff > 0.0) // [7]
        {
            while (--numSamples >= 0)
            {
                currentSample = synthFunction(synthID, currentAngle, level, (float)tailOff);

                for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                    outputBuffer.addSample(i, startSample, currentSample);

                currentAngle += angleDelta;
                ++startSample;

                tailOff *= 0.99f;

                if (tailOff <= 0.005)
                {
                    clearCurrentNote();

                    angleDelta = 0.0;
                    break;
                }
            }
        }
        else
        {
            while (--numSamples >= 0)
            {
                currentSample = synthFunction(synthID, currentAngle, level, (float)tailOff);

                for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                    outputBuffer.addSample(i, startSample, currentSample);

                currentAngle += angleDelta;
                ++startSample;
            }
        }
    }
}

float SyntheticWaveVoice::synthFunction(int sID, double angle, double lvl, float tOff)
{
    float currentSample = 0.0;
    
    switch (sID)
    {
    case 0:
    {
        currentSample = (float)(std::sin(angle) * lvl);
        break;
    }
    case 1:
    {
        currentSample = (float)(round(std::sin(angle)) * lvl);
        break;
    }
    default:
        break;
    }
    if (tOff != 0)
    {
        currentSample = currentSample * tOff;
    }
    return currentSample;
}
