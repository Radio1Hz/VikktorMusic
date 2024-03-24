/*
  ==============================================================================

    BaseComponent.cpp
    Created: 18 Aug 2021 9:23:07am
    Author:  viktor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "BaseComponent.h"
using namespace juce;
//==============================================================================
BaseComponent::BaseComponent()
{

}

BaseComponent::BaseComponent(int ms)
{
    minSize = ms;
}

BaseComponent::~BaseComponent()
{
    removeAllChildren();
}

void BaseComponent::paint (Graphics& g)
{
    drawOutline(g);
}
float BaseComponent::getFontSize()
{
    return 10.0f * (float)(getReducedLocalBounds().getWidth())/ (float)(this->getTopLevelComponent()->getWidth());
}
void BaseComponent::drawOutline (Graphics& g)
{
    Rectangle<int> rect = getLocalBounds();
    g.setColour(Colours::black);
    g.fillRect(rect);

    g.setFont(15.0f);
    rect.setHeight(headerHeight);
    g.setColour(Colours::darkorange);
    g.fillRect(rect);
    g.drawRect(getLocalBounds(), 1);
    g.setColour(Colours::white);
    rect.translate(5, 0);
    
    juce::String state = juce::String(name);
    state.append(juce::String(" S:"), 7);
    state.append(juce::String(component_state), 6);
    
    g.drawText(state, rect, Justification::centredLeft, true);
}

void BaseComponent::resized()
{
    setSize(250, 250);
}

void BaseComponent::zoomEvent(const juce::MouseEvent & /*event*/, const juce::MouseWheelDetails & /*wheel*/){}
void BaseComponent::controlClickEvent(const juce::MouseEvent& /*event*/) {}
void BaseComponent::controlDragEvent(const juce::MouseEvent& /*event*/) {}
void BaseComponent::controlDoubleClickEvent(const juce::MouseEvent& /*event*/) {}
void BaseComponent::controlMouseDownEvent(const juce::MouseEvent& /*event*/) {}
void BaseComponent::controlMouseUpEvent(const juce::MouseEvent& /*event*/) {}
void BaseComponent::shiftMouseDownEvent(const juce::MouseEvent& /*event*/) {}

Rectangle<int> BaseComponent::getReducedLocalBounds()
{
    Rectangle<int> rect = getLocalBounds();
    rect.translate(1, headerHeight);
    rect.setHeight(rect.getHeight() - headerHeight - 1);
    rect.setWidth(rect.getWidth() - 2);
    return rect;
}

Rectangle<int> BaseComponent::getReducedBounds()
{
    Rectangle<int> rect = getBounds();
    rect.translate(1, headerHeight);
    rect.setHeight(rect.getHeight() - headerHeight - 1);
    rect.setWidth(rect.getWidth() - 2);
    return rect;
}

void BaseComponent::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
    if (event.mods.isCtrlDown())
    {
        zoomEvent(event, wheel);
    }
    else
    {
        juce::Rectangle<float> rect = getBounds().toFloat();
        //float originalSize = rect.getWidth();
        Point<float> originalCenter = rect.getCentre();
        float newSize = rect.getWidth() * (1.0f + wheel.deltaY);
        if (newSize <= (float)minSize)
        {
            newSize = (float)minSize;
        }
        rect.setSize(newSize, newSize);
        rect.setCentre(originalCenter);
        setBounds((int)rect.getX(), (int)rect.getY(), (int)rect.getWidth(), (int)rect.getHeight());
        getParentComponent()->repaint();
    }
}


void BaseComponent::mouseDrag(const juce::MouseEvent& event)
{
    if (!event.mods.isShiftDown())
    {
        if (event.mods.isCtrlDown())
        {
            controlDragEvent(event);
        }
        else
        {
            myDragger.dragComponent(this, event, nullptr);
            getParentComponent()->repaint();
        }
    }
    
}

void BaseComponent::mouseUp(const juce::MouseEvent& event)
{
    if (event.mods.isCtrlDown())
    {
        controlMouseUpEvent(event);
    }
}

void BaseComponent::mouseDown(const juce::MouseEvent& event)
{
    if (event.mods.isCtrlDown())
    {
        if (event.getNumberOfClicks() == 1)
        {
            controlClickEvent(event);
        }

        if (event.getNumberOfClicks() == 2)
        {
            controlDoubleClickEvent(event);
        }

        controlMouseDownEvent(event);
    }
    else if(event.mods.isShiftDown())
    {
        shiftMouseDownEvent(event);
    }
    else
    {
        myDragger.startDraggingComponent(this, event);
    }
}