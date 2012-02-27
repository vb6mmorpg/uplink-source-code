
/*

  Bank Robbery Event

    Created when the player attempts to transfer money 
    into his own bank account

    Runs roughly two minutes later

    If the player hasn't covered his tracks, its game over

  */



#ifndef _included_bankrobberyevent_h
#define _included_bankrobberyevent_h

#include "world/scheduler/eventscheduler.h"


class BankRobberyEvent : public UplinkEvent
{

public:

    char source_ip      [128];
    char source_accno   [128];
    char target_ip      [128];
    char target_accno   [128];
    int amount;
    Date hackdate;

public:

    BankRobberyEvent ();
    ~BankRobberyEvent ();

	void Run ();

    void SetDetails ( char *newsourceip, char *newsourceaccno,
                      char *newtargetip, char *newtargetaccno,
                      int newamount, Date *newhackdate );

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
