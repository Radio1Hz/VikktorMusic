/*
  ==============================================================================

    EuclidianRythmTimerComponent.h
    Created: 10 Sep 2021 11:32:13pm
    Author:  viktor

  ==============================================================================
*/
#include <JuceHeader.h>
#include "TimerComponent.h"


#pragma once
using namespace juce;
//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class EuclidianRythmTimerComponent : public TimerComponent,  public Slider::Listener
{
public:
    //==============================================================================
    EuclidianRythmTimerComponent(int size, int pulses);
    ~EuclidianRythmTimerComponent();
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void timerCallback() override;
    void sliderValueChanged(Slider* slider) override;
    void generateSequence();
    void resized() override;
    int numberOfPulses;
    Array<float> rhythm;


protected:
    void paint(Graphics&) override;
    Random rnd;
    int gcd(int a, int b);
    Slider sliderTotal;
    Slider sliderPulses;
   
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EuclidianRythmTimerComponent)
};