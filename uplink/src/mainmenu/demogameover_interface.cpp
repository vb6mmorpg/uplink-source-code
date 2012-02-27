
#include <time.h>

#include "eclipse.h"
#include "soundgarden.h"

#include "app/app.h"
#include "app/opengl.h"
#include "app/opengl_interface.h"

#include "mainmenu/mainmenu.h"
#include "mainmenu/demogameover_interface.h"

#include "options/options.h"

#include "mmgr.h"



DemoGameOverInterface::DemoGameOverInterface ()
{
}

DemoGameOverInterface::~DemoGameOverInterface ()
{
}

void DemoGameOverInterface::ReturnToMainMenuClick ( Button *button )
{

	app->GetMainMenu ()->RunScreen ( MAINMENU_LOGIN );

}

void DemoGameOverInterface::Create ()
{

	if ( !IsVisible () ) {

//		SgStopMod ();

		EclRegisterButton ( 120, 130, 450, 300, " ", "", "demogameover" );
		EclRegisterButtonCallbacks ( "demogameover", textbutton_draw, NULL, NULL, NULL );

		starttime = time(NULL);

	}


}

void DemoGameOverInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "demogameover" );
		EclRemoveButton ( "demogameover_mainmenu" );

	}

}

void DemoGameOverInterface::Update ()
{

	if ( IsVisible () ) {

		if ( starttime != -1 && time(NULL) > starttime + 4 ) {

			if ( strcmp ( EclGetButton ( "demogameover" )->caption, " " ) == 0 ) {

				EclRegisterCaptionChange ( "demogameover", "Uplink Corporation has detected that you are still using\n"
                                                           "the Demo Uplink Client, and that you have reached the highest\n"
                                                           "attainable Uplink rating for a Demo user.\n\n"
                                                           "For security reasons, Uplink Corporation cannot allow you to continue\n"
                                                           "working for our company as a freelance Agent.\n"
                                                           "As such, we have terminated your Agent status\n"
                                                           "and closed down your accounts.\n\n"
                                                           "If you are interested in becoming a full-time Uplink Agent,\n"
                                                           "you will need to purchase the full Uplink CD.\n"
                                                           "Details can be found on our web site.\n\n"
                                                           "www.introversion.co.uk\n\n" );
				starttime = time(NULL);				

			}
			else {

				// Exit button

				int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
				int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");

				EclRegisterButton ( screenw - 40, screenh - 40, 32, 32, "", "Return to Main Menu", "demogameover_mainmenu" );
				EclRegisterButtonCallback ( "demogameover_mainmenu", ReturnToMainMenuClick );
				button_assignbitmaps ( "demogameover_mainmenu", "mainmenu/exitgame.tif", "mainmenu/exitgame_h.tif", "mainmenu/exitgame_c.tif" );

				starttime = -1;

			}

		}

	}

}

bool DemoGameOverInterface::IsVisible ()
{

	return ( EclGetButton ( "demogameover" ) != NULL );

}


int  DemoGameOverInterface::ScreenID ()
{
	return MAINMENU_DEMOGAMEOVER;
}
