
/*

  High Security Screen

	Represents a set of security logins 
	which must all be bypassed in order to 
	access the system.

	Looks like a menu screen.

  */

#ifndef _included_highsecurityscreen_h
#define _included_highsecurityscreen_h

#include "world/computer/computerscreen/computerscreen.h"
class MenuScreenOption;


class HighSecurityScreen : public ComputerScreen
{

public:

	LList <MenuScreenOption *> systems;
	int nextpage;

public:

	HighSecurityScreen ();
	~HighSecurityScreen ();

	void AddSystem ( char *name, int page );
	void SetNextPage ( int newnextpage );

	void ResetSecurity ();							// Sets all systems to "Enabled"

	// Common functions

	bool Load  ( FILE *file );			
	void Save  ( FILE *file );			
	void Print ();						
	
	char *GetID ();						
	int   GetOBJECTID ();				

};


#endif

