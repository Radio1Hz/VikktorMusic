/*
  ==============================================================================

    BaseAudioAppComponent.h
    Created: 28 Aug 2021 2:23:23pm
    Author:  viktor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
using namespace juce;

//==============================================================================

class BaseAudioAppComponent  : public juce::AudioAppComponent
{
public:
    BaseAudioAppComponent();
    ~BaseAudioAppComponent() override;

    void mouseWheelMove(const MouseEvent& event, const MouseWheelDetails& wheel) override;
    void paint(Graphics&) override;
    virtual void zoomEvent(const MouseEvent& event, const MouseWheelDetails& wheel);
    void drawOutline(Graphics&);
    void mouseDrag(const MouseEvent& event) override;
    void mouseUp(const MouseEvent& event) override;
    void mouseDown(const MouseEvent& event) override;
    Rectangle<int> getReducedLocalBounds();
    Rectangle<int> getReducedBounds();
    void resized() override;
  
public:
    int minSize = 50;
    String name = "Unknown";
    int headerHeight = 20;
    ComponentDragger myDragger;
    int defaultSize = 200;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BaseAudioAppComponent)
};
