/*
  ==============================================================================

	MIDITimelineMarkovComponent.cpp
	Created: 25 Aug 2024 9:53:45am
	Author:  viktor

  ==============================================================================
*/
#include "MIDITimelineComponent.h"
#include "MIDITimelineMarkovComponent.h"
#include "MarkovMatrixComponent.h"
#include "ApplicationProperties.h"
#include "MainComponent.h"
#include "Markov.01.h"

MIDITimelineMarkovComponent::MIDITimelineMarkovComponent(int nM) : MIDITimelineComponent(nM)
{
	this->synthID = 2;
	outlineColour = Colours::yellowgreen;
}

MIDITimelineMarkovComponent::~MIDITimelineMarkovComponent()
{
}

void MIDITimelineMarkovComponent::generateRhythm()
{
	int musicalThoughtDuration = 128;
	vector<string> generatedRhythm(musicalThoughtDuration);
	composition = make_unique<Markov01>();
	composition->generateRhythm(numMeasures, noteEventMatrix, 4.0f, numTimeUnitsPerMeasure, musicMath.getNoteRangeStart(), musicMath.getNoteRangeEnd(), rhythmMatrix.get());
	repaintMatrixImage();
	repaint();
}

void MIDITimelineMarkovComponent::generateContexts()
{
	composition->generateContexts(numMeasures, noteEventMatrix, 4.0f, numTimeUnitsPerMeasure, musicMath.getNoteRangeStart(), musicMath.getNoteRangeEnd());
}

void MIDITimelineMarkovComponent::paint(Graphics& g)
{
	MIDITimelineComponent::paint(g);

	vector<vector<vector<ContextDesc>>> contextPerMeasureAndQuarterVector = AppProperties::getContextPerMeasureAndQuarterVector();
	vector<vector<ContextDesc>> contextPerMeasureVector = AppProperties::getContextPerMeasureVector();
	Rectangle<float> parentBounds = getReducedLocalBounds().toFloat();

	// Paint current context and next
	if (contextPerMeasureAndQuarterVector.size() > 0 && contextPerMeasureVector.size() > 0)
	{
		int numberOfTimeUnits = (int)noteEventMatrix[0].size();
		float cursorWidth = parentBounds.getWidth() / numberOfTimeUnits;
		float currentX = ((float)currentTimeUnit / (float)numberOfTimeUnits) * parentBounds.getWidth();
		Rectangle<float> cursorTimeUnitInfoRect(currentX + cursorWidth, parentBounds.getTopRight().y, 160.0f, 60.0f);
		g.setColour(Colour::fromRGBA(128, 128, 128, 128));
		g.fillRect(currentX, parentBounds.getTopLeft().y, cursorWidth, parentBounds.getHeight());
		g.fillRect(cursorTimeUnitInfoRect);
		int currentMeasureIndex = (int)floor((float)numMeasures * ((float)currentTimeUnit / (float)numberOfTimeUnits));
		g.setColour(Colours::white);
		g.setFont(12.0f);
		String currentTimeUnitFormattedText = "";
		String currentContextText = "";
		String nextContextText = "";

		if (contextPerMeasureVector[currentMeasureIndex].size() > 0)
		{
			currentContextText = contextPerMeasureVector[currentMeasureIndex][0].friendlyName();
			if (currentMeasureIndex == numMeasures - 1)
			{
				nextContextText = contextPerMeasureVector[0][0].friendlyName();
			}
			else
			{
				if (contextPerMeasureVector[currentMeasureIndex + 1].size() > 0)
				{
					nextContextText = contextPerMeasureVector[currentMeasureIndex + 1][0].friendlyName();
				}
			}
			nextContextText = "..." + nextContextText;
			g.setFont(30.0f);
			cursorTimeUnitInfoRect.translate(0, 12.0f);
			cursorTimeUnitInfoRect.setHeight(cursorTimeUnitInfoRect.getHeight() - 24.0f);
			g.drawText(currentContextText, cursorTimeUnitInfoRect, Justification::topLeft, true);
			g.setFont(17.0f);
			g.drawText(nextContextText, cursorTimeUnitInfoRect, Justification::bottomRight, true);
		}

	}
}

void MIDITimelineMarkovComponent::resized()
{
	if (rhythmMatrix == nullptr)
	{
		rhythmMatrix = make_unique<MarkovMatrixComponent>(vector<int>{2, 4, 8, 16}, 2,
			vector<vector<float>>
		{
			{ 0.1f, 0.2f, 0.3f, 0.4f },
			{ 0.2f, 0.3f, 0.4f, 0.1f },
			{ 0.3f, 0.4f, 0.1f, 0.2f },
			{ 0.4f, 0.1f, 0.2f, 0.3f }
		});

		rhythmMatrix->setSize(200, 200);
		addAndMakeVisible(rhythmMatrix.get());
		rhythmMatrix->resized();
	}
	//rhythmMatrix->setBounds(getLocalBounds().reduced(10));
}

