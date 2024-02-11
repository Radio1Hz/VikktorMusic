#include "MainComponent.h"
#include "DebugComponent.h"

//==============================================================================


MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);
    
    
    
    addAndMakeVisible(logSpaceComponent,-1);
    //addAndMakeVisible(debugComponent,-1);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    
    //removeChildComponent(&debugComponent);
    //removeChildComponent(&logSpaceComponent);
    removeAllChildren();
    deleteAllChildren();
}

bool MainComponent::keyPressed(const KeyPress& /*key*/, Component* /*originatingComponent*/)
{
    return true;
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);
    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    logSpaceComponent.setSize(getWidth(), getHeight());
    debugComponent.setBounds(0, 0, getWidth(), 30);
}
