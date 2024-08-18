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
void BaseComponent::prepareToPlay(int /*samplesPerBlockExpected*/, double /*sampleRate*/)
{
}
void BaseComponent::releaseResources()
{
}
void BaseComponent::getNextAudioBlock(const AudioSourceChannelInfo& /*bufferToFill*/)
{
}

void BaseComponent::drawOutline (Graphics& g)
{
    if (!this->embeddedMode)
    {
        Rectangle<int> screenPortionRect = getScreenBounds();
        Rectangle<int> rect = getLocalBounds();
        //g.setColour(Colours::black);
        g.drawRect(rect);

        g.setFont(15.0f);
        rect.setHeight(headerHeight);
        g.setColour(Colours::darkred);
        g.fillRect(rect);
        g.drawRect(getLocalBounds(), 1);
        g.setColour(Colours::white);
        rect.translate(5, 0);

        String state = String(name);
        state.append(String(" S:"), 7);
        state.append(String(component_state), 6);

        if (screenPortionRect.getX() < 0)
        {
            rect.translate(-screenPortionRect.getX(), 0);
        }
        g.drawText(state, rect, Justification::centredLeft, true);
    }
}

void BaseComponent::resized()
{
    setSize(250, 250);
}

void BaseComponent::zoomEvent(const MouseEvent & /*event*/, const MouseWheelDetails & /*wheel*/){}
void BaseComponent::controlClickEvent(const MouseEvent& /*event*/) {}
void BaseComponent::controlDragEvent(const MouseEvent& /*event*/) {}
void BaseComponent::shiftDragEvent(const MouseEvent& /*event*/){}
void BaseComponent::shiftMouseUpEvent(const MouseEvent& /*event*/){}
void BaseComponent::controlDoubleClickEvent(const MouseEvent& /*event*/) {}
void BaseComponent::controlMouseDownEvent(const MouseEvent& /*event*/) {}
void BaseComponent::controlMouseUpEvent(const MouseEvent& /*event*/) {}
void BaseComponent::shiftMouseDownEvent(const MouseEvent& /*event*/) {}
void BaseComponent::mouseDoubleClickEvent(const MouseEvent& /*event*/){}
void BaseComponent::mouseMoveEvent(const MouseEvent& /*event*/) {}
void BaseComponent::mouseDownEvent(const MouseEvent& /*event*/){}
void BaseComponent::mouseUpEvent(const MouseEvent& /*event*/){}

Rectangle<int> BaseComponent::getReducedLocalBounds()
{
    Rectangle<int> rect = getLocalBounds();
    if (!this->embeddedMode)
    {
        rect.translate(1, headerHeight);
        rect.setHeight(rect.getHeight() - headerHeight - 1);
        rect.setWidth(rect.getWidth() - 2);
    }
    return rect;
}

Rectangle<int> BaseComponent::getReducedBounds()
{
    Rectangle<int> rect = getBounds();
    if (!this->embeddedMode)
    {
        rect.translate(1, headerHeight);
        rect.setHeight(rect.getHeight() - headerHeight - 1);
        rect.setWidth(rect.getWidth() - 2);
    }
    return rect;
}

void BaseComponent::mouseWheelMove(const MouseEvent& event, const MouseWheelDetails& wheel)
{
    if (event.mods.isCtrlDown())
    {
        zoomEvent(event, wheel);
    }
    else
    {
        if (!this->embeddedMode)
        {
            Rectangle<int> rect = getReducedBounds();
            Point<float> originPointRelative = Point<float>(((float)(event.x - 1) / (float)rect.getWidth()), (int)((float)event.y - (float)headerHeight + 1.0f) / (float)rect.getHeight());
            int deltaWidth = (int)((float)rect.getWidth() * wheel.deltaY);
            int deltaHeight = (int)((float)rect.getHeight() * wheel.deltaY);

            int newWidth = rect.getWidth() + deltaWidth;
            int newHeight = rect.getHeight() + deltaHeight;
            double aspectRatio = (double)newWidth / (double)newHeight;

            if (newHeight > minSize && newWidth < maxSize)
            {
                if (newWidth > maxSize)
                {
                    newWidth = maxSize;
                    newHeight = roundToInt((double)newWidth / aspectRatio);
                }
                if (newHeight <= minSize)
                {
                    newHeight = minSize;
                    newWidth = roundToInt((double)newHeight * aspectRatio);
                }

                rect.setSize(newWidth, newHeight);
                rect.translate(roundToInt(-(float)deltaWidth * originPointRelative.x), roundToInt(-(float)deltaHeight * originPointRelative.y));

                setBounds((int)rect.getX() - 1, (int)rect.getY() - headerHeight, (int)rect.getWidth() + 2, (int)rect.getHeight() + headerHeight + 1);
                getParentComponent()->repaint();
            }
        }
        else
        {
            getParentComponent()->mouseWheelMove(event, wheel);
        }
        
    }
     
}
void BaseComponent::mouseMove(const MouseEvent& event)
{
    mouseMoveEvent(event);
}

void BaseComponent::mouseDrag(const MouseEvent& event)
{
    if (event.mods.isCtrlDown())
    {
        controlDragEvent(event);
    }
    else
    {
        if (event.mods.isShiftDown())
        {
            shiftDragEvent(event);
        }
        else
        {
            if (!embeddedMode)
            {
                myDragger.dragComponent(this, event, nullptr);
                getParentComponent()->repaint();
            }
        }
        
    }
}

void BaseComponent::mouseUp(const MouseEvent& event)
{
    if (event.mods.isCtrlDown())
    {
        controlMouseUpEvent(event);
    }

    if (event.mods.isShiftDown())
    {
        shiftMouseUpEvent(event);
    }
    else
    {
        mouseUpEvent(event);
    }
    
}

void BaseComponent::mouseDown(const MouseEvent& event)
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
        if (event.getNumberOfClicks() == 2)
        {
            mouseDoubleClickEvent(event);
        }
        myDragger.startDraggingComponent(this, event);
        mouseDownEvent(event);
    }

    if (menu.getNumItems() > 0)
    {
        if (event.mods.isRightButtonDown())
        {
            menu.showMenuAsync(PopupMenu::Options(),
                [](int result)
                {
                    if (result == 0)
                    {
                        // user dismissed the menu without picking anything
                    }
                    else if (result == 1)
                    {
                        // user picked item 1
                    }
                    else if (result == 2)
                    {
                        // user picked item 2

                    }
                });
        }
    }
}


