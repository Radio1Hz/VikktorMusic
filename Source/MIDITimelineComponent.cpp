/*
  ==============================================================================

    MIDITimeline.cpp
    Created: 17 Apr 2024 5:37:02pm
    Author:  viktor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MIDITimelineComponent.h"
using namespace juce;

//==============================================================================
MIDITimelineComponent::MIDITimelineComponent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    this->name = "MIDI Timeline";
    this->menu.clear();
    this->menu.addItem("Load MIDI", std::bind(&MIDITimelineComponent::loadMIDI, this));
    //addMouseListener(this, true);
}

MIDITimelineComponent::~MIDITimelineComponent()
{
    removeMouseListener(this);
    removeAllChildren();
    deleteAllChildren();
}

void MIDITimelineComponent::prepareToPlay(int /*samplesPerBlockExpected*/, double /*sampleRate*/)
{
}

void MIDITimelineComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& /*bufferToFill*/)
{
}

void MIDITimelineComponent::releaseResources()
{
}

void MIDITimelineComponent::paint (juce::Graphics& g)
{
    drawOutline(g);
   
    if (midiFile != nullptr)
    {
        Rectangle<float> parentBounds = getReducedLocalBounds().toFloat();
        float trackHeight = parentBounds.getHeight() / midiTracks.size();
        Rectangle<float> trackRect = parentBounds;
        trackRect.setHeight(trackHeight);
        
        for (int i = 0; i < midiTracks.size(); i++)
        {
            g.setColour(juce::Colours::grey);
            if (i + 1 < midiTracks.size())
            {
                g.drawLine(0.0f, trackRect.getBottom(), trackRect.getWidth(), trackRect.getBottom());
            }

            g.setFont(trackHeight / 25.0f);
            g.drawText("Track " + String(i + 1) + " (" + String(midiTracks[i]->getNumEvents()) + " events) " + String(midiTracks[i]->getStartTime()) + "-" + String(midiTracks[i]->getEndTime()), trackRect, juce::Justification::topLeft, true);
            drawMIDIEvents(trackRect, i, g);
            trackRect.translate(0, (float)trackHeight);
        }
    }
}

void MIDITimelineComponent::drawMIDIEvents(Rectangle<float> trackRect, int trackIndex, Graphics& g)
{
    double duration = midiFile->getLastTimestamp();
    int numerator, denominator;

    for (int i = 0; i < midiTracks[trackIndex]->getNumEvents(); i++)
    {
        juce::MidiMessageSequence::MidiEventHolder* midiEvent = midiTracks[trackIndex]->getEventPointer(i);
        MidiMessage midiMessage = midiEvent->message;

        if (midiMessage.isTimeSignatureMetaEvent())
        {
            midiMessage.getTimeSignatureInfo(numerator, denominator);
        }

        if (midiMessage.isNoteOn())
        {
            juce::MidiMessageSequence::MidiEventHolder* noteOffHolder = midiEvent->noteOffObject;
            int noteNumber = midiMessage.getNoteNumber();
            double noteStart = midiMessage.getTimeStamp();
            double noteEnd = noteStart;
            if (noteOffHolder)
            {
                noteEnd = noteOffHolder->message.getTimeStamp();
            }

            float pixelStart = (float)(noteStart * trackRect.getWidth() / duration);
            float pixelEnd = (float)(noteEnd * trackRect.getWidth() / duration);
            float yVel = (float)(128-noteNumber) * trackRect.getHeight() / 128.0f;
            g.setColour(Colours::lightgreen);
            g.drawLine(trackRect.getTopLeft().getX() + pixelStart, trackRect.getTopLeft().getY() + yVel, trackRect.getTopLeft().getX() + pixelEnd, trackRect.getTopLeft().getY() + yVel, 1.0f);
        }
    }
}

void MIDITimelineComponent::resized()
{
   
}

void MIDITimelineComponent::loadMIDI()
{
    fileChooser = std::make_unique<FileChooser>("Please select the .mid file you want to load...", File("C:\\Data\\Music\\Midis\\bach\\invent"), "*.mid");

    auto folderChooserFlags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles;

    fileChooser->launchAsync(folderChooserFlags, [this](const FileChooser& chooser)
    {
        File file(chooser.getResult());

        if (file != juce::File{})
        {
            midiFile = std::make_unique<MidiFile>();
            std::unique_ptr<FileInputStream> str = file.createInputStream();
            midiFile->readFrom(*str);
            name = file.getFileName()+" - Midi timeformat: " + String(midiFile->getTimeFormat()) + ", last timestamp: " + String(midiFile->getLastTimestamp()) + ", tracks: " + String(midiFile->getNumTracks());
            processMidi();
        }
    });
}

void MIDITimelineComponent::processMidi()
{
    midiTracks.clear();
   
    for (int i = 0; i < midiFile->getNumTracks(); i++)
    {
        const MidiMessageSequence* seq = midiFile->getTrack(i);
        MidiMessageSequence* seqCopy = new MidiMessageSequence(*seq);
        midiTracks.add(seqCopy);
    }
    Rectangle<float> parentBounds = getParentComponent()->getBounds().toFloat();
    float trackHeight = parentBounds.getHeight() / (midiTracks.size());
    setBounds((int)parentBounds.getX(), (int)parentBounds.getY(), (int)parentBounds.getWidth(), (int)trackHeight * midiTracks.size());

    repaint();
}

void MIDITimelineComponent::changeListenerCallback(ChangeBroadcaster* /*source*/)
{
   
}
