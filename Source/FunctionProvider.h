/*
  ==============================================================================

    FunctionProvider.h
    Created: 27 Aug 2021 9:55:11pm
    Author:  viktor

  ==============================================================================
*/
#include <JuceHeader.h>
#pragma once
using namespace juce;
class FunctionProvider
{
public:
	FunctionProvider() {};

	int CollatzConjecture(int input)
	{
		if (input % 2 == 0)
		{
			return input / 2;
		}
		else
		{
			return 3 * input - 1;
		}
	}

	juce::Point<float> LogisticFunction(float lambda, float x)
	{
		juce::Point<float> math_pt_f = juce::Point<float>(x, 0);

		math_pt_f.y = lambda * x * (1 - x);
		return math_pt_f;
	}

	juce::Point<float> LogisticFunctionInverse(float lambda, float y)
	{
		juce::Point<float> math_pt_f = juce::Point<float>(0, y);

		math_pt_f.x = lambda * y * (1 - y);
		return math_pt_f;
	}

	juce::Point<float> LogisticFunctionSin(float lambda, float x)
	{
		juce::Point<float> math_pt_f = juce::Point<float>(x, 0);

		math_pt_f.y = lambda * sin(juce::MathConstants<float>().pi * x)/4;
		return math_pt_f;
	}

	juce::Point<float> LogisticFunctionInverseSin(float lambda, float y)
	{
		juce::Point<float> math_pt_f = juce::Point<float>(0, y);

		math_pt_f.x = lambda * sin(juce::MathConstants<float>().pi * y)/4;
		return math_pt_f;
	}
	int common = 0;

	long LCM(juce::Array<long> numbers)
	{
		common = 0;
		long lcm = 1;
		for (int i = 0; i < numbers.size(); i++)
		{
			lcm = _lcm(lcm, numbers[0]);
		}
		
	}
	
	long _lcm(long n1, long n2)
	{
		// increase common
		common += n2;
		if (common % n1 == 0)
			return common; // base case
		else
			return _lcm(n1, n2); //general case
	}
};

