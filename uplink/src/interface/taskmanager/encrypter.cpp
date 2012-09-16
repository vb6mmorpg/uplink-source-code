
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
#include "interface/taskmanager/encrypter.h"

#include "world/world.h"
#include "world/player.h"
#include "world/computer/databank.h"

#include "mmgr.h"


Encrypter::Encrypter () : UplinkTask ()
{
	
	source = NULL;
	sourceindex = -1;
	status = ENCRYPTER_OFF;
	numticksrequired = 0;
	progress = 0;

}

Encrypter::~Encrypter ()
{
}

void Encrypter::MoveTo ( int x, int y, int time_ms )
{

	UplinkTask::MoveTo ( x, y, time_ms );

	int pid = SvbLookupPID ( this );

	char stitle    [128];
	char sborder   [128];
	char sprogress [128];
	char sclose    [128];

	UplinkSnprintf ( stitle, sizeof ( stitle ), "encrypter_title %d", pid );
	UplinkSnprintf ( sborder, sizeof ( sborder ), "encrypter_border %d", pid );
	UplinkSnprintf ( sprogress, sizeof ( sprogress ), "encrypter_progress %d", pid );
	UplinkSnprintf ( sclose, sizeof ( sclose ), "encrypter_close %d", pid );	

	EclRegisterMovement ( stitle, x, y, time_ms);
	EclRegisterMovement ( sborder, x + 20, y, time_ms );
	EclRegisterMovement ( sprogress, x + 20, y + 1, time_ms );
	EclRegisterMovement ( sclose, x + 140, y + 1, time_ms );

	EclButtonBringToFront ( stitle );
	EclButtonBringToFront ( sborder );
	EclButtonBringToFront ( sprogress );
	EclButtonBringToFront ( sclose );

}

void Encrypter::SetTarget ( UplinkObject *uo, char *uos, int uoi )
{

	/*

		uo  : DataBank containing data to be decrypted
		uos : Name of button representing data
		uoi : index of data

		*/

	if ( status == ENCRYPTER_OFF ) {

		if ( uo->GetOBJECTID () == OID_DATABANK ) {

			// Databank selected

			if ( ((DataBank *) uo)->GetData (uoi) ) {

				source = (DataBank *) uo;
				sourceindex = uoi;

				Data *data = source->GetData (sourceindex);
				UplinkAssert (data);

				numticksrequired = data->size * TICKSREQUIRED_DECRYPT;
				progress = 0;

                remotefile = strstr ( uos, "fileserverscreen" ) ? true : false;

				Button *button = EclGetButton ( uos );
				UplinkAssert (button);

				MoveTo ( button->x, button->y + button->height + 1, 1000 );

			}

		}

	}

}

void Encrypter::BorderDraw ( Button *button, bool highlighted, bool clicked )
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

void Encrypter::ProgressDraw ( Button *button, bool highlighted, bool clicked )
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

void Encrypter::CloseClick ( Button *button )
{

	int pid;
	char bname [128];
	sscanf ( button->name, "%s %d", bname, &pid );

	SvbRemoveTask ( pid );

}

void Encrypter::BorderClick ( Button *button )
{

	int pid;
	char bname [128];
	sscanf ( button->name, "%s %d", bname, &pid );

	game->GetInterface ()->GetTaskManager ()->SetTargetProgram ( pid );

}

void Encrypter::Initialise ()
{
}

static bool Encrypter_ReSetTargetProgram ( int pid )
{

	TaskManager *tm = game->GetInterface ()->GetTaskManager ();
	if ( tm->IsTargetProgramLast ( pid ) ) {

		char stitle    [128];
		char sborder   [128];
		char sprogress [128];
		char sclose    [128];

		UplinkSnprintf ( stitle, sizeof ( stitle ), "ecrypter_title %d", pid );
		UplinkSnprintf ( sborder, sizeof ( sborder ), "encrypter_border %d", pid );
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "encrypter_progress %d", pid );
		UplinkSnprintf ( sclose, sizeof ( sclose ), "encrypter_close %d", pid );	

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

void Encrypter::Tick ( int n )
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );
		char sprogress [128];
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "encrypter_progress %d", pid );

		if ( status == ENCRYPTER_OFF ) {

			// Not encrypting - look for a new target

			if ( source ) {

				// A new source file has been specified - start encrypting it
				status = ENCRYPTER_INPROGRESS;

				EclRegisterCaptionChange ( sprogress, "Encrypting...", 0 );

			}
			else {

				Encrypter_ReSetTargetProgram ( pid );

			}

		}
		else if ( status == ENCRYPTER_INPROGRESS ) {

			UplinkAssert (source);

            // Check we are still connected
            if ( remotefile && !game->GetWorld ()->GetPlayer ()->IsConnected () ) {
                SvbRemoveTask (pid);
                return;
            }


			Data *data = source->GetData (sourceindex);
			
			// The data was probably removed
			if ( !data ) {

				EclGetButton ( sprogress )->width = 120;
				EclDirtyButton ( sprogress );

				status = ENCRYPTER_FINISHED;
				EclRegisterCaptionChange ( sprogress, "Failed" );

				Encrypter_ReSetTargetProgram ( pid );

			}
			else {

				for ( int count = 0; count < n; ++count ) {

					// Encrypt in progress
					if ( version <= data->encrypted )		++progress;		
					else									progress += (int) (version - data->encrypted);

					UplinkAssert ( EclGetButton ( sprogress ) );
					EclGetButton ( sprogress )->width = (int) ( 120 * ( (float) progress / (float) numticksrequired ) );
					EclDirtyButton ( sprogress );

					if ( progress >= numticksrequired ) {

						// Finished decrypting now
						status = ENCRYPTER_FINISHED;

						
						if ( data->encrypted < version ) {
						
							EclRegisterCaptionChange ( sprogress, "Finished" );			
							data->encrypted = (int) version;

						}
						else {

							EclRegisterCaptionChange ( sprogress, "Failed" );

						}
						

						Encrypter_ReSetTargetProgram ( pid );

						break;

					}

				}

			}

		}
		else if ( status == ENCRYPTER_FINISHED ) {

			source = NULL;
			sourceindex = -1;
			progress = 0;	
			numticksrequired = 0;
			status = ENCRYPTER_OFF;

			Encrypter_ReSetTargetProgram ( pid );

		}

	}

}

void Encrypter::CreateInterface ()
{

	if ( !IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );
		
		char stitle    [128];
		char sborder   [128];
		char sprogress [128];
		char sclose    [128];

		UplinkSnprintf ( stitle, sizeof ( stitle ), "encrypter_title %d", pid );
		UplinkSnprintf ( sborder, sizeof ( sborder ), "encrypter_border %d", pid );
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "encrypter_progress %d", pid );
		UplinkSnprintf ( sclose, sizeof ( sclose ), "encrypter_close %d", pid );	

		EclRegisterButton ( 265, 450, 20, 15, "", "Encrypter", stitle );
		button_assignbitmap ( stitle, "software/enc.tif" );

		EclRegisterButton ( 285, 450, 120, 15, "", "", sborder );
		EclRegisterButtonCallbacks ( sborder, BorderDraw, BorderClick, button_click, button_highlight );

		EclRegisterButton ( 285, 450, 120, 13, "Select target", "Shows the progress of the encryption", sprogress );
		EclRegisterButtonCallbacks ( sprogress, ProgressDraw, BorderClick, button_click, button_highlight );		

		EclRegisterButton ( 405, 450, 13, 13, "", "Close the Encrypter (and stop encrypting)", sclose );		
		button_assignbitmaps ( sclose, "close.tif", "close_h.tif", "close_c.tif" );
		EclRegisterButtonCallback ( sclose, CloseClick );
		
	}

}

void Encrypter::RemoveInterface ()
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char stitle    [128];
		char sborder   [128];
		char sprogress [128];
		char sclose    [128];

		UplinkSnprintf ( stitle, sizeof ( stitle ), "encrypter_title %d", pid );
		UplinkSnprintf ( sborder, sizeof ( sborder ), "encrypter_border %d", pid );
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "encrypter_progress %d", pid );
		UplinkSnprintf ( sclose, sizeof ( sclose ), "encrypter_close %d", pid );	

		EclRemoveButton ( stitle );
		EclRemoveButton ( sborder );
		EclRemoveButton ( sprogress );
		EclRemoveButton ( sclose );

	}

}

void Encrypter::ShowInterface ()
{

	if ( !IsInterfaceVisible () ) CreateInterface ();

	int pid = SvbLookupPID ( this );

	char stitle    [128];
	char sborder   [128];
	char sprogress [128];
	char sclose    [128];

	UplinkSnprintf ( stitle, sizeof ( stitle ), "encrypter_title %d", pid );
	UplinkSnprintf ( sborder, sizeof ( sborder ), "encrypter_border %d", pid );
	UplinkSnprintf ( sprogress, sizeof ( sprogress ), "encrypter_progress %d", pid );
	UplinkSnprintf ( sclose, sizeof ( sclose ), "encrypter_close %d", pid );	

	EclButtonBringToFront ( stitle );
	EclButtonBringToFront ( sborder );
	EclButtonBringToFront ( sprogress );
	EclButtonBringToFront ( sclose );

}

bool Encrypter::IsInterfaceVisible ()
{

	int pid = SvbLookupPID ( this );

	char stitle [128];
	UplinkSnprintf ( stitle, sizeof ( stitle ), "encrypter_border %d", pid );
	
	return ( EclGetButton (stitle) != NULL );

}
