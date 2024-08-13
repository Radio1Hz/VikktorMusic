/*
  ==============================================================================

    BaseComponent.h
    Created: 18 Aug 2021 9:23:07am
    Author:  viktor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
using namespace juce;
//==============================================================================
/*
*/
class BaseComponent : public juce::AudioAppComponent
{
public:
    BaseComponent();
    BaseComponent(int minSize);
    ~BaseComponent() override;
    void mouseWheelMove(const MouseEvent& event, const MouseWheelDetails& wheel) override;
    void paint (Graphics&) override;
    void resized() override;
    virtual void zoomEvent(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel);
    virtual void controlClickEvent(const juce::MouseEvent& event);
    virtual void controlDragEvent(const juce::MouseEvent& event);
    virtual void shiftDragEvent(const juce::MouseEvent& event);
    virtual void shiftMouseUpEvent(const juce::MouseEvent& event);
    virtual void controlDoubleClickEvent(const juce::MouseEvent& event);
    virtual void controlMouseDownEvent(const juce::MouseEvent& event);
    virtual void controlMouseUpEvent(const juce::MouseEvent& event);
    virtual void shiftMouseDownEvent(const juce::MouseEvent& event);
    virtual void mouseDoubleClickEvent(const juce::MouseEvent& event);
    
    void drawOutline (Graphics&);
    void mouseDrag(const MouseEvent& event) override;
    void mouseUp(const MouseEvent& event) override;
    void mouseDown(const MouseEvent& event) override;
    Rectangle<int> getReducedLocalBounds();
    Rectangle<int> getReducedBounds();
    
    int component_state = 0;
    int minSize = 50;
    int maxSize = 16000;
    String name = "Unknown";
    int headerHeight = 20;
    bool embeddedMode = false;
    bool readOnlyMode = false;

protected: 
    float getFontSize();
    ComponentDragger myDragger;
    
    int defaultSize = 200;
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    juce::PopupMenu menu;
private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BaseComponent)
};
