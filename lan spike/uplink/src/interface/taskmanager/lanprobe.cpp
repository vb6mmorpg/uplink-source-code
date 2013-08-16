
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h> /*_glu_extention_library_*/

#include "vanbakel.h"
#include "soundgarden.h"
#include "redshirt.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"
#include "app/serialise.h"

#include "game/game.h"
#include "game/data/data.h"

#include "world/world.h"
#include "world/player.h"
#include "world/computer/lancomputer.h"
#include "world/computer/lanmonitor.h"

#include "interface/interface.h"
#include "interface/taskmanager/taskmanager.h"
#include "interface/taskmanager/lanprobe.h"
#include "interface/localinterface/lan_interface.h"

#include "mmgr.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))

LanProbe::LanProbe() : UplinkTask()
{

	status = LANPROBE_UNUSED;
	progress = 0;
	numticksrequired = 0;
	comp = NULL;
	systemIndex = 0;

}

LanProbe::~LanProbe()
{
}

void LanProbe::BorderDraw ( Button *button, bool highlighted, bool clicked )
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

void LanProbe::ProgressDraw ( Button *button, bool highlighted, bool clicked )
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

void LanProbe::CloseClick ( Button *button )
{

	int pid;
	char bname [64];
	sscanf ( button->name, "%s %d", bname, &pid );

	SvbRemoveTask ( pid );

}

void LanProbe::BorderClick ( Button *button )
{

	int pid;
	char bname [64];
	sscanf ( button->name, "%s %d", bname, &pid );

	game->GetInterface ()->GetTaskManager ()->SetTargetProgram ( pid );

}

void LanProbe::SetTarget ( UplinkObject *uo, char *uos, int uoi )
{

	/*

		uo  : LanComputer object
		uos : Name of button representing system
		uoi : index of system in lan computer

		*/

	if ( status == LANPROBE_UNUSED || status == LANPROBE_FINISHED ) {

		if ( uo->GetOBJECTID() == OID_LANCOMPUTER ) {

			comp = (LanComputer *) uo;
			if ( !comp->systems.ValidIndex( uoi ) ) return;
			systemIndex = uoi;
			LanComputerSystem *system = comp->systems.GetData( uoi );
            LanInterfaceObject *lio = LanInterface::GetLanInterfaceObject( system->TYPE );

			Button *button = EclGetButton ( uos );
			UplinkAssert ( button );
			MoveTo ( button->x - 20, button->y + lio->height + 5, 300 );

			progress = 0;
			numticksrequired = TICKSREQUIRED_SCANLANSYSTEM;
			
		}

	}

}

void LanProbe::MoveTo ( int x, int y, int time_ms )
{

	UplinkTask::MoveTo ( x, y, time_ms );

	int pid = SvbLookupPID ( this );

	char stitle    [128];
	char sborder   [128];
	char sprogress [128];
	char sclose    [128];

	UplinkSnprintf ( stitle, sizeof ( stitle ), "lanprobe_title %d", pid );
	UplinkSnprintf ( sborder, sizeof ( sborder ), "lanprobe_border %d", pid );
	UplinkSnprintf ( sprogress, sizeof ( sprogress ), "lanprobe_progress %d", pid );
	UplinkSnprintf ( sclose, sizeof ( sclose ), "lanprobe_close %d", pid );	

	EclRegisterMovement ( stitle, x, y, time_ms);
	EclRegisterMovement ( sborder, x + 20, y, time_ms );
	EclRegisterMovement ( sprogress, x + 20, y + 1, time_ms );
	EclRegisterMovement ( sclose, x + 140, y + 1, time_ms );

	EclButtonBringToFront ( stitle );
	EclButtonBringToFront ( sborder );
	EclButtonBringToFront ( sprogress );
	EclButtonBringToFront ( sclose );

}

void LanProbe::Initialise ()
{
}

void LanProbe::Tick ( int n )
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );
		char sprogress [128];
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "lanprobe_progress %d", pid );

		if ( status == LANPROBE_UNUSED ) {

			if ( comp ) {

				EclRegisterCaptionChange ( sprogress, "Scanning system...", 0 );
				status = LANPROBE_SCANNINGSYSTEM;

			}

		}
		else if ( status == LANPROBE_SCANNINGSYSTEM ) {

			// Check the system still exists
			if ( !comp || !comp->systems.ValidIndex( systemIndex ) ) {
				SvbRemoveTask ( pid );
				return;
			}

            // Check we are still connected
            if ( !game->GetWorld ()->GetPlayer ()->IsConnected () ) {
                SvbRemoveTask (pid);
                return;
            }

			UplinkAssert (comp->systems.ValidIndex(systemIndex));
			LanComputerSystem *system = comp->systems.GetData( systemIndex );
			UplinkAssert (system);

			progress = min ( progress + n, numticksrequired );
			UplinkAssert ( EclGetButton ( sprogress ) );
			EclGetButton ( sprogress )->width = (int) ( 60 * ( (float) progress / (float) numticksrequired ) );
			EclDirtyButton ( sprogress );
			
			if ( system->visible >= LANSYSTEMVISIBLE_TYPE || 
				 progress >= numticksrequired ) {

				system->IncreaseVisibility( LANSYSTEMVISIBLE_TYPE );

				EclRegisterCaptionChange ( sprogress, "Scanning links...", 0 );
				status = LANPROBE_SCANNINGLINKS;
				progress = 0;
				numticksrequired = TICKSREQUIRED_SCANLANLINKS;

			}
	
		}
		else if ( status == LANPROBE_SCANNINGLINKS ) {

			// Check the system still exists
			if ( !comp || !comp->systems.ValidIndex( systemIndex ) ) {
				SvbRemoveTask ( pid );
				return;
			}

            // Check we are still connected
            if ( !game->GetWorld ()->GetPlayer ()->IsConnected () ) {
                SvbRemoveTask (pid);
                return;
            }

			UplinkAssert (comp->systems.ValidIndex(systemIndex));
			LanComputerSystem *system = comp->systems.GetData( systemIndex );
			UplinkAssert (system);

			progress = min ( progress + n, numticksrequired );
			UplinkAssert ( EclGetButton ( sprogress ) );
			EclGetButton ( sprogress )->width = (int) ( 60 + 60 * ( (float) progress / (float) numticksrequired ) );
			EclDirtyButton ( sprogress );

            if ( LanMonitor::IsAccessable( systemIndex ) ) {

			    // Every link with a port number <= this has been scanned
			    int portDone = (int) ( LAN_LINKPORTRANGE * ( (float) progress / (float) TICKSREQUIRED_SCANLANLINKS ) );

			    for ( int i = 0; i < comp->links.Size(); ++i ) {
				    if ( comp->links.ValidIndex( i ) ) {

					    LanComputerLink *link = comp->links.GetData( i );
					    UplinkAssert (link);
	    
					    if ( link->visible < LANLINKVISIBLE_AWARE &&
						     link->port <= portDone ) {
   
						    if ( link->from == systemIndex ) {

    					    	SgPlaySound ( RsArchiveFileOpen ( "sounds/done.wav" ), "sounds/done.wav" );

                                if ( version >= link->security ) {
								    link->IncreaseVisibility( LANLINKVISIBLE_AWARE );

							        if ( comp->systems.ValidIndex( link->to ) ) {
								        LanComputerSystem *target = comp->systems.GetData( link->to );
								        UplinkAssert (target);
								        target->IncreaseVisibility( LANSYSTEMVISIBLE_AWARE );
							        }

                                }
							    else
                                    link->IncreaseVisibility( LANLINKVISIBLE_FROMAWARE );


						    }
						    else if ( link->to == systemIndex ) {

    					    	SgPlaySound ( RsArchiveFileOpen ( "sounds/done.wav" ), "sounds/done.wav" );

                                if ( version >= link->security ) {
								    link->IncreaseVisibility( LANLINKVISIBLE_AWARE );

							        if ( comp->systems.ValidIndex( link->from ) ) {
								        LanComputerSystem *target = comp->systems.GetData( link->from );
								        UplinkAssert (target);
								        target->IncreaseVisibility( LANSYSTEMVISIBLE_AWARE );
							        }

                                }
							    else
								    link->IncreaseVisibility( LANLINKVISIBLE_TOAWARE );

						    }

					    }						

				    }

                }

			}

			if ( progress >= TICKSREQUIRED_SCANLANLINKS ) {
				EclRegisterCaptionChange ( sprogress, "Finished", 0 );
				status = LANPROBE_FINISHED;
			}

		}
        else if ( status == LANPROBE_FINISHED ) {

            progress = 0;
            systemIndex = -1;
            comp = NULL;
            status = LANPROBE_UNUSED;

        }

	}

}
	
void LanProbe::CreateInterface ()
{

	if ( !IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char stitle    [128];
		char sborder   [128];
		char sprogress [128];
		char sclose    [128];
		char tooltip   [128];

		UplinkSnprintf ( stitle, sizeof ( stitle ), "lanprobe_title %d", pid );
		UplinkSnprintf ( sborder, sizeof ( sborder ), "lanprobe_border %d", pid );
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "lanprobe_progress %d", pid );
		UplinkSnprintf ( sclose, sizeof ( sclose ), "lanprobe_close %d", pid );	
		UplinkSnprintf ( tooltip, sizeof ( tooltip ), "LAN Probe v%1.1f", version );

		EclRegisterButton ( 265, 450, 20, 15, "", tooltip, stitle );
		button_assignbitmap ( stitle, "software/lan.tif" );

		EclRegisterButton ( 285, 450, 120, 15, "", "", sborder );
		EclRegisterButtonCallbacks ( sborder, BorderDraw, BorderClick, button_click, button_highlight );

		EclRegisterButton ( 285, 450, 120, 13, "Select target system", "Shows the progress of the LAN Probe", sprogress );
		EclRegisterButtonCallbacks ( sprogress, ProgressDraw, BorderClick, button_click, button_highlight );		

		EclRegisterButton ( 405, 450, 13, 13, "", "Close the LAN Probe", sclose );		
		button_assignbitmaps ( sclose, "close.tif", "close_h.tif", "close_c.tif" );
		EclRegisterButtonCallback ( sclose, CloseClick );
		
	}

}

void LanProbe::RemoveInterface ()
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char stitle    [128];
		char sborder   [128];
		char sprogress [128];
		char sclose    [128];

		UplinkSnprintf ( stitle, sizeof ( stitle ), "lanprobe_title %d", pid );
		UplinkSnprintf ( sborder, sizeof ( sborder ), "lanprobe_border %d", pid );
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "lanprobe_progress %d", pid );
		UplinkSnprintf ( sclose, sizeof ( sclose ), "lanprobe_close %d", pid );	

		EclRemoveButton ( stitle );
		EclRemoveButton ( sborder );
		EclRemoveButton ( sprogress );
		EclRemoveButton ( sclose );

	}

}

void LanProbe::ShowInterface ()
{

	if ( !IsInterfaceVisible () ) CreateInterface ();

	int pid = SvbLookupPID ( this );

	char stitle    [128];
	char sborder   [128];
	char sprogress [128];
	char sclose    [128];

	UplinkSnprintf ( stitle, sizeof ( stitle ), "lanprobe_title %d", pid );
	UplinkSnprintf ( sborder, sizeof ( sborder ), "lanprobe_border %d", pid );
	UplinkSnprintf ( sprogress, sizeof ( sprogress ), "lanprobe_progress %d", pid );
	UplinkSnprintf ( sclose, sizeof ( sclose ), "lanprobe_close %d", pid );	

	EclButtonBringToFront ( stitle );
	EclButtonBringToFront ( sborder );
	EclButtonBringToFront ( sprogress );
	EclButtonBringToFront ( sclose );

}

bool LanProbe::IsInterfaceVisible ()
{
	
	int pid = SvbLookupPID ( this );

	char stitle [128];
	UplinkSnprintf ( stitle, sizeof ( stitle ), "lanprobe_border %d", pid );
	
	return ( EclGetButton (stitle) != NULL );
	
}

