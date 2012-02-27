

/*

  Change Gateway Event

	When this occurs, the player is told
	he must log out, then his gateway
	will upgrade to the new type

  */


#ifndef _included_changegatewayevent_h
#define _included_changegatewayevent_h


#include "world/scheduler/uplinkevent.h"


class GatewayDef;


class ChangeGatewayEvent : public UplinkEvent
{

public:

	GatewayDef* newgatewaydef;

public:

	ChangeGatewayEvent ();
	~ChangeGatewayEvent ();

	void SetNewGateway ( GatewayDef *value );

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

