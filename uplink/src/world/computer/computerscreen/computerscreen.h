
/*

  Computer screen base object

	This should be derived by any computer screens
	such as passwordscreens, logscreens etc

	Note that you will have to provide the Common Functions
	(load, save, GetID etc) for each derived class

  */


#ifndef _included_computerscreen_h
#define _included_computerscreen_h

// ============================================================================

#include "app/uplinkobject.h"


#define SIZE_COMPUTERSCREEN_MAINTITLE 64
#define SIZE_COMPUTERSCREEN_SUBTITLE 64

#define SIZE_COMPUTER_NAME   64


class Computer;

// ============================================================================


class ComputerScreen : public UplinkObject  
{

public:

	char maintitle [SIZE_COMPUTERSCREEN_MAINTITLE];
	char subtitle  [SIZE_COMPUTERSCREEN_SUBTITLE];
	char computer  [SIZE_COMPUTER_NAME];

public:

	ComputerScreen();
	virtual ~ComputerScreen();


	void SetMainTitle ( char *newmaintitle );
	void SetSubTitle  ( char *newsubtitle );
	void SetComputer  ( char *newcomputer );

	Computer *GetComputer ();


	// Common functions

	virtual bool Load  ( FILE *file );			
	virtual void Save  ( FILE *file );			
	virtual void Print ();		
	virtual void Update ();			
	
	virtual char *GetID ();	
	virtual int GetOBJECTID ();

};


#endif
