
/*  =================================================================
	Probability class object

	Programmer inputs his probabilities
	then calls a function which spits out one of his possible inputs,
	based on the probabilities he programmed in.

	*/

#ifndef _included_probability_h
#define _included_probability_h

#include "tosser.h"

class Probability  
{

protected:

	DArray <int> chances;

public:

	Probability();
	virtual ~Probability();

	void InputProbability ( int value, int percentagechance );
	void ChangeProbability ( int value, int percentagechance );

	bool Validate ();							// True if all percentages sum to 100

	int GetValue ();

};


#endif
