
/*

  Transfer event 

	Occurs when money needs to be moved from account to account
	(usually for hacking)

  */

#ifndef _included_transferevent_h
#define _included_transferevent_h


#include "world/person.h"
#include "world/scheduler/uplinkevent.h"


class TransferEvent : public UplinkEvent
{

protected:

	char *source;
	int accountfrom;
	
	char *dest;
	int accountto;

	int amount;
	char *reason;

public:

	TransferEvent ();
	~TransferEvent ();

	void SetFrom	( char *sourcecomputer, int sourceaccount);
	void SetTo		( char *destcomputer, int destaccount);
	void SetDetails ( int transferamount, char *transferreason);

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

