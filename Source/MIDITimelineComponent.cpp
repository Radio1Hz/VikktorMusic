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

MIDITimelineComponent::MIDITimelineComponent(int nM)
{
	numMeasures = nM;
	name = "MIDI Timeline (" + String(numMeasures) + " measures)";
	synthID = 0;
	defaultMIDIChannel = 1;
	currentCursorPosition.resize(2);
	currentCursorPosition[0] = -1;
	currentCursorPosition[1] = -1;
	musicMath.setNoteRange(32, 92);
	notesOffInFuture.resize(musicMath.getNoteRangeSize());
	formatManager.registerBasicFormats();
	clearNoteEventMatrix();
}

//Called after its addAndMakeVisible()
void MIDITimelineComponent::init()
{
	setAudioChannels(0, 2);
	MidiDeviceInfo defaultDevice = MidiOutput::getDefaultDevice();
	Array<MidiDeviceInfo> availableDevices = MidiOutput::getAvailableDevices();

	for (MidiDeviceInfo di : availableDevices)
	{
		if (di.name == "loopMIDI Port")
		{
			midiOutput = MidiOutput::openDevice(di.identifier);
		}
	}

	if (midiOutput == nullptr)
	{
		midiOutput = MidiOutput::openDevice(defaultDevice.identifier);
	}

	// Init audioBuffer to 20sec
	audioBuffer.setSize(2, 20 * (int)sampleRateInt, false);
	repaintMatrixImage();
	setComponentSize();
	initMenu();

	repaint();
}

void MIDITimelineComponent::clearNoteEventMatrix()
{
	numQuartersPerMeasure = 4.0f * (float)AppProperties::getNumerator() / (float)AppProperties::getDenominator();
	numTimeUnitsPerMeasure = (int)(numQuartersPerMeasure * 4.0f);
	noteEventMatrix.resize(musicMath.getNoteRangeSize());

	for (int i = 0; i < this->musicMath.getNoteRangeSize(); i++)
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
	if (midiOutput != nullptr)
		midiOutput->stopBackgroundThread();
	shutdownAudio();
}

void MIDITimelineComponent::mouseMoveEvent(const MouseEvent& event)
{
	//If cursor is within reduced region
	if (event.y > headerHeight + 1)
	{
		int overRowScreen = (int)((double)(musicMath.getNoteRangeSize()) * ((double)(event.y - headerHeight - 1) / ((double)getReducedLocalBounds().getHeight())));
		int overTimeUnit = (int)((double)(numTimeUnitsPerMeasure * numMeasures) * (((double)event.x - 1.0) / (double)getReducedLocalBounds().getWidth()));

		currentCursorPosition[0] = overRowScreen;
		currentCursorPosition[1] = overTimeUnit;

		repaint();
	}

}

void MIDITimelineComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	samplesPerBlockExpectedInt = samplesPerBlockExpected;
	sampleRateInt = sampleRate;

	audioSource = make_unique<SynthAudioSource>(8, synthID, sampleRateInt);
	audioSource->prepareToPlay(samplesPerBlockExpectedInt, sampleRateInt);
}

void MIDITimelineComponent::scanPlugins()
{

}

void MIDITimelineComponent::getNextAudioBlock(const AudioSourceChannelInfo& bufferToFill)
{
	if (isPlaying)
	{
		//Check Midi Messages
		if (audioSource != nullptr)
		{
			bool produceAudio = AppProperties::getInternalSynthAudioOut();
			bool produceMIDI = AppProperties::getMIDIOut();

			int numberOfTimeUnits = (int)noteEventMatrix[0].size();
			int currentTimeUnitSnapshot = currentTimeUnit;

			//Duration of one quarter. 
			//Time units are sexteenths and there are [numerator] units in one beat
			double beatDuration = 60.0 / (double)AppProperties::getTempo();
			double timeUnitDuration = beatDuration / 4.0;
			double samplesPerTimeUnit = sampleRateInt * timeUnitDuration;

			int currentTimeUnitPredicted = (int)floor((double)sampleIndex / samplesPerTimeUnit);

			// If end happened - replay			
			if (currentTimeUnitPredicted >= numberOfTimeUnits)
			{
				currentTimeUnit = 0;
				sampleIndex = 0;

				if (produceAudio)
				{
					audioSource->synth.allNotesOff(defaultMIDIChannel, true);
				}

				if (midiOutput != nullptr && produceMIDI)
				{
					MidiMessage noteOff(MidiMessage::allNotesOff(defaultMIDIChannel));
					midiOutput->sendMessageNow(noteOff);
				}
				triggerRepaint();
			}

			// If change happened
			if (currentTimeUnitPredicted > currentTimeUnitSnapshot)
			{
				for (int i = 0; i < noteEventMatrix.size(); i++)
				{
					//Check if some notes needs to be turned off
					for (int n = 0; n < notesOffInFuture.size(); n++)
					{
						if (notesOffInFuture[n] > 0 && notesOffInFuture[n] < samplesElapsed)
						{
							if (produceAudio)
							{
								audioSource->synth.noteOff(defaultMIDIChannel, n + musicMath.getNoteRangeStart(), 0.0f, true);
							}

							if (midiOutput != nullptr && produceMIDI)
							{
								MidiMessage noteOff(MidiMessage::noteOff(defaultMIDIChannel, n + musicMath.getNoteRangeStart(), (uint8)0));
								midiOutput->sendMessageNow(noteOff);
							}
							notesOffInFuture.set(n, 0);
						}
					}

					if (noteEventMatrix[i][currentTimeUnit].EventType == NoteEventType::NoteOn)
					{
						int playingNoteNumber = noteEventMatrix[i][currentTimeUnit].NoteNumber;

						//Note On
						if (produceAudio)
						{
							audioSource->synth.noteOn(defaultMIDIChannel, playingNoteNumber, 0.5f);
						}

						if (midiOutput != nullptr && produceMIDI)
						{
							MidiMessage noteOn(MidiMessage::noteOn(defaultMIDIChannel, noteEventMatrix[i][currentTimeUnit].NoteNumber, (uint8)128));
							midiOutput->sendMessageNow(noteOn);
						}

						//Send Note Off into Future
						int playingNoteDuration = noteEventMatrix[i][currentTimeUnit].NoteDuration; // In TimeUnits
						int playingNoteDurationInSamples = playingNoteDuration * (int)samplesPerTimeUnit;
						notesOffInFuture.set(playingNoteNumber - musicMath.getNoteRangeStart(), samplesElapsed + playingNoteDurationInSamples);
					}
				}
				currentTimeUnit++;
				//If Loop
				if (loopCellStart > -1)
				{
					if (currentTimeUnit >= loopCellStart + loopWidthInTimeUnits)
					{
						currentTimeUnit = loopCellStart;
						sampleIndex = (int)samplesPerTimeUnit * loopCellStart;
					}
				}
				triggerRepaint();
			}

			audioSource->getNextAudioBlock(bufferToFill);

			if (AppProperties::getShouldSaveAudio())
			{
				//Transfer and append samples from bufferToFill to audioBuffer
				int numSamplesInBuffer = audioBuffer.getNumSamples();
				for (int i = 0; i < bufferToFill.numSamples; i++)
				{
					audioBuffer.setSample(0, audioBufferSampleIndex, bufferToFill.buffer->getSample(0, i));
					audioBuffer.setSample(1, audioBufferSampleIndex, bufferToFill.buffer->getSample(1, i));

					audioBufferSampleIndex++;

					if (audioBufferSampleIndex >= numSamplesInBuffer)
					{
						audioBufferSampleIndex = 0;
					}
				}
			}
			sampleIndex += bufferToFill.numSamples;
			samplesElapsed += bufferToFill.numSamples;
		}
	}
}

void MIDITimelineComponent::releaseResources()
{
	audioSource->releaseResources();
}

void MIDITimelineComponent::paint(Graphics& g)
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
	double totalDurationSec = (timeUnitDuration * (double)numMeasures * (double)numTimeUnitsPerMeasure);
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
			g.setColour(Colours::grey);
			if (i < midiTracks.size())
			{
				g.drawLine(0.0f, trackRect.getTopLeft().y, trackRect.getWidth(), trackRect.getTopLeft().y);
			}

			g.setFont(trackHeight / 25.0f);
			g.drawText("Track " + String(i + 1), trackRect, Justification::topLeft, true);
			drawMIDIEvents(trackRect, i, g);
			trackRect.translate(0, (float)trackHeight);
		}
	}
	else
	{
		if (noteEventMatrix.size() > 0)
		{
			vector<vector<vector<ContextDesc>>> contextPerMeasureAndQuarterVector = AppProperties::getContextPerMeasureAndQuarterVector();
			vector<vector<ContextDesc>> contextPerMeasureVector = AppProperties::getContextPerMeasureVector();
			float measureWidthInPixels = parentBounds.getWidth() / (float)numMeasures;
			float timeUnitWidthInPixels = parentBounds.getWidth() / ((float)numMeasures * (float)numTimeUnitsPerMeasure);
			int numberOfTimeUnits = (int)noteEventMatrix[0].size();
			g.drawImage(matrixImage, parentBounds);
			float cursorWidth = parentBounds.getWidth() / numberOfTimeUnits;
			float currentX = ((float)currentTimeUnit / (float)numberOfTimeUnits) * parentBounds.getWidth();
			Rectangle<float> cursorTimeUnitInfoRect(currentX + cursorWidth, parentBounds.getTopRight().y + 15.0f, 40.0f, 15.0f);
			g.setColour(Colour::fromRGBA(128, 128, 128, 128));
			g.fillRect(currentX, parentBounds.getTopLeft().y, cursorWidth, parentBounds.getHeight());
			g.fillRect(cursorTimeUnitInfoRect);
			int currentMeasureIndex = (int)floor((float)numMeasures * ((float)currentTimeUnit / (float)numberOfTimeUnits));
			int currentTimeUnitWithinMeasureIndex = (currentMeasureIndex * numTimeUnitsPerMeasure + currentTimeUnit) % (numTimeUnitsPerMeasure);
			g.setColour(Colours::white);
			g.setFont(12.0f);
			String currentTimeUnitFormattedText = String::formatted("%02d", currentMeasureIndex + 1) + "|" + String::formatted("%02d", currentTimeUnitWithinMeasureIndex + 1);

			g.drawText(currentTimeUnitFormattedText, cursorTimeUnitInfoRect, Justification::centred, true);

			//Draw Contexts probabilities per measure and quarters
			if (measureWidthInPixels > 80)
			{
				g.setFont(10.0f);
				Rectangle<int> relativeScreenRect = getScreenBounds();

				for (int currMeasure = 0; currMeasure < contextPerMeasureAndQuarterVector.size(); currMeasure++)
				{
					if (contextPerMeasureAndQuarterVector[currMeasure].size() > 0)
					{
						Rectangle<float> measureQuarterTonalityRect(measureWidthInPixels, contextPerMeasureAndQuarterVector[currMeasure][0].size() * 10.0f);
						Rectangle<float> measureTonalityRect(measureWidthInPixels, contextPerMeasureVector[currMeasure].size() * 10.0f);
						int xPosition = currMeasure * (int)measureWidthInPixels + (int)timeUnitWidthInPixels;

						int screenWidth = getParentComponent()->getWidth();

						int currentScreen = (int)(xPosition / screenWidth);
						int currentOffsetWithinScreen = xPosition % screenWidth;

						if (relativeScreenRect.getTopLeft().x + (currentScreen * screenWidth) + currentOffsetWithinScreen > 0 && relativeScreenRect.getTopLeft().x + (currentScreen * screenWidth) + currentOffsetWithinScreen < screenWidth)
						{
							for (float q = 0; q < ceil(numQuartersPerMeasure); q += 1.0f)
							{
								measureQuarterTonalityRect.setPosition(((float)currMeasure + q / numQuartersPerMeasure) * measureWidthInPixels + timeUnitWidthInPixels, 2 * timeUnitWidthInPixels + (float)headerHeight + 1.0f);
								measureTonalityRect.setPosition(((float)currMeasure) * measureWidthInPixels + timeUnitWidthInPixels, 2 * timeUnitWidthInPixels + (float)headerHeight + 1.0f);
								String text = "";
								for (int t = 0; t < contextPerMeasureAndQuarterVector[currMeasure][(int)q].size(); t++)
								{
									if (t == 0)
									{
										text += contextPerMeasureAndQuarterVector[currMeasure][(int)q][t].friendlyName();
									}
									else
									{
										text += "\r\n" + contextPerMeasureAndQuarterVector[currMeasure][(int)q][t].friendlyName();
									}
								}
								if (q == 0)
								{
									if (contextPerMeasureVector[currMeasure].size() > 0)
									{
										text += "\r\n*" + contextPerMeasureVector[currMeasure][0].friendlyName() + "*";
									}
								}
								g.drawMultiLineText(text, (int)measureTonalityRect.getTopLeft().x, (int)measureTonalityRect.getTopLeft().y, (int)measureWidthInPixels, Justification::left);
							}
						}
					}
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

			//Draw Cursor Info Position
			if (currentCursorPosition[0] > -1 && currentCursorPosition[1] > -1)
			{
				int cMeasureIndex = (int)floor((float)numMeasures * ((float)currentCursorPosition[1] / (float)numberOfTimeUnits));
				int cTimeUnitWithinMeasureIndex = currentCursorPosition[1] % numTimeUnitsPerMeasure;
				String cTimeUnitFormattedText = String::formatted("%02d", cMeasureIndex + 1) + "|" + String::formatted("%02d", cTimeUnitWithinMeasureIndex + 1);

				float cWidth = parentBounds.getWidth() / numberOfTimeUnits;
				float cHeight = parentBounds.getHeight() / musicMath.getNoteRangeSize();
				Rectangle<float> currentScreenCursorPositionRect(cWidth, cHeight);
				currentScreenCursorPositionRect.setPosition(currentCursorPosition[1] * cWidth + 1, currentCursorPosition[0] * cHeight + headerHeight + 1);
				g.setColour(Colour::fromRGBA(192, 192, 192, 128));
				g.fillRect(currentScreenCursorPositionRect);

				float minWidth = 50.0f;
				float cursorInfoWidth = cWidth * 4;
				float cursorInfoHeight = cWidth * 3;

				float aspectRatio = cursorInfoWidth / cursorInfoHeight;
				if (cursorInfoWidth < minWidth)
				{
					cursorInfoWidth = minWidth;
					cursorInfoHeight = minWidth / aspectRatio;
				}

				Rectangle<float> currentScreenCursorInfoRect(cursorInfoWidth, cursorInfoHeight);
				currentScreenCursorInfoRect.setPosition(((currentCursorPosition[1] + 2) * cWidth) + 1, ((currentCursorPosition[0] + 2) * cHeight) + headerHeight);
				g.setColour(Colour::fromRGBA(20, 20, 20, 255));
				g.fillRect(currentScreenCursorInfoRect);
				g.setColour(Colours::white);
				g.setFont(12.0f);
				g.drawMultiLineText(String(musicMath.getNoteRangeEnd() - currentCursorPosition[0]) + " (" + musicMath.getNoteNameByMIDINoteNumber(musicMath.getNoteRangeEnd() - currentCursorPosition[0]) + ")\r\n" + cTimeUnitFormattedText, (int)currentScreenCursorInfoRect.getTopLeft().x, (int)currentScreenCursorInfoRect.getCentreY(), (int)currentScreenCursorInfoRect.getWidth(), Justification::centred);
			}


		}
	}
}

void MIDITimelineComponent::drawMIDIEvents(Rectangle<float> trackRect, int trackIndex, Graphics& g)
{
	double totalDuration = midiFile->getLastTimestamp();

	for (int i = 0; i < midiTracks[trackIndex]->getNumEvents(); i++)
	{
		MidiMessageSequence::MidiEventHolder* midiEvent = midiTracks[trackIndex]->getEventPointer(i);
		MidiMessage midiMessage = midiEvent->message;

		if (midiMessage.isNoteOn())
		{
			MidiMessageSequence::MidiEventHolder* noteOffHolder = midiEvent->noteOffObject;
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
				g.drawText(MusicMath::getNoteNameByMIDINoteNumber(noteNumber), Rectangle<float>(trackRect.getTopLeft().getX() + pixelStart, trackRect.getTopLeft().getY() + yVel - getFontSize() / 8.0f, getFontSize(), getFontSize() / 8.0f), Justification::topLeft);

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

			if (file != File{})
			{
				clearNoteEventMatrix();
				stopMIDI();
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
	if (audioSource != nullptr)
	{
		audioSource->synth.allNotesOff(defaultMIDIChannel, true);
	}

	if (midiOutput != nullptr)
	{
		MidiMessage noteOff(MidiMessage::allNotesOff(defaultMIDIChannel));
		midiOutput->sendMessageNow(noteOff);
	}
}

void MIDITimelineComponent::numMeasuresChanged()
{
	if (this->numMeasures != AppProperties::getNumMeasures())
	{
		this->numMeasures = AppProperties::getNumMeasures();
		clearNoteEventMatrix();
		repaintMatrixImage();
		setComponentSize();
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
		//double midiTicksPerMeasure = ((double)numTimeUnitsPerMeasure);
		double maxMatrixTicksCapacity = (double)numMeasures * numQuartersPerMeasure * (double)defaultTicksPerQuarterNote;

		for (int trackIndex = 0; trackIndex < midiTracks.size(); trackIndex++)
		{
			for (int eventIndex = 0; eventIndex < midiTracks[trackIndex]->getNumEvents(); eventIndex++)
			{
				MidiMessageSequence::MidiEventHolder* midiEvent = midiTracks[trackIndex]->getEventPointer(eventIndex);
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
					MidiMessageSequence::MidiEventHolder* midiEvent = midiTracks[trackIndex]->getEventPointer(eventIndex);
					MidiMessage midiMessage = midiEvent->message;

					if (midiMessage.isNoteOn())
					{
						MidiMessageSequence::MidiEventHolder* noteOffHolder = midiEvent->noteOffObject;
						int noteNumber = midiMessage.getNoteNumber();
						double noteStart = midiMessage.getTimeStamp();

						double noteEnd = noteStart;

						if (noteOffHolder)
						{
							noteEnd = noteOffHolder->message.getTimeStamp();
						}

						int ticksPerMeasure = (int)((float)defaultTicksPerQuarterNote * (float)numQuartersPerMeasure);

						double ticksPerTimeUnit = ((double)ticksPerMeasure / (double)numQuartersPerMeasure) / 4.0;

						int duration = roundToInt((noteEnd - noteStart) / ticksPerTimeUnit);
						int column = roundToInt(noteStart / ticksPerTimeUnit);

						if (noteNumber >= musicMath.getNoteRangeStart() && noteNumber < musicMath.getNoteRangeEnd() && column < matrixWidth && duration > 0)
						{
							NoteEventDesc nEventOn(noteNumber, duration, 1);
							noteEventMatrix[noteNumber - musicMath.getNoteRangeStart()][column] = nEventOn;
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

	repaint();
}

void MIDITimelineComponent::initMenu()
{
	this->menu.clear();
	this->menu.addSectionHeader("Menu");

	this->menu.addItem("Load MIDI", std::bind(&MIDITimelineComponent::loadMIDI, this));

	if (noteEventMatrix.size() > 0)
	{
		PopupMenu selectionSubMenu;
		selectionSubMenu.addItem("Analyze", std::bind(&MIDITimelineComponent::analyzeContextInSelection, this));
		selectionSubMenu.addItem("Save as .mid", std::bind(&MIDITimelineComponent::saveSelectionAsMIDIFile, this));
		selectionSubMenu.addItem("Clear", std::bind(&MIDITimelineComponent::clearSelection, this));

		PopupMenu generateSubMenu;
		generateSubMenu.addItem("Ramp - Chromatic", std::bind(&MIDITimelineComponent::generateRampChromatic, this));
		if (synthID == 1)
		{
			generateSubMenu.addItem("Generate Rhythm", std::bind(&MIDITimelineComponent::generateRhythm, this));
		}
		if (synthID == 2)
		{
			generateSubMenu.addItem("Generate Contexts", std::bind(&MIDITimelineComponent::generateContexts, this));
		}

		menu.addSeparator();
		menu.addSubMenu("Selection", selectionSubMenu, true);
		menu.addSeparator();

		menu.addSubMenu("Generate", generateSubMenu, true);
		menu.addSeparator();

		menu.addItem("Play", std::bind(&MIDITimelineComponent::playMIDI, this));
		menu.addItem("Loop Selection", std::bind(&MIDITimelineComponent::loopSelection, this));
		menu.addItem("Stop", std::bind(&MIDITimelineComponent::stopMIDI, this));
		menu.addItem("Clear", std::bind(&MIDITimelineComponent::clearTimeline, this));
		menu.addItem("Repaint Matrix", std::bind(&MIDITimelineComponent::repaintMatrixImage, this));
		menu.addItem("Save audioBuffer to disk", std::bind(&MIDITimelineComponent::saveAudioBufferToDisk, this));
		menu.addItem("Save Timeline to MIDI", std::bind(&MIDITimelineComponent::saveMIDIFileToDisk, this));
		menu.addItem("Set Contexts from MIDI events", std::bind(&MIDITimelineComponent::defineAllContextsPerMeasures, this));
		menu.addItem("Delete", std::bind(&MIDITimelineComponent::deleteTimeline, this));
	}
}

void MIDITimelineComponent::populateSelectionMatrix()
{
	selectionMatrix.reset(new Matrix<NoteEventDesc>(musicMath.getNoteRangeSize(), 1 + selectedCellEnd - selectedCellStart));

	for (int j = selectedCellStart; j <= selectedCellEnd; j++)
	{
		for (int i = 0; i < musicMath.getNoteRangeSize(); i++)
		{
			// If there is note
			if (noteEventMatrix[i][j].EventType == NoteEventType::NoteOn)
			{
				selectionMatrix->operator()(i, j - selectedCellStart).NoteNumber = noteEventMatrix[i][j].NoteNumber;
				selectionMatrix->operator()(i, j - selectedCellStart).EventType = noteEventMatrix[i][j].EventType;
				selectionMatrix->operator()(i, j - selectedCellStart).NoteDuration = noteEventMatrix[i][j].NoteDuration;
			}
		}
	}
}

void MIDITimelineComponent::clearTimeline()
{
	clearNoteEventMatrix();
	stopMIDI();
	vector<vector<vector<ContextDesc>>> contextPerMeasureAndQuarterVector(numMeasures);
	vector<vector<ContextDesc>> contextPerMeasureVector = AppProperties::getContextPerMeasureVector();
	contextPerMeasureAndQuarterVector.clear();
	contextPerMeasureAndQuarterVector.resize(numMeasures);
	contextPerMeasureVector.clear();
	contextPerMeasureVector.resize(numMeasures);
	AppProperties::setContextPerMeasureAndQuarterVector(contextPerMeasureAndQuarterVector);
	AppProperties::setContextPerMeasureVector(contextPerMeasureVector);
	repaintMatrixImage();
	repaint();
}

void MIDITimelineComponent::playMIDI()
{
	isPlaying = true;
	notesOffInFuture.fill(0);
	samplesElapsed = 0;
}

void MIDITimelineComponent::stopMIDI()
{
	isPlaying = false;
	notesOffInFuture.fill(0);

	sampleIndex = 0;
	currentTimeUnit = 0;

	loopCellStart = -1;
	loopWidthInTimeUnits = 0;

	samplesElapsed = 0;

	if (audioSource != nullptr)
	{
		audioSource->synth.allNotesOff(defaultMIDIChannel, true);
	}

	if (midiOutput != nullptr)
	{
		if (midiOutput->isBackgroundThreadRunning())
			midiOutput->sendMessageNow(MidiMessage::allNotesOff(defaultMIDIChannel));
	}
}

void MIDITimelineComponent::repaintMatrixImage()
{
	if (noteEventMatrix.size() > 0)
	{
		vector<vector<vector<ContextDesc>>> contextPerMeasureAndQuarterVector = AppProperties::getContextPerMeasureAndQuarterVector();
		vector<vector<ContextDesc>> contextPerMeasureVector = AppProperties::getContextPerMeasureVector();

		Rectangle<int> parentBounds = getReducedLocalBounds();
		Rectangle<int> newImageSize(200, 200);
		float timeUnitWidthPixels = minCellWidth;
		int numberOfTimeUnits = (int)noteEventMatrix[0].size();

		newImageSize.setWidth((int)((float)numberOfTimeUnits * timeUnitWidthPixels));
		newImageSize.setHeight((int)((float)musicMath.getNoteRangeSize() * timeUnitWidthPixels));
		double aspectRatio = newImageSize.getWidth() / newImageSize.getHeight();

		auto maxBitmapSize = 16000;
		if (newImageSize.getWidth() > maxBitmapSize)
		{
			float sizeRatio = (float)maxBitmapSize / (float)newImageSize.getWidth();
			newImageSize.setWidth(maxBitmapSize);
			newImageSize.setHeight((int)((double)maxBitmapSize / aspectRatio));
			timeUnitWidthPixels = minCellWidth * sizeRatio;
		}
		matrixImage = Image(Image::RGB, newImageSize.getWidth(), newImageSize.getHeight(), true);

		Graphics g0(matrixImage);
		g0.setColour(Colours::grey);

		for (int i = 0; i < musicMath.getNoteRangeSize(); i++)
		{
			float currentY = (float)(newImageSize.getHeight() * (((float)musicMath.getNoteRangeSize() - (float)i - 1.0f) / (float)musicMath.getNoteRangeSize()));
			float currentX = 0.0f;

			Rectangle<float> textBox(0.0f, currentY, timeUnitWidthPixels, (float)(newImageSize.getHeight() / (float)musicMath.getNoteRangeSize()));
			Rectangle<float> textBoxWider(textBox);

			float fontSize = (float)(timeUnitWidthPixels);
			g0.setFont(fontSize);

			textBoxWider.setWidth(fontSize * 3.0f);

			g0.drawText(String(musicMath.getNoteRangeStart() + i), textBoxWider, Justification::left);
			g0.drawLine(0.0f, currentY, (float)newImageSize.getWidth(), currentY, 0.2f);

			for (int j = 0; j < numberOfTimeUnits; j++)
			{
				int currentMeasureIndex = (int)floor(j / numTimeUnitsPerMeasure); // roundToInt((float)numMeasures * ((float)j / (float)numberOfTimeUnits));
				int currentQuarterIndex = (int)((j % (int)ceil(numQuartersPerMeasure))); // roundToInt((float)numMeasures * ((float)j / (float)numberOfTimeUnits));
				if (contextPerMeasureAndQuarterVector.size() > 0)
				{
					if (contextPerMeasureAndQuarterVector[currentMeasureIndex].size() > 0 && contextPerMeasureAndQuarterVector[currentMeasureIndex][0].size() <= (int)ceil(numQuartersPerMeasure))
					{
						if (contextPerMeasureAndQuarterVector[currentMeasureIndex][currentQuarterIndex].size() > 0)
						{
							ContextDesc cDesc = contextPerMeasureAndQuarterVector[currentMeasureIndex][currentQuarterIndex][0];
						}
					}

				}
				if (contextPerMeasureVector.size() > 0)
				{
					if (contextPerMeasureVector[currentMeasureIndex].size() > 0)
					{
						ContextDesc cDesc = contextPerMeasureVector[currentMeasureIndex][0];
					}
				}

				currentX = (float)newImageSize.getWidth() * ((float)j / (float)numberOfTimeUnits);

				textBox.setPosition(Point<float>(currentX, currentY));
				textBoxWider.setPosition(Point<float>(currentX, currentY));
				textBox.setWidth(fontSize * 2);

				if (j % numTimeUnitsPerMeasure == 0)
				{
					g0.setColour(Colours::lightgrey);
					g0.drawLine(textBox.getBottomLeft().x, textBox.getBottomLeft().y, textBox.getTopLeft().x, textBox.getTopLeft().y, 1.0f);

					if (i == musicMath.getNoteRangeSize() - 1)
					{
						g0.drawText(String(currentMeasureIndex + 1), textBoxWider, Justification::centred);
					}
				}
				else
				{
					if ((currentMeasureIndex * numTimeUnitsPerMeasure + j) % (numTimeUnitsPerMeasure / (int)numQuartersPerMeasure) == 0)
					{
						if (i < musicMath.getNoteRangeSize() - 1)
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

				if (noteEventMatrix[i][j].EventType == NoteEventType::NoteOn) //if note on
				{
					textBox.setWidth(timeUnitWidthPixels * (float)noteEventMatrix[i][j].NoteDuration);
					g0.setColour(Colours::darkred);

					if (noteEventMatrix[i][j].Generated)
					{
						g0.setColour(Colours::darkkhaki);
					}

					g0.fillRect(textBox);
					g0.setColour(Colours::white);
					String stringToDisplay = String(MusicMath::getNoteNameByMIDINoteNumber(noteEventMatrix[i][j].NoteNumber));
					g0.drawText(stringToDisplay, textBox, Justification::left);
					if (noteEventMatrix[i][j].NoteRole != -1)
					{
						textBox.translate(0, -timeUnitWidthPixels);
						stringToDisplay = "[" + String(noteEventMatrix[i][j].NoteRole + 1) + "]";
						g0.drawText(stringToDisplay, textBox, Justification::left);
					}

				}

				if (noteEventMatrix[i][j].EventType == NoteEventType::NoteOff) //if note off, show white line
				{
					textBox.setWidth(timeUnitWidthPixels * (float)noteEventMatrix[i][j].NoteDuration);
					g0.setColour(Colours::white);
					g0.drawLine(textBox.getBottomLeft().x, textBox.getBottomLeft().y, textBox.getTopLeft().x, textBox.getTopLeft().y, 0.6f);
				}
			}
		}

		// Save in text format
		if (projectName == "")
		{
			projectName = "New Project";
		}

		FileOutputStream stream = FileOutputStream(File(AppProperties::getProjectPath() + projectName + "." + String(Time::currentTimeMillis()) + ".proj"));

		for (int i = 0; i < musicMath.getNoteRangeSize(); i++)
		{
			String str = "";
			for (int j = 0; j < noteEventMatrix[i].size(); j++)
			{
				if (noteEventMatrix[i][j].EventType == NoteEventType::NoteOn)
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
	if (selectedCellStart > -1)
	{
		populateSelectionMatrix();
		list<ContextDesc> allPossibleTonalities = musicMath.getContextDescriptions(noteEventMatrix, selectedCellStart, selectedCellEnd, defaultContextAnalysisMethodID);
		for (ContextDesc& desc : allPossibleTonalities)
		{
			DBG(desc.friendlyName());
		}
	}
}

void MIDITimelineComponent::loopSelection()
{
	if (selectedCellStart > -1)
	{
		// 0 - 15: Width = 16
		int selectionWidth = selectedCellEnd - selectedCellStart + 1;
		stopMIDI();
		currentTimeUnit = selectedCellStart;
		loopCellStart = selectedCellStart;
		loopWidthInTimeUnits = selectionWidth;
		double beatDuration = 60.0 / (double)AppProperties::getTempo();
		double timeUnitDuration = beatDuration / 4.0;
		double samplesPerTimeUnit = sampleRateInt * timeUnitDuration;
		sampleIndex = (int)samplesPerTimeUnit * loopCellStart;
		playMIDI();
		repaint();
	}
}

void MIDITimelineComponent::defineAllContextsPerMeasures()
{
	vector<vector<vector<ContextDesc>>> contextPerMeasureAndQuarterVector = AppProperties::getContextPerMeasureAndQuarterVector();
	vector<vector<ContextDesc>> contextPerMeasureVector = AppProperties::getContextPerMeasureVector();
	contextPerMeasureAndQuarterVector.clear();
	contextPerMeasureAndQuarterVector.resize(numMeasures);
	contextPerMeasureVector.clear();
	contextPerMeasureVector.resize(numMeasures);

	bool shouldDefinePerQuarters = true;
	vector<ContextDesc> prevDesc(1);
	for (int z = 0; z < numMeasures; z++)
	{

		if (contextPerMeasureAndQuarterVector[z].empty())
		{
			contextPerMeasureAndQuarterVector[z] = vector<vector<ContextDesc>>(4);
		}

		for (float q = 0; q < ceil(numQuartersPerMeasure); q += 1.0f)
		{
			int pseudoSelectedCellStart = z * numTimeUnitsPerMeasure + (int)(q * ((float)numTimeUnitsPerMeasure / (float)ceil(numQuartersPerMeasure)));
			int pseudoSelectedCellEnd = 0;
			if (q > floor(numQuartersPerMeasure) - 1)
			{
				pseudoSelectedCellEnd = z * numTimeUnitsPerMeasure + (numTimeUnitsPerMeasure - (int)((float)ceil(numQuartersPerMeasure) / 4.0f));
			}
			else
			{
				pseudoSelectedCellEnd = z * numTimeUnitsPerMeasure + (int)((q + 1.0f) * ((float)numTimeUnitsPerMeasure / (float)ceil(numQuartersPerMeasure))) - 1;
			}

			if (shouldDefinePerQuarters)
			{
				list<ContextDesc> allPossiblieTonalities = musicMath.getContextDescriptions(noteEventMatrix, pseudoSelectedCellStart, pseudoSelectedCellEnd, defaultContextAnalysisMethodID);
				if (allPossiblieTonalities.size() > 0)
				{
					vector<ContextDesc> vec(1);
					vec[0] = allPossiblieTonalities.front();
					contextPerMeasureAndQuarterVector[z][(int)q] = vec;
					prevDesc = vec;
				}
				else
				{
					contextPerMeasureAndQuarterVector[z][(int)q] = prevDesc;
				}
			}

			if (q == 0)
			{
				pseudoSelectedCellEnd = (z + 1) * numTimeUnitsPerMeasure - 1;
				list<ContextDesc> allPossiblieTonalitiesMeasure = musicMath.getContextDescriptions(noteEventMatrix, pseudoSelectedCellStart, pseudoSelectedCellEnd, defaultContextAnalysisMethodID);
				if (allPossiblieTonalitiesMeasure.size() > 0)
				{
					if (contextPerMeasureVector[z].empty())
					{
						std::vector<ContextDesc> vec;
						contextPerMeasureVector[z] = vec;
					}
					for (ContextDesc& i : allPossiblieTonalitiesMeasure)
					{
						contextPerMeasureVector[z].push_back(i);
					}
				}
			}
		}
	}
	AppProperties::setContextPerMeasureAndQuarterVector(contextPerMeasureAndQuarterVector);
	AppProperties::setContextPerMeasureVector(contextPerMeasureVector);
}

void MIDITimelineComponent::operationOnSelection01()
{

}

void MIDITimelineComponent::saveAudioBufferToDisk()
{
	String wavPath = AppProperties::getProjectPath() + projectName + " audioBuffer " + String(Time::currentTimeMillis()) + ".wav";
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

			//No need for defaultMIDIChannel because we save it to disk.
			if (noteInMatrix.EventType == NoteEventType::NoteOn)
			{
				MidiMessage msg = MidiMessage::noteOn(1, noteInMatrix.NoteNumber, (uint8)128);
				msg.setTimeStamp(currentMIDITimestamp);
				seq.addEvent(msg);

				double offTimestamp = currentMIDITimestamp + ((double)defaultTicksPerQuarterNote / 4.0) * (double)noteInMatrix.NoteDuration - 1;
				MidiMessage msgOff = MidiMessage::noteOff(1, noteInMatrix.NoteNumber, (uint8)0);
				msgOff.setTimeStamp(offTimestamp);
				seq.addEvent(msgOff);
			}
		}
	}
	seq.updateMatchedPairs();

	MidiFile outputFile;
	outputFile.setTicksPerQuarterNote(defaultTicksPerQuarterNote);
	outputFile.addTrack(seq);
	FileOutputStream outStr(File(AppProperties::getProjectPath() + projectName + "_out." + String(Time::currentTimeMillis()) + ".mid"));
	outputFile.writeTo(outStr);
	outStr.flush();
}

void MIDITimelineComponent::saveSelectionAsMIDIFile()
{
	if (selectedCellStart > -1)
	{
		populateSelectionMatrix();
		MidiMessageSequence seq;

		MidiMessage msgKeySig = MidiMessage::keySignatureMetaEvent(0, false); // Cmaj
		MidiMessage msgTimeSig = MidiMessage::timeSignatureMetaEvent(AppProperties::getNumerator(), AppProperties::getDenominator()); // Cmaj
		msgKeySig.setTimeStamp(0);
		msgTimeSig.setTimeStamp(0);
		seq.addEvent(msgKeySig);
		seq.addEvent(msgTimeSig);

		float totalDurationInMIDITicks = (float)selectionMatrix->getNumColumns() * (float)defaultTicksPerQuarterNote / 4.0f;
		for (int j = selectedCellStart; j <= selectedCellEnd; j++)
		{
			for (int i = musicMath.getNoteRangeStart(); i < musicMath.getNoteRangeEnd(); i++)
			{
				double currentMIDITimestamp = ((float)(j - selectedCellStart) / (float)selectionMatrix->getNumColumns()) * totalDurationInMIDITicks;
				NoteEventDesc noteInMatrix = noteEventMatrix[i - musicMath.getNoteRangeStart()][j];

				if (noteInMatrix.EventType == NoteEventType::NoteOn)
				{
					MidiMessage msg = MidiMessage::noteOn(1, noteInMatrix.NoteNumber, (uint8)128);
					msg.setTimeStamp(currentMIDITimestamp);
					seq.addEvent(msg);

					double offTimestamp = currentMIDITimestamp + ((double)defaultTicksPerQuarterNote / 4.0) * (double)noteInMatrix.NoteDuration - 1;
					MidiMessage msgOff = MidiMessage::noteOff(1, noteInMatrix.NoteNumber, (uint8)0);
					msgOff.setTimeStamp(offTimestamp);
					seq.addEvent(msgOff);
				}
			}
		}
		seq.updateMatchedPairs();
		MidiFile outputFile;
		const String outFileName = AppProperties::getProjectPath() + projectName + ".selection." + String(selectedCellStart) + "-" + String(selectedCellEnd) + "." + String(Time::currentTimeMillis()) + ".mid";
		outputFile.addTrack(seq);
		outputFile.setTicksPerQuarterNote(defaultTicksPerQuarterNote);

		FileOutputStream outStr(File(outFileName + ""));
		outputFile.writeTo(outStr);
		outStr.flush();
	}
}

void MIDITimelineComponent::clearSelection()
{
	if (selectedCellStart > -1)
	{
		for (int j = selectedCellStart; j <= selectedCellEnd; j++)
		{
			for (int i = musicMath.getNoteRangeStart(); i < musicMath.getNoteRangeEnd(); i++)
			{
				NoteEventDesc currentDesc = noteEventMatrix[i - musicMath.getNoteRangeStart()][j];
				if (currentDesc.EventType == NoteEventType::NoteOn)
				{
					noteEventMatrix[i - musicMath.getNoteRangeStart()][j].EventType = NoteEventType::NoEvent;
					noteEventMatrix[i - musicMath.getNoteRangeStart()][j].NoteNumber = 0;
					noteEventMatrix[i - musicMath.getNoteRangeStart()][j].NoteDuration = 0;
				}

				if (currentDesc.EventType == NoteEventType::NoteOff)
				{
					noteEventMatrix[i - musicMath.getNoteRangeStart()][j].EventType = NoteEventType::NoEvent;
					noteEventMatrix[i - musicMath.getNoteRangeStart()][j].NoteNumber = 0;
					noteEventMatrix[i - musicMath.getNoteRangeStart()][j].NoteDuration = 0;
				}
			}
		}

		repaintMatrixImage();
		repaint();
	}
}

void MIDITimelineComponent::generateRampChromatic()
{
	if (selectedCellStart > -1)
	{
		int currentNote = musicMath.getNoteRangeStart();
		for (int i = selectedCellStart; i <= selectedCellEnd; i++)
		{
			noteEventMatrix[currentNote - musicMath.getNoteRangeStart()][i].EventType = NoteEventType::NoteOn;
			noteEventMatrix[currentNote - musicMath.getNoteRangeStart()][i].NoteNumber = currentNote;
			noteEventMatrix[currentNote - musicMath.getNoteRangeStart()][i].NoteDuration = 1;

			currentNote++;

			if (currentNote >= musicMath.getNoteRangeEnd())
			{
				currentNote = musicMath.getNoteRangeStart();
			}
		}
		repaintMatrixImage();
		repaint();
	}
}

void MIDITimelineComponent::generateRhythm()
{

}

void MIDITimelineComponent::generateContexts()
{
}


void MIDITimelineComponent::shiftDragEvent(const MouseEvent& event)
{
	int currentCell = (int)((double)(numTimeUnitsPerMeasure * numMeasures) * ((double)event.x / (double)getLocalBounds().getWidth()));
	if (selectedCellStart == -1)
	{
		selectedCellStart = currentCell;
	}

	selectedCellEnd = currentCell;

	if (selectedCellEnd > numTimeUnitsPerMeasure * numMeasures - 1)
		selectedCellEnd = numTimeUnitsPerMeasure * numMeasures - 1;

	repaint();
}

void MIDITimelineComponent::shiftMouseDownEvent(const MouseEvent& event)
{
	if (!selectionInProgress)
	{
		selectionInProgress = true;
		int currentCell = (int)((double)(numTimeUnitsPerMeasure * numMeasures) * ((double)event.x / (double)getLocalBounds().getWidth()));
		selectedCellStart = currentCell;
		selectedCellEnd = currentCell;
	}
}

void MIDITimelineComponent::shiftMouseUpEvent(const MouseEvent& /*event*/)
{
	selectionInProgress = false;
}

void MIDITimelineComponent::mouseDoubleClickEvent(const MouseEvent& event)
{
	selectionInProgress = false;
	selectedCellStart = -1;
	selectedCellEnd = -1;

	int currentCell = (int)((double)(numTimeUnitsPerMeasure * numMeasures) * ((double)event.x / (double)getLocalBounds().getWidth()));
	currentTimeUnit = currentCell;
	double beatDuration = 60.0 / (double)AppProperties::getTempo();
	double timeUnitDuration = beatDuration / 4.0;
	double samplesPerTimeUnit = sampleRateInt * timeUnitDuration;
	sampleIndex = (int)samplesPerTimeUnit * currentCell;
	if (audioSource != nullptr)
	{
		audioSource->synth.allNotesOff(defaultMIDIChannel, true);
	}
	repaint();
}

void MIDITimelineComponent::mouseDownEvent(const MouseEvent& /*event*/)
{

}

void MIDITimelineComponent::controlMouseDownEvent(const MouseEvent& /*event*/)
{
	midiOutput->sendMessageNow(MidiMessage::noteOn(defaultMIDIChannel, musicMath.getNoteRangeEnd() - currentCursorPosition[0], (uint8)255));
	audioSource->synth.noteOn(defaultMIDIChannel, musicMath.getNoteRangeEnd() - currentCursorPosition[0], 1.0f);
}

void MIDITimelineComponent::mouseUpEvent(const MouseEvent& /*event*/)
{
	midiOutput->sendMessageNow(MidiMessage::noteOff(defaultMIDIChannel, musicMath.getNoteRangeEnd() - currentCursorPosition[0], (uint8)255));
	audioSource->synth.noteOff(defaultMIDIChannel, musicMath.getNoteRangeEnd() - currentCursorPosition[0], 1.0f, true);
}

void MIDITimelineComponent::triggerRepaint()
{
	triggerAsyncUpdate();
}

void MIDITimelineComponent::deleteTimeline()
{
	((LogSpaceComponent*)getParentComponent())->deleteMIDITimelineComponent(this);
}
