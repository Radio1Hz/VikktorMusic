/*
  ==============================================================================

    MarkovMatrix.h
    Created: 14 Aug 2021 7:20:36pm
    Author:  viktor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ProbabilitySlider.h"
#include "BaseComponent.h"
#include "CommunicationAgent.h"
using namespace juce;
//==============================================================================
/*
*/
class MarkovMatrixComponent  : public BaseComponent, public CommunicationAgent
{
public:
    MarkovMatrixComponent(int size);
    ~MarkovMatrixComponent() override;

    void paint (juce::Graphics&) override;
    void markCurrentState();
    void resized() override;
    juce::Array<double> getProbabilitiesArray();
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    
    void nextStep();
    void resetMatrix();
    void createChildren();

private:
    //************************   simple variables   ************************
    int current_state = 0;
    int size = 4;
    int smaller_size = 1;
    void rearrangeMatrix(ProbabilitySlider* d, int algorythmId);
    //************************   JUCE complex objects   ************************
    juce::dsp::Matrix<double> *matrix;
    juce::OwnedArray<ProbabilitySlider> sliders;

    //************************   functions, methods   ************************
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MarkovMatrixComponent)
};
