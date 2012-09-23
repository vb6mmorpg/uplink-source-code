

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h> /*_glu_extention_library_*/


#include "vanbakel.h"
#include "eclipse.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"
#include "app/opengl_interface.h"
#include "app/miscutils.h"

#include "game/game.h"
#include "game/data/data.h"

#include "interface/interface.h"
#include "interface/taskmanager/taskmanager.h"
#include "interface/taskmanager/logmodifier.h"

#include "world/world.h"
#include "world/date.h"
#include "world/player.h"
#include "world/computer/logbank.h"

#include "mmgr.h"

static int numlogtypes = 9 - 1;					// (-1 to 7)

static char *logtypes [] = {	"Deleted",
								"None",
								"Text",
								"ConectionOpened",
								"ConnectionClosed",
								"BounceBegin",
								"Bounce",
								"TransferTo",
								"TransferFrom"   };


void LogModifier::CloseClick ( Button *button )
{

	int pid;
	char bname [64];
	sscanf ( button->name, "%s %d", bname, &pid );

	SvbRemoveTask ( pid );

}

void LogModifier::BorderClick ( Button *button )
{

	int pid;
	char bname [64];
	sscanf ( button->name, "%s %d", bname, &pid );

	game->GetInterface ()->GetTaskManager ()->SetTargetProgram ( pid );

}

void LogModifier::BackgroundDraw ( Button *button, bool highlighted, bool clicked )
{

	glBegin ( GL_QUADS );
		glColor3ub ( 8, 20, 124 );		glVertex2i ( button->x - 1, button->y );
		glColor3ub ( 8, 20, 0 );		glVertex2i ( button->x + button->width, button->y );
		glColor3ub ( 8, 20, 124 );		glVertex2i ( button->x + button->width, button->y + button->height );
		glColor3ub ( 8, 20, 0 );		glVertex2i ( button->x, button->y + button->height );
	glEnd ();

	glColor3ub ( 81, 138, 215 );
	border_draw ( button );
	
}

void LogModifier::CommitClick ( Button *button )
{

	int pid;
	char bname [64];
	sscanf ( button->name, "%s %d", bname, &pid );

	LogModifier *thistask = (LogModifier *) SvbGetTask ( pid );
	UplinkAssert (thistask);
	
	// Stop the existing text buttons from being modified

	char nexttype  [64];
	char fromip	[64];
	char data1	[64];
	
	UplinkSnprintf ( nexttype, sizeof ( nexttype ), "logmodifier_nexttype %d",	pid );
	UplinkSnprintf ( fromip, sizeof ( fromip ),	"logmodifier_fromip %d",	pid );
	UplinkSnprintf ( data1, sizeof ( data1 ),	"logmodifier_data1 %d",		pid );

	EclRemoveButton ( nexttype );
	EclRegisterButtonCallbacks ( fromip, textbutton_draw, NULL, NULL, NULL );
	EclRegisterButtonCallbacks ( data1, textbutton_draw, NULL, NULL, NULL );

	// Set the status up for modifying the log

	thistask->status = LOGMODIFIER_STATUS_MODIFYING;
	thistask->numticksrequired = TICKSREQUIRED_LOGMODIFIER;
	thistask->progress = 0;
	
}

void LogModifier::NextTypeClick ( Button *button )
{

	int pid;
	char bname [64];
	sscanf ( button->name, "%s %d", bname, &pid );

	LogModifier *thistask = (LogModifier *) SvbGetTask ( pid );
	UplinkAssert (thistask);

	thistask->logtype ++;

	if ( thistask->logtype >= numlogtypes )
		thistask->logtype = -1;

	ChangeLogType ( pid );

}

void LogModifier::ChangeLogType ( int pid )
{

	LogModifier *thistask = (LogModifier *) SvbGetTask ( pid );
	UplinkAssert (thistask);

	char ltype	[64];
	UplinkSnprintf ( ltype, sizeof ( ltype ),	"logmodifier_ltype %d",		pid );
	EclGetButton ( ltype )->SetCaption ( logtypes [thistask->logtype+1] );

	char data1 [64];
	UplinkSnprintf ( data1, sizeof ( data1 ), "logmodifier_tdata1 %d", pid );
	
	switch ( thistask->logtype ) {
		
		case LOG_TYPE_DELETED:
		case LOG_TYPE_NONE:
		case LOG_TYPE_TEXT:
		case LOG_TYPE_CONNECTIONOPENED:
		case LOG_TYPE_CONNECTIONCLOSED:

			EclGetButton ( data1 )->SetCaption ( "NA" );
			break;

		case LOG_TYPE_BOUNCEBEGIN:
		case LOG_TYPE_BOUNCE:

			EclGetButton ( data1 )->SetCaption ( "To" );
			break;

		default:

			EclGetButton ( data1 )->SetCaption ( "NA" );

	};
	
}

void LogModifier::BorderDraw ( Button *button, bool highlighted, bool clicked )
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

void LogModifier::ProgressDraw ( Button *button, bool highlighted, bool clicked )
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

LogModifier::LogModifier () : UplinkTask ()
{

	source = NULL;
	sourceindex = -1;
	status = LOGMODIFIER_STATUS_NONE;

}

LogModifier::~LogModifier ()
{
}


void LogModifier::SetTarget ( UplinkObject *uo, char *uos, int uoi )
{

	/*

		uo  : LogBank object containing AccessLog to be un-deleted
		uos : Name of button representing data
		uoi : index of log in logbank

		*/

	UplinkAssert (uo);
	
	if ( uo->GetOBJECTID () == OID_LOGBANK ) {

		if ( status != LOGMODIFIER_STATUS_MODIFYING ) {

			if ( ((LogBank *) uo)->logs.ValidIndex (uoi) ) {
				// Copy those variables!

				source = (LogBank *) uo;
				sourceindex = uoi;

				if ( source->logs.ValidIndex ( uoi ) ) {
	
					// Create the expanded interface
					CreateExpandedInterface ();

				}
				else {

					// Targetting a blank spot - create a new log if >= v2.0

					if ( version >= 2.0 ) {

						status = LOGMODIFIER_STATUS_CREATING;
						numticksrequired = TICKSREQUIRED_LOGMODIFIER;
						progress = 0;

					}

				}

				// Move the whole thing

				Button *b = EclGetButton ( uos );
				UplinkAssert (b);
				MoveTo ( b->x, b->y, 1000 );

			}
		}

	}

}

void LogModifier::MoveTo ( int x, int y, int time_ms )
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char stitle    [64];
		char sborder   [64];
		char sprogress [64];
		char sclose    [64];
				
		UplinkSnprintf ( stitle, sizeof ( stitle ), "logmodifier_title %d", pid );
		UplinkSnprintf ( sborder, sizeof ( sborder ), "logmodifier_border %d", pid );
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "logmodifier_progress %d", pid );
		UplinkSnprintf ( sclose, sizeof ( sclose ), "logmodifier_close %d", pid );	
				
		EclRegisterMovement ( stitle, x, y, time_ms );
		EclRegisterMovement ( sborder, x + 20, y, time_ms );
		EclRegisterMovement ( sprogress, x + 20, y, time_ms );
		EclRegisterMovement ( sclose, x + 140, y, time_ms );
		
		if ( IsExpandedInterfaceVisible () ) {

			char background [64];
			char t_ltype	[64];
			char ltype		[64];
			char t_fromip	[64];
			char fromip		[64];
			char t_data1	[64];
			char data1		[64];

			char nexttype   [64];
			char commit		[64];
			char close		[64];

			UplinkSnprintf ( background, sizeof ( background ), "logmodifier_background %d",	pid );
			UplinkSnprintf ( t_ltype, sizeof ( t_ltype ),	  "logmodifier_tltype %d",		pid );
			UplinkSnprintf ( ltype, sizeof ( ltype ),	  "logmodifier_ltype %d",		pid );
			UplinkSnprintf ( t_fromip, sizeof ( t_fromip ),	  "logmodifier_tfromip %d",		pid );
			UplinkSnprintf ( fromip, sizeof ( fromip ),  	  "logmodifier_fromip %d",		pid );
			UplinkSnprintf ( t_data1, sizeof ( t_data1 ),	  "logmodifier_tdata1 %d",		pid );
			UplinkSnprintf ( data1, sizeof ( data1 ),	  "logmodifier_data1 %d",		pid );
			
			UplinkSnprintf ( nexttype, sizeof ( nexttype ),	  "logmodifier_nexttype %d",	pid );
			UplinkSnprintf ( commit, sizeof ( commit ),	  "logmodifier_commit %d",		pid );
			UplinkSnprintf ( close, sizeof ( close ),	  "logmodifier_close2 %d",		pid );

			EclRegisterMovement ( background,	x + 155, y,			time_ms );
			EclRegisterMovement ( t_ltype,		x + 165, y + 10,	time_ms );
			EclRegisterMovement ( ltype,		x + 215, y + 10,	time_ms );
			EclRegisterMovement ( t_fromip,		x + 165, y + 35,	time_ms );
			EclRegisterMovement ( fromip,		x + 215, y + 35,	time_ms );
			EclRegisterMovement ( t_data1,		x + 165, y + 60,	time_ms );
			EclRegisterMovement ( data1,		x + 215, y + 60,	time_ms );

			EclRegisterMovement ( nexttype,		x + 315, y + 10,	time_ms );
			EclRegisterMovement ( commit,		x + 165, y + 85,	time_ms );
			EclRegisterMovement ( close,		x + 220, y + 85,	time_ms );

		}

	}

}


void LogModifier::Initialise ()
{
}

void LogModifier::Tick ( int n )
{

	if ( IsInterfaceVisible () ) {

		if ( status == LOGMODIFIER_STATUS_MODIFYING ) {

			UplinkAssert (source);

			int pid = SvbLookupPID ( this );

			for ( int i = 0; i < n; ++i ) {

				// Check log is still there
				if ( !source->logs.ValidIndex (sourceindex) ) {
					status = LOGMODIFIER_STATUS_FINISHED;
					break;
				}

				// Check we are still connected
				if ( !game->GetWorld ()->GetPlayer ()->IsConnected () ) {
					SvbRemoveTask (pid);
					return;
				}

				AccessLog *log = source->logs.GetData (sourceindex);
				UplinkAssert (log);
				
				char sprogress [64];
				UplinkSnprintf ( sprogress, sizeof ( sprogress ), "logmodifier_progress %d", pid );

				// Modify in progress
				++progress;		
				UplinkAssert ( EclGetButton ( sprogress ) );
				EclGetButton ( sprogress )->width = (int) ( 120 * ( (float) progress / (float) numticksrequired ) );
				EclDirtyButton ( sprogress );

				if ( progress >= numticksrequired ) {

					// All done - modify the log

					char fromip	  [64];
					char data1	  [64];

					UplinkSnprintf ( fromip, sizeof ( fromip ),	"logmodifier_fromip %d",	pid );
					UplinkSnprintf ( data1, sizeof ( data1 ),	"logmodifier_data1 %d",		pid );

					Button *b_fromip = EclGetButton ( fromip );
					Button *b_data1  = EclGetButton ( data1 );

					UplinkAssert (b_fromip);
					UplinkAssert (b_data1);

					log->SetTYPE   ( logtype );

					char *tempIP = StripCarriageReturns ( b_fromip->caption );
					if ( strlen( tempIP ) >= SIZE_VLOCATION_IP )
						tempIP[ SIZE_VLOCATION_IP - 1 ] = '\0';
					log->SetFromIP ( tempIP );
					delete [] tempIP;

					char *tempData1 = StripCarriageReturns ( b_data1->caption );
					log->SetData1  ( tempData1 );
					delete [] tempData1;

					// This should be undetectable - so copy the result into internal logs

					if ( source->internallogs.ValidIndex (sourceindex) ) {
						source->internallogs.GetData ( sourceindex )->SetProperties ( log );

					}
					else {
						AccessLog *internalcopy = new AccessLog ();
						internalcopy->SetProperties ( log );
						source->internallogs.PutData ( internalcopy, sourceindex );
					}

					status = LOGMODIFIER_STATUS_FINISHED;

				}

			}

		}
		else if ( status == LOGMODIFIER_STATUS_CREATING ) {

			for ( int i = 0; i < n; ++i ) {

				// Check target area is still empty

				if ( source->logs.ValidIndex (sourceindex) ) {
					status = LOGMODIFIER_STATUS_FINISHED;
					break;
				}

				int pid = SvbLookupPID ( this );

				char sprogress [64];
				UplinkSnprintf ( sprogress, sizeof ( sprogress ), "logmodifier_progress %d", pid );

				// Modify in progress
				++progress;		
				UplinkAssert ( EclGetButton ( sprogress ) );
				EclGetButton ( sprogress )->width = (int) ( 120 * ( (float) progress / (float) numticksrequired ) );
				EclDirtyButton ( sprogress );

				if ( progress >= numticksrequired ) {

					// All done - create the new log

					AccessLog *newlog = new AccessLog ();
					newlog->SetProperties ( &(game->GetWorld ()->date), "New IP", "PLAYER" );
					newlog->SetData1 ( "Blank Action" );

					source->AddLog ( newlog, sourceindex );

					status = LOGMODIFIER_STATUS_FINISHED;

				}

			}

		}
		else if ( status == LOGMODIFIER_STATUS_FINISHED ) {

			RemoveExpandedInterface ();

			int pid = SvbLookupPID ( this );

	/*
			char sprogress [64];
			UplinkSnprintf ( sprogress, sizeof ( sprogress ), "logmodifier_progress %d", pid );
			EclRegisterCaptionChange ( sprogress, "Select target log" );

			source = NULL;
			sourceindex = -1;

			status = LOGMODIFIER_STATUS_WAITING;
	*/

			SvbRemoveTask ( pid );

		}

	}

}

	
void LogModifier::CreateInterface ()
{

	if ( !IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char stitle    [64];
		char sborder   [64];
		char sprogress [64];
		char sclose    [64];
		char tooltip   [64];

		UplinkSnprintf ( stitle, sizeof ( stitle ), "logmodifier_title %d", pid );
		UplinkSnprintf ( sborder, sizeof ( sborder ), "logmodifier_border %d", pid );
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "logmodifier_progress %d", pid );
		UplinkSnprintf ( sclose, sizeof ( sclose ), "logmodifier_close %d", pid );	
		UplinkSnprintf ( tooltip, sizeof ( tooltip ), "Log Modifier v%1.1f", version );

		EclRegisterButton ( 265, 450, 20, 15, "", tooltip, stitle );
		button_assignbitmap ( stitle, "software/log.tif" );

		EclRegisterButton ( 285, 450, 120, 15, "", "", sborder );
		EclRegisterButtonCallbacks ( sborder, BorderDraw, BorderClick, button_click, button_highlight );

		EclRegisterButton ( 285, 450, 120, 13, "Select target log", "", sprogress );
		EclRegisterButtonCallbacks ( sprogress, ProgressDraw, BorderClick, button_click, button_highlight );		

		EclRegisterButton ( 405, 450, 13, 13, "", "Close the LogModifier", sclose );		
		button_assignbitmaps ( sclose, "close.tif", "close_h.tif", "close_c.tif" );
		EclRegisterButtonCallback ( sclose, CloseClick );



		status = LOGMODIFIER_STATUS_WAITING;

	}

}

void LogModifier::RemoveInterface ()
{
	
	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char stitle    [64];
		char sborder   [64];
		char sprogress [64];
		char sclose    [64];

		UplinkSnprintf ( stitle, sizeof ( stitle ), "logmodifier_title %d", pid );
		UplinkSnprintf ( sborder, sizeof ( sborder ), "logmodifier_border %d", pid );
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "logmodifier_progress %d", pid );
		UplinkSnprintf ( sclose, sizeof ( sclose ), "logmodifier_close %d", pid );	

		EclRemoveButton ( stitle );
		EclRemoveButton ( sborder );
		EclRemoveButton ( sprogress );
		EclRemoveButton ( sclose );

		RemoveExpandedInterface ();

	}

}

void LogModifier::ShowInterface ()
{

	if ( !IsInterfaceVisible () ) CreateInterface ();

	int pid = SvbLookupPID ( this );

	char stitle    [64];
	char sborder   [64];
	char sprogress [64];
	char sclose    [64];

	UplinkSnprintf ( stitle, sizeof ( stitle ), "logmodifier_title %d", pid );
	UplinkSnprintf ( sborder, sizeof ( sborder ), "logmodifier_border %d", pid );
	UplinkSnprintf ( sprogress, sizeof ( sprogress ), "logmodifier_progress %d", pid );
	UplinkSnprintf ( sclose, sizeof ( sclose ), "logmodifier_close %d", pid );	

	EclButtonBringToFront ( stitle );
	EclButtonBringToFront ( sborder );
	EclButtonBringToFront ( sprogress );
	EclButtonBringToFront ( sclose );


	if ( IsExpandedInterfaceVisible () ) {

		char background [64];
		char t_ltype	[64];
		char ltype		[64];
		char t_fromip	[64];
		char fromip		[64];
		char t_data1	[64];
		char data1		[64];

		char nexttype   [64];
		char commit		[64];
		char close		[64];

		UplinkSnprintf ( background, sizeof ( background ), "logmodifier_background %d",	pid );
		UplinkSnprintf ( t_ltype, sizeof ( t_ltype ),	  "logmodifier_tltype %d",		pid );
		UplinkSnprintf ( ltype, sizeof ( ltype ),	  "logmodifier_ltype %d",		pid );
		UplinkSnprintf ( t_fromip, sizeof ( t_fromip ),	  "logmodifier_tfromip %d",		pid );
		UplinkSnprintf ( fromip, sizeof ( fromip ),  	  "logmodifier_fromip %d",		pid );
		UplinkSnprintf ( t_data1, sizeof ( t_data1 ),	  "logmodifier_tdata1 %d",		pid );
		UplinkSnprintf ( data1, sizeof ( data1 ),	  "logmodifier_data1 %d",		pid );
		
		UplinkSnprintf ( nexttype, sizeof ( nexttype ),	  "logmodifier_nexttype %d",	pid );
		UplinkSnprintf ( commit, sizeof ( commit ),	  "logmodifier_commit %d",		pid );
		UplinkSnprintf ( close, sizeof ( close ),	  "logmodifier_close2 %d",		pid );

		EclButtonBringToFront ( background );
		EclButtonBringToFront ( t_ltype );
		EclButtonBringToFront ( ltype );
		EclButtonBringToFront ( t_fromip );
		EclButtonBringToFront ( fromip );
		EclButtonBringToFront ( t_data1 );
		EclButtonBringToFront ( data1 );
		
		EclButtonBringToFront ( nexttype );
		EclButtonBringToFront ( commit );
		EclButtonBringToFront ( close );

	}

}

bool LogModifier::IsInterfaceVisible ()
{

	int pid = SvbLookupPID ( this );
	char stitle    [128];
	UplinkSnprintf ( stitle, sizeof ( stitle ), "logmodifier_title %d", pid );

	return ( EclGetButton ( stitle ) != NULL );

}


void LogModifier::CreateExpandedInterface ()
{

	if ( !IsExpandedInterfaceVisible () ) {

		if ( source && source->logs.ValidIndex (sourceindex) ) {

			int pid = SvbLookupPID ( this );

			// Find the top left

			char titlename [64];
			UplinkSnprintf ( titlename, sizeof ( titlename ), "logmodifier_title %d", pid );
			Button *btitle = EclGetButton ( titlename );
			UplinkAssert (btitle);

			// Create the text and buttons

			char background [64];
			char t_ltype	[64];
			char ltype		[64];
			char t_fromip	[64];
			char fromip		[64];
			char t_data1	[64];
			char data1		[64];
			char commit		[64];
			char close		[64];
			char nexttype	[64];

			UplinkSnprintf ( background, sizeof ( background ), "logmodifier_background %d",	pid );
			UplinkSnprintf ( t_ltype, sizeof ( t_ltype ),	  "logmodifier_tltype %d",		pid );
			UplinkSnprintf ( ltype, sizeof ( ltype ),	  "logmodifier_ltype %d",		pid );
			UplinkSnprintf ( t_fromip, sizeof ( t_fromip ),	  "logmodifier_tfromip %d",		pid );
			UplinkSnprintf ( fromip, sizeof ( fromip ),  	  "logmodifier_fromip %d",		pid );
			UplinkSnprintf ( t_data1, sizeof ( t_data1 ),	  "logmodifier_tdata1 %d",		pid );
			UplinkSnprintf ( data1, sizeof ( data1 ),	  "logmodifier_data1 %d",		pid );
			UplinkSnprintf ( commit, sizeof ( commit ),	  "logmodifier_commit %d",		pid );
			UplinkSnprintf ( close, sizeof ( close ),	  "logmodifier_close2 %d",		pid );
			UplinkSnprintf ( nexttype, sizeof ( nexttype ),	  "logmodifier_nexttype %d",	pid );
			
			EclRegisterButton ( btitle->x + 155, btitle->y, 180, 110, "", "", background );
			EclRegisterButtonCallbacks ( background, BackgroundDraw, NULL, NULL, NULL );

			EclRegisterButton ( btitle->x + 165, btitle->y + 10, 50, 15, "Type", "", t_ltype );
			EclRegisterButton ( btitle->x + 215, btitle->y + 10, 100, 15, "", "Enter new type here", ltype );
			EclRegisterButton ( btitle->x + 165, btitle->y + 35, 50, 15, "From", "", t_fromip );
			EclRegisterButton ( btitle->x + 215, btitle->y + 35, 100, 15, "", "Enter new data here", fromip );
			EclRegisterButton ( btitle->x + 165, btitle->y + 60, 50, 15, "", "", t_data1 );
			EclRegisterButton ( btitle->x + 215, btitle->y + 60, 100, 15, "", "Enter new data here", data1 );

			EclRegisterButtonCallbacks ( t_ltype, textbutton_draw, NULL, NULL, NULL );
			EclRegisterButtonCallbacks ( ltype, textbutton_draw, NULL, NULL, NULL );
			EclRegisterButtonCallbacks ( t_fromip, textbutton_draw, NULL, NULL, NULL );
			EclRegisterButtonCallbacks ( fromip, textbutton_draw, NULL, button_click, button_highlight );
			EclRegisterButtonCallbacks ( t_data1, textbutton_draw, NULL, NULL, NULL );
			EclRegisterButtonCallbacks ( data1, textbutton_draw, NULL, button_click, button_highlight );

			EclRegisterButton ( btitle->x + 215 + 100, btitle->y + 10, 15, 15, ">", "Change the type of this log", nexttype );
			EclRegisterButtonCallback ( nexttype, NextTypeClick );

			EclRegisterButton ( btitle->x + 165, btitle->y + 85, 50, 15, "Commit", "Click here to commit your changes", commit );
			EclRegisterButtonCallback ( commit, CommitClick );

			EclRegisterButton ( btitle->x + 220, btitle->y + 85, 50, 15, "Close", "Close the log modifier", close );
			EclRegisterButtonCallback ( close, CloseClick );

			EclMakeButtonEditable ( ltype );
			EclMakeButtonEditable ( fromip );
			EclMakeButtonEditable ( data1 );

			// Register those caption changes

			AccessLog *log = source->logs.GetData (sourceindex);
			UplinkAssert (log);		

			log->Print ();

			EclRegisterCaptionChange ( fromip, log->fromip );
			if ( log->data1 ) EclRegisterCaptionChange ( data1, log->data1 );

			logtype = log->TYPE;
			ChangeLogType ( pid );

			status = LOGMODIFIER_STATUS_ACTIVE;

		}
		else {

			status = LOGMODIFIER_STATUS_WAITING;

		}

	}

}

void LogModifier::RemoveExpandedInterface ()
{

	if ( IsExpandedInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char background [64];
		char t_ltype	[64];
		char ltype		[64];
		char t_fromip	[64];
		char fromip		[64];
		char t_data1	[64];
		char data1		[64];

		char nexttype   [64];
		char commit		[64];
		char close		[64];

		UplinkSnprintf ( background, sizeof ( background ), "logmodifier_background %d",	pid );
		UplinkSnprintf ( t_ltype, sizeof ( t_ltype ),	  "logmodifier_tltype %d",		pid );
		UplinkSnprintf ( ltype, sizeof ( ltype ),	  "logmodifier_ltype %d",		pid );
		UplinkSnprintf ( t_fromip, sizeof ( t_fromip ),	  "logmodifier_tfromip %d",		pid );
		UplinkSnprintf ( fromip, sizeof ( fromip ),  	  "logmodifier_fromip %d",		pid );
		UplinkSnprintf ( t_data1, sizeof ( t_data1 ),	  "logmodifier_tdata1 %d",		pid );
		UplinkSnprintf ( data1, sizeof ( data1 ),	  "logmodifier_data1 %d",		pid );
		
		UplinkSnprintf ( nexttype, sizeof ( nexttype ),	  "logmodifier_nexttype %d",	pid );
		UplinkSnprintf ( commit, sizeof ( commit ),	  "logmodifier_commit %d",		pid );
		UplinkSnprintf ( close, sizeof ( close ),	  "logmodifier_close2 %d",		pid );

		EclRemoveButton ( background );
		EclRemoveButton ( t_ltype );
		EclRemoveButton ( ltype );
		EclRemoveButton ( t_fromip );
		EclRemoveButton ( fromip );
		EclRemoveButton ( t_data1 );
		EclRemoveButton ( data1 );
		
		EclRemoveButton ( nexttype );
		EclRemoveButton ( commit );
		EclRemoveButton ( close );

	}

}

bool LogModifier::IsExpandedInterfaceVisible ()
{

	int pid = SvbLookupPID ( this );
	char fromip_title [64];
	UplinkSnprintf ( fromip_title, sizeof ( fromip_title ), "logmodifier_background %d", pid );

	return ( EclGetButton ( fromip_title ) != NULL );

}
	
