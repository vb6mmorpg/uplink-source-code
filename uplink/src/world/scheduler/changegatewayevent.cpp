
#include "app/globals.h"
#include "app/serialise.h"

#include "game/game.h"

#include "world/world.h"
#include "world/player.h"
#include "world/message.h"
#include "world/scheduler/changegatewayevent.h"

#include "mmgr.h"


ChangeGatewayEvent::ChangeGatewayEvent ()
{

	newgatewaydef = NULL;

}

ChangeGatewayEvent::~ChangeGatewayEvent ()
{

	if ( newgatewaydef )
		delete newgatewaydef;

}

void ChangeGatewayEvent::SetNewGateway ( GatewayDef *value )
{

	UplinkAssert ( value );

	if ( newgatewaydef )
		delete newgatewaydef;

	newgatewaydef = new GatewayDef ( *value );

}

void ChangeGatewayEvent::Run ()
{

	// Do the exchange

	if ( newgatewaydef )
		game->GetWorld ()->GetPlayer ()->gateway.ExchangeGateway ( newgatewaydef );


	// Send a mail to the player

	Message *msg = new Message ();
	msg->SetTo ( "PLAYER" );
	msg->SetFrom ( "Uplink Corporation" );
	msg->SetSubject ( "New Gateway Ready" );
	msg->SetBody ( "We have finished preparing your new gateway machine.  We have installed "
				   "identical hardware in this new gateway, and we have copied your memory "
				   "banks onto the new system.\n\n"
				   "In order to complete the upgrade, you must now log out from Uplink.  When "
				   "you next log back in to the system, you will be automatically redirected "
				   "to your new gateway system.\n\n"
				   "We will then immediately dismantle your old system and wipe the memory banks "
				   "of all data stored.\n\n"
				   "Regards\n"
				   "[END]" );
	msg->Send ();

}

void ChangeGatewayEvent::RunWarning ()
{

}

char *ChangeGatewayEvent::GetShortString ()
{

	size_t shortstringsize = 128 + ( (newgatewaydef)? strlen ( newgatewaydef->name ) : 4 );
	char *shortstring = new char [shortstringsize];
	UplinkSnprintf ( shortstring, shortstringsize, "Exchanging gateway for '%s'", (newgatewaydef)? newgatewaydef->name : "NULL" );
	return shortstring;
		
}

char *ChangeGatewayEvent::GetLongString ()
{

	return GetShortString ();
		
}

bool ChangeGatewayEvent::Load  ( FILE *file )
{

	LoadID ( file );

	newgatewaydef = NULL;

	if ( !UplinkEvent::Load ( file ) ) return false;

	if ( strcmp( game->GetLoadedSavefileVer(), "SAV61" ) >= 0 ) {
		int exists;
		if ( !FileReadData ( &exists, sizeof(exists), 1, file ) ) return false;
		if ( exists != 0 ) {
			newgatewaydef = new GatewayDef ();
			if ( !newgatewaydef->Load ( file ) ) return false;
		}
	}
	else {
		int old_newtype = -1;
		if ( !FileReadData ( &old_newtype, sizeof(old_newtype), 1, file ) ) return false;
		if ( game->GetWorld ()->gatewaydefs.ValidIndex ( old_newtype ) ) {
			newgatewaydef = new GatewayDef ( *game->GetWorld ()->gatewaydefs.GetData ( old_newtype ) );
		}
	}

	LoadID_END ( file );

	return true;
	
}

void ChangeGatewayEvent::Save  ( FILE *file )
{

	SaveID ( file );

	UplinkEvent::Save ( file );

	int exists;
	if ( newgatewaydef ) {
		exists = 1;
		fwrite ( &exists, sizeof(exists), 1, file );
		newgatewaydef->Save ( file );
	}
	else {
		exists = 0;
		fwrite ( &exists, sizeof(exists), 1, file );
	}

	SaveID_END ( file );
		
}

void ChangeGatewayEvent::Print ()
{

	printf ( "ChangeGateway Event\n" );
	UplinkEvent::Print ();
	printf ( "New gateway '%s'\n", (newgatewaydef)? newgatewaydef->name : "NULL" );

}
	
char *ChangeGatewayEvent::GetID ()
{

	return "EVT_CGE";

}

int ChangeGatewayEvent::GetOBJECTID ()
{

	return OID_CHANGEGATEWAYEVENT;

}
