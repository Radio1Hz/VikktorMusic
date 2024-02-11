/*
  ==============================================================================

    TimeObject.cpp
    Created: 10 Aug 2021 10:08:19am
    Author:  viktor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "TimeObject.h"

//==============================================================================
TimeObjectComponent::TimeObjectComponent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    
    this->name = "Time Object ";
    
}

TimeObjectComponent::~TimeObjectComponent()
{
    removeAllChildren();
    deleteAllChildren();
}

void TimeObjectComponent::paint (juce::Graphics& g)
{
    drawOutline(g);
    g.setColour (juce::Colours::white);
    
    for (int i = 0; i < levelDimensions.size(); i++)
    {
        Rectangle<int> rect = getReducedLocalBounds();
        rect.setWidth(getReducedLocalBounds().getWidth() / levelDimensions[i]); 
        rect.setHeight(getReducedLocalBounds().getWidth() / levelDimensions[i]);
        for (int j = 0; j < levelDimensions[i]; j++)
        {
            g.drawRect(rect, 1);
            rect.translate(rect.getWidth(), 0);
        }
    }
}

void TimeObjectComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
}

void  TimeObjectComponent::changeListenerCallback(juce::ChangeBroadcaster*)
{

}
