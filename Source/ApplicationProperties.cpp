/*
  ==============================================================================

	ApplicationProperties.cpp
	Created: 11 Aug 2024 12:32:46pm
	Author:  viktor

  ==============================================================================
*/
#include <JuceHeader.h>
#include "ApplicationProperties.h"
#include "MusicMath.h"

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

int AppProperties::getNumMeasures()
{
	return AppProperties::numMeasures;
}

void AppProperties::setNumMeasures(int numMes)
{
	AppProperties::numMeasures = numMes;
}

bool AppProperties::getShouldSaveAudio()
{
	return AppProperties::shouldSaveAudio;
}

void AppProperties::setShouldSaveAudio(bool ssa)
{
	AppProperties::shouldSaveAudio = ssa;
}

bool AppProperties::getInternalSynthAudioOut()
{
	return AppProperties::internalSynthAudioOut;
}

void AppProperties::setInternalSynthAudioOut(bool iSO)
{
	AppProperties::internalSynthAudioOut = iSO;
}

bool AppProperties::getMIDIOut()
{
	return AppProperties::MIDIOut;
}

void AppProperties::setMIDIOut(bool mO)
{
	AppProperties::MIDIOut = mO;
}

String AppProperties::getProjectPath()
{
	return AppProperties::projectPath;
}

void AppProperties::setProjectPath(String path)
{
	AppProperties::projectPath = path;
}

vector<vector<vector<ContextDesc>>> AppProperties::getContextPerMeasureAndQuarterVector()
{
	if (AppProperties::contextPerMeasureAndQuarterVector.size() == 0)
	{
		AppProperties::contextPerMeasureAndQuarterVector.resize(numMeasures);
	}
	return AppProperties::contextPerMeasureAndQuarterVector;
}

vector<vector<ContextDesc>> AppProperties::getContextPerMeasureVector()
{
	if (AppProperties::contextPerMeasureVector.size() == 0)
	{
		AppProperties::contextPerMeasureVector.resize(numMeasures);
	}
	return AppProperties::contextPerMeasureVector;
}

void AppProperties::setContextPerMeasureAndQuarterVector(vector<vector<vector<ContextDesc>>> vec)
{
	AppProperties::contextPerMeasureAndQuarterVector = vec;
}

void AppProperties::setContextPerMeasureVector(vector<vector<ContextDesc>> vec)
{
	AppProperties::contextPerMeasureVector = vec;
}

