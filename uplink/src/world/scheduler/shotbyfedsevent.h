
/*

  Shot By Feds event object

	Occurs when someone is shot by federal agents

  */


#ifndef _included_shotbyfedsevent_h
#define _included_shotbyfedsevent_h

// ============================================================================

#include "eclipse.h"

#include "world/person.h"
#include "world/scheduler/uplinkevent.h"

// ============================================================================



class ShotByFedsEvent : public UplinkEvent
{

protected:

	char *reason;

public:

	char name [SIZE_PERSON_NAME];

public:

	ShotByFedsEvent ();
	~ShotByFedsEvent ();

	void SetName ( char *newname );
	void SetReason ( char *newreason );

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

