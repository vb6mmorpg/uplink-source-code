
/*

  Attempt Mission Event

	Represents a NPC agent attempting one of his missions

  */

#ifndef _included_attemptmissionevent_h
#define _included_attemptmissionevent_h


#include "world/person.h"
#include "world/scheduler/uplinkevent.h"


class AttemptMissionEvent : public UplinkEvent
{

public:

	char agentname [SIZE_PERSON_NAME];

public:

	AttemptMissionEvent ();
	~AttemptMissionEvent ();

	void Run ();

	void SetAgentName ( char *newagentname );

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

