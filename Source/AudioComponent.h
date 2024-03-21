/*
  ==============================================================================

    AudioComponent.h
    Created: 28 Aug 2021 2:06:42pm
    Author:  viktor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "BaseComponent.h"
#include "BaseAudioAppComponent.h"
#include "CommunicationAgent.h"

using namespace juce;
//==============================================================================
/*
*/
class AudioComponent  : public BaseAudioAppComponent, public CommunicationAgent
{
public:
    AudioComponent();
    ~AudioComponent() override;
    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void paint (juce::Graphics&) override;
    void resized() override;
    void thumbnailChanged();
    void paintIfNoFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);
    void paintIfFileLoaded(juce::Graphics & g, const juce::Rectangle<int>&thumbnailBounds);
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;
    
    juce::AudioThumbnailCache thumbnailCache;                  // [1]
    juce::AudioThumbnail thumbnail;
private:
    AudioSampleBuffer* masterBuffer = NULL;
    
    long samplesIndex = 0;
    int bufferSize = 0;
    double sampleRate = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioComponent)
};
