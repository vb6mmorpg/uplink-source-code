
#include <time.h>

#include "eclipse.h"
#include "soundgarden.h"

#include "app/app.h"
#include "app/opengl.h"
#include "app/opengl_interface.h"

#include "mainmenu/mainmenu.h"
#include "mainmenu/warezgameover_interface.h"

#include "options/options.h"

#include "mmgr.h"



WarezGameOverInterface::WarezGameOverInterface ()
{
}

WarezGameOverInterface::~WarezGameOverInterface ()
{
}

void WarezGameOverInterface::ReturnToMainMenuClick ( Button *button )
{

	app->GetMainMenu ()->RunScreen ( MAINMENU_LOGIN );

}

void WarezGameOverInterface::Create ()
{

	if ( !IsVisible () ) {

//		SgStopMod ();

		EclRegisterButton ( 120, 130, 450, 300, " ", "", "warezgameover" );
		EclRegisterButtonCallbacks ( "warezgameover", textbutton_draw, NULL, NULL, NULL );

		starttime = time(NULL);

	}


}

void WarezGameOverInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "warezgameover" );
		EclRemoveButton ( "warezgameover_mainmenu" );

	}

}

void WarezGameOverInterface::Update ()
{

	if ( IsVisible () ) {

		if ( starttime != -1 && time(NULL) > starttime + 4 ) {

			if ( strcmp ( EclGetButton ( "warezgameover" )->caption, " " ) == 0 ) {

				EclRegisterCaptionChange ( "warezgameover", "Uplink Corporation has detected that you are using\n"
                                                            "an illegal copy of this product.\n\n"
                                                            "For security reasons, Uplink Corporation cannot allow you to continue\n"
                                                            "working for our company.\n"
                                                            "As such, we have terminated your Agent status\n"
                                                            "and closed down your accounts.\n\n"
                                                            "If you are interested in becoming an authorised Uplink Agent,\n"
                                                            "you will need to purchase the full Uplink CD.\n"
                                                            "Details can be found on our web site.\n\n"
                                                            "www.introversion.co.uk\n\n" );
				starttime = time(NULL);				

			}
			else {

				// Exit button

				int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
				int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");

				EclRegisterButton ( screenw - 40, screenh - 40, 32, 32, "", "Return to Main Menu", "warezgameover_mainmenu" );
				EclRegisterButtonCallback ( "warezgameover_mainmenu", ReturnToMainMenuClick );
				button_assignbitmaps ( "warezgameover_mainmenu", "mainmenu/exitgame.tif", "mainmenu/exitgame_h.tif", "mainmenu/exitgame_c.tif" );

				starttime = -1;

			}

		}

	}

}

bool WarezGameOverInterface::IsVisible ()
{

	return ( EclGetButton ( "warezgameover" ) != NULL );

}


int  WarezGameOverInterface::ScreenID ()
{
	return MAINMENU_WAREZGAMEOVER;
}
