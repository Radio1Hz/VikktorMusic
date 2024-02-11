/*
  ==============================================================================

    FunctionProviderComplex.h
    Created: 27 Aug 2021 9:55:22pm
    Author:  viktor

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "FunctionProvider.h"
#include "ComplexNumber.h"
#include <math.h>

class FunctionProviderComplex :
	public FunctionProvider
{
public:
	FunctionProviderComplex() {};

	ComplexNumber IdFunction(ComplexNumber c)
	{
		return c;
	}

	ComplexNumber Multiply(ComplexNumber c, ComplexNumber d)
	{
		return ComplexNumber((d.x * c.x) - (d.y * c.y), (d.y * c.x + d.x * c.y));
	}
	// c^2 = (c.x + ic.y) * (c.x + ic.y) = (c.x)^2 + i*c.x*c.y + i*c.y * c.x - (c.y)^2
	ComplexNumber Square(ComplexNumber c, ComplexNumber)
	{
		return ComplexNumber(std::powf(c.x, 2) - std::powf(c.y, 2), c.x * c.y + c.y * c.x);
	}
};


