/*
  ==============================================================================

    MIDISynthComponent.cpp
    Created: 10 Sep 2021 8:04:45pm
    Author:  viktor

  ==============================================================================
*/

#include "MIDISynthComponent.h"


//==============================================================================
MIDISynthComponent::MIDISynthComponent()
{
    // Populate Soundfonts
    soundfontFiles = getSoundfontsDirectory().findChildFiles(File::findFiles, false, "*.sf2");
    defaultSoundFontFile = (getSoundfontsDirectory().findChildFiles(File::findFiles, false, "PNS Drum Kit.sf2"))[0];
    
    int selIndex = 0;
    int index = 0;
    for (auto f : soundfontFiles) {
        soundfontNames.add(f.getFileNameWithoutExtension());
        if (defaultSoundFontFile.getFileNameWithoutExtension() == f.getFileNameWithoutExtension())
        {
            selIndex = index;
        }
        index++;
    }
    soundfontSelector = std::make_unique<ComboBox>("SoundfontSelector");
    addAndMakeVisible(*soundfontSelector);
    soundfontSelector->addListener(this);
    soundfontSelector->addItemList(soundfontNames, 1);
    soundfontSelector->setSelectedItemIndex(selIndex);
    keyboardComponent = std::make_unique<MidiKeyboardComponent>(keyState, MidiKeyboardComponent::horizontalKeyboard);
    addAndMakeVisible(*keyboardComponent);
    keyState.addListener(this);

    // Make sure you set the size of the component after
    // you add any child components.
    //setSize(800, 200);

    // specify the number of input and output channels that we want to open
    setAudioChannels(2, 2);
    name = "MIDI Synth";
}

MIDISynthComponent::~MIDISynthComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

void MIDISynthComponent::changeListenerCallback(juce::ChangeBroadcaster*)
{
    /*soundfontPlayer.noteOn(51, 1 * 127, 1);
    soundfontPlayer.noteOff(51, 1);*/
    //this->startTimer(100);
}

void MIDISynthComponent::timerCallback()
{
    //soundfontPlayer.noteOff(56, 1);
    //stopTimer();
}
//==============================================================================
void MIDISynthComponent::prepareToPlay(int /*samplesPerBlockExpected*/, double /*sampleRate*/)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()

    //soundfontPlayer.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MIDISynthComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
    // Your audio-processing code goes here!

    // For more details, see the help for AudioProcessor::getNextAudioBlock()

    // Right now we are not producing any data, in which case we need to clear the buffer
    // (to prevent the output of random noise)
    bufferToFill.clearActiveBufferRegion();

    //soundfontPlayer.getNextAudioBlock(bufferToFill);
}

void MIDISynthComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()

    //soundfontPlayer.releaseResources();
}

//==============================================================================
void MIDISynthComponent::paint(Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    //g.fillAll(getLookAndFeel().findColour(ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
    drawOutline(g);
}

void MIDISynthComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.

    Rectangle<int> myBounds = getReducedLocalBounds();
    const int selectorHeight = 20;
    const int keyboardHeight = 40;
    
    soundfontSelector->setBounds(myBounds.getX(), myBounds.getY(), myBounds.getWidth(), selectorHeight);
    keyboardComponent->setBounds(myBounds.getX(), myBounds.getY() + selectorHeight, myBounds.getWidth(), keyboardHeight);
}

void MIDISynthComponent::comboBoxChanged(ComboBox* comboBoxThatWasChanged)
{
    if ((std::unique_ptr<ComboBox>)comboBoxThatWasChanged == soundfontSelector) {
        File soundfontFile = getFileFromName(soundfontSelector->getText());
        
    }
}

void MIDISynthComponent::handleNoteOn(MidiKeyboardState*, int /*midiChannel*/, int /*midiNoteNumber*/, float /*velocity*/)
{
    //soundfontPlayer.noteOn(midiNoteNumber, (int)velocity * 127, midiChannel);
}

void MIDISynthComponent::handleNoteOff(MidiKeyboardState*,  int /*midiChannel*/, int /*midiNoteNumber*/, float)
{
    //soundfontPlayer.noteOff(midiNoteNumber, midiChannel);
}
