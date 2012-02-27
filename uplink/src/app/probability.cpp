// Probability.cpp: implementation of the Probability class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "app/probability.h"

#include "mmgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Probability::Probability()
{

	// Initialise the first value to 0
	// Just in case we don't insert a zero value

	chances.PutData ( 0 );

}

Probability::~Probability()
{

    chances.Empty ();

}

void Probability::InputProbability ( int value, int percentagechance )
{

	if ( value >= chances.Size () )
		chances.SetSize ( value + 1 );

	chances.PutData ( percentagechance, value );

}

void Probability::ChangeProbability ( int value, int percentagechance )
{

	if ( chances.ValidIndex (value) )
		chances.ChangeData ( percentagechance, value );

	else
		printf ( "WARNING: Probability::ChangeProbability, invalid value passed\n" );

}

bool Probability::Validate ()
{

	int sum = 0;

	for ( int i = 0; i < chances.Size (); ++i ) {

		if ( chances.ValidIndex ( i ) ) {

			int value = chances.GetData (i);

			if ( value >= 0 && value <= 100 )
				sum += value;

			else
				return false;

		}
		else {

			return false;

		}

	}
	
	return ( sum == 100 );
	
}

int Probability::GetValue ()
{

	int r = (int) ( 100 * ((float) rand () / (float) RAND_MAX) );

	int result = 0;
	int totalscore = 0;

	for ( int i = 0; i < chances.Size (); ++i ) {
		if ( chances.ValidIndex ( i ) ) {
			if ( r >= totalscore && r < totalscore + chances.GetData (i) ) {
				result = i;
				break;
			}
			totalscore += chances.GetData (i);
		}
	}

	return result;

}
