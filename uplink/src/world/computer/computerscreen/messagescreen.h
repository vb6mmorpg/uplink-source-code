
/*

  Message screen class object

	Shows a message and the company title

  */


#ifndef _included_messagescreen_h
#define _included_messagescreen_h

// ============================================================================

#include <stdio.h>

#include "world/computer/computerscreen/computerscreen.h"

class ComputerScreen;

// ============================================================================


class MessageScreen : public ComputerScreen  
{

public:

	int nextpage;
	char *textmessage;
	char *buttonmessage;
	bool mailthistome;	

public:

	MessageScreen();
	virtual ~MessageScreen();

	void SetNextPage ( int newnextpage );
	void SetTextMessage ( char *newtextmessage );
	void SetButtonMessage ( char *newbuttonmessage );
	void SetMailThisToMe ( bool newvalue );

	// Common functions

	bool Load  ( FILE *file );			
	void Save  ( FILE *file );			
	void Print ();						
	
	char *GetID ();						
	int   GetOBJECTID ();				

};

#endif
