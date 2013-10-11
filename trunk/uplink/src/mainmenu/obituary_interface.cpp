
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h> /* glu extention library */

#include "eclipse.h"
#include "soundgarden.h"
#include "redshirt.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"

#include "options/options.h"

#include "game/game.h"
#include "game/gameobituary.h"

#include "world/generator/plotgenerator.h"

#include "mainmenu/mainmenu.h"
#include "mainmenu/obituary_interface.h"

#include "mmgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void ObituaryInterface::MediumTextDraw ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert (button);

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
	glEnable ( GL_SCISSOR_TEST );

	clear_draw ( button->x, button->y, button->width, button->height );

	glColor4f ( 1.0f, 1.0f, 1.0f, ALPHA );    
	int xpos = button->x + 10;
	int ypos = (button->y + button->height / 2) + 3;
	GciDrawText ( xpos, ypos, button->caption, HELVETICA_12 );

	if ( highlighted || clicked ) border_draw ( button );

	glDisable ( GL_SCISSOR_TEST );

}

void ObituaryInterface::LargeTextDraw  ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert (button);

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
	glEnable ( GL_SCISSOR_TEST );

	clear_draw ( button->x, button->y, button->width, button->height );

	glColor4f ( 1.0f, 1.0f, 1.0f, ALPHA );    
	int xpos = button->x + 10;
	int ypos = (button->y + button->height / 2) + 3;
	GciDrawText ( xpos, ypos, button->caption, HELVETICA_18 );

	if ( highlighted || clicked ) border_draw ( button );

	glDisable ( GL_SCISSOR_TEST );

}


void ObituaryInterface::ReturnToMainMenuClick ( Button *button )
{

	app->GetMainMenu ()->RunScreen ( MAINMENU_LOGIN );

}

void ObituaryInterface::PiClick ( Button *button )
{

	app->GetMainMenu ()->RunScreen ( MAINMENU_THETEAM );

}

ObituaryInterface::ObituaryInterface()
{
}

ObituaryInterface::~ObituaryInterface()
{
}

void ObituaryInterface::Create ()
{	

	if ( !IsVisible () ) {

		if ( app->GetOptions ()->IsOptionEqualTo ( "sound_musicenabled", 1 ) ) {

            //SgPlaylist_Play ("sad");
            SgPlaylist_Play ("main");

		}

		MainMenuScreen::Create ();

		int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
		int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");

		GameObituary *gob = game->GetObituary ();

#ifdef GAME_WINNING_CODE

		if ( game->IsCodeWon () ) {

			char *winningCode = game->GetWinningCode ();
			char *winningDesc = game->GetWinningCodeDesc ();

			EclRegisterButton ( 20, 150, 400, 15, winningDesc, "", "obituary_winningcodedesc" );
			EclRegisterButtonCallbacks ( "obituary_winningcodedesc", MediumTextDraw, NULL, NULL, NULL );

			EclRegisterButton ( 20, 170, 400, 15, "Code: ", "", "obituary_winningcodelabel" );
			EclRegisterButtonCallbacks ( "obituary_winningcodelabel", MediumTextDraw, NULL, NULL, NULL );

			EclRegisterButton ( 60, 170, 400, 15, winningCode, "", "obituary_winningcode" );
			EclRegisterButtonCallbacks ( "obituary_winningcode", MediumTextDraw, NULL, NULL, NULL );

			delete [] winningDesc;
			delete [] winningCode;

		}

#endif
		
		char finances [32];
		UplinkSnprintf ( finances, sizeof ( finances ), "%d credits", gob->money );

		char name [128];
		UplinkSnprintf ( name, sizeof ( name ), "Agent %s", gob->name );

		EclRegisterButton ( 20, 200, 200, 40, name, "", "obituary_name" );
		EclRegisterButtonCallbacks ( "obituary_name", LargeTextDraw, NULL, NULL, NULL );

		EclRegisterButton ( 20, 240, 400, 50, gob->GameOverReason (), "obituary_text" );
		EclRegisterButtonCallbacks ( "obituary_text", textbutton_draw, NULL, NULL, NULL );

		//
		// Left column

		EclRegisterButton ( 20, 300, 140, 15, "Finances", "", "obituary_financestext" );
		EclRegisterButton ( 20, 320, 140, 15, "Uplink Rating", "", "obituary_uratingtext" );
		EclRegisterButton ( 20, 340, 140, 15, "Neuromancer Rating", "", "obituary_nratingtext" );

		EclRegisterButton ( 190, 300, 100, 15, finances, "", "obituary_finances" );
		EclRegisterButton ( 190, 320, 100, 15, Rating::GetUplinkRatingString ( gob->uplinkrating ), "", "obituary_urating" );
		EclRegisterButton ( 190, 340, 100, 15, Rating::GetNeuromancerRatingString ( gob->neuromancerrating ), "", "obituary_nrating" );

		EclRegisterButtonCallbacks ( "obituary_financestext", MediumTextDraw, NULL, NULL, NULL );
		EclRegisterButtonCallbacks ( "obituary_uratingtext", MediumTextDraw, NULL, NULL, NULL );
		EclRegisterButtonCallbacks ( "obituary_nratingtext", MediumTextDraw, NULL, NULL, NULL );
		EclRegisterButtonCallbacks ( "obituary_finances", textbutton_draw, NULL, NULL, NULL );
		EclRegisterButtonCallbacks ( "obituary_urating", textbutton_draw, NULL, NULL, NULL );
		EclRegisterButtonCallbacks ( "obituary_nrating", textbutton_draw, NULL, NULL, NULL );

		//
		// Special missions buttons

		EclRegisterButton ( 20, 400, 120, 15, "Special Awards", "", "obituary_specialmissions" );
        EclRegisterButtonCallbacks ( "obituary_specialmissions", MediumTextDraw, NULL, NULL, NULL );

        int x = 30;
        int y = 420;
        int numcompleted = 0;

        for ( int i = 0; i < 16; ++i ) {
            if ( gob->specialmissionscompleted & (1 << i) ) {

                char name [128];
                char tooltip [128];
                char filename [256];
                UplinkSnprintf ( name, sizeof ( name ), "obituary_award %d", i );
                UplinkSnprintf ( tooltip, sizeof ( tooltip ), "Completed Special Mission '%s'", PlotGenerator::SpecialMissionTitle (i) );
                UplinkSnprintf ( filename, sizeof ( filename ), "awards/award%d.tif", i );

                EclRegisterButton ( x, y, 16, 16, " ", tooltip, name );                
                button_assignbitmaps ( name, filename, filename, filename );
                EclRegisterButtonCallbacks ( name, imagebutton_draw, NULL, button_click, button_highlight );
                
                x += 18;
                numcompleted++;

            }            
        }

        char numberoutof[128];
        UplinkSnprintf ( numberoutof, sizeof ( numberoutof ), "(%d out of %d)", numcompleted, 12 );
        EclRegisterButton ( 190, 400, 120, 15, numberoutof, "", "obituary_numberoutof" );
        EclRegisterButtonCallbacks ( "obituary_numberoutof", textbutton_draw, NULL, NULL, NULL );

		//
		// Right column

		char highsecurityhacks [128];
		char livesruined [128];
		char systemsdestroyed [128];
		
		UplinkSnprintf ( highsecurityhacks, sizeof ( highsecurityhacks ), "You compromised %d high security systems", gob->score_highsecurityhacks );
		UplinkSnprintf ( livesruined, sizeof ( livesruined ), "You ruined the lives of %d people", gob->score_peoplefucked );
		UplinkSnprintf ( systemsdestroyed, sizeof ( systemsdestroyed ), "You destroyed %d computers", gob->score_systemsfucked );

		EclRegisterButton ( 350, 300, 300, 15, highsecurityhacks, "", "obituary_highsecurityhacks" );		
		EclRegisterButton ( 350, 320, 300, 15, systemsdestroyed, "", "obituary_systemsdestroyed" );
		EclRegisterButton ( 350, 340, 300, 15, livesruined, "", "obituary_livesruined" );

		EclRegisterButtonCallbacks ( "obituary_highsecurityhacks", textbutton_draw, NULL, NULL, NULL );
		EclRegisterButtonCallbacks ( "obituary_livesruined", textbutton_draw, NULL, NULL, NULL );
		EclRegisterButtonCallbacks ( "obituary_systemsdestroyed", textbutton_draw, NULL, NULL, NULL );

		char score [64];
		UplinkSnprintf ( score, sizeof ( score ), "Final Score    %d", gob->score );

		EclRegisterButton ( 360, 410, 200, 25, score, "", "obituary_score" );
		EclRegisterButtonCallbacks ( "obituary_score", LargeTextDraw, NULL, NULL, NULL );

		// Exit button

		EclRegisterButton ( screenw - 40, screenh - 40, 32, 32, "", "Return to Main Menu", "obituary_mainmenu" );
		EclRegisterButtonCallback ( "obituary_mainmenu", ReturnToMainMenuClick );
		button_assignbitmaps ( "obituary_mainmenu", "mainmenu/exitgame.tif", "mainmenu/exitgame_h.tif", "mainmenu/exitgame_c.tif" );

		// Secret "PI" button

#ifndef DEMOGAME
		EclRegisterButton ( 5, 5, 16, 16, " ", "???????????Meet the makers", "pi" );
		button_assignbitmaps ( "pi", "mainmenu/pi.tif", "mainmenu/pi_h.tif", "mainmenu/pi_c.tif" );
		EclRegisterButtonCallback ( "pi", PiClick );
#endif

	}

}

void ObituaryInterface::Remove ()
{

	if ( IsVisible () ) {

		MainMenuScreen::Remove ();

#ifdef GAME_WINNING_CODE

		EclRemoveButton ( "obituary_winningcodedesc" );
		EclRemoveButton ( "obituary_winningcodelabel" );
		EclRemoveButton ( "obituary_winningcode" );

#endif

		EclRemoveButton ( "obituary_name" );
		EclRemoveButton ( "obituary_text" );

		EclRemoveButton ( "obituary_financestext" );
		EclRemoveButton ( "obituary_uratingtext" );
		EclRemoveButton ( "obituary_nratingtext" );
		EclRemoveButton ( "obituary_finances" );
		EclRemoveButton ( "obituary_urating" );
		EclRemoveButton ( "obituary_nrating" );

		EclRemoveButton ( "obituary_specialmissions" );
        EclRemoveButton ( "obituary_numberoutof" );
        for ( int i = 0; i < 16; ++i ) {

            char name [128];
            UplinkSnprintf ( name, sizeof ( name ), "obituary_award %d", i );
            EclRemoveButton (name);

        }
        
		EclRemoveButton ( "obituary_highsecurityhacks" );
		EclRemoveButton ( "obituary_livesruined" );
		EclRemoveButton ( "obituary_systemsdestroyed" );
		EclRemoveButton ( "obituary_score" );

		EclRemoveButton ( "obituary_mainmenu" );

#ifndef DEMOGAME
		EclRemoveButton ( "pi" );
#endif

	}

}

void ObituaryInterface::Update ()
{
}

bool ObituaryInterface::IsVisible ()
{

	return ( EclGetButton ( "obituary_text" ) != 0 );

}

int ObituaryInterface::ScreenID ()
{

	return MAINMENU_OBITUARY;

}
