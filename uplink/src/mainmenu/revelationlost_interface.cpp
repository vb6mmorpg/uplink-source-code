
#include <time.h>

#include "eclipse.h"
#include "soundgarden.h"

#include "app/app.h"
#include "app/opengl.h"
#include "app/opengl_interface.h"

#include "mainmenu/mainmenu.h"
#include "mainmenu/revelationlost_interface.h"

#include "options/options.h"

#include "mmgr.h"



RevelationLostInterface::RevelationLostInterface ()
{
}

RevelationLostInterface::~RevelationLostInterface ()
{
}

void RevelationLostInterface::ReturnToMainMenuClick ( Button *button )
{

	app->GetMainMenu ()->RunScreen ( MAINMENU_LOGIN );

}

void RevelationLostInterface::Create ()
{

	if ( !IsVisible () ) {

//		SgStopMod ();

		EclRegisterButton ( 120, 130, 450, 200, " ", "", "revelationlost" );
		EclRegisterButtonCallbacks ( "revelationlost", textbutton_draw, NULL, NULL, NULL );

		starttime = time(NULL);

	}


}

void RevelationLostInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "revelationlost" );
		EclRemoveButton ( "revelationlost_mainmenu" );

	}

}

void RevelationLostInterface::Update ()
{

	if ( IsVisible () ) {

		if ( starttime != -1 && time(NULL) > starttime + 4 ) {

			if ( strcmp ( EclGetButton ( "revelationlost" )->caption, " " ) == 0 ) {
				
				EclRegisterCaptionChange ( "revelationlost", "We have terminated your connection to your Gateway.\n\n"
                                                             "We have been watching your moves for the last few days, \n"
                                                             "and we have known for some time that you were working for \n"
                                                             "Andromeda Research Corporation.\n\n"
                                                             "Recent actions of that company, and your part in those actions, \n"
                                                             "have forced us to disavow your agent profile and knock you off \n"
                                                             "the active agent list.\n\n"
                                                             "Despite the damage you have caused, Revelation has not succeeded.\n\n"
                                                             "We hope you will think more carefully about your actions next time." );
				starttime = time(NULL);				

			}
			else {

				// Exit button

				int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
				int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");

				EclRegisterButton ( screenw - 40, screenh - 40, 32, 32, "", "Return to Main Menu", "revelationlost_mainmenu" );
				EclRegisterButtonCallback ( "revelationlost_mainmenu", ReturnToMainMenuClick );
				button_assignbitmaps ( "revelationlost_mainmenu", "mainmenu/exitgame.tif", "mainmenu/exitgame_h.tif", "mainmenu/exitgame_c.tif" );

				starttime = -1;

			}

		}

	}

}

bool RevelationLostInterface::IsVisible ()
{

	return ( EclGetButton ( "revelationlost" ) != NULL );

}


int  RevelationLostInterface::ScreenID ()
{

	return MAINMENU_REVELATIONLOST;

}
