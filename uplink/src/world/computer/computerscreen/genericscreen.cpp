
#include "app/serialise.h"

#include "interface/remoteinterface/remoteinterface.h"

#include "world/computer/computerscreen/genericscreen.h"

#include "mmgr.h"



GenericScreen::GenericScreen ()
{
	
	nextpage = -1;
	SCREEN_TYPE = SCREEN_UNKNOWN;

}

GenericScreen::~GenericScreen ()
{
}

void GenericScreen::SetNextPage ( int newnextpage )
{
	
	nextpage = newnextpage;

}

void GenericScreen::SetScreenType ( int newSCREEN_TYPE )
{
	
	SCREEN_TYPE = newSCREEN_TYPE;

}

bool GenericScreen::Load ( FILE *file )
{

	LoadID ( file );

	if ( !ComputerScreen::Load ( file ) ) return false;

	if ( !FileReadData ( &nextpage, sizeof(nextpage), 1, file ) ) return false;
	if ( !FileReadData ( &SCREEN_TYPE, sizeof(SCREEN_TYPE), 1, file ) ) return false;

	LoadID_END ( file );

	return true;

}

void GenericScreen::Save ( FILE *file )
{

	SaveID ( file );

	ComputerScreen::Save ( file );

	fwrite ( &nextpage, sizeof(nextpage), 1, file );
	fwrite ( &SCREEN_TYPE, sizeof(SCREEN_TYPE), 1, file );

	SaveID_END ( file );

}

void GenericScreen::Print ()
{
	
	printf ( "GenericScreen :\n" );
	ComputerScreen::Print ();
	printf ( "TYPE=%d, nextpage=%d\n", SCREEN_TYPE, nextpage );

}
	
char *GenericScreen::GetID ()
{
	
	return "SCR_GEN";

}

int GenericScreen::GetOBJECTID ()
{

	return OID_GENERICSCREEN;

}

