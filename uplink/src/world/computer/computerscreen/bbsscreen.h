
/*

  BBS Screen class object

	Acts as a repository for messages which can be posted by anyone.
	
	Currently hacked to view world->missions only

  */

#ifndef _included_bbsscreen_h
#define _included_bbsscreen_h


#include "world/computer/computerscreen/computerscreen.h"


class BBSScreen : public ComputerScreen  
{

public:

	int nextpage;
	int contactpage;

public:

	BBSScreen();
	virtual ~BBSScreen();

	void SetNextPage ( int newnextpage );
	void SetContactPage ( int newcontactpage );

	// Common functions

	bool Load  ( FILE *file );			
	void Save  ( FILE *file );			
	void Print ();						
	
	char *GetID ();						
	int   GetOBJECTID ();				

};

#endif
