
/*

  LAN Monitor

    A static object which keeps an eye
    on the players current LAN attack

  */


#ifndef _included_lanmonitor_h
#define _included_lanmonitor_h


class LanComputer;

#define SYSADMIN_ASLEEP             0
#define SYSADMIN_CURIOUS            1
#define SYSADMIN_SEARCHING          2
#define SYSADMIN_FOUNDYOU           3


class LanMonitor
{

protected:

    static bool currentlyActive;                                // Only true if we are connected to a LAN
    static LanComputer *lanComputer;                            

public:

	static int currentSystem;									// Index of current system
	static int currentSelected;									// Index of currently selected system
	static int currentSpoof;									// Our current "From" address, maybe spoofed

	static LList <int> connection;								// The player's connection through the LAN (system indexes)

    static int sysAdminState;
    static int sysAdminCurrentSystem;
    static int sysAdminTimer;

public:

	static void BeginAttack ();									// Call when player connects somewhere
	static void EndAttack ();									// Call when player disconnects
    static void ResetAll ();                                    // Empties all data structures

	static void SetCurrentSystem	( int newCurrentSystem );
	static void SetCurrentSelected	( int newCurrentSelected );
	static void SetCurrentSpoof		( int newCurrentSpoof );

	static void ExtendConnection	    ( int newNode );				// May not do anything
    static void ForceExtendConnection   ( int newNode );              // Will extend every time
    static void RetractConnection	    ();                           // Remove last link
	static bool IsInConnection		    ( int node );
    static int  GetNodeIndex            ( int node );
    static bool IsAccessable            ( int node );

    static void SysAdminAwaken ();
    static void SysAdminSleep ();

	static void Update ();										// Call frequently

};


#endif
