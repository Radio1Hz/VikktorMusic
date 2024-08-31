/*
  ==============================================================================

	CompositionBase.h
	Created: 27 Aug 2024 8:36:54am
	Author:  viktor

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "MusicMath.h"
#include "ApplicationProperties.h"

using namespace juce;
using namespace std;
class CompositionBase
{
public:
	CompositionBase()
	{
	}

	~CompositionBase()
	{
	}

	void generateContexts(int numMeasures, vector<vector<NoteEventDesc>>& noteEventMatrix, float /*numQuartersPerMeasure*/, int numTimeUnitsPerMeasure, int noteRangeStart, int /*noteRangeEnd*/)
	{
		vector<vector<ContextDesc>> vecMeasures = AppProperties::getContextPerMeasureVector();
		vector<vector<vector<ContextDesc>>> vecMeasuresQuarters = AppProperties::getContextPerMeasureAndQuarterVector();
		int currentRepetitionNumber = 0; //Ionian

		int index = numRepetitions[currentRepetitionNumber];
		int currentContextInUseIndex = 0; //First Context in vector
		int root = contextsInUse[currentContextInUseIndex].RootMIDINote; // C + keyIndex
		int currentMode = contextsInUse[currentContextInUseIndex].Mode;
		bool shouldCreateChordNotes = true;
		ContextDesc currentContext(0, 0);

		//Generate Contexts
		for (int i = 0; i < numMeasures; i++)
		{
			currentContext = ContextDesc(root, currentMode, 1.0f);

			if (index <= 0)
			{
				index = numRepetitions[(++currentRepetitionNumber) % (int)numRepetitions.size()];
				currentContext = contextsInUse[(++currentContextInUseIndex) % (int)contextsInUse.size()];
				currentMode = currentContext.Mode;
				root = currentContext.RootMIDINote;
			}

			index--;
			vecMeasures[i].push_back(currentContext);
			// Set vecMeasureQuarters
			/*for (int j = 0; j < (int)ceil(numQuartersPerMeasure); j++)
			{
				ContextDesc cDescQuarter(root, currentMode, 1.0f);
				if (vecMeasuresQuarters[i].size() == 0)
				{
					vecMeasuresQuarters[i].resize((int)ceil(numQuartersPerMeasure));
				}
				vecMeasuresQuarters[i][j].push_back(cDescQuarter);
			}*/

			if (shouldCreateChordNotes)
			{
				NoteEventDesc descRoot(currentContext.getAbsoluteNoteFromKeyModeAndRole(0), numTimeUnitsPerMeasure, 1, 0, false, defaultContextVelocity);  // Root = 0
				NoteEventDesc descThird(currentContext.getAbsoluteNoteFromKeyModeAndRole(2), numTimeUnitsPerMeasure, 1, 2, false, defaultContextVelocity); // Third = 2
				NoteEventDesc descFifth(currentContext.getAbsoluteNoteFromKeyModeAndRole(4), numTimeUnitsPerMeasure, 1, 4, false, defaultContextVelocity); // Fifth = 4
				noteEventMatrix[currentContext.getAbsoluteNoteFromKeyModeAndRole(0) - noteRangeStart][i * numTimeUnitsPerMeasure] = descRoot;
				noteEventMatrix[currentContext.getAbsoluteNoteFromKeyModeAndRole(2) - noteRangeStart][i * numTimeUnitsPerMeasure] = descThird;
				noteEventMatrix[currentContext.getAbsoluteNoteFromKeyModeAndRole(4) - noteRangeStart][i * numTimeUnitsPerMeasure] = descFifth;
			}
		}
		AppProperties::setContextPerMeasureVector(vecMeasures);
		AppProperties::setContextPerMeasureAndQuarterVector(vecMeasuresQuarters);
	}

	void generateMelody(int numMeasures, vector<vector<NoteEventDesc>>& noteEventMatrix, float /*numQuartersPerMeasure*/, int numTimeUnitsPerMeasure, int noteRangeStart, int noteRangeEnd)
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

		for (int i = 0; i < 12; i++)
		{
			float resultingMult = 1.0f;
			for (ContextDesc& desc : contextsInUse)
			{
				int roleID = desc.getNoteRoleIndexByAbsoluteMIDINoteNumber(i);
				if (roleID != -1)
				{
					resultingMult = resultingMult * 1.0f;
				}
				else
				{
					resultingMult = 0.0f;
				}
			}
			commonNotesForAllContexts[i] = resultingMult;
		}

		for (int i = 0; i < 12; i++)
		{
			commonNotesForAllContexts[i] = commonNotesForAllContexts[i] / contextsInUse.size();
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

protected:
	int middleC = 48;
	vector<ContextDesc> contextsInUse;
	vector<int> numRepetitions;
	vector<vector<string>> basicRhythmsPattern;
	float defaultContextVelocity = 0.25f;
};
