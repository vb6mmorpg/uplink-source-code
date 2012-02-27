
/*

  Install Hardware Event

	Occurs when the player has ordered a hardware upgrade 
	for his gateway and it finally arrives.

  */


#ifndef _included_installhardwareevent_h
#define _included_installhardwareevent_h

// ============================================================================

#include "world/scheduler/uplinkevent.h"

class Sale;

// ============================================================================


class InstallHardwareEvent : public UplinkEvent
{

protected:

	Sale *hwsale;
	int version;

public:

	InstallHardwareEvent ();
	~InstallHardwareEvent ();

	void SetHWSale ( Sale *newsale, int newversion );
	
	void Run ();
	void RunWarning ();

	char *GetShortString ();
	char *GetLongString ();

	// Common functions

	bool Load  ( FILE *file );
	void Save  ( FILE *file );
	void Print ();
	
	char *GetID ();				
	int   GetOBJECTID ();		


};


#endif

