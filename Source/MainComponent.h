#pragma once

#include <JuceHeader.h>
#include "DebugComponent.h"
#include "TimerComponent.h"
#include "LogSpaceComponent.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::Component, juce::KeyListener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

   
    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;
    bool MainComponent::keyPressed(const KeyPress& key, Component* originatingComponent) override;

    LogSpaceComponent logSpaceComponent;

private:
    //==============================================================================
    // Your private member variables go here...
    DebugComponent debugComponent;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
