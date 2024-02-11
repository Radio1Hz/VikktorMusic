/*
  ==============================================================================

    MarkovMatrix.cpp
    Created: 15 Oct 2021 1:14:16pm
    Author:  viktor

  ==============================================================================
*/

#include "MarkovMatrix.h"

MarkovMatrix::MarkovMatrix()
{
    
}

MarkovMatrix::MarkovMatrix(int size)
{
    matrix = new dsp::Matrix<double>(size, size);
}

MarkovMatrix::~MarkovMatrix()
{
    delete matrix;
}

void MarkovMatrix::addSymbol(String s)
{
    this->symbols.add(s);
}