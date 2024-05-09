/*
  ==============================================================================

    BaseAudioAppComponent.cpp
    Created: 28 Aug 2021 2:23:23pm
    Author:  viktor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "BaseAudioAppComponent.h"

using namespace juce;

//==============================================================================
BaseAudioAppComponent::BaseAudioAppComponent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
}

BaseAudioAppComponent::~BaseAudioAppComponent()
{

}

void BaseAudioAppComponent::paint(Graphics& g)
{
    drawOutline(g);
}


