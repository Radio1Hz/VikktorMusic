/*
  ==============================================================================

    MathRendererComplex.h
    Created: 27 Aug 2021 9:54:57pm
    Author:  viktor

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "MathRenderer.h"
#include "FunctionProviderComplex.h"
#include "ComplexNumber.h"

class MathRendererComplex :
    public MathRenderer
{
private:
    ComplexNumber math_center_complex;
    FunctionProviderComplex function_provider_complex = FunctionProviderComplex();
public:
    MathRendererComplex(juce::Rectangle<int> area, juce::Point<float> math_center_point, float math_max_size) : MathRenderer(area, math_center_point, math_max_size) {
        this->math_center_complex = ComplexNumber(math_center.x, math_center.y);
    }

    void Draw(Graphics& g)
    {
        g.fillAll(juce::Colours::darkgrey);
        g.setColour(juce::Colours::lightgrey);
        area = g.getClipBounds();
        area_width = (float)area.getWidth();
        area_height = (float)area.getHeight();
        screen_math_ratio = std::min(area_width, area_height) / math_max_size;
        math_width = area_width / screen_math_ratio;
        math_height = area_height / screen_math_ratio;

        screen_center = juce::Point<float>(area_width / 2, area_height / 2);
        x0 = juce::Point<float>(screen_center.x - math_center.x * screen_math_ratio, screen_center.y + math_center.y * screen_math_ratio);

        DoYourThing(g);

        math_pointer = ScreenToMathPoint(screen_pointer);
        DrawPointWithLabel(g, math_pointer);
    }

    void DoYourThing(Graphics& g)
    {
        DrawCoordinateSys(g);
        DrawPointWithLabel(g, math_center);

        //math_width = std::min(screen_width, screen_height) / math_max_size;
        ComplexNumber current_number = ComplexNumber(math_center.x, math_center.y);
        ComplexNumber current_result;
        ComplexNumber constant = ComplexNumber(1, 0);

        float delta = 0.2f;

        juce::Point<float> path_point = juce::Point<float>(-1000, -1000);
        for (float i = -math_width / 2; i < math_width / 2; i += delta)
        {
            for (float j = -math_height / 2; j < math_height / 2; j += delta)
            {
                current_result = function_provider_complex.Square(ComplexNumber(i, j), constant);
                DrawPoint(g, ComplexToMathPoint(current_result));
                DrawSegment(g, ComplexToMathPoint(current_result), path_point, juce::Colours::pink);
            }
            path_point = juce::Point<float>(-1000, -1000);
        }
    }

    juce::Point<float> ComplexToMathPoint(ComplexNumber c)
    {
        return juce::Point<float>(c.getX(), c.getY());
    }
    void DrawCoordinateSys(Graphics& g)
    {
        g.setColour(juce::Colours::lightpink);
        g.drawHorizontalLine((int)x0.y, area.getX(), area_width);
        g.drawVerticalLine((int)x0.x, area.getY(), area_height);

        juce::Point<float> min_math_point = juce::Point<float>(ScreenToMathPoint(juce::Point<float>(0, 0)));
        juce::Point<float> max_math_point = juce::Point<float>(ScreenToMathPoint(juce::Point<float>(area_width, area_height)));

        /*float min_grid_resolution = 200;
        float max_dimension = std::max(screen_width, screen_height);

        float count_lines = max_dimension / min_grid_resolution;
        float multiplier = 1;
        while (math_max_size * multiplier < count_lines)
        {
            multiplier *= 10;
        }
        count_lines *= multiplier;

        for (int i = 1; i < round(count_lines/2); i++)
        {
            DrawMathematicalLine(g, juce::Point<float>(i / multiplier,min_math_point.y), juce::Point<float>(i / multiplier, max_math_point.y), juce::Colours::lightpink, true);
            DrawMathematicalLine(g, juce::Point<float>(-i / multiplier, min_math_point.y), juce::Point<float>(-i / multiplier, max_math_point.y), juce::Colours::lightpink, true);
        }*/

    }
};

