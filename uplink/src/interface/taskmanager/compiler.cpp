
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h> /*_glu_extention_library_*/


#include "vanbakel.h"
#include "eclipse.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"
#include "app/miscutils.h"

#include "game/game.h"
#include "game/data/data.h"

#include "interface/taskmanager/compiler.h"

#include "world/world.h"
#include "world/vlocation.h"
#include "world/player.h"

#include "mmgr.h"

void Compiler::DisplayDraw ( Button *button, bool highlighted, bool clicked )
{

	if ( button->width == 370 )
	{
		// Draw a grey background

		glBegin ( GL_QUADS );
			glColor4f ( 0.6f, 0.6f, 0.6f, ALPHA );		glVertex2i ( button->x, button->y );
			glColor4f ( 0.4f, 0.4f, 0.4f, ALPHA );		glVertex2i ( button->x + button->width, button->y );
			glColor4f ( 0.6f, 0.6f, 0.6f, ALPHA );		glVertex2i ( button->x + button->width, button->y + button->height );
			glColor4f ( 0.4f, 0.4f, 0.4f, ALPHA );		glVertex2i ( button->x, button->y + button->height );
		glEnd ();
	}
	else
	{
		float scale = (float) button->width / 370.0f;
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
	}
	// Draw ordinary text

	//text_draw ( button, highlighted, clicked );
	int xpos = button->x + 5;
	int ypos = (button->y + button->height / 2) + 3;
		
	glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );    
    GciDrawText ( xpos, ypos, button->caption, HELVETICA_10 );

	// Draw a border

	if ( highlighted || clicked ) border_draw ( button );

}

void Compiler::CloseClick ( Button *button )
{

	int pid;
	sscanf ( button->name, "compiler_close %d", &pid );

	SvbRemoveTask ( pid );

}

void Compiler::GoClick ( Button *button )
{

	int pid;
	sscanf ( button->name, "compiler_go %d", &pid );

	Compiler *task = (Compiler *) SvbGetTask ( pid );

	if ( task->status == COMPILER_IDLE )
	{
		char name_display [64];
		UplinkSnprintf ( name_display, sizeof ( name_display ), "compiler_display %d", pid );

		if ( EclGetButton(name_display) )
		{
			char title[64];
			UplinkSafeStrcpy(title, EclGetButton(name_display)->caption);
			//task->SetFilename( EclGetButton(name_display)->caption );
			task->SetFilename(title);
			task->status = COMPILER_COMPILING_SPEC;
		}
	}
	else
		task->status = COMPILER_IDLE;

}

Compiler::Compiler () : UplinkTask ()
{

	status = COMPILER_IDLE;
	dataversion = 0;
	datasize = 0;
	progress = 100;
	numticksrequired = 100;
	datatype = SOFTWARETYPE_NONE;

}

Compiler::~Compiler ()
{
}

void Compiler::Initialise ()
{
}

void Compiler::SetFilename ( char *newfilename )
{
	//if ( filename ) { delete [] filename; }
	//filename = new char[64];
	UplinkSnprintf(filename, sizeof(filename), "%s", newfilename);
}

void Compiler::Tick ( int n )
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );
		char name_display [64];
		UplinkSnprintf ( name_display, sizeof ( name_display ), "compiler_display %d", pid );
				
		Button *button = EclGetButton ( name_display );
		UplinkAssert ( button );

		for ( int count = 0; count < n; ++count ) {

			if ( progress >= numticksrequired ) {
				progress = numticksrequired;
				break;
			}
			++progress;		
			button->width = (int)(370 * ( (float) progress / (float) numticksrequired ));
			EclDirtyButton ( button->name );


		}
		if ( status == COMPILER_IDLE ) {

			// Do nothing

		}
		else if ( status == COMPILER_COMPILING_SPEC ) {
			if ( progress >= numticksrequired )
			{
				//char *filename = StripCarriageReturns (button->caption);

				char specname[SIZE_DATA_TITLE];
				UplinkSnprintf( specname, sizeof( specname ), "%sSpec", filename);
				
				char caption[128];

				Data *data = game->GetWorld ()->GetPlayer ()->gateway.databank.GetData(specname);
				if ( data )
				{
					if ( data->encrypted )
					{
						status = COMPILER_FAILED;

						UplinkSnprintf( caption, sizeof( caption ), "Error: %s is encrypted", specname);
						EclRegisterCaptionChange ( name_display, caption );

						timeout = time(NULL) + 5;
						progress = numticksrequired = 100;
					}
					else if ( data->compressed )
					{
						status = COMPILER_FAILED;

						UplinkSnprintf( caption, sizeof( caption ), "Error: %s is compressed", specname);
						EclRegisterCaptionChange ( name_display, caption );

						timeout = time(NULL) + 5;
						progress = numticksrequired = 100;
					}
					else
					{
						UplinkSnprintf( caption, sizeof( caption ), "%s : Compiling %s...", filename, specname);
						EclRegisterCaptionChange ( name_display, caption );

						timeout = time(NULL) + (int)( (float)data->size * 5.0f / version );
						status = COMPILER_COMPILING_CORE;
						datasize += data->size;
						progress = 0;
						numticksrequired = (int)(TICKSREQUIRED_COMPILE * ((float) data->size / version));
						datatype = data->softwareTYPE;
					}

				}
				else
				{
					status = COMPILER_FAILED;

					UplinkSnprintf( caption, sizeof( caption ), "Cannot find %s", specname);
					EclRegisterCaptionChange ( name_display, caption );

					timeout = time(NULL) + 5;
					progress = numticksrequired = 100;
				}
			}
		}
		else if ( status == COMPILER_COMPILING_CORE ) {
			if ( progress >= numticksrequired )
			{
				//char *filename = StripCarriageReturns (button->caption);
				
				//char realfilename[SIZE_DATA_TITLE];
				//sscanf(filename, "%s", realfilename);

				char corename[SIZE_DATA_TITLE];
				UplinkSnprintf( corename, sizeof( corename ), "%sCore", filename);
				
				char caption[128];

				Data *data = game->GetWorld ()->GetPlayer ()->gateway.databank.GetData(corename);
				if ( !data )
				{
					// Alternative CoreName for Revelation
					UplinkSnprintf( corename, sizeof( corename ), "%sCoreV0.7", filename);
					data = game->GetWorld ()->GetPlayer ()->gateway.databank.GetData(corename);
				}
				if ( data )
				{
					if ( data->encrypted )
					{
						status = COMPILER_FAILED;

						UplinkSnprintf( caption, sizeof( caption ), "Error: %s is encrypted", corename);
						EclRegisterCaptionChange ( name_display, caption );

						timeout = time(NULL) + 5;
						progress = numticksrequired = 100;
					}
					else if ( data->compressed )
					{
						status = COMPILER_FAILED;

						UplinkSnprintf( caption, sizeof( caption ), "Error: %s is compressed", corename);
						EclRegisterCaptionChange ( name_display, caption );

						timeout = time(NULL) + 5;
						progress = numticksrequired = 100;
					}
					else
					{
						UplinkSnprintf( caption, sizeof( caption ), "%s : Compiling %s...", filename, corename);
						EclRegisterCaptionChange ( name_display, caption );

						timeout = time(NULL) + (int)( (float)data->size * 5.0f / version );
						status = COMPILER_COMPILING_DATA;
						datasize += data->size;
						progress = 0;
						numticksrequired = (int)(TICKSREQUIRED_COMPILE * ((float) data->size / version));
					}
				}
				else
				{
					status = COMPILER_FAILED;

					UplinkSnprintf( caption, sizeof( caption ), "Cannot find %s", corename);
					EclRegisterCaptionChange ( name_display, caption );

					timeout = time(NULL) + 5;
					progress = numticksrequired = 100;
				}
			}
		}
		else if ( status == COMPILER_COMPILING_DATA ) {
			if ( progress >= numticksrequired )
			{
				//char *filename = StripCarriageReturns (button->caption);
				
				//char realfilename[SIZE_DATA_TITLE];
				//sscanf(filename, "%s", realfilename);

				char dataname[SIZE_DATA_TITLE];
				UplinkSnprintf( dataname, sizeof( dataname ), "%sData-%d", filename, dataversion);
				
				char caption[128];

				Data *data = game->GetWorld ()->GetPlayer ()->gateway.databank.GetData(dataname);
				if ( data )
				{
					if ( data->encrypted )
					{
						status = COMPILER_FAILED;

						UplinkSnprintf( caption, sizeof( caption ), "Error: %s is encrypted", dataname);
						EclRegisterCaptionChange ( name_display, caption );

						timeout = time(NULL) + 5;
						progress = numticksrequired = 100;
					}
					else if ( data->compressed )
					{
						status = COMPILER_FAILED;

						UplinkSnprintf( caption, sizeof( caption ), "Error: %s is compressed", dataname);
						EclRegisterCaptionChange ( name_display, caption );

						timeout = time(NULL) + 5;
						progress = numticksrequired = 100;
					}
					else
					{
						UplinkSnprintf( caption, sizeof( caption ), "%s : Compiling %s...", filename, dataname);
						EclRegisterCaptionChange ( name_display, caption );

						timeout = time(NULL) + (int)( (float)data->size * 5.0f / version );
						status = COMPILER_COMPILING_DATA;
						datasize += data->size;
						dataversion++;
						progress = 0;
						numticksrequired = (int)(TICKSREQUIRED_COMPILE * ((float) data->size / version));
					}
				}
				else if ( dataversion > 0 )
				{
					status = COMPILER_LINKING;

					UplinkSnprintf( caption, sizeof( caption ), "Linking %s", filename);
					EclRegisterCaptionChange ( name_display, caption );

					timeout = time(NULL) + (int)( ((float) datasize + (float)dataversion) * 5.0f / version );
					progress = 0;
					numticksrequired = (int)(TICKSREQUIRED_COMPILE_LINK * ((float) datasize / version));
				}
				else
				{
					status = COMPILER_FAILED;

					UplinkSnprintf( caption, sizeof( caption ), "Cannot find %s", dataname);
					EclRegisterCaptionChange ( name_display, caption );

					timeout = time(NULL) + 5;
					progress = numticksrequired = 100;
				}
			}
		}
		else if ( status == COMPILER_LINKING ) {

			// Revert to idle after 5 secs

			if ( progress >= numticksrequired )
			{
				//char *filename = StripCarriageReturns (button->caption);
				
				//char realfilename[SIZE_DATA_TITLE];
				//sscanf(filename, "Linking %s", realfilename);

				datasize = datasize - (int) version;
				if ( datasize < dataversion ) datasize = dataversion;

				Data *data = new Data();
				data->SetTitle(filename);
				data->SetDetails(DATATYPE_PROGRAM, datasize/5, 0, 0, (float) dataversion, datatype);
				game->GetWorld ()->GetPlayer ()->gateway.databank.PutData(data);

				timeout = time(NULL) + 5;
				status = COMPILER_FINISHED;
				progress = numticksrequired = 100;
				
				EclRegisterCaptionChange(button->name,"Finished");
			}

		}
		else if ( status == COMPILER_FINISHED || status == COMPILER_FAILED ) {

			// Revert to idle after 5 secs

			if ( time (NULL) > timeout ) {

				EclRegisterCaptionChange ( name_display, "Enter Program Name" );
				status = COMPILER_IDLE;
				progress = numticksrequired = 100;
				datasize = 0;
				dataversion = 0;

				
			}

		}
		else {

			UplinkAbort ( "Unkown Status" );

		}
	}

}

void Compiler::CreateInterface ()
{
	
	if ( !IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char name_go [64];
		char name_display [64];
		char name_close [64];

		UplinkSnprintf ( name_go, sizeof ( name_go ), "compiler_go %d", pid );
		UplinkSnprintf ( name_display, sizeof ( name_display ), "compiler_display %d", pid );
		UplinkSnprintf ( name_close, sizeof ( name_close ), "compiler_close %d", pid );

		EclRegisterButton ( 63, 444, 20, 15, "", "Compile the program", name_go );
		EclRegisterButton ( 83, 444, 370, 14, "Enter Program Name", "Type your program name here", name_display );
		EclRegisterButton ( 453, 445, 13, 13, "", "Close the compiler program", name_close );

		button_assignbitmap ( name_go, "software/go.tif" );
		EclRegisterButtonCallback ( name_go, GoClick );
		EclRegisterButtonCallbacks ( name_display, DisplayDraw, NULL, button_click, button_highlight );
		button_assignbitmaps ( name_close, "close.tif", "close_h.tif", "close_c.tif" );
		EclRegisterButtonCallback ( name_close, CloseClick );

		EclMakeButtonEditable ( name_display );

	}

}

void Compiler::RemoveInterface ()
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char name_go [64];
		char name_display [64];
		char name_close [64];

		UplinkSnprintf ( name_go, sizeof ( name_go ), "compiler_go %d", pid );
		UplinkSnprintf ( name_display, sizeof ( name_display ), "compiler_display %d", pid );
		UplinkSnprintf ( name_close, sizeof ( name_close ), "compiler_close %d", pid );

		EclRemoveButton ( name_go );
		EclRemoveButton ( name_display );
		EclRemoveButton ( name_close );

	}

}

void Compiler::ShowInterface ()
{

	if ( !IsInterfaceVisible () ) CreateInterface ();

	int pid = SvbLookupPID ( this );

	char name_go [64];
	char name_display [64];
	char name_close [64];

	UplinkSnprintf ( name_go, sizeof ( name_go ), "compiler_go %d", pid );
	UplinkSnprintf ( name_display, sizeof ( name_display ), "compiler_display %d", pid );
	UplinkSnprintf ( name_close, sizeof ( name_close ), "compiler_close %d", pid );

	EclButtonBringToFront ( name_go );
	EclButtonBringToFront ( name_display );
	EclButtonBringToFront ( name_close );


}

bool Compiler::IsInterfaceVisible ()
{

	int pid = SvbLookupPID ( this );
	char name_display [64];
	UplinkSnprintf ( name_display, sizeof ( name_display ), "compiler_display %d", pid );

	return ( EclGetButton ( name_display ) != 0 );

}
