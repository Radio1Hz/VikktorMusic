/*
  ==============================================================================

    PlotComponentLogistic.h
    Created: 21 Jun 2022 8:49:04pm
    Author:  viktor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "BaseComponent.h"
#include "CommunicationAgent.h"
#include "MathRenderer.h"

//==============================================================================
/*
*/
class PlotComponentLogistic  : public BaseComponent, public CommunicationAgent, public Slider::Listener, public KeyListener
{
public:
    PlotComponentLogistic();
    ~PlotComponentLogistic() override;
    void sliderValueChanged(Slider* slider) override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void zoomEvent(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;
    void controlClickEvent(const juce::MouseEvent& event) override;
    void controlDoubleClickEvent(const juce::MouseEvent& event) override;
    void controlDragEvent(const juce::MouseEvent& event) override;
    void controlMouseDownEvent(const juce::MouseEvent& event) override;
    void controlMouseUpEvent(const juce::MouseEvent& event) override;
    void paint(juce::Graphics&) override;
    void resized() override;
    bool keyPressed(const KeyPress& key, Component* originatingComponent) override;

private:
    bool renderedInitialized = false;
    MathRenderer* renderer;
    int generalIterator = 0;
    Point<float> dragStart;
    Point<float> dragEnd;
    Slider sliderR;
    Label labelR;
    Slider sliderStart;
    Label labelStart;
    
    //static juce::Identifier myNodeType(float);
    juce::ValueTree RPoints(Array<float>);
    int RPointsResolution = 100;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlotComponentLogistic)
};
