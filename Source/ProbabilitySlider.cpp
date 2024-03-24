/*
  ==============================================================================

    ProbabilitySlider.cpp
    Created: 14 Aug 2021 9:45:43pm
    Author:  viktor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "ProbabilitySlider.h"

//==============================================================================



ProbabilitySlider::ProbabilitySlider(float value, int row, int col)
{
    this->row = row;
    this->col = col;
    slider.setValue(value);
    slider.setRange(0, 1.0);
    slider.addListener(this);
    slider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    slider.setSliderStyle(juce::Slider::SliderStyle::LinearBarVertical);
    slider.setColour(Slider::ColourIds::trackColourId, Colours::white);
    slider.setBoundsRelative(1.0f, 1.0f, 1.0f, 0.5f);
    this->value = value;
    addAndMakeVisible(slider);
    slider.setScrollWheelEnabled(true);
    addMouseListener(this, true);
}

ProbabilitySlider::~ProbabilitySlider()
{
    removeAllChildren();
    deleteAllChildren();
}

void ProbabilitySlider::sliderValueChanged(juce::Slider* sourceSlider)
{
    value = (float)sourceSlider->getValue();
    sendChangeMessage();
    this->slider.repaint();
}

void ProbabilitySlider::mouseDoubleClick(const juce::MouseEvent& /*event*/)
{

}

void ProbabilitySlider::mouseDrag(const juce::MouseEvent& /*event*/)
{
   
}

void ProbabilitySlider::mouseUp(const juce::MouseEvent& /*event*/)
{

}

void ProbabilitySlider::mouseDown(const juce::MouseEvent& /*event*/)
{
    
}

void ProbabilitySlider::mouseWheelMove(const juce::MouseEvent& /*event*/, const juce::MouseWheelDetails& /*wheel*/)
{
    
}

void ProbabilitySlider::mouseEnter(const juce::MouseEvent& /*event*/)
{
    mouseOver = true;
    repaint();
}

void ProbabilitySlider::mouseExit(const juce::MouseEvent& /*event*/)
{
    mouseOver = false;
    repaint();
}

float ProbabilitySlider::getValue()
{
    return this->value;
}

void ProbabilitySlider::setValue(float val)
{
    this->value = val;
    slider.setValue(val);
}

void ProbabilitySlider::paint (juce::Graphics& g)
{
    if (mouseOver==true)
    {
        g.fillAll(juce::Colours::darkgreen);   
    }
    else
    {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));  
    }
    
    if (isHighlighted==true)
    {
        g.fillAll(juce::Colours::rebeccapurple);   // clear the background
    }

    juce::Rectangle<int> local_bounds = getLocalBounds();
    
    float line_width = local_bounds.getHeight() * 0.05f;
    g.setColour (juce::Colours::orange);
    g.drawLine(local_bounds.toFloat().getBottomLeft().getX(), local_bounds.toFloat().getBottomLeft().getY(), local_bounds.toFloat().getBottomRight().getX(), local_bounds.toFloat().getBottomRight().getY(), line_width);   // draw line on bottom
    g.setColour(juce::Colours::white);
    
    juce::Rectangle<int> r = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay()->userArea;
    juce::Point<int> center_point = getLocalBounds().getCentre();
    center_point.addXY(-slider.getWidth()/2, 0);
    
    float font_size = 120.0f * local_bounds.getWidth() / r.getWidth();
    float max_radius = local_bounds.getWidth() * 0.75f;
    
    g.setFont(font_size);
    
    g.fillEllipse((float)center_point.getX() - max_radius * value/2, (float)center_point.getY() - max_radius * value/2, max_radius * value, max_radius * value);   // draw some placeholder text
    g.drawText("[" + juce::String(row) + "," + juce::String(col) + "]", getLocalBounds().getX() + (int)line_width/2, getLocalBounds().getY() + getLocalBounds().getHeight() - (int)font_size - (int)line_width/2, (int)max_radius/2, (int)font_size, juce::Justification::topLeft, true);
    g.drawText(juce::String(this->value), getLocalBounds().getX() + getLocalBounds().getWidth() / 2, getLocalBounds().getY() + getLocalBounds().getHeight() - (int)font_size - (int)line_width/2, (int)max_radius/2, (int)font_size, juce::Justification::topLeft, true);
}

void ProbabilitySlider::resized()
{
    juce::Rectangle<int> rect = getLocalBounds();
    int sliderWidth = rect.getWidth() / 10;
    rect.setWidth(sliderWidth);
    rect.setHeight(rect.getHeight()*97/100);
    rect.translate(getLocalBounds().getWidth() - rect.getWidth(), 0);
    slider.setBounds(rect);
}
