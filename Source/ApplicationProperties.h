/*
  ==============================================================================

	ApplicationProperties.h
	Created: 11 Aug 2024 12:32:46pm
	Author:  viktor

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "MusicMath.h"

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

	static int getNumMeasures();
	static void setNumMeasures(int numMes);

	static bool getShouldSaveAudio();
	static void setShouldSaveAudio(bool ssa);

	static bool getInternalSynthAudioOut();
	static void setInternalSynthAudioOut(bool iSO);

	static bool getMIDIOut();
	static void setMIDIOut(bool mO);

	static String getProjectPath();
	static void setProjectPath(String path);

	static vector<vector<vector<ContextDesc>>> getContextPerMeasureAndQuarterVector();
	static vector<vector<ContextDesc>> getContextPerMeasureVector();
	static void setContextPerMeasureAndQuarterVector(vector<vector<vector<ContextDesc>>> vec);
	static void setContextPerMeasureVector(vector<vector<ContextDesc>> vec);

	static AppProperties& getInstance()
	{
		static AppProperties instance;
		return instance;
	}

private:
	AppProperties();
	inline static float tempo;
	inline static bool shouldSaveAudio = false;
	inline static bool internalSynthAudioOut = false;
	inline static bool MIDIOut = false;
	inline static int numerator;
	inline static int denominator;
	inline static int numMeasures = 0;
	inline static vector<vector<vector<ContextDesc>>> contextPerMeasureAndQuarterVector;
	inline static vector<vector<ContextDesc>> contextPerMeasureVector;
	inline static String projectPath;
};