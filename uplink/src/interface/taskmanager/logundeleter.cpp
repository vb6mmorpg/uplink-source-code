
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h> /*_glu_extention_library_*/


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
#include "interface/taskmanager/logundeleter.h"
#include "interface/remoteinterface/remoteinterface.h"

#include "world/world.h"
#include "world/player.h"
#include "world/computer/computer.h"
#include "world/computer/logbank.h"
#include "world/computer/computerscreen/computerscreen.h"

#include "mmgr.h"


LogUnDeleter::LogUnDeleter () : UplinkTask ()
{

	source = NULL;
	sourceindex = -1;
	status = LOGUNDELETER_OFF;
	numticksrequired = 0;
	progress = 0;

}

LogUnDeleter::~LogUnDeleter ()
{
}

void LogUnDeleter::MoveTo ( int x, int y, int time_ms )
{

	UplinkTask::MoveTo ( x, y, time_ms );

	int pid = SvbLookupPID ( this );

	char stitle    [128];
	char sborder   [128];
	char sprogress [128];
	char sclose    [128];

	UplinkSnprintf ( stitle, sizeof ( stitle ), "logundeleter_title %d", pid );
	UplinkSnprintf ( sborder, sizeof ( sborder ), "logundeleter_border %d", pid );
	UplinkSnprintf ( sprogress, sizeof ( sprogress ), "logundeleter_progress %d", pid );
	UplinkSnprintf ( sclose, sizeof ( sclose ), "logundeleter_close %d", pid );	

	EclRegisterMovement ( stitle, x, y, time_ms);
	EclRegisterMovement ( sborder, x + 20, y, time_ms );
	EclRegisterMovement ( sprogress, x + 20, y + 1, time_ms );
	EclRegisterMovement ( sclose, x + 140, y + 1, time_ms );

	EclButtonBringToFront ( stitle );
	EclButtonBringToFront ( sborder );
	EclButtonBringToFront ( sprogress );
	EclButtonBringToFront ( sclose );

}

void LogUnDeleter::SetTarget ( UplinkObject *uo, char *uos, int uoi )
{

	/*

		uo  : LogBank object containing AccessLog to be un-deleted
		uos : Name of button representing data
		uoi : index of log in logbank

		*/

	if ( status == LOGUNDELETER_OFF ) {

		if ( uo->GetOBJECTID () == OID_LOGBANK ) {

			// Logbank selected

			if ( ((LogBank *) uo)->logs.ValidIndex (uoi) &&
				 ((LogBank *) uo)->internallogs.ValidIndex (uoi) ) {

				source = (LogBank *) uo;
				sourceindex = uoi;

				numticksrequired = TICKSREQUIRED_LOGUNDELETER;
				progress = 0;

				Button *button = EclGetButton ( uos );
				UplinkAssert (button);

				MoveTo ( button->x, button->y, 1000 );

				Computer *comp = game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ()->GetComputer ();
				UplinkAssert (comp);
		
				if ( comp->security.IsRunning_Monitor () ) 
					game->GetWorld ()->GetPlayer ()->GetConnection ()->BeginTrace ();

			}

		}

	}

}

void LogUnDeleter::BorderDraw ( Button *button, bool highlighted, bool clicked )
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

void LogUnDeleter::ProgressDraw ( Button *button, bool highlighted, bool clicked )
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

void LogUnDeleter::CloseClick ( Button *button )
{

	int pid;
	char bname [64];
	sscanf ( button->name, "%s %d", bname, &pid );

	SvbRemoveTask ( pid );

}

void LogUnDeleter::BorderClick ( Button *button )
{

	int pid;
	char bname [64];
	sscanf ( button->name, "%s %d", bname, &pid );

	game->GetInterface ()->GetTaskManager ()->SetTargetProgram ( pid );

}

void LogUnDeleter::Initialise ()
{
}

static bool LogUnDeleter_ReSetTargetProgram ( int pid )
{

	TaskManager *tm = game->GetInterface ()->GetTaskManager ();
	if ( tm->IsTargetProgramLast ( pid ) ) {

		char stitle    [128];
		char sborder   [128];
		char sprogress [128];
		char sclose    [128];

		UplinkSnprintf ( stitle, sizeof ( stitle ), "logundeleter_title %d", pid );
		UplinkSnprintf ( sborder, sizeof ( sborder ), "logundeleter_border %d", pid );
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "logundeleter_progress %d", pid );
		UplinkSnprintf ( sclose, sizeof ( sclose ), "logundeleter_close %d", pid );	

		int animationid;
		if ( ( animationid = EclIsNoCaptionChangeActive( stitle ) ) != -1 )
			EclRemoveAnimation ( animationid );

		if ( ( animationid = EclIsNoCaptionChangeActive( sborder ) ) != -1 )
			EclRemoveAnimation ( animationid );

		if ( ( animationid = EclIsNoCaptionChangeActive( sprogress ) ) != -1 )
			EclRemoveAnimation ( animationid );

		if ( ( animationid = EclIsNoCaptionChangeActive( sclose ) ) != -1 )
			EclRemoveAnimation ( animationid );

		tm->SetTargetProgram ( pid );
		return true;

	}

	return false;

}

void LogUnDeleter::Tick ( int n )
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );
		char sprogress [128];
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "logundeleter_progress %d", pid );

		for ( int tick = 0; tick < n; ++tick ) {

			if ( status == LOGUNDELETER_OFF ) {

				// Look for a new target

				if ( source ) {

					// A new source log has been specified - start undeleting it
					status = LOGUNDELETER_INPROGRESS;

					EclRegisterCaptionChange ( sprogress, "Recovering log...", 0 );

				}
				else {

					LogUnDeleter_ReSetTargetProgram ( pid );

				}

			}
			else if ( status == LOGUNDELETER_INPROGRESS ) {

				UplinkAssert (source);
				
				// Check log is still there
				if ( !source->logs.ValidIndex (sourceindex) ) status = LOGUNDELETER_FINISHED;
				if ( !source->internallogs.ValidIndex (sourceindex) ) status = LOGUNDELETER_FINISHED;

                // Check we are still connected
                if ( !game->GetWorld ()->GetPlayer ()->IsConnected () ) {
                    SvbRemoveTask (pid);
                    return;
                }

				// Delete in progress
				++progress;		
				UplinkAssert ( EclGetButton ( sprogress ) );
				EclGetButton ( sprogress )->width = (int) ( 120 * ( (float) progress / (float) numticksrequired ) );
				EclDirtyButton ( sprogress );

				if ( progress >= numticksrequired ) {

					// Un-delete the log	
					//source->logs.PutData ( source->internallogs.GetData (sourceindex), sourceindex );
					source->logs.GetData ( sourceindex )->SetProperties ( source->internallogs.GetData (sourceindex) );

					// Finished 
					status = LOGUNDELETER_FINISHED;
					EclRegisterCaptionChange ( sprogress, "Finished" );

					LogUnDeleter_ReSetTargetProgram ( pid );

				}
				

			}
			else if ( status == LOGUNDELETER_FINISHED ) {
				
				source = NULL;
				sourceindex = -1;
				numticksrequired = 0;
				progress = 0;
				EclRegisterCaptionChange ( sprogress, "Finished" );
				status = LOGUNDELETER_OFF;		

				LogUnDeleter_ReSetTargetProgram ( pid );

			}

		}

	}

}

void LogUnDeleter::CreateInterface ()
{

	if ( !IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char stitle    [128];
		char sborder   [128];
		char sprogress [128];
		char sclose    [128];
		char tooltip   [128];

		UplinkSnprintf ( stitle, sizeof ( stitle ), "logundeleter_title %d", pid );
		UplinkSnprintf ( sborder, sizeof ( sborder ), "logundeleter_border %d", pid );
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "logundeleter_progress %d", pid );
		UplinkSnprintf ( sclose, sizeof ( sclose ), "logundeleter_close %d", pid );	
		UplinkSnprintf ( tooltip, sizeof ( tooltip ), "Log Deleter v%1.1f", version );

		EclRegisterButton ( 265, 450, 20, 15, "", tooltip, stitle );
		button_assignbitmap ( stitle, "software/log.tif" );

		EclRegisterButton ( 285, 450, 120, 15, "", "", sborder );
		EclRegisterButtonCallbacks ( sborder, BorderDraw, BorderClick, button_click, button_highlight );

		EclRegisterButton ( 285, 450, 120, 13, "Select target log", "Shows the progress of the log undelete", sprogress );
		EclRegisterButtonCallbacks ( sprogress, ProgressDraw, BorderClick, button_click, button_highlight );		

		EclRegisterButton ( 405, 450, 13, 13, "", "Close the LogUnDeleter", sclose );		
		button_assignbitmaps ( sclose, "close.tif", "close_h.tif", "close_c.tif" );
		EclRegisterButtonCallback ( sclose, CloseClick );
		
	}

}

void LogUnDeleter::RemoveInterface ()
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char stitle    [128];
		char sborder   [128];
		char sprogress [128];
		char sclose    [128];

		UplinkSnprintf ( stitle, sizeof ( stitle ), "logundeleter_title %d", pid );
		UplinkSnprintf ( sborder, sizeof ( sborder ), "logundeleter_border %d", pid );
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "logundeleter_progress %d", pid );
		UplinkSnprintf ( sclose, sizeof ( sclose ), "logundeleter_close %d", pid );	

		EclRemoveButton ( stitle );
		EclRemoveButton ( sborder );
		EclRemoveButton ( sprogress );
		EclRemoveButton ( sclose );

	}

}

void LogUnDeleter::ShowInterface ()
{

	if ( !IsInterfaceVisible () ) CreateInterface ();

	int pid = SvbLookupPID ( this );

	char stitle    [128];
	char sborder   [128];
	char sprogress [128];
	char sclose    [128];

	UplinkSnprintf ( stitle, sizeof ( stitle ), "logundeleter_title %d", pid );
	UplinkSnprintf ( sborder, sizeof ( sborder ), "logundeleter_border %d", pid );
	UplinkSnprintf ( sprogress, sizeof ( sprogress ), "logundeleter_progress %d", pid );
	UplinkSnprintf ( sclose, sizeof ( sclose ), "logundeleter_close %d", pid );	

	EclButtonBringToFront ( stitle );
	EclButtonBringToFront ( sborder );
	EclButtonBringToFront ( sprogress );
	EclButtonBringToFront ( sclose );


}

bool LogUnDeleter::IsInterfaceVisible ()
{

	int pid = SvbLookupPID ( this );

	char stitle [128];
	UplinkSnprintf ( stitle, sizeof ( stitle ), "logundeleter_border %d", pid );
	
	return ( EclGetButton (stitle) != NULL );

}
