/*
  ==============================================================================

    DebugComponent.cpp
    Created: 2 Aug 2021 7:18:37am
    Author:  viktor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "DebugComponent.h"

//==============================================================================
DebugComponent::DebugComponent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
}

void DebugComponent::setInformation(juce::String info)
{
    this->information = juce::String(info);
    repaint();
}

DebugComponent::~DebugComponent()
{
    
}

void DebugComponent::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */
    
    g.fillAll (juce::Colours::darkblue); // clear the background

    g.setColour (juce::Colours::orange);
    g.setFont (12.0f);

    g.drawText (information, getLocalBounds(),
                juce::Justification::left, true);   // draw some placeholder text
}

void DebugComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}
