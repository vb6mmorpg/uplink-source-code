
/*
	
	Uplink Event object

	Base class for all Future Events that can occur

  */


#ifndef _included_uplinkevent_h
#define _included_uplinkevent_h

// ============================================================================

#include "app/uplinkobject.h"

#include "world/date.h"

// ============================================================================



class UplinkEvent : public UplinkObject
{

public:
	
	Date rundate;

public:

	UplinkEvent ();
	~UplinkEvent ();
	
	void SetRunDate ( Date *newrundate );
	
	virtual void Run ();					// You must override this
	virtual void RunWarning ();				// You can override this


	virtual char *GetShortString ();
	virtual char *GetLongString ();

	// Common functions

	bool Load  ( FILE *file );
	void Save  ( FILE *file );
	void Print ();
	
	char *GetID ();							// You must override this
	int   GetOBJECTID ();					// You must override this

};


#endif

