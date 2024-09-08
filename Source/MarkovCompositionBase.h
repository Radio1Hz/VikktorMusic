/*
  ==============================================================================

	MarkovCompositionBase.h
	Created: 27 Aug 2024 8:36:54am
	Author:  viktor

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "MusicMath.h"
#include "ApplicationProperties.h"
#include "CompositionBase.h"
#include "MarkovMatrixComponent.h"

using namespace juce;
using namespace std;

class MarkovCompositionBase : public CompositionBase
{
public:
	MarkovCompositionBase()
	{
	}

	~MarkovCompositionBase()
	{
	}

	void generateRhythm(int numMeasures, vector<vector<NoteEventDesc>>& noteEventMatrix, float numQuartersPerMeasure, int numTimeUnitsPerMeasure, int noteRangeStart, int noteRangeEnd, MarkovMatrixComponent* matrix)
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

	void generateMelody(int numMeasures, vector<vector<NoteEventDesc>>& noteEventMatrix, float /*numQuartersPerMeasure*/, int numTimeUnitsPerMeasure, int noteRangeStart, int noteRangeEnd) override
	{
		// Generate melody using basicRhythm
		NoteEventDesc rhythmEventDesc(0, 1, 2);
		vector<vector<ContextDesc>> vecMeasures = AppProperties::getContextPerMeasureVector();
		vector<vector<vector<ContextDesc>>> vecMeasuresQuarters = AppProperties::getContextPerMeasureAndQuarterVector();
		ContextDesc currentContext(0, 0);

		int musicalThoughtLengthInMeasures = (int)ceil((float)basicRhythmsPattern[0].size() / (float)numTimeUnitsPerMeasure);
		int currentMusicalThoughtIndex = -1;
		int previousNote = -1;

		vector<float> commonNotesForAllContexts(12);
		MusicMath tempMath;

		float maxRes = 0;
		for (int i = 0; i < 12; i++)
		{
			float resultingMult = 1.0f;
			for (ContextDesc& desc : contextsInUse)
			{
				int roleID = desc.getNoteRoleIndexByAbsoluteMIDINoteNumber(i);
				if (roleID != -1)
				{
					if (roleID == 0 || roleID == 2 || roleID == 4)
					{
						resultingMult *= 2.0f;
					}
					else
					{
						resultingMult *= 1.0f;
					}
				}
				else
				{
					resultingMult = 0.0f;
				}
			}
			commonNotesForAllContexts[i] = resultingMult;
			if (maxRes < resultingMult)
			{
				maxRes = resultingMult;
			}
		}

		for (int i = 0; i < 12; i++) 
		{
			commonNotesForAllContexts[i] = commonNotesForAllContexts[i] / maxRes;
		}

		for (int i = 0; i < numTimeUnitsPerMeasure * numMeasures; i++)
		{
			bool noteIsGenerated = false;
			int currentMeasure = (int)(i / numTimeUnitsPerMeasure);
			currentContext = ContextDesc(vecMeasures[currentMeasure][0]);
			if (i % (musicalThoughtLengthInMeasures * numTimeUnitsPerMeasure) == 0)
			{
				currentMusicalThoughtIndex = (currentMusicalThoughtIndex + 1) % basicRhythmsPattern.size();
			}

			if (basicRhythmsPattern[currentMusicalThoughtIndex][i % (int)(basicRhythmsPattern[currentMusicalThoughtIndex].size())] != "")
			{
				int tempRoleIndex = -1;
				String symbol1 = basicRhythmsPattern[currentMusicalThoughtIndex][i % (int)(basicRhythmsPattern[currentMusicalThoughtIndex].size())];

				if (MusicMath::isNumber(symbol1))
				{
					tempRoleIndex = String(basicRhythmsPattern[currentMusicalThoughtIndex][i % (int)(basicRhythmsPattern[currentMusicalThoughtIndex].size())]).getIntValue();
				}
				else
				{
					if (symbol1 == "C") //Consonance
					{
						tempRoleIndex = MusicMath::getRandomConsonanceRoleIndex(commonNotesForAllContexts, previousNote, currentContext, false);
					}
					if (symbol1 == "D") //Dissonance
					{
						tempRoleIndex = MusicMath::getRandomDissonanceRoleIndex(commonNotesForAllContexts, previousNote, currentContext);
					}
					noteIsGenerated = true;
				}

				if (tempRoleIndex != -1 && vecMeasures[currentMeasure].size() > 0)
				{
					int nextNote = MusicMath::getNoteNumberByRoleNumber(vecMeasures[currentMeasure][0].RootMIDINote, vecMeasures[currentMeasure][0].Mode, tempRoleIndex);
					if (nextNote != -1)
					{
						nextNote += 12;
						if (noteRangeStart <= nextNote && nextNote <= noteRangeEnd)
						{
							NoteEventDesc newEvent(nextNote, 2, 1, tempRoleIndex, noteIsGenerated);
							noteEventMatrix[newEvent.NoteNumber - noteRangeStart][i] = newEvent;
						}
					}
				}
			}
		}
	}
	String Name = "";
};

