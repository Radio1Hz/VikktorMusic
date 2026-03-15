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
class BaseComponent : public AudioAppComponent
{
public:
    BaseComponent();
    BaseComponent(int minSize);
    ~BaseComponent() override;
    void mouseWheelMove(const MouseEvent& event, const MouseWheelDetails& wheel) override;
    void paint (Graphics&) override;
    void resized() override;
    virtual void zoomEvent(const MouseEvent& event, const MouseWheelDetails& wheel);
    virtual void controlClickEvent(const MouseEvent& event);
    virtual void controlDragEvent(const MouseEvent& event);
    virtual void shiftDragEvent(const MouseEvent& event);
    virtual void shiftMouseUpEvent(const MouseEvent& event);
    virtual void controlDoubleClickEvent(const MouseEvent& event);
    virtual void controlMouseDownEvent(const MouseEvent& event);
    virtual void controlMouseUpEvent(const MouseEvent& event);
    virtual void shiftMouseDownEvent(const MouseEvent& event);
    virtual void mouseDoubleClickEvent(const MouseEvent& event);
    virtual void mouseMoveEvent(const MouseEvent& event);
    virtual void mouseDownEvent(const MouseEvent& event);
    virtual void mouseUpEvent(const MouseEvent& event);

    void drawOutline (Graphics&);
    void mouseDrag(const MouseEvent& event) override;
    void mouseUp(const MouseEvent& event) override;
    void mouseDown(const MouseEvent& event) override;
    void mouseMove(const MouseEvent& event) override;

    Rectangle<int> getReducedLocalBounds();
    Rectangle<int> getReducedBounds();
    
    int component_state = 0;
    int minSize = 50;
    int maxSize = 160000;
    String name = "Unknown";
    int headerHeight = 20;
    bool embeddedMode = false;
    bool readOnlyMode = false;
    bool isMinSize = false;
    bool showInFullscreen = false;

protected: 
    float getFontSize();
    ComponentDragger myDragger;
    Colour outlineColour = Colours::darkred;
    int defaultSize = 400;
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill) override;
    PopupMenu menu;
private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BaseComponent)
};
