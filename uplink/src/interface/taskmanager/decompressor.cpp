
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
#include "interface/taskmanager/decompressor.h"

#include "world/world.h"
#include "world/player.h"
#include "world/computer/databank.h"


Decompressor::Decompressor () : UplinkTask ()
{
	
	source = NULL;
	sourceindex = -1;
	status = DECOMPRESSOR_OFF;
	numticksrequired = 0;
	progress = 0;

}

Decompressor::~Decompressor ()
{
}

void Decompressor::MoveTo ( int x, int y, int time_ms )
{

	UplinkTask::MoveTo ( x, y, time_ms );

	int pid = SvbLookupPID ( this );

	char stitle    [128];
	char sborder   [128];
	char sprogress [128];
	char sclose    [128];

	sprintf ( stitle, "compressor_title %d", pid );
	sprintf ( sborder, "compressor_border %d", pid );
	sprintf ( sprogress, "compressor_progress %d", pid );
	sprintf ( sclose, "compressor_close %d", pid );	

	EclRegisterMovement ( stitle, x, y, time_ms);
	EclRegisterMovement ( sborder, x + 20, y, time_ms );
	EclRegisterMovement ( sprogress, x + 20, y + 1, time_ms );
	EclRegisterMovement ( sclose, x + 140, y + 1, time_ms );

	EclButtonBringToFront ( stitle );
	EclButtonBringToFront ( sborder );
	EclButtonBringToFront ( sprogress );
	EclButtonBringToFront ( sclose );

}

void Decompressor::SetTarget ( UplinkObject *uo, char *uos, int uoi )
{

	/*

		uo  : DataBank containing data to be decrypted
		uos : Name of button representing data
		uoi : index of data

		*/

	if ( status == DECOMPRESSOR_OFF ) {

		if ( uo->GetOBJECTID () == OID_DATABANK ) {

			// Databank selected

			if ( ((DataBank *) uo)->GetData (uoi) ) {

				source = (DataBank *) uo;
				sourceindex = uoi;

				Data *data = source->GetData (sourceindex);
				UplinkAssert (data);

				numticksrequired = data->size * TICKSREQUIRED_DECOMPRESS * (data->compressed +1);
				progress = 0;

                remotefile = strstr ( uos, "fileserverscreen" ) ? true : false;

				Button *button = EclGetButton ( uos );
				UplinkAssert (button);

				MoveTo ( button->x, button->y + button->height + 1, 1000 );

			}

		}

	}

}

void Decompressor::BorderDraw ( Button *button, bool highlighted, bool clicked )
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

void Decompressor::ProgressDraw ( Button *button, bool highlighted, bool clicked )
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
		
	glColor4f ( 1.0, 1.0, 1.0, 1.0 );    
    GciDrawText ( xpos, ypos, button->caption, HELVETICA_10 );

}

void Decompressor::CloseClick ( Button *button )
{

	int pid;
	char bname [128];
	sscanf ( button->name, "%s %d", bname, &pid );

	SvbRemoveTask ( pid );

}

void Decompressor::BorderClick ( Button *button )
{

	int pid;
	char bname [128];
	sscanf ( button->name, "%s %d", bname, &pid );

	game->GetInterface ()->GetTaskManager ()->SetTargetProgram ( pid );

}

void Decompressor::Initialise ()
{
}

static bool Decompressor_ReSetTargetProgram ( int pid )
{

	TaskManager *tm = game->GetInterface ()->GetTaskManager ();
	if ( tm->IsTargetProgramLast ( pid ) ) {

		char stitle    [128];
		char sborder   [128];
		char sprogress [128];
		char sclose    [128];

		UplinkSnprintf ( stitle, sizeof ( stitle ), "decompressor_title %d", pid );
		UplinkSnprintf ( sborder, sizeof ( sborder ), "decompressor_border %d", pid );
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "decompressor_progress %d", pid );
		UplinkSnprintf ( sclose, sizeof ( sclose ), "decompressor_close %d", pid );	

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

void Decompressor::Tick ( int n )
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );
		char sprogress [128];
		sprintf ( sprogress, "compressor_progress %d", pid );

		if ( status == DECOMPRESSOR_OFF ) {

			// Not decrypting - look for a new target

			if ( source ) {

				// A new source file has been specified - start decrypting it
				status = DECOMPRESSOR_INPROGRESS;

				EclRegisterCaptionChange ( sprogress, "Decompressing...", 0 );

			} else {

				Decompressor_ReSetTargetProgram ( pid );

			}

		}
		else if ( status == DECOMPRESSOR_INPROGRESS ) {

			UplinkAssert (source);

            // Check we are still connected
            if ( remotefile && !game->GetWorld ()->GetPlayer ()->IsConnected () ) {
                SvbRemoveTask (pid);
                return;
            }


			Data *data = source->GetData (sourceindex);
			UplinkAssert (data);

			for ( int count = 0; count < n; ++count ) {

				// Decrypt in progress
				++progress;		

				UplinkAssert ( EclGetButton ( sprogress ) );
				EclGetButton ( sprogress )->width = (int) (120 * ( (float) progress / (float) numticksrequired ));
				EclDirtyButton ( sprogress );

				if ( progress >= numticksrequired ) {

					// Finished decrypting now
					status = DECOMPRESSOR_FINISHED;

					if ( data->compressed <= version ) {
						Data *newdata = new Data(data);
						Data *olddata = new Data(data);

						newdata->SetDetails(data->TYPE,data->size +data->compressed, data->encrypted, 0,
							data->version,data->softwareTYPE);

						source->RemoveData(sourceindex);
						if ( source->FindValidPlacement(newdata) == -1 )
						{
							EclRegisterCaptionChange ( sprogress, "Insufficient Space" );
							source->PutData(olddata);
						} else {
							EclRegisterCaptionChange ( sprogress, "Finished" );
							source->PutData(newdata);
						}

					}
					else {

						EclRegisterCaptionChange ( sprogress, "Failed" );

					}
					Decompressor_ReSetTargetProgram ( pid );
					break;

				}

			}

		}
		else if ( status == DECOMPRESSOR_FINISHED ) {

			//source = NULL;
			//sourceindex = -1;
			//progress = 0;	
			//numticksrequired = 0;
			//status = DECOMPRESSOR_OFF;

			source = NULL;
			sourceindex = -1;
			status = DECOMPRESSOR_OFF;
			numticksrequired = 0;
			progress = 0;
			remotefile = true;

			UplinkAssert ( EclGetButton ( sprogress ) );
			EclGetButton ( sprogress )->width = 120;
			EclRegisterCaptionChange ( sprogress, "Select target" );
			EclDirtyButton ( sprogress );

			Decompressor_ReSetTargetProgram ( pid );
		}

	}

}

void Decompressor::CreateInterface ()
{

	if ( !IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );
		
		char stitle    [128];
		char sborder   [128];
		char sprogress [128];
		char sclose    [128];

		sprintf ( stitle, "compressor_title %d", pid );
		sprintf ( sborder, "compressor_border %d", pid );
		sprintf ( sprogress, "compressor_progress %d", pid );
		sprintf ( sclose, "compressor_close %d", pid );	

		EclRegisterButton ( 265, 450, 20, 15, "", "Compressor", stitle );
		button_assignbitmap ( stitle, "software/dec.tif" );

		EclRegisterButton ( 285, 450, 120, 15, "", "", sborder );
		EclRegisterButtonCallbacks ( sborder, BorderDraw, BorderClick, button_click, button_highlight );

		EclRegisterButton ( 285, 450, 120, 13, "Select target", "Shows the progress of the decompression", sprogress );
		EclRegisterButtonCallbacks ( sprogress, ProgressDraw, BorderClick, button_click, button_highlight );		

		EclRegisterButton ( 405, 450, 13, 13, "", "Close the Decompressor (and stop decompressing)", sclose );		
		button_assignbitmaps ( sclose, "close.tif", "close_h.tif", "close_c.tif" );
		EclRegisterButtonCallback ( sclose, CloseClick );
		
	}

}

void Decompressor::RemoveInterface ()
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char stitle    [128];
		char sborder   [128];
		char sprogress [128];
		char sclose    [128];

		sprintf ( stitle, "compressor_title %d", pid );
		sprintf ( sborder, "compressor_border %d", pid );
		sprintf ( sprogress, "compressor_progress %d", pid );
		sprintf ( sclose, "compressor_close %d", pid );	

		EclRemoveButton ( stitle );
		EclRemoveButton ( sborder );
		EclRemoveButton ( sprogress );
		EclRemoveButton ( sclose );

	}

}

void Decompressor::ShowInterface ()
{

	if ( !IsInterfaceVisible () ) CreateInterface ();

	int pid = SvbLookupPID ( this );

	char stitle    [128];
	char sborder   [128];
	char sprogress [128];
	char sclose    [128];

	sprintf ( stitle, "compressor_title %d", pid );
	sprintf ( sborder, "compressor_border %d", pid );
	sprintf ( sprogress, "compressor_progress %d", pid );
	sprintf ( sclose, "compressor_close %d", pid );	

	EclButtonBringToFront ( stitle );
	EclButtonBringToFront ( sborder );
	EclButtonBringToFront ( sprogress );
	EclButtonBringToFront ( sclose );

}

bool Decompressor::IsInterfaceVisible ()
{

	int pid = SvbLookupPID ( this );

	char stitle [128];
	sprintf ( stitle, "compressor_border %d", pid );
	
	return ( EclGetButton (stitle) != NULL );

}

