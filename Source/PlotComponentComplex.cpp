/*
  ==============================================================================

    PlotComponentComplex.cpp
    Created: 19 Apr 2026 1:49:24pm
    Author:  viktor

  ==============================================================================
*/
#include <JuceHeader.h>
#include "PlotComponentComplex.h"

//==============================================================================
PlotComponentComplex::PlotComponentComplex()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    this->name = "Plot Logistic";
    this->showInFullscreen = true;

    this->component_state = 0;
	this->renderer = new MathRendererComplex(getReducedLocalBounds(), Point<float>(0, 0), 10.0);
    // Add key listener and make focusable
    addKeyListener(this);
    setWantsKeyboardFocus(true);
}

PlotComponentComplex::~PlotComponentComplex()
{
    delete renderer;
    removeAllChildren();
    deleteAllChildren();
}

void PlotComponentComplex::controlClickEvent(const juce::MouseEvent& event)
{
    renderer->SetNewMathPointer(renderer->ScreenToMathPoint(Point<float>((float)event.x, (float)event.y)));
    repaint();
}

void PlotComponentComplex::controlDoubleClickEvent(const juce::MouseEvent& event)
{
    renderer->SetNewMathCenter((float)event.x, (float)event.y);
    repaint();
}

void PlotComponentComplex::paint(juce::Graphics& g)
{
	g.setColour(juce::Colours::lightgrey);
    g.fillAll(Colour::fromRGB(31, 31, 31));  // or any color you want as background
    drawOutline(g);
    g.drawSingleLineText("Press left/right to change state, space to iterate in state 3", 10, getHeight() - 60);
    g.drawSingleLineText("State: " + juce::String(component_state), 10, 20);
	renderer->Draw(g);
}

void PlotComponentComplex::zoomEvent(const juce::MouseEvent&, const juce::MouseWheelDetails& wheel)
{
    renderer->SetNewMathSize(-wheel.deltaY);
    renderer->SetNewFontSize(getFontSize());
    repaint();
}
void PlotComponentComplex::resized()
{
    renderer->area = getReducedLocalBounds();
}


void PlotComponentComplex::controlMouseDownEvent(const juce::MouseEvent&)
{
    repaint();
}
void PlotComponentComplex::controlMouseUpEvent(const juce::MouseEvent&)
{

    repaint();
}
void PlotComponentComplex::controlDragEvent(const juce::MouseEvent&)
{
    repaint();
}

bool PlotComponentComplex::keyPressed(const KeyPress& key, Component* /*originatingComponent*/)
{
    if (key == KeyPress::rightKey)
    {
        if (renderer->state < 5)
        {
            renderer->state++;
            component_state = renderer->state;
            repaint();
            return true;
        }
        return false;
    }
    else if (key == KeyPress::leftKey)
    {
        if (renderer->state > 0)
        {
            renderer->state--;
            component_state = renderer->state;
            repaint();
            return true;
        }
        return false;
    }
    else if (key == KeyPress::spaceKey)
    {
        renderer->generalIterator++;
        repaint();  
        return true;
    }
    return false;
}

void PlotComponentComplex::sliderValueChanged(Slider* slider)
{
    // Handle slider value changes here 
}

void PlotComponentComplex::changeListenerCallback(juce::ChangeBroadcaster* source)
{
}
