
#ifdef WIN32
#include <windows.h>
#endif

#ifndef HAVE_GLES
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <GLES/gl.h>
#include <GLES/glues.h>
#endif


#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"
#include "app/miscutils.h"
#include "app/opengl_interface.h"

#include "game/game.h"

#include "world/world.h"
#include "world/agent.h"
#include "world/player.h"
#include "world/computer/computerscreen/genericscreen.h"
#include "world/generator/numbergenerator.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/rankingscreen_interface.h"

#include "mmgr.h"


#define for if(1) for


void RankingScreenInterface::ExitClick ( Button *button )
{

	GenericScreen *sss = (GenericScreen *) game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
	UplinkAssert (sss);

	game->GetInterface ()->GetRemoteInterface ()->RunScreen ( sss->nextpage, sss->GetComputer () );

}

void RankingScreenInterface::ActiveAgentDraw ( Button *button, bool highlighted, bool clicked )
{
		
	clear_draw ( button->x, button->y, button->width, button->height );

	SetColour ( "DefaultText" );

	int xpos = button->x + 10;
	int	ypos = button->y + 10;

	GciDrawText ( xpos, ypos, button->caption );		

}

void RankingScreenInterface::PlayerDraw ( Button *button, bool highlighted, bool clicked )
{
		
#ifndef HAVE_GLES
	glBegin ( GL_QUADS );
		SetColour ( "PanelBackgroundA" );   glVertex2i ( button->x - 1, button->y );
		SetColour ( "PanelBackgroundB" );   glVertex2i ( button->x + button->width, button->y );
		SetColour ( "PanelBackgroundA" );   glVertex2i ( button->x + button->width, button->y + button->height );
		SetColour ( "PanelBackgroundB" );   glVertex2i ( button->x, button->y + button->height );
	glEnd ();
#else
        ColourOption *col1, *col2;
        col1 = GetColour("PanelBackgroundA");
        col2 = GetColour("PanelBackgroundB");
        GLfloat verts[] = {
                button->x - 1, button->y,
                button->x + button->width, button->y,
                button->x + button->width, button->y + button->height,
                button->x, button->y + button->height
        };
        GLfloat colors[] = {
                col1->r, col1->g, col1->b, 1.0f,
                col2->r, col2->g, col2->b, 1.0f,
                col1->r, col1->g, col1->b, 1.0f,
                col2->r, col2->g, col2->b, 1.0f
        };

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        glVertexPointer(2, GL_FLOAT, 0, verts);
        glColorPointer(4, GL_FLOAT, 0, colors);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
#endif

	SetColour ( "PanelBorder" );
	border_draw ( button );

	SetColour ( "DefaultText" );

	int xpos = button->x + 10;
	int	ypos = button->y + 10;

	GciDrawText ( xpos, ypos, button->caption );		

}

void RankingScreenInterface::NonActiveAgentDraw ( Button *button, bool highlighted, bool clicked )
{

	clear_draw ( button->x, button->y, button->width, button->height );

	SetColour ( "DimmedText" );

	int xpos = button->x + 10;
	int	ypos = button->y + 10;

	GciDrawText ( xpos, ypos, button->caption );		

}

bool RankingScreenInterface::ReturnKeyPressed ()
{

	ExitClick ( NULL );
	return true;

}

bool RankingScreenInterface::EscapeKeyPressed ()
{

	ExitClick ( NULL );
	return true;

}

void RankingScreenInterface::Create ( ComputerScreen *newcs )
{

	if ( !IsVisible () ) {

		cs = newcs;

		// 
		// Build a list of agents sorted on rating order
		//

		DArray <Person *> *allpeople = game->GetWorld ()->people.ConvertToDArray ();
		LList  <Person *> sorted;

		for ( int ip = 0; ip < allpeople->Size (); ++ip ) {
			if ( allpeople->ValidIndex (ip) ) {

				Person *p = allpeople->GetData (ip);
				UplinkAssert (p);

				if ( p->GetOBJECTID () == OID_AGENT ||
					 p->GetOBJECTID () == OID_PLAYER ) {

					bool inserted = false;

					for ( int is = 0; is < sorted.Size (); ++is ) {

						Person *s = sorted.GetData (is);
						UplinkAssert (s);

						if ( p->rating.uplinkscore >= s->rating.uplinkscore ) {

							sorted.PutDataAtIndex ( p, is );
							inserted = true;
							break;

						}

					}

					if ( !inserted ) sorted.PutDataAtEnd ( p );
	
				}

			}
		}

		delete allpeople;

		//
		// Find the players rank
		//

		int playerrank = -1;

		for ( int ir = 0; ir < sorted.Size (); ++ir ) {
			if ( sorted.GetData (ir) == game->GetWorld ()->GetPlayer () ) {
				playerrank = ir;
				break;
			}
		}

		UplinkAssert ( playerrank != -1 );

		// 
		// Build the interface
		//

		EclRegisterButton ( 80, 60, 350, 25, GetComputerScreen ()->maintitle, "", "ranking_maintitle" );
		EclRegisterButtonCallbacks ( "ranking_maintitle", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButton ( 80, 80, 350, 20, GetComputerScreen ()->subtitle, "", "ranking_subtitle" );
		EclRegisterButtonCallbacks ( "ranking_subtitle", DrawSubTitle, NULL, NULL, NULL );


		for ( int i = 0; i < 10; ++i ) {

			char name   [24];
			char status [24];

			UplinkSnprintf ( name, sizeof ( name ), "ranking_name %d", i );
			UplinkSnprintf ( status, sizeof ( status ), "ranking_status %d", i );

			if ( sorted.ValidIndex (i) ) {

				UplinkAssert ( sorted.GetData (i)->GetOBJECTID () == OID_AGENT ||
							   sorted.GetData (i)->GetOBJECTID () == OID_PLAYER );

				Agent *agent = (Agent *) sorted.GetData (i);

				char cname [128];
				UplinkSnprintf ( cname, sizeof ( cname ), "%2d  :  Agent %s", i + 1, agent->handle );
				
				char cstatus [32];
				if      ( agent->GetStatus () == PERSON_STATUS_NONE ) {			UplinkStrncpy ( cstatus, "Active", sizeof ( cstatus ) );
				} else if ( agent->GetStatus () == PERSON_STATUS_INJAIL ) {		UplinkStrncpy ( cstatus, "In Jail", sizeof ( cstatus ) );
				} else if ( agent->GetStatus () == PERSON_STATUS_DEAD ) {		UplinkStrncpy ( cstatus, "Deceased", sizeof ( cstatus ) );
				} else {														UplinkStrncpy ( cstatus, "Unknown", sizeof ( cstatus ) );
				}

				EclRegisterButton ( 30, 120 + 20 * i, 170, 15, cname, "", name );
				EclRegisterButton ( 200, 120 + 20 * i, 100, 15, cstatus, "", status );

				if ( i == playerrank ) {
				
					EclRegisterButtonCallbacks ( name, PlayerDraw, NULL, NULL, NULL );
					EclRegisterButtonCallbacks ( status, PlayerDraw, NULL, NULL, NULL );

				}
				else {

					if ( agent->GetStatus () == PERSON_STATUS_NONE ) {
						EclRegisterButtonCallbacks ( name,   ActiveAgentDraw, NULL, NULL, NULL );
						EclRegisterButtonCallbacks ( status, ActiveAgentDraw, NULL, NULL, NULL );
					}
					else {
						EclRegisterButtonCallbacks ( name,   NonActiveAgentDraw, NULL, NULL, NULL );
						EclRegisterButtonCallbacks ( status, NonActiveAgentDraw, NULL, NULL, NULL );
					}

				}

			}

		}

		//
		// If the player is not in the top 10, 
		// Put his rank at the bottom
		//

		if ( playerrank >= 10 ) {

			// Special case - if the player has a score of 1 (ie rank beginner) or 2 (very crap)
			// Then make him look like he is much further down, to make the world seem bigger   ;)

			if ( game->GetWorld ()->GetPlayer ()->rating.uplinkrating == 1 ) {
				playerrank *= 3;
				playerrank += NumberGenerator::RandomNumber ( 20 );
			}
			else if ( game->GetWorld ()->GetPlayer ()->rating.uplinkrating == 2 ) {
				playerrank *= 2;
				playerrank += NumberGenerator::RandomNumber ( 10 );
			}

			char cname [128];
			UplinkSnprintf ( cname, sizeof ( cname ), "%2d  :  Agent %s", playerrank + 1, game->GetWorld ()->GetPlayer ()->handle );

			EclRegisterButton ( 60, 340, 170, 15, cname, "", "ranking_playerrank" );
			EclRegisterButton ( 230, 340, 100, 15, "Active", "", "ranking_playerstatus" );
			
			EclRegisterButtonCallbacks ( "ranking_playerrank", ActiveAgentDraw, NULL, NULL, NULL );
			EclRegisterButtonCallbacks ( "ranking_playerstatus", ActiveAgentDraw, NULL, NULL, NULL );
			
		}

		//
		// Exit button
		//

		EclRegisterButton ( 355, 387, 70, 15, "Exit", "Close the screen and return to the main menu", "ranking_exit" );
		EclRegisterButtonCallback ( "ranking_exit", ExitClick );

	}
		
}

void RankingScreenInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "ranking_maintitle" );
		EclRemoveButton ( "ranking_subtitle" );

		for ( int i = 0; i < 10; ++i ) {

			char name   [24];
			char status [24];

			UplinkSnprintf ( name, sizeof ( name ), "ranking_name %d", i );
			UplinkSnprintf ( status, sizeof ( status ), "ranking_status %d", i );

			EclRemoveButton ( name );
			EclRemoveButton ( status );

		}

		EclRemoveButton ( "ranking_playerrank" );
		EclRemoveButton ( "ranking_playerstatus" );

		EclRemoveButton ( "ranking_exit" );

	}
		
}

bool RankingScreenInterface::IsVisible ()
{

	return ( EclGetButton ( "ranking_maintitle" ) != NULL );

}

int RankingScreenInterface::ScreenID ()
{
	
	return SCREEN_RANKINGSCREEN;

}

GenericScreen *RankingScreenInterface::GetComputerScreen ()
{

	UplinkAssert (cs);
	return (GenericScreen *) cs;

}
