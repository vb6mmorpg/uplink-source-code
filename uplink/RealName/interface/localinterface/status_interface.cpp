// StatusInterface.cpp: implementation of the StatusInterface class.
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h>


#include "eclipse.h"
#include "soundgarden.h"
#include "redshirt.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/miscutils.h"
#include "app/opengl_interface.h"

#include "game/game.h"
#include "game/data/data.h"

#include "options/options.h"

#include "interface/interface.h"
#include "interface/localinterface/localinterface.h"
#include "interface/localinterface/status_interface.h"

#include "world/world.h"
#include "world/player.h"
#include "world/computer/recordbank.h"
#include "world/generator/recordgenerator.h"

#include "mmgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void StatusInterface::TitleClick ( Button *button )
{

	game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_NONE );

}

void StatusInterface::MiniTitleDraw ( Button *button, bool highlighted, bool clicked )
{

	SetColour ( "TitleText" );
	GciDrawText ( button->x + 10, button->y + 10, button->caption, HELVETICA_18 );

}

void StatusInterface::Create ()
{

	if ( !IsVisible () ) {

		LocalInterfaceScreen::Create ();

		int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
		int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");
		int paneltop = (int) ( 100.0 * ( (screenw * PANELSIZE) / 188.0 ) + 30 );
		int panelwidth = (int) ( screenw * PANELSIZE );

		int cwidth = 95;

		EclRegisterButton ( screenw - panelwidth, paneltop + 3, panelwidth - 7, 15, "STATUS", "Remove the status screen", "status_title" );
		EclRegisterButtonCallback ( "status_title", TitleClick );

		//
		// Personal
		//

		EclRegisterButton ( screenw - panelwidth + 50, paneltop + 40, 80, 20, "Personal", "", "status_personaltitle" );
		EclRegisterButtonCallbacks ( "status_personaltitle", MiniTitleDraw, NULL, NULL, NULL );

		EclRegisterButton ( screenw - panelwidth, paneltop + 70, panelwidth - 5, 30, "", "", "status_personalname" );
		EclRegisterButton ( screenw - panelwidth, paneltop + 90, panelwidth - 5, 30, "", "", "status_personalhandle" );
		
		EclRegisterButtonCallbacks ( "status_personalname", text_draw, NULL, NULL, NULL );		
		EclRegisterButtonCallbacks ( "status_personalhandle", text_draw, NULL, NULL, NULL );

        char pname [256];        
        char phandle [256];
		UplinkSnprintf ( pname,   sizeof ( pname ),   "Real Name: %s\n", game->GetWorld ()->GetPlayer ()->realname );
        UplinkSnprintf ( phandle, sizeof ( phandle ), "Handle: %s\n", game->GetWorld ()->GetPlayer ()->handle );
		EclRegisterCaptionChange ( "status_personalname", pname );
		EclRegisterCaptionChange ( "status_personalhandle", phandle );

		//
		// Ratings
		//

		EclRegisterButton ( screenw - panelwidth + 50, paneltop + 160, 80, 20, "Ratings", "", "status_ratingstitle" );
		EclRegisterButtonCallbacks ( "status_ratingstitle", MiniTitleDraw, NULL, NULL, NULL );

//		EclRegisterButton ( screenw - panelwidth, paneltop + 70, cwidth, 30, "UPLINK", "", "status_tuplinkrating" );		
//		EclRegisterButton ( screenw - panelwidth, paneltop + 110, cwidth, 30, "NEUROMANCER", "", "status_tneuromancerrating" );
		
//		EclRegisterButtonCallbacks ( "status_tuplinkrating", text_draw, NULL, NULL, NULL );		
//		EclRegisterButtonCallbacks ( "status_tneuromancerrating", text_draw, NULL, NULL, NULL );

		EclRegisterButton ( screenw - panelwidth, paneltop + 190, panelwidth - 5, 30, "", "", "status_uplinkrating" );
		EclRegisterButton ( screenw - panelwidth, paneltop + 240, panelwidth - 5, 30, "", "", "status_neuromancerrating" );
		
		EclRegisterButtonCallbacks ( "status_uplinkrating", text_draw, NULL, NULL, NULL );		
		EclRegisterButtonCallbacks ( "status_neuromancerrating", text_draw, NULL, NULL, NULL );


        char urating [256];        
        char nrating [256];
        UplinkSnprintf ( urating, sizeof ( urating ), "GRADE %d UPLINK AGENT\nRating: %s", NUM_UPLINKRATINGS - game->GetWorld ()->GetPlayer ()->rating.uplinkrating,
													  game->GetWorld ()->GetPlayer ()->rating.GetUplinkRating () );
        UplinkSnprintf ( nrating, sizeof ( nrating ), "NEUROMANCER (Unofficial)\nRating: %s", game->GetWorld ()->GetPlayer ()->rating.GetNeuromancerRating () );
		EclRegisterCaptionChange ( "status_uplinkrating", urating );
		EclRegisterCaptionChange ( "status_neuromancerrating", nrating );


		// 
		// Legal
		//

		EclRegisterButton ( screenw - panelwidth + 50, paneltop + 310, 80, 20, "Criminal", "", "status_legaltitle" );
		EclRegisterButtonCallbacks ( "status_legaltitle", MiniTitleDraw, NULL, NULL, NULL );
			
		Record *rec = RecordGenerator::GetCriminal ( "PLAYER" );
		char *crimrec = NULL;
		if ( rec ) crimrec = rec->GetField ( "Convictions" );					

		EclRegisterButton ( screenw - panelwidth + 5, paneltop + 340, panelwidth - 15, 60, "", "", "status_legal" );
		EclRegisterButtonCallbacks ( "status_legal", text_draw, NULL, NULL, NULL );
		EclRegisterCaptionChange ( "status_legal", crimrec ? crimrec : (char *) "None" );


        //
        // Awards
        //

        int x = screenw - panelwidth + 2;                        
        //int y = SY(405);
        int y = paneltop + ( SY(300) - 16 - 3 );

        for ( int i = 0; i < 16; ++i ) {
            if ( game->GetWorld ()->plotgenerator.PlayerCompletedSpecialMission (i) ) {

                char name [128];
                char tooltip [128];
                char filename [256];
                UplinkSnprintf ( name, sizeof ( name ), "status_award %d", i );
                UplinkSnprintf ( tooltip, sizeof ( tooltip ), "Completed Special Mission '%s'", game->GetWorld ()->plotgenerator.SpecialMissionTitle (i) );
                UplinkSnprintf ( filename, sizeof ( filename ), "awards/award%d.tif", i );

                EclRegisterButton ( x, y, 16, 16, " ", tooltip, name );                
                button_assignbitmaps_blend ( name, filename, filename, filename );
                EclRegisterButtonCallbacks ( name, imagebutton_draw, NULL, button_click, button_highlight );
                
                x += 18;

            }            
        }

	}

}

void StatusInterface::Remove ()
{

	if ( IsVisible () ) {

		LocalInterfaceScreen::Remove ();

		EclRemoveButton ( "status_title" );

		EclRemoveButton ( "status_personaltitle" );
		EclRemoveButton ( "status_personalname" );
		EclRemoveButton ( "status_personalhandle" );

		EclRemoveButton ( "status_ratingstitle" );
//		EclRemoveButton ( "status_tuplinkrating" );
//		EclRemoveButton ( "status_tneuromancerrating" );
		
		EclRemoveButton ( "status_uplinkrating" );
		EclRemoveButton ( "status_neuromancerrating" );
		
		EclRemoveButton ( "status_legaltitle" );
		EclRemoveButton ( "status_legal" );

        for ( int i = 0; i < 16; ++i ) {

            char name [128];
            UplinkSnprintf ( name, sizeof ( name ), "status_award %d", i );
            EclRemoveButton (name);

        }

	}

}

void StatusInterface::Update ()
{

	/*

		I don't bother checking the player's ratings here - even though they
		could conceivably change whilst he is looking at this screen.

		My justification is that it would be pretty slow to keep looking up the player
		every Update cycle, and we don't want that.
		Also, the player's Uplink Rating does not change very often - maybe 10 times in 
		a complete game, which may last several days.

	*/

}

bool StatusInterface::IsVisible ()
{

	return ( EclGetButton ( "status_title" ) != NULL );

}

int StatusInterface::ScreenID ()
{
	
	return SCREEN_STATUS;

}
