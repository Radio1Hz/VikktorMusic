/*
  ==============================================================================

    TimerComponent.cpp
    Created: 4 Aug 2021 5:56:51pm
    Author:  viktor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "LogSpaceComponent.h"
#include "TimerComponent.h"

//==============================================================================

TimerComponent::TimerComponent()
{
    this->name = "Timer";
    this->numberOfSteps = 16;
}

TimerComponent::TimerComponent(int numberOfSteps)
{    
    this->numberOfSteps = numberOfSteps;
    this->name = "Timer " + String(numberOfSteps);
}

TimerComponent::TimerComponent(int numberOfSteps, int time_ms)
{
    this->numberOfSteps = numberOfSteps;
    this->name = "Timer " + String(numberOfSteps);
    this->period_ms = time_ms;
    this->start(time_ms);
}


TimerComponent::~TimerComponent()
{
    this->stopTimer();
}

void TimerComponent::timerCallback()
{
    this->currentStep++;
    if (this->currentStep % this->numberOfSteps == 0)
    {
        this->currentStep = 0;
    }
    this->data = this->currentStep;
    this->component_state = this->currentStep;
    sendChangeMessage();
    repaint();
}

void TimerComponent::start(int milliseconds)
{
    this->startTimer(milliseconds);
    this->name = "Timer " + String(numberOfSteps) + " / " + String(milliseconds) + "ms";
    repaint();
}

void TimerComponent::start()
{
    this->startTimer(this->period_ms);
    this->name = "Timer " + String(numberOfSteps) + " / " + String(this->period_ms) + "ms";
    repaint();
}

void TimerComponent::paint (juce::Graphics& g)
{
    drawOutline(g);
    Rectangle<int> reducedBounds = getReducedLocalBounds();
    reducedBounds.setHeight(10);
    g.setColour (juce::Colours::lightgrey);
    g.setFont (12.0f);
    g.drawText (juce::String(this->currentStep), reducedBounds, juce::Justification::centred, true);   // draw some placeholder text
    reducedBounds.translate(0, 10);
    
    int unitWidth = reducedBounds.getWidth() / numberOfSteps;
    reducedBounds.setWidth(unitWidth);
    Rectangle<float> unitRectangle(reducedBounds.toFloat());
    unitRectangle.setHeight(unitWidth * 1.0f);

    for (int i = 0; i < numberOfSteps; i++)
    {
        if (i == currentStep)
        {
            g.setColour(Colours::orange);
            g.drawRoundedRectangle(unitRectangle, 10, 1.0f);
        }
        else
        {
            g.setColour(Colours::lightgrey);
            g.drawRoundedRectangle(unitRectangle, 10, 0.25f);
        }
        
        unitRectangle.translate(unitWidth * 1.0f, 0);
    }
}

void TimerComponent::resized()
{
}

void TimerComponent::changeListenerCallback(ChangeBroadcaster* source)
{
    ((CommunicationAgent*)source)->id;
}

void TimerComponent::shiftMouseDownEvent(const juce::MouseEvent& /*event*/) {
    if (LogSpaceComponent* lsc = dynamic_cast<LogSpaceComponent*>(this->getParentComponent()))
    {
        lsc->listeningSourceComponent = this;
    }
}
