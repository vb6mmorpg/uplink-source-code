
#include "stdafx.h"

#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"

#include "game/game.h"
#include "game/data/data.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"

#include "world/world.h"
#include "world/vlocation.h"
#include "world/player.h"
#include "world/message.h"
#include "world/computer/computer.h"
#include "world/computer/lanmonitor.h"

#include "world/generator/numbergenerator.h"
#include "world/generator/consequencegenerator.h"

#include "mmgr.h"

Player::Player () : Agent ()
{

	score_peoplefucked = 0;
	score_systemsfucked = 0;
	score_highsecurityhacks = 0;

}

Player::~Player ()
{

    DeleteBTreeData ( &shares );

}

void Player::GiveAllLinks ()
{

	DArray <VLocation *> *locs = game->GetWorld ()->locations.ConvertToDArray ();

	for ( int i = 0; i < locs->Size (); ++i )
		if ( locs->ValidIndex (i) )
			GiveLink ( locs->GetData (i)->ip );

	delete locs;

}

void Player::GiveMessage ( Message *message )
{

	Agent::GiveMessage ( message );

	// Transfer any attached files from the message into memory

	if ( message->GetData () ) {
		Data *datacopy = new Data ( message->GetData () );
		gateway.databank.PutData ( datacopy );
	}

}

bool Player::IsPlayerAccount ( char *bankip, char *accno )
{

	char searchstring [64];
	UplinkSnprintf ( searchstring, sizeof ( searchstring ), "%s %s", bankip, accno );

	for ( int i = 0; i < accounts.Size (); ++i )
		if ( accounts.ValidIndex (i) )
			if ( strcmp ( accounts.GetData (i), searchstring ) == 0 )
				return true;

	return false;

}

int Player::NumSharesOwned	( char *companyname )
{

	char *stringdata = shares.GetData ( companyname );

	if ( !stringdata )
		return 0;

	else {

		int numowned;
		int pricepaid;
		sscanf ( stringdata, "%d %d", &numowned, &pricepaid );
		return numowned;

	}

}

int Player::SharesPricePaid ( char *companyname )
{

	char *stringdata = shares.GetData ( companyname );

	if ( !stringdata )
		return 0;

	else {

		int numowned;
		int pricepaid;
		sscanf ( stringdata, "%d %d", &numowned, &pricepaid );
		return pricepaid;

	}

}

void Player::TradeShares ( char *companyname, int howmany )
{

	// Look up all neccisary data on company
	// and the shares we own in it

	Company *company = game->GetWorld ()->GetCompany ( companyname );
	UplinkAssert (company);

	char *stringdata = shares.GetData ( companyname );


	if ( !stringdata ) {										// No existing transactions

		if ( howmany > 0 ) {

			int cost = howmany * company->GetSharePrice ();

			if ( GetBalance () >= cost ) {

				size_t newentrysize = 16;
				char *newentry = new char [newentrysize];
				UplinkSnprintf ( newentry, newentrysize, "%d %d", howmany, cost );
				shares.PutData ( companyname, newentry );

				ChangeBalance ( howmany * company->GetSharePrice () * -1, "International Stock Market" );

			}

		}

	}
	else {														// We already have some

		int numowned;
		int pricepaid;
		sscanf ( stringdata, "%d %d", &numowned, &pricepaid );

		if ( howmany > 0 ) {									// We are buying more

			numowned += howmany;
			pricepaid += howmany * company->GetSharePrice ();

			if ( howmany * company->GetSharePrice () > GetBalance () ) return;			// You can't afford this

			ChangeBalance ( howmany * company->GetSharePrice () * -1, "International Stock Market" );

		}
		else if ( howmany < 0 ) {								// We are selling some

			if ( numowned > 0 ) {

				float fractionsold = (float) howmany / (float) numowned;			// (howmany is a negative number)
				float fractionremaining = 1 + fractionsold;

				numowned += howmany;
				pricepaid = (int) ( pricepaid * fractionremaining );

				ChangeBalance ( howmany * company->GetSharePrice () * -1, "International Stock Market" );

			}

		}

		shares.RemoveData ( companyname );
		delete [] stringdata;

		size_t newentrysize = 16;
		char *newentry = new char [newentrysize];
		UplinkSnprintf ( newentry, newentrysize, "%d %d", numowned, pricepaid );
		shares.PutData ( companyname, newentry );

	}

}

bool Player::Load ( FILE *file )
{

	LoadID ( file );

	if ( !Agent::Load ( file ) ) return false;

	if ( !gateway.Load ( file ) ) return false;

	if ( !FileReadData ( &score_peoplefucked, sizeof(score_peoplefucked), 1, file ) ) return false;
	if ( !FileReadData ( &score_systemsfucked, sizeof(score_systemsfucked), 1, file ) ) return false;
	if ( !FileReadData ( &score_highsecurityhacks, sizeof(score_highsecurityhacks), 1, file ) ) return false;

	if ( !LoadBTree ( &shares, file ) ) return false;

	LoadID_END ( file );

	return true;

}

void Player::Save ( FILE *file )
{

	SaveID ( file );

	Agent::Save ( file );

	gateway.Save ( file );

	fwrite ( &score_peoplefucked, sizeof(score_peoplefucked), 1, file );
	fwrite ( &score_systemsfucked, sizeof(score_systemsfucked), 1, file );
	fwrite ( &score_highsecurityhacks, sizeof(score_highsecurityhacks), 1, file );

	SaveBTree ( &shares, file );

	SaveID_END ( file );

}

void Player::Print  ()
{

	printf ( "Player : \n" );
	Agent::Print ();
	gateway.Print ();
	printf ( "Score : People fucked : %d\n", score_peoplefucked );
	printf ( "Score : Systems fucked : %d\n", score_systemsfucked );
	printf ( "Score : High Security Hacks : %d\n", score_highsecurityhacks );
	PrintBTree ( &shares );

}

int Player::TimeRemaining ()
{

    int timeremaining = 0;

    int numlocations = GetConnection ()->GetSize () - GetConnection ()->traceprogress - 1;

    for ( int i = numlocations; i >= 0; --i ) {

        char *thisip = GetConnection ()->vlocations.GetData ( i );
        UplinkAssert (thisip);
        timeremaining += TimeToTrace ( GetRemoteHost ()->ip, thisip );

    }

    return timeremaining;

}

int Player::TimeToTrace ( char *tracerIP, char *routerIP )
{

    VLocation *tracer = game->GetWorld ()->GetVLocation (tracerIP);
    UplinkAssert (tracer);
    Computer *comp = tracer->GetComputer ();
    UplinkAssert (comp);

	int timetonexttrace = comp->tracespeed;

    //
    // Modifiers to the time to next trace

    int playeraccesslevel = game->GetWorld ()->GetPlayer ()->HasAccount ( routerIP );

    if ( playeraccesslevel == -1 && tracer->GetComputer ()->traceaction > COMPUTER_TRACEACTION_FINE ) {
        timetonexttrace = (int) ( timetonexttrace * TRACESPEED_MODIFIER_NOACCOUNT );
    }
    else if ( playeraccesslevel == 1 ) {
        timetonexttrace = (int) ( timetonexttrace * TRACESPEED_MODIFIER_ADMINACCESS );
    }
    else if ( playeraccesslevel != -1 ) {
        timetonexttrace = (int) ( timetonexttrace * TRACESPEED_MODIFIER_HASACCOUNT );
    }

    if ( playeraccesslevel != -1 && tracer->GetComputer ()->TYPE == COMPUTER_TYPE_CENTRALMAINFRAME ) {
        timetonexttrace = (int) ( timetonexttrace * COMPUTER_TYPE_CENTRALMAINFRAME );
    }

    if ( playeraccesslevel == 1 && tracer->GetComputer ()->TYPE == COMPUTER_TYPE_PUBLICBANKSERVER ) {
        timetonexttrace = (int) ( timetonexttrace * TRACESPEED_MODIFIER_PUBLICBANKSERVERADMIN );
    }

    if ( timetonexttrace < 2 ) timetonexttrace = 2;

    return timetonexttrace;

}

void Player::Update ()
{

	Agent::Update ();

	//
	// Update the remote monitors watching the hack on the
	// target computer
	//

    if ( IsConnected () ) {
        SecurityMonitor::Update ();
        LanMonitor::Update ();
    }


	//
	// Update the trace on the player if one exists
	//

	if ( IsConnected () && GetRemoteHost ()->GetComputer ()->tracespeed > -1 && GetConnection ()->TraceInProgress () ) {

        if ( GetConnection ()->traceprogress < GetConnection ()->GetSize () - 1 ) {

			// Trace in progress - maybe advance

			if ( game->GetWorld ()->date.After ( &(GetConnection ()->nexttrace) ) ) {

				++(GetConnection ()->traceprogress);

                // Work out time to next trace

                char *thisip = GetConnection ()->vlocations.GetData ( GetConnection ()->GetSize () - GetConnection ()->traceprogress - 1 );
                UplinkAssert (thisip);

                int timetonexttrace = TimeToTrace ( GetRemoteHost ()->ip, thisip );
                timetonexttrace = NumberGenerator::ApplyVariance ( timetonexttrace, (int) ( TRACESPEED_VARIANCE * 100 ) );

				GetConnection ()->nexttrace.SetDate ( &(game->GetWorld ()->date) );
				GetConnection ()->nexttrace.AdvanceSecond ( timetonexttrace );

			}

		}

		// Check if the trace is now complete IMMEDIATELY
		// (So the player can't quickly disconnect between now and the next player update)

		if ( GetConnection ()->traceprogress == GetConnection ()->GetSize () - 1 ) {

			// Trace completed - act on action

			Computer *comp = GetRemoteHost ()->GetComputer ();
			char *username = game->GetInterface ()->GetRemoteInterface ()->security_name;

			ConsequenceGenerator::CaughtHacking ( this, comp );

		}

	}

}


char *Player::GetID ()
{
	return "PLAYER";
}

int Player::GetOBJECTID ()
{
	return OID_PLAYER;
}

