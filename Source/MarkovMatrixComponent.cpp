/*
  ==============================================================================

    MarkovMatrix.cpp
    Created: 14 Aug 2021 7:20:36pm
    Author:  viktor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "LogSpaceComponent.h"
#include "MarkovMatrixComponent.h"
#include "ProbabilitySlider.h"
#include "TimerComponent.h"
#include "ArraySorter.h"

using namespace juce;

MarkovMatrixComponent::MarkovMatrixComponent(int size)
{
    this->size = size;
    this->name = "Markov Matrix " + String(size);
    matrix = new juce::dsp::Matrix<double>(size, size);
    resetMatrix();
    createChildren();
    addMouseListener(this, true);
}

void MarkovMatrixComponent::resetMatrix()
{
    for (int i = 0; i < size; i++)
    {
        Array<double> arr = getProbabilitiesArray();
        for (int j = 0; j < size; j++)
        {
            matrix->operator()(i, j) = arr[j];
        }
    }
}

void MarkovMatrixComponent::nextStep()
{
    for (int i = 0; i < size*size; i++)
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
        /*for (int j = 0; j < size; j++)
        {                
            sliders[d->row*size+j]->setValue(matrix->operator()(d->row, j));
        }*/
    }
    repaint();
}

void MarkovMatrixComponent::rearrangeMatrix(ProbabilitySlider* d, int /*algorythmId*/)
{
    double currentVal = matrix->operator()(d->row, d->col);
    double newValue = d->getValue();
    double difference = newValue - currentVal;
    matrix->operator()(d->row, d->col) = newValue;

    double diff = difference / (size - 1);

    for (int i = 0; i < size; i++)
    {
        if (i != d->col)
        {
            if ((matrix->operator()(d->row, i) - diff) < 0)
            {
                diff = diff - (matrix->operator()(d->row, i) - diff);
                matrix->operator()(d->row, i) = 0;
            }
            else if ((matrix->operator()(d->row, i) - diff) > 1)
            {
                diff = diff - (matrix->operator()(d->row, i) - diff) - 1;
                matrix->operator()(d->row, i) = 1;
            }
            else
            {
                matrix->operator()(d->row, i) = (matrix->operator()(d->row, i) - diff);
            }
            sliders[d->row * size + i]->setValue(matrix->operator()(d->row, i));
        }
    }

    repaint();
}

// Original
//void MarkovMatrixComponent::rearrangeMatrix(ProbabilitySlider* d, int /*algorythmId*/)
//{
//    double currentValue = matrix->operator()(d->row, d->col);
//    double newValue = d->getValue();
//    double totalDiff = newValue - currentValue;
//    //double restDiff = totalDiff;
//    
//    bool isNegative = signbit(totalDiff);
//
//    int nonZeroElementsNumber = 0;
//
//    for (int i = 0; i < size; i++)
//    {
//        if (matrix->operator()(d->row, i) > 0 && i!=d->col)
//        {
//            nonZeroElementsNumber++;
//        }
//    }
//
//    double localDiff = totalDiff / ((double)nonZeroElementsNumber);
//    matrix->operator()(d->row, d->col) = d->getValue();
//
//    if (isNegative)
//    {
//        for (int i = 0; i < size; i++)
//        {
//            if (i != d->col)
//            {
//                matrix->operator()(d->row, i) -= localDiff;
//                if (matrix->operator()(d->row, i) > 1)
//                {
//                    matrix->operator()(d->row, i) = 1;
//                    
//                    localDiff -= 1-matrix->operator()(d->row, i);
//                }
//                sliders[d->row * size + i]->setValue(matrix->operator()(d->row, i));
//            }
//        }
//    }
//    else
//    {
//        for (int i = 0; i < size; i++)
//        {
//            if (i != d->col)
//            {
//                matrix->operator()(d->row, i) -= localDiff;
//                if (matrix->operator()(d->row, i) < 0)
//                {
//                    matrix->operator()(d->row, i) = 0;
//                    localDiff += -matrix->operator()(d->row, i);
//                }
//                sliders[d->row * size + i]->setValue(matrix->operator()(d->row, i));
//            }
//        }
//    }
//    
//    repaint();
//}

juce::Array<double> MarkovMatrixComponent::getProbabilitiesArray()
{
    juce::Array<double> arr;
    juce::Array<double> ret;
    double sum = 0;

    arr.clear();

    for (int i = 0; i < size - 1; i++)
    {
        arr.add(juce::Random::getSystemRandom().nextDouble());
    }

    arr.sort();
    ret.clear();

    for (int i = 0; i < size - 1; i++)
    {
        ret.add(arr[i] - sum);
        sum = arr[i];
    }

    ret.add(1 - arr[size - 2]);
    return ret;
}

MarkovMatrixComponent::~MarkovMatrixComponent()
{
    removeMouseListener(this);
    delete matrix;
    removeAllChildren();
    deleteAllChildren();
}

void MarkovMatrixComponent::paint (juce::Graphics& g)
{
    drawOutline(g);
    Rectangle<int> localRect = getReducedLocalBounds();
    
    g.setColour(Colours::black);
    g.fillRect(localRect);
    markCurrentState();
    float fontSize = 15;
    Rectangle<int> sumRect(headerHeight, headerHeight, localRect.getHeight()/size, (int)fontSize);
    g.setColour(Colours::white);
    g.setFont(headerHeight * 0.7f);
    g.addTransform(AffineTransform::rotation(MathConstants<float>::halfPi, localRect.getCentreX() * 1.0f, localRect.getCentreY()*1.0f));

    for (int i = 0; i < size; i++)
    {
        double sum = 0;
        for (int j = 0; j < size; j++)
        {
            sum += matrix->operator()(i, j);
        }
        g.setColour(Colours::white);
        g.drawText(String(sum), sumRect, Justification::centredLeft);
        sumRect.translate(localRect.getHeight() / size, 0);
    }

    g.addTransform(AffineTransform::rotation(-MathConstants<float>::halfPi, localRect.getCentreX() * 1.0f, localRect.getCentreY() * 1.0f));
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
    
    int smaller_dim = juce::jmin(sliders_width, sliders_height);
    smaller_size = smaller_dim * 0.9;
    int padding = smaller_dim * 0.1;

    for (auto* sO : sliders)
    {
        sO->setBounds(juce::Rectangle<int>(padding + getReducedLocalBounds().getX() + sO->col * smaller_size / size, padding + getReducedLocalBounds().getY() + sO->row * smaller_size / size, smaller_size/size, smaller_size/size));
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
            addAndMakeVisible(sO);
        }
    }
}