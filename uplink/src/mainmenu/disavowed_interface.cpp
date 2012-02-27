
#include <time.h>

#include "eclipse.h"
#include "soundgarden.h"

#include "app/app.h"
#include "app/opengl.h"
#include "app/opengl_interface.h"

#include "mainmenu/mainmenu.h"
#include "mainmenu/disavowed_interface.h"

#include "options/options.h"

#include "mmgr.h"



DisavowedInterface::DisavowedInterface ()
{
}

DisavowedInterface::~DisavowedInterface ()
{
}

void DisavowedInterface::ReturnToMainMenuClick ( Button *button )
{

	app->GetMainMenu ()->RunScreen ( MAINMENU_LOGIN );

}

void DisavowedInterface::Create ()
{

	if ( !IsVisible () ) {

//		SgStopMod ();

		EclRegisterButton ( 120, 130, 450, 200, " ", "", "disavowed" );
		EclRegisterButtonCallbacks ( "disavowed", textbutton_draw, NULL, NULL, NULL );

		starttime = time(NULL);

	}


}

void DisavowedInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "disavowed" );
		EclRemoveButton ( "disavowed_mainmenu" );

	}

}

void DisavowedInterface::Update ()
{

	if ( IsVisible () ) {

		if ( starttime != -1 && time(NULL) > starttime + 4 ) {

			if ( strcmp ( EclGetButton ( "disavowed" )->caption, " " ) == 0 ) {

				EclRegisterCaptionChange ( "disavowed", "We have recently been informed by a large corporation that you have \n"
													 	"been caught hacking into one of their primary computer systems.\n\n"
														"As such, we have been forced to disavow all knowledge of your actions \n"
														"and strike you off the active agents list.\n\n"
														"Your gateway and accounts have already been siezed.\n\n"
														"Let this be a warning to be more careful next time." );
				starttime = time(NULL);				

			}
			else {

				// Exit button

				int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
				int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");

				EclRegisterButton ( screenw - 40, screenh - 40, 32, 32, "", "Return to Main Menu", "disavowed_mainmenu" );
				EclRegisterButtonCallback ( "disavowed_mainmenu", ReturnToMainMenuClick );
				button_assignbitmaps ( "disavowed_mainmenu", "mainmenu/exitgame.tif", "mainmenu/exitgame_h.tif", "mainmenu/exitgame_c.tif" );

				starttime = -1;

			}

		}

	}

}

bool DisavowedInterface::IsVisible ()
{

	return ( EclGetButton ( "disavowed" ) != NULL );

}


int  DisavowedInterface::ScreenID ()
{
	return MAINMENU_DISAVOWED;
}
