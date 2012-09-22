

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h> /*_glu_extention_library_*/


#include "eclipse.h"
#include "vanbakel.h"
#include "soundgarden.h"
#include "redshirt.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"
#include "app/opengl_interface.h"

#include "game/game.h"
#include "game/data/data.h"

#include "world/world.h"
#include "world/player.h"
#include "world/computer/computer.h"
#include "world/computer/lancomputer.h"
#include "world/computer/computerscreen/genericscreen.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/taskmanager/lanscan.h"

#include "mmgr.h"

#define min(a,b) (((a) < (b)) ? (a) : (b))

LanScan::LanScan() : UplinkTask()
{
	status = LANSCAN_UNUSED;
	progress = 0;
	numticksrequired = 0;
	UplinkStrncpy ( ip, "N", sizeof ( ip ) );
}

LanScan::~LanScan()
{
}

void LanScan::BorderDraw ( Button *button, bool highlighted, bool clicked )
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

void LanScan::ProgressDraw ( Button *button, bool highlighted, bool clicked )
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

void LanScan::GoClick ( Button *button )
{

	int pid;
	char bname [64];
	sscanf ( button->name, "%s %d", bname, &pid );

	LanScan *thistask = (LanScan *) SvbGetTask ( pid );
	UplinkAssert (thistask);
		
	Computer *comp = game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ()->GetComputer ();
	UplinkAssert ( comp );

	if ( comp->security.IsRunning_Encryption () ) {
		create_msgbox ( "Error", "Connection is encrypted." );
		return;
	}

	// Set it going

	UplinkStrncpy ( thistask->ip, game->GetWorld ()->GetPlayer ()->remotehost, sizeof ( thistask->ip ) );
	thistask->numticksrequired = TICKSREQUIRED_LANSCAN;
	thistask->progress = 0;
	thistask->status = LANSCAN_SCANNING;

	char sprogress [128];
	UplinkSnprintf ( sprogress, sizeof ( sprogress ), "lanscan_progress %d", pid );
	EclRegisterCaptionChange ( sprogress, "Scanning...", 0 );	

}

void LanScan::CloseClick ( Button *button )
{

	int pid;
	char bname [64];
	sscanf ( button->name, "%s %d", bname, &pid );

	SvbRemoveTask ( pid );

}

void LanScan::SetTarget ( UplinkObject *uo, char *uos, int uoi )
{

	/*
		Don't do anything - we get our target from the current
		remote host

		*/

}

void LanScan::MoveTo ( int x, int y, int time_ms )
{
}

void LanScan::Initialise ()
{
}

void LanScan::Tick ( int n )
{

	if ( IsInterfaceVisible () ) {

		// Stop the task if we've disconnected and we were running

		if ( progress > 0 &&
		     strcmp ( game->GetWorld ()->GetPlayer ()->remotehost, ip ) != 0 ) {

			SvbRemoveTask ( SvbLookupPID ( this ) );
            return;

		}	

		char sprogress [128];
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "lanscan_progress %d", SvbLookupPID ( this ) );

		if ( status == LANSCAN_SCANNING ) {


			VLocation *vl = game->GetWorld ()->GetVLocation ( ip );
			UplinkAssert (vl);
			Computer *comp = vl->GetComputer ();
			UplinkAssert (comp);

			if ( comp->GetOBJECTID () != OID_LANCOMPUTER ) {
				status = LANSCAN_FINISHED;
				return;
			}

			//
			// Have anymore systems become visible ?

			progress = min ( progress + n, numticksrequired );

			EclGetButton ( sprogress )->width = (int) ( 120 * ((float) progress / (float) numticksrequired) );
			EclDirtyButton ( sprogress );

			int numSubnetsScanned = (int) ( LAN_SUBNETRANGE * ((float) progress / (float) numticksrequired) );

			LanComputer *lanComp = (LanComputer *) comp;

			for ( int i = 0; i < lanComp->systems.Size (); ++i ) {
				if ( lanComp->systems.ValidIndex( i ) ) {

					LanComputerSystem *system = lanComp->systems.GetData(i);
					UplinkAssert (system);

					if ( system->subnet <= numSubnetsScanned &&
						system->visible < LANSYSTEMVISIBLE_TYPE ) {

						int difference = (int) ( (version - 1) - system->security );

                        if ( difference == 0 ) {
							system->IncreaseVisibility( LANSYSTEMVISIBLE_AWARE );
                            SgPlaySound ( RsArchiveFileOpen ( "sounds/done.wav" ), "sounds/done.wav" );
                        }
                        else if ( difference == 1 ) {
							system->IncreaseVisibility( LANSYSTEMVISIBLE_TYPE );
                            SgPlaySound ( RsArchiveFileOpen ( "sounds/done.wav" ), "sounds/done.wav" );
                        }
                            
					}

				}
			}

			//
			// Have we finished?

			if ( progress >= numticksrequired )
				status = LANSCAN_FINISHED;

		}
		else if ( status == LANSCAN_FINISHED ) {

			EclRegisterCaptionChange ( sprogress, "Finished...", 0 );	
			progress = 0;
			numticksrequired = 0;
			status = LANSCAN_UNUSED;

		}

	}

}
	
void LanScan::CreateInterface ()
{

	if ( !IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char stitle    [128];
		char sborder   [128];
		char sprogress [128];
		char sclose    [128];

		UplinkSnprintf ( stitle, sizeof ( stitle ), "lanscan_title %d", pid );
		UplinkSnprintf ( sborder, sizeof ( sborder ), "lanscan_border %d", pid );
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "lanscan_progress %d", pid );
		UplinkSnprintf ( sclose, sizeof ( sclose ), "lanscan_close %d", pid );	

		EclRegisterButton ( 265, 422, 20, 15, "", "Scan the Local Area Network for systems", stitle );
		button_assignbitmap ( stitle, "software/go.tif" );
		EclRegisterButtonCallback ( stitle, GoClick );

		EclRegisterButton ( 285, 422, 120, 15, "", "", sborder );
		EclRegisterButtonCallbacks ( sborder, BorderDraw, NULL, NULL, NULL );

		EclRegisterButton ( 285, 423, 0, 13, "LAN Scan", "Shows the progress of the scan", sprogress );
		EclRegisterButtonCallbacks ( sprogress, ProgressDraw, NULL, NULL, NULL );		

		EclRegisterButton ( 405, 423, 13, 13, "", "Close the LAN Scan", sclose );		
		button_assignbitmaps ( sclose, "close.tif", "close_h.tif", "close_c.tif" );
		EclRegisterButtonCallback ( sclose, CloseClick );
		
	}

}

void LanScan::RemoveInterface ()
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char stitle    [128];
		char sborder   [128];
		char sprogress [128];
		char sclose    [128];

		UplinkSnprintf ( stitle, sizeof ( stitle ), "lanscan_title %d", pid );
		UplinkSnprintf ( sborder, sizeof ( sborder ), "lanscan_border %d", pid );
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "lanscan_progress %d", pid );
		UplinkSnprintf ( sclose, sizeof ( sclose ), "lanscan_close %d", pid );			

		EclRemoveButton ( stitle );
		EclRemoveButton ( sborder );
		EclRemoveButton ( sprogress );
		EclRemoveButton ( sclose );

	}

}

void LanScan::ShowInterface ()
{

	if ( !IsInterfaceVisible () ) CreateInterface ();

	int pid = SvbLookupPID ( this );

	char stitle    [128];
	char sborder   [128];
	char sprogress [128];
	char sclose    [128];

	UplinkSnprintf ( stitle, sizeof ( stitle ), "lanscan_title %d", pid );
	UplinkSnprintf ( sborder, sizeof ( sborder ), "lanscan_border %d", pid );
	UplinkSnprintf ( sprogress, sizeof ( sprogress ), "lanscan_progress %d", pid );
	UplinkSnprintf ( sclose, sizeof ( sclose ), "lanscan_close %d", pid );			

	EclButtonBringToFront ( stitle );
	EclButtonBringToFront ( sborder );
	EclButtonBringToFront ( sprogress );
	EclButtonBringToFront ( sclose );

}

bool LanScan::IsInterfaceVisible ()
{
	
	int pid = SvbLookupPID ( this );
	char stitle [128];
	UplinkSnprintf ( stitle, sizeof ( stitle ), "lanscan_title %d", pid );

	return ( EclGetButton ( stitle ) != NULL );

}

