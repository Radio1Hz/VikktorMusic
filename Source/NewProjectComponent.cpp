/*
  ==============================================================================

  This is an automatically generated GUI class created by the Projucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Projucer version: 7.0.11

  ------------------------------------------------------------------------------

  The Projucer is part of the JUCE library.
  Copyright (c) 2020 - Raw Material Software Limited.

  ==============================================================================
*/

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "NewProjectComponent.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
NewProjectComponent::NewProjectComponent ()
{
    //[Constructor_pre] You can add your own custom stuff here..
    //[/Constructor_pre]

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

    lengthLabel.reset (new juce::Label ("lengthLabel",
                                        TRANS ("Length [sec]")));
    addAndMakeVisible (lengthLabel.get());
    lengthLabel->setFont (juce::Font (14.80f, juce::Font::plain).withTypefaceStyle ("Regular"));
    lengthLabel->setJustificationType (juce::Justification::centredLeft);
    lengthLabel->setEditable (false, false, false);
    lengthLabel->setColour (juce::TextEditor::textColourId, juce::Colours::black);
    lengthLabel->setColour (juce::TextEditor::backgroundColourId, juce::Colour (0x00000000));

    lengthLabel->setBounds (16, 48, 95, 24);

    lengthEditor.reset (new juce::TextEditor ("new text editor"));
    addAndMakeVisible (lengthEditor.get());
    lengthEditor->setMultiLine (false);
    lengthEditor->setReturnKeyStartsNewLine (false);
    lengthEditor->setReadOnly (false);
    lengthEditor->setScrollbarsShown (true);
    lengthEditor->setCaretVisible (true);
    lengthEditor->setPopupMenuEnabled (true);
    lengthEditor->setText (TRANS ("120"));

    lengthEditor->setBounds (129, 51, 54, 24); 


    //[UserPreSize]
    //[/UserPreSize]

    setSize (200, 200);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

NewProjectComponent::~NewProjectComponent()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    okButton = nullptr;
    bpmLabel = nullptr;
    bpmEditor = nullptr;
    lengthLabel = nullptr;
    lengthEditor = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void NewProjectComponent::paint (juce::Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (juce::Colour (0xff323e44));

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void NewProjectComponent::resized()
{
    //[UserPreResize] Add your own custom resize code here..
    //[/UserPreResize]

    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void NewProjectComponent::buttonClicked (juce::Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == okButton.get())
    {
        //[UserButtonCode_okButton] -- add your button handler code here..
        //[/UserButtonCode_okButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Projucer information section --

    This is where the Projucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="TestComponent" componentName=""
                 parentClasses="public juce::Component" constructorParams="" variableInitialisers=""
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="300" initialHeight="300">
  <BACKGROUND backgroundColour="ff323e44"/>
  <TEXTBUTTON name="okButton" id="1e39b879d69c1107" memberName="okButton" virtualName=""
              explicitFocusOrder="0" pos="120 128 62 24" buttonText="OK" connectedEdges="0"
              needsCallback="1" radioGroupId="0"/>
  <LABEL name="bpmLabel" id="d0144d91b4ace9cc" memberName="bpmLabel" virtualName=""
         explicitFocusOrder="0" pos="16 16 78 24" edTextCol="ff000000"
         edBkgCol="0" labelText="BPM" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="14.8"
         kerning="0.0" bold="0" italic="0" justification="33"/>
  <TEXTEDITOR name="new text editor" id="79a7001fd6b7595a" memberName="bpmEditor"
              virtualName="" explicitFocusOrder="0" pos="128 16 54 24" initialText="60"
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
  <LABEL name="lengthLabel" id="b680e1ab0d8fb70" memberName="lengthLabel"
         virtualName="" explicitFocusOrder="0" pos="16 48 95 24" edTextCol="ff000000"
         edBkgCol="0" labelText="Length [sec]" editableSingleClick="0"
         editableDoubleClick="0" focusDiscardsChanges="0" fontname="Default font"
         fontsize="14.8" kerning="0.0" bold="0" italic="0" justification="33"/>
  <TEXTEDITOR name="new text editor" id="82b2e4e3a3f83787" memberName="lengthEditor"
              virtualName="" explicitFocusOrder="0" pos="129 51 54 24" initialText="120"
              multiline="0" retKeyStartsLine="0" readonly="0" scrollbars="1"
              caret="1" popupmenu="1"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]

