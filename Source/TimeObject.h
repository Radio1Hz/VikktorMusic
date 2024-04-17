/*
  ==============================================================================

    TimeObject.h
    Created: 10 Aug 2021 10:08:19am
    Author:  viktor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "BaseComponent.h"
#include "FunctionProvider.h"
#include "CommunicationAgent.h"
#include "MarkovMatrix.h"
#include "BjorklundAlgorithm.h"

using namespace juce;
//==============================================================================
/*
*/

class TimeObjectComponent: public BaseComponent, public CommunicationAgent
{
public:
    TimeObjectComponent();
    ~TimeObjectComponent() override;
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;
    void paint (juce::Graphics&) override;
    void resized() override;
    Array<int> levelDimensions; // [2, 4, 4]

private:

    FunctionProvider fp;
    //float **rhythm; // [10][10001000][10001000100010001000100010001000]
    int max_steps = 4;
    //dsp::Matrix<double> *timeTableMatrix;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TimeObjectComponent)
};

class TimePatternSeed
{
public: 

    TimePatternSeed()
    {
        this->level = 0;
        this->size = 1;
        this->frequency = 1;    
        this->rnd = Random::getSystemRandom();
        this->phase = rnd.nextInt(this->size);
    }

    TimePatternSeed(int level, int size, int frequency)
    {
        this->level = level;
        this->size = size;
        this->frequency = frequency;
        this->rnd = Random::getSystemRandom();
        this->phase = rnd.nextInt(this->size);
    }

    ~TimePatternSeed()
    {
        
    }

    void collapsePattern(MarkovMatrix*)
    {
        Bjorklund* bO = new Bjorklund(size, frequency, 0);
        vector<bool> viktor = bO->LoadSequence();
    }

    int level = 0;
    int size = 1;
    int frequency = 1;
    int phase = 0;

    
private:
    Random rnd;
};

class TimePattern
{
public:

    TimePattern()
    {

    }
};