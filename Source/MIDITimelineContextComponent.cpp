/*
  ==============================================================================

	MIDITimelineContextComponent.cpp
	Created: 25 Aug 2024 9:53:45am
	Author:  viktor

  ==============================================================================
*/
#include "MIDITimelineComponent.h"
#include "MIDITimelineContextComponent.h"
#include "ApplicationProperties.h"
#include "MainComponent.h"

MIDITimelineContextComponent::MIDITimelineContextComponent(int nM) : MIDITimelineComponent(nM)
{
	this->synthID = 2;
	outlineColour = Colours::darkcyan;
}

MIDITimelineContextComponent::~MIDITimelineContextComponent()
{
}

void MIDITimelineContextComponent::generateRhythm()
{
}

void MIDITimelineContextComponent::generateContexts()
{
	musicMath.generateContexts(numMeasures, noteEventMatrix, numQuartersPerMeasure, numTimeUnitsPerMeasure);
	repaintMatrixImage();
	setComponentSize();
	repaint();
}

void MIDITimelineContextComponent::paint(Graphics& g)
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
