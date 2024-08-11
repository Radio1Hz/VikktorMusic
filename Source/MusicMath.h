/*
  ==============================================================================

	MusicMath.h
	Created: 10 May 2023 8:17:33am
	Author:  viktor

  ==============================================================================
*/
#pragma once
#include <JuceHeader.h>

class NoteEventDesc
{
public:

	juce::String	NoteName = "";
	int				NoteNumber = -1;
	int				NoteDuration = 0;
	NoteEventDesc::NoteEventDesc();
	NoteEventDesc::NoteEventDesc(juce::String noteName, int noteNumber);
	NoteEventDesc::NoteEventDesc(juce::String noteName, int noteNumber, int noteDuration);
	NoteEventDesc::~NoteEventDesc();
};

class MusicMath
{
public:
	MusicMath();
	~MusicMath();

	juce::String				displayKeys();
	juce::String				displayModes();
	juce::String				getKeyName(int);
	juce::String				getModeName();
	juce::String				getModeDegree(int);
	juce::String				getChordName();
	void						allNotesOff(int channel);
	juce::Colour				getKeyColour(int keyIndex);
	juce::String				GetNoteName(int noteRoleIndex); // noteRoleIndex in (0, 1, 2, 3, 4, 5, 6) / Root, Second, Third, etc...
	void						transformMIDINoteMessage(juce::MidiMessage& modifiedMsg);
	std::vector<juce::String>	GetKeys();
	std::vector<juce::String>	GetModes();

	int							BPM;							// Beats per minute = Quarter notes per minute (tempo)

	int							TPQN;							// Ticks per quarter note
	double						Tb;								// Time duration of a beat in seconds
	double						Tt;								// Time duration of a tick in seconds
	int							TimeSignatureNominator = 4;		// 4/4
	int							TimeSignatureDenominator = 4;
	int							currentKey = 0;
	int							currentKeyIndex = 0;
	int							currentModeIndex = 0;

	int							translateKeyIndex(int keyIndex);
	void						changeKey(int newKeyIndex);
	void						changeMode(int newModeIndex);
	int							translateRoleIndex(const juce::MidiMessage&);
	int							translateRoleToModeOffset(int, int);
	int							getRoleByNoteNumber(int noteNumber);
	//MIDIAudio*					midiAudioObj;

private:
	std::vector<int>			_keys_offset =					// Number of seminotes from C, sorted by circle of fifths
	{
		0,			// C
		7,			// G
		2,			// D
		9,			// A
		4,			// E
		11,			// B
		6,			// F#
		1,			// C#
		8,			// G#
		3,			// D#
		10,			// Bb
		5			// F
	};

	std::vector<juce::String>	_keys_display =
	{
		"C",
		"G",
		"D",
		"A",
		"E",
		"B",
		"F#",
		"C#",
		"G#",
		"D#",
		"Bb",
		"F"
	};

	std::vector<juce::Colour>	_keys_colour_codes =
	{
		juce::Colour::fromRGBA(255, 165, 0, 255),	//C Major
		juce::Colour::fromRGBA(0, 200, 0, 255),		//G Major
		juce::Colour::fromRGBA(128, 128, 128, 255),	//D Major 
		juce::Colour::fromRGBA(128, 0, 128, 255),	//A Major
		juce::Colour::fromRGBA(200, 0, 0, 255),		//E Major
		juce::Colour::fromRGBA(0, 0, 200, 255),		//B Major
		juce::Colour::fromRGBA(199, 21, 133, 255),	//F# Major
		juce::Colour::fromRGBA(200, 90, 0, 255),	//C# Major
		juce::Colour::fromRGBA(0, 150, 0, 255),		//G# Major
		juce::Colour::fromRGBA(90, 90, 90, 255),	//D# Major
		juce::Colour::fromRGBA(90, 0, 90, 255),		//Bb Major
		juce::Colour::fromRGBA(255, 105, 180, 255)	//F Major
	};

	std::vector<std::vector<int>> _modes_offset
	{
		{0, 2, 4, 5, 7, 9, 11},
		{0, 2, 3, 5, 7, 9, 10},
		{0, 1, 3, 5, 7, 8, 10},
		{0, 2, 4, 6, 7, 9, 11},
		{0, 2, 4, 5, 7, 9, 10},
		{0, 2, 3, 5, 7, 8, 10},
		{0, 1, 3, 5, 6, 8, 10}
	};

	std::vector<juce::String>	_modes_display =
	{
		"Ionian",
		"Dorian",
		"Phrygian",
		"Lydian",
		"Mixolydian",
		"Aeolian",
		"Locrian"
	};

	std::vector<juce::String>	_modes_display_degrees =
	{
		"I",
		"II",
		"III",
		"IV",
		"V",
		"VI",
		"VII"
	};
};



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

	Keys: (Base)
	Display
	C	G	D	A	E	B	F♯	C♯	G♯	D♯	B♭	F
	Math
	0	7	2	9	4	11	6	1	8	3	10	5

	Modes:
	Display
	I, II, III, IV, V, VI, VII
	Math
		0	1	2	3	4	5	6	7	8	9	10	11
		----------------------------------------------
	I	o	.	o	.	o	o	.	o	.	o	.	o
	II	o	.	o	o	.	o	.	o	.	o	o	.
	III	o	o	.	o	.	o	.	o	o	.	o	.
	IV	o	.	o	.	o	.	o	o	.	o	.	o
	V	o	.	o	.	o	o	.	o	.	o	o	.
	VI	o	.	o	o	.	o	.	o	o	.	o	.
	VII	o	o	.	o	.	o	o	.	o	.	o	.


		0	1	2	3	4	5	6	7	8	9	10	11
		----------------------------------------------
	I	0	.	2	.	4	5	.	7	.	9	.	11
	II	0	.	2	3	.	5	.	7	.	9	10	.
	III	0	1	.	3	.	5	.	7	8	.	10	.
	IV	0	.	2	.	4	.	6	7	.	9	.	11
	V	0	.	2	.	4	5	.	7	.	9	10	.
	VI	0	.	2	3	.	5	.	7	8	.	10	.
	VII	0	1	.	3	.	5	6	.	8	.	10	.


		1	2	3	4	5	6	7
		----------------------------------------------
	I	0	2	4	5	7	9	11
	II	0	2	3	5	7	9	10
	III	0	1	3	5	7	8	10
	IV	0	2	4	6	7	9	11
	V	0	2	4	5	7	9	10
	VI	0	2	3	5	7	8	10
	VII	0	1	3	5	6	8	10

Keys Colour coding:

- C is Orange				(#FFA500 or rgba(255, 165, 0, 1))
- C♯/D♭ is DarkOrange		(#FF8C00 or rgba(255, 140, 0, 1))
- D is Gray					(#808080 or rgba(128, 128, 128, 1))
- D♯/E♭ is	LightSlateGray	(#778899 or rgba(119, 136, 153, 1))
- E is Red					(#FF0000 or rgba(255, 0, 0, 1))
- F is Pink					(#FF69B4 or rgba(255, 105, 180, 1))
- F♯/G♭ is MediumVioletRed (#C71585 or rgba(199, 21, 133, 1))
- G is Green				(#008000 or rgba(0, 128, 0, 1))
- G♯/A♭ is OliveDrab		(#6B8E23 or rgba(107, 142, 35, 1))
- A is Purple				(#800080 or rgba(128, 0, 128, 1))
- A♯/B♭ is CornflowerBlue	(#6495ED or rgba(100, 149, 237, 1))
- B is Blue					(#0000FF or rgba(0, 0, 255, 1))

*/