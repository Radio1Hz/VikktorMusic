/*
  ==============================================================================

	MIDITimelineRhythmComponent.h
	Created: 25 Aug 2024 5:51:21pm
	Author:  viktor

  ==============================================================================
*/
#include <JuceHeader.h>
#include "MIDITimelineComponent.h"

#pragma once
using namespace juce;
using namespace std;

class MIDITimelineContextComponent : public MIDITimelineComponent
{
public:
	MIDITimelineContextComponent(int nM);
	~MIDITimelineContextComponent();
	void MIDITimelineContextComponent::generateRhythm() override;
	void MIDITimelineContextComponent::generateContexts() override;
	void paint(Graphics&) override;

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDITimelineContextComponent)
};