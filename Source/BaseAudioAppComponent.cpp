/*
  ==============================================================================

    BaseAudioAppComponent.cpp
    Created: 28 Aug 2021 2:23:23pm
    Author:  viktor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "BaseAudioAppComponent.h"

using namespace juce;

//==============================================================================
BaseAudioAppComponent::BaseAudioAppComponent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.

}

BaseAudioAppComponent::~BaseAudioAppComponent()
{

}
void BaseAudioAppComponent::zoomEvent(const MouseEvent& event, const MouseWheelDetails& wheel)
{}
void BaseAudioAppComponent::mouseWheelMove(const MouseEvent& event, const MouseWheelDetails& wheel)
{
    if (event.mods.isCtrlDown())
    {
        zoomEvent(event, wheel);
    }
    else
    {
        juce::Rectangle<int> rect = getBounds();
        float originalSize = rect.getWidth();
        float newSize = (float)rect.getWidth() * (1 + wheel.deltaY);
        if (newSize <= minSize)
        {
            newSize = minSize;
        }
        rect.setSize(newSize, newSize);
        setBounds(rect);
    }
}
void BaseAudioAppComponent::paint(Graphics& g)
{
    
}

void BaseAudioAppComponent::drawOutline(Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));   // clear the background
    Rectangle<int> rect = getLocalBounds();
    
    g.setFont(15.0f);
    rect.setHeight(headerHeight);
    g.setColour(Colours::darkblue);
    g.fillRect(rect);
    g.drawRect(getLocalBounds(), 1);
    g.setColour(Colours::white);
    g.drawText(name, rect, Justification::centredLeft, true);   // draw an outline around the component
}

Rectangle<int> BaseAudioAppComponent::getReducedLocalBounds()
{
    Rectangle<int> rect = getLocalBounds();
    rect.translate(1, headerHeight);
    rect.setHeight(rect.getHeight() - headerHeight - 1);
    rect.setWidth(rect.getWidth() - 2);
    return rect;
}
void BaseAudioAppComponent::resized()
{

}


Rectangle<int> BaseAudioAppComponent::getReducedBounds()
{
    Rectangle<int> rect = getBounds();
    rect.translate(1, headerHeight);
    rect.setHeight(rect.getHeight() - headerHeight - 1);
    rect.setWidth(rect.getWidth() - 2);
    return rect;
}


void BaseAudioAppComponent::mouseDrag(const juce::MouseEvent& event)
{
    myDragger.dragComponent(this, event, nullptr);
    getParentComponent()->repaint();
}

void BaseAudioAppComponent::mouseUp(const juce::MouseEvent& event)
{

}

void BaseAudioAppComponent::mouseDown(const juce::MouseEvent& event)
{
    myDragger.startDraggingComponent(this, event);
}