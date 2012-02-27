
/*

  Warning event

	Triggered as a warning that another Uplink Event is about to happen
	eg Your gateway will be seized in 10 minutes

  */


#ifndef _included_warningevent_h
#define _included_warningevent_h


#include "world/scheduler/uplinkevent.h"


class WarningEvent : public UplinkEvent
{

protected:

	UplinkEvent *event;

public:

	WarningEvent ();
	~WarningEvent ();

	void SetEvent ( UplinkEvent *newevent );

	void Run ();

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

