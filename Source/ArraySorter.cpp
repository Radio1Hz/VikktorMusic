/*
  ==============================================================================

    ArraySorter.cpp
    Created: 22 Aug 2021 3:13:22pm
    Author:  viktor

  ==============================================================================
*/

#include "ArraySorter.h"


IndexValue::IndexValue()
{
    this->index = 0;
    this->value = 0;
}

IndexValue::IndexValue(int index, double value)
{
    this->index = index;
    this->value = value;
}