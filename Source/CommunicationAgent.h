/*
  ==============================================================================

    CommunicationAgent.h
    Created: 19 Aug 2021 9:05:48am
    Author:  viktor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
using namespace juce;
//==============================================================================
/*
*/
class CommunicationAgent  : public ChangeListener, public ChangeBroadcaster
{
public:
    CommunicationAgent();
    ~CommunicationAgent() override;
    void subscribeToMe(CommunicationAgent* agent);
    void iWillListenToYou(CommunicationAgent* agent);
    void weCommunicate(CommunicationAgent* agent);
    void messageReceived(CommunicationAgent* agent);
    int data;
    int id;
    bool doIListenToYou(CommunicationAgent* agent);
    bool doYouListenToMe(CommunicationAgent* agent);

protected:
    Array<CommunicationAgent*> theyListenToMe;
    Array<CommunicationAgent*> iListenToThem;
    

private:
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CommunicationAgent)
};
