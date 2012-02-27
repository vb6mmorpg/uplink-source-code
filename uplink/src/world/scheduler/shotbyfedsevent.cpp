
#include <strstream>

#include "gucci.h"

#include "app/app.h"
#include "app/serialise.h"
#include "app/globals.h"

#include "game/game.h"

#include "world/world.h"
#include "world/scheduler/shotbyfedsevent.h"
#include "world/generator/consequencegenerator.h"

#include "options/options.h"

#include "mmgr.h"


ShotByFedsEvent::ShotByFedsEvent ()
{

	reason = NULL;
	memset ( name, 0, sizeof ( name ) );

}

ShotByFedsEvent::~ShotByFedsEvent ()
{

	if ( reason ) delete [] reason;

}

void ShotByFedsEvent::SetName ( char *newname )
{

	UplinkAssert ( strlen (newname) < SIZE_PERSON_NAME );
	UplinkStrncpy ( name, newname, sizeof ( name ) );

}

void ShotByFedsEvent::SetReason ( char *newreason )
{

	if ( reason )
		delete [] reason;
	reason = new char [strlen(newreason)+1];
	UplinkSafeStrcpy ( reason, newreason );

}

void ShotByFedsEvent::Run ()
{

	Person *person = game->GetWorld ()->GetPerson ( name );
	UplinkAssert (person);

	person->SetStatus ( PERSON_STATUS_DEAD );

	ConsequenceGenerator::ShotByFeds ( person, reason );

}

char *ShotByFedsEvent::GetShortString ()
{

	size_t shortstringsize = strlen(name) + 32;
	char *shortstring = new char [shortstringsize];
	UplinkSnprintf ( shortstring, shortstringsize, "ShotByFeds for %s", name );
	return shortstring;

}

char *ShotByFedsEvent::GetLongString ()
{

	std::ostrstream longstring;
	longstring << "Shot By Feds Event\n"
			   << "For : " << name << "\n"
			   << "Reason : " << reason
			   << '\x0';
	return longstring.str ();

}

bool ShotByFedsEvent::Load  ( FILE *file )
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

	LoadID_END ( file );

	return true;

}

void ShotByFedsEvent::Save  ( FILE *file )
{

	SaveID ( file );

	UplinkEvent::Save ( file );

	SaveDynamicString ( name, sizeof(name), file );
	SaveDynamicString ( reason, file );

	SaveID_END ( file );

}

void ShotByFedsEvent::Print ()
{

	printf ( "ShotByFedsEvent : for %s\n", name );
	UplinkEvent::Print ();
	printf ( "Reason : %s\n", reason );

}

char *ShotByFedsEvent::GetID ()
{

	return "EVT_SHOT";

}

int   ShotByFedsEvent::GetOBJECTID ()
{

	return OID_SHOTBYFEDSEVENT;

}
