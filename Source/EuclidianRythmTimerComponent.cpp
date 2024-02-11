/*
  ==============================================================================

    EuclidianRythmTimerComponent.cpp
    Created: 10 Sep 2021 11:32:13pm
    Author:  viktor

  ==============================================================================
*/

#include "EuclidianRythmTimerComponent.h"
#include "BjorklundAlgorithm.h"

EuclidianRythmTimerComponent::EuclidianRythmTimerComponent(int size, int pulses) : TimerComponent::TimerComponent(size, 300)
{
    numberOfSteps = size;
    numberOfPulses = pulses;
    
    rnd = Random::getSystemRandom();

    addAndMakeVisible(sliderTotal);
    addAndMakeVisible(sliderPulses);
    
    sliderTotal.setRange(1, 32, 1);
    sliderTotal.addListener(this);
    sliderTotal.setValue(numberOfSteps);

    sliderPulses.setRange(numberOfPulses, numberOfSteps, 1);
    sliderPulses.addListener(this);
    sliderPulses.setValue(numberOfPulses);

    int res = gcd(numberOfSteps, numberOfPulses);
    generateSequence();
    name = "Euclidian Rhythm";
}

void EuclidianRythmTimerComponent::generateSequence()
{
    Bjorklund* bjorki = new Bjorklund(numberOfSteps, numberOfPulses, 0);
    vector<bool> sequence;
    sequence = bjorki->LoadSequence();
 
    rhythm.clear();

    for (int i = 0; i < numberOfSteps; i++)
    {
        if (sequence[i])
        {
            rhythm.add(1);
        }
        else
        {
            rhythm.add(0);
        }
    }
    delete bjorki;
}
void EuclidianRythmTimerComponent::sliderValueChanged(Slider* sliderParam)
{
    if (sliderParam == &sliderTotal)
    {
        numberOfSteps = sliderTotal.getValue();
        sliderPulses.setRange(1, (double)numberOfSteps, 1);
        
    }
    if (sliderParam == &sliderPulses)
    {
        numberOfPulses = sliderPulses.getValue();
        
    }
    generateSequence();
}

void EuclidianRythmTimerComponent::paint(Graphics& g)
{
    drawOutline(g);
    Rectangle<int> reducedBounds = getReducedLocalBounds();
    reducedBounds.setHeight(10);
    g.setColour(juce::Colours::lightgrey);
    g.setFont(12.0f);
    g.drawText(juce::String(this->currentStep), reducedBounds, juce::Justification::centred, true);   // draw some placeholder text
    reducedBounds.translate(0, 10);

    int unitWidth = reducedBounds.getWidth() / numberOfSteps;
    reducedBounds.setWidth(unitWidth);
    Rectangle<float> unitRectangle(reducedBounds.toFloat());
    unitRectangle.setHeight(unitWidth);

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

        g.setColour(Colours::lightgrey);
        Rectangle<int> elRect;
        elRect.setWidth(unitWidth * rhythm[i]);
        elRect.setHeight(unitWidth * rhythm[i]);
        elRect.setCentre(unitRectangle.getCentre().toInt());

        if (rhythm[i] < 0.5)
        {
            g.setColour(juce::Colours::darkgrey);
        }
        
        g.fillEllipse(elRect.getX(), elRect.getY(), unitWidth * rhythm[i], unitWidth * rhythm[i]);

        unitRectangle.translate(unitWidth, 0);
    }
}
void EuclidianRythmTimerComponent::resized()
{
    Rectangle<int> reducedBounds = getReducedLocalBounds();
    reducedBounds.setHeight(reducedBounds.getHeight()/10);
    reducedBounds.translate(0, 8*reducedBounds.getHeight());
    sliderTotal.setBounds(reducedBounds);
    reducedBounds.translate(0, reducedBounds.getHeight());
    sliderPulses.setBounds(reducedBounds);
}
int EuclidianRythmTimerComponent::gcd(int a, int b)
{
    if (a == 0)
        return b;
    return gcd(b % a, a);
}

EuclidianRythmTimerComponent::~EuclidianRythmTimerComponent()
{

}

void EuclidianRythmTimerComponent::timerCallback()
{
   //TimerComponent::sendChangeMessage();
}

void EuclidianRythmTimerComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    repaint();
    if (ChangeBroadcaster* cb = dynamic_cast<ChangeBroadcaster*>(source))
    {
        if (rhythm[this->currentStep] >= 0.5f)
        {
            TimerComponent::sendChangeMessage();
        }
    }

    this->currentStep = (this->currentStep + 1) % this->numberOfSteps;
}