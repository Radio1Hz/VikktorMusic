/*
  ==============================================================================

    CommunicationAgent.cpp
    Created: 19 Aug 2021 9:05:48am
    Author:  viktor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CommunicationAgent.h"

//==============================================================================
CommunicationAgent::CommunicationAgent()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    data = 1;
    id = Random::getSystemRandom().nextInt();
}

CommunicationAgent::~CommunicationAgent()
{

}
void CommunicationAgent::messageReceived(CommunicationAgent* /*agent*/) {}

void CommunicationAgent::iWillListenToYou(CommunicationAgent* agent)
{
    agent->addChangeListener(this);
    iListenToThem.add(agent);
}

void CommunicationAgent::subscribeToMe(CommunicationAgent* agent)
{
    addChangeListener(agent);
    theyListenToMe.add(agent);
}

void CommunicationAgent::weCommunicate(CommunicationAgent* agent)
{
    addChangeListener(agent);
    agent->addChangeListener(this);
    theyListenToMe.add(agent);
    iListenToThem.add(agent);
}

bool CommunicationAgent::doIListenToYou(CommunicationAgent* agent)
{
    bool ret = false;
    for (CommunicationAgent* a : theyListenToMe)
    {
        if (agent == a)
        {
            ret = true;
            break;
        }
    }
    return ret;
}

bool CommunicationAgent::doYouListenToMe(CommunicationAgent* agent)
{
    bool ret = false;
    for (CommunicationAgent* a : iListenToThem)
    {
        if (agent == a)
        {
            ret = true;
            break;
        }
    }
    return ret;
}
