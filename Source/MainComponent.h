#pragma once

#include <JuceHeader.h>
#include "DebugComponent.h"
#include "TimerComponent.h"
#include "LogSpaceComponent.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent, juce::KeyListener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    /*
    AudioAppComponent::setAudioChannels(): We must call this to register the number of input and output channels we need. Typically, we do this in our constructor. In turn, this function triggers the start of audio processing in our application.
    AudioAppComponent::shutdownAudio(): We must call this to shutdown the audio system. Typically, we do this in our destructor.
    AudioAppComponent::prepareToPlay() : This is called just before audio processing starts.
    AudioAppComponent::releaseResources() : This is called when audio processing has finished.
    AudioAppComponent::getNextAudioBlock() : This is called each time the audio hardware needs a new block of audio data.
    */
   
    void prepareToPlay(int	samplesPerBlockExpected, double	sampleRate);
    void releaseResources();
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill);

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;
    bool MainComponent::keyPressed(const KeyPress& key, Component* originatingComponent) override;

    LogSpaceComponent logSpaceComponent;

private:
    private:
    juce::Random random;
    DebugComponent debugComponent;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
