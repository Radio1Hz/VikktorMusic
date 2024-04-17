/*
  ==============================================================================

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/


#include "NewProjectComponent.h"

//==============================================================================
NewProjectComponent::NewProjectComponent ()
{

    okButton.reset (new juce::TextButton ("okButton"));
    addAndMakeVisible (okButton.get());
    okButton->setButtonText (TRANS ("OK"));
    okButton->addListener (this);

    okButton->setBounds (120, 128, 62, 24);

    bpmLabel.reset (new juce::Label ("bpmLabel",
                                     TRANS ("BPM")));
    addAndMakeVisible (bpmLabel.get());
    bpmLabel->setFont (juce::Font (14.80f, juce::Font::plain).withTypefaceStyle ("Regular"));
    bpmLabel->setJustificationType (juce::Justification::centredLeft);
    bpmLabel->setEditable (false, false, false);
    bpmLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    bpmLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    bpmLabel->setBounds (16, 16, 78, 24);

    bpmEditor.reset (new juce::TextEditor ("new text editor"));
    addAndMakeVisible (bpmEditor.get());
    bpmEditor->setMultiLine (false);
    bpmEditor->setReturnKeyStartsNewLine (false);
    bpmEditor->setReadOnly (false);
    bpmEditor->setScrollbarsShown (true);
    bpmEditor->setCaretVisible (true);
    bpmEditor->setPopupMenuEnabled (true);
    bpmEditor->setText (TRANS ("60"));

    bpmEditor->setBounds (128, 16, 54, 24);

    measuresLabel.reset (new juce::Label ("lengthLabel",
                                        TRANS ("Length [sec]")));
    addAndMakeVisible (measuresLabel.get());
    measuresLabel->setFont (juce::Font (14.80f, juce::Font::plain).withTypefaceStyle ("Regular"));
    measuresLabel->setJustificationType (juce::Justification::centredLeft);
    measuresLabel->setEditable (false, false, false);
    measuresLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    measuresLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    measuresLabel->setBounds (16, 48, 95, 24);

    measuresEditor.reset (new juce::TextEditor ("new text editor"));
    addAndMakeVisible (measuresEditor.get());
    measuresEditor->setMultiLine (false);
    measuresEditor->setReturnKeyStartsNewLine (false);
    measuresEditor->setReadOnly (false);
    measuresEditor->setScrollbarsShown (true);
    measuresEditor->setCaretVisible (true);
    measuresEditor->setPopupMenuEnabled (true);
    measuresEditor->setText (TRANS ("120"));

    measuresEditor->setBounds (129, 51, 54, 24);
    setSize (300, 200);
}

NewProjectComponent::~NewProjectComponent()
{
    okButton = nullptr;
    bpmLabel = nullptr;
    bpmEditor = nullptr;
    measuresLabel = nullptr;
    measuresEditor = nullptr;
    exitModalState(0);
}

//==============================================================================
void NewProjectComponent::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff323e44));
}

void NewProjectComponent::resized()
{

}

void NewProjectComponent::buttonClicked (juce::Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == okButton.get())
    {
        
    }
}
