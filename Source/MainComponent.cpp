#include "MainComponent.h"
#include "DebugComponent.h"

//==============================================================================


MainComponent::MainComponent()
{
    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);
    addAndMakeVisible(logSpaceComponent, -1);
    setAudioChannels(0, 1); // no inputs, two outputs
    addAndMakeVisible(debugComponent, -1);
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
    removeAllChildren();
    deleteAllChildren();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    juce::String message;
    message << "Preparing to play audio...\n";
    message << " samplesPerBlockExpected = " << samplesPerBlockExpected << "\n";
    message << " sampleRate = " << sampleRate;
    juce::Logger::getCurrentLogger()->writeToLog(message);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    for (auto channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        // Get a pointer to the start sample in the buffer for this audio output channel
        auto* buffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);

        // Fill the required number of samples with noise between -0.125 and +0.125
        for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
            buffer[sample] = random.nextFloat() * 0.25f - 0.125f;
    }
}

void MainComponent::releaseResources()
{
    juce::Logger::getCurrentLogger()->writeToLog("Releasing audio resources");
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
