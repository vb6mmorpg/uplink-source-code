
/*
	
	Class UserIDScreen

	A computer screen which allows the user to id himself

	*/


#ifndef _included_useridscreen_h
#define _included_useridscreen_h

#include "world/computer/computerscreen/computerscreen.h"


class UserIDScreen : public ComputerScreen
{

public:

	int nextpage;
	int difficulty;									// 1 = easy, >1 = getting harder
	
public:

	UserIDScreen ();
	virtual ~UserIDScreen ();

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

