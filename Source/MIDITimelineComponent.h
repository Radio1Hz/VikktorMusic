/*
  ==============================================================================

    MIDITimeline.h
    Created: 17 Apr 2024 5:37:02pm
    Author:  viktor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "BaseComponent.h"
#include "CommunicationAgent.h"
using namespace juce;
//==============================================================================

class MIDITimelineComponent : public BaseComponent, public CommunicationAgent
{
public:
    MIDITimelineComponent();
    ~MIDITimelineComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void changeListenerCallback(ChangeBroadcaster* source) override;

private:
    void loadMIDI();
    void processMidi();
    void drawMIDIEvents(Rectangle<float> trackRect, int trackIndex, Graphics& g);
    std::unique_ptr<FileChooser> fileChooser;
    std::unique_ptr<MidiFile> midiFile;
    OwnedArray<MidiMessageSequence> midiTracks;
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MIDITimelineComponent)
        
};
