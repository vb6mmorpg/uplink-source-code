// PasswordScreen.cpp: implementation of the PasswordScreen class.
//
//////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdio.h>

#include "gucci.h"

#include "game/game.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"

#include "world/computer/computerscreen/passwordscreen.h"

#include "mmgr.h"


PasswordScreen::PasswordScreen()
{

	nextpage = -1;
	difficulty = 1;
	memset( password, 0, sizeof ( password ) );

}

PasswordScreen::~PasswordScreen()
{

}

void PasswordScreen::SetNextPage ( int newnextpage )
{

	nextpage = newnextpage;

}

void PasswordScreen::SetPassword ( char *newpassword )
{

	UplinkAssert ( strlen (newpassword) < SIZE_PASSWORDSCREEN_PASSWORD );
	UplinkStrncpy ( password, newpassword, sizeof ( password ) );

}

void PasswordScreen::SetDifficulty ( int newdifficulty )
{

	difficulty = newdifficulty;

}

bool PasswordScreen::Load  ( FILE *file )
{

	LoadID ( file );

	if ( !ComputerScreen::Load ( file ) ) return false;

	if ( !FileReadData ( &nextpage, sizeof(nextpage), 1, file ) ) return false;

	if ( strcmp( game->GetLoadedSavefileVer(), "SAV59" ) >= 0 ) {
		if ( !LoadDynamicStringStatic ( password, sizeof(password), file ) ) return false;
	}
	else {
		if ( !FileReadData ( password, sizeof(password), 1, file ) ) {
			password [ sizeof(password) - 1 ] = '\0';
			return false;
		}
		password [ sizeof(password) - 1 ] = '\0';
	}

	if ( !FileReadData ( &difficulty, sizeof(difficulty), 1, file ) ) return false;

	LoadID_END ( file );

	return true;

}

void PasswordScreen::Save  ( FILE *file )
{

	SaveID ( file );

	ComputerScreen::Save ( file );

	fwrite ( &nextpage, sizeof(nextpage), 1, file );
	SaveDynamicString ( password, sizeof(password), file );
	fwrite ( &difficulty, sizeof(difficulty), 1, file );

	SaveID_END ( file );

}

void PasswordScreen::Print ()
{

	printf ( "PasswordScreen : \n" );
	ComputerScreen::Print ();
	printf ( "Nextpage = %d, Password = %s, Difficulty = %d\n", nextpage, password, difficulty );

}
	
char *PasswordScreen::GetID ()
{

	return "SCR_PASS";

}

int PasswordScreen::GetOBJECTID ()
{

	return OID_PASSWORDSCREEN;

}

