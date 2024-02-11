/*
  ==============================================================================

    DebugComponent.h
    Created: 2 Aug 2021 7:18:37am
    Author:  viktor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/
class DebugComponent  : public juce::Component
{
public:
    DebugComponent();
    ~DebugComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DebugComponent)
};
