
#include <strstream>

#include "soundgarden.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"
#include "app/opengl.h"
#include "app/opengl_interface.h"

#include "mainmenu/mainmenu.h"

#include "game/game.h"
#include "game/data/data.h"

#include "world/world.h"
#include "world/player.h"
#include "world/message.h"
#include "world/generator/consequencegenerator.h"
#include "world/scheduler/seizegatewayevent.h"

#include "options/options.h"

#include "mmgr.h"



SeizeGatewayEvent::SeizeGatewayEvent ()
{

	reason = NULL;
	gateway_id = 0;
	memset ( name, 0, sizeof ( name ) );

}

SeizeGatewayEvent::~SeizeGatewayEvent ()
{

	if ( reason ) delete [] reason;

}

void SeizeGatewayEvent::SetName ( char *newname )
{

	UplinkAssert ( strlen(newname) < SIZE_PERSON_NAME );
	UplinkStrncpy ( name, newname, sizeof ( name ) );

}

void SeizeGatewayEvent::SetReason ( char *newreason )
{

	if ( reason )
		delete [] reason;
	reason = new char [strlen(newreason)+1];
	UplinkSafeStrcpy ( reason, newreason );

}

void SeizeGatewayEvent::SetGatewayID ( int newid )
{

	gateway_id = newid;

}

void SeizeGatewayEvent::Run ()
{

	Person *person = game->GetWorld ()->GetPerson ( name );
	UplinkAssert (person);

	ConsequenceGenerator::SeizeGateway ( person, reason );

	//
	// If this is the player, then it's GAME OVER
	//

	if ( strcmp ( name, "PLAYER" ) == 0 ) {

		if ( gateway_id == game->GetWorld ()->GetPlayer ()->gateway.id ) {

			std::ostrstream deathmsg;

			deathmsg << "Disavowed by Uplink Corporation at " << game->GetWorld ()->date.GetLongString ()
					 << "\nFor " << reason
					 << "\n" << '\x0';

			game->GameOver ( deathmsg.str () );

			deathmsg.rdbuf()->freeze( 0 );
			//delete [] deathmsg.str ();

		}
		else {

			// The player's old gateway was seized
			// Not quite as severe - not game over

			Message *m = new Message ();
			m->SetTo ( "PLAYER" );
			m->SetFrom ( "Uplink Corporation" );
			m->SetSubject ( "Gateway seizure" );
			m->SetBody ( "Your old gateway was recently seized by federal agents, shortly after you blew it up.\n"
						 "It seems that no evidence remains of the crime that you didn't commit.\n\n"
						 "A very lucky escape, if you ask us.\n" );
			m->Send ();

			// Uplink Corporation won't like you for this (it's reckless after all)
			// Other hackers love this - everybody knows you did it but
			// you got away with it scott free!

			game->GetWorld ()->GetPlayer ()->rating.ChangeNeuromancerScore ( NEUROMANCERCHANGE_GATEWAYNEARMISS );
			game->GetWorld ()->GetPlayer ()->rating.ChangeUplinkScore ( UPLINKCHANGE_GATEWAYNEARMISS );

		}

	}

}

void SeizeGatewayEvent::RunWarning ()
{

	//
	// Agents are near (2 of em!)
	//

	game->GetWorld ()->GetPlayer ()->gateway.IncrementProximity ( 2 );

}

char *SeizeGatewayEvent::GetShortString ()
{

	size_t shortstringsize = strlen(name) + 32;
	char *shortstring = new char [shortstringsize];
	UplinkSnprintf ( shortstring, shortstringsize, "Seize Gateway of %s", name );
	return shortstring;

}

char *SeizeGatewayEvent::GetLongString ()
{

	std::ostrstream longstring;
	longstring << "Seize Gateway Event\n"
			   << "For : " << name << "\n"
			   << "GatewayID : " << gateway_id << "\n"
			   << "Reason : " << reason
			   << '\x0';
	return longstring.str ();

}


bool SeizeGatewayEvent::Load  ( FILE *file )
{

	LoadID ( file );

	if ( !UplinkEvent::Load ( file ) ) return false;

	if ( strcmp( game->GetLoadedSavefileVer(), "SAV59" ) >= 0 ) {
		if ( !LoadDynamicStringStatic ( name, sizeof(name), file ) ) return false;
	}
	else {
		if ( !FileReadData ( name, sizeof(name), 1, file ) ) {
			name [ sizeof(name) - 1 ] = '\0';
			return false;
		}
		name [ sizeof(name) - 1 ] = '\0';
	}
	if ( !LoadDynamicStringPtr ( &reason, file ) ) return false;
	if ( !FileReadData ( &gateway_id, sizeof(gateway_id), 1, file ) ) return false;

	LoadID_END ( file );

	return true;

}

void SeizeGatewayEvent::Save  ( FILE *file )
{

	SaveID ( file );

	UplinkEvent::Save ( file );

	SaveDynamicString ( name, sizeof(name), file );
	SaveDynamicString ( reason, file );
	fwrite ( &gateway_id, sizeof(gateway_id), 1, file );

	SaveID_END ( file );

}

void SeizeGatewayEvent::Print ()
{

	printf ( "Seize Gateway Event for %s, Gateway no. %d:\n", name, gateway_id );
	UplinkEvent::Print ();
	printf ( "%s\n", reason );


}

char *SeizeGatewayEvent::GetID ()
{

	return "EVT_SEZ";

}

int   SeizeGatewayEvent::GetOBJECTID ()
{

	return OID_SEIZEGATEWAYEVENT;

}

