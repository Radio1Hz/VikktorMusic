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

//==============================================================================
MIDITimelineComponent::MIDITimelineComponent()
{
	init();
}

MIDITimelineComponent::MIDITimelineComponent(int numMeasures, int synthID)
{
	this->numMeasures = numMeasures;
	this->synthID = synthID;
	init();
}

void MIDITimelineComponent::init()
{
	noteRangeSize = noteRangeEnd - noteRangeStart;
	noteEventMatrix.resize(noteRangeSize);
	clearMatrix();
	this->name = "MIDI Timeline (" +String(this->numMeasures) + " measures)";
	this->initMenu();
	setAudioChannels(0, 2);
	//currentTimeUnit = 2000;
}

void MIDITimelineComponent::clearMatrix()
{
	numQuartersPerMeasure = (4.0f / (float)AppProperties::getDenominator()) * (float)AppProperties::getNumerator();
	numTimeUnitsInMeasure = (int)(numQuartersPerMeasure * 4.0f);

	for (int i = 0; i < this->noteRangeSize; i++)
	{
		noteEventMatrix[i].clear();
		std::vector<NoteEventDesc> vec;
		vec.resize((int)(4.0f * (float)this->numMeasures * numQuartersPerMeasure));
		noteEventMatrix[i] = vec;
	}
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
			double timeUnitDuration = beatDuration / (double)AppProperties::getDenominator();
			//double totalDuration = numberOfTimeUnits * timeUnitDuration;
			double samplesPerTimeUnit = sampleRateInt * timeUnitDuration;
			
			samplesElapsedSincePlay += bufferToFill.numSamples;
			currentTimeUnit = (int)floor((double)samplesElapsedSincePlay / samplesPerTimeUnit);
			
			// If end happened - replay			
			if (currentTimeUnit >= numberOfTimeUnits)
			{
				currentTimeUnit = 0;
				samplesElapsedSincePlay = 0;
				triggerRepaint();
			}
			
			// If change happened
			if (currentTimeUnit != currentTimeUnitSnapshot)
			{
				synths[0]->synth.allNotesOff(0,true);
				for (int i = 0; i < noteEventMatrix.size(); i++)
				{
					if (noteEventMatrix[i][currentTimeUnit].NoteName != "")
					{
						synths[0]->synth.noteOn(0, noteEventMatrix[i][currentTimeUnit].NoteNumber, 0.5f);
					}
				}
				triggerRepaint();
			}
			synths[0]->getNextAudioBlock(bufferToFill);
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
		String midiInfo = "Midi timeformat: " + String(midiFile->getTimeFormat()) + " Last timestamp: " + String(midiFile->getLastTimestamp()) + " Tracks: " + String(midiFile->getNumTracks());
		this->name += midiInfo;
	}
	
	double beatDuration = 60.0 / (double)AppProperties::getTempo();
	double timeUnitDuration = beatDuration / (double)AppProperties::getDenominator();
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
		int ticksPerMeasure = (int)(midiFile->getTimeFormat() * (4.0f / (float)AppProperties::getDenominator()) * (float)AppProperties::getNumerator());
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
			int numberOfTimeUnits = (int)noteEventMatrix[0].size();
			g.drawImage(matrixImage, parentBounds);
			float cursorWidth = parentBounds.getWidth() / numberOfTimeUnits;
			float currentX = ((float)currentTimeUnit / (float)numberOfTimeUnits) * parentBounds.getWidth();
			Rectangle<float> cursorInfoRect(currentX + cursorWidth, parentBounds.getTopRight().y + 15.0f, 40.0f, 15.0f);
			g.setColour(Colour::fromRGB(128, 128, 128));
			g.fillRect(currentX, parentBounds.getTopLeft().y, cursorWidth, parentBounds.getHeight());
			g.setColour(juce::Colours::darkgrey);
			g.fillRect(cursorInfoRect);
			int currentMeasureIndex = (int)floor((float)numMeasures * ((float)currentTimeUnit / (float)numberOfTimeUnits));
			int currentTimeUnitWithinMeasureIndex = (currentMeasureIndex * numTimeUnitsInMeasure + currentTimeUnit) % (numTimeUnitsInMeasure);
			g.setColour(juce::Colours::white);
			g.setFont(12.0f);
			String cursorText = String::formatted("%02d", currentMeasureIndex + 1) + "|" + String::formatted("%02d", currentTimeUnitWithinMeasureIndex + 1);
			
			g.drawText(cursorText, cursorInfoRect, juce::Justification::centred, true);
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
				currentTimeUnit = 0;
				midiFile = std::make_unique<MidiFile>();
				std::unique_ptr<FileInputStream> str = file.createInputStream();
				midiFile->readFrom(*str);
				projectName = file.getFullPathName();
				name = file.getFileName() + " - Midi timeformat: " + String(midiFile->getTimeFormat()) + ", last timestamp: " + String(midiFile->getLastTimestamp()) + ", tracks: " + String(midiFile->getNumTracks() + ", tempo: " + String(AppProperties::getTempo()));
				processMidi();
			}
		});
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
		double maxMatrixTicksCapacity = (double)numMeasures * numQuartersPerMeasure * (double)midiFile->getTimeFormat();

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

						
						int ticksPerMeasure = (int)(midiFile->getTimeFormat() * (4.0f / (float)AppProperties::getDenominator()) * (float)AppProperties::getNumerator());

						double ticksPerTimeUnit = ((double)ticksPerMeasure / (double)AppProperties::getNumerator()) / (double)AppProperties::getDenominator();
						double totalDurationTicks = matrixWidth * ticksPerTimeUnit;

						int duration = roundToInt((noteEnd - noteStart) / ticksPerTimeUnit);

						int column = (int)floor(((noteStart / totalDurationTicks) * (double)matrixWidth));
						if (noteNumber >= noteRangeStart && noteNumber <= noteRangeEnd && column < matrixWidth)
						{
							NoteEventDesc nEvent(midiMessage.getMidiNoteName(noteNumber, true, false, 4), noteNumber, duration);
							noteEventMatrix[noteNumber - noteRangeStart][column] = nEvent;
						}
					}
				}
			}
			//---------------------
		}
		if (projectName == "")
		{
			projectName = "New Project";
		}

		FileOutputStream stream = FileOutputStream(File(AppProperties::getProjectPath() +  projectName + "." + String(Time::currentTimeMillis()) + ".proj"));

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

		this->initMenu();
		setComponentSize();
		repaintMatrixImage();
		FileOutputStream str(File(AppProperties::getProjectPath() + projectName + "." + String(Time::currentTimeMillis()) + ".png"));
		PNGImageFormat pngWriter;
		pngWriter.writeImageToStream(matrixImage, str);
	}
}

void MIDITimelineComponent::setComponentSize()
{
	int minCellWidth = 5;
	Rectangle<float> localBounds = getParentComponent()->getLocalBounds().toFloat();
	if (getWidth()>minSize)
	{
		localBounds = getBounds().toFloat();
	}

	if (noteEventMatrix.size() > 0)
	{
		int numberOfTimeUnits = (int)noteEventMatrix[0].size();
		setBounds((int)localBounds.getX(), (int)localBounds.getY(), minCellWidth * numberOfTimeUnits, (int)(localBounds.getHeight()));
	}
	if(midiFile)
	{
		float trackHeight = (localBounds.getHeight() * (1 - this->timelineHeightRatio)) / (midiTracks.size());
		setBounds((int)localBounds.getX(), (int)localBounds.getY(), (int)localBounds.getWidth(), (int)trackHeight * midiTracks.size() + (int)(localBounds.getHeight() * (this->timelineHeightRatio)));
	}
	repaint();
}

void MIDITimelineComponent::initMenu()
{
	this->menu.clear();
	this->menu.addItem("Load MIDI", std::bind(&MIDITimelineComponent::loadMIDI, this));

	if (noteEventMatrix.size()>0)
	{
		this->menu.addItem("Play", std::bind(&MIDITimelineComponent::playMIDI, this));
		this->menu.addItem("Stop", std::bind(&MIDITimelineComponent::stopMIDI, this));
		this->menu.addItem("Clear", std::bind(&MIDITimelineComponent::clearTimeline, this));
	}
}

void MIDITimelineComponent::clearTimeline()
{
	clearMatrix();
	repaintMatrixImage();
}

void MIDITimelineComponent::playMIDI()
{
	isPlaying = true;
}

void MIDITimelineComponent::stopMIDI()
{
	isPlaying = false;
	samplesElapsedSincePlay = 0;
	if (synths.size() > 0)
	{
		synths[0]->synth.allNotesOff(0, true);
	}
}

void MIDITimelineComponent::repaintMatrixImage()
{
	Rectangle<int> parentBounds = getReducedLocalBounds();
	parentBounds.setWidth(2 * parentBounds.getWidth());
	parentBounds.setHeight(2 * parentBounds.getHeight());

	if (parentBounds.getWidth() > 16000)
	{
		parentBounds.setWidth(16000);
	}

	matrixImage = juce::Image(juce::Image::RGB, parentBounds.getWidth(), parentBounds.getHeight(), true);

	Graphics g0(matrixImage);
	g0.fillAll(Colours::black);

	if (noteEventMatrix.size() > 0)
	{
		int numberOfTimeUnits = (int)noteEventMatrix[0].size();
		g0.setColour(Colours::grey);

		
		for (int i = 0; i < noteRangeSize; i++)
		{
			float currentY = (float)(parentBounds.getHeight() * (((float)noteRangeSize - (float)i -1.0f) / (float)noteRangeSize));
			float currentX = 0.0f;
			float timeUnitWidthPixels = (float)(parentBounds.getWidth() / (float)numberOfTimeUnits);
			Rectangle<float> textBox(0.0f, currentY, timeUnitWidthPixels, (float)(parentBounds.getHeight() / (float)noteRangeSize));
			float fontSize = (float)(0.9f * parentBounds.getHeight() / (float)noteRangeSize);
			g0.setFont(fontSize);
			Rectangle<float> textBoxWider(textBox);
			textBoxWider.setWidth(fontSize * 3.0f);
			g0.drawText(String(noteRangeStart + i), textBoxWider, Justification::left);
			g0.drawLine(0, currentY, (float)parentBounds.getWidth(), currentY, 0.2f);

			for (int j = 0; j < numberOfTimeUnits; j++)
			{
				int currentMeasureIndex = (int)floor((float)numMeasures * ((float)j / (float)numberOfTimeUnits));
				currentX = parentBounds.getWidth() * ((float)j / (float)numberOfTimeUnits);
				textBox.setPosition(Point<float>(currentX, currentY));
				textBoxWider.setPosition(Point<float>(currentX, currentY));
				textBox.setWidth(fontSize * 2);

				if (j % numTimeUnitsInMeasure == 0)
				{
					g0.setColour(Colours::lightgrey);
					g0.drawLine(textBoxWider.getBottomLeft().x, textBoxWider.getBottomLeft().y, textBoxWider.getTopLeft().x, textBoxWider.getTopLeft().y, 1.0f);
					if (i == noteRangeSize-1)
					{
						g0.drawText(String(currentMeasureIndex +1), textBoxWider, Justification::centred);
					}
				}
				else
				{
					if ((currentMeasureIndex * numTimeUnitsInMeasure + j) % (numTimeUnitsInMeasure/(int)numQuartersPerMeasure)== 0)
					{
						if (i < noteRangeSize - 1)
						{
							g0.setColour(Colours::white);
							g0.drawLine(textBoxWider.getBottomLeft().x, textBoxWider.getBottomLeft().y, textBoxWider.getTopLeft().x, textBoxWider.getTopLeft().y, 0.5f);
						}
					}
					else
					{
						g0.setColour(Colours::lightgrey);
						g0.drawLine(textBox.getBottomRight().x, textBox.getBottomRight().y, textBox.getTopRight().x, textBox.getTopRight().y, 0.2f);
					}
				}
				

				if (noteEventMatrix[i][j].NoteName != "")
				{
					textBox.setWidth(timeUnitWidthPixels * noteEventMatrix[i][j].NoteDuration);
					g0.fillRect(textBox);
					g0.setColour(Colours::darkgrey);
					g0.drawText(String(noteEventMatrix[i][j].NoteName), textBox, Justification::left);
				}
			}
		}
		
		repaint();
	}
}



void MIDITimelineComponent::changeListenerCallback(ChangeBroadcaster* /*source*/)
{

}

void MIDITimelineComponent::handleAsyncUpdate()
{
	repaint();
}

void MIDITimelineComponent::triggerRepaint()
{
	triggerAsyncUpdate();
}
