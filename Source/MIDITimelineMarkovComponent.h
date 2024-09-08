/*
  ==============================================================================

	MIDITimelineMarkovComponent.h
	Created: 25 Aug 2024 5:51:21pm
	Author:  viktor

  ==============================================================================
*/
#include <JuceHeader.h>
#include "MIDITimelineComponent.h"
#include "MarkovMatrixComponent.h"
#include "MarkovCompositionBase.h"

#pragma once
using namespace juce;
using namespace std;

class MIDITimelineMarkovComponent : public MIDITimelineComponent
{
public:
	MIDITimelineMarkovComponent(int nM);
	~MIDITimelineMarkovComponent();
	void generateRhythm() override;
	void generateContexts() override;
	void paint(Graphics&) override;
	void resized() override;
	unique_ptr<MarkovMatrixComponent> rhythmMatrix;

private:
	//Composition
	unique_ptr<MarkovCompositionBase> composition;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MIDITimelineMarkovComponent)
};