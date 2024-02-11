/*
  ==============================================================================

    ProbabilitySlider.h
    Created: 14 Aug 2021 9:45:43pm
    Author:  viktor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
using namespace juce;
class ProbabilitySlider  : public juce::Component, public juce::Slider::Listener, public juce::ChangeBroadcaster
{
public:
    ProbabilitySlider(double value, int row, int col);
    ~ProbabilitySlider() override;
public:
    int row = 0;
    int col = 0;
    bool isHighlighted = false;
    void paint (juce::Graphics&) override;
    void resized() override;
    void mouseDoubleClick(const juce::MouseEvent& event) override;
    void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;
    void sliderValueChanged(juce::Slider* slider) override;

    double getValue();
    void setValue(double val);

private:
    juce::Slider slider;
    bool mouseOver = false;
    double value = 0;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProbabilitySlider)
};
