
/*

  Represents a location in the Uplink world

  */


#ifndef _included_vlocation_h
#define _included_vlocation_h

// ============================================================================

#include <stdio.h>

#include "app/uplinkobject.h"
 
#define SIZE_VLOCATION_IP  24
#define SIZE_COMPUTER_NAME   64

#define VIRTUAL_WIDTH     594
#define VIRTUAL_HEIGHT    315

class Computer;

// ============================================================================


class VLocation : public UplinkObject
{

public:

	char ip       [SIZE_VLOCATION_IP];				// Unique
	char computer [SIZE_COMPUTER_NAME];

	int x, y;					// Physical location on map
	bool listed;				// Am I publicly listed?
    bool displayed;             // Should I be displayed on the world map?
	bool colored;               // Should I be displayed in a distinct color on the world map?

public:

	VLocation();
	virtual ~VLocation();

	void SetIP ( char *newip );
	static bool VerifyIP ( char *newip );
	void SetComputer ( char *newcomputer );

	void SetPLocation ( int newx, int newy );
	static bool VerifyPLocation ( int newx, int newy );
	void SetListed ( bool newlisted );
    void SetDisplayed ( bool newdisplayed );
	void SetColorDisplayed ( bool newcolored );

	Computer *GetComputer ();				// Returns NULL if none

	// Common functions

	bool Load  ( FILE *file );
	void Save  ( FILE *file );
	void Print ();
	
	char *GetID ();
	int   GetOBJECTID ();

};

/*
	Special Virtual Location

	Used to specify which screen you enter the system at
	Currently used on modems, which can jump into a system at any screen

  */

class VLocationSpecial : public VLocation
{

public:

	int screenIndex;
    int securitySystemIndex;

public:

	VLocationSpecial ();
	virtual ~VLocationSpecial ();

	void SetScreenIndex ( int newScreenIndex );
    void SetSecuritySystemIndex ( int newIndex );

	// Common functions

	bool Load  ( FILE *file );
	void Save  ( FILE *file );
	void Print ();
	
	char *GetID ();
	int   GetOBJECTID ();

};

#endif 
