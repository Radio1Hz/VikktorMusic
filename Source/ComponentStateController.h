/*
  ==============================================================================

    ComponentStateController.h
    Created: 11 Jun 2022 1:42:51pm
    Author:  viktor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "BaseComponent.h"   
#include "CommunicationAgent.h"   

//==============================================================================
/*
*/
class ComponentStateController  : public BaseComponent, public juce::Timer, public CommunicationAgent, public juce::Button::Listener
{
public:
    ComponentStateController();
    ~ComponentStateController() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void changeListenerCallback(ChangeBroadcaster* source) override;
    void buttonClicked(juce::Button* button) override;

private:
    juce::TextButton buttonMinus;
    juce::TextButton buttonPlus;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ComponentStateController)
};
