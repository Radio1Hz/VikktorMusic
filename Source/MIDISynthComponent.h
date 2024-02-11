/*
  ==============================================================================

    MIDISynthComponent.cpp
    Created: 10 Sep 2021 8:04:45pm
    Author:  viktor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SoundfontAudioSource.h"
#include "BaseAudioAppComponent.h"
#include "CommunicationAgent.h"

using namespace juce;
//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MIDISynthComponent : public BaseAudioAppComponent, public ComboBox::Listener, public MidiKeyboardStateListener, public CommunicationAgent, public Timer
{
public:
    //==============================================================================
    MIDISynthComponent();
    ~MIDISynthComponent();

    // AudioSource
    //==============================================================================
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    // Component
    //==============================================================================
    void paint(Graphics& g) override;
    void resized() override;

    // ComboBox::Listener
    //==============================================================================
    void comboBoxChanged(ComboBox* comboBoxThatWasChanged) override;

    // MidiKeyboardStateListener
    //==============================================================================
    void handleNoteOn(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;
    void handleNoteOff(MidiKeyboardState* source, int midiChannel, int midiNoteNumber, float velocity) override;

    //CommunicationAgent
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

    //Timer
    void timerCallback() override;

    //==============================================================================
    const File getSoundfontsDirectory()
    {
        // In this demo, the soundfonts are stored next to the Source folder that contains this file
        return File("C:/Data/SOUNDFONTS/");
    }

    const File getFileFromName(const String& filename)
    {
        return soundfontFiles[soundfontNames.indexOf(filename)];
    }

private:
    //==============================================================================
    // Your private member variables go here...

    SoundfontAudioSource soundfontPlayer;
    MidiKeyboardState keyState;
    Array<File> soundfontFiles;
    File defaultSoundFontFile;
    StringArray soundfontNames;
    ScopedPointer<MidiKeyboardComponent> keyboardComponent;
    ScopedPointer<ComboBox> soundfontSelector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDISynthComponent)
};
