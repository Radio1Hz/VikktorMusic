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
#include "MIDITimelineComponent.h"
#include "EuclidianRythmTimerComponent.h"
#include "ComponentStateController.h"
#include "ApplicationProperties.h"

using namespace juce;
//==============================================================================

class LogSpaceComponent : public juce::Component
{
//Properties
public:

	double zoom = 1;
	juce::Point<double> currentPosition;
	CommunicationAgent* listeningSourceComponent;
	//MainComponent* mainComponent;

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
	juce::OwnedArray<MIDITimelineComponent> MIDITimelineObjects;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LogSpaceComponent)

public:

	//LogSpaceComponent(MainComponent* mainComponentPtr)
	//{
	//	//mainComponent = mainComponentPtr;
	//	rnd = Random::getSystemRandom();
	//	initMenu();
	//}

	LogSpaceComponent()
	{
		rnd = Random::getSystemRandom();
		initMenu();
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
		menu.addItem("Add new MIDI Timeline", std::bind(&LogSpaceComponent::addNewMIDITimelineComponent, this));
	}

	void LogSpaceComponent::addNewMIDITimelineComponent()
	{
		MIDITimelineComponent* t0 = new MIDITimelineComponent(32, 0);
		MIDITimelineObjects.add(t0);
		addAndMakeVisible(t0);
		t0->processMidi();
		resized();

		MIDITimelineComponent* t1 = new MIDITimelineComponent(32, 1);
		MIDITimelineObjects.add(t1);
		addAndMakeVisible(t1);
		t1->processMidi();
		resized();
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
		//aO->iWillListenToYou(timerObjects[0]);
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
		//MarkovMatrixComponent* mO = new MarkovMatrixComponent(3);
		MarkovMatrixComponent* mO = new MarkovMatrixComponent(juce::Random::getSystemRandom().nextInt(4) + 2);
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

	void LogSpaceComponent::mouseDown(const juce::MouseEvent& event) override
	{
		if (event.getNumberOfClicks() == 2)
		{

		}

		if (event.getNumberOfClicks() == 1)
		{

		}

		if (event.mods.isRightButtonDown())
		{
			menu.showMenuAsync(PopupMenu::Options(),
				[](int result)
				{
					if (result == 0)
					{
						// user dismissed the menu without picking anything
					}
					else if (result == 1)
					{
						// user picked item 1
					}
					else if (result == 2)
					{
						// user picked item 2

					}
				});
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
									g.drawArrow(Line<float>(Point<float>(a2->getX() * 1.0f, a2->getY() * 1.0f), Point<float>(b2->getX() * 1.0f, b2->getY() * 1.0f)), 1, 10, 10);
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
									g.drawArrow(Line<float>(Point<float>(a2->getX() * 1.0f, a2->getY() * 1.0f), Point<float>(b2->getX() * 1.0f, b2->getY() * 1.0f)), 1, 10, 10);
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
				if (getLocalBounds().getWidth() >= 200)
				{
					cO->setBounds(juce::Random::getSystemRandom().nextInt(getLocalBounds().getWidth() - 200), juce::Random::getSystemRandom().nextInt(getLocalBounds().getHeight() - 200), 200, 200);
				}

			}
		}
		repaint();
	}

};

struct ExampleListener : public juce::ValueTree::Listener
{
	ExampleListener(juce::ValueTree v)
		: tree(v)
	{
		tree.addListener(this);
	}

	juce::ValueTree tree;
};