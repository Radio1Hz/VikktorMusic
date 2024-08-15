/*
  ==============================================================================

    ApplicationProperties.cpp
    Created: 11 Aug 2024 12:32:46pm
    Author:  viktor

  ==============================================================================
*/
#include <JuceHeader.h>
#include "ApplicationProperties.h"

using namespace juce;

AppProperties::AppProperties()
{
   
}

float AppProperties::getTempo()
{
    return AppProperties::tempo;
}

void AppProperties::setTempo(float tmp)
{
    AppProperties::tempo = tmp;
}

int AppProperties::getNumerator()
{
    return AppProperties::numerator;
}

void AppProperties::setNumerator(int num)
{
    AppProperties::numerator = num;
}

int AppProperties::getDenominator()
{
    return AppProperties::denominator;
}

void AppProperties::setDenominator(int den)
{
    AppProperties::denominator = den;
}

bool AppProperties::getShouldSaveAudio()
{
    return AppProperties::shouldSaveAudio;
}

void AppProperties::setShouldSaveAudio(bool ssa)
{
    AppProperties::shouldSaveAudio = ssa;
}

String AppProperties::getProjectPath()
{
    return AppProperties::projectPath;
}

void AppProperties::setProjectPath(String path)
{
    AppProperties::projectPath = path;
}

