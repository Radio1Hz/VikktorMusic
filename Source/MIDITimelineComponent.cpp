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

using namespace juce;

//==============================================================================
MIDITimelineComponent::MIDITimelineComponent()
{
	// In your constructor, you should add any child components, and
	// initialise any special settings that your component needs.
	this->noteProbabilities.assign(12, 0.0f);
	this->name = "MIDI Timeline";
	this->menu.clear();
	this->menu.addItem("Load MIDI", std::bind(&MIDITimelineComponent::loadMIDI, this));
	setAudioChannels(0, 2);
	//addMouseListener(this, true);
}

MIDITimelineComponent::~MIDITimelineComponent()
{
	removeMouseListener(this);
	removeAllChildren();
	deleteAllChildren();
	shutdownAudio();
}

void MIDITimelineComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	this->samplesPerBlockExpectedInt = samplesPerBlockExpected;
	this->sampleRateInt = sampleRate;
}

void MIDITimelineComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
	if(!synths.isEmpty())
	synths[0]->getNextAudioBlock(bufferToFill);
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
	drawOutline(g);

	if (midiFile != nullptr)
	{
		g.setColour(Colour::fromRGB(35, 35, 35));
		Rectangle<float> parentBounds = getReducedLocalBounds().toFloat();
		double totalDuration = midiFile->getLastTimestamp();
		int numMeasures = 0;
		if (this->denominator && this->numerator)
		{
			//measure length
			g.setFont(getFontSize());
			int ticksPerMeasure = (int)(midiFile->getTimeFormat() * (4.0f / (float)this->denominator) * (float)this->numerator);
			numMeasures = roundToInt(totalDuration / (double)ticksPerMeasure);
			for (int measureIndex = 0; measureIndex < numMeasures; measureIndex++)
			{
				float x = (float)(measureIndex * ticksPerMeasure * parentBounds.getWidth() / totalDuration);
				g.setColour(Colour::fromRGB(30, 30, 30));
				g.drawLine(parentBounds.getTopLeft().getX() + x, parentBounds.getTopLeft().getY(), parentBounds.getTopLeft().getX() + x, parentBounds.getBottomRight().getY(), 1.0f);

				g.setColour(Colours::lightgrey);
				g.drawText(String(measureIndex+1), Rectangle<float>(parentBounds.getTopLeft().getX() + x + 2, parentBounds.getTopLeft().getY() + 2, parentBounds.getWidth()/numMeasures, 15.0f), Justification::topLeft);
			}
		}

		float trackHeight = (parentBounds.getHeight() *(1.0f - this->timelineHeightRatio)) / midiTracks.size();
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
	if(this-measureMatrices.size()>0 && midiFile)
	{ 
		int width = getReducedBounds().getWidth();
		int height = getReducedBounds().getHeight();
		int minimumSize = jmin<int>((int)(height * this->timelineHeightRatio), width / this->measureMatrices.size());
		Rectangle<int> rect(minimumSize, minimumSize);
		rect.translate(1, headerHeight);
		for (auto matrix : this->measureMatrices)
		{
			matrix->setBounds(rect);
			rect.translate((int)((float)width / (float)this->measureMatrices.size()), 0);
		}
	}
	
}

void MIDITimelineComponent::loadMIDI()
{
	fileChooser = std::make_unique<FileChooser>("Please select the .mid file you want to load...", File("C:\\Data\\Music\\Midis\\test"), "*.mid");

	auto folderChooserFlags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectFiles;

	fileChooser->launchAsync(folderChooserFlags, [this](const FileChooser& chooser)
	{
		File file(chooser.getResult());

		if (file != juce::File{})
		{
			synths.clear();
			midiFile = std::make_unique<MidiFile>();
			std::unique_ptr<FileInputStream> str = file.createInputStream();
			midiFile->readFrom(*str);
			name = file.getFileName() + " - Midi timeformat: " + String(midiFile->getTimeFormat()) + ", last timestamp: " + String(midiFile->getLastTimestamp()) + ", tracks: " + String(midiFile->getNumTracks());
			processMidi();
			setComponentSize();
		}
	});
}

void MIDITimelineComponent::processMidi()
{
	midiTracks.clear();

	for (int i = 0; i < midiFile->getNumTracks(); i++)
	{
		const MidiMessageSequence* seq = midiFile->getTrack(i);
		MidiMessageSequence* seqCopy = new MidiMessageSequence(*seq);
		midiTracks.add(seqCopy);
		SynthAudioSource* src = new SynthAudioSource();
		src->prepareToPlay(samplesPerBlockExpectedInt, sampleRateInt);
		src->synth.noteOn(0, 60, 1.0f);
		src->synth.noteOn(0, 64, 1.0f);
		src->synth.noteOn(0, 67, 1.0f);
		synths.add(src);
	}
	
	// Analyze MIDI
	
	int numberOfSharpsOrFlats = 0;
	this->noteProbabilities.assign(12, 0.0f);
	this->numerator = 0;
	this->denominator = 0;
	String majMin = "";

	for (int trackIndex = 0; trackIndex < midiTracks.size(); trackIndex++)
	{
		for (int eventIndex = 0; eventIndex < midiTracks[trackIndex]->getNumEvents(); eventIndex++)
		{
			juce::MidiMessageSequence::MidiEventHolder* midiEvent = midiTracks[trackIndex]->getEventPointer(eventIndex);
			MidiMessage midiMessage = midiEvent->message;

			if (midiMessage.isTimeSignatureMetaEvent())
			{
				midiMessage.getTimeSignatureInfo(this->numerator, this->denominator);
			}

			if (midiMessage.isMidiChannelMetaEvent())
			{
				DBG("Track " + String(trackIndex) + ":" + String(midiMessage.getMidiChannelMetaEventChannel()));
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

			if (midiMessage.isNoteOn())
			{
				juce::MidiMessageSequence::MidiEventHolder* noteOffHolder = midiEvent->noteOffObject;
				int noteNumber = midiMessage.getNoteNumber();
				double noteStart = midiMessage.getTimeStamp();
				double noteEnd = noteStart;
				double duration = 0;

				if (noteOffHolder)
				{
					noteEnd = noteOffHolder->message.getTimeStamp();
					duration = noteEnd - noteStart;
				}

				if (musicMath.getRoleByNoteNumber(noteNumber) != -1)
				{
					DBG(musicMath.GetNoteName(musicMath.getRoleByNoteNumber(noteNumber)) + String(musicMath.getRoleByNoteNumber(noteNumber)) + " - " + String(duration));
				}

				this->noteProbabilities[noteNumber%12] += (float)duration * 1.0f;
			}
		}
	}
	double totalDuration = midiFile->getLastTimestamp();
	int numMeasures = 0;
	if (this->denominator && this->numerator)
	{
		int ticksPerMeasure = (int)(midiFile->getTimeFormat() * (4.0f / (float)this->denominator) * (float)this->numerator);
		numMeasures = roundToInt(totalDuration / (double)ticksPerMeasure);
	}
	measureMatrices.clear();

	for (int measure = 0; measure < numMeasures; measure++)
	{
		MarkovMatrixComponent* mmc = new MarkovMatrixComponent(12);
		mmc->embeddedMode = true;
		measureMatrices.add(mmc);
		this->addAndMakeVisible(mmc);
	}

	float maxValue = 0;

	for (float val : this->noteProbabilities)
	{
		if (val > maxValue)
		{
			maxValue = val;
		}
	}

	for (int index = 0; index < 12; index++)
	{
		this->noteProbabilities[index] = this->noteProbabilities[index] / maxValue;
	}

	if (this->denominator && this->numerator)
	{
		this->name += ", T["+ String(numerator) +"/"+String(denominator) + "]";
	}

	if (numberOfSharpsOrFlats < 0)
	{
		numberOfSharpsOrFlats = numberOfSharpsOrFlats + 12;
	}

	this->name += ", " + musicMath.getKeyName(numberOfSharpsOrFlats) + majMin;
}

void MIDITimelineComponent::setComponentSize()
{
	Rectangle<float> parentBounds = getParentComponent()->getBounds().toFloat();
	float trackHeight = (parentBounds.getHeight() * (1 - this->timelineHeightRatio)) / (midiTracks.size());
	setBounds((int)parentBounds.getX(), (int)parentBounds.getY(), (int)parentBounds.getWidth(), (int)trackHeight * midiTracks.size() + (int)(parentBounds.getHeight() * (this->timelineHeightRatio)));
	repaint();
}

void MIDITimelineComponent::changeListenerCallback(ChangeBroadcaster* /*source*/)
{

}
