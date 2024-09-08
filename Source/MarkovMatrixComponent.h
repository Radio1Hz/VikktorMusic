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
#include "MarkovMatrixComponent.h"
#include "BaseComponent.h"
#include "CommunicationAgent.h"
#include "TimerComponent.h"
#include "ArraySorter.h"

using namespace juce;
using namespace std;
//==============================================================================

class MarkovMatrixComponent : public BaseComponent, public CommunicationAgent
{
public:
    MarkovMatrixComponent(int size, bool readOnly=false);
    MarkovMatrixComponent(vector<int> states, int initialState, vector<vector<float>> transitionMatrix);
    ~MarkovMatrixComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    int nextState();

    int current_state = 0;

private:
    //************************   simple variables   ************************
    int current_state_index = 0;

    int size = 4;
    int smaller_size = 1;
    void rearrangeMatrix(ProbabilitySlider* d, int algorythmId);
    //************************   JUCE complex objects   ************************
    std::unique_ptr<juce::dsp::Matrix<float>> matrix;
    juce::OwnedArray<ProbabilitySlider> sliders;
    vector<int> states;
    void markCurrentState();
    juce::Array<float> getProbabilitiesArray(bool);
    void resetMatrix();
    void createChildren();

    //************************   functions, methods   ************************
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MarkovMatrixComponent)
};
