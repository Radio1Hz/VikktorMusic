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
#include "MIDITimelineRhythmComponent.h"
#include "MIDITimelineContextComponent.h"
#include "MIDITimelineMarkovComponent.h"
#include "EuclidianRythmTimerComponent.h"
#include "ComponentStateController.h"
#include "ApplicationProperties.h"

using namespace juce;
//==============================================================================

class LogSpaceComponent : public Component
{

public:

	double zoom = 1;
	Point<double> currentPosition;
	CommunicationAgent* listeningSourceComponent;

private:
	bool isResized = false;
	int dragX = 0;
	int dragY = 0;

	PopupMenu menu;
	Random rnd;
	OwnedArray<TimeObjectComponent> timeObjects;
	OwnedArray<MarkovMatrixComponent> markovObjects;
	OwnedArray<TimerComponent> timerObjects;
	OwnedArray<PlotComponentLogistic> plotObjects;
	OwnedArray<AudioComponent> audioObjects;
	OwnedArray<MIDISynthComponent> midiObjects;
	OwnedArray<EuclidianRythmTimerComponent> euclidTimerObjects;
	OwnedArray<ComponentStateController> stateControllerObjects;
	OwnedArray<MIDITimelineComponent> MIDITimelineObjects;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LogSpaceComponent)

public:

	LogSpaceComponent()
	{
		rnd = Random::getSystemRandom();
		initMenu();
		addNewPlot();
	}

	void LogSpaceComponent::initMenu()
	{
		menu.clear();
		//menu.addItem("Add new Timer", std::bind(&LogSpaceComponent::addNewTimer, this));
		menu.addItem("Add new Plot", std::bind(&LogSpaceComponent::addNewPlot, this));
		//menu.addItem("Add new Audio Component", std::bind(&LogSpaceComponent::addNewAudioComponent, this));
		//menu.addItem("Add new MIDI Synth Component", std::bind(&LogSpaceComponent::addNewMIDISynthComponent, this));
		//menu.addItem("Add new Euclidian Rhythm Component", std::bind(&LogSpaceComponent::addNewEuclidianTimer, this));
		//menu.addItem("Add new Time Object Componet", std::bind(&LogSpaceComponent::addNewTimeObjectComponent, this));
		menu.addItem("Add new Markov Matrix", std::bind(&LogSpaceComponent::addMarkovMatrix, this));
		menu.addItem("Add new MIDI Timeline", std::bind(&LogSpaceComponent::addNewMIDITimelineComponent, this));
		menu.addItem("Add new MIDI Context Timeline", std::bind(&LogSpaceComponent::addNewMIDITimelineContextComponent, this));
		menu.addItem("Add new MIDI Markov Timeline", std::bind(&LogSpaceComponent::addNewMIDITimelineMarkovComponent, this));
		menu.addItem("Add new MIDI Rhythm Timeline", std::bind(&LogSpaceComponent::addNewMIDITimelineRhythmComponent, this));
	}

	void LogSpaceComponent::addNewMIDITimelineComponent()
	{
		MIDITimelineComponent* t0 = new MIDITimelineComponent(AppProperties::getNumMeasures());
		MIDITimelineObjects.add(t0);
		addAndMakeVisible(t0);
		t0->init();
	}
	
	void LogSpaceComponent::addNewMIDITimelineRhythmComponent()
	{
		MIDITimelineRhythmComponent* t0 = new MIDITimelineRhythmComponent(AppProperties::getNumMeasures());
		MIDITimelineObjects.add(t0);
		addAndMakeVisible(t0);
		t0->init();
	}
	
	void LogSpaceComponent::addNewMIDITimelineMarkovComponent()
	{
		MIDITimelineMarkovComponent* t0 = new MIDITimelineMarkovComponent(AppProperties::getNumMeasures());
		MIDITimelineObjects.add(t0);
		addAndMakeVisible(t0);
		t0->init();
	}
	
	void LogSpaceComponent::addNewMIDITimelineContextComponent()
	{
		MIDITimelineContextComponent* t0 = new MIDITimelineContextComponent(AppProperties::getNumMeasures());
		MIDITimelineObjects.add(t0);
		addAndMakeVisible(t0);
		t0->init();
	}

	void LogSpaceComponent::deleteMIDITimelineComponent(MIDITimelineComponent* midiTC)
	{
		int index = 0;
		for (MIDITimelineComponent* tl : MIDITimelineObjects)
		{
			if (midiTC == tl)
			{
				removeChildComponent(tl);
				MIDITimelineObjects.remove(index);
				repaint();
			}
			index++;
		}
	}

	void LogSpaceComponent::addNewTimeObjectComponent()
	{
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
		//pO->iWillListenToYou(stateControllerObjects[0]);
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
		//MarkovMatrix* mO = new MarkovMatrix(Random::getSystemRandom().nextInt(24)+1);
		//MarkovMatrixComponent* mO = new MarkovMatrixComponent(3);
		MarkovMatrixComponent* mO = new MarkovMatrixComponent(Random::getSystemRandom().nextInt(4) + 2);
		//mO->iWillListenToYou(euclidTimerObjects[euclidTimerObjects.size() - 1]);
		markovObjects.add(mO);
		addAndMakeVisible(mO);
		resized();
	}

	~LogSpaceComponent() override
	{
		removeAllChildren();
		deleteAllChildren();
	}

	void LogSpaceComponent::shiftMouseUpEvent(const MouseEvent& event)
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

	void LogSpaceComponent::mouseDown(const MouseEvent& event) override
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

	void LogSpaceComponent::mouseUp(const MouseEvent& event) override
	{
		dragX = 0;
		dragY = 0;
		if (event.mods.isShiftDown())
		{
			shiftMouseUpEvent(event);
		}
	}
	void LogSpaceComponent::mouseDrag(const MouseEvent& event) override
	{
		int x = event.getDistanceFromDragStartX();
		int y = event.getDistanceFromDragStartY();

		Rectangle<int> rect;

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

	void paint(Graphics& g) override
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
					//cO->setBounds(Random::getSystemRandom().nextInt(getLocalBounds().getWidth() - 500), Random::getSystemRandom().nextInt(getLocalBounds().getHeight() - 200), 500, 200);
					//cO->setBounds(0, 0, getScreenBounds().getWidth(), getScreenBounds().getHeight());
					cO->setBounds(0, 0, 1375, 750);
				}

			}
		}
		repaint();
	}

};

struct ExampleListener : public ValueTree::Listener
{
	ExampleListener(ValueTree v)
		: tree(v)
	{
		tree.addListener(this);
	}

	ValueTree tree;
};