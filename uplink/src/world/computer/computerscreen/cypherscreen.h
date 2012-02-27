
/*
	
	Class CypherScreen

	A computer screen which asks the user
	to enter a great big number

	*/


#ifndef _included_cypherscreen_h
#define _included_cypherscreen_h

#include "world/computer/computerscreen/computerscreen.h"


class CypherScreen : public ComputerScreen
{

public:

	int nextpage;
	int difficulty;									// 1 = easy, >1 = getting harder
	
public:

	CypherScreen ();
	virtual ~CypherScreen ();

	void SetNextPage ( int newnextpage );
	void SetDifficulty ( int newdifficulty );

	// Common functions

	bool Load  ( FILE *file );			
	void Save  ( FILE *file );			
	void Print ();						
	
	char *GetID ();						
	int   GetOBJECTID ();			

};


#endif

