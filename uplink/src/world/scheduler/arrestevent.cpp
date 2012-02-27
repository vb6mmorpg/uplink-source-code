
#include <strstream>

#include "soundgarden.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"

#include "game/game.h"

#include "world/world.h"
#include "world/vlocation.h"
#include "world/generator/consequencegenerator.h"
#include "world/scheduler/arrestevent.h"

#include "options/options.h"

#include "mmgr.h"



ArrestEvent::ArrestEvent ()
{

	reason = NULL;
	ip = NULL;
	memset ( name, 0, sizeof ( name ) );

}

ArrestEvent::~ArrestEvent ()
{

	if ( reason ) delete [] reason;
	if ( ip ) delete [] ip;

}

void ArrestEvent::SetName ( char *newname )
{

	UplinkAssert ( strlen(newname) < SIZE_PERSON_NAME );
	UplinkStrncpy ( name, newname, sizeof ( name ) );

}

void ArrestEvent::SetReason ( char *newreason )
{

	if ( reason )
		delete [] reason;
	reason = new char [strlen(newreason)+1];
	UplinkSafeStrcpy ( reason, newreason );

}

void ArrestEvent::SetIP ( char *newip )
{

	if ( ip )
		delete [] ip;
	ip = new char [strlen(newip)+1];
	UplinkSafeStrcpy ( ip, newip );

}

void ArrestEvent::Run ()
{

	Person *person = game->GetWorld ()->GetPerson ( name );
	UplinkAssert (person);

	person->SetStatus ( PERSON_STATUS_INJAIL );

	// Look up the computer they messed with

	Computer *comp = NULL;

	if ( ip ) {
		VLocation *vl = game->GetWorld ()->GetVLocation ( ip );
		UplinkAssert (vl);
		comp = vl->GetComputer ();
		UplinkAssert (comp);
	}

	ConsequenceGenerator::Arrested ( person, comp, reason );

}

char *ArrestEvent::GetShortString ()
{

	size_t shortstringsize = strlen(name) + 16;
	char *shortstring = new char [shortstringsize];
	UplinkSnprintf ( shortstring, shortstringsize, "Arrest for %s", name );
	return shortstring;
}

char *ArrestEvent::GetLongString ()
{

	std::ostrstream longstring;
	longstring << "ArrestEvent:\n"
			   << "For : " << name << "\n"
			   << "Reason : " << reason
			   << '\x0';

	return longstring.str ();

}


bool ArrestEvent::Load  ( FILE *file )
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
	if ( !LoadDynamicStringPtr ( &ip, file) ) return false;

	LoadID_END ( file );

	return true;

}

void ArrestEvent::Save  ( FILE *file )
{

	SaveID ( file );

	UplinkEvent::Save ( file );

	SaveDynamicString ( name, sizeof(name), file );

	SaveDynamicString ( reason, file );
	SaveDynamicString ( ip, file );

	SaveID_END ( file );

}

void ArrestEvent::Print ()
{

	printf ( "Arrest Event for %s:\n", name );
	UplinkEvent::Print ();

	printf ( "%s\n", reason );
	if ( ip ) printf ( "IP:%s\n", ip );


}

char *ArrestEvent::GetID ()
{

	return "EVT_ARR";

}

int   ArrestEvent::GetOBJECTID ()
{

	return OID_ARRESTEVENT;

}

