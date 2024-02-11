/*
  ==============================================================================

    ArraySorter.h
    Created: 22 Aug 2021 3:13:22pm
    Author:  viktor

  ==============================================================================
*/

#pragma once
class IndexValue
{
public:
    IndexValue(int index, double value);
    IndexValue();
    double value = 0;
    int index = 0;
};



class ArraySorter
{
public:

    static int ArraySorter::compareElements(IndexValue a, IndexValue b)
    {
        if (a.value < b.value)
            return -1;
        else if (a.value > b.value)
            return 1;
        else // if a == b
            return 0;
    }
};

