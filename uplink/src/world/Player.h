
/*

  Player class object

	Player ISA Agent ISA Person

  */


#ifndef _included_player_h
#define _included_player_h

// ============================================================================

#include "world/agent.h"
#include "world/computer/gateway.h"

// ============================================================================


class Player : public Agent
{

protected:

	BTree <char *> shares;															// Indexed on company name, values are "numowned totalpricepaid"

public:

	Gateway gateway;	

	int score_peoplefucked;
	int score_systemsfucked;
	int score_highsecurityhacks;

public:

	Player ();
	~Player ();

	bool IsPlayerAccount   ( char *bankip, char *accno );							// True if this account is owned by the player
	
	void GiveAllLinks ();
	void GiveMessage ( Message *message );

	int NumSharesOwned	( char *companyname );
	int SharesPricePaid ( char *companyname );
	void TradeShares	( char *companyname, int howmany );

    static int TimeToTrace ( char *tracerIP, char *routerIP );                     // How long for the people at tracerIP to trace through routerIP
    int TimeRemaining ();                                                          // How long remaining in this trace?

	// Common functions

	bool Load   ( FILE *file );
	void Save   ( FILE *file );
	void Print  ();
	void Update ();

	char *GetID ();
	int   GetOBJECTID ();

};


#endif

