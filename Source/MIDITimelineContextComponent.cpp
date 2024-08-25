/*
  ==============================================================================

	MIDITimelineContextComponent.cpp
	Created: 25 Aug 2024 9:53:45am
	Author:  viktor

  ==============================================================================
*/

#include "MIDITimelineContextComponent.h"
#include "ApplicationProperties.h"
#include "MainComponent.h"

MIDITimelineContextComponent::MIDITimelineContextComponent(int nM) : MIDITimelineComponent(nM)
{
	this->synthID = 2;
	outlineColour = Colours::darkcyan;
}

MIDITimelineContextComponent::~MIDITimelineContextComponent()
{
}

void MIDITimelineContextComponent::generateRhythm()
{
}

void MIDITimelineContextComponent::generateContexts()
{
	vector<vector<ContextDesc>> vecMeasures(numMeasures);
	vector<vector<vector<ContextDesc>>> vecMeasuresQuarters(numMeasures);

	int middleC = 60;
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

	//Generate Contexts
	for (int i = 0; i < numMeasures; i++)
	{
		if (index <= 0)
		{
			index = numRepetitions[(++currentRepetitionNumber) % (int)numRepetitions.size()];
			ContextDesc& desc = contextsInUse[(++currentContextInUseIndex) % (int)contextsInUse.size()];
			currentMode = desc.Mode;
			root = desc.RootMIDINote;
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
					NoteEventDesc descRoot(cDescQuarter.getAbsoluteNoteFromKeyModeAndRole(0), duration, 1);  // Root = 0
					NoteEventDesc descThird(cDescQuarter.getAbsoluteNoteFromKeyModeAndRole(2), duration, 1); // Third = 2
					NoteEventDesc descFifth(cDescQuarter.getAbsoluteNoteFromKeyModeAndRole(4), duration, 1); // Fifth = 4
					noteEventMatrix[cDescQuarter.getAbsoluteNoteFromKeyModeAndRole(0) - musicMath.getNoteRangeStart()][i * numTimeUnitsPerMeasure + j * (int)floor(numTimeUnitsPerMeasure / numQuartersPerMeasure)] = descRoot;
					noteEventMatrix[cDescQuarter.getAbsoluteNoteFromKeyModeAndRole(2) - musicMath.getNoteRangeStart()][i * numTimeUnitsPerMeasure + j * (int)floor(numTimeUnitsPerMeasure / numQuartersPerMeasure)] = descThird;
					noteEventMatrix[cDescQuarter.getAbsoluteNoteFromKeyModeAndRole(4) - musicMath.getNoteRangeStart()][i * numTimeUnitsPerMeasure + j * (int)floor(numTimeUnitsPerMeasure / numQuartersPerMeasure)] = descFifth;
				}
			}
		}
	}
	AppProperties::setContextPerMeasureVector(vecMeasures);
	AppProperties::setContextPerMeasureAndQuarterVector(vecMeasuresQuarters);

	// Generate melody using basicRhythm
	NoteEventDesc rhythmEventDesc(0, 1, 2);

	vector<int> basicRhythm{ 1, -1, -1, -1, 1, -1, -1, -1, 1, -1, 1, -1, -1, -1, 1, -1, -1, -1, 1, -1, -1, -1, 1, -1, -1, -1, -1, -1, -1, -1, 1, -1, 1, -1, -1, -1, 1, -1, -1, -1, 1, -1, 1, -1, -1, -1, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
	//int musicalThoughtLengthInMeasures = (int)ceil((float)basicRhythm.size() / (float)numTimeUnitsPerMeasure);
	//int currentMusicalThoughtIndex = 0;

	for (int i = 0; i < numTimeUnitsPerMeasure * numMeasures; i++)
	{
		int currentMeasure = (int)(i / numTimeUnitsPerMeasure);
		int currentQuarter = (int)(i % (int)ceil(numQuartersPerMeasure));
		if (basicRhythm[i % (int)(basicRhythm.size())] != -1)
		{
			int nextNote = 12 + musicMath.getNoteNumberByRoleNumber(vecMeasuresQuarters[currentMeasure][currentQuarter][0].RootMIDINote, vecMeasuresQuarters[currentMeasure][currentQuarter][0].Mode, musicMath.getRandomRoleIndex(false));
			if (musicMath.isNoteInRange(nextNote))
			{
				NoteEventDesc newEvent(nextNote, 2, 1);
				noteEventMatrix[newEvent.NoteNumber - musicMath.getNoteRangeStart()][i] = newEvent;
			}
			
		}

		/*if (i / numTimeUnitsPerMeasure < numMeasures)
		{

		}*/
	}

	repaintMatrixImage();
	setComponentSize();
	repaint();
}

void MIDITimelineContextComponent::paint(Graphics& g)
{
	String tempoInfo = " Tempo: " + String(AppProperties::getTempo());
	String measuresInfo = " Measures: " + String(this->numMeasures);
	this->name = this->projectName + " - ";

	if (this->midiFile)
	{
		String midiInfo = "Midi timeformat: " + String(defaultTicksPerQuarterNote) + " Last timestamp: " + String(midiFile->getLastTimestamp()) + " Tracks: " + String(midiFile->getNumTracks());
		this->name += midiInfo;
	}

	double beatDuration = 60.0 / (double)AppProperties::getTempo();
	double timeUnitDuration = beatDuration / 4.0f;
	double totalDurationSec = (timeUnitDuration * (double)numMeasures * (double)numTimeUnitsPerMeasure);
	double currentTimeSec = currentTimeUnit * timeUnitDuration;

	this->name += tempoInfo + measuresInfo + " Time: " + MainComponent::displayProgress(currentTimeSec, totalDurationSec);

	drawOutline(g);

	Rectangle<float> parentBounds = getReducedLocalBounds().toFloat();
	g.setColour(Colour::fromRGB(35, 35, 35));

	vector<vector<vector<ContextDesc>>> contextPerMeasureAndQuarterVector = AppProperties::getContextPerMeasureAndQuarterVector();
	vector<vector<ContextDesc>> contextPerMeasureVector = AppProperties::getContextPerMeasureVector();

	if (contextPerMeasureAndQuarterVector.size() > 0 && contextPerMeasureVector.size() > 0)
	{
		float measureWidthInPixels = parentBounds.getWidth() / (float)numMeasures;
		float timeUnitWidthInPixels = parentBounds.getWidth() / ((float)numMeasures * (float)numTimeUnitsPerMeasure);
		int numberOfTimeUnits = (int)noteEventMatrix[0].size();
		g.drawImage(matrixImage, parentBounds);
		float cursorWidth = parentBounds.getWidth() / numberOfTimeUnits;
		float currentX = ((float)currentTimeUnit / (float)numberOfTimeUnits) * parentBounds.getWidth();
		Rectangle<float> cursorTimeUnitInfoRect(currentX + cursorWidth, parentBounds.getTopRight().y, 160.0f, 60.0f);
		g.setColour(Colour::fromRGBA(128, 128, 128, 128));
		g.fillRect(currentX, parentBounds.getTopLeft().y, cursorWidth, parentBounds.getHeight());
		g.fillRect(cursorTimeUnitInfoRect);
		int currentMeasureIndex = (int)floor((float)numMeasures * ((float)currentTimeUnit / (float)numberOfTimeUnits));
		int currentTimeUnitWithinMeasureIndex = (currentMeasureIndex * numTimeUnitsPerMeasure + currentTimeUnit) % (numTimeUnitsPerMeasure);
		g.setColour(Colours::white);
		g.setFont(12.0f);
		String currentTimeUnitFormattedText = "";
		String currentContextText = "";
		String nextContextText = "";

		currentTimeUnitFormattedText = String::formatted("%02d", currentMeasureIndex + 1) + "|" + String::formatted("%02d", currentTimeUnitWithinMeasureIndex + 1);
		g.drawText(currentTimeUnitFormattedText, cursorTimeUnitInfoRect, Justification::top, true);

		if (contextPerMeasureAndQuarterVector[currentMeasureIndex].size() > 0 && contextPerMeasureAndQuarterVector[currentMeasureIndex].size() <= (int)ceil(numQuartersPerMeasure))
		{
			currentContextText = contextPerMeasureVector[currentMeasureIndex][0].friendlyName();
			if (currentMeasureIndex == numMeasures - 1)
			{
				nextContextText = contextPerMeasureVector[0][0].friendlyName();
			}
			else
			{
				nextContextText = contextPerMeasureVector[currentMeasureIndex + 1][0].friendlyName();
			}
			nextContextText = "..." + nextContextText;
			g.setFont(30.0f);
			cursorTimeUnitInfoRect.translate(0, 12.0f);
			cursorTimeUnitInfoRect.setHeight(cursorTimeUnitInfoRect.getHeight() - 24.0f);
			g.drawText(currentContextText, cursorTimeUnitInfoRect, Justification::topLeft, true);
			g.setFont(17.0f);
			g.drawText(nextContextText, cursorTimeUnitInfoRect, Justification::bottomRight, true);
		}

		//Draw Contexts probabilities per measure and quarters
		if (measureWidthInPixels > 80)
		{
			g.setFont(10.0f);
			Rectangle<int> relativeScreenRect = getScreenBounds();

			for (int currMeasure = 0; currMeasure < contextPerMeasureAndQuarterVector.size(); currMeasure++)
			{
				if (contextPerMeasureAndQuarterVector[currMeasure].size() > 0)
				{
					Rectangle<float> measureQuarterTonalityRect(measureWidthInPixels, contextPerMeasureAndQuarterVector[currMeasure][0].size() * 10.0f);
					Rectangle<float> measureTonalityRect(measureWidthInPixels, contextPerMeasureVector[currMeasure].size() * 10.0f);
					int xPosition = currMeasure * (int)measureWidthInPixels + (int)timeUnitWidthInPixels;

					int screenWidth = getParentComponent()->getWidth();

					int currentScreen = (int)(xPosition / screenWidth);
					int currentOffsetWithinScreen = xPosition % screenWidth;

					if (relativeScreenRect.getTopLeft().x + (currentScreen * screenWidth) + currentOffsetWithinScreen > 0 && relativeScreenRect.getTopLeft().x + (currentScreen * screenWidth) + currentOffsetWithinScreen < screenWidth)
					{
						for (float q = 0; q < ceil(numQuartersPerMeasure); q += 1.0f)
						{
							measureQuarterTonalityRect.setPosition(((float)currMeasure + q / numQuartersPerMeasure) * measureWidthInPixels + timeUnitWidthInPixels, 2 * timeUnitWidthInPixels + (float)headerHeight + 1.0f);
							measureTonalityRect.setPosition(((float)currMeasure) * measureWidthInPixels + timeUnitWidthInPixels, 2 * timeUnitWidthInPixels + (float)headerHeight + 1.0f);
							String text = "";
							for (int t = 0; t < contextPerMeasureAndQuarterVector[currMeasure][(int)q].size(); t++)
							{
								if (t == 0)
								{
									text += contextPerMeasureAndQuarterVector[currMeasure][(int)q][t].friendlyName();
								}
								else
								{
									text += "\r\n" + contextPerMeasureAndQuarterVector[currMeasure][(int)q][t].friendlyName();
								}
								g.drawMultiLineText(text, (int)measureQuarterTonalityRect.getTopLeft().x, (int)measureQuarterTonalityRect.getTopLeft().y, (int)(measureWidthInPixels / numQuartersPerMeasure), Justification::left);

							}
							if (q == 0)
							{
								if (contextPerMeasureVector[currMeasure].size() > 0)
								{
									text += "\r\n*" + contextPerMeasureVector[currMeasure][0].friendlyName() + "*";
								}
							}
							//g.drawMultiLineText(text, (int)measureTonalityRect.getTopLeft().x, (int)measureTonalityRect.getTopLeft().y, (int)measureWidthInPixels, Justification::left);
						}
					}
				}
			}
		}

		//Draw Selection
		if (selectedCellStart > -1)
		{
			Rectangle<float> selectionRect(selectedCellStart * cursorWidth, 0.0f, cursorWidth * (selectedCellEnd - selectedCellStart + 1), parentBounds.getHeight());
			selectionRect.translate(1.0f, (float)headerHeight + cursorWidth);
			g.setColour(Colour::fromRGBA(192, 192, 192, 128));
			g.fillRect(selectionRect);
		}

		//Draw Cursor Info Position
		if (currentCursorPosition[0] > -1 && currentCursorPosition[1] > -1)
		{
			int cMeasureIndex = (int)floor((float)numMeasures * ((float)currentCursorPosition[1] / (float)numberOfTimeUnits));
			int cTimeUnitWithinMeasureIndex = currentCursorPosition[1] % numTimeUnitsPerMeasure;
			String cTimeUnitFormattedText = String::formatted("%02d", cMeasureIndex + 1) + "|" + String::formatted("%02d", cTimeUnitWithinMeasureIndex + 1);

			float cWidth = parentBounds.getWidth() / numberOfTimeUnits;
			float cHeight = parentBounds.getHeight() / musicMath.getNoteRangeSize();
			Rectangle<float> currentScreenCursorPositionRect(cWidth, cHeight);
			currentScreenCursorPositionRect.setPosition(currentCursorPosition[1] * cWidth + 1, currentCursorPosition[0] * cHeight + headerHeight + 1);
			g.setColour(Colour::fromRGBA(192, 192, 192, 128));
			g.fillRect(currentScreenCursorPositionRect);

			float minWidth = 50.0f;
			float cursorInfoWidth = cWidth * 4;
			float cursorInfoHeight = cWidth * 3;

			float aspectRatio = cursorInfoWidth / cursorInfoHeight;
			if (cursorInfoWidth < minWidth)
			{
				cursorInfoWidth = minWidth;
				cursorInfoHeight = minWidth / aspectRatio;
			}

			Rectangle<float> currentScreenCursorInfoRect(cursorInfoWidth, cursorInfoHeight);
			currentScreenCursorInfoRect.setPosition(((currentCursorPosition[1] + 2) * cWidth) + 1, ((currentCursorPosition[0] + 2) * cHeight) + headerHeight);
			g.setColour(Colour::fromRGBA(20, 20, 20, 255));
			g.fillRect(currentScreenCursorInfoRect);
			g.setColour(Colours::white);
			g.setFont(12.0f);
			g.drawMultiLineText(String(musicMath.getNoteRangeEnd() - currentCursorPosition[0]) + " (" + musicMath.getNoteNameByMIDINoteNumber(musicMath.getNoteRangeEnd() - currentCursorPosition[0]) + ")\r\n" + cTimeUnitFormattedText, (int)currentScreenCursorInfoRect.getTopLeft().x, (int)currentScreenCursorInfoRect.getCentreY(), (int)currentScreenCursorInfoRect.getWidth(), Justification::centred);
		}
	}
}
