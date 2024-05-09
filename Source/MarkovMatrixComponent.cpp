/*
  ==============================================================================

	MarkovMatrix.cpp
	Created: 14 Aug 2021 7:20:36pm
	Author:  viktor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MarkovMatrixComponent.h"
#include "ProbabilitySlider.h"
#include "TimerComponent.h"
#include "ArraySorter.h"

using namespace juce;

MarkovMatrixComponent::MarkovMatrixComponent(int size)
{
	this->size = size;
	this->name = "Markov Matrix " + String(size);
	matrix = std::make_unique<juce::dsp::Matrix<float>>(size, size);
	resetMatrix();
	createChildren();
}

void MarkovMatrixComponent::resetMatrix()
{
	for (int i = 0; i < size; i++)
	{
		Array<float> arr = getProbabilitiesArray(false);
		for (int j = 0; j < size; j++)
		{
			matrix->operator()(i, j) = arr[j];
		}
	}
}

void MarkovMatrixComponent::nextStep()
{
	for (int i = 0; i < size * size; i++)
	{
		auto* comp = getChildComponent(i);
		((ProbabilitySlider*)comp)->isHighlighted = false;
	}
	double nextDouble = Random::getSystemRandom().nextDouble();
	double sum = 0;
	for (int i = 0; i < size; i++)
	{
		sum += matrix->operator()(current_state, i);
		if (nextDouble <= sum)
		{
			current_state = i;
			break;
		}
	}
	if (current_state == 0)
	{
		sendChangeMessage();
	}
	repaint();
}

void MarkovMatrixComponent::changeListenerCallback(juce::ChangeBroadcaster* source)
{
	if (TimerComponent* d = dynamic_cast<TimerComponent*>(source))
	{
		nextStep();
	}

	if (MarkovMatrixComponent* d = dynamic_cast<MarkovMatrixComponent*>(source))
	{
		nextStep();
	}

	if (ProbabilitySlider* d = dynamic_cast<ProbabilitySlider*>(source))
	{
		rearrangeMatrix(d, 1);
	}
	repaint();
}

void MarkovMatrixComponent::rearrangeMatrix(ProbabilitySlider* d, int /*algorythmId*/)
{
	float currentVal = matrix->operator()(d->row, d->col);
	float newValue = d->getValue();
	float difference = newValue - currentVal;
	matrix->operator()(d->row, d->col) = newValue;

	if (difference != 0)
	{
		for (int i = 0; i < size; i++)
		{
			if (difference == 0)
			{
				break;
			}

			if (i != d->col)
			{
				if ((matrix->operator()(d->row, i) - difference) < 0)
				{
					difference = difference - (matrix->operator()(d->row, i));
					matrix->operator()(d->row, i) = 0;
				}
				else if ((matrix->operator()(d->row, i) - difference) > 1)
				{
					difference = difference - (1 - (matrix->operator()(d->row, i)));
					matrix->operator()(d->row, i) = 1;
				}
				else
				{
					matrix->operator()(d->row, i) = (matrix->operator()(d->row, i) - difference);
					difference = 0;
				}
				sliders[d->row * size + i]->setValue(matrix->operator()(d->row, i));
			}

		}
	}

	repaint();
}


juce::Array<float> MarkovMatrixComponent::getProbabilitiesArray(bool test = false)
{
	juce::Array<float> arr;
	juce::Array<float> ret;
	float sum = 0;

	arr.clear();

	for (int i = 0; i < size - 1; i++)
	{
		arr.add(juce::Random::getSystemRandom().nextFloat());
	}

	arr.sort();
	ret.clear();


	if (test)
	{
		for (int i = 0; i < size - 1; i++)
		{
			ret.add(i == 0 ? 1.0f : 0.0f);
		}
	}
	else
	{
		for (int i = 0; i < size - 1; i++)
		{
			ret.add(arr[i] - sum);
			sum = arr[i];
		}

		ret.add(1 - arr[size - 2]);
	}

	return ret;
}

MarkovMatrixComponent::~MarkovMatrixComponent()
{
	removeMouseListener(this);
	removeAllChildren();
	deleteAllChildren();
}

void MarkovMatrixComponent::paint(juce::Graphics& g)
{
	drawOutline(g);
	Rectangle<int> localRect = getReducedLocalBounds();

	g.setColour(Colours::black);
	g.fillRect(localRect);
	markCurrentState();
	float fontSize = 15;
	Rectangle<int> sumRect(headerHeight, headerHeight, localRect.getHeight() / size, (int)fontSize);
	g.setColour(Colours::white);
	g.setFont(headerHeight * 0.7f);
}

void MarkovMatrixComponent::markCurrentState()
{
	for (int i = 0; i < size; i++)
	{
		auto* comp = getChildComponent(current_state * size + i);
		((ProbabilitySlider*)comp)->isHighlighted = true;
	}
}

void MarkovMatrixComponent::resized()
{
	int sliders_width = getReducedLocalBounds().getWidth();
	int sliders_height = getReducedLocalBounds().getHeight();

	for (auto* sO : sliders)
	{
		sO->setBounds(juce::Rectangle<int>(getReducedLocalBounds().getX() + sO->col * sliders_width / size, getReducedLocalBounds().getY() + sO->row * sliders_height/ size, sliders_width / size, sliders_height/ size));
	}
}

void MarkovMatrixComponent::createChildren()
{
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			ProbabilitySlider* sO = new ProbabilitySlider(matrix->operator()(i, j), i, j);
			sliders.add(sO);
			sO->addChangeListener(this);
			sO->addMouseListener(this, true);
			addAndMakeVisible(sO);
		}
	}
}