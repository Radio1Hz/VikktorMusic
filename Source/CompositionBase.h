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
	virtual void generateContexts(int /*numMeasures*/, vector<vector<NoteEventDesc>>& /*noteEventMatrix*/, float /*numQuartersPerMeasure*/, int /*numTimeUnitsPerMeasure*/, int /*noteRangeStart*/, int /*noteRangeEnd*/) {};
protected:
	int middleC = 48;
};