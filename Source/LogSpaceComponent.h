/*
  ==============================================================================

    LogSpace.h
    Created: 5 Aug 2021 11:59:24am
    Author:  viktor

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "TimeObject.h"
#include "TimerComponent.h"
#include "MarkovMatrixComponent.h"
#include "CommunicationAgent.h"
#include "PlotComponent.h"
#include "PlotComponentLogistic.h"
#include "AudioComponent.h"
#include "MIDISynthComponent.h"
#include "EuclidianRythmTimerComponent.h"
#include "ComponentStateController.h"

using namespace juce;
//==============================================================================
/*
*/
struct ExampleListener : public juce::ValueTree::Listener
{
    ExampleListener(juce::ValueTree v)
        : tree(v)
    {
        tree.addListener(this);
    }

    juce::ValueTree tree;
};

class LogSpaceComponent : public juce::Component
{
//Properties
public:
    
    double zoom = 1;
    juce::Point<double> currentPosition;
    CommunicationAgent* listeningSourceComponent = NULL;

private:
    bool isResized = false;
    int dragX = 0;
    int dragY = 0;

    juce::PopupMenu menu;
    juce::Random rnd;
    juce::OwnedArray<TimeObjectComponent> timeObjects;
    juce::OwnedArray<MarkovMatrixComponent> markovObjects;
    juce::OwnedArray<TimerComponent> timerObjects;
    juce::OwnedArray<PlotComponentLogistic> plotObjects;
    juce::OwnedArray<AudioComponent> audioObjects;
    juce::OwnedArray<MIDISynthComponent> midiObjects;
    juce::OwnedArray<EuclidianRythmTimerComponent> euclidTimerObjects;
    juce::OwnedArray<ComponentStateController> stateControllerObjects;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LogSpaceComponent)

public:

    LogSpaceComponent()
    {
        rnd = Random::getSystemRandom();
        
        // In your constructor, you should add any child components, and
        // initialise any special settings that your component needs.
        
        //timerObjects.add(new TimerComponent(16));
        //currentPosition.x = 0.0;
        //currentPosition.y = 0.0;
        //timeObjects.clear(true);
        //markovObjects.clear(true);
        //markovObjects.add(new MarkovMatrix(3));
        //
        //int temp[] = { 2,4,4 };
        //timeObjects.add(new TimeObject(0, 30, 2000, temp, sizeof(temp) / sizeof(int)));
        initMenu();
        //for (auto* tO : timeObjects)
        //{
        //    addAndMakeVisible(tO);
        //}
        //    
        //for (auto* mO : markovObjects)
        //{
        //    addAndMakeVisible(mO);
        //    timerObjects[0]->subscribeToMe(mO);
        //}

        //addAndMakeVisible(timerObjects[0]);
        ////addAndMakeVisible(timerObjects[1]);
        //timerObjects[0]->start(500);
        ////timerObjects[1]->start(250);
        

        //timerObjects.add(new TimerComponent(8, 300));
        /*for (auto* tO : timerObjects)
        {
            addAndMakeVisible(tO);
            tO->start();
        }*/

        /*stateControllerObjects.add(new ComponentStateController());
        for (auto* cO : stateControllerObjects)
        {
            addAndMakeVisible(cO);
        }*/

        //addNewPlot();

       //plotObjects.add(new PlotComponent());
       /* for (auto* pO : plotObjects)
        {
            addAndMakeVisible(pO);
            pO->iWillListenToYou(timerObjects[0]);
        }

        euclidTimerObjects.add(new EuclidianRythmTimerComponent(12, 4));
        for (auto* eO : euclidTimerObjects)
        {
            addAndMakeVisible(eO);
            eO->iWillListenToYou(timerObjects[0]);
            eO->start();
        }*/

        //midiObjects.add(new MIDISynthComponent());
      /*  for (auto* mO : midiObjects)
        {
            addAndMakeVisible(mO);
            mO->iWillListenToYou(euclidTimerObjects[0]);
        }
        
        timeObjects.add(new TimeObjectComponent());
        for (auto* tO : timeObjects)
        {
            tO->levelDimensions.add(2);
            tO->levelDimensions.add(4);
            tO->levelDimensions.add(4);
            addAndMakeVisible(tO);
            tO->iWillListenToYou(timerObjects[0]);
        }*/

    }

    void LogSpaceComponent::shiftMouseUpEvent(const juce::MouseEvent& event) 
    {
        if (this->listeningSourceComponent != NULL)
        {
            if (CommunicationAgent* bc = dynamic_cast<CommunicationAgent*>(this->listeningSourceComponent))
            {
                if (CommunicationAgent* bctarget = dynamic_cast<CommunicationAgent*>(this->getComponentAt(event.getScreenX(), event.getScreenY())))
                {
                    bctarget->iWillListenToYou(bc);
                }
                this->listeningSourceComponent = NULL;
            }
        }
    }

    void LogSpaceComponent::initMenu()
    {
        menu.clear();
        menu.addItem("Add new Timer", std::bind(&LogSpaceComponent::addNewTimer, this));
        menu.addItem("Add new Markov Matrix", std::bind(&LogSpaceComponent::addMarkovMatrix, this));
        menu.addItem("Add new Plot", std::bind(&LogSpaceComponent::addNewPlot, this));
        menu.addItem("Add new Audio Component", std::bind(&LogSpaceComponent::addNewAudioComponent, this));
        menu.addItem("Add new MIDI Synth Component", std::bind(&LogSpaceComponent::addNewMIDISynthComponent, this));
        menu.addItem("Add new Euclidian Rhythm Component", std::bind(&LogSpaceComponent::addNewEuclidianTimer, this));
        menu.addItem("Add new Time Object Componet", std::bind(&LogSpaceComponent::addNewTimeObjectComponent, this));
    }
    
    void LogSpaceComponent::addNewTimeObjectComponent()
    {
        //int size = rnd.nextInt(20) + 4;
        //int pulses = rnd.nextInt(size / 2) + 1;
        TimeObjectComponent* tO = new TimeObjectComponent();
        tO->levelDimensions.add(2);
        tO->levelDimensions.add(4);
        tO->levelDimensions.add(4);
        tO->iWillListenToYou(timerObjects[0]);
        addAndMakeVisible(tO);
        resized();
    }
    void LogSpaceComponent::addNewTimer()
    {
        TimerComponent* timer = new TimerComponent();
        addAndMakeVisible(timer);
        timerObjects.add(timer);
        timer->start();
        resized();
    }

    void LogSpaceComponent::addNewEuclidianTimer()
    {
        int size = 12;//rnd.nextInt(20) + 4;
        int pulses = 7;//rnd.nextInt(size/2) + 1;
        EuclidianRythmTimerComponent* eO = new EuclidianRythmTimerComponent(size, pulses);
        eO->iWillListenToYou(timerObjects[0]);
        addAndMakeVisible(eO);
        euclidTimerObjects.add(eO);
        resized();
    }
    void LogSpaceComponent::addNewPlot()
    {
        PlotComponentLogistic* pO = new PlotComponentLogistic();
        pO->iWillListenToYou(stateControllerObjects[0]);
        addAndMakeVisible(pO);
        plotObjects.add(pO);
        resized();
    }
    
    void LogSpaceComponent::addNewAudioComponent()
    {
        AudioComponent* aO = new AudioComponent();
        aO->iWillListenToYou(timerObjects[0]);
        addAndMakeVisible(aO);
        audioObjects.add(aO);
        resized();
    }

    void LogSpaceComponent::addNewMIDISynthComponent()
    {
        MIDISynthComponent* mO = new MIDISynthComponent();
        //mO->iWillListenToYou(euclidTimerObjects[euclidTimerObjects.size()-1]);
        addAndMakeVisible(mO);
        midiObjects.add(mO);
        resized();
    }
    
    void LogSpaceComponent::addMarkovMatrix()
    {
        //MarkovMatrix* mO = new MarkovMatrix(juce::Random::getSystemRandom().nextInt(24)+1);
        MarkovMatrixComponent* mO = new MarkovMatrixComponent(3);
        //mO->iWillListenToYou(euclidTimerObjects[euclidTimerObjects.size() - 1]);
        markovObjects.add(mO);
        addAndMakeVisible(mO);
        resized();
    }
    
    ~LogSpaceComponent() override
    {
        DBG("destructing LogSpaceComponent"); //add the breakpoint on this line
        removeAllChildren();
        deleteAllChildren();
        
       /* juce::OwnedArray<TimeObjectComponent> timeObjects;
        juce::OwnedArray<MarkovMatrixComponent> markovObjects;
        juce::OwnedArray<TimerComponent> timerObjects;
        juce::OwnedArray<PlotComponent> plotObjects;
        juce::OwnedArray<AudioComponent> audioObjects;
        juce::OwnedArray<MIDISynthComponent> midiObjects;
        juce::OwnedArray<EuclidianRythmTimerComponent> euclidTimerObjects;*/
        
    }

    void LogSpaceComponent::mouseDown(const juce::MouseEvent& event) override
    {
        if (event.getNumberOfClicks() == 2)
        {
            addMarkovMatrix();
        }

        if (event.getNumberOfClicks() == 1)
        {
            
        }

        if (event.mods.isRightButtonDown())
        {
            
        }
    }

    void LogSpaceComponent::mouseUp(const juce::MouseEvent& event) override
    {
        dragX = 0;
        dragY = 0;
        if (event.mods.isShiftDown())
        {
            shiftMouseUpEvent(event);
        }
    }
    void LogSpaceComponent::mouseDrag(const juce::MouseEvent& event) override
    {
        int x = event.getDistanceFromDragStartX();
        int y = event.getDistanceFromDragStartY();

        juce::Rectangle<int> rect;

        for (auto* tO : getChildren())
        {
            rect = tO->getBounds();
            rect.translate(-dragX, -dragY);
            rect.translate(x, y);
            tO->setBounds(rect);
        }

        dragX = x;
        dragY = y;

        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(Colours::darkgrey);
        g.setColour(Colours::darkorange);
        g.setOpacity(0.5f);

        for (auto comp : getChildren())
        {
            if (CommunicationAgent* a1 = dynamic_cast<CommunicationAgent*>(comp))
            {
                if (BaseComponent* a2 = dynamic_cast<BaseComponent*>(comp))
                {
                    for (auto* comp_comm : getChildren())
                    {
                        if (CommunicationAgent* b1 = dynamic_cast<CommunicationAgent*>(comp_comm))
                        {
                            if (b1->doIListenToYou(a1))
                            {
                                if (BaseComponent* b2 = dynamic_cast<BaseComponent*>(comp_comm))
                                {
                                    g.drawArrow(Line<float>(Point<float>(a2->getX() * 1.0f, a2->getY() * 1.0f), Point<float>(b2->getX() * 1.0f, b2->getY() * 1.0f)), 1, 10, 10);
                                }
                            }
                            if (a1->doYouListenToMe(b1))
                            {
                                if (BaseComponent* b2 = dynamic_cast<BaseComponent*>(comp_comm))
                                {
                                    g.drawArrow(Line<float>(Point<float>(a2->getX()*1.0f, a2->getY() * 1.0f), Point<float>(b2->getX() * 1.0f, b2->getY() * 1.0f)), 1, 10, 10);
                                }
                            }
                            
                        }
                    }
                }
                if (BaseAudioAppComponent* a2 = dynamic_cast<BaseAudioAppComponent*>(comp))
                {
                    for (auto comp_audio : getChildren())
                    {
                        if (CommunicationAgent* b1 = dynamic_cast<CommunicationAgent*>(comp_audio))
                        {
                            if (b1->doIListenToYou(a1))
                            {
                                if (BaseAudioAppComponent* b2 = dynamic_cast<BaseAudioAppComponent*>(comp_audio))
                                {
                                    g.drawArrow(Line<float>(Point<float>(a2->getX() * 1.0f, a2->getY() * 1.0f), Point<float>(b2->getX() * 1.0f, b2->getY() * 1.0f)), 1, 10, 10);
                                }
                            }
                            if (a1->doYouListenToMe(b1))
                            {
                                if (BaseAudioAppComponent* b2 = dynamic_cast<BaseAudioAppComponent*>(comp_audio))
                                {
                                    g.drawArrow(Line<float>(Point<float>(a2->getX()*1.0f, a2->getY() * 1.0f), Point<float>(b2->getX() * 1.0f, b2->getY() * 1.0f)), 1, 10, 10);
                                }
                            }

                        }
                    }
                }
            }
        }

        

    }

    void resized() override
    {
        for (auto* cO : getChildren())
        {
            if (cO->getWidth() <= 0)
            {
                if (getLocalBounds().getWidth() > 200)
                {
                    cO->setBounds(juce::Random::getSystemRandom().nextInt(getLocalBounds().getWidth() - 200), juce::Random::getSystemRandom().nextInt(getLocalBounds().getHeight() - 200), 200, 200);
                }
                
            }
        }
        repaint();
    }

};
