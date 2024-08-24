/*
  ==============================================================================

	MIDITimelineRhythmComponent.cpp
	Created: 23 Aug 2024 5:51:21pm
	Author:  viktor

  ==============================================================================
*/
#include <JuceHeader.h>
#include "MIDITimelineRhythmComponent.h"

MIDITimelineRhythmComponent::MIDITimelineRhythmComponent()
{

}

MIDITimelineRhythmComponent::MIDITimelineRhythmComponent(int numMeasures) : MIDITimelineComponent(numMeasures)
{
	this->synthID = 1;
	outlineColour = Colours::darkblue;
}

MIDITimelineRhythmComponent::~MIDITimelineRhythmComponent()
{
}

