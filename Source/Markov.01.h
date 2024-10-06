/*
  ==============================================================================

	Markov01.h
	Created: 6 Oct 2024 4:33:28pm
	Author:  viktor

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "MarkovCompositionBase.h"
#include "MusicMath.h"
#include "ApplicationProperties.h"

using namespace juce;
using namespace std;
class Markov01 : public MarkovCompositionBase
{
public:
	Markov01()
	{}

	void generateRhythm(int numMeasures, vector<vector<NoteEventDesc>>& noteEventMatrix, float numQuartersPerMeasure, int numTimeUnitsPerMeasure, int noteRangeStart, int noteRangeEnd, MarkovMatrixComponent* matrix) override
	{
		int duration = 0;
		int musicalThoughtDuration = 16;
		vector<string> generatedRhythm(musicalThoughtDuration);

		for (int measureIndex = 0; measureIndex < numMeasures; measureIndex++)
		{
			duration = 0;
			while (duration < musicalThoughtDuration)
			{
				for (int i = 0; i < matrix->current_state; i++)
				{
					if (i == 0)
					{
						generatedRhythm[duration++] = "C";
					}
					else
					{
						generatedRhythm[duration++] = "";
					}
					if (duration == musicalThoughtDuration)
					{
						break;
					}
				}
				matrix->nextState();
			}
			basicRhythmsPattern.push_back(generatedRhythm);
		}
		generateContexts(numMeasures, noteEventMatrix, numQuartersPerMeasure, numTimeUnitsPerMeasure, noteRangeStart, noteRangeEnd);
		generateMelody(numMeasures, noteEventMatrix, numQuartersPerMeasure, numTimeUnitsPerMeasure, noteRangeStart, noteRangeEnd);
	}

	void generateContexts(int numMeasures, vector<vector<NoteEventDesc>>& /*noteEventMatrix*/, float /*numQuartersPerMeasure*/, int /*numTimeUnitsPerMeasure*/, int /*noteRangeStart*/, int /*noteRangeEnd*/) override
	{
		vector<vector<ContextDesc>> vecko(numMeasures);
		for (int i = 0; i < numMeasures; i++)
		{
			vector<ContextDesc> desc{ ContextDesc(48, 0) };
			vecko[i] = desc;
		}
		AppProperties::setContextPerMeasureVector(vecko);
		contextsInUse = vector <ContextDesc>{ ContextDesc(48, 0) };
	}


private:
	int markovOrder;
};