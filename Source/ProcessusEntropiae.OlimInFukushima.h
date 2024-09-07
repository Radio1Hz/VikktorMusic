/*
  ==============================================================================

	ProcessusEntropiae.OlimInFukushima.h
	Created: 27 Aug 2024 8:34:02am
	Author:  viktor

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CompositionBase.h"
#include "MusicMath.h"
#include "ApplicationProperties.h"

using namespace juce;
using namespace std;

class OlimInFukushima : public CompositionBase
{
public:
	OlimInFukushima()
	{
		Name = "Processus Entropiae - Olim in Fukushima";
		contextsInUse = vector<ContextDesc>{ ContextDesc(middleC + 10, 1), ContextDesc(middleC + 0, 2), ContextDesc(middleC + 3, 4), ContextDesc(middleC + 3, 4) };
		numRepetitions = vector<int>{ 1, 1, 1, 1 };

		/*
			C	0
			C#	1
			D	2
			D#	3
			E	4
			F	5
			F#	6
			G	7
			G#	8
			A	9
			A#	10
			B	11
			//Olim in Fukushima
			A#m				Cm				D#				D#
			Dorian			Phrigian		Mixolydian		Mixolydian
			10-1			0-2				3-4				3-4
		*/

		basicRhythmsPattern = vector<vector<string>>
		{
			// C - Consonace
			// D - Dissonace
			{
				"C", "", "", "",		"", "", "", "",			"", "", "", "",			"", "", "", "",
				"C", "", "", "",		"", "", "", "",			"", "", "", "",			"", "", "", "",
				"C", "", "", "",		"", "", "", "",			"", "", "", "",			"", "", "", "",
				"C", "", "", "",		"", "", "", "",			"", "", "", "",			"", "", "", "",

				"D", "", "", "",		"", "", "", "",			"", "", "", "",			"", "", "", "",
				"D", "", "", "",		"", "", "", "",			"", "", "", "",			"", "", "", "",
				"D", "", "", "",		"", "", "", "",			"", "", "", "",			"", "", "", "",
				"D", "", "", "",		"", "", "", "",			"", "", "", "",			"", "", "", ""
			}
		};
	}

	~OlimInFukushima()
	{
	}

};
