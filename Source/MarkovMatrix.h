/*
  ==============================================================================

    MarkovMatrix.h
    Created: 15 Oct 2021 1:14:15pm
    Author:  viktor

  ==============================================================================
*/

#include <JuceHeader.h>
#include <math.h>

using namespace juce;

#pragma once
class MarkovMatrix
{

public:
    MarkovMatrix();
    MarkovMatrix(int size);
    ~MarkovMatrix();
    void addSymbol(String symbol);
    
private:
    juce::dsp::Matrix<double>* matrix;
    Array<String> symbols;
};
