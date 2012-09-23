
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h> /*_glu_extention_library_*/

#include "eclipse.h"
#include "vanbakel.h"
#include "soundgarden.h"
#include "redshirt.h"

#include "app/opengl_interface.h"
#include "app/globals.h"
#include "app/serialise.h"

#include "game/game.h"

#include "world/world.h"
#include "world/player.h"
#include "world/computer/databank.h"
#include "world/generator/numbergenerator.h"
#include "world/generator/spamgenerator.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/taskmanager/taskmanager.h"
#include "interface/taskmanager/virus.h"


Virus::Virus () : UplinkTask ()
{
	virustype = 0;
	downloading = VIRUS_IDLE;
	currentMemorySlot = 0;
	progress = numticksrequired = 1;
	remotefile = false;
	source = NULL;
	sourceindex = -1;
}

Virus::Virus( int newvirustype )
{
	virustype = 0;
	downloading = VIRUS_IDLE;
	currentMemorySlot = 0;
	progress = numticksrequired = 1;
	remotefile = false;

	virustype = newvirustype;
	source = NULL;
	sourceindex = -1;
}

void Virus::Initialise ()
{
}

static bool Virus_ReSetTargetProgram ( int pid )
{

	TaskManager *tm = game->GetInterface ()->GetTaskManager ();
	if ( tm->IsTargetProgramLast ( pid ) ) {

		char stitle    [128];
		char sborder   [128];
		char sprogress [128];
		char sclose    [128];

		UplinkSnprintf ( stitle, sizeof ( stitle ), "virus_title %d", pid );
		UplinkSnprintf ( sborder, sizeof ( sborder ), "virus_border %d", pid );
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "virus_progress %d", pid );
		UplinkSnprintf ( sclose, sizeof ( sclose ), "virus_close %d", pid );	

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

void Virus::Tick ( int n )
{
	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );
		char sprogress [128];
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "virus_progress %d", pid );

		if ( downloading == VIRUS_IDLE ) {

			// Not downloading - look for a new target

			if ( source && (virustype == VIRUSTYPE_CRYPT || virustype == VIRUSTYPE_CORRUPT) ) {

				// A new source file has been specified - start downloading it
				downloading = VIRUS_WORKING;

				EclRegisterCaptionChange ( sprogress, "Working...", 0 );

			} else if ( source && (virustype == VIRUSTYPE_FORMAT || virustype == VIRUSTYPE_PORN) ) {

				downloading = VIRUS_SCANNING;
				currentMemorySlot = 0;

				if ( virustype == VIRUSTYPE_PORN )
				{
					Data *viewer = new Data();
					viewer->SetTitle("Porn_Viewer");
					viewer->SetDetails(DATATYPE_PROGRAM,2,0,0,version,SOFTWARETYPE_NONE);

					source->PutData(viewer);
				}
				EclRegisterCaptionChange ( sprogress, "Working...", 0 );
			}

			else {

				Virus_ReSetTargetProgram ( pid );

			}

		}
		else if ( downloading == VIRUS_WORKING ) {

            // Check we are still connected
            if ( remotefile && !game->GetWorld ()->GetPlayer ()->IsConnected () ) {
                SvbRemoveTask (pid);
                return;
            }

			// Download in progress
			Data *data = source->GetData (sourceindex);	
            if ( !data ) {
                SvbRemoveTask(pid);
                return;
            }
            			
			for ( int count = 0; count < n; ++count ) {

				++progress;
				UplinkAssert ( EclGetButton ( sprogress ) );
				EclGetButton ( sprogress )->width = (int)(120 * ( (float) progress / (float) numticksrequired ));
				EclDirtyButton ( sprogress );

				if ( progress >= numticksrequired ) {

					if ( virustype == VIRUSTYPE_CRYPT )
					{
						CryptEffect(data);
					}
					else if ( virustype == VIRUSTYPE_CORRUPT )
					{
						CorruptEffect(data);
					}

					// Finished downloading now
					downloading = VIRUS_IDLE;
					source = NULL;
					EclRegisterCaptionChange ( sprogress, "Select source" );

					Virus_ReSetTargetProgram ( pid );

					break;

				}

			}

		}
		else if ( downloading == VIRUS_SCANNING ) {

            // Check we are still connected
            if ( remotefile && !game->GetWorld ()->GetPlayer ()->IsConnected () ) {
                SvbRemoveTask (pid);
                return;
            }

			for ( int count = 0; count < n; ++count ) {

				++progress;		
				UplinkAssert ( EclGetButton ( sprogress ) );
				EclGetButton ( sprogress )->width = (int)(120 * ( (float) progress / (float) numticksrequired ));
				EclDirtyButton ( sprogress );

				if ( virustype == VIRUSTYPE_FORMAT )
				{
					FormatEffect(source);
				}
				else if ( virustype == VIRUSTYPE_PORN )
				{
					PornEffect(source);
				}

				if ( progress >= numticksrequired ) {

					// Finished downloading now
					downloading = VIRUS_IDLE;
					source = NULL;
					EclRegisterCaptionChange ( sprogress, "Select source" );
					Virus_ReSetTargetProgram ( pid );

					break;

				}

			}
		}
	}
}


void Virus::CreateInterface ()
{
	if ( !IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char stitle    [128];
		char sborder   [128];
		char sprogress [128];
		char sclose    [128];

		UplinkSnprintf ( stitle, sizeof ( stitle ), "virus_title %d", pid );
		UplinkSnprintf ( sborder, sizeof ( sborder ), "virus_border %d", pid );
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "virus_progress %d", pid );
		UplinkSnprintf ( sclose, sizeof ( sclose ), "virus_close %d", pid );	

		switch ( virustype )
		{
		case VIRUSTYPE_PORN:
			EclRegisterButton ( 265, 432, 20, 15, "", "Pr0n H4x3r", stitle ); break;
		case VIRUSTYPE_FORMAT:
			EclRegisterButton ( 265, 432, 20, 15, "", "D34d H4x3r", stitle ); break;
		case VIRUSTYPE_CRYPT:
			EclRegisterButton ( 265, 432, 20, 15, "", "r3x4H", stitle ); break;
		case VIRUSTYPE_CORRUPT:
			EclRegisterButton ( 265, 432, 20, 15, "", "&^#*%..!", stitle ); break;
		default:
			EclRegisterButton ( 265, 432, 20, 15, "", "H4x3r", stitle );
		}
		button_assignbitmap ( stitle, "software/cpy.tif" );

		EclRegisterButton ( 285, 432, 120, 15, "", "", sborder );
		EclRegisterButtonCallbacks ( sborder, BorderDraw, BorderClick, button_click, button_highlight );

		EclRegisterButton ( 285, 433, 120, 13, "Select source", "Shows the progress of the operation", sprogress );
		EclRegisterButtonCallbacks ( sprogress, ProgressDraw, BorderClick, button_click, button_highlight );		

		EclRegisterButton ( 405, 433, 13, 13, "", "Close the Program", sclose );		
		button_assignbitmaps ( sclose, "close.tif", "close_h.tif", "close_c.tif" );
		EclRegisterButtonCallback ( sclose, CloseClick );
				
	}
}

void Virus::RemoveInterface ()
{
	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char stitle    [128];
		char sborder   [128];
		char sprogress [128];
		char sclose    [128];

		UplinkSnprintf ( stitle, sizeof ( stitle ), "virus_title %d", pid );
		UplinkSnprintf ( sborder, sizeof ( sborder ), "virus_border %d", pid );
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "virus_progress %d", pid );
		UplinkSnprintf ( sclose, sizeof ( sclose ), "virus_close %d", pid );	

		EclRemoveButton ( stitle );
		EclRemoveButton ( sborder );
		EclRemoveButton ( sprogress );
		EclRemoveButton ( sclose );

	}
}

void Virus::ShowInterface ()
{

	if ( !IsInterfaceVisible () ) CreateInterface ();

	int pid = SvbLookupPID ( this );

	char stitle    [128];
	char sborder   [128];
	char sprogress [128];
	char sclose    [128];

	UplinkSnprintf ( stitle, sizeof ( stitle ), "virus_title %d", pid );
	UplinkSnprintf ( sborder, sizeof ( sborder ), "virus_border %d", pid );
	UplinkSnprintf ( sprogress, sizeof ( sprogress ), "virus_progress %d", pid );
	UplinkSnprintf ( sclose, sizeof ( sclose ), "virus_close %d", pid );	

	EclButtonBringToFront ( stitle );
	EclButtonBringToFront ( sborder );
	EclButtonBringToFront ( sprogress );
	EclButtonBringToFront ( sclose );
}

bool Virus::IsInterfaceVisible ()
{
	int pid = SvbLookupPID ( this );

	char stitle [128];
	UplinkSnprintf ( stitle, sizeof ( stitle ), "virus_border %d", pid );
	
	return ( EclGetButton (stitle) != NULL );
}

void Virus::BorderDraw ( Button *button, bool highlighted, bool clicked )
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

void Virus::ProgressDraw ( Button *button, bool highlighted, bool clicked )
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

void Virus::CloseClick ( Button *button )
{

	int pid;
	char bname [64];
	sscanf ( button->name, "%s %d", bname, &pid );

	SvbRemoveTask ( pid );

}

void Virus::BorderClick ( Button *button )
{

	int pid;
	char bname [64];
	sscanf ( button->name, "%s %d", bname, &pid );

	game->GetInterface ()->GetTaskManager ()->SetTargetProgram ( pid );

}

void Virus::FormatEffect ( DataBank *databank )
{
	int nbDataDatabank = databank->NumDataFiles ();
	if ( nbDataDatabank ) {

		float currentProgress = (float) progress / (float) numticksrequired;
		//EclGetButton ( sprogress )->width = (int) ( 120 * currentProgress );
		//EclDirtyButton ( sprogress );

		int totalmemory = databank->GetSize ();
		int newMemorySlot = (int)(totalmemory * currentProgress);

		if ( newMemorySlot > currentMemorySlot ) {
			for ( int i = currentMemorySlot + 1; i <= newMemorySlot; ++i )
				HandleNewMemorySlot ( databank, i, false, true );
			currentMemorySlot = newMemorySlot;
		}
	}
}

void Virus::PornEffect ( DataBank *databank )
{
	int nbDataDatabank = databank->NumDataFiles ();
	if ( nbDataDatabank ) {

		float currentProgress = (float) progress / (float) numticksrequired;
		//EclGetButton ( sprogress )->width = (int) ( 120 * currentProgress );
		//EclDirtyButton ( sprogress );

		int totalmemory = databank->GetSize ();
		int newMemorySlot = (int)(totalmemory * currentProgress);

		if ( newMemorySlot > currentMemorySlot ) {
			for ( int i = currentMemorySlot + 1; i <= newMemorySlot; ++i )
				HandleNewMemorySlot ( databank, i, true, false );
			currentMemorySlot = newMemorySlot;
		}
	}

}

void Virus::CryptEffect ( Data *data )
{
	if ( data )
	{
		if ( data->TYPE == DATATYPE_DATA )
			data->encrypted = NumberGenerator::RandomNumber((int)version)+1;
	}
}

void Virus::CorruptEffect ( Data *data )
{
	if ( data )
	{
		if ( data->TYPE == DATATYPE_PROGRAM )
		{
			int effect = NumberGenerator::RandomNumber(5);
			switch (effect)
			{
			case 0:
				data->TYPE = DATATYPE_DATA;
				break;
			case 1:
				data->softwareTYPE = SOFTWARETYPE_NONE;
				break;
			case 2:
				if ( data->version > 1.0f )
					data->version = 1.0f;
				else
					data->TYPE = DATATYPE_DATA;
				break;
			default:
				int dt = data->softwareTYPE;
				data->softwareTYPE = NumberGenerator::RandomNumber(5)+1;
				if ( dt == data->softwareTYPE )
					data->softwareTYPE = SOFTWARETYPE_OTHER;
			}
		}
	}
}

void Virus::HandleNewMemorySlot ( DataBank *databank, int index, bool fill, bool format )
{

	Data *file = databank->GetData( index );

	if ( file ) {

		if ( strcmp(file->title, "PornData") != 0 )
		{
			if ( format )
			{
				databank->RemoveDataFile ( databank->GetDataIndex (index) );
			}
			if ( fill )
			{
				int comp = NumberGenerator::RandomNumber(5);
				if ( comp < 0 ) comp = 0;
				Data *data = new Data();
				data->SetTitle("PornData");
				data->SetDetails(DATATYPE_DATA, 1, NumberGenerator::RandomNumber(8), comp);
				databank->PutData(data);
			}
		}
	}
}

void Virus::SetTarget ( UplinkObject *uo, char *uos, int uoi )
{

	if ( downloading == VIRUS_IDLE ) {

		if ( uo->GetOBJECTID () == OID_DATABANK ) {

			/*
				DataBank selected

				uo  : DataBank object containing Data to be downloaded
				uos : Name of button representing downloadable data
				uoi : memory index of data in databank

				*/

			if ( ((DataBank *) uo)->GetData (uoi) ) {

				source = (DataBank *) uo;
				sourceindex = uoi;

				Data *data = source->GetData (sourceindex);
				UplinkAssert (data);

				numticksrequired = (int)(TICKSREQUIRED_DECRYPT * ((float) data->size / (float) game->GetWorld ()->GetPlayer ()->gateway.GetBandwidth ()));
				progress = 0;

                remotefile = strstr ( uos, "fileserverscreen" ) ? true : false;

				Button *button = EclGetButton ( uos );
				UplinkAssert (button);

				MoveTo ( button->x, button->y, 1000 );

			}

		}

	}
}

void Virus::MoveTo ( int x, int y, int time_ms )
{

	UplinkTask::MoveTo ( x, y, time_ms );

	int pid = SvbLookupPID ( this );

	char stitle    [128];
	char sborder   [128];
	char sprogress [128];
	char sclose    [128];

	UplinkSnprintf ( stitle, sizeof ( stitle ), "virus_title %d", pid );
	UplinkSnprintf ( sborder, sizeof ( sborder ), "virus_border %d", pid );
	UplinkSnprintf ( sprogress, sizeof ( sprogress ), "virus_progress %d", pid );
	UplinkSnprintf ( sclose, sizeof ( sclose ), "virus_close %d", pid );	
		
	EclRegisterMovement ( stitle, x, y, time_ms);
	EclRegisterMovement ( sborder, x + 20, y, time_ms );
	EclRegisterMovement ( sprogress, x + 20, y + 1, time_ms );
	EclRegisterMovement ( sclose, x + 140, y + 1, time_ms );

	EclButtonBringToFront ( stitle );
	EclButtonBringToFront ( sborder );
	EclButtonBringToFront ( sprogress );
	EclButtonBringToFront ( sclose );

}