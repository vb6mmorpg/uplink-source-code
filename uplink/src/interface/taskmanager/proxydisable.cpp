

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h> /*_glu_extention_library_*/

#include "vanbakel.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"

#include "game/game.h"
#include "game/data/data.h"

#include "interface/taskmanager/proxydisable.h"

#include "world/world.h"
#include "world/vlocation.h"
#include "world/player.h"
#include "world/computer/computer.h"
#include "world/computer/securitysystem.h"

#include "mmgr.h"


void ProxyDisable::BorderDraw ( Button *button, bool highlighted, bool clicked )
{

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

	if ( highlighted || clicked ) {

		glColor4f ( 0.3f, 0.3f, 0.7f, 1.0f );
		border_draw ( button );	

	}

}

void ProxyDisable::ProgressDraw ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert ( button );

	float scale = (float) button->width / 100.0f;
	if ( highlighted ) scale *= 2;

	glBegin ( GL_QUADS );

		glColor4f ( 0.0f, 1.5f - scale, scale, 0.6f );
		glVertex3i ( button->x, button->y, 0 );

		glColor4f ( 0.0f, 1.5f - scale, scale, 0.6f );
		glVertex3i ( button->x + button->width, button->y, 0 );

		glColor4f ( 0.0f, 1.5f - scale, scale, 0.6f );
		glVertex3i ( button->x + button->width, button->y + button->height, 0 );

		glColor4f ( 0.0f, 1.5f - scale, scale, 0.6f );
		glVertex3i ( button->x, button->y + button->height, 0 );

	glEnd ();

	int xpos = button->x + 5;
	int ypos = (button->y + button->height / 2) + 3;
		
	glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );    
    GciDrawText ( xpos, ypos, button->caption, HELVETICA_10 );

}

void ProxyDisable::GoClick ( Button *button )
{

	int pid;
	char bname [64];
	sscanf ( button->name, "%s %d", bname, &pid );

	ProxyDisable *thistask = (ProxyDisable *) SvbGetTask ( pid );
	UplinkAssert (thistask);

	// Set it going

	thistask->progress = 1;

	UplinkStrncpy ( thistask->ip, game->GetWorld ()->GetPlayer ()->remotehost, sizeof ( thistask->ip ) );
	
	// Look up the target

	VLocation *vl = game->GetWorld ()->GetVLocation ( thistask->ip );
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);

	thistask->ticksrequired = TICKSREQUIRED_ANALYSEPROXY * comp->security.NumSystems ();
	thistask->ticksdone = 0;

	// Update the display

//	char sprogress [128];
//	UplinkSnprintf ( sprogress, sizeof ( sprogress ), "proxydisable_progress %d", pid );
//	EclRegisterCaptionChange ( sprogress, "Analysing...", 0 );	
    
	
}

void ProxyDisable::CloseClick ( Button *button )
{

	int pid;
	char bname [64];
	sscanf ( button->name, "%s %d", bname, &pid );

	SvbRemoveTask ( pid );

}

ProxyDisable::ProxyDisable () : UplinkTask ()
{

	progress = 0;

}

ProxyDisable::~ProxyDisable ()
{
}


void ProxyDisable::SetTarget ( UplinkObject *uo, char *uos, int uoi )
{

	/*
		Don't do anything - we get our target from the current
		remote host

		*/

}

void ProxyDisable::MoveTo ( int x, int y, int time_ms )
{
}

void ProxyDisable::Initialise ()
{
}

void ProxyDisable::Tick ( int n )
{

	if ( IsInterfaceVisible () ) {

		// Stop the task if we've disconnected and we were running

		if ( progress > 0 &&
		     strcmp ( game->GetWorld ()->GetPlayer ()->remotehost, ip ) != 0 ) {

			SvbRemoveTask ( SvbLookupPID ( this ) );
            return;

		}
			

		char sprogress [128];
		//char sborder   [128];
		
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "proxydisable_progress %d", SvbLookupPID ( this ) );

		if ( progress == 0 ) {												// Do nothing - waiting for user to click GO			

		}
		else if ( progress == 1 ) {											// We are analysing			

			++ticksdone;

			EclGetButton ( sprogress )->width = (int) ( 40 * ((float) ticksdone / (float) ticksrequired) );
			EclDirtyButton ( sprogress );
				

			if ( ticksdone >= ticksrequired ) {

				// Start it disabling

				progress = 2;

				// Look up the target

				VLocation *vl = game->GetWorld ()->GetVLocation ( ip );
				UplinkAssert (vl);
				Computer *comp = vl->GetComputer ();
				UplinkAssert (comp);

				// Update the display

//				EclRegisterCaptionChange ( sprogress, "Disabling..." );
				
				ticksrequired = TICKSREQUIRED_DISABLEPROXY * comp->security.NumSystems ();
				ticksdone = 0;

                // They start tracing us

        		if ( comp->security.IsRunning_Monitor () )
					game->GetWorld ()->GetPlayer ()->connection.BeginTrace ();
                
			}

		}
		else if ( progress == 2 ) {											// We are disabling

			++ticksdone;

			EclGetButton ( sprogress )->width = (int) ( 40 + 80 * ((float) ticksdone / (float) ticksrequired) );
			EclDirtyButton ( sprogress );

			if ( ticksdone >= ticksrequired ) {

				// Finished - decide if we were successful or not
				// First Look up the target

				VLocation *vl = game->GetWorld ()->GetVLocation ( ip );
				UplinkAssert (vl);
				Computer *comp = vl->GetComputer ();
				UplinkAssert (comp);

				bool failed = false;

				for ( int i = 0; i < comp->security.NumSystems (); ++i ) {

					SecuritySystem *ss = comp->security.GetSystem (i);
					if ( ss && ss->TYPE == SECURITY_TYPE_PROXY && ss->enabled && !ss->bypassed ) {
						if ( version >= ss->level )
							ss->Disable ();

                        else
							failed = true;
					}
							

				}

				if ( failed ) {
					progress = 3;
					EclRegisterCaptionChange ( sprogress, "Failed" );
				}
				else {
					progress = 4;
					EclRegisterCaptionChange ( sprogress, "Success" );
				}

				timersync = time(NULL) + 2;

			}

		}
		else if ( progress == 3 ) {											// Unable to proceed due to insufficient version

			if ( time(NULL) > timersync ) {

				SvbRemoveTask ( SvbLookupPID ( this ) );

			}

		}
		else if ( progress == 4 ) {											// We are done

			if ( time(NULL) > timersync ) {

				SvbRemoveTask ( SvbLookupPID ( this ) );

			}

		}


	}

}
	
void ProxyDisable::CreateInterface ()
{

	if ( !IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char stitle    [128];
		char sborder   [128];
		char sprogress [128];
		char sclose    [128];

		UplinkSnprintf ( stitle, sizeof ( stitle ), "proxydisable_title %d", pid );
		UplinkSnprintf ( sborder, sizeof ( sborder ), "proxydisable_border %d", pid );
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "proxydisable_progress %d", pid );
		UplinkSnprintf ( sclose, sizeof ( sclose ), "proxydisable_close %d", pid );	

		EclRegisterButton ( 265, 422, 20, 15, "", "Disable any proxy servers running on this machine", stitle );
		button_assignbitmap ( stitle, "software/go.tif" );
		EclRegisterButtonCallback ( stitle, GoClick );

		EclRegisterButton ( 285, 422, 120, 15, "", "", sborder );
		EclRegisterButtonCallbacks ( sborder, BorderDraw, NULL, NULL, NULL );

		EclRegisterButton ( 285, 423, 0, 13, "Proxy Disable", "Shows the progress of the disable", sprogress );
		EclRegisterButtonCallbacks ( sprogress, ProgressDraw, NULL, NULL, NULL );		

		EclRegisterButton ( 405, 423, 13, 13, "", "Close the Proxy disabler", sclose );		
		button_assignbitmaps ( sclose, "close.tif", "close_h.tif", "close_c.tif" );
		EclRegisterButtonCallback ( sclose, CloseClick );
		
	}

}

void ProxyDisable::RemoveInterface ()
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char stitle    [128];
		char sborder   [128];
		char sprogress [128];
		char sclose    [128];

		UplinkSnprintf ( stitle, sizeof ( stitle ), "proxydisable_title %d", pid );
		UplinkSnprintf ( sborder, sizeof ( sborder ), "proxydisable_border %d", pid );
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "proxydisable_progress %d", pid );
		UplinkSnprintf ( sclose, sizeof ( sclose ), "proxydisable_close %d", pid );			

		EclRemoveButton ( stitle );
		EclRemoveButton ( sborder );
		EclRemoveButton ( sprogress );
		EclRemoveButton ( sclose );

	}

}

void ProxyDisable::ShowInterface ()
{

	if ( !IsInterfaceVisible () ) CreateInterface ();

	int pid = SvbLookupPID ( this );

	char stitle    [128];
	char sborder   [128];
	char sprogress [128];
	char sclose    [128];

	UplinkSnprintf ( stitle, sizeof ( stitle ), "proxydisable_title %d", pid );
	UplinkSnprintf ( sborder, sizeof ( sborder ), "proxydisable_border %d", pid );
	UplinkSnprintf ( sprogress, sizeof ( sprogress ), "proxydisable_progress %d", pid );
	UplinkSnprintf ( sclose, sizeof ( sclose ), "proxydisable_close %d", pid );			

	EclButtonBringToFront ( stitle );
	EclButtonBringToFront ( sborder );
	EclButtonBringToFront ( sprogress );
	EclButtonBringToFront ( sclose );

}

bool ProxyDisable::IsInterfaceVisible ()
{

	int pid = SvbLookupPID ( this );
	char stitle [128];
	UplinkSnprintf ( stitle, sizeof ( stitle ), "proxydisable_title %d", pid );

	return ( EclGetButton ( stitle ) != NULL );

}
