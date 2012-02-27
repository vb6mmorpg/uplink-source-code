
#include <time.h>

#include "eclipse.h"
#include "soundgarden.h"

#include "app/app.h"
#include "app/opengl.h"
#include "app/opengl_interface.h"

#include "mainmenu/mainmenu.h"
#include "mainmenu/connectionlost_interface.h"

#include "options/options.h"

#include "mmgr.h"



ConnectionLostInterface::ConnectionLostInterface ()
{

    starttime = 0;

}

ConnectionLostInterface::~ConnectionLostInterface ()
{
}

void ConnectionLostInterface::ReturnToMainMenuClick ( Button *button )
{

	app->GetMainMenu ()->RunScreen ( MAINMENU_LOGIN );

}

void ConnectionLostInterface::Create ()
{

	if ( !IsVisible () ) {

//		SgStopMod ();

		EclRegisterButton ( 220, 230, 450, 200, " ", "", "connectionlost" );
		EclRegisterButtonCallbacks ( "connectionlost", textbutton_draw, NULL, NULL, NULL );

		starttime = time(NULL);

	}


}

void ConnectionLostInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "connectionlost" );
		EclRemoveButton ( "connectionlost_mainmenu" );

	}

}

void ConnectionLostInterface::Update ()
{

	if ( IsVisible () ) {

		if ( starttime != -1 && time(NULL) > starttime + 4 ) {

			if ( strcmp ( EclGetButton ( "connectionlost" )->caption, " " ) == 0 ) {
				
				EclRegisterCaptionChange ( "connectionlost", "Connection to GATEWAY lost" );
				starttime = time(NULL);				
                
			}
			else {

				// Exit button

				int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
				int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");

				EclRegisterButton ( screenw - 40, screenh - 40, 32, 32, "", "Return to Main Menu", "connectionlost_mainmenu" );
				EclRegisterButtonCallback ( "connectionlost_mainmenu", ReturnToMainMenuClick );
				button_assignbitmaps ( "connectionlost_mainmenu", "mainmenu/exitgame.tif", "mainmenu/exitgame_h.tif", "mainmenu/exitgame_c.tif" );

				starttime = -1;

			}

		}

	}

}

bool ConnectionLostInterface::IsVisible ()
{

	return ( EclGetButton ( "connectionlost" ) != NULL );

}


int  ConnectionLostInterface::ScreenID ()
{
	return MAINMENU_CONNECTIONLOST;
}
