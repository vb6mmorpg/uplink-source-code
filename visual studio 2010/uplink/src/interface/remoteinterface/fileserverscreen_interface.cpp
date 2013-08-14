
#ifdef WIN32
#include <windows.h>
#endif

#include <string.h>

#include <GL/gl.h>

#include <GL/glu.h>

#include "eclipse.h"
#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"

#include "game/game.h"

#include "interface/interface.h"
#include "interface/scrollbox.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/fileserverscreen_interface.h"
#include "interface/taskmanager/taskmanager.h"

#include "world/world.h"
#include "world/message.h"
#include "world/player.h"
#include "world/computer/computer.h"
#include "world/computer/logbank.h"
#include "world/computer/computerscreen/genericscreen.h"

#include "mmgr.h"


int FileServerScreenInterface::baseoffset = 0;
int FileServerScreenInterface::previousnumfiles = 0;


void FileServerScreenInterface::CloseClick ( Button *button )
{

	UplinkAssert ( button );

	int nextpage;
	char ip [ SIZE_VLOCATION_IP ] = {0};
	sscanf ( button->name, "fileserverscreen_click %d %s", &nextpage, ip );

	Computer *comp = NULL;
	if ( ip ) {
		VLocation *loc = game->GetWorld ()->GetVLocation ( ip );
		if ( loc )
			comp = loc->GetComputer ();
	}
	if ( nextpage != -1 ) game->GetInterface ()->GetRemoteInterface ()->RunScreen ( nextpage, comp );

}

bool FileServerScreenInterface::EscapeKeyPressed ()
{

	char name [128 + SIZE_VLOCATION_IP + 1];
	UplinkSnprintf ( name, sizeof ( name ), "fileserverscreen_click %d %s", GetComputerScreen ()->nextpage, GetComputerScreen ()->GetComputer ()->ip );
    Button *button = EclGetButton (name);
    
	if ( button )
		CloseClick (button);
    return true;

}

void FileServerScreenInterface::FileClick ( Button *button )
{

	UplinkAssert ( button );

	int fileindex;
	sscanf ( button->name, "fileserverscreen_file %d", &fileindex );
	fileindex += baseoffset;

	Computer *comp = game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ()->GetComputer ();
	UplinkAssert ( comp );

	if ( comp->security.IsRunning_Firewall () ) {
		create_msgbox ( "Error", "Denied access by Firewall" );
		return;
	}

	/*
	Data *data = comp->databank.GetDataFile (fileindex);
	int memoryindex = comp->databank.GetMemoryIndex ( fileindex );
	*/

	Data *data;
	int sizeData;
	int memoryindex;
	int nbRowsDisplayDataBank = GetInfoRowDisplayDataBank ( &comp->databank, fileindex, &data, &sizeData, &memoryindex );

	if ( data && memoryindex != -1 ) {

		// Log this access
		char action [64];

		UplinkSnprintf ( action, sizeof ( action ), "Accessed file %s", data->title );
		AccessLog *log = new AccessLog ();
		log->SetProperties ( &(game->GetWorld ()->date), 
							 game->GetWorld ()->GetPlayer ()->GetConnection ()->GetGhost (), "PLAYER",
							 LOG_NOTSUSPICIOUS, LOG_TYPE_TEXT );
		log->SetData1 ( action );
		
		comp->logbank.AddLog ( log );

		// Target this databank
		game->GetInterface ()->GetTaskManager ()->SetProgramTarget ( &(comp->databank), button->name, memoryindex );

	}
	/*
	else {
	*/
	else if ( memoryindex != -1 ) {

		/*
		// There is nothing here, so target an empty spot in memory
		game->GetInterface ()->GetTaskManager ()->SetProgramTarget ( &(comp->databank), button->name, -1 );
		*/
		game->GetInterface ()->GetTaskManager ()->SetProgramTarget ( &(comp->databank), button->name, memoryindex );

		// Log this access
		char action [64];

		UplinkSnprintf ( action, sizeof ( action ), "Accessed memory file index %d", fileindex );
		AccessLog *log = new AccessLog ();
		log->SetProperties ( &(game->GetWorld ()->date), 
							 game->GetWorld ()->GetPlayer ()->GetConnection ()->GetGhost (), "PLAYER",
							 LOG_NOTSUSPICIOUS, LOG_TYPE_TEXT );
		log->SetData1 ( action );
		
		comp->logbank.AddLog ( log );

	}

}

/*
void FileServerScreenInterface::ScrollUpClick ( Button *button ) 
{

	--baseoffset;
	if ( baseoffset < 0 ) baseoffset = 0;

	for ( int i = 0; i < 15; ++i ) {

		char name [128];
		UplinkSnprintf ( name, sizeof ( name ), "fileserverscreen_file %d", i );
		EclDirtyButton ( name );

	}

}

void FileServerScreenInterface::ScrollDownClick ( Button *button )
{

	++baseoffset;

	for ( int i = 0; i < 15; ++i ) {

		char name [128];
		UplinkSnprintf ( name, sizeof ( name ), "fileserverscreen_file %d", i );
		EclDirtyButton ( name );

	}

}
*/

int FileServerScreenInterface::GetNbRowsDisplayDataBank ( DataBank *db )
{

	int fileindex = 0;
	Data *data;
	int size;
	int memoryindex;

	return GetInfoRowDisplayDataBank ( db, fileindex, &data, &size, &memoryindex );

}

int FileServerScreenInterface::GetInfoRowDisplayDataBank ( DataBank *db, int fileindex, Data **data, int *size, int *memoryindex )
{

	*data = NULL;
	*size = 0;
	*memoryindex = -1;

	int targetRow = fileindex + 1;

	int dataSize = db->GetDataSize ();
	int memorySize =  db->GetSize ();
	int nbRows = 0;

	if ( dataSize > 0 && memorySize > 0 ) {
		int lastIndexMemory = -1;
		Data *lastData = NULL;

		for ( int indexMemory = 0; indexMemory < memorySize; indexMemory++ ) {
			int indexData = db->GetDataIndex ( indexMemory );
			Data *curData = db->GetDataFile ( indexData );

			if ( curData && curData != lastData ) {
				if ( lastIndexMemory == -1 ) {
					if ( indexMemory > 0 ) {
						//An empty space at the beginning
						nbRows++;
						if ( nbRows == targetRow ) {
							*size = indexMemory;
							*memoryindex = 0;
						}
					}
				}
				else if ( indexMemory > lastIndexMemory ) {
					//There is some space between this data and the previous one
					nbRows++;
					if ( nbRows == targetRow ) {
						*size = indexMemory - lastIndexMemory;
						*memoryindex = lastIndexMemory;
					}
				}

				nbRows++;
				if ( nbRows == targetRow ) {
					*data = curData;
					*size = curData->size;
					*memoryindex = indexMemory;
				}

				lastIndexMemory = indexMemory + curData->size;
				lastData = curData;
			}
		}

		if ( lastIndexMemory != -1 && memorySize > lastIndexMemory ) {
			//An empty space at the end
			nbRows++;
			if ( nbRows == targetRow ) {
				*size = memorySize - lastIndexMemory;
				*memoryindex = lastIndexMemory;
			}
		}
	}
	
	if ( nbRows == 0 && memorySize > 0 ) {
		nbRows++;
		if ( nbRows == targetRow ) {
			*size = memorySize;
			*memoryindex = 0;
		}
	}

	return nbRows;

}

void FileServerScreenInterface::ScrollChange ( char *scrollname, int newValue )
{

    baseoffset = newValue;

	for ( int i = 0; i < 15; ++i ) {

		char name [128];
		UplinkSnprintf ( name, sizeof ( name ), "fileserverscreen_file %d", i );
		EclDirtyButton ( name );

	}

}

void FileServerScreenInterface::FileDraw ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert ( button );

	int fileindex;
	sscanf ( button->name, "fileserverscreen_file %d", &fileindex );
	fileindex += baseoffset;

	Computer *comp = game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ()->GetComputer ();
	UplinkAssert ( comp );

	/*
	Data *data = comp->databank.GetDataFile (fileindex);
	*/

	Data *data;
	int sizeData;
	int memoryindex;
	int nbRowsDisplayDataBank = GetInfoRowDisplayDataBank ( &comp->databank, fileindex, &data, &sizeData, &memoryindex );

	if ( data ) {

		if ( fileindex % 2 == 0 ) {

			glBegin ( GL_QUADS );
				glColor3ub ( 8, 20, 80 );		glVertex2i ( button->x, button->y );
				glColor3ub ( 8, 20, 0 );		glVertex2i ( button->x + button->width, button->y );
				glColor3ub ( 8, 20, 80 );		glVertex2i ( button->x + button->width, button->y + button->height );
				glColor3ub ( 8, 20, 0 );		glVertex2i ( button->x, button->y + button->height );
			glEnd ();

		}
		else {

			glBegin ( GL_QUADS );
				glColor3ub ( 8, 20, 0 );		glVertex2i ( button->x, button->y );
				glColor3ub ( 8, 20, 80 );		glVertex2i ( button->x + button->width, button->y );
				glColor3ub ( 8, 20, 0 );		glVertex2i ( button->x + button->width, button->y + button->height );
				glColor3ub ( 8, 20, 80 );		glVertex2i ( button->x, button->y + button->height );
			glEnd ();

		}

		glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );

		GciDrawText ( button->x, button->y + 10, data->title );

		char size [64];
		UplinkSnprintf ( size, sizeof ( size ), "%d GigaQuads", data->size );
		GciDrawText ( button->x + 150, button->y + 10, size );

		if ( data->encrypted > 0 ) {
			char encrypttext [32];
			UplinkSnprintf ( encrypttext, sizeof ( encrypttext ), "Level %d", data->encrypted );
			GciDrawText ( button->x + 250, button->y + 10, encrypttext );
		}

		if ( data->compressed > 0 ) {
			char compressedtext [32];
			UplinkSnprintf ( compressedtext, sizeof ( compressedtext ), "Level %d", data->compressed );
			GciDrawText ( button->x + 330, button->y + 10, compressedtext );
		}

	}
	else if ( memoryindex != -1 && sizeData > 0 ) {

		clear_draw ( button->x, button->y, button->width, button->height );

		glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );

		GciDrawText ( button->x, button->y + 10, "Free space" );

		char size [64];
		UplinkSnprintf ( size, sizeof ( size ), "%d GigaQuads", sizeData );
		GciDrawText ( button->x + 150, button->y + 10, size );

	}
	else {

		clear_draw ( button->x, button->y, button->width, button->height );

	}

	// Draw a bounding box

	/*
	if ( highlighted && fileindex <= comp->databank.GetSize () ) {
	*/
	if ( highlighted && fileindex < nbRowsDisplayDataBank ) {

		glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );
		border_draw ( button );

	}
	
}

void FileServerScreenInterface::Create ()
{

	if ( cs ) Create ( cs );
	else printf ( "FileServerScreenInterface::Create, tried to create when GenericScreen==NULL\n" );

}

void FileServerScreenInterface::Create ( ComputerScreen *newcs )
{

	UplinkAssert ( newcs );
	cs = newcs;

	if ( !IsVisible () ) {

		DataBank *db = &(GetComputerScreen ()->GetComputer ()->databank);

		AccessLog *log = new AccessLog ();
		log->SetProperties ( &(game->GetWorld ()->date), 
							 game->GetWorld ()->GetPlayer ()->GetConnection ()->GetGhost (), "PLAYER" );
		log->SetData1 ( "Accessed fileserver" );
		GetComputerScreen ()->GetComputer ()->logbank.AddLog ( log );

		EclRegisterButton ( 80, 60, 350, 25, GetComputerScreen ()->maintitle, "", "fileserverscreen_maintitle" );
		EclRegisterButtonCallbacks ( "fileserverscreen_maintitle", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButton ( 80, 80, 350, 20, GetComputerScreen ()->subtitle, "", "fileserverscreen_subtitle" );
		EclRegisterButtonCallbacks ( "fileserverscreen_subtitle", DrawSubTitle, NULL, NULL, NULL );

		EclRegisterButton ( 15, 120, 144, 15, "Filename", "", "fileserverscreen_filenametitle" );
		EclRegisterButton ( 162, 120, 96, 15, "Size", "", "fileserverscreen_sizetitle" );
		EclRegisterButton ( 261, 120, 76, 15, "Encryption", "", "fileserverscreen_encryption" );
		EclRegisterButton ( 340, 120, 75, 15, "Compression", "", "fileserverscreen_compression" );


		// Create the file entries

		for ( int i = 0; i < 15; ++i ) {

			char name [128];
			UplinkSnprintf ( name, sizeof ( name ), "fileserverscreen_file %d", i );
			EclRegisterButton ( 15, 140 + i * 15, 400, 14, "", "Select this file", name );
			EclRegisterButtonCallbacks ( name, FileDraw, FileClick, button_click, button_highlight );

		}

		/*
		if ( db->NumDataFiles () >= 14 ) {
		*/

		int nbRowsDisplayDataBank = GetNbRowsDisplayDataBank ( db );

		if ( nbRowsDisplayDataBank >= 15 ) {

			// Create the scroll bar

			CreateScrollBar ( nbRowsDisplayDataBank );

			/*
			EclRegisterButton ( 420, 140, 15, 15, "^", "Scroll up", "fileserverscreen_scrollup" );
			button_assignbitmaps ( "fileserverscreen_scrollup", "up.tif", "up_h.tif", "up_c.tif" );
			EclRegisterButtonCallback ( "fileserverscreen_scrollup", ScrollUpClick );

			EclRegisterButton ( 420, 157, 15, 13 * 15 - 4, "", "fileserverscreen_scrollbar" );

			EclRegisterButton ( 420, 140 + 14 * 15, 15, 15, "v", "Scroll down", "fileserverscreen_scrolldown" );
			button_assignbitmaps ( "fileserverscreen_scrolldown", "down.tif", "down_h.tif", "down_c.tif" );
			EclRegisterButtonCallback ( "fileserverscreen_scrolldown", ScrollDownClick );
			*/

		}

		// Create the close button

		char name [128 + SIZE_VLOCATION_IP + 1];
		UplinkSnprintf ( name, sizeof ( name ), "fileserverscreen_click %d %s", GetComputerScreen ()->nextpage, GetComputerScreen ()->GetComputer ()->ip );
		EclRegisterButton ( 421, 121, 13, 13, "", "Close the File Server Screen", name );
		button_assignbitmaps ( name, "close.tif", "close_h.tif", "close_c.tif" );
		EclRegisterButtonCallback ( name, CloseClick );
		

        // Move to the top

        baseoffset = 0;

	}

}

void FileServerScreenInterface::CreateScrollBar ( int nbItems )
{

	ScrollBox::CreateScrollBox( "fileserverscreen_scroll", 420, 140, 15, 
	                            15 * 15, nbItems, 15, 0, ScrollChange );

}


void FileServerScreenInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "fileserverscreen_maintitle" );
		EclRemoveButton ( "fileserverscreen_subtitle" );

		EclRemoveButton ( "fileserverscreen_filenametitle" );
		EclRemoveButton ( "fileserverscreen_sizetitle" );
		EclRemoveButton ( "fileserverscreen_encryption" );
		EclRemoveButton ( "fileserverscreen_compression" );

		for ( int i = 0; i < 15; ++i ) {

			char name [128];
			UplinkSnprintf ( name, sizeof ( name ), "fileserverscreen_file %d", i );
			EclRemoveButton ( name );

		}

		/*
		EclRemoveButton ( "fileserverscreen_scrollup" );
		EclRemoveButton ( "fileserverscreen_scrollbar" );
		EclRemoveButton ( "fileserverscreen_scrolldown" );
		*/
        if ( ScrollBox::GetScrollBox( "fileserverscreen_scroll" ) != NULL )
            ScrollBox::RemoveScrollBox( "fileserverscreen_scroll" );

		char name [128 + SIZE_VLOCATION_IP + 1];
		UplinkSnprintf ( name, sizeof ( name ), "fileserverscreen_click %d %s", GetComputerScreen ()->nextpage, GetComputerScreen ()->GetComputer ()->ip );
		EclRemoveButton ( name );

	}

}

bool FileServerScreenInterface::IsVisible ()
{

	return ( EclGetButton ( "fileserverscreen_maintitle" ) != NULL );

}

void FileServerScreenInterface::Update ()
{

	Computer *comp = game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ()->GetComputer ();
	UplinkAssert ( comp );

	/*
	int newnumfiles = comp->databank.NumDataFiles ();
	*/

	int newnumfiles = GetNbRowsDisplayDataBank ( &comp->databank );

	if ( newnumfiles != previousnumfiles ) {

		for ( int i = 0; i < 15; ++i ) {

			char name [128];
			UplinkSnprintf ( name, sizeof ( name ), "fileserverscreen_file %d", i );
			EclDirtyButton ( name );

		}
		
		previousnumfiles = newnumfiles;

		//
		// Update the scrollbar
		//

		ScrollBox *sb = ScrollBox::GetScrollBox("fileserverscreen_scroll");

		if ( newnumfiles >= 15 ) {
			if ( !sb ) {
				baseoffset = 0;
				CreateScrollBar ( newnumfiles );
			}
			else
				sb->SetNumItems( newnumfiles );
		}
		else {
			baseoffset = 0;
			if ( sb )
				ScrollBox::RemoveScrollBox( "fileserverscreen_scroll" );
		}

	}

}

GenericScreen *FileServerScreenInterface::GetComputerScreen ()
{

	UplinkAssert (cs);
	return (GenericScreen *) cs;

}

int FileServerScreenInterface::ScreenID ()
{

	return SCREEN_FILESERVERSCREEN;

}
