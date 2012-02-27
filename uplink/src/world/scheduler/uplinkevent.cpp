
#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"

#include "world/scheduler/uplinkevent.h"

#include "mmgr.h"


UplinkEvent::UplinkEvent ()
{
}

UplinkEvent::~UplinkEvent ()
{
}
	
void UplinkEvent::SetRunDate ( Date *newrundate )
{

	UplinkAssert (newrundate);
	rundate.SetDate ( newrundate );

}
	
void UplinkEvent::Run ()
{

	UplinkAbort ( "This MUST be overridden" );

}

void UplinkEvent::RunWarning ()
{
}

char *UplinkEvent::GetShortString ()
{

	size_t shortstringsize = 32;
	char *shortstring = new char [shortstringsize];
	UplinkStrncpy ( shortstring, "Uplink Event", shortstringsize );
	return shortstring;
		
}

char *UplinkEvent::GetLongString ()
{

	size_t longstringsize = 32;
	char *longstring = new char [longstringsize];
	UplinkStrncpy ( longstring, "Uplink Event", longstringsize );
	return longstring;

}

bool UplinkEvent::Load  ( FILE *file )
{

	LoadID ( file );
	
	if ( !rundate.Load ( file ) ) return false;

	LoadID_END ( file );

	return true;
		
}

void UplinkEvent::Save  ( FILE *file )
{

	SaveID ( file );

	rundate.Save ( file );

	SaveID_END ( file );

}

void UplinkEvent::Print ()
{

	printf ( "UplinkEvent:\n" );
	rundate.Print ();

}
	
char *UplinkEvent::GetID ()
{

	UplinkAbort ( "This MUST be overridden" );
	return NULL;

}

int UplinkEvent::GetOBJECTID ()
{

	UplinkAbort ( "This MUST be overridden" );
	return -1;

}
