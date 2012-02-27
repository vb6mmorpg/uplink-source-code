// BBSScreen.cpp: implementation of the BBSScreen class.
//
//////////////////////////////////////////////////////////////////////


#include "app/serialise.h"
#include "world/computer/computerscreen/bbsscreen.h"

#include "mmgr.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BBSScreen::BBSScreen()
{
	nextpage = -1;
	contactpage = -1;
}

BBSScreen::~BBSScreen()
{

}

void BBSScreen::SetNextPage ( int newnextpage )
{

	nextpage = newnextpage;

}

void BBSScreen::SetContactPage ( int newcontactpage )
{

	contactpage = newcontactpage;

}

bool BBSScreen::Load ( FILE *file )
{

	LoadID ( file );

	if ( !ComputerScreen::Load ( file ) ) return false;

	if ( !FileReadData ( &nextpage, sizeof(nextpage), 1, file ) ) return false;
	if ( !FileReadData ( &contactpage, sizeof(contactpage), 1, file ) ) return false;

	LoadID_END ( file );

	return true;

}

void BBSScreen::Save ( FILE *file )
{

	SaveID ( file );

	ComputerScreen::Save ( file );

	fwrite ( &nextpage, sizeof(nextpage), 1, file );
	fwrite ( &contactpage, sizeof(contactpage), 1, file );

	SaveID_END ( file );

}

void BBSScreen::Print ()
{

	printf ( "BBSScreen\n" );
	ComputerScreen::Print ();
	printf ( "Nextpage:%d, Contactpage:%d\n", nextpage, contactpage );

}
	
char *BBSScreen::GetID ()
{
	
	return "SCR_BBS";

}

int BBSScreen::GetOBJECTID ()
{

	return OID_BBSSCREEN;

}			
