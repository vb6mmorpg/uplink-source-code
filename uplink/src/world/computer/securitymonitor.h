
/*

  Security Monitor object

	Used as a static class to represent the 
	current security system being attacked by the player

    Never serialised, only 1 in existence

  */



#ifndef _included_securitymonitor_h
#define _included_securitymonitor_h

#include "world/date.h"


class SecurityMonitor
{

public:

	static int status;											// 0 = unused, 1 = enabled, 2 = monitering

	static DArray <bool> currentmonitor;						// One bool for each system, true if monitored

	static Date nextmonitor_date;								// Date of next system monitor beginning
	static int  nextmonitor_index;								// Index of next system to monitor

public:

	static void BeginAttack ();									// Call when player connects somewhere
	static void EndAttack ();									// Call when player disconnects

	static bool IsMonitored ( int index );						

	static void Update ();										// Call frequently

};


#endif

