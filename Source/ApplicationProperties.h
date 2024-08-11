/*
  ==============================================================================

    ApplicationProperties.h
    Created: 11 Aug 2024 12:32:46pm
    Author:  viktor

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
using namespace juce;

class AppProperties
{
public:
    
    static float getTempo();
    static void setTempo(float tmp);

    static int getNumerator();
    static void setNumerator(int num);

    static int getDenominator();
    static void setDenominator(int den);
    
    static String getProjectPath();
    static void setProjectPath(String path);

    static AppProperties& getInstance()
    {
        static AppProperties instance;
        return instance;
    }

private:
    AppProperties();
    inline static float tempo;
    inline static int numerator;
    inline static int denominator;
    inline static String projectPath;
};