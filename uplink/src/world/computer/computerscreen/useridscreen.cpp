
#include "app/serialise.h"

#include "world/computer/computerscreen/useridscreen.h"

#include "mmgr.h"


UserIDScreen::UserIDScreen ()
{

	nextpage = -1;
	difficulty = 1;
}

UserIDScreen::~UserIDScreen ()
{
}

void UserIDScreen::SetNextPage ( int newnextpage )
{
	
	nextpage = newnextpage;

}

void UserIDScreen::SetDifficulty ( int newdifficulty )
{

	difficulty = newdifficulty;

}

bool UserIDScreen::Load  ( FILE *file )
{

	LoadID ( file );

	if ( !ComputerScreen::Load ( file ) ) return false;

	if ( !FileReadData ( &nextpage, sizeof(nextpage), 1, file ) ) return false;
	if ( !FileReadData ( &difficulty, sizeof(difficulty), 1, file ) ) return false;

	LoadID_END ( file );

	return true;

}

void UserIDScreen::Save  ( FILE *file )
{

	SaveID ( file );

	ComputerScreen::Save ( file );

	fwrite ( &nextpage, sizeof(nextpage), 1, file );
	fwrite ( &difficulty, sizeof(difficulty), 1, file );

	SaveID_END ( file );

}

void UserIDScreen::Print ()
{
	
	printf ( "UserIDScreen : \n" );
	ComputerScreen::Print ();
	printf ( "\tNextPage:%d, Difficulty:%d\n", nextpage, difficulty );

}

char *UserIDScreen::GetID ()
{

	return "SCR_UID";

}

int UserIDScreen::GetOBJECTID ()
{

	return OID_USERIDSCREEN;

}
