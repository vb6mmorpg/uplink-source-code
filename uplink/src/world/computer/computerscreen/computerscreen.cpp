// ComputerScreen.cpp: implementation of the ComputerScreen class.
//
//////////////////////////////////////////////////////////////////////

#include "gucci.h"

#include "game/game.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"

#include "world/world.h"
#include "world/computer/computerscreen/computerscreen.h"

#include "mmgr.h"



ComputerScreen::ComputerScreen()
{

	UplinkStrncpy ( maintitle, " ", sizeof ( maintitle ) );
	UplinkStrncpy ( subtitle, " ", sizeof ( subtitle ) );
	UplinkStrncpy ( computer, " ", sizeof ( computer ) );

}

ComputerScreen::~ComputerScreen()
{

}

void ComputerScreen::SetMainTitle ( char *newmaintitle )
{

	UplinkAssert ( strlen(newmaintitle) < SIZE_COMPUTERSCREEN_MAINTITLE );
	UplinkStrncpy ( maintitle, newmaintitle, sizeof ( maintitle ) );

}

void ComputerScreen::SetSubTitle ( char *newsubtitle )
{

	UplinkAssert ( strlen(newsubtitle) < SIZE_COMPUTERSCREEN_SUBTITLE );
	UplinkStrncpy ( subtitle, newsubtitle, sizeof ( subtitle ) );

}

void ComputerScreen::SetComputer ( char *newcomputer )
{

	UplinkAssert ( strlen(newcomputer) < SIZE_COMPUTER_NAME );
	UplinkStrncpy ( computer, newcomputer, sizeof ( computer ) );

}

Computer *ComputerScreen::GetComputer ()
{

	return game->GetWorld ()->GetComputer ( computer );

}

bool ComputerScreen::Load  ( FILE *file )
{

	LoadID ( file );

	if ( !LoadDynamicStringStatic ( maintitle, SIZE_COMPUTERSCREEN_MAINTITLE, file ) ) return false;
	if ( !LoadDynamicStringStatic ( subtitle, SIZE_COMPUTERSCREEN_SUBTITLE, file ) ) return false;
	if ( !LoadDynamicStringStatic ( computer, SIZE_COMPUTER_NAME, file ) ) return false;

	LoadID_END ( file );

	return true;

}

void ComputerScreen::Save  ( FILE *file )
{

	SaveID ( file );

	SaveDynamicString ( maintitle, SIZE_COMPUTERSCREEN_MAINTITLE, file );
	SaveDynamicString ( subtitle, SIZE_COMPUTERSCREEN_SUBTITLE, file );
	SaveDynamicString ( computer, SIZE_COMPUTER_NAME, file );

	SaveID_END ( file );

}

void ComputerScreen::Print ()
{

	printf ( "MainTitle : %s\n", maintitle );
	printf ( "SubTitle  : %s\n", subtitle );

	printf ( "Computer  : %s\n", computer );

}

void ComputerScreen::Update ()
{
}

char *ComputerScreen::GetID ()
{

	return "COMPSCR";

}

int ComputerScreen::GetOBJECTID ()
{
	
	return OID_UNKNOWN;

}

