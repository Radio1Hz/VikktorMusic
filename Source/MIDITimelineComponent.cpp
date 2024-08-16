/*
  ==============================================================================

	MIDITimeline.cpp
	Created: 17 Apr 2024 5:37:02pm
	Author:  viktor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "MIDITimelineComponent.h"
#include "MusicMath.h"
#include "ApplicationProperties.h"
#include "MainComponent.h"

using namespace juce;
using namespace dsp;

//==============================================================================
MIDITimelineComponent::MIDITimelineComponent()
{

}

MIDITimelineComponent::MIDITimelineComponent(int numMeasures, int synthID)
{
	this->numMeasures = numMeasures;
	this->synthID = synthID;
}

void MIDITimelineComponent::init()
{

	noteRangeSize = noteRangeEnd - noteRangeStart;
	noteEventMatrix.resize(noteRangeSize);
	clearMatrix();

	this->name = "MIDI Timeline (" + String(this->numMeasures) + " measures)";
	this->initMenu();
	setAudioChannels(0, 2);
	// Init audioBuffer to 10sec
	audioBuffer.setSize(2, 10 * (int)sampleRateInt, false);
	repaintMatrixImage();
	setComponentSize();
	repaint();
}

void MIDITimelineComponent::clearMatrix()
{
	numQuartersPerMeasure = 4.0f * (float)AppProperties::getNumerator() / (float)AppProperties::getDenominator();
	numTimeUnitsInMeasure = (int)(numQuartersPerMeasure * 4.0f);

	for (int i = 0; i < this->noteRangeSize; i++)
	{
		noteEventMatrix[i].clear();
		std::vector<NoteEventDesc> vec;
		vec.resize((int)(4.0f * (float)this->numMeasures * numQuartersPerMeasure));
		noteEventMatrix[i] = vec;
	}
	stopMIDI();
}
MIDITimelineComponent::~MIDITimelineComponent()
{
	removeMouseListener(this);
	removeAllChildren();
	deleteAllChildren();
	for (auto arr : noteEventMatrix)
	{
		arr.clear();
	}
	noteEventMatrix.clear();
	shutdownAudio();
}

void MIDITimelineComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	this->samplesPerBlockExpectedInt = samplesPerBlockExpected;
	this->sampleRateInt = sampleRate;
	synths.clear();
	SynthAudioSource* src = new SynthAudioSource(8, synthID);
	src->prepareToPlay(samplesPerBlockExpectedInt, sampleRateInt);
	synths.add(src);
	scanPlugins();
}

void MIDITimelineComponent::scanPlugins()
{

}

void MIDITimelineComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
	if (isPlaying)
	{
		if (!synths.isEmpty())
		{
			int numberOfTimeUnits = (int)noteEventMatrix[0].size();
			int currentTimeUnitSnapshot = currentTimeUnit;

			//Duration of one quarter. 
			//Time units are sexteenths and there are [numerator] units in one beat

			double beatDuration = 60.0 / (double)AppProperties::getTempo();
			double timeUnitDuration = beatDuration / 4.0;
			//double totalDuration = numberOfTimeUnits * timeUnitDuration;
			double samplesPerTimeUnit = sampleRateInt * timeUnitDuration;


			int currentTimeUnitPredicted = (int)floor((double)samplesElapsedSincePlay / samplesPerTimeUnit);

			// If end happened - replay			
			if (currentTimeUnitPredicted >= numberOfTimeUnits)
			{
				currentTimeUnit = 0;
				samplesElapsedSincePlay = 0;
				this->synths[0]->synth.allNotesOff(0, true);
				triggerRepaint();
			}

			// If change happened
			if (currentTimeUnitPredicted > currentTimeUnitSnapshot)
			{
				//synths[0]->synth.allNotesOff(0,true);
				for (int i = 0; i < noteEventMatrix.size(); i++)
				{
					if (noteEventMatrix[i][currentTimeUnit].EventType != -1)
					{
						if (noteEventMatrix[i][currentTimeUnit].EventType == 1)
						{
							synths[0]->synth.noteOn(0, noteEventMatrix[i][currentTimeUnit].NoteNumber, 0.5f);
						}
						if (noteEventMatrix[i][currentTimeUnit].EventType == 0)
						{
							synths[0]->synth.noteOff(0, noteEventMatrix[i][currentTimeUnit].NoteNumber, 0.5f, true);
						}
					}
				}
				currentTimeUnit++;
				triggerRepaint();
			}
			synths[0]->getNextAudioBlock(bufferToFill);


			if (AppProperties::getShouldSaveAudio())
			{
				//Transfer and append samples from bufferToFill to audioBuffer

				int numSamplesInBuffer = audioBuffer.getNumSamples();
				for (int i = 0; i < bufferToFill.numSamples; i++)
				{
					audioBuffer.setSample(0, (samplesElapsedSincePlay + i) % numSamplesInBuffer, bufferToFill.buffer->getSample(0, i));
					audioBuffer.setSample(1, (samplesElapsedSincePlay + i) % numSamplesInBuffer, bufferToFill.buffer->getSample(1, i));
				}
			}
			samplesElapsedSincePlay += bufferToFill.numSamples;
		}
	}
}

void MIDITimelineComponent::releaseResources()
{
	for (auto synth : synths)
	{
		synth->releaseResources();
	}
}


void MIDITimelineComponent::paint(juce::Graphics& g)
{
	String tempoInfo = " Tempo: " + String(AppProperties::getTempo());
	String measuresInfo = " Measures: " + String(this->numMeasures);
	this->name = this->projectName + " - ";

	if (this->midiFile)
	{
		String midiInfo = "Midi timeformat: " + String(defaultTicksPerQuarterNote) + " Last timestamp: " + String(midiFile->getLastTimestamp()) + " Tracks: " + String(midiFile->getNumTracks());
		this->name += midiInfo;
	}

	double beatDuration = 60.0 / (double)AppProperties::getTempo();
	double timeUnitDuration = beatDuration / 4.0f;
	double totalDurationSec = (timeUnitDuration * (double)numMeasures * (double)numTimeUnitsInMeasure);
	double currentTimeSec = currentTimeUnit * timeUnitDuration;

	this->name += tempoInfo + measuresInfo + " Time: " + MainComponent::displayProgress(currentTimeSec, totalDurationSec);

	drawOutline(g);

	Rectangle<float> parentBounds = getReducedLocalBounds().toFloat();
	g.setColour(Colour::fromRGB(35, 35, 35));

	if (midiFile != nullptr && viewMode == 0)
	{
		float totalDuration = (float)midiFile->getLastTimestamp();

		g.setFont(getFontSize());
		int ticksPerMeasure = (int)((float)defaultTicksPerQuarterNote * numQuartersPerMeasure);
		for (int measureIndex = 0; measureIndex < numMeasures; measureIndex++)
		{
			float x = (float)(measureIndex * ticksPerMeasure * parentBounds.getWidth() / totalDuration);
			g.setColour(Colour::fromRGB(30, 30, 30));
			g.drawLine(parentBounds.getTopLeft().getX() + x, parentBounds.getTopLeft().getY(), parentBounds.getTopLeft().getX() + x, parentBounds.getBottomRight().getY(), 1.0f);

			g.setColour(Colours::lightgrey);
			g.drawText(String(measureIndex + 1), Rectangle<float>(parentBounds.getTopLeft().getX() + x + 2, parentBounds.getTopLeft().getY() + 2, parentBounds.getWidth() / numMeasures, 15.0f), Justification::topLeft);
		}

		float trackHeight = (parentBounds.getHeight() * (1.0f - this->timelineHeightRatio)) / midiTracks.size();
		Rectangle<float> trackRect = parentBounds;
		trackRect.setHeight(trackHeight);
		trackRect.translate(0.0f, (float)(parentBounds.getHeight() * this->timelineHeightRatio));

		for (int i = 0; i < midiTracks.size(); i++)
		{
			g.setColour(juce::Colours::grey);
			if (i < midiTracks.size())
			{
				g.drawLine(0.0f, trackRect.getTopLeft().y, trackRect.getWidth(), trackRect.getTopLeft().y);
			}

			g.setFont(trackHeight / 25.0f);
			g.drawText("Track " + String(i + 1), trackRect, juce::Justification::topLeft, true);
			drawMIDIEvents(trackRect, i, g);
			trackRect.translate(0, (float)trackHeight);
		}
	}
	else
	{
		if (noteEventMatrix.size() > 0)
		{
			float measureWidthInPixels = parentBounds.getWidth() / (float)numMeasures;
			float timeUnitWidthInPixels = parentBounds.getWidth() / ((float)numMeasures * (float)numTimeUnitsInMeasure);
			int numberOfTimeUnits = (int)noteEventMatrix[0].size();
			g.drawImage(matrixImage, parentBounds);
			float cursorWidth = parentBounds.getWidth() / numberOfTimeUnits;
			float currentX = ((float)currentTimeUnit / (float)numberOfTimeUnits) * parentBounds.getWidth();
			Rectangle<float> cursorInfoRect(currentX + cursorWidth, parentBounds.getTopRight().y + 15.0f, 40.0f, 15.0f);
			g.setColour(Colour::fromRGBA(128, 128, 128, 128));
			g.fillRect(currentX, parentBounds.getTopLeft().y, cursorWidth, parentBounds.getHeight());
			g.fillRect(cursorInfoRect);
			int currentMeasureIndex = (int)floor((float)numMeasures * ((float)currentTimeUnit / (float)numberOfTimeUnits));
			int currentTimeUnitWithinMeasureIndex = (currentMeasureIndex * numTimeUnitsInMeasure + currentTimeUnit) % (numTimeUnitsInMeasure);
			g.setColour(juce::Colours::white);
			g.setFont(12.0f);
			String cursorText = String::formatted("%02d", currentMeasureIndex + 1) + "|" + String::formatted("%02d", currentTimeUnitWithinMeasureIndex + 1);

			g.drawText(cursorText, cursorInfoRect, juce::Justification::centred, true);

			if (measureWidthInPixels > 60)
			{
				g.setFont(10.0f);

				for (int currMeasure = 0; currMeasure < contextPerMeasureVector.size(); currMeasure++)
				{
					Rectangle<float> measureTonalityRect(measureWidthInPixels, contextPerMeasureVector[currMeasure].size() * 10.0f);

					measureTonalityRect.setPosition(currMeasure * measureWidthInPixels + timeUnitWidthInPixels, 2 * timeUnitWidthInPixels + (float)headerHeight + 1.0f);
					String text = "";
					for (int t = 0; t < contextPerMeasureVector[currMeasure].size(); t++)
					{
						if (t == 0)
						{
							text += contextPerMeasureVector[currMeasure][t].debug();
						}
						else
						{
							text += "\r\n" + contextPerMeasureVector[currMeasure][t].debug();
						}
					}

					g.drawMultiLineText(text, (int)measureTonalityRect.getTopLeft().x, (int)measureTonalityRect.getTopLeft().y, (int)measureWidthInPixels, juce::Justification::left);

				}
			}

			//Draw Selection
			if (selectedCellStart > -1)
			{
				Rectangle<float> selectionRect(selectedCellStart * cursorWidth, 0.0f, cursorWidth * (selectedCellEnd - selectedCellStart + 1), parentBounds.getHeight());
				selectionRect.translate(1.0f, (float)headerHeight + cursorWidth);
				g.setColour(Colour::fromRGBA(192, 192, 192, 128));
				g.fillRect(selectionRect);
			}

		}
	}
}

void MIDITimelineComponent::drawMIDIEvents(Rectangle<float> trackRect, int trackIndex, Graphics& g)
{
	double totalDuration = midiFile->getLastTimestamp();

	for (int i = 0; i < midiTracks[trackIndex]->getNumEvents(); i++)
	{
		juce::MidiMessageSequence::MidiEventHolder* midiEvent = midiTracks[trackIndex]->getEventPointer(i);
		MidiMessage midiMessage = midiEvent->message;

		if (midiMessage.isNoteOn())
		{
			juce::MidiMessageSequence::MidiEventHolder* noteOffHolder = midiEvent->noteOffObject;
			int noteNumber = midiMessage.getNoteNumber();
			double noteStart = midiMessage.getTimeStamp();
			double noteEnd = noteStart;

			if (noteOffHolder)
			{
				noteEnd = noteOffHolder->message.getTimeStamp();
			}

			float pixelStart = (float)(noteStart * trackRect.getWidth() / totalDuration);
			float pixelEnd = (float)(noteEnd * trackRect.getWidth() / totalDuration);
			float yVel = (float)(128 - noteNumber) * trackRect.getHeight() / 128.0f;
			g.setColour(Colours::lightgreen);

			if (getFontSize() / 8 > 1)
			{
				g.setFont(getFontSize() / 8);
				g.drawText(midiMessage.getMidiNoteName(noteNumber, true, true, 4), Rectangle<float>(trackRect.getTopLeft().getX() + pixelStart, trackRect.getTopLeft().getY() + yVel - getFontSize() / 8.0f, getFontSize(), getFontSize() / 8.0f), Justification::topLeft);

			}
			g.drawLine(trackRect.getTopLeft().getX() + pixelStart, trackRect.getTopLeft().getY() + yVel, trackRect.getTopLeft().getX() + pixelEnd, trackRect.getTopLeft().getY() + yVel, 1.0f);
		}
	}


}

void MIDITimelineComponent::resized()
{
	//setComponentSize();
}

void MIDITimelineComponent::loadMIDI()
{
	fileChooser = std::make_unique<FileChooser>("Please select the .mid file you want to load...", File("C:\\Data\\SourceCode\\everything\\Music\\MIDI"), "*.mid");

	auto folderChooserFlags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles;

	fileChooser->launchAsync(folderChooserFlags, [this](const FileChooser& chooser)
		{
			File file(chooser.getResult());

			if (file != juce::File{})
			{
				clearMatrix();
				currentTimeUnit = 0;
				midiFile = std::make_unique<MidiFile>();
				std::unique_ptr<FileInputStream> str = file.createInputStream();
				midiFile->readFrom(*str);
				defaultTicksPerQuarterNote = midiFile->getTimeFormat();
				projectName = file.getFileName();
				name = file.getFileName() + " - Midi timeformat: " + String(defaultTicksPerQuarterNote) + ", last timestamp: " + String(midiFile->getLastTimestamp()) + ", tracks: " + String(midiFile->getNumTracks() + ", tempo: " + String(AppProperties::getTempo()));
				processMidi();
				defineAllContextsPerMeasures();
				repaintMatrixImage();
				setComponentSize();
				repaint();
			}
		});
}

void MIDITimelineComponent::allNotesOff()
{
	if (this->synths.size() > 0)
	{
		this->synths[0]->synth.allNotesOff(0, true);
	}
}

void MIDITimelineComponent::processMidi()
{
	if (midiFile)
	{
		midiTracks.clear();

		for (int i = 0; i < midiFile->getNumTracks(); i++)
		{
			const MidiMessageSequence* seq = midiFile->getTrack(i);
			MidiMessageSequence* seqCopy = new MidiMessageSequence(*seq);
			midiTracks.add(seqCopy);
		}

		// Analyze MIDI
		int numberOfSharpsOrFlats = 0;
		this->noteProbabilities.assign(12, 0.0f);

		String majMin = "";
		//double midiTicksPerMeasure = ((double)numTimeUnitsInMeasure);
		double maxMatrixTicksCapacity = (double)numMeasures * numQuartersPerMeasure * (double)defaultTicksPerQuarterNote;

		for (int trackIndex = 0; trackIndex < midiTracks.size(); trackIndex++)
		{
			for (int eventIndex = 0; eventIndex < midiTracks[trackIndex]->getNumEvents(); eventIndex++)
			{
				juce::MidiMessageSequence::MidiEventHolder* midiEvent = midiTracks[trackIndex]->getEventPointer(eventIndex);
				MidiMessage midiMessage = midiEvent->message;
				double currentMIDITicks = midiMessage.getTimeStamp();
				if (currentMIDITicks > maxMatrixTicksCapacity)
				{
					break;
				}

				if (midiMessage.isKeySignatureMetaEvent())
				{
					if (!midiMessage.isKeySignatureMajorKey())
					{
						majMin = "min";
					}
					else
					{
						majMin = "maj";
					}
					numberOfSharpsOrFlats = midiMessage.getKeySignatureNumberOfSharpsOrFlats();
				}
			}
		}
	}

	if (noteEventMatrix.size() > 0)
	{
		int matrixWidth = (int)noteEventMatrix[0].size();

		// If MIDI file is loaded, parse it and fill the matrix
		if (midiFile)
		{
			//---------------------
			for (int trackIndex = 0; trackIndex < midiTracks.size(); trackIndex++)
			{
				for (int eventIndex = 0; eventIndex < midiTracks[trackIndex]->getNumEvents(); eventIndex++)
				{
					juce::MidiMessageSequence::MidiEventHolder* midiEvent = midiTracks[trackIndex]->getEventPointer(eventIndex);
					MidiMessage midiMessage = midiEvent->message;

					if (midiMessage.isNoteOn())
					{
						juce::MidiMessageSequence::MidiEventHolder* noteOffHolder = midiEvent->noteOffObject;
						int noteNumber = midiMessage.getNoteNumber();
						double noteStart = midiMessage.getTimeStamp();

						double noteEnd = noteStart;

						if (noteOffHolder)
						{
							noteEnd = noteOffHolder->message.getTimeStamp();
						}

						int ticksPerMeasure = (int)((float)defaultTicksPerQuarterNote * (float)numQuartersPerMeasure);

						double ticksPerTimeUnit = ((double)ticksPerMeasure / (double)numQuartersPerMeasure) / 4.0;
						double totalDurationTicks = matrixWidth * ticksPerTimeUnit;

						int duration = roundToInt((noteEnd - noteStart) / ticksPerTimeUnit);
						int column = roundToInt(((noteStart / totalDurationTicks) * (double)matrixWidth));

						if (noteNumber >= noteRangeStart && noteNumber < noteRangeEnd && column < matrixWidth)
						{
							if (duration > 0)
							{
								NoteEventDesc nEventOff(midiMessage.getMidiNoteName(noteNumber, true, false, 4), noteNumber, duration, 0);

								if (column + duration < noteEventMatrix[0].size())
								{
									noteEventMatrix[noteNumber - noteRangeStart][column + duration] = nEventOff;
								}
								else
								{
									noteEventMatrix[noteNumber - noteRangeStart][noteEventMatrix[0].size() - 1] = nEventOff;
								}

								NoteEventDesc nEventOn(midiMessage.getMidiNoteName(noteNumber, true, false, 4), noteNumber, duration, 1);
								noteEventMatrix[noteNumber - noteRangeStart][column] = nEventOn;
							}
							else
							{
								duration = 0;
							}

						}
					}
				}
			}
			//---------------------
		}


		this->initMenu();
	}
}

void MIDITimelineComponent::setComponentSize()
{

	Rectangle<float> localBounds = getParentComponent()->getLocalBounds().toFloat();
	double aspectRatio = (double)matrixImage.getWidth() / (double)matrixImage.getHeight();

	if (noteEventMatrix.size() > 0)
	{
		setBounds((int)localBounds.getX(), (int)localBounds.getY(), (int)((float)aspectRatio * localBounds.getHeight()), (int)(localBounds.getHeight()));
	}
	//if (midiFile)
	//{
	//	float trackHeight = (localBounds.getHeight() * (1 - this->timelineHeightRatio)) / (midiTracks.size());
	//	setBounds((int)localBounds.getX(), (int)localBounds.getY(), (int)localBounds.getWidth(), (int)trackHeight * midiTracks.size() + (int)(localBounds.getHeight() * (this->timelineHeightRatio)));
	//}
	repaint();
}

void MIDITimelineComponent::initMenu()
{
	this->menu.clear();
	this->menu.addItem("Load MIDI", std::bind(&MIDITimelineComponent::loadMIDI, this));

	if (noteEventMatrix.size() > 0)
	{
		this->menu.addItem("Play", std::bind(&MIDITimelineComponent::playMIDI, this));
		this->menu.addItem("Stop", std::bind(&MIDITimelineComponent::stopMIDI, this));
		this->menu.addItem("Repaint Matrix", std::bind(&MIDITimelineComponent::repaintMatrixImage, this));
		this->menu.addItem("Clear", std::bind(&MIDITimelineComponent::clearTimeline, this));
		this->menu.addItem("Analyze Context in Selection", std::bind(&MIDITimelineComponent::analyzeContextInSelection, this));
		this->menu.addItem("Process Selection", std::bind(&MIDITimelineComponent::processSelection, this));
		this->menu.addItem("Save audioBuffer to disk", std::bind(&MIDITimelineComponent::saveAudioBufferToDisk, this));
		this->menu.addItem("Save Timeline to MIDI", std::bind(&MIDITimelineComponent::saveMIDIFileToDisk, this));
	}
}

void MIDITimelineComponent::populateSelectionMatrix()
{
	selectionMatrix = std::make_unique<Matrix<int>>(noteRangeSize, 1 + selectedCellEnd - selectedCellStart);
	selectionMatrix->clear();

	for (int i = 0; i < noteRangeSize; i++)
	{
		for (int j = selectedCellStart; j <= selectedCellEnd; j++)
		{
			// If there is note
			if (noteEventMatrix[i][j].EventType == 1)
			{
				selectionMatrix->operator()(i, j - selectedCellStart) = noteEventMatrix[i][j].NoteNumber;
			}
		}
	}
}

void MIDITimelineComponent::clearTimeline()
{
	clearMatrix();
	defineAllContextsPerMeasures();
	repaintMatrixImage();
	repaint();
}

void MIDITimelineComponent::playMIDI()
{
	isPlaying = true;
}

void MIDITimelineComponent::stopMIDI()
{
	isPlaying = false;
	samplesElapsedSincePlay = 0;
	currentTimeUnit = 0;

	if (synths.size() > 0)
	{
		synths[0]->synth.allNotesOff(0, true);
	}
}

void MIDITimelineComponent::repaintMatrixImage()
{
	if (noteEventMatrix.size() > 0)
	{
		Rectangle<int> parentBounds = getReducedLocalBounds();
		Rectangle<int> newImageSize(200, 200);
		float timeUnitWidthPixels = minCellWidth;
		int numberOfTimeUnits = (int)noteEventMatrix[0].size();

		newImageSize.setWidth((int)((float)numberOfTimeUnits * timeUnitWidthPixels));
		newImageSize.setHeight((int)((float)noteRangeSize * timeUnitWidthPixels));
		double aspectRatio = newImageSize.getWidth() / newImageSize.getHeight();

		auto maxBitmapSize = 16000;
		if (newImageSize.getWidth() > maxBitmapSize)
		{
			double sizeRatio = (double)maxBitmapSize / (double)newImageSize.getWidth();
			newImageSize.setWidth(maxBitmapSize);
			newImageSize.setHeight((int)((double)maxBitmapSize / aspectRatio));
			timeUnitWidthPixels = minCellWidth * sizeRatio;
		}
		matrixImage = juce::Image(juce::Image::RGB, newImageSize.getWidth(), newImageSize.getHeight(), true);

		Graphics g0(matrixImage);
		g0.setColour(Colours::grey);

		for (int i = 0; i < noteRangeSize; i++)
		{
			float currentY = (float)(newImageSize.getHeight() * (((float)noteRangeSize - (float)i - 1.0f) / (float)noteRangeSize));
			float currentX = 0.0f;

			Rectangle<float> textBox(0.0f, currentY, timeUnitWidthPixels, (float)(newImageSize.getHeight() / (float)noteRangeSize));
			Rectangle<float> textBoxWider(textBox);

			float fontSize = (float)(timeUnitWidthPixels);
			g0.setFont(fontSize);

			textBoxWider.setWidth(fontSize * 3.0f);

			g0.drawText(String(noteRangeStart + i), textBoxWider, Justification::left);
			g0.drawLine(0.0f, currentY, (float)newImageSize.getWidth(), currentY, 0.2f);

			for (int j = 0; j < numberOfTimeUnits; j++)
			{
				int currentMeasureIndex = (int)floor((float)numMeasures * ((float)j / (float)numberOfTimeUnits));
				currentX = (float)newImageSize.getWidth() * ((float)j / (float)numberOfTimeUnits);

				textBox.setPosition(Point<float>(currentX, currentY));
				textBoxWider.setPosition(Point<float>(currentX, currentY));
				textBox.setWidth(fontSize * 2);

				if (j % numTimeUnitsInMeasure == 0)
				{
					g0.setColour(Colours::lightgrey);
					g0.drawLine(textBox.getBottomLeft().x, textBox.getBottomLeft().y, textBox.getTopLeft().x, textBox.getTopLeft().y, 1.0f);

					if (i == noteRangeSize - 1)
					{
						g0.drawText(String(currentMeasureIndex + 1), textBoxWider, Justification::centred);
					}
				}
				else
				{
					if ((currentMeasureIndex * numTimeUnitsInMeasure + j) % (numTimeUnitsInMeasure / (int)numQuartersPerMeasure) == 0)
					{
						if (i < noteRangeSize - 1)
						{
							g0.setColour(Colours::white);
							g0.drawLine(textBox.getBottomLeft().x, textBox.getBottomLeft().y, textBox.getTopLeft().x, textBox.getTopLeft().y, 0.5f);
						}
					}
					else
					{
						g0.setColour(Colours::lightgrey);
						g0.drawLine(textBox.getBottomLeft().x, textBox.getBottomLeft().y, textBox.getTopLeft().x, textBox.getTopLeft().y, 0.2f);
					}
				}

				if (noteEventMatrix[i][j].EventType == 1) //if note on
				{
					textBox.setWidth(timeUnitWidthPixels * (float)noteEventMatrix[i][j].NoteDuration);
					g0.setColour(Colours::darkred);
					g0.fillRect(textBox);
					g0.setColour(Colours::white);
					g0.drawText(String(noteEventMatrix[i][j].NoteName), textBox, Justification::left);
				}
			}
		}

		// Save in text format
		if (projectName == "")
		{
			projectName = "New Project";
		}

		FileOutputStream stream = FileOutputStream(File(AppProperties::getProjectPath() + projectName + "." + String(Time::currentTimeMillis()) + ".proj"));

		for (int i = 0; i < noteRangeSize; i++)
		{
			String str = "";
			for (int j = 0; j < noteEventMatrix[i].size(); j++)
			{
				if (noteEventMatrix[i][j].NoteName != "")
				{
					str += String(noteEventMatrix[i][j].NoteNumber) + "\t";
				}
				else
				{
					str += "\t";
				}
			}
			stream.writeText(str + "\r\n", false, false, nullptr);
		}
		stream.flush();
		//Thread thread("PNGFileSaveThread");
		/*Image matrixImageCopy(matrixImage);
		FileOutputStream str(File(AppProperties::getProjectPath() + projectName + "." + String(Time::currentTimeMillis()) + ".png"));
		if (str.openedOk())
		{
			str.setPosition(0);
			str.truncate();
		}
		PNGImageFormat pngWriter;
		pngWriter.writeImageToStream(matrixImageCopy, str);
		str.flush();*/
	}
}

void MIDITimelineComponent::processSelection()
{

	if (selectedCellStart > -1)
	{
		populateSelectionMatrix();
		analyzeContextInSelection();
	}
}

void MIDITimelineComponent::changeListenerCallback(ChangeBroadcaster* /*source*/)
{

}

void MIDITimelineComponent::handleAsyncUpdate()
{
	repaint();
}

void MIDITimelineComponent::analyzeContextInSelection()
{
	Matrix<int>& defMajorScaleVector(*musicMath._defaultMajorScaleDefinitionVector.get());
	Matrix<int>& defMinorScaleVector(*musicMath._defaultMinorScaleDefinitionVector.get());

	Matrix<int> defMajorChordVector(*musicMath._defaultMajorChordDefinitionVector.get());
	Matrix<int> defMinorChordVector(*musicMath._defaultMinorChordDefinitionVector.get());

	Matrix<int> defMajorScaleVectorFull(1, noteRangeEnd - noteRangeStart);
	Matrix<int> defMinorScaleVectorFull(1, noteRangeEnd - noteRangeStart);

	Matrix<int> defMajorChordVectorFull(1, noteRangeEnd - noteRangeStart);
	Matrix<int> defMinorChordVectorFull(1, noteRangeEnd - noteRangeStart);

	int mostProbableRoot = 0;
	String mostProbableRootFlavor = "maj";

	int maxSum = 0;

	Matrix<int> tempMatrix(*selectionMatrix);
	list<ContextDesc> allPossiblieTonalities;

	//Normalize Matrix
	for (int c = 0; c < selectionMatrix->getNumColumns(); c++)
	{
		for (int n = noteRangeStart; n < noteRangeEnd; n++)
		{
			if (tempMatrix.operator()(n - noteRangeStart, c) != 0)
			{
				tempMatrix.operator()(n - noteRangeStart, c) = 1;
			}
		}
	}

	//Go through all 12 tonalities
	for (int t = 0; t < 12; t++)
	{
		defMajorScaleVectorFull.clear();
		defMinorScaleVectorFull.clear();

		defMajorChordVectorFull.clear();
		defMinorChordVectorFull.clear();

		String tonalityRootName = String(MidiMessage::getMidiNoteName(t, true, false, 4));
		for (int c = 0; c < selectionMatrix->getNumColumns(); c++)
		{
			//Define template vectors for Major and Minor for the Tonality[t]
			for (int n = noteRangeStart; n < noteRangeEnd; n++)
			{
				defMajorScaleVectorFull.operator()(0, n - noteRangeStart) = defMajorScaleVector.operator()(0, (n - t) % 12);
				defMinorScaleVectorFull.operator()(0, n - noteRangeStart) = defMinorScaleVector.operator()(0, (n - t) % 12);

				defMajorChordVectorFull.operator()(0, n - noteRangeStart) = defMajorChordVector.operator()(0, (n - t) % 12);
				defMinorChordVectorFull.operator()(0, n - noteRangeStart) = defMinorChordVector.operator()(0, (n - t) % 12);
			}
		}

		Matrix<int> resMajChord = musicMath.multiplyMatrixAndVector(tempMatrix, defMajorChordVectorFull);
		Matrix<int> resMinChord = musicMath.multiplyMatrixAndVector(tempMatrix, defMinorChordVectorFull);
		Matrix<int> resMajScale = musicMath.multiplyMatrixAndVector(tempMatrix, defMajorScaleVectorFull);
		Matrix<int> resMinScale = musicMath.multiplyMatrixAndVector(tempMatrix, defMinorScaleVectorFull);

		int sumMajChord = musicMath.sumOfCellsInMatrix(resMajChord);
		int sumMinChord = musicMath.sumOfCellsInMatrix(resMinChord);

		int sumMajScale = musicMath.sumOfCellsInMatrix(resMajScale);
		int sumMinScale = musicMath.sumOfCellsInMatrix(resMinScale);

		if (maxSum < sumMajChord + sumMinChord + sumMajScale + sumMinScale)
		{
			if (sumMajChord + sumMajScale >= sumMinChord + sumMinScale)
			{
				mostProbableRootFlavor = "maj";
			}
			else
			{
				mostProbableRootFlavor = "min";
			}

			float p = (float)(sumMajChord + sumMinChord + sumMajScale + sumMinScale);
			mostProbableRoot = t;
			maxSum = sumMajChord + sumMinChord + sumMajScale + sumMinScale;
			ContextDesc cd(mostProbableRoot, mostProbableRootFlavor == "maj", p);
			allPossiblieTonalities.push_back(cd);
		}
	}

	allPossiblieTonalities.sort([](const ContextDesc& f, const ContextDesc& s) { return f.Probability > s.Probability; });

	for (ContextDesc& ctx : allPossiblieTonalities)
	{
		ctx.Probability = ctx.Probability / (float)maxSum;
		DBG(ctx.debug());
	}
}

void MIDITimelineComponent::defineAllContextsPerMeasures()
{
	Matrix<int>& defMajorScaleVector(*musicMath._defaultMajorScaleDefinitionVector.get());
	Matrix<int>& defMinorScaleVector(*musicMath._defaultMinorScaleDefinitionVector.get());

	Matrix<int> defMajorChordVector(*musicMath._defaultMajorChordDefinitionVector.get());
	Matrix<int> defMinorChordVector(*musicMath._defaultMinorChordDefinitionVector.get());

	Matrix<int> defMajorScaleVectorFull(1, noteRangeEnd - noteRangeStart);
	Matrix<int> defMinorScaleVectorFull(1, noteRangeEnd - noteRangeStart);

	Matrix<int> defMajorChordVectorFull(1, noteRangeEnd - noteRangeStart);
	Matrix<int> defMinorChordVectorFull(1, noteRangeEnd - noteRangeStart);

	// Martix of a size of a measure
	contextPerMeasureVector.clear();
	contextPerMeasureVector.resize(numMeasures);

	for (int z = 0; z < numMeasures; z++)
	{
		int mostProbableRoot = 0;
		String mostProbableRootFlavor = "maj";
		int maxSum = 0;
		list<ContextDesc> allPossiblieTonalities;
		int timeunitStart = z * numTimeUnitsInMeasure;
		//Copy portion of noteEventMatrix of one measure 1M
		Matrix<int> tempMatrix(noteRangeEnd - noteRangeStart, numTimeUnitsInMeasure);
		//Normalize Matrix
		bool shouldNormalizeMatrix = true;
		tempMatrix.clear();

		for (int c = 0; c < tempMatrix.getNumColumns(); c++)
		{
			for (int n = noteRangeStart; n < noteRangeEnd; n++)
			{
				if (noteEventMatrix[n - noteRangeStart][timeunitStart + c].NoteNumber == n && noteEventMatrix[n - noteRangeStart][timeunitStart + c].EventType == 1)
				{
					tempMatrix.operator()(n - noteRangeStart, c) = shouldNormalizeMatrix ? 1 : n;
				}
				else
				{
					tempMatrix.operator()(n - noteRangeStart, c) = 0;
				}
			}
		}

		for (int t = 0; t < 12; t++)
		{
			defMajorScaleVectorFull.clear();
			defMinorScaleVectorFull.clear();

			defMajorChordVectorFull.clear();
			defMinorChordVectorFull.clear();

			String tonalityRootName = String(MidiMessage::getMidiNoteName(t, true, false, 4));
			for (int c = 0; c < tempMatrix.getNumColumns(); c++)
			{
				//Define template vectors for Major and Minor for the Tonality[t]
				for (int n = noteRangeStart; n < noteRangeEnd; n++)
				{
					defMajorScaleVectorFull.operator()(0, n - noteRangeStart) = defMajorScaleVector.operator()(0, (n - t) % 12);
					defMinorScaleVectorFull.operator()(0, n - noteRangeStart) = defMinorScaleVector.operator()(0, (n - t) % 12);

					defMajorChordVectorFull.operator()(0, n - noteRangeStart) = defMajorChordVector.operator()(0, (n - t) % 12);
					defMinorChordVectorFull.operator()(0, n - noteRangeStart) = defMinorChordVector.operator()(0, (n - t) % 12);
				}
			}

			Matrix<int> resMajChord = musicMath.multiplyMatrixAndVector(tempMatrix, defMajorChordVectorFull);
			Matrix<int> resMinChord = musicMath.multiplyMatrixAndVector(tempMatrix, defMinorChordVectorFull);
			Matrix<int> resMajScale = musicMath.multiplyMatrixAndVector(tempMatrix, defMajorScaleVectorFull);
			Matrix<int> resMinScale = musicMath.multiplyMatrixAndVector(tempMatrix, defMinorScaleVectorFull);

			int sumMajChord = musicMath.sumOfCellsInMatrix(resMajChord);
			int sumMinChord = musicMath.sumOfCellsInMatrix(resMinChord);

			int sumMajScale = musicMath.sumOfCellsInMatrix(resMajScale);
			int sumMinScale = musicMath.sumOfCellsInMatrix(resMinScale);

			if (maxSum < sumMajChord + sumMinChord + sumMajScale + sumMinScale)
			{
				if (sumMajChord + sumMajScale >= sumMinChord + sumMinScale)
				{
					mostProbableRootFlavor = "maj";
				}
				else
				{
					mostProbableRootFlavor = "min";
				}

				float p = (float)(sumMajChord + sumMinChord + sumMajScale + sumMinScale);
				mostProbableRoot = t;
				maxSum = sumMajChord + sumMinChord + sumMajScale + sumMinScale;
				ContextDesc cd(mostProbableRoot, mostProbableRootFlavor == "maj", p);
				allPossiblieTonalities.push_back(cd);
			}
		}

		allPossiblieTonalities.sort([](const ContextDesc& f, const ContextDesc& s) { return f.Probability > s.Probability; });

		if (allPossiblieTonalities.size() > 0)
		{
			if (contextPerMeasureVector[z].empty())
			{
				std::vector<ContextDesc> vec;
				contextPerMeasureVector[z] = vec;
			}
			for (ContextDesc& i : allPossiblieTonalities)
			{
				i.Probability = (float)(i.Probability) / (float)maxSum;
				contextPerMeasureVector[z].push_back(i);
			}
		}
	}
}

void MIDITimelineComponent::operationOnSelection01()
{
	Matrix<int>& defMajorScaleVector(*musicMath._defaultMajorScaleDefinitionVector.get());
	Matrix<int>& defMinorScaleVector(*musicMath._defaultMinorScaleDefinitionVector.get());

	Matrix<int> defMajorScaleVectorFull(1, noteRangeEnd - noteRangeStart);
	Matrix<int> defMinorScaleVectorFull(1, noteRangeEnd - noteRangeStart);

	//Go through all 12 tonalities
	for (int t = 0; t < 12; t++)
	{
		int tonalityMajSum = 0;
		int tonalityMinSum = 0;
		String tonalityRootName = String(MidiMessage::getMidiNoteName(t, true, false, 4));

		defMajorScaleVectorFull.clear();
		defMinorScaleVectorFull.clear();

		//Define template vectors for Major and Minor for the Tonality[t]
		for (int n = noteRangeStart; n < noteRangeEnd; n++)
		{
			defMajorScaleVectorFull.operator()(0, n - noteRangeStart) = defMajorScaleVector.operator()(0, (n - t) % 12);
			defMinorScaleVectorFull.operator()(0, n - noteRangeStart) = defMinorScaleVector.operator()(0, (n - t) % 12);
		}

		for (int c = 0; c < selectionMatrix->getNumColumns(); c++)
		{
			//DBG("Timeunit: " + String(c) + ", " + String(tonalityRootName));
			// Matrix that only have data in column c, otherwise zero
			Matrix<int> tempMatrix(*selectionMatrix);

			for (int n = noteRangeStart; n < noteRangeEnd; n++)
			{
				for (int c1 = 0; c1 < selectionMatrix->getNumColumns(); c1++)
				{
					if (c1 != c)
					{
						tempMatrix.operator()(n - noteRangeStart, c1) = 0;
					}
					else
					{
						if (tempMatrix.operator()(n - noteRangeStart, c1) != 0)
						{
							tempMatrix.operator()(n - noteRangeStart, c1) = 1;
						}
					}
				}
			}

			Matrix<int> resMaj = musicMath.multiplyMatrixAndVector(tempMatrix, defMajorScaleVectorFull);
			Matrix<int> resMin = musicMath.multiplyMatrixAndVector(tempMatrix, defMinorScaleVectorFull);
			//DBG("Timeunit: " + String(c) + ", " + String(tonalityRootName) + " MAJOR ");
			//musicMath.debugMatrix(resMaj, noteRangeStart, noteRangeEnd, "resMaj result of multiplication");
			//DBG("Timeunit: " + String(c) + ", " + String(tonalityRootName) + " MINOR ");
			//musicMath.debugMatrix(resMin, noteRangeStart, noteRangeEnd, "resMin result of multiplication");
			int sumMaj = musicMath.sumOfCellsInMatrix(resMaj);
			int sumMin = musicMath.sumOfCellsInMatrix(resMin);

			tonalityMajSum += sumMaj;
			tonalityMinSum += sumMin;
		}

		if (tonalityMajSum > 0)
		{
			DBG(String(tonalityRootName) + " maj, sum: " + String(tonalityMajSum));
		}

		if (tonalityMinSum > 0)
		{
			DBG(String(tonalityRootName) + " min, sum: " + String(tonalityMinSum));
		}
	}
}

void MIDITimelineComponent::saveAudioBufferToDisk()
{
	String wavPath = AppProperties::getProjectPath() + projectName + " audioBuffer.wav";
	File file(wavPath);
	file.deleteFile();

	WavAudioFormat format;
	std::unique_ptr<AudioFormatWriter> writer;

	writer.reset(format.createWriterFor(new FileOutputStream(file),
		(double)sampleRateInt,
		audioBuffer.getNumChannels(),
		24,
		{},
		0));

	if (writer != nullptr)
	{
		writer->writeFromAudioSampleBuffer(audioBuffer, 0, audioBuffer.getNumSamples());
		writer->flush();
	}

}

void MIDITimelineComponent::saveMIDIFileToDisk()
{
	MidiMessageSequence seq;
	MidiMessage msgKeySig = MidiMessage::keySignatureMetaEvent(0, false); // Cmaj
	MidiMessage msgTimeSig = MidiMessage::timeSignatureMetaEvent(AppProperties::getNumerator(), AppProperties::getDenominator()); // Cmaj
	msgKeySig.setTimeStamp(0);
	msgTimeSig.setTimeStamp(0);
	seq.addEvent(msgKeySig);
	seq.addEvent(msgTimeSig);

	float totalDurationInMIDITicks = (float)defaultTicksPerQuarterNote * (float)numMeasures * numQuartersPerMeasure;
	for (int i = 0; i < noteEventMatrix.size(); i++)
	{
		for (int j = 0; j < noteEventMatrix[0].size(); j++)
		{
			double currentMIDITimestamp = ((float)j / (float)noteEventMatrix[0].size()) * totalDurationInMIDITicks;
			NoteEventDesc noteInMatrix = noteEventMatrix[i][j];

			if (noteInMatrix.EventType == 0)
			{
				MidiMessage msg = MidiMessage::noteOff(1, noteInMatrix.NoteNumber, (uint8)128);
				msg.setTimeStamp(currentMIDITimestamp);
				seq.addEvent(msg);
			}

			if (noteInMatrix.EventType == 1)
			{
				MidiMessage msg = MidiMessage::noteOn(1, noteInMatrix.NoteNumber, (uint8)128);
				msg.setTimeStamp(currentMIDITimestamp);
				seq.addEvent(msg);
			}
		}
	}
	MidiFile outputFile;
	outputFile.setTicksPerQuarterNote(defaultTicksPerQuarterNote);
	outputFile.addTrack(seq);
	FileOutputStream outStr(File(AppProperties::getProjectPath() + projectName + "_out.mid"));
	outputFile.writeTo(outStr);
	outStr.flush();
}

void MIDITimelineComponent::shiftDragEvent(const juce::MouseEvent& event)
{
	int currentCell = (int)((double)(numTimeUnitsInMeasure * numMeasures) * ((double)event.x / (double)getLocalBounds().getWidth()));
	if (selectedCellStart == -1)
	{
		selectedCellStart = currentCell;
	}

	selectedCellEnd = currentCell;

	if (selectedCellEnd > numTimeUnitsInMeasure * numMeasures - 1)
		selectedCellEnd = numTimeUnitsInMeasure * numMeasures - 1;

	repaint();
}

void MIDITimelineComponent::shiftMouseDownEvent(const juce::MouseEvent& event)
{
	if (!selectionInProgress)
	{
		selectionInProgress = true;
		int currentCell = (int)((double)(numTimeUnitsInMeasure * numMeasures) * ((double)event.x / (double)getLocalBounds().getWidth()));
		selectedCellStart = currentCell;
		selectedCellEnd = currentCell;
	}
}

void MIDITimelineComponent::shiftMouseUpEvent(const juce::MouseEvent& /*event*/)
{
	selectionInProgress = false;
}

void MIDITimelineComponent::mouseDoubleClickEvent(const juce::MouseEvent& event)
{
	selectionInProgress = false;
	selectedCellStart = -1;
	selectedCellEnd = -1;

	int currentCell = (int)((double)(numTimeUnitsInMeasure * numMeasures) * ((double)event.x / (double)getLocalBounds().getWidth()));
	currentTimeUnit = currentCell;
	double beatDuration = 60.0 / (double)AppProperties::getTempo();
	double timeUnitDuration = beatDuration / 4.0;
	double samplesPerTimeUnit = sampleRateInt * timeUnitDuration;
	samplesElapsedSincePlay = (int)samplesPerTimeUnit * currentCell;
	if (synths.size() > 0)
	{
		synths[0]->synth.allNotesOff(0, true);
	}
	repaint();
}

void MIDITimelineComponent::triggerRepaint()
{
	triggerAsyncUpdate();
}
