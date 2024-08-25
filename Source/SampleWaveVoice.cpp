/*
  ==============================================================================

	SampleWaveVoice.cpp
	Created: 23 Aug 2024 7:31:28pm
	Author:  viktor

  ==============================================================================
*/

#include "SampleWaveVoice.h"
#include "BaseWaveSound.h"


SampleWaveVoice::SampleWaveVoice(map<int, String>& samplesPerMIDIN, int sampleR) : samplesPerMIDINote(samplesPerMIDIN)
{
	sampleRate = sampleR;
	formatManager.registerBasicFormats();
	buffersPerMIDINote = make_unique<map<int, AudioSampleBuffer>>();
	loadSamples();
}

SampleWaveVoice::~SampleWaveVoice()
{
}

void SampleWaveVoice::loadSamples()
{
	for (const auto& p : samplesPerMIDINote) {
		File file(p.second);
		if (file != File{})
		{
			std::unique_ptr<AudioFormatReader> reader(formatManager.createReaderFor(file));
			if (reader != nullptr)
			{
				if (reader->sampleRate != sampleRate)
				{
					AudioSampleBuffer tempInBuffer, tempOutBuffer;
					tempInBuffer.setSize(reader->numChannels, (int)reader->lengthInSamples);
					reader->read(tempInBuffer.getArrayOfWritePointers(), reader->numChannels, 0, (int)reader->lengthInSamples);

					tempOutBuffer.setSize(reader->numChannels, (int)((sampleRate / reader->sampleRate) * reader->lengthInSamples));
					//this->internalBufferTotalLengthInSeconds = (float)reader->lengthInSamples / reader->sampleRate;
					AudioSampleBuffer buffer = AudioSampleBuffer(reader->numChannels, tempOutBuffer.getNumSamples());
					buffersPerMIDINote->insert(pair<int, AudioSampleBuffer>(p.first, buffer));

					LagrangeInterpolator resampler = LagrangeInterpolator();
					for (unsigned int i = 0; i < reader->numChannels; i++)
					{
						resampler.process(reader->sampleRate / sampleRate, tempInBuffer.getReadPointer(i), tempOutBuffer.getWritePointer(i), tempOutBuffer.getNumSamples());
						buffersPerMIDINote->at((int)p.first).copyFrom(i, 0, tempOutBuffer.getReadPointer(i), tempOutBuffer.getNumSamples());
					}
				}
				else
				{
					buffersPerMIDINote->insert(pair<int, AudioSampleBuffer>(p.first, AudioSampleBuffer(reader->numChannels, (int)reader->lengthInSamples)));
					reader->read(&buffersPerMIDINote->at((int)p.first), 0, (int)reader->lengthInSamples, 0, true, true);
				}
			}
		}
	}
}

bool SampleWaveVoice::canPlaySound(SynthesiserSound* /*sound*/)
{
	//return true;
	return !isPlaying;
}

void SampleWaveVoice::startNote(int midiNoteNumber, float velocity, SynthesiserSound* /*sound*/, int /*currentPitchWheelPosition*/)
{
	isPlaying = true;
	playingMIDINoteNumber = midiNoteNumber;
	playingNoteVelocity = velocity;
	bufferSampleCounter = 0;
}

void SampleWaveVoice::stopNote(float /*velocity*/, bool /*allowTailOff*/)
{
	isPlaying = false;
	playingMIDINoteNumber = 0;
}

void SampleWaveVoice::pitchWheelMoved(int /*newPitchWheelValue*/)
{
}

void SampleWaveVoice::controllerMoved(int /*controllerNumber*/, int /*newControllerValue*/)
{
}

void SampleWaveVoice::renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
	if (isPlaying)
	{
		float currentSample = 0.0f;
		outputBuffer.clear();

		if (playingMIDINoteNumber > 0)
		{
			if (buffersPerMIDINote->count(playingMIDINoteNumber) > 0)
			{
				AudioSampleBuffer& internalBuffer = buffersPerMIDINote->at(playingMIDINoteNumber);
				bool shouldStop = false;
				while (--numSamples >= 0)
				{
					for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
					{
						currentSample = playingNoteVelocity * internalBuffer.getSample(i, bufferSampleCounter % internalBuffer.getNumSamples());
						if (bufferSampleCounter >= internalBuffer.getNumSamples())
						{
							currentSample = 0.0f;
							shouldStop = true;
						}
						outputBuffer.addSample(i, startSample, currentSample);
					}
					bufferSampleCounter++;
					startSample++;
				}
				if (shouldStop)
				{
					bufferSampleCounter = 0;
					isPlaying = false;
				}
			}
		}
	}
}
