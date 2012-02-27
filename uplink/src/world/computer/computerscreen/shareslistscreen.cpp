

#include "app/serialise.h"

#include "interface/remoteinterface/remoteinterface.h"

#include "world/computer/computerscreen/shareslistscreen.h"

#include "mmgr.h"



SharesListScreen::SharesListScreen ()
{
	
	nextpage = -1;
	SCREENTYPE = SHARESLISTSCREENTYPE_NONE;

}

SharesListScreen::~SharesListScreen ()
{
}

void SharesListScreen::SetNextPage ( int newnextpage )
{
	
	nextpage = newnextpage;

}

void SharesListScreen::SetViewPage ( int newviewpage )
{

	viewpage = newviewpage;

}

void SharesListScreen::SetScreenType ( int newSCREEN_TYPE )
{
	
	SCREENTYPE = newSCREEN_TYPE;

}

bool SharesListScreen::Load ( FILE *file )
{

	LoadID ( file );

	if ( !ComputerScreen::Load ( file ) ) return false;

	if ( !FileReadData ( &nextpage, sizeof(nextpage), 1, file ) ) return false;
	if ( !FileReadData ( &viewpage, sizeof(viewpage), 1, file ) ) return false;
	if ( !FileReadData ( &SCREENTYPE, sizeof(SCREENTYPE), 1, file ) ) return false;

	LoadID_END ( file );

	return true;

}

void SharesListScreen::Save ( FILE *file )
{

	SaveID ( file );

	ComputerScreen::Save ( file );

	fwrite ( &nextpage, sizeof(nextpage), 1, file );
	fwrite ( &viewpage, sizeof(viewpage), 1, file );
	fwrite ( &SCREENTYPE, sizeof(SCREENTYPE), 1, file );

	SaveID_END ( file );

}

void SharesListScreen::Print ()
{
	
	printf ( "SharesListScreen :\n" );
	ComputerScreen::Print ();
	printf ( "TYPE=%d, nextpage=%d, viewpage=%d\n", SCREENTYPE, nextpage, viewpage );

}
	
char *SharesListScreen::GetID ()
{
	
	return "SCR_SHAR";

}

int SharesListScreen::GetOBJECTID ()
{

	return OID_SHARESLISTSCREEN;

}
