/*
  ==============================================================================

	MIDITimelineRhythmComponent.h
	Created: 23 Aug 2024 5:51:21pm
	Author:  viktor

  ==============================================================================
*/
#include <JuceHeader.h>
#include "MIDITimelineComponent.h"
#pragma once
using namespace juce;
using namespace std;

class MIDITimelineRhythmComponent : public MIDITimelineComponent
{
public:
	MIDITimelineRhythmComponent();
	MIDITimelineRhythmComponent(int numMeasures);
	~MIDITimelineRhythmComponent();

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDITimelineRhythmComponent)
};