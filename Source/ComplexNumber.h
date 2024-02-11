/*
  ==============================================================================

    ComplexNumber.h
    Created: 27 Aug 2021 9:55:36pm
    Author:  viktor

  ==============================================================================
*/
#include <JuceHeader.h>
#pragma once
using namespace juce;

class ComplexNumber
{

public:
	float x = 0, y = 0;
	ComplexNumber() {};
	ComplexNumber(float x, float y) {
		this->x = x;
		this->y = y;
	}

	ComplexNumber(Point<float> c)
	{
		this->x = c.x;
		this->y = c.y;
	}

	float getX()
	{
		return this->x;
	}

	float getY()
	{
		return this->y;
	}

	void setX(float value)
	{
		this->x = value;
	}

	void setY(float value)
	{
		this->y = value;
	}

	ComplexNumber Add(ComplexNumber c)
	{
		return ComplexNumber(this->x + c.x, this->y + c.y);
	}
};