
/*

  Disconnected screen class object

	Shows a message to the player
    "You have been disconnected"
    "You lost the following accounts:"

  */


#ifndef _included_disconnectedscreen_h
#define _included_disconnectedscreen_h

// ============================================================================

#include <stdio.h>

#include "world/computer/computerscreen/computerscreen.h"

class ComputerScreen;

// ============================================================================


class DisconnectedScreen : public ComputerScreen  
{

public:

	int nextpage;
	char *textmessage;

    static DArray <char *> loginslost;

public:

	DisconnectedScreen();
	virtual ~DisconnectedScreen();

	void SetNextPage ( int newnextpage );
	void SetTextMessage ( char *newtextmessage );

    static void AddLoginLost ( char *ip );
    static void ClearLoginsLost ();

    // Common functions

	bool Load  ( FILE *file );			
	void Save  ( FILE *file );			
	void Print ();						
	
	char *GetID ();						
	int   GetOBJECTID ();				

};

#endif
