/*
  ==============================================================================

    ApplicationProperties.cpp
    Created: 11 Aug 2024 12:32:46pm
    Author:  viktor

  ==============================================================================
*/

#include "ApplicationProperties.h"

AppProperties::AppProperties()
{
   
}
float AppProperties::getTempo()
{
    return AppProperties::tempo;
}

void AppProperties::setTempo(float tmp)
{
    AppProperties::tempo = tmp;
}
