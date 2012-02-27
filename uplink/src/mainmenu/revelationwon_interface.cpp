
#include <time.h>

#include "eclipse.h"
#include "soundgarden.h"

#include "app/app.h"
#include "app/opengl.h"
#include "app/opengl_interface.h"

#include "mainmenu/mainmenu.h"
#include "mainmenu/revelationwon_interface.h"

#include "options/options.h"

#include "mmgr.h"



RevelationWonInterface::RevelationWonInterface ()
{
}

RevelationWonInterface::~RevelationWonInterface ()
{
}

void RevelationWonInterface::ReturnToMainMenuClick ( Button *button )
{

	app->GetMainMenu ()->RunScreen ( MAINMENU_LOGIN );

}

void RevelationWonInterface::Create ()
{

	if ( !IsVisible () ) {

//		SgStopMod ();

		EclRegisterButton ( 180, 130, 450, 350, " ", "", "revelationwon" );
		EclRegisterButtonCallbacks ( "revelationwon", textbutton_draw, NULL, NULL, NULL );

		starttime = time(NULL);

	}


}

void RevelationWonInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "revelationwon" );
		EclRemoveButton ( "revelationwon_mainmenu" );

	}

}

void RevelationWonInterface::Update ()
{

	if ( IsVisible () ) {

		if ( starttime != -1 && time(NULL) > starttime + 4 ) {

			if ( strcmp ( EclGetButton ( "revelationwon" )->caption, " " ) == 0 ) {
				
				EclRegisterCaptionChange ( "revelationwon", "There appears to be a serious problem\n"
                                                            "with our Core network service providers.\n\n"
                                                            "This may be the result of a virus.\n\n"
                                                            "We will try to restore serBVs sasa XXXX sas\n"
                                                            "00sd00001010001  101 1 1011 10 \n\n\n"
                                                            "1001 101 10 101000 1010 0 10010 010\n"
                                                            "01010 0010 1001 1010 01001010 01 01 0\n"
                                                            "01 1010 0 10110 00 010 01 0010 001010 \n"
                                                            "01 010 012 0120 12 10 010 010 010 010 0\n\n"
                                                            "00100 01 01 0 01 01 101 010 0010 01 0\n"
                                                            "1001 101 10 101000 1010 0 10010 010\n"
                                                            "01010 0010 1001 1010 01001010 01 01 0\n"
                                                            "01 1010 0 10110 00 010 01 0010 001010 \n"
                                                            "01 010 01 010 10 1 10 010 010 010 010 0\n"
                                                            "00100 01 01 0 01 01 101 010 0010 01 0\n"
                                                            "1001 101 10 1010001 01 1 00 110 010\n"
                                                            "01010 0010 1001 1010 01001010 01 01 0\n"
                                                            "01 1010 0 10110 00 010 01 0010 001010 \n"
                                                            "01101 100 1  010  0 10 1 0 010 10 110 0\n"
                                                            "00100 01 01 0 01 01 101 010 0010 01 0\n"
                                                            "1001 101 10 101000 1010 0 10010 010\n"
                                                            "01010 0010 1001 1010 01001010 01 01 0\n"
                                                            "01 1010 0 10110 00 010 01 0010 001010 \n"
                                                            "01 010 012 0120 12 10 010 010 010 010 0\n"
                                                            "00100 110 1001 010 01001 1000010 01 0\n"
                                                            "1001 101 10 101000 1010 0 10010 010\n"
                                                            "01010 0010 1001 1010 01001010 01 01 0\n"
                                                            "01 1010 0 10110 00 010 01 0010 001010 \n"
                                                            "01 010 012 0120 12 10 010 010 010 010 0\n");
				starttime = time(NULL);				

			}
			else {

				// Exit button

				int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
				int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");

				EclRegisterButton ( screenw - 40, screenh - 40, 32, 32, "", "Return to Main Menu", "revelationwon_mainmenu" );
				EclRegisterButtonCallback ( "revelationwon_mainmenu", ReturnToMainMenuClick );
				button_assignbitmaps ( "revelationwon_mainmenu", "mainmenu/exitgame.tif", "mainmenu/exitgame_h.tif", "mainmenu/exitgame_c.tif" );

				starttime = -1;

			}

		}

	}

}

bool RevelationWonInterface::IsVisible ()
{

	return ( EclGetButton ( "revelationwon" ) != NULL );

}


int  RevelationWonInterface::ScreenID ()
{

	return MAINMENU_REVELATIONWON;

}
