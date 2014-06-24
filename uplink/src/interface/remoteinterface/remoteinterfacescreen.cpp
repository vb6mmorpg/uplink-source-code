// RemoteInterfaceScreen.cpp: implementation of the RemoteInterfaceScreen class.
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include "gucci.h"

#include "app/globals.h"
#include "app/app.h"
#include "app/miscutils.h"
#include "app/opengl_interface.h"

#include "options/options.h"

#include "game/game.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/remoteinterfacescreen.h"

#include "mmgr.h"


RemoteInterfaceScreen::RemoteInterfaceScreen()
{
	cs = NULL;
}

RemoteInterfaceScreen::~RemoteInterfaceScreen()
{
}

void RemoteInterfaceScreen::DrawMainTitle ( Button *button, bool highlighted, bool clicked )
{

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
	glEnable ( GL_SCISSOR_TEST );


    SetColour ( "MenuText" );
    int ypos = (button->y + button->height / 2) + 5;
	GciDrawText ( button->x, ypos, button->caption, HELVETICA_18 );


	glDisable ( GL_SCISSOR_TEST );

}

void RemoteInterfaceScreen::DrawSubTitle ( Button *button, bool highlighted, bool clicked )
{

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
	glEnable ( GL_SCISSOR_TEST );

	
	SetColour ( "DefaultText" );
	int ypos = (button->y + button->height / 2) + 5;
	GciDrawText ( button->x, ypos, button->caption, HELVETICA_12 );


	glDisable ( GL_SCISSOR_TEST );

}

bool RemoteInterfaceScreen::ReturnKeyPressed ()
{

	return false;

}

bool RemoteInterfaceScreen::EscapeKeyPressed ()
{

    return false;

}

void RemoteInterfaceScreen::Create ()
{

	if ( cs ) Create ( cs );
	else printf ( "Error : RemoteInterfaceScreen::Create, cs==NULL, ScreenID=%d\n", ScreenID () );

}

void RemoteInterfaceScreen::Create ( ComputerScreen *newcs )
{

	cs = newcs;

}

void RemoteInterfaceScreen::Remove ()
{
}

void RemoteInterfaceScreen::Update ()
{
}

bool RemoteInterfaceScreen::IsVisible ()
{
	return false;
}

ComputerScreen *RemoteInterfaceScreen::GetComputerScreen ()
{

	UplinkAssert ( cs );
	return cs;

}

int RemoteInterfaceScreen::ScreenID ()
{

	return 0;

}

RemoteInterfaceScreen *RemoteInterfaceScreen::GetInterfaceScreen ( int screenID )
{

	RemoteInterfaceScreen *result = game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert ( result );
	UplinkAssert ( screenID == result->ScreenID () );
	return result;

}

