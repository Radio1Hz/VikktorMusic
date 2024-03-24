/*
  ==============================================================================

    AudioComponent.cpp
    Created: 28 Aug 2021 2:06:42pm
    Author:  viktor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AudioComponent.h"
#include "MarkovMatrixComponent.h"
#include "ProbabilitySlider.h"
#include "TimerComponent.h"
#include "ArraySorter.h"
//==============================================================================
AudioComponent::AudioComponent():
                            thumbnailCache(5),                            
                            thumbnail(480, formatManager, thumbnailCache)
{
    formatManager.registerBasicFormats();
    transportSource.addChangeListener(this);
    thumbnail.addChangeListener(this);

    name = "Audio";

    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio)
        && !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
            [&](bool granted) { setAudioChannels(granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels(0, 2);
    }
}

AudioComponent::~AudioComponent()
{
    shutdownAudio();
    delete masterBuffer;
}

//==============================================================================
void AudioComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRateExpected)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
    this->sampleRate = sampleRateExpected;
    this->bufferSize = samplesPerBlockExpected;

    this->name += ": " + String(round(sampleRate)) + "/" + String(bufferSize);
}

void AudioComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()
    
    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    if (masterBuffer == NULL) 
    {
        masterBuffer = new AudioSampleBuffer(bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
        masterBuffer->clear();
        
        for (auto channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
        {
            auto* buffer = masterBuffer->getWritePointer(channel, bufferToFill.startSample);
           //masterBuffer->setSample(channel, 0, Random::getSystemRandom().nextFloat());
            for (auto sample = 0; sample < bufferSize; ++sample)
            //buffer[sample] = Random::getSystemRandom().nextFloat() * 0.25f - 0.125f;
            buffer[sample] = 0.5f * (float)sin(100 * MathConstants<double>::twoPi*((double)sample/(double)bufferSize) + MathConstants<double>::halfPi * (double)channel);
        }
        
    }

    for (auto channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        //masterBuffer.makeCopyOf(bufferToFill.buffer, true);
        // Get a pointer to the start sample in the buffer for this audio output channel
        //auto* buffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);
        bufferToFill.buffer->copyFrom(channel, bufferToFill.startSample, masterBuffer->getReadPointer(channel), bufferToFill.numSamples);
        //// Fill the required number of samples with noise between -0.125 and +0.125
        //for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
        //    buffer[sample] = sin(sample) * 0.25f - 0.125f;
    }
    sendChangeMessage();
}

void AudioComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}
void AudioComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{

    if (source == &thumbnail)       thumbnailChanged();
    if (TimerComponent* d = dynamic_cast<TimerComponent*>(source))
    {
        //nextStep();
    }

    if (MarkovMatrixComponent* d = dynamic_cast<MarkovMatrixComponent*>(source))
    {
        //nextStep();
    }

    /*if (source == &transportSource)
    {
        if (transportSource.isPlaying())
            changeState(Playing);
        else
            changeState(Stopped);
    }*/

    repaint();
}

void AudioComponent::thumbnailChanged()
{
    repaint();
}

void AudioComponent::paint (juce::Graphics& g)
{
    drawOutline(g);

    //g.setColour (juce::Colours::grey);
    //g.drawRect (getReducedLocalBounds(), 1);   // draw an outline around the component
    //g.setColour(juce::Colours::white);

    //for (int channel = 0; channel < masterBuffer->getNumChannels(); channel++)
    //{
    //    const auto* buffer = masterBuffer->getReadPointer(channel);
    //    int compWidth = getReducedLocalBounds().getWidth();
    //    int compHeight = getReducedLocalBounds().getHeight();

    //    if (bufferSize > compWidth)
    //    {
    //        for (int i = 0; i < compWidth; i++)
    //        {
    //            int index = i * bufferSize / compWidth;
    //            g.drawLine(i+1, compHeight / (2+ channel), i+1, compHeight / (2 + channel) * (1 - buffer[index]));
    //        }
    //    }
    //    else
    //    {
    //        for (int i = 0; i < bufferSize; i++)
    //        {
    //            int index = i * compWidth / bufferSize;
    //            g.drawLine(index+1, compHeight / (2 + channel), index+1, compHeight / (2 + channel) * (1 - buffer[i]));
    //        }
    //    }
    //    
    //}

    juce::Rectangle<int> thumbnailBounds = getReducedLocalBounds();

    if (thumbnail.getNumChannels() == 0)
        paintIfNoFileLoaded(g, thumbnailBounds);
    else
        paintIfFileLoaded(g, thumbnailBounds);
}

void AudioComponent::paintIfNoFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
{
    g.setColour(juce::Colours::darkgrey);
    g.fillRect(thumbnailBounds);
    g.setColour(juce::Colours::white);
    g.drawFittedText("No File Loaded", thumbnailBounds, juce::Justification::centred, 1);
}

void AudioComponent::paintIfFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
{
    g.setColour(juce::Colours::white);
    g.fillRect(thumbnailBounds);

    g.setColour(juce::Colours::red);                               // [8]

    thumbnail.drawChannels(g,                                      // [9]
        thumbnailBounds,
        0.0,                                    // start time
        thumbnail.getTotalLength(),             // end time
        1.0f);                                  // vertical zoom
}

void AudioComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
}
