/*
  ==============================================================================

    PlotComponent.cpp
    Created: 22 Aug 2021 3:10:44pm
    Author:  viktor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PlotComponent.h"

//==============================================================================
PlotComponent::PlotComponent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    this->name = "Plot";
}

PlotComponent::~PlotComponent()
{
    //delete renderer;
    removeAllChildren();
    deleteAllChildren();
}

void PlotComponent::paint (juce::Graphics& g)
{
    drawOutline(g);
}

void PlotComponent::zoomEvent(const juce::MouseEvent& /*event*/, const juce::MouseWheelDetails&)
{
    repaint();
}

void PlotComponent::controlClickEvent(const juce::MouseEvent& event)
{
    renderer->SetNewMathPointer(renderer->ScreenToMathPoint(Point<float>((float)event.x, (float)event.y)));
    repaint();
}

void PlotComponent::controlDoubleClickEvent(const juce::MouseEvent& event)
{
    renderer->SetNewMathCenter((float)event.x, (float)event.y);
    repaint();
}

void PlotComponent::controlMouseDownEvent(const juce::MouseEvent&)
{
    
    repaint();
}
void PlotComponent::controlMouseUpEvent(const juce::MouseEvent&)
{

    repaint();
}
void PlotComponent::controlDragEvent(const juce::MouseEvent&)
{
    repaint();
}

void PlotComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    
    
}

void PlotComponent::changeListenerCallback(juce::ChangeBroadcaster*)
{
   
    repaint();
}


