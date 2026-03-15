/*
  ==============================================================================

    PlotComponentLogistic.cpp
    Created: 21 Jun 2022 8:49:04pm
    Author:  viktor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "PlotComponentLogistic.h"

//==============================================================================
PlotComponentLogistic::PlotComponentLogistic()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    this->name = "Plot Logistic";
    
    addAndMakeVisible(sliderR);
    sliderR.setRange(0, 4);
    sliderR.addListener(this);

    addAndMakeVisible(sliderStart);
    sliderStart.setRange(0, 1);
    sliderStart.addListener(this);

    addAndMakeVisible(labelR);
    labelR.setText("r (0-4)", juce::dontSendNotification);
    labelR.attachToComponent(&sliderR, true);

    addAndMakeVisible(labelStart);
    labelStart.setText("x (0-1)", juce::dontSendNotification);
    labelStart.attachToComponent(&sliderStart, true);

    this->component_state = 0;

    // Add key listener and make focusable
    addKeyListener(this);
    setWantsKeyboardFocus(true);
}

PlotComponentLogistic::~PlotComponentLogistic()
{
    delete renderer;
    removeAllChildren();
    deleteAllChildren();
}

void PlotComponentLogistic::sliderValueChanged(juce::Slider* sliderParam)
{
    if (sliderParam == &sliderR)
    {
        renderer->lambda = (float)sliderR.getValue();
		renderer->setLambda((float)sliderR.getValue());
        repaint();
    }
    if (sliderParam == &sliderStart)
    {
        renderer->start = (float)sliderStart.getValue();
        
        /*for (float rv = 0; rv < 4; rv += 4 / RPointsResolution)
        {
            RPoints.add(rv, NULL);
        }
        */
        repaint();
    }
}

void PlotComponentLogistic::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    //renderer->changeListenerCallback(source);
    if (CommunicationAgent* cb = dynamic_cast<CommunicationAgent*>(source))
    {
        this->component_state = cb->data;
        renderer->state = cb->data;
        if (renderer->state < 4)
        {
            labelR.setText("r (0-4)", juce::dontSendNotification);
            sliderR.setRange(0, 4);
            labelStart.setText("x0 (0-1)", juce::dontSendNotification);
            sliderStart.setRange(0, 1);
        }
        else
        {
            labelR.setText("r (0-10)", juce::dontSendNotification);
            sliderR.setRange(0, 10);
            labelStart.setText("x0 (0-3.1415)", juce::dontSendNotification);
            sliderStart.setRange(0, 3.1415);

        }
    }
    this->getParentComponent()->repaint();
    repaint();
}

void PlotComponentLogistic::controlClickEvent(const juce::MouseEvent& event)
{
    renderer->SetNewMathPointer(renderer->ScreenToMathPoint(Point<float>((float)event.x, (float)event.y)));
    repaint();
}

void PlotComponentLogistic::controlDoubleClickEvent(const juce::MouseEvent& event)
{
    renderer->SetNewMathCenter((float)event.x, (float)event.y);
    repaint();
}

void PlotComponentLogistic::paint (juce::Graphics& g)
{
    g.fillAll(Colour::fromRGB(31, 31, 31));  // or any color you want as background
    drawOutline(g);
    sliderStart.setVisible(false);
    if (component_state > 2)
    {
        sliderStart.setVisible(true);
    }
    Rectangle<int> remainingRect = getReducedLocalBounds();
    renderer->Draw(g, remainingRect.toFloat());

}
void PlotComponentLogistic::zoomEvent(const juce::MouseEvent&, const juce::MouseWheelDetails& wheel)
{
    renderer->SetNewMathSize(-wheel.deltaY);
    renderer->SetNewFontSize(getFontSize());
    repaint();
}
void PlotComponentLogistic::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    auto sliderLeft = getWidth()/2;
    sliderR.setBounds(sliderLeft, 20, getWidth() - sliderLeft - 10, 20);
    labelR.setBounds(sliderLeft - 50, 20, 50, 20);
    sliderStart.setBounds(sliderLeft, 50, getWidth() - sliderLeft - 10, 20);
    labelStart.setBounds(sliderLeft-50, 50, 50, 20);

    if (!renderedInitialized)
    {
        renderer = new MathRenderer(getReducedLocalBounds(), Point<float>(1.5, 0.5), 1.5f);
        sliderStart.setValue(0.5);
        sliderR.setValue(1.0);
        renderer->SetNewFontSize(getFontSize());
        renderedInitialized = true;
    }
    
    renderer->area = getReducedLocalBounds();
}


void PlotComponentLogistic::controlMouseDownEvent(const juce::MouseEvent&)
{
    repaint();
}
void PlotComponentLogistic::controlMouseUpEvent(const juce::MouseEvent&)
{

    repaint();
}
void PlotComponentLogistic::controlDragEvent(const juce::MouseEvent&)
{
    repaint();
}

bool PlotComponentLogistic::keyPressed(const KeyPress& key, Component* /*originatingComponent*/)
{
    if (key == KeyPress::rightKey)
    {
        renderer->state++;
        component_state = renderer->state;
        repaint();
        return true;
    }
    else if (key == KeyPress::leftKey)
    {
        renderer->state--;
        component_state = renderer->state;
        repaint();
        return true;
    }
    else if (key == KeyPress::spaceKey)
    {
        renderer->generalIterator++;
        repaint();
        return true;
    }
    return false;
}
