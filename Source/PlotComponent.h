/*
  ==============================================================================

    PlotComponent.h
    Created: 22 Aug 2021 3:10:44pm
    Author:  viktor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "BaseComponent.h"
#include "CommunicationAgent.h"
#include "MathRenderer.h"

using namespace juce;
//==============================================================================
/*
*/
class PlotComponent  : public BaseComponent, public CommunicationAgent
{
public:
    PlotComponent();
    ~PlotComponent() override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void zoomEvent(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;
    void controlClickEvent(const juce::MouseEvent& event) override;
    void controlDoubleClickEvent(const juce::MouseEvent& event) override;
    void controlDragEvent(const juce::MouseEvent& event) override;
    void controlMouseDownEvent(const juce::MouseEvent& event) override;
    void controlMouseUpEvent(const juce::MouseEvent& event) override;
    void paint (juce::Graphics&) override;
    void resized() override;
    bool renderedInitialized = false;
    MathRenderer* renderer;
    int generalIterator = 0;
    Point<float> dragStart;
    Point<float> dragEnd;
    

private:
    
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PlotComponent)
};
