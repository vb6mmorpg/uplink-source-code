
#include <strstream>

#include "app/globals.h"
#include "app/serialise.h"

#include "world/scheduler/warningevent.h"

#include "mmgr.h"


WarningEvent::WarningEvent ()
{

	event = NULL;

}

WarningEvent::~WarningEvent ()
{
}


void WarningEvent::SetEvent ( UplinkEvent *newevent )
{

	event = newevent;

}


void WarningEvent::Run ()
{

	if ( event )		event->RunWarning ();

}

char *WarningEvent::GetShortString ()
{

	size_t shortstringsize = 32;
	char *shortstring = new char [shortstringsize];
	UplinkStrncpy ( shortstring, "Warning Event", shortstringsize );
	return shortstring;

}

char *WarningEvent::GetLongString ()
{

	char *eventstring = event->GetLongString ();

	std::ostrstream longstring;
	longstring << "Warning Event for :\n"
			   << eventstring
			   << '\x0';

	delete [] eventstring;

	return longstring.str ();

}

bool WarningEvent::Load  ( FILE *file )
{

	LoadID ( file );

	event = NULL;

	if  ( !UplinkEvent::Load ( file ) ) return false;

	bool eventexists;
	if ( !FileReadData ( &eventexists, sizeof(eventexists), 1, file ) ) return false;

	if ( eventexists ) {
		int objectID;
		if ( !FileReadData ( &objectID, sizeof(objectID), 1, file ) ) return false;
		event = (UplinkEvent *) CreateUplinkObject ( objectID );
		if ( !event ) return false;
		if ( !event->Load ( file ) ) return false;
	}

	LoadID_END ( file );

	return true;

}

void WarningEvent::Save  ( FILE *file )
{

	SaveID ( file );

	UplinkEvent::Save ( file );

	bool eventexists = ( event != NULL );
	fwrite ( &eventexists, sizeof(eventexists), 1, file );

	if ( eventexists ) {
		int objectID = event->GetOBJECTID ();
		fwrite ( &objectID, sizeof(objectID), 1, file );
		event->Save ( file );
	}

	SaveID_END ( file );

}

void WarningEvent::Print ()
{

	printf ( "Warning Event : \n" );

	UplinkEvent::Print ();

	if ( event ) event->Print ();

}

char *WarningEvent::GetID ()
{

	return "EVT_WARN";

}


int   WarningEvent::GetOBJECTID ()
{

	return OID_WARNINGEVENT;

}
