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
		noteEventMatrix[62 - musicMath.getNoteRangeStart()][(i * numTimeUnitsPerMeasure + 8)] = snare;
		NoteEventDesc hihat(64, 4, 1);
		noteEventMatrix[64 - musicMath.getNoteRangeStart()][(i * numTimeUnitsPerMeasure + 4)] = hihat;
		noteEventMatrix[64 - musicMath.getNoteRangeStart()][(i * numTimeUnitsPerMeasure + 12)] = hihat;
	}
	repaintMatrixImage();
	setComponentSize();
	repaint();
}

