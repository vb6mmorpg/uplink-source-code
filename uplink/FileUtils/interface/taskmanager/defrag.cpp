

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

#include "interface/taskmanager/defrag.h"
#include "interface/interface.h"
#include "interface/localinterface/localinterface.h"
#include "interface/localinterface/localinterfacescreen.h"
#include "interface/localinterface/memory_interface.h"

#include "world/world.h"
#include "world/vlocation.h"
#include "world/player.h"
#include "world/computer/computer.h"
#include "world/computer/securitysystem.h"

#include "mmgr.h"


void Defrag::BorderDraw ( Button *button, bool highlighted, bool clicked )
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

void Defrag::ProgressDraw ( Button *button, bool highlighted, bool clicked )
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

void Defrag::GoClick ( Button *button )
{

	int pid;
	char bname [64];
	sscanf ( button->name, "%s %d", bname, &pid );

	Defrag *thistask = (Defrag *) SvbGetTask ( pid );
	UplinkAssert (thistask);

	// Set it going

	thistask->progress = 1;

	int totalmemory = game->GetWorld ()->GetPlayer ()->gateway.databank.GetSize ();
	thistask->ticksrequired = TICKSREQUIRED_DEFRAG * totalmemory;
	thistask->ticksdone = 0;
	thistask->currentMemorySlot = -1;
	thistask->firstFreeMemorySlot = -1;
	thistask->nbDataDatabank = game->GetWorld ()->GetPlayer ()->gateway.databank.NumDataFiles ();
  	
}

void Defrag::CloseClick ( Button *button )
{

	int pid;
	char bname [64];
	sscanf ( button->name, "%s %d", bname, &pid );

	Defrag *thistask = (Defrag *) SvbGetTask ( pid );
	UplinkAssert (thistask);

	thistask->StopDefrag ();

	//
	// Now we can shut down

	SvbRemoveTask ( pid );

}

void Defrag::StopDefrag ( )
{

	//
	// Should dump any files in transit to memory now
	// Or they will be lost

	for ( int i = 0; i < transitData.Size(); ++i ) {
	
		DataBank *databank = &(game->GetWorld ()->GetPlayer ()->gateway.databank);
		UplinkAssert (databank);

		Data *data = transitData.GetData (i);
		UplinkAssert (data);

		databank->PutData ( data );

	}
	
	transitData.Empty ();

	/********** Start code by François Gagné **********/
	//
	// If the player is looking at his memory banks right now
	// Then we have to tell them to update

	int currentScreen = game->GetInterface ()->GetLocalInterface ()->InScreen ();
	if ( currentScreen == SCREEN_MEMORY ) {

		LocalInterfaceScreen *lis = game->GetInterface ()->GetLocalInterface ()->GetInterfaceScreen ();   
		UplinkAssert (lis);
		MemoryInterface *mi = (MemoryInterface *) lis;
		mi->SpecialHighlight ( -1 ); // No highlight
		mi->ForceUpdateAll ();

	}
	/********** End code by François Gagné **********/

}

Defrag::Defrag () : UplinkTask ()
{

	progress = 0;
	nbDataDatabank = 0;

}

Defrag::~Defrag ()
{
}

void Defrag::Initialise ()
{
}

void Defrag::HandleNewMemorySlot ( int index )
{

	DataBank *databank = &(game->GetWorld ()->GetPlayer ()->gateway.databank);
	UplinkAssert (databank);
	Data *file = databank->GetData( index );

	//
	// Should we pick up the file in this memory slot?
	// Pick up all files when they are first encountered

	if ( file ) {

		Data *theFileCopy = new Data ( file );
		databank->RemoveDataFile ( databank->GetDataIndex (index) );
		transitData.PutDataAtEnd( theFileCopy );

	}

	//
	// Should we update the start of blank space?
	// 

	file = databank->GetData( index );

	if ( !file ) {

		if ( firstFreeMemorySlot == -1 )
			firstFreeMemorySlot = index;

	}

	//
	// Should we dump any of our stored files here?
	// Dump files from transitData if there is enough room

	int freeSpace = 1 + (index - firstFreeMemorySlot);
	if ( transitData.Size () > 0 ) {

		Data *theTransitData = transitData.GetData(0);
		UplinkAssert (theTransitData);
		if ( theTransitData->size <= freeSpace ) {

			databank->PutData( theTransitData, firstFreeMemorySlot );
			transitData.RemoveData ( 0 );
			
			if ( !databank->GetData( firstFreeMemorySlot + theTransitData->size ) )
				firstFreeMemorySlot = firstFreeMemorySlot + theTransitData->size;

			else
				firstFreeMemorySlot = -1;

		}

	}

    //
    // If the player is looking at his memory banks right now
    // Then we have to tell them to update

    int currentScreen = game->GetInterface ()->GetLocalInterface ()->InScreen ();
    if ( currentScreen == SCREEN_MEMORY ) {

        LocalInterfaceScreen *lis = game->GetInterface ()->GetLocalInterface ()->GetInterfaceScreen ();    
        UplinkAssert (lis);
        MemoryInterface *mi = (MemoryInterface *) lis;
        mi->SpecialHighlight ( index );
        mi->ForceUpdateAll ();

    }

}

void Defrag::Tick ( int n )
{

	if ( IsInterfaceVisible () ) {
		
		char sprogress [128];
		
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "defrag_progress %d", SvbLookupPID ( this ) );

		if ( progress == 0 ) {												// Do nothing - waiting for user to click GO			

		}
		else if ( progress == 1 ) {											// We are defragging

			if ( nbDataDatabank == game->GetWorld ()->GetPlayer ()->gateway.databank.NumDataFiles () ) {

				++ticksdone;

				float currentProgress = (float) ticksdone / (float) ticksrequired;
				EclGetButton ( sprogress )->width = (int) ( 120 * currentProgress );
				EclDirtyButton ( sprogress );

				int totalmemory = game->GetWorld ()->GetPlayer ()->gateway.databank.GetSize ();
				int newMemorySlot = (int)(totalmemory * currentProgress);

				if ( newMemorySlot > currentMemorySlot ) {
					for ( int i = currentMemorySlot + 1; i <= newMemorySlot; ++i )
						HandleNewMemorySlot ( i );
					currentMemorySlot = newMemorySlot;
				}

				nbDataDatabank = game->GetWorld ()->GetPlayer ()->gateway.databank.NumDataFiles ();
							
				if ( ticksdone >= ticksrequired ) {

					progress = 4;
					EclRegisterCaptionChange ( sprogress, "Finished" );
					timersync = time(NULL) + 2;

				}

			}
			else {

				StopDefrag ();

				EclGetButton ( sprogress )->width = 120;
				EclDirtyButton ( sprogress );

				progress = 4;
				EclRegisterCaptionChange ( sprogress, "Interrupted" );
				timersync = time(NULL) + 2;

			}

		}
		else if ( progress == 2 ) {											// We are done

			if ( time(NULL) > timersync ) {

				SvbRemoveTask ( SvbLookupPID ( this ) );

			}

		}


	}

}
	
void Defrag::CreateInterface ()
{

	if ( !IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char stitle    [128];
		char sborder   [128];
		char sprogress [128];
		char sclose    [128];

		UplinkSnprintf ( stitle, sizeof ( stitle ), "defrag_title %d", pid );
		UplinkSnprintf ( sborder, sizeof ( sborder ), "defrag_border %d", pid );
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "defrag_progress %d", pid );
		UplinkSnprintf ( sclose, sizeof ( sclose ), "defrag_close %d", pid );	

		EclRegisterButton ( 265, 422, 20, 15, "", "Defrag the files on your computer", stitle );
		button_assignbitmap ( stitle, "software/go.tif" );
		EclRegisterButtonCallback ( stitle, GoClick );

		EclRegisterButton ( 285, 422, 120, 15, "", "", sborder );
		EclRegisterButtonCallbacks ( sborder, BorderDraw, NULL, NULL, NULL );

		EclRegisterButton ( 285, 423, 0, 13, "Defrag", "Shows the progress of the defrag", sprogress );
		EclRegisterButtonCallbacks ( sprogress, ProgressDraw, NULL, NULL, NULL );		

		EclRegisterButton ( 405, 423, 13, 13, "", "Close the Defragger", sclose );		
		button_assignbitmaps ( sclose, "close.tif", "close_h.tif", "close_c.tif" );
		EclRegisterButtonCallback ( sclose, CloseClick );
		
	}

}

void Defrag::RemoveInterface ()
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char stitle    [128];
		char sborder   [128];
		char sprogress [128];
		char sclose    [128];

		UplinkSnprintf ( stitle, sizeof ( stitle ), "defrag_title %d", pid );
		UplinkSnprintf ( sborder, sizeof ( sborder ), "defrag_border %d", pid );
		UplinkSnprintf ( sprogress, sizeof ( sprogress ), "defrag_progress %d", pid );
		UplinkSnprintf ( sclose, sizeof ( sclose ), "defrag_close %d", pid );			

		EclRemoveButton ( stitle );
		EclRemoveButton ( sborder );
		EclRemoveButton ( sprogress );
		EclRemoveButton ( sclose );

	}

}

void Defrag::ShowInterface ()
{

	if ( !IsInterfaceVisible () ) CreateInterface ();

	int pid = SvbLookupPID ( this );

	char stitle    [128];
	char sborder   [128];
	char sprogress [128];
	char sclose    [128];

	UplinkSnprintf ( stitle, sizeof ( stitle ), "defrag_title %d", pid );
	UplinkSnprintf ( sborder, sizeof ( sborder ), "defrag_border %d", pid );
	UplinkSnprintf ( sprogress, sizeof ( sprogress ), "defrag_progress %d", pid );
	UplinkSnprintf ( sclose, sizeof ( sclose ), "defrag_close %d", pid );			

	EclButtonBringToFront ( stitle );
	EclButtonBringToFront ( sborder );
	EclButtonBringToFront ( sprogress );
	EclButtonBringToFront ( sclose );

}

bool Defrag::IsInterfaceVisible ()
{

	int pid = SvbLookupPID ( this );
	char stitle [128];
	UplinkSnprintf ( stitle, sizeof ( stitle ), "defrag_title %d", pid );

	return ( EclGetButton ( stitle ) != NULL );

}
