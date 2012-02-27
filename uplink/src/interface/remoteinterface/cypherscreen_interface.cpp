
#ifdef WIN32
#include "windows.h"
#endif

#ifndef HAVE_GLES
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <GLES/gl.h>
#include <GLES/glues.h>
#endif


#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"

#include "options/options.h"

#include "game/game.h"

#include "world/generator/numbergenerator.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/cypherscreen_interface.h"
#include "interface/taskmanager/taskmanager.h"

#include "mmgr.h"


CypherScreenInterface::CypherScreenInterface ()
{

	lastupdate = 0;
	numlocked = 0;

	for ( int i = 0; i < CYPHER_WIDTH; ++i ) {
		for ( int j = 0; j < CYPHER_HEIGHT; ++j ) {

			cypher [i][j] = '0' + NumberGenerator::RandomNumber (10);
			cypherlock [i][j] = false;

		}
	}

}

CypherScreenInterface::~CypherScreenInterface ()
{
}

bool CypherScreenInterface::ReturnKeyPressed ()
{

	return false;

}

void CypherScreenInterface::DrawCypher ( Button *button, bool highlighted, bool clicked )
{

	CypherScreenInterface *thisint = (CypherScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (thisint);

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
	glEnable ( GL_SCISSOR_TEST );


	clear_draw ( button->x, button->y, button->width, button->height );
	
	for ( int i = 0; i < CYPHER_WIDTH; ++i ) {
		for ( int j = 0; j < CYPHER_HEIGHT; ++j ) {

			int xpos = button->x + (int)(button->width * ( (float) i / (float) CYPHER_WIDTH ));
			int ypos = button->y + 12 + (int)(button->height * ( (float) j / (float) CYPHER_HEIGHT ));

			char text [2];
			UplinkSnprintf ( text, sizeof ( text ), "%c", thisint->cypher[i][j] );

			if ( thisint->cypherlock[i][j] ) {
				
				float shade = 0.2f + (float) (thisint->cypher [i][j] - '0') / 10.0f;
				glColor4f ( shade, shade, shade, 1.0f );

				int cubeW = (button->width / CYPHER_WIDTH) + 1;
				int cubeH = (button->height / CYPHER_HEIGHT) + 1;

#ifndef HAVE_GLES
				glBegin ( GL_QUADS );
					glVertex2i ( xpos, ypos - 10 );
					glVertex2i ( xpos + cubeW, ypos - 10 );
					glVertex2i ( xpos + cubeW, ypos + cubeH - 10 );
					glVertex2i ( xpos, ypos + cubeH - 10 );
				glEnd ();
#else
				GLfloat verts[] = {
					xpos, ypos - 10,
					xpos + cubeW, ypos - 10,
					xpos + cubeW, ypos + cubeH - 10,
					xpos, ypos + cubeH - 10
				};

				glEnableClientState(GL_VERTEX_ARRAY);
				glVertexPointer(2, GL_FLOAT, 0, verts);
				glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
				glDisableClientState(GL_VERTEX_ARRAY);
#endif

				glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );

			}
			else
				glColor4f ( 0.6f, 0.6f, 0.6f, 1.0f );

			GciDrawText ( xpos, ypos, text, HELVETICA_12 );

		}
	}

	if ( clicked || highlighted ) {
		glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );
		border_draw ( button );
	}

	glDisable ( GL_SCISSOR_TEST );

}

void CypherScreenInterface::ClickCypher ( Button *button )
{

	UplinkAssert ( button );

	RemoteInterfaceScreen *ris = game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (ris);
	CypherScreenInterface *csi = (CypherScreenInterface *) ris;

	game->GetInterface ()->GetTaskManager ()->SetProgramTarget ( csi->GetComputerScreen (), button->name, -1 );

}

void CypherScreenInterface::BypassClick ( Button *button )
{

	RemoteInterfaceScreen *ris = game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (ris);
	CypherScreenInterface *csi = (CypherScreenInterface *) ris;
	CypherScreen *cs = csi->GetComputerScreen ();
	UplinkAssert (cs);

	game->GetInterface ()->GetRemoteInterface ()->SetSecurity ( "Admin", 1 );

	if ( cs->nextpage != -1 )
		game->GetInterface ()->GetRemoteInterface ()->RunScreen ( cs->nextpage, cs->GetComputer () );

}

void CypherScreenInterface::ProceedClick ( Button *button )
{

	RemoteInterfaceScreen *ris = game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (ris);
	CypherScreenInterface *csi = (CypherScreenInterface *) ris;

	if ( csi->NumUnLocked () == 0 ) {
	
		CypherScreen *cs = csi->GetComputerScreen ();
		UplinkAssert (cs);

		game->GetInterface ()->GetRemoteInterface ()->SetSecurity ( "Admin", 1 );

		if ( cs->nextpage != -1 )
			game->GetInterface ()->GetRemoteInterface ()->RunScreen ( cs->nextpage, cs->GetComputer () );

	}

}


int CypherScreenInterface::NumLocked ()
{

	return numlocked;

}

int CypherScreenInterface::NumUnLocked ()
{

	return (CYPHER_WIDTH * CYPHER_HEIGHT) - numlocked;
		
}

bool CypherScreenInterface::IsLocked ( int x, int y )
{
	
	return ( cypherlock [x][y] );

}

void CypherScreenInterface::CypherLock ()
{

	if ( NumUnLocked () > 0 ) {

		bool finished = false;

		while ( !finished ) {

			int x = NumberGenerator::RandomNumber (CYPHER_WIDTH);
			int y = NumberGenerator::RandomNumber (CYPHER_HEIGHT);

			if ( !IsLocked (x, y) ) {
				CypherLock (x, y);
				finished = true;
			}

		}

	}

}

void CypherScreenInterface::CypherLock ( int x, int y )
{

	if ( !cypherlock [x][y] ) {
		numlocked++;
		cypher[x][y] = '9';
		cypherlock [x][y] = true;
	}

}

void CypherScreenInterface::Cycle ()
{

	for ( int i = 0; i < CYPHER_WIDTH; ++i ) 
		for ( int j = 0; j < CYPHER_HEIGHT; ++j ) 
			if ( !cypherlock [i][j] ) {

				if ( i < CYPHER_WIDTH+1 && cypher[i+1][j] == '1' && cypher[i][j] != '1' )
					cypher [i][j] = '1';

				else
					cypher [i][j] = '0' + NumberGenerator::RandomNumber (10);

			}
			else 
				if ( cypher [i][j] > '0' )
					-- cypher [i][j];

				

	EclDirtyButton ( "cypherscreen_cypher" );

}

void CypherScreenInterface::Create ( ComputerScreen *newcs )
{

	UplinkAssert ( newcs );
	cs = newcs;

	if ( !IsVisible () ) {

		EclRegisterButton ( 80, 60, 350, 25, GetComputerScreen ()->maintitle, "", "cypherscreen_maintitle" );
		EclRegisterButtonCallbacks ( "cypherscreen_maintitle", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButton ( 80, 80, 350, 20, GetComputerScreen ()->subtitle, "", "cypherscreen_subtitle" );
		EclRegisterButtonCallbacks ( "cypherscreen_subtitle", DrawSubTitle, NULL, NULL, NULL );

		EclRegisterButton ( 50, 130, 380, 250, " ", " ", "cypherscreen_cypher" );
		EclRegisterButtonCallbacks ( "cypherscreen_cypher", DrawCypher, ClickCypher, button_click, button_highlight );

		EclRegisterButton ( 350, 380, 80, 15, "Proceed", "Click here when done", "cypherscreen_proceed" );
		button_assignbitmaps ( "cypherscreen_proceed", "proceed.tif", "proceed_h.tif", "proceed_c.tif" );
		EclRegisterButtonCallback ( "cypherscreen_proceed", ProceedClick );
		
#ifdef CHEATMODES_ENABLED
		// Create a symbol for quick entry into this screen
		EclRegisterButton ( 3, 20, 30, 15, "#", "Click here to bypass this screen (DEBUG MODE ONLY)", "cypherscreen_bypass" );
		EclRegisterButtonCallbacks ( "cypherscreen_bypass", textbutton_draw, BypassClick, button_click, button_highlight );
#endif						

	}

}

void CypherScreenInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "cypherscreen_maintitle" );
		EclRemoveButton ( "cypherscreen_subtitle" );
		EclRemoveButton ( "cypherscreen_cypher" );

		EclRemoveButton ( "cypherscreen_bypass" );
		EclRemoveButton ( "cypherscreen_proceed" );

	}

}

bool CypherScreenInterface::IsVisible ()
{

	return ( EclGetButton ( "cypherscreen_maintitle" ) != NULL );

}

void CypherScreenInterface::Update ()
{

	if ( EclGetAccurateTime () > lastupdate + 75 ) {

		Cycle ();
		lastupdate = (int)EclGetAccurateTime ();

	}

}

int CypherScreenInterface::ScreenID ()
{

	return SCREEN_CYPHERSCREEN;

}


CypherScreen *CypherScreenInterface::GetComputerScreen ()
{

	UplinkAssert ( cs );
	return (CypherScreen *) cs;

}
