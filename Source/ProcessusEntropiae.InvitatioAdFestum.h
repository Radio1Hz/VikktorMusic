/*
  ==============================================================================

	ProcessusEntropiae.InvitatioAdFestum.h
	Created: 27 Aug 2024 8:33:09am
	Author:  viktor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CompositionBase.h"
#include "MusicMath.h"
#include "ApplicationProperties.h"

using namespace juce;
using namespace std;
class InvitatioAdFestum : CompositionBase
{
public:
	InvitatioAdFestum()
	{
	}
	~InvitatioAdFestum()
	{
	}
	void generateContexts(int numMeasures, vector<vector<NoteEventDesc>>& noteEventMatrix, float numQuartersPerMeasure, int numTimeUnitsPerMeasure, int noteRangeStart, int noteRangeEnd) override
	{
		vector<vector<ContextDesc>> vecMeasures = AppProperties::getContextPerMeasureVector();
		vector<vector<vector<ContextDesc>>> vecMeasuresQuarters = AppProperties::getContextPerMeasureAndQuarterVector();
		int currentRepetitionNumber = 0; //Ionian
		vector<int> numRepetitions{ 1, 1, 1, 1 };

		/*
			C	0
			C#	1
			D	2
			D#	3
			E	4
			F	5
			F#	6
			G	7
			G#	8
			A	9
			A#	10
			B	11
			//Invitatio ad festum
			Gm				G#				Am				A#
			7-1				8-3				9-1				10-3
		*/

		vector<ContextDesc> contextsInUse{ ContextDesc(middleC + 7, 1), ContextDesc(middleC + 8, 3), ContextDesc(middleC + 9, 1), ContextDesc(middleC + 10, 3) };
		int index = numRepetitions[currentRepetitionNumber];
		int currentContextInUseIndex = 0; //First Context in vector
		int root = contextsInUse[currentContextInUseIndex].RootMIDINote; // C + keyIndex
		int currentMode = contextsInUse[currentContextInUseIndex].Mode;
		bool shouldCreateChordNotes = true;
		ContextDesc previousContext;

		if (vecMeasures[0].size() == 0)
		{
			//Generate Contexts
			for (int i = 0; i < numMeasures; i++)
			{
				if (index <= 0)
				{
					index = numRepetitions[(++currentRepetitionNumber) % (int)numRepetitions.size()];
					previousContext = contextsInUse[(++currentContextInUseIndex) % (int)contextsInUse.size()];
					currentMode = previousContext.Mode;
					root = previousContext.RootMIDINote;
				}
				index--;
				ContextDesc cDesc(root, currentMode, 1.0f);
				vecMeasures[i].push_back(cDesc);

				for (int j = 0; j < (int)ceil(numQuartersPerMeasure); j++)
				{
					ContextDesc cDescQuarter(root, currentMode, 1.0f);
					if (vecMeasuresQuarters[i].size() == 0)
					{
						vecMeasuresQuarters[i].resize((int)ceil(numQuartersPerMeasure));
					}
					vecMeasuresQuarters[i][j].push_back(cDescQuarter);

					if (shouldCreateChordNotes)
					{
						int duration = (int)ceil(numTimeUnitsPerMeasure / numQuartersPerMeasure);

						if (j == (int)ceil(numQuartersPerMeasure) - 1)
						{
							if ((int)ceil(numQuartersPerMeasure) - (int)floor(numQuartersPerMeasure) > 0)
							{
								duration = 2 * ((int)ceil(numQuartersPerMeasure) - (int)floor(numQuartersPerMeasure));
							}
						}

						if (duration > 0)
						{
							NoteEventDesc descRoot(cDesc.getAbsoluteNoteFromKeyModeAndRole(0), duration, 1);  // Root = 0
							NoteEventDesc descThird(cDesc.getAbsoluteNoteFromKeyModeAndRole(2), duration, 1); // Third = 2
							NoteEventDesc descFifth(cDesc.getAbsoluteNoteFromKeyModeAndRole(4), duration, 1); // Fifth = 4
							noteEventMatrix[cDesc.getAbsoluteNoteFromKeyModeAndRole(0) - noteRangeStart][i * numTimeUnitsPerMeasure + j * (int)floor(numTimeUnitsPerMeasure / numQuartersPerMeasure)] = descRoot;
							noteEventMatrix[cDesc.getAbsoluteNoteFromKeyModeAndRole(2) - noteRangeStart][i * numTimeUnitsPerMeasure + j * (int)floor(numTimeUnitsPerMeasure / numQuartersPerMeasure)] = descThird;
							noteEventMatrix[cDesc.getAbsoluteNoteFromKeyModeAndRole(4) - noteRangeStart][i * numTimeUnitsPerMeasure + j * (int)floor(numTimeUnitsPerMeasure / numQuartersPerMeasure)] = descFifth;
						}
					}
				}
			}
			AppProperties::setContextPerMeasureVector(vecMeasures);
			AppProperties::setContextPerMeasureAndQuarterVector(vecMeasuresQuarters);
		}


		// Generate melody using basicRhythm
		NoteEventDesc rhythmEventDesc(0, 1, 2);

		vector<vector<int>> basicRhythms2
		{
			{
				1, -1, -1, -1,		1, -1, -1, -1,		3, -1, 2, -1,		-1, -1, 2, -1,
				-1, -1, 1, -1,		-1, -1, 4, -1,		-1, -1, -1, -1,		-1, -1, 3, -1,
				5, -1, -1, -1,		5, -1,-1, -1,		4, -1, 3, -1,		-1, -1, 3, -1,
				-1, -1, -1, -1,		-1, -1, -1, -1,		-1, -1, -1, -1,		-1, -1, -1, -1,

				1, -1, -1, -1,		1, -1, -1, -1,		3, -1, 2, -1,		-1, -1, 2, -1,
				-1, -1, 1, -1,		-1, -1, 4, -1,		-1, -1, -1, -1,		-1, -1, 3, -1,
				5, -1, -1, -1,		5, -1,-1, -1,		4, -1, 3, -1,		-1, -1, 3, -1,
				2, -1, -1, -1,		3, -1, -1, -1,		4, -1, 5, -1,		 -1, -1, 6, -1
			},
			{
				2, -1, -1, -1,		2, -1, -1, -1,		2, -1, 3, -1,		-1, -1, 3, -1,
				-1, -1, 2, -1,		-1, -1, -1, -1,		-1, -1, -1, -1,		-1, -1, 2, -1,
				2, -1, -1, -1,		2, -1, -1, -1,		2, -1, 3, -1,		-1, -1, 3, -1,
				-1, -1, -1, -1,		-1, -1, -1, -1,		2, -1, 1, -1,		1, -1, 0, -1,

				2, -1, -1, -1,		2, -1, -1, -1,		2, -1, 3, -1,		-1, -1, 3, -1,
				-1, -1, 2, -1,		-1, -1, -1, -1,		-1, -1, -1, -1,		-1, -1, 2, -1,
				2, -1, -1, -1,		2, -1, -1, -1,		2, -1, 3, -1,		-1, -1, 3, -1,
				-1, -1, -1, -1,		-1, -1, -1, -1,		-1, -1, -1, -1,		-1, -1, -1, -1
			}

		};

		vector<vector<string>> basicRhythmsPattern
		{
			// C - Consonace
			// D - Dissonace
			{
				"C", "", "", "",		"C", "", "", "",		"C", "", "C", "",		"", "", "C", "",
				"", "", "C", "",		"", "", "C", "",		"", "", "", "",			"", "", "C", "",
				"C", "", "", "",		"C", "","", "",			"C", "", "C", "",		"", "", "C", "",
				"", "", "", "",			"", "", "", "",			"", "", "", "",			"", "", "", "",

				"C", "", "", "",		"C", "", "", "",		"C", "", "C", "",		"", "", "C", "",
				"", "", "C", "",		"", "", "C", "",		"", "", "", "",			"", "", "C", "",
				"C", "", "", "",		"C", "","", "",			"C", "", "C", "",		"", "", "C", "",
				"C", "", "", "",		"C", "", "", "",		"C", "", "C", "",		"", "", "C", ""
			},
			{
				"2", "", "", "",		"2", "", "", "",		"2", "", "3", "",		"", "", "3", "",
				"", "", "2", "",		"", "", "", "",			"", "", "", "",			"", "", "2", "",
				"2", "", "", "",		"2", "", "", "",		"2", "", "3", "",		"", "", "3", "",
				"", "", "", "",			"", "", "", "",			"2", "", "1", "",		"1", "", "0", "",

				"2", "", "", "",		"2", "", "", "",		"2", "", "3", "",		"", "", "3", "",
				"", "", "2", "",		"", "", "", "",			"", "", "", "",			"", "", "2", "",
				"2", "", "", "",		"2", "", "", "",		"2", "", "3", "",		"", "", "3", "",
				"", "", "", "",			"", "", "", "",			"", "", "", "",			"", "", "", ""
			}
		};


		int musicalThoughtLengthInMeasures = (int)ceil((float)basicRhythmsPattern[0].size() / (float)numTimeUnitsPerMeasure);
		int currentMusicalThoughtIndex = -1;
		int previousNote = -1;
		previousContext = vecMeasures[0][0];
		
		for (int i = 0; i < numTimeUnitsPerMeasure * numMeasures; i++)
		{
			bool noteIsGenerated = false;
			int currentMeasure = (int)(i / numTimeUnitsPerMeasure);

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
						tempRoleIndex = MusicMath::getRandomConsonanceRoleIndex(previousNote, previousContext, false);
					}
					if (symbol1 == "D") //Dissonance
					{
						tempRoleIndex = MusicMath::getRandomDissonanceRoleIndex(previousNote, previousContext);
					}
					noteIsGenerated = true;
				}

				int nextNote = 12 + MusicMath::getNoteNumberByRoleNumber(vecMeasures[currentMeasure][0].RootMIDINote, vecMeasures[currentMeasure][0].Mode, tempRoleIndex);
				if (noteRangeStart <= nextNote && nextNote <= noteRangeEnd)
				{
					NoteEventDesc newEvent(nextNote, 2, 1, tempRoleIndex, noteIsGenerated);
					noteEventMatrix[newEvent.NoteNumber - noteRangeStart][i] = newEvent;
					previousNote = nextNote;
				}
			}

			previousContext = vecMeasures[currentMeasure][0];
		}
	}
};