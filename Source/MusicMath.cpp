/*
  ==============================================================================

	MusicMath.cpp
	Created: 10 May 2023 8:17:33am
	Author:  viktor

  ==============================================================================
*/

#include "MusicMath.h"
#include "ApplicationProperties.h"
#include "ProcessusEntropiae.InvitatioAdFestum.h"
#include "ProcessusEntropiae.OlimInFukushima.h"
#include "Contrapunctus.One.h"

MusicMath::MusicMath()
{

}

MusicMath::MusicMath(int noteRangeStart, int noteRangeEnd)
{
	this->noteRangeStart = noteRangeStart;
	this->noteRangeEnd = noteRangeEnd;
}

MusicMath::~MusicMath()
{

}

String MusicMath::getNoteNameByMIDINoteNumber(int noteNumber)
{
	return MidiMessage::getMidiNoteName(noteNumber, true, false, 4);
}

String MusicMath::displayKeys()
{
	String out = "";
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

String MusicMath::displayModes()
{
	String out = "";
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

std::vector<String> MusicMath::GetKeys()
{
	std::vector<String> copyKeys = _keys_display;
	return copyKeys;
}

std::vector<String> MusicMath::GetModes()
{
	std::vector<String> copyModes = _modes_display_degrees;
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
	MidiMessage anOff = MidiMessage::allNotesOff(channel);
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

String MusicMath::getKeyName(int keyIndex)
{
	return _keys_display[keyIndex];
}
Colour MusicMath::getKeyColour(int keyIndex)
{
	return _keys_colour_codes[keyIndex];
}

String MusicMath::getModeName()
{
	return _modes_display[currentModeIndex];
}

String MusicMath::getModeDegree(int modeIndex)
{
	return _modes_display_degrees[modeIndex];
}

String MusicMath::getChordName()
{
	int realNoteNumber = _keys_offset[currentKeyIndex] + _modes_offset[0][currentModeIndex];
	String chordName = MusicMath::getNoteNameByMIDINoteNumber(realNoteNumber);

	if (chordName == "A#")  // Ugly fix
		chordName = "Bb";

	if (currentModeIndex == 1 || currentModeIndex == 2 || currentModeIndex == 5)    // Minor modes: Dorian, Phrygian and Aeolian
		chordName += "m";

	if (currentModeIndex == 6)  // Locrian is dim
		chordName += "dim";

	return chordName;
}

String MusicMath::GetNoteName(int noteRoleIndex)
{
	int realNoteNumber = _keys_offset[currentKeyIndex] + _modes_offset[0][currentModeIndex] + _modes_offset[currentModeIndex][noteRoleIndex];
	return MusicMath::getNoteNameByMIDINoteNumber(realNoteNumber);
}

void MusicMath::transformMIDINoteMessage(MidiMessage& modifiedMsg)
{
	if (!MidiMessage::isMidiNoteBlack(modifiedMsg.getNoteNumber()))
	{
		int octave = modifiedMsg.getNoteNumber() / 12;
		int roleIndex = translateRoleIndex(modifiedMsg);
		int noteOffset = translateRoleToModeOffset(roleIndex, currentModeIndex);
		int modeOffset = translateRoleToModeOffset(currentModeIndex, 0);
		modifiedMsg.setVelocity(1.0); // Optional
		modifiedMsg.setNoteNumber(octave * 12 + currentKey + modeOffset + noteOffset);
	}
}

void MusicMath::setNoteRange(int nRS, int nRE)
{
	this->noteRangeStart = nRS;
	this->noteRangeEnd = nRE;
}

int MusicMath::getNoteRangeSize()
{
	return noteRangeEnd - noteRangeStart + 1;
}

int MusicMath::getNoteRangeStart()
{
	return noteRangeStart;
}

int MusicMath::getNoteRangeEnd()
{
	return noteRangeEnd;
}

// Input: MidiNoteNumber (60, 61, 62...) Output: Role in the scale (white keys index 0, 1, 2, 3 [root, second, third...]) 
// Returns -1 if black key
int MusicMath::translateRoleIndex(const MidiMessage& midiNote)
{
	return getRoleByNoteNumber(midiNote.getNoteNumber());
}

int MusicMath::getRoleByNoteNumber(int noteNumber)
{
	int moduoNumber = noteNumber % 12;
	if (!MidiMessage::isMidiNoteBlack(moduoNumber))
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

int MusicMath::getNoteNumberByRoleNumber(int keyRoot, int modeIndex, int roleIndex)
{
	return keyRoot + _modes_offset[modeIndex][roleIndex];
	/*
	_modes_offset
		{0, 2, 4, 5, 7, 9, 11, 12},
		{0, 2, 3, 5, 7, 9, 10, 12},
		{0, 1, 3, 5, 7, 8, 10, 12},
		{0, 2, 4, 6, 7, 9, 11, 12},
		{0, 2, 4, 5, 7, 9, 10, 12},
		{0, 2, 3, 5, 7, 8, 10, 12},
		{0, 1, 3, 5, 6, 8, 10, 12},
		{12, 14, 16, 17, 19, 21, 23, 24}, // Octave
	*/
}

int MusicMath::getRandomConsonanceRoleIndex(vector<float>& probabilityVector, int /*previousNote*/, ContextDesc& currentContext, bool /*shouldBePerfect*/)
{
	//int previousRole = currentContext.getNoteRoleIndexByAbsoluteMIDINoteNumber(previousNote);
	int startVectorIndex = AppProperties::random.nextInt(12);
	int returnRoleIndex = -1;

	for (int i = 0; i < 12; i++)
	{
		if (probabilityVector[(i + startVectorIndex) % 12] > 0.0f)
		{
			returnRoleIndex = currentContext.getNoteRoleIndexByAbsoluteMIDINoteNumber((i + startVectorIndex) % 12);
			break;
		}
	}

	return returnRoleIndex;
}
// Returns random II, IV, VII
int MusicMath::getRandomDissonanceRoleIndex(vector<float>& probabilityVector, int /*previousNote*/, ContextDesc& currentContext)
{
	//int previousRole = currentContext.getNoteRoleIndexByAbsoluteMIDINoteNumber(previousNote);
	int startVectorIndex = AppProperties::random.nextInt(12);
	int returnRoleIndex = -1;

	for (int i = 0; i < 12; i++)
	{
		if (probabilityVector[(i + startVectorIndex) % 12] > 0.0f)
		{
			returnRoleIndex = currentContext.getNoteRoleIndexByAbsoluteMIDINoteNumber(i);
			break;
		}
	}

	return returnRoleIndex;
}

int MusicMath::getRandomRoleIndex(bool onlyConsonants)
{
	if (onlyConsonants)
	{
		int num = AppProperties::random.nextInt(3);
		if (num == 0)
		{
			return 0; //Root
		}
		if (num == 1)
		{
			return 2; //Third
		}
		if (num == 2)
		{
			return 4; //Fifth
		}
	}
	return AppProperties::random.nextInt(7);
}

list<ContextDesc> MusicMath::getContextDescriptions(vector<vector<NoteEventDesc>>& noteEventMatrix, int sCS, int sCE, int methodID)
{
	list<ContextDesc> allPossiblieTonalities;
	switch (methodID)
	{
	case 0:
		return getContextDescriptionsBasicMethod(noteEventMatrix, sCS, sCE);
	case 1:
		return getContextDescriptionsWeightedPitchMethod(noteEventMatrix, sCS, sCE);
	default:
		break;
	}
	return allPossiblieTonalities;
}

list<ContextDesc> MusicMath::getContextDescriptionsBasicMethod(vector<vector<NoteEventDesc>>& /*noteEventMatrix*/, int /*sCS*/, int /*sCE*/)
{
	list<ContextDesc> allPossiblieTonalities;

	//vector<int> defMajorScaleVectorFull(noteRangeEnd - noteRangeStart);
	//vector<int> defMinorScaleVectorFull(noteRangeEnd - noteRangeStart);
	//vector<int> defMajorChordVectorFull(noteRangeEnd - noteRangeStart);
	//vector<int> defMinorChordVectorFull(noteRangeEnd - noteRangeStart);

	//int mostProbableRoot = 0;
	//String mostProbableRootFlavor = "maj";

	//int maxSum = 0;
	//int numColumns = sCE - sCS + 1;
	//Matrix<int> tempMatrix(noteRangeEnd - noteRangeStart, numColumns);
	//tempMatrix.clear();

	//int startTonality = -1;
	////Normalize Matrix
	//bool shouldNormalize = true;

	////Copy portion of noteEventMatrix into tempMatrix
	//for (int c = 0; c < tempMatrix.getNumColumns(); c++)
	//{
	//	for (int n = noteRangeStart; n < noteRangeEnd; n++)
	//	{
	//		if (noteEventMatrix[n - noteRangeStart][sCS + c].NoteNumber == n && noteEventMatrix[n - noteRangeStart][sCS + c].EventType == 1)
	//		{
	//			if (startTonality < 0)
	//			{
	//				startTonality = noteEventMatrix[n - noteRangeStart][sCS + c].NoteNumber % 12;
	//			}
	//			tempMatrix.operator()(n - noteRangeStart, c) = shouldNormalize ? 1 : n;
	//		}
	//		else
	//		{
	//			tempMatrix.operator()(n - noteRangeStart, c) = 0;
	//		}
	//	}
	//}

	////Go through all 12 tonalities
	//for (int tIndex = 0; tIndex < 12; tIndex++)
	//{
	//	int t = (startTonality + tIndex) % 12;

	//	String tonalityRootName = MusicMath::getNoteNameByMIDINoteNumber(t);
	//	for (int c = 0; c < numColumns; c++)
	//	{
	//		//Define template vectors for Major and Minor for the Tonality[t]
	//		for (int n = noteRangeStart; n < noteRangeEnd; n++)
	//		{
	//			defMajorScaleVectorFull[n - noteRangeStart] = _defaultMajorScaleDefinitionVector[(n - t) % 12];
	//			defMinorScaleVectorFull[n - noteRangeStart] = _defaultMinorScaleDefinitionVector[(n - t) % 12];
	//			defMajorChordVectorFull[n - noteRangeStart] = _defaultMajorChordDefinitionVector[(n - t) % 12];
	//			defMinorChordVectorFull[n - noteRangeStart] = _defaultMinorChordDefinitionVector[(n - t) % 12];
	//		}
	//	}

	//	vector<int> resMajChord = multiplyMatrixAndVector(tempMatrix, defMajorChordVectorFull);
	//	vector<int> resMinChord = multiplyMatrixAndVector(tempMatrix, defMinorChordVectorFull);
	//	vector<int> resMajScale = multiplyMatrixAndVector(tempMatrix, defMajorScaleVectorFull);
	//	vector<int> resMinScale = multiplyMatrixAndVector(tempMatrix, defMinorScaleVectorFull);

	//	debugVector(resMajChord, "resMajChord", true);
	//	debugVector(resMinChord, "resMinChord", true);
	//	debugVector(resMajScale, "resMajScale", true);
	//	debugVector(resMinScale, "resMinScale", true);

	//	int sumMajChord = sumOfCellsInVector(resMajChord);
	//	int sumMinChord = sumOfCellsInVector(resMinChord);
	//	int sumMajScale = sumOfCellsInVector(resMajScale);
	//	int sumMinScale = sumOfCellsInVector(resMinScale);

	//	if (maxSum < sumMajChord + sumMinChord + sumMajScale + sumMinScale)
	//	{
	//		if (sumMajChord + sumMajScale >= sumMinChord + sumMinScale)
	//		{
	//			mostProbableRootFlavor = "maj";
	//		}
	//		else
	//		{
	//			mostProbableRootFlavor = "min";
	//		}

	//		float p = (float)(sumMajChord + sumMinChord + sumMajScale + sumMinScale);
	//		mostProbableRoot = t;
	//		maxSum = sumMajChord + sumMinChord + sumMajScale + sumMinScale;
	//		ContextDesc cd(mostProbableRoot, "", p);
	//		allPossiblieTonalities.push_back(cd);
	//	}
	//}
	//allPossiblieTonalities.sort([](const ContextDesc& f, const ContextDesc& s) { return f.Probability > s.Probability; });

	//for (ContextDesc& ctx : allPossiblieTonalities)
	//{
	//	ctx.Probability = ctx.Probability / (float)maxSum;
	//}

	return allPossiblieTonalities;
}

list<ContextDesc> MusicMath::getContextDescriptionsWeightedPitchMethod(vector<vector<NoteEventDesc>>& noteEventMatrix, int sCS, int sCE)
{
	list<ContextDesc> allPossiblieTonalities;

	vector<vector<int>> _modesWeightVectorsFull(noteRangeEnd - noteRangeStart);

	int maxSum = 0;
	int numColumns = sCE - sCS + 1;
	Matrix<int> tempMatrix(noteRangeEnd - noteRangeStart, numColumns);
	tempMatrix.clear();

	int startTonality = -1;
	//Normalize Matrix
	bool shouldNormalize = true;
	bool isAreaEmpty = true;
	//Copy portion of noteEventMatrix into tempMatrix

	for (int c = 0; c < tempMatrix.getNumColumns(); c++)
	{
		for (int n = noteRangeStart; n < noteRangeEnd; n++)
		{
			if (noteEventMatrix[n - noteRangeStart][sCS + c].NoteNumber == n && noteEventMatrix[n - noteRangeStart][sCS + c].EventType == 1)
			{
				if (startTonality < 0)
				{
					startTonality = noteEventMatrix[n - noteRangeStart][sCS + c].NoteNumber % 12;
				}
				tempMatrix.operator()(n - noteRangeStart, c) = shouldNormalize ? 1 : n;
				isAreaEmpty = false;
			}
			else
			{
				tempMatrix.operator()(n - noteRangeStart, c) = 0;
			}
		}
	}

	if (!isAreaEmpty)
	{
		//Go through all 12 tonalities
		maxSum = 0;

		for (int tIndex = 0; tIndex < 12; tIndex++)
		{
			int t = (startTonality + tIndex) % 12;

			for (int c = 0; c < numColumns; c++)
			{
				//Define template vectors for Major and Minor for the Tonality[t]
				for (int n = noteRangeStart; n < noteRangeEnd; n++)
				{
					for (int m = 0; m < 7; m++)
					{
						if (_modesWeightVectorsFull[n - noteRangeStart].empty())
						{
							_modesWeightVectorsFull[n - noteRangeStart] = vector<int>(7);
						}
						_modesWeightVectorsFull[n - noteRangeStart][m] = _modesWeightVectors[m][(n - t) % 12];
					}
				}
			}
			for (int m = 0; m < 7; m++)
			{
				int resultingSum = 0;
				vector<vector<int>> resultingVector = multiplyMatrixWithModes(tempMatrix, _modesWeightVectorsFull);

				for (int n = noteRangeStart; n < noteRangeEnd; n++)
				{
					resultingSum += resultingVector[n - noteRangeStart][m];
				}

				if (maxSum < resultingSum)
				{
					ContextDesc cd(t + middleC, m, (float)resultingSum);
					allPossiblieTonalities.push_back(cd);
					maxSum = resultingSum;
				}
			}
		}
		allPossiblieTonalities.sort([](const ContextDesc& f, const ContextDesc& s) { return f.Probability > s.Probability; });

		for (ContextDesc& ctx : allPossiblieTonalities)
		{
			ctx.Probability = ctx.Probability / (float)maxSum;
		}
	}

	return allPossiblieTonalities;
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

int MusicMath::sumOfCellsInVector(const vector<int>& vec)
{
	int res = 0;
	for (int i = 0; i < vec.size(); i++)
	{
		res += vec[i];
	}
	return res;
}

void MusicMath::debugMatrix(const Matrix<int>& mat, String friendlyName, bool showNoteRange)
{
	if (showNoteRange)
	{
		DBG(friendlyName + " Matrix [" + String(mat.getNumRows()) + ", " + String(mat.getNumColumns()) + "]" + " Note Range: " + String(noteRangeStart) + "-" + String(noteRangeEnd) + " ------------------------------------------------------------");
	}
	else
	{
		DBG(friendlyName + " Matrix [" + String(mat.getNumRows()) + ", " + String(mat.getNumColumns()) + "] ------------------------------------------------------------");
	}
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
void MusicMath::debugVector(const vector<int>& vec, String friendlyName, bool showNoteRange)
{
	if (showNoteRange)
	{
		DBG(friendlyName + " Vector [" + String(vec.size()) + "], Note Range: " + String(noteRangeStart) + "-" + String(noteRangeEnd) + " ------------------------------------------------------------");
	}
	else
	{
		DBG(friendlyName + " Vector [" + String(vec.size()) + "] ------------------------------------------------------------");
	}
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

	row = "";
	for (int i = 0; i < vec.size(); i++)
	{
		row += String(vec[i]) + "\t";
	}
	DBG(row);

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
vector<int> MusicMath::multiplyMatrixAndVector(const Matrix<int>& mat, const vector<int>& vec)
{
	// Check if the matrices can be multiplied
	if (vec.size() != mat.getNumRows())
		throw std::invalid_argument("Matrix and Vector dimensions do not match for multiplication.");
	vector<int> result(vec.size());

	for (int i = 0; i < mat.getNumColumns(); i++)
	{
		for (int j = 0; j < mat.getNumRows(); j++)
		{
			result[j] += mat(j, i) * vec[j];
		}
	}
	return result;
}

vector<vector<int>> MusicMath::multiplyMatrixWithModes(const Matrix<int>& mat, const vector<vector<int>>& modes)
{
	vector<vector<int>> resultingMultiplication(mat.getNumRows());
	if (modes.size() != mat.getNumRows())
		throw std::invalid_argument("Matrix and Vectors dimensions do not match for multiplication.");

	for (int i = 0; i < mat.getNumColumns(); i++)
	{
		for (int j = 0; j < mat.getNumRows(); j++)
		{
			if (resultingMultiplication[j].empty())
			{
				resultingMultiplication[j] = vector<int>(7);
			}

			for (int m = 0; m < 7; m++)
			{
				resultingMultiplication[j][m] += mat(j, i) * modes[j][m];
			}

		}
	}
	return resultingMultiplication;
}

bool MusicMath::isNoteInRange(int noteNumber)
{
	return noteNumber <= noteRangeEnd && noteNumber >= noteRangeStart;
}

void MusicMath::generateContexts(int numMeasures, vector<vector<NoteEventDesc>>& noteEventMatrix, float numQuartersPerMeasure, int numTimeUnitsPerMeasure)
{
	//InvitatioAdFestum composition;
	OlimInFukushima composition;
	composition.generateContexts(numMeasures, noteEventMatrix, numQuartersPerMeasure, numTimeUnitsPerMeasure, noteRangeStart, noteRangeEnd);
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
{
}
NoteEventDesc::NoteEventDesc(int noteNumber)
{
	NoteNumber = noteNumber;
	NoteDuration = 0;
}

NoteEventDesc::NoteEventDesc(int noteNumber, int noteDuration) : NoteEventDesc::NoteEventDesc(noteNumber)
{
	NoteDuration = noteDuration;
}

NoteEventDesc::NoteEventDesc(int noteNumber, int noteDuration, int eventType) : NoteEventDesc::NoteEventDesc(noteNumber, noteDuration)
{
	EventType = eventType;
}

NoteEventDesc::NoteEventDesc(int noteNumber, int noteDuration, int eventType, int noteRole) : NoteEventDesc::NoteEventDesc(noteNumber, noteDuration, eventType)
{
	NoteRole = noteRole;
}

NoteEventDesc::NoteEventDesc(int noteNumber, int noteDuration, int eventType, int noteRole, bool gnrtd) : NoteEventDesc::NoteEventDesc(noteNumber, noteDuration, eventType, noteRole)
{
	Generated = gnrtd;
}

NoteEventDesc::NoteEventDesc(int noteNumber, int noteDuration, int eventType, int noteRole, bool gnrtd, float vel) : NoteEventDesc::NoteEventDesc(noteNumber, noteDuration, eventType, noteRole, gnrtd)
{
	Velocity = vel;
}

NoteEventDesc::~NoteEventDesc()
{
}

ContextDesc::ContextDesc()
{}

ContextDesc::ContextDesc(int rootMIDINote, int mode)
{
	this->Mode = mode;
	this->RootMIDINote = rootMIDINote;
	this->ContextDuration = 0;

}
ContextDesc::ContextDesc(int rootMIDINote, int mode, float probability)
{
	this->Mode = mode;
	this->RootMIDINote = rootMIDINote;
	this->ContextDuration = 0;
	this->Probability = probability;
}

ContextDesc::~ContextDesc()
{
}

String ContextDesc::friendlyName()
{
	MusicMath math;
	String suffixText = "";
	if (Mode == 1 || Mode == 2 || Mode == 5 || Mode == 6)
	{
		suffixText = "m";
	}
	if (Mode == 6)
	{
		suffixText = "dim";
	}
	return MusicMath::getNoteNameByMIDINoteNumber(RootMIDINote) + suffixText + " (" + (Mode > -1 ? math.getModeDegree(Mode) : "") + ")";
}

int ContextDesc::getAbsoluteNoteFromKeyModeAndRole(int noteRoleIndex)
{
	MusicMath musicMath;
	return musicMath._modes_offset[Mode][noteRoleIndex] + RootMIDINote;
	/*{
		{0, 2, 4, 5, 7, 9, 11},
		{0, 2, 3, 5, 7, 9, 10},
		{0, 1, 3, 5, 7, 8, 10},
		{0, 2, 4, 6, 7, 9, 11},
		{0, 2, 4, 5, 7, 9, 10},
		{0, 2, 3, 5, 7, 8, 10},
		{0, 1, 3, 5, 6, 8, 10}
	};*/
}

int ContextDesc::getNoteRoleIndexByAbsoluteMIDINoteNumber(int MIDINoteNumber)
{
	int stepsDifference = MIDINoteNumber % 12; // Number of Half-steps above root (can be negative)
	MusicMath musicMath;
	int roleIndex = -1;
	for (int i : musicMath._modes_offset[Mode])
	{
		roleIndex++;
		if (stepsDifference == (RootMIDINote + i) % 12)
		{
			return roleIndex;
		}
	}
	return -1;
}
