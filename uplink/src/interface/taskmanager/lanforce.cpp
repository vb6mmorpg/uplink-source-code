
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h> /*_glu_extention_library_*/

#include "vanbakel.h"

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
#include "interface/taskmanager/lanforce.h"
#include "interface/localinterface/lan_interface.h"

#include "mmgr.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))

LanForce::LanForce() : UplinkTask()
{

	status = LANFORCE_UNUSED;
	progress = 0;
	numticksrequired = 0;
	comp = NULL;
	systemIndex = 0;

}

LanForce::~LanForce()
{
}

void LanForce::BorderDraw ( Button *button, bool highlighted, bool clicked )
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

void LanForce::ProgressDraw ( Button *button, bool highlighted, bool clicked )
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

void LanForce::CloseClick ( Button *button )
{

	int pid;
	char bname [64];
	sscanf ( button->name, "%s %d", bname, &pid );

	SvbRemoveTask ( pid );

}

void LanForce::BorderClick ( Button *button )
{

	int pid;
	char bname [64];
	sscanf ( button->name, "%s %d", bname, &pid );

	game->GetInterface ()->GetTaskManager ()->SetTargetProgram ( pid );

}

void LanForce::SetTarget ( UplinkObject *uo, char *uos, int uoi )
{

	/*

		uo  : LanComputer object
		uos : Name of button representing system
		uoi : index of system in lan computer

		*/

	if ( status == LANFORCE_UNUSED || status == LANFORCE_FINISHED ) {

		if ( uo->GetOBJECTID() == OID_LANCOMPUTER ) {

			comp = (LanComputer *) uo;
			if ( !comp->systems.ValidIndex( uoi ) ) return;
			systemIndex = uoi;
			LanComputerSystem *system = comp->systems.GetData( uoi );
            LanInterfaceObject *lio = LanInterface::GetLanInterfaceObject( system->TYPE );
		
            if ( system->TYPE == LANSYSTEM_LOCK ) {

			    Button *button = EclGetButton ( uos );
			    UplinkAssert ( button );
			    MoveTo ( button->x - 20, button->y + lio->height + 5, 300 );

			    progress = 0;
			    numticksrequired = TICKSREQUIRED_FORCELANLOCK;

            }
            else {

                comp = NULL;
                systemIndex = 0;

            }

		}

	}

}

void LanForce::MoveTo ( int x, int y, int time_ms )
{

	UplinkTask::MoveTo ( x, y, time_ms );

	int pid = SvbLookupPID ( this );

	char stitle    [128];
	char sborder   [128];
	char sprogress [128];
	char sclose    [128];

	UplinkSnprintf ( stitle, sizeof ( stitle ), "lanforce_title %d", pid );
	UplinkSnprintf ( sborder, sizeof ( sborder ), "lanforce_border %d", pid );
	UplinkSnprintf ( sprogress, sizeof ( sprogress ), "lanforce_progress %d", pid );
	UplinkSnprintf ( sclose, sizeof ( sclose ), "lanforce_close %d", pid );	

	EclRegisterMovement ( stitle, x, y, time_ms);
	EclRegisterMovement ( sborder, x + 20, y, time_ms );
	EclRegisterMovement ( sprogress, x + 20, y + 1, time_ms );
	EclRegisterMovement ( sclose, x + 140, y + 1, time_ms );

	EclButtonBringToFront ( stitle );
	EclButtonBringToFront ( sborder );
	EclButtonBringToFront ( sprogress );
	EclButtonBringToFront ( sclose );

}

void LanForce::Initialise ()
{
}

void LanForce::Tick ( int n )
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );
		char sprogress [128];
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "lanforce_progress %d", pid );

		if ( status == LANFORCE_UNUSED ) {

			if ( comp ) {

				EclRegisterCaptionChange ( sprogress, "Forcing Lock...", 0 );
				status = LANFORCE_FORCING;

			}

		}
		else if ( status == LANFORCE_FORCING ) {

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

			progress = min ( progress + n, numticksrequired );
			UplinkAssert ( EclGetButton ( sprogress ) );
			EclGetButton ( sprogress )->width = (int) ( 120 * ( (float) progress / (float) numticksrequired ) );
			EclDirtyButton ( sprogress );
			
			if ( progress >= numticksrequired ) {

				UplinkAssert (comp->systems.ValidIndex(systemIndex));
    			LanComputerSystem *system = comp->systems.GetData( systemIndex );
	    		UplinkAssert (system);
                system->data1 = 0;

                LanMonitor::SysAdminAwaken ();

				EclRegisterCaptionChange ( sprogress, "Finished", 0 );
				status = LANFORCE_FINISHED;            
            }

		}
        else if ( status == LANFORCE_FINISHED ) {

            progress = 0;
            systemIndex = -1;
            comp = NULL;
            status = LANFORCE_UNUSED;

        }

	}

}
	
void LanForce::CreateInterface ()
{

	if ( !IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char stitle    [128];
		char sborder   [128];
		char sprogress [128];
		char sclose    [128];
		char tooltip   [128];

		UplinkSnprintf ( stitle, sizeof ( stitle ), "lanforce_title %d", pid );
		UplinkSnprintf ( sborder, sizeof ( sborder ), "lanforce_border %d", pid );
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "lanforce_progress %d", pid );
		UplinkSnprintf ( sclose, sizeof ( sclose ), "lanforce_close %d", pid );	
		UplinkSnprintf ( tooltip, sizeof ( tooltip ), "LAN Force v%1.1f", version );

		EclRegisterButton ( 265, 450, 20, 15, "", tooltip, stitle );
		button_assignbitmap ( stitle, "software/lan.tif" );

		EclRegisterButton ( 285, 450, 120, 15, "", "", sborder );
		EclRegisterButtonCallbacks ( sborder, BorderDraw, BorderClick, button_click, button_highlight );

		EclRegisterButton ( 285, 450, 120, 13, "Select target system", "Shows the progress of the LAN Force", sprogress );
		EclRegisterButtonCallbacks ( sprogress, ProgressDraw, BorderClick, button_click, button_highlight );		

		EclRegisterButton ( 405, 450, 13, 13, "", "Close the LAN Force", sclose );		
		button_assignbitmaps ( sclose, "close.tif", "close_h.tif", "close_c.tif" );
		EclRegisterButtonCallback ( sclose, CloseClick );
		
	}

}

void LanForce::RemoveInterface ()
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char stitle    [128];
		char sborder   [128];
		char sprogress [128];
		char sclose    [128];

		UplinkSnprintf ( stitle, sizeof ( stitle ), "lanforce_title %d", pid );
		UplinkSnprintf ( sborder, sizeof ( sborder ), "lanforce_border %d", pid );
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "lanforce_progress %d", pid );
		UplinkSnprintf ( sclose, sizeof ( sclose ), "lanforce_close %d", pid );	

		EclRemoveButton ( stitle );
		EclRemoveButton ( sborder );
		EclRemoveButton ( sprogress );
		EclRemoveButton ( sclose );

	}

}

void LanForce::ShowInterface ()
{

	if ( !IsInterfaceVisible () ) CreateInterface ();

	int pid = SvbLookupPID ( this );

	char stitle    [128];
	char sborder   [128];
	char sprogress [128];
	char sclose    [128];

	UplinkSnprintf ( stitle, sizeof ( stitle ), "lanforce_title %d", pid );
	UplinkSnprintf ( sborder, sizeof ( sborder ), "lanforce_border %d", pid );
	UplinkSnprintf ( sprogress, sizeof ( sprogress ), "lanforce_progress %d", pid );
	UplinkSnprintf ( sclose, sizeof ( sclose ), "lanforce_close %d", pid );	

	EclButtonBringToFront ( stitle );
	EclButtonBringToFront ( sborder );
	EclButtonBringToFront ( sprogress );
	EclButtonBringToFront ( sclose );

}

bool LanForce::IsInterfaceVisible ()
{
	
	int pid = SvbLookupPID ( this );

	char stitle [128];
	UplinkSnprintf ( stitle, sizeof ( stitle ), "lanforce_border %d", pid );
	
	return ( EclGetButton (stitle) != NULL );
	
}

