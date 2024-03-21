/*
  ==============================================================================

    ComponentStateController.cpp
    Created: 11 Jun 2022 1:42:51pm
    Author:  viktor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ComponentStateController.h"

//==============================================================================
ComponentStateController::ComponentStateController()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    this->name = "State Controller";
    buttonMinus.setButtonText("-");
    buttonPlus.setButtonText("+");
    buttonMinus.addListener(this);
    buttonPlus.addListener(this);
    addAndMakeVisible(buttonMinus);
    addAndMakeVisible(buttonPlus);
    
}

ComponentStateController::~ComponentStateController()
{

}

void ComponentStateController::paint (juce::Graphics& g)
{
    drawOutline(g);
    juce::Rectangle<int> rect = getReducedLocalBounds();
    rect.setHeight(rect.getHeight()/2);
    g.setColour(juce::Colours::lightgrey);
    g.setFont(30.0f);
    g.drawText(juce::String(component_state), rect, Justification::centred, true);
}

void ComponentStateController::buttonClicked(juce::Button* button)
{
    if (button == &buttonMinus)
    {
        component_state--;
        if (component_state < 0)
        {
            this->component_state = 0; 
        }
        this->data = this->component_state;
        sendChangeMessage();
        repaint();
    }

    if (button == &buttonPlus)
    {
        component_state++;
        this->data = this->component_state;
        sendChangeMessage();
        repaint();
    }
}

void ComponentStateController::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
    Rectangle<int> reducedBounds = getReducedLocalBounds();
    reducedBounds.setWidth(reducedBounds.getWidth() / 2);
    reducedBounds.setHeight(reducedBounds.getHeight() / 2);
    reducedBounds.translate(0, reducedBounds.getHeight());
    buttonMinus.setBounds(reducedBounds);
    reducedBounds.translate(reducedBounds.getWidth(), 0);
    buttonPlus.setBounds(reducedBounds);
    
    //renderer = new MathRenderer(getReducedLocalBounds(), Point<float>(0.5, 0.5), 1.2f);
    //sliderStart.setValue(0.5);
    //sliderR.setValue(1.0);
    //renderer->SetNewFontSize(getFontSize());
    //renderedInitialized = true;
}

void ComponentStateController::changeListenerCallback(ChangeBroadcaster* /*source*/)
{
    //((CommunicationAgent*)source)->id;
}

void ComponentStateController::timerCallback()
{
  
}
