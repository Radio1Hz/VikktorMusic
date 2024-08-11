/*
  ==============================================================================

    ApplicationProperties.h
    Created: 11 Aug 2024 12:32:46pm
    Author:  viktor

  ==============================================================================
*/

#pragma once
class AppProperties
{
public:
    
    static float getTempo();
    static void setTempo(float tmp);

    static AppProperties& getInstance()
    {
        static AppProperties instance;
        return instance;
    }

private:
    AppProperties();
    inline static float tempo;
};