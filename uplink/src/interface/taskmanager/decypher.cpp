
#ifdef WIN32
#include <windows.h>
#endif

#ifndef HAVE_GLES
#include <GL/gl.h>
#include <GL/glu.h> /*_glu_extention_library_*/
#else
#include <GLES/gl.h>
#include <GLES/glues.h>
#endif


#include "eclipse.h"
#include "vanbakel.h"
#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"
#include "app/serialise.h"

#include "game/game.h"
#include "game/data/data.h"

#include "interface/interface.h"
#include "interface/taskmanager/taskmanager.h"
#include "interface/taskmanager/decypher.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/cypherscreen_interface.h"

#include "world/world.h"
#include "world/player.h"
#include "world/computer/computer.h"
#include "world/computer/databank.h"
#include "world/computer/computerscreen/cypherscreen.h"
#include "world/generator/numbergenerator.h"

#include "mmgr.h"


Decypher::Decypher () : UplinkTask ()
{
	
	status = DECYPHER_OFF;
	numticksrequired = 0;
	progress = 0;

}

Decypher::~Decypher ()
{
}

void Decypher::MoveTo ( int x, int y, int time_ms )
{

	UplinkTask::MoveTo ( x, y, time_ms );

	int pid = SvbLookupPID ( this );

	char stitle    [128];
	char sborder   [128];
	char sprogress [128];
	char sclose    [128];

	UplinkSnprintf ( stitle, sizeof ( stitle ), "decypher_title %d", pid );
	UplinkSnprintf ( sborder, sizeof ( sborder ), "decypher_border %d", pid );
	UplinkSnprintf ( sprogress, sizeof ( sprogress ), "decypher_progress %d", pid );
	UplinkSnprintf ( sclose, sizeof ( sclose ), "decypher_close %d", pid );	

	EclRegisterMovement ( stitle, x, y, time_ms);
	EclRegisterMovement ( sborder, x + 20, y, time_ms );
	EclRegisterMovement ( sprogress, x + 20, y + 1, time_ms );
	EclRegisterMovement ( sclose, x + 140, y + 1, time_ms );

	EclButtonBringToFront ( stitle );
	EclButtonBringToFront ( sborder );
	EclButtonBringToFront ( sprogress );
	EclButtonBringToFront ( sclose );

}

void Decypher::SetTarget ( UplinkObject *uo, char *uos, int uoi )
{

	/*

		uo  : CypherScreen object (ie the screen interface object)
		uos : Name of button representing data
		uoi : unused

		*/

	if ( status == DECYPHER_OFF ) {

		if ( uo->GetOBJECTID () == OID_CYPHERSCREEN ) {

			CypherScreen *cs = (CypherScreen *) uo;
			UplinkAssert (cs);
			Computer *comp = cs->GetComputer ();
			UplinkAssert (comp);

			Button *button = EclGetButton ( uos );
			UplinkAssert (button);

			MoveTo ( button->x + button->width - 300, button->y + button->height, 1000 );

			int security = game->GetInterface ()->GetRemoteInterface ()->security_level;
			int difficulty = (int)(CYPHER_WIDTH * CYPHER_HEIGHT * TICKSREQUIRED_BYPASSCYPHER * cs->difficulty);
			numticksrequired = NumberGenerator::ApplyVariance ( difficulty, (int)(HACKDIFFICULTY_VARIANCE * 100) );
			progress = 0;

			if ( comp->security.IsRunning_Monitor () )
				game->GetWorld ()->GetPlayer ()->GetConnection ()->BeginTrace ();

			int pid = SvbLookupPID ( this );
			char sprogress [128];
			UplinkSnprintf ( sprogress, sizeof ( sprogress ), "decypher_progress %d", pid );
			EclRegisterCaptionChange ( sprogress, "Decyphering..." );

			status = DECYPHER_INPROGRESS;

		}

	}

}

void Decypher::BorderDraw ( Button *button, bool highlighted, bool clicked )
{

#ifndef HAVE_GLES
	glBegin ( GL_QUADS );

		if      ( clicked )		glColor4f ( 0.5f, 0.5f, 0.6f, ALPHA );
		else if ( highlighted ) glColor4f ( 0.2f, 0.2f, 0.5f, ALPHA );
		else					glColor4f ( 0.2f, 0.2f, 0.4f, ALPHA );
		glVertex2i ( button->x, button->y );

		if		( clicked )		glColor4f ( 0.7f, 0.7f, 0.6f, ALPHA );
		else if ( highlighted ) glColor4f ( 0.5f, 0.5f, 0.6f, ALPHA );
		else					glColor4f ( 0.3f, 0.3f, 0.5f, ALPHA );
		glVertex2i ( button->x + button->width, button->y );

		if		( clicked )		glColor4f ( 0.5f, 0.5f, 0.6f, ALPHA );
		else if ( highlighted ) glColor4f ( 0.2f, 0.2f, 0.5f, ALPHA );
		else					glColor4f ( 0.2f, 0.2f, 0.4f, ALPHA );
		glVertex2i ( button->x + button->width, button->y + button->height );

		if		( clicked )		glColor4f ( 0.7f, 0.7f, 0.6f, ALPHA );
		else if ( highlighted ) glColor4f ( 0.5f, 0.5f, 0.6f, ALPHA );
		else					glColor4f ( 0.3f, 0.3f, 0.5f, ALPHA );
		glVertex2i ( button->x, button->y + button->height );

	glEnd ();
#else
	GLfloat r1, g1, b1, r2, g2, b2;
	if (clicked) {
		r1 = g1 = 0.5f; b1 = 0.6f;
		r2 = g2 = 0.7f; b2 = 0.6f;
	} else if (highlighted) {
		r1 = g1 = 0.2f; b1 = 0.5f;
		r2 = g2 = 0.5f; b2 = 0.6f;
	} else {
		r1 = g1 = 0.2f; b1 = 0.4f;
		r2 = g2 = 0.3f; b2 = 0.5f;
	}       

	GLfloat verts[] = {                             
		button->x, button->y + button->height,
		button->x, button->y,
		button->x + button->width, button->y,   
		button->x + button->width, button->y + button->height 
	};      

	GLfloat colors[] = {
		r1, g1, b1, ALPHA,
		r1, g1, b1, ALPHA,
		r2, g2, b2, ALPHA,                      
		r2, g2, b2, ALPHA
	};

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);            

	glVertexPointer(2, GL_FLOAT, 0, verts);
	glColorPointer(4, GL_FLOAT, 0, colors);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
#endif

	if ( highlighted || clicked ) {

		glColor4f ( 0.3f, 0.3f, 0.7f, 1.0f );
		border_draw ( button );

	}

}

void Decypher::ProgressDraw ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert ( button );

	float scale = (float) button->width / 100.0f;
	if ( highlighted ) scale *= 2;

	glColor4f ( 0.0f, 1.5f - scale, scale, 0.6f );
#ifndef HAVE_GLES
	glBegin ( GL_QUADS );

		glVertex3i ( button->x, button->y, 0 );
		glVertex3i ( button->x + button->width, button->y, 0 );
		glVertex3i ( button->x + button->width, button->y + button->height, 0 );
		glVertex3i ( button->x, button->y + button->height, 0 );

	glEnd ();
#else
	GLfloat verts[] = {
		button->x, button->y,
		button->x + button->width, button->y,
		button->x + button->width, button->y + button->height,
		button->x, button->y + button->height
	};

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, verts);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
#endif

	int xpos = button->x + 5;
	int ypos = (button->y + button->height / 2) + 3;
		
	glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );    
    GciDrawText ( xpos, ypos, button->caption, HELVETICA_10 );

}

void Decypher::CloseClick ( Button *button )
{

	int pid;
	char bname [64];
	sscanf ( button->name, "%s %d", bname, &pid );

	SvbRemoveTask ( pid );

}

void Decypher::BorderClick ( Button *button )
{

	int pid;
	char bname [64];
	sscanf ( button->name, "%s %d", bname, &pid );

	game->GetInterface ()->GetTaskManager ()->SetTargetProgram ( pid );

}

void Decypher::Initialise ()
{
}

void Decypher::Tick ( int n )
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );
		char sprogress [128];
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "decypher_progress %d", pid );

		if ( status == DECYPHER_OFF ) {

			// Not decyphering - look for a new target


		}
		else if ( status == DECYPHER_INPROGRESS ) {

			// Make sure we are still looking at the
			// same interface screen

			RemoteInterfaceScreen *ris = game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
			UplinkAssert (ris);

			if ( ris->ScreenID () != SCREEN_CYPHERSCREEN ) {
				SvbRemoveTask ( this );
				// WARNING - this has now been deleted
				return;
			}

			CypherScreenInterface *csi = (CypherScreenInterface *) ris;
				
			progress += (int)(n * version);

			UplinkAssert ( EclGetButton ( sprogress ) );
			EclGetButton ( sprogress )->width = (int)(120 * ( (float) progress / (float) numticksrequired ));
			EclDirtyButton ( sprogress );

			if ( progress >= numticksrequired ) {

				// Finished

				while ( csi->NumUnLocked () > 0 )
					csi->CypherLock ();

				status = DECYPHER_FINISHED;

			}
			else {

				int percentagedone = (int)(100.0f * (float) progress / (float) numticksrequired);

				while ( 100.0 * (float) csi->NumLocked () / (float) (CYPHER_WIDTH*CYPHER_HEIGHT) <= percentagedone )
					csi->CypherLock ();

			}
			
		}
		else if ( status == DECYPHER_FINISHED ) {

			SvbRemoveTask ( this );
			return;

		}

	}

}

void Decypher::CreateInterface ()
{

	if ( !IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );
		
		char stitle    [128];
		char sborder   [128];
		char sprogress [128];
		char sclose    [128];

		UplinkSnprintf ( stitle, sizeof ( stitle ), "decypher_title %d", pid );
		UplinkSnprintf ( sborder, sizeof ( sborder ), "decypher_border %d", pid );
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "decypher_progress %d", pid );
		UplinkSnprintf ( sclose, sizeof ( sclose ), "decypher_close %d", pid );	

		EclRegisterButton ( 265, 450, 20, 15, "", "Decypher", stitle );
		button_assignbitmap ( stitle, "software/dec.tif" );

		// Fix for decypher unpickability by NeoThermic

		EclRegisterButton ( 285, 450, 120, 15, "", "", sborder );
		EclRegisterButtonCallbacks ( sborder, BorderDraw, BorderClick, button_click, button_highlight );

		EclRegisterButton ( 285, 450, 120, 13, "Select target", "Shows the progress of the decypher", sprogress );
		EclRegisterButtonCallbacks ( sprogress, ProgressDraw, BorderClick, button_click, button_highlight );		

		EclRegisterButton ( 405, 450, 13, 13, "", "Close the Decypher (and stop)", sclose );		
		button_assignbitmaps ( sclose, "close.tif", "close_h.tif", "close_c.tif" );
		EclRegisterButtonCallback ( sclose, CloseClick );
		
	}

}

void Decypher::RemoveInterface ()
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char stitle    [128];
		char sborder   [128];
		char sprogress [128];
		char sclose    [128];

		UplinkSnprintf ( stitle, sizeof ( stitle ), "decypher_title %d", pid );
		UplinkSnprintf ( sborder, sizeof ( sborder ), "decypher_border %d", pid );
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "decypher_progress %d", pid );
		UplinkSnprintf ( sclose, sizeof ( sclose ), "decypher_close %d", pid );	

		EclRemoveButton ( stitle );
		EclRemoveButton ( sborder );
		EclRemoveButton ( sprogress );
		EclRemoveButton ( sclose );

	}

}

void Decypher::ShowInterface ()
{

	if ( !IsInterfaceVisible () ) CreateInterface ();

	int pid = SvbLookupPID ( this );

	char stitle    [128];
	char sborder   [128];
	char sprogress [128];
	char sclose    [128];

	UplinkSnprintf ( stitle, sizeof ( stitle ), "decypher_title %d", pid );
	UplinkSnprintf ( sborder, sizeof ( sborder ), "decypher_border %d", pid );
	UplinkSnprintf ( sprogress, sizeof ( sprogress ), "decypher_progress %d", pid );
	UplinkSnprintf ( sclose, sizeof ( sclose ), "decypher_close %d", pid );	

	EclButtonBringToFront ( stitle );
	EclButtonBringToFront ( sborder );
	EclButtonBringToFront ( sprogress );
	EclButtonBringToFront ( sclose );

}

bool Decypher::IsInterfaceVisible ()
{

	int pid = SvbLookupPID ( this );

	char stitle [128];
	UplinkSnprintf ( stitle, sizeof ( stitle ), "decypher_border %d", pid );
	
	return ( EclGetButton (stitle) != NULL );

}
