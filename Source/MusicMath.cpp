/*
  ==============================================================================

	MusicMath.cpp
	Created: 10 May 2023 8:17:33am
	Author:  viktor

  ==============================================================================
*/

#include "MusicMath.h"
#include "ApplicationProperties.h"

MusicMath::MusicMath()
{
	_keysDefinitions = make_unique<Matrix<int>>(12, 12);
	_defaultMajorScaleDefinitionVector = make_unique<Matrix<int>>(1, 12);
	_defaultMinorScaleDefinitionVector = make_unique<Matrix<int>>(1, 12);
	_defaultMajorChordDefinitionVector = make_unique<Matrix<int>>(1, 12);
	_defaultMinorChordDefinitionVector = make_unique<Matrix<int>>(1, 12);

	_defaultMajorScaleDefinitionVector->clear();
	_defaultMinorScaleDefinitionVector->clear();
	_defaultMajorChordDefinitionVector->clear();
	_defaultMinorChordDefinitionVector->clear();
	/* Default Major = Ionian
		1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 0
	}*/

	_defaultMajorScaleDefinitionVector->operator()(0, 0) = 1;
	_defaultMajorScaleDefinitionVector->operator()(0, 1) = 0;
	_defaultMajorScaleDefinitionVector->operator()(0, 2) = 1;
	_defaultMajorScaleDefinitionVector->operator()(0, 3) = 0;
	_defaultMajorScaleDefinitionVector->operator()(0, 4) = 1;
	_defaultMajorScaleDefinitionVector->operator()(0, 5) = 1;
	_defaultMajorScaleDefinitionVector->operator()(0, 6) = 0;
	_defaultMajorScaleDefinitionVector->operator()(0, 7) = 1;
	_defaultMajorScaleDefinitionVector->operator()(0, 8) = 0;
	_defaultMajorScaleDefinitionVector->operator()(0, 9) = 1;
	_defaultMajorScaleDefinitionVector->operator()(0, 10) = 0;
	_defaultMajorScaleDefinitionVector->operator()(0, 11) = 1;

	/* Default Minor = Aeolian
		1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0
	}*/

	_defaultMinorScaleDefinitionVector->operator()(0, 0) = 1;
	_defaultMinorScaleDefinitionVector->operator()(0, 1) = 0;
	_defaultMinorScaleDefinitionVector->operator()(0, 2) = 1;
	_defaultMinorScaleDefinitionVector->operator()(0, 3) = 1;
	_defaultMinorScaleDefinitionVector->operator()(0, 4) = 0;
	_defaultMinorScaleDefinitionVector->operator()(0, 5) = 1;
	_defaultMinorScaleDefinitionVector->operator()(0, 6) = 0;
	_defaultMinorScaleDefinitionVector->operator()(0, 7) = 1;
	_defaultMinorScaleDefinitionVector->operator()(0, 8) = 1;
	_defaultMinorScaleDefinitionVector->operator()(0, 9) = 0;
	_defaultMinorScaleDefinitionVector->operator()(0, 10) = 1;
	_defaultMinorScaleDefinitionVector->operator()(0, 11) = 0;

	
	_defaultMajorChordDefinitionVector->operator()(0, 0) = 1;
	_defaultMajorChordDefinitionVector->operator()(0, 4) = 1;
	_defaultMajorChordDefinitionVector->operator()(0, 7) = 1;

	_defaultMinorChordDefinitionVector->operator()(0, 0) = 1;
	_defaultMinorChordDefinitionVector->operator()(0, 3) = 1;
	_defaultMinorChordDefinitionVector->operator()(0, 7) = 1;

}

MusicMath::~MusicMath()
{

}

String MusicMath::getNoteNameByMIDINoteNumber(int noteNumber)
{
	return MidiMessage::getMidiNoteName(noteNumber, true, false, 4);
}

juce::String MusicMath::displayKeys()
{
	juce::String out = "";
	for (int i = 0; i < 12; i++)
	{
		if (currentKey == _keys_offset[i])
		{
			out += "[" + _keys_display[i] += "] ";
		}
		else
		{
			out += _keys_display[i] + " ";
		}
	}
	return out;
}

juce::String MusicMath::displayModes()
{
	juce::String out = "";
	for (int i = 0; i < 7; i++)
	{
		if (currentModeIndex == i)
		{
			out += "[" + _modes_display[i] + "] ";
		}
		else
		{
			out += _modes_display[i] + " ";
		}

	}
	return out;
}

std::vector<juce::String> MusicMath::GetKeys()
{
	std::vector<juce::String> copyKeys = _keys_display;
	return copyKeys;
}

std::vector<juce::String> MusicMath::GetModes()
{
	std::vector<juce::String> copyModes = _modes_display_degrees;
	return copyModes;
}

int MusicMath::translateKeyIndex(int index)
{
	if (index > 11 || index < 0)
	{
		return 0;
	}
	else
	{
		return _keys_offset[index];
	}

}

void MusicMath::changeKey(int newKeyIndex)
{
	currentKeyIndex = newKeyIndex;
	if (newKeyIndex > 11 || newKeyIndex < 0)
	{
		currentKeyIndex = 0;
	}

	currentKey = translateKeyIndex(currentKeyIndex);
	//allNotesOff(midiAudioObj->synthMIDIChannel);
	//allNotesOff(midiAudioObj->contextMIDIChannel);
}

void MusicMath::allNotesOff(int channel)
{
	juce::MidiMessage anOff = juce::MidiMessage::allNotesOff(channel);
	//midiAudioObj->playNote(anOff);
}

void MusicMath::changeMode(int newModeIndex)
{
	currentModeIndex = newModeIndex;
	if (newModeIndex > 6 || newModeIndex < 0)
	{
		currentModeIndex = 0;
	}
	//allNotesOff(midiAudioObj->synthMIDIChannel);
	//allNotesOff(midiAudioObj->contextMIDIChannel);
}

int MusicMath::translateRoleToModeOffset(int roleIndex, int modeIndex)
{
	if (roleIndex < 0 || roleIndex > 6 || modeIndex < 0 || modeIndex > 6)
	{
		return -1;
	}

	return _modes_offset[modeIndex][roleIndex];
}

juce::String MusicMath::getKeyName(int keyIndex)
{
	return _keys_display[keyIndex];
}
juce::Colour MusicMath::getKeyColour(int keyIndex)
{
	return _keys_colour_codes[keyIndex];
}

juce::String MusicMath::getModeName()
{
	return _modes_display[currentModeIndex];
}

juce::String MusicMath::getModeDegree(int modeIndex)
{
	return _modes_display_degrees[modeIndex];
}

juce::String MusicMath::getChordName()
{
	int realNoteNumber = _keys_offset[currentKeyIndex] + _modes_offset[0][currentModeIndex];
	juce::String chordName = MusicMath::getNoteNameByMIDINoteNumber(realNoteNumber);

	if (chordName == "A#")  // Ugly fix
		chordName = "Bb";

	if (currentModeIndex == 1 || currentModeIndex == 2 || currentModeIndex == 5)    // Minor modes: Dorian, Phrygian and Aeolian
		chordName += "m";

	if (currentModeIndex == 6)  // Locrian is dim
		chordName += "dim";

	return chordName;
}

juce::String MusicMath::GetNoteName(int noteRoleIndex)
{
	int realNoteNumber = _keys_offset[currentKeyIndex] + _modes_offset[0][currentModeIndex] + _modes_offset[currentModeIndex][noteRoleIndex];
	juce::String noteName = MusicMath::getNoteNameByMIDINoteNumber(realNoteNumber);
	return noteName;
}

void MusicMath::transformMIDINoteMessage(juce::MidiMessage& modifiedMsg)
{
	if (!juce::MidiMessage::isMidiNoteBlack(modifiedMsg.getNoteNumber()))
	{
		int octave = modifiedMsg.getNoteNumber() / 12;
		int roleIndex = translateRoleIndex(modifiedMsg);
		int noteOffset = translateRoleToModeOffset(roleIndex, currentModeIndex);
		int modeOffset = translateRoleToModeOffset(currentModeIndex, 0);
		modifiedMsg.setVelocity(1.0); // Optional
		modifiedMsg.setNoteNumber(octave * 12 + currentKey + modeOffset + noteOffset);
	}
}

// Input: MidiNoteNumber (60, 61, 62...) Output: Role in the scale (white keys index 0, 1, 2, 3 [root, second, third...]) 
// Returns -1 if black key
int MusicMath::translateRoleIndex(const juce::MidiMessage& midiNote)
{
	return getRoleByNoteNumber(midiNote.getNoteNumber());
}

int MusicMath::getRoleByNoteNumber(int noteNumber)
{
	int moduoNumber = noteNumber % 12;
	if (!juce::MidiMessage::isMidiNoteBlack(moduoNumber))
	{
		switch (moduoNumber)
		{
		case 0:
			return 0; // DO     - ROOT
			break;
		case 2:
			return 1; // RE     - SECOND
			break;
		case 4:
			return 2; // MI     - THIRD
			break;
		case 5:
			return 3; // FA     - FOURTH
			break;
		case 7:
			return 4; // SOL    - FIFTH
			break;
		case 9:
			return 5; // LA     - SIXTH
			break;
		case 11:
			return 6; // TI     - SEVENTH
			break;
		default:
			break;
		}
	}
	return -1;
}


int MusicMath::sumOfCellsInMatrix(const Matrix<int>& mat)
{
	int res = 0;
	for (int i = 0; i < mat.getNumRows(); i++)
	{	
		for (int j = 0; j < mat.getNumColumns(); j++)
		{
			res += mat(i, j);
		}
	}
	return res;
}

void MusicMath::debugMatrix(const Matrix<int>& mat, String friendlyName)
{
	DBG(friendlyName + " Matrix [" + String(mat.getNumRows()) + ", " + String(mat.getNumColumns()) + "] ------------------------------------------------------------");
	for (int i = 0; i < mat.getNumRows(); i++)
	{
		String row = "";
		for (int j = 0; j < mat.getNumColumns(); j++)
		{
			row += String(mat(i, j)) + "\t";
		}
		DBG(row);
	}
}

void MusicMath::debugMatrix(const Matrix<int>& mat, int noteRangeStart, int noteRangeEnd, String friendlyName)
{
	DBG(friendlyName + " Matrix [" + String(mat.getNumRows()) + ", " + String(mat.getNumColumns()) + "]" + " Note Range: " + String(noteRangeStart) + "-" + String(noteRangeEnd) + " ------------------------------------------------------------");
	String row = "";
	String row2 = "";
	
	for (int n = noteRangeStart; n < noteRangeEnd; n++)
	{
		String note = MusicMath::getNoteNameByMIDINoteNumber(n);
		row += String(n) + "\t";
		row2 += String(note) + "\t";
	}
	DBG(row);
	DBG(row2);

	for (int i = 0; i < mat.getNumRows(); i++)
	{
		row = "";
		for (int j = 0; j < mat.getNumColumns(); j++)
		{
			row += String(mat(i, j)) + "\t";
		}
		DBG(row);
	}
}
// Input Matrix selectionMatrix[60, 5] x defMajorMatrix[5, 60]
Matrix<int> MusicMath::multiplyMatrices(const Matrix<int>& mat1, const Matrix<int>& mat2)
{
	// Check if the matrices can be multiplied
	if (mat1.getNumColumns() != mat2.getNumRows())
		throw std::invalid_argument("Matrix dimensions do not match for multiplication.");

	// Create a result matrix with appropriate dimensions
	Matrix<int> result(mat1.getNumRows(), mat2.getNumColumns());

	// Perform matrix multiplication
	for (int i = 0; i < mat1.getNumRows(); ++i)
	{
		for (int j = 0; j < mat2.getNumColumns(); ++j)
		{
			int sum = 0;
			for (int k = 0; k < mat1.getNumColumns(); ++k)
			{
				sum += mat1(i, k) * mat2(k, j);
			}
			result(i, j) = sum;
		}
	}

	return result;
}

//Input Mat1[60, 5] Mat2[1, 60] Output Result[1, 60]
Matrix<int> MusicMath::multiplyMatrixAndVector(const Matrix<int>& mat1, const Matrix<int>& mat2)
{
	// Check if the matrices can be multiplied
	if (mat2.getNumColumns() != mat1.getNumRows())
		throw std::invalid_argument("Matrix dimensions do not match for multiplication.");
	Matrix<int> result(1, mat1.getNumRows());
	result.clear();

	for (int i = 0; i < mat1.getNumColumns(); i++)
	{
		for (int j = 0; j < mat1.getNumRows(); j++)
		{
			result(0, j) += mat1(j, i) * mat2(0, j);
		}
	}
	return result;
}


/*
Octave	C	C#	D	D#	E	F	F#	G	G#	A	Bb	B
   -2	0	1	2	3	4	5	6	7	8	9	10	11
   -1	12	13	14	15	16	17	18	19	20	21	22	23
	0	24	25	26	27	28	29	30	31	32	33	34	35
	1	36	37	38	39	40	41	42	43	44	45	46	47
	2	48	49	50	51	52	53	54	55	56	57	58	59
	3	60	61	62	63	64	65	66	67	68	69	70	71
	4	72	73	74	75	76	77	78	79	80	81	82	83
	5	84	85	86	87	88	89	90	91	92	93	94	95
	6	96	97	98	99	100	101	102	103	104	104	106	107
	7	108	109	110	111	112	113	114	115	116	117	118	119
	8	120	121	122	123	124	125	126	127
*/
NoteEventDesc::NoteEventDesc()
{}
NoteEventDesc::NoteEventDesc(juce::String noteName, int noteNumber)
{
	this->NoteName = noteName;
	this->NoteNumber = noteNumber;
}

NoteEventDesc::NoteEventDesc(juce::String noteName, int noteNumber, int noteDuration)
{
	this->NoteName = noteName;
	this->NoteNumber = noteNumber;
	this->NoteDuration = noteDuration;
}

NoteEventDesc::NoteEventDesc(juce::String noteName, int noteNumber, int noteDuration, int eventType)
{
	this->NoteName = noteName;
	this->NoteNumber = noteNumber;
	this->NoteDuration = noteDuration;
	this->EventType = eventType;
}

NoteEventDesc::~NoteEventDesc()
{
}

ContextDesc::ContextDesc()
{}

ContextDesc::ContextDesc(int rootMIDINote, bool isMajor)
{
	this->IsMajor = isMajor;
	this->RootMIDINote = rootMIDINote;
	this->ContextDuration = 0;
	
}
ContextDesc::ContextDesc(int rootMIDINote, bool isMajor, float probability)
{
	this->IsMajor = isMajor;
	this->RootMIDINote = rootMIDINote;
	this->ContextDuration = 0;
	this->Probability = probability;
}

ContextDesc::~ContextDesc()
{
}

String ContextDesc::debug()
{
	return MusicMath::getNoteNameByMIDINoteNumber(RootMIDINote) + (IsMajor ? "maj" : "min") + " p: " + String::formatted("%1.1f", this->Probability);
}
