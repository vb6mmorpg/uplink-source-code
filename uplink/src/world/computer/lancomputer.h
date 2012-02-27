
/*
    LAN Computer system

  a computer made up of lots of smaller computers
  arranged in a network

  */

// ============================================================================


#ifndef _included_lancomputer_h
#define _included_lancomputer_h

#include "world/computer/computer.h"

class LanComputerSystem;
class LanComputerLink;


// ============================================================================



class LanComputer : public Computer
{

public:

    DArray <LanComputerSystem *> systems;
    DArray <LanComputerLink *> links;

public:

    LanComputer ();
    ~LanComputer ();

    int AddLanSystem ( LanComputerSystem *system );
    int AddLanSystem ( int TYPE, int x, int y, int security = 2, int screenIndex = -1, int dataScreenIndex = -1 );
    int AddLanLink ( int from, float fromX, float fromY, 
                     int to, float toX, float toY, int security = 1 );

	// Common functions

	bool Load  ( FILE *file );
	void Save  ( FILE *file );
	void Print ();
	void Update ();
	
	char *GetID ();
	int   GetOBJECTID ();

};


// ============================================================================


#define LANSYSTEM_NUMTYPES          17

#define LANSYSTEM_NONE              0
#define LANSYSTEM_ROUTER            1
#define LANSYSTEM_HUB               2
#define LANSYSTEM_SWITCH            3
#define LANSYSTEM_TERMINAL          4
#define LANSYSTEM_MAINSERVER        5                             // data = locks that must be unlocked first
#define LANSYSTEM_MAILSERVER        6
#define LANSYSTEM_FILESERVER        7
#define LANSYSTEM_AUTHENTICATION    8							  // data = systems to unlock
#define LANSYSTEM_LOCK              9							  // data1 = 1 (locked) or 0 (unlocked)
#define LANSYSTEM_ISOLATIONBRIDGE   10                            // data1 = gateway lock, data2 = critical system lock
#define LANSYSTEM_MODEM             11
#define LANSYSTEM_SESSIONKEYSERVER  12                            
#define LANSYSTEM_RADIOTRANSMITTER  13                            
#define LANSYSTEM_RADIORECEIVER     14                            // data1 = freq ghz, data2 = freq mhz
#define LANSYSTEM_FAXPRINTER        15
#define LANSYSTEM_LOGSERVER         16

#define LANSYSTEMVISIBLE_NONE             0                       // Can't see anything
#define LANSYSTEMVISIBLE_AWARE            1                       // Aware of its existence
#define LANSYSTEMVISIBLE_TYPE             2                       // We know what sort of system it is
#define LANSYSTEMVISIBLE_FULL             3                       // We know all about it

#define LANLINKVISIBLE_NONE				0						// Can't see anything
#define LANLINKVISIBLE_FROMAWARE		1						// Aware of where it comes from
#define LANLINKVISIBLE_TOAWARE			2						// Aware of where it goes to
#define LANLINKVISIBLE_AWARE			3						// Are of the full link, from and to



class LanComputerSystem : public UplinkObject
{

public:

    int TYPE;
    int x;
    int y;
    int visible;
	int subnet;
	int security;

	int screenIndex;
    int dataScreenIndex;                            // Index of a screen in which we can dump information

	int data1;
	int data2;
	int data3;

	DArray <int> validSubnets;									// All valid FROM connections

public:

    LanComputerSystem ();
    ~LanComputerSystem ();

    void IncreaseVisibility ( int newValue );

	// Common functions

	bool Load  ( FILE *file );
	void Save  ( FILE *file );
	void Print ();
	
	char *GetID ();
	int   GetOBJECTID ();

};


// ============================================================================


class LanComputerLink : public UplinkObject
{

public:

    int visible;
	int port;
	int security;

    int from;
    int to;

    float fromX;                                    // Range from 0.0 to 1.0, along the X and Y sides
    float fromY;                                    // So the link can be positioned at any point on the edge of the system

    float toX;
    float toY;

public:

    LanComputerLink ();
    ~LanComputerLink ();

    void IncreaseVisibility ( int newValue );

    // Common functions

	bool Load  ( FILE *file );
	void Save  ( FILE *file );
	void Print ();
	
	char *GetID ();
	int   GetOBJECTID ();

};


#endif

