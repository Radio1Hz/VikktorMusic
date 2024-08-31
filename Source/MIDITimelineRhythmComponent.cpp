/*
  ==============================================================================

	MIDITimelineRhythmComponent.cpp
	Created: 23 Aug 2024 5:51:21pm
	Author:  viktor

  ==============================================================================
*/
#include <JuceHeader.h>
#include "MIDITimelineRhythmComponent.h"

MIDITimelineRhythmComponent::MIDITimelineRhythmComponent(int numMeasures) : MIDITimelineComponent(numMeasures)
{
	this->synthID = 1;
	outlineColour = Colours::darkblue;
}

MIDITimelineRhythmComponent::~MIDITimelineRhythmComponent()
{
}

void MIDITimelineRhythmComponent::generateRhythm()
{
	for (int i = 0; i < numMeasures; i++)
	{
		//60 - Kick 62 - Snare 64 HiHat
		NoteEventDesc kick(60, 4, 1);
		noteEventMatrix[60 - musicMath.getNoteRangeStart()][i * numTimeUnitsPerMeasure] = kick;
		NoteEventDesc snare(62, 4, 1);
		noteEventMatrix[62 - musicMath.getNoteRangeStart()][(i * numTimeUnitsPerMeasure + 2 * (int)((float)numTimeUnitsPerMeasure / numQuartersPerMeasure))] = snare;
		NoteEventDesc hihat(64, 4, 1);
		noteEventMatrix[64 - musicMath.getNoteRangeStart()][(i * numTimeUnitsPerMeasure + (int)((float)numTimeUnitsPerMeasure / numQuartersPerMeasure))] = hihat;
		
		if (numQuartersPerMeasure > 3.0f)
		{
			if ((i * numTimeUnitsPerMeasure + 3 * (int)((float)numTimeUnitsPerMeasure / numQuartersPerMeasure)) < numTimeUnitsPerMeasure * numMeasures)
			{
				noteEventMatrix[64 - musicMath.getNoteRangeStart()][(i * numTimeUnitsPerMeasure + 3 * (int)((float)numTimeUnitsPerMeasure / numQuartersPerMeasure))] = hihat;
			}
		}
	}

	repaintMatrixImage();
	setComponentSize();
}

