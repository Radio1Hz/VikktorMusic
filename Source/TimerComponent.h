/*
  ==============================================================================

    TimerComponent.h
    Created: 4 Aug 2021 5:56:51pm
    Author:  viktor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "MarkovMatrixComponent.h"   
#include "BaseComponent.h"   
#include "CommunicationAgent.h"   

//==============================================================================
/*
*/
class TimerComponent  : public BaseComponent, public juce::Timer, public CommunicationAgent
{
public:
    TimerComponent();
    TimerComponent(int);
    TimerComponent(int, int);
    ~TimerComponent() override;

    int numberOfSteps = 0;
    void paint (Graphics&) override;
    void resized() override;
    void start(int);
    void start();
    void timerCallback() override;
    //void onChangeObserved(ChangeBroadcaster* source) override;
    void changeListenerCallback(ChangeBroadcaster* source) override;
    void shiftMouseDownEvent(const juce::MouseEvent& event) override;

protected:
    int currentStep = 0;
    int period_ms = 0;
    bool shouldPaint = false;
    OwnedArray<TimerComponent> childrenComponents;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TimerComponent)
};
