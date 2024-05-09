/*
  ==============================================================================

    BaseAudioAppComponent.h
    Created: 28 Aug 2021 2:23:23pm
    Author:  viktor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "BaseComponent.h"
using namespace juce;

//==============================================================================

class BaseAudioAppComponent  : public BaseComponent
{
public:
    BaseAudioAppComponent();
    ~BaseAudioAppComponent() override;

    void paint(Graphics&) override;
    juce::PopupMenu menu;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BaseAudioAppComponent)
};
