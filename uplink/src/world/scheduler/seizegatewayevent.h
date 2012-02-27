
/*

  Seize Gateway event

	Occurs when federal agents decide to grab your gateway
	(Only applies to Uplink Agents)

  */

#ifndef _included_seizeevent_h
#define _included_seizeevent_h


#include "world/person.h"
#include "world/scheduler/uplinkevent.h"


class SeizeGatewayEvent : public UplinkEvent
{

protected:

	char *reason;

public:

	char name [SIZE_PERSON_NAME];
	int gateway_id;							// id number of gateway to seize

public:

	SeizeGatewayEvent ();
	~SeizeGatewayEvent ();

	void SetName ( char *newname );
	void SetReason ( char *newreason );
	void SetGatewayID ( int newid );

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

