
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h>

#include <string.h>

#include "soundgarden.h"
#include "soundgarden.h"
#include "redshirt.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/miscutils.h"
#include "app/opengl_interface.h"

#include "options/options.h"

#include "game/game.h"

#include "world/world.h"
#include "world/player.h"

#include "interface/interface.h"
#include "interface/localinterface/sw_interface.h"
#include "interface/taskmanager/taskmanager.h"

#include "mmgr.h"


int SWInterface::currentsubmenu = SOFTWARETYPE_NONE;
int SWInterface::currentprogrambutton = -1;
int SWInterface::timesync = -1;


void SWInterface::SoftwareClick ( Button *button ) 
{

	UplinkAssert (button);

	char softwarename [128];
	float version;
	sscanf ( button->caption, "%s v%f", softwarename, &version );

	// Check this is still in memory

	bool inmemory = false;
	DataBank *db = &(game->GetWorld ()->GetPlayer ()->gateway.databank);
	int numfiles = db->GetDataSize ();

	for ( int i = 0; i < numfiles; ++i ) {

		if ( db->GetDataFile (i) &&
			strcmp ( db->GetDataFile (i)->title, softwarename ) == 0 &&
			db->GetDataFile (i)->version == version ) {
			inmemory = true;
			break;
		}

	}

	// Run it if it is

    if ( inmemory ) {
		game->GetInterface ()->GetTaskManager ()->RunSoftware ( softwarename, version );
        SgPlaySound ( RsArchiveFileOpen ("sounds/runsoftware.wav"), "sounds/runsoftware.wav", false );
    }

	// Remove the software menu

	ToggleSoftwareMenu ();

}

void SWInterface::SoftwareDraw ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert (  button );

	// Highlight this button anyway if it is the currentprogrambutton
	
	char junk [32];
	bool nhighlighted;

	int areyoutheone;
	sscanf ( button->name, "%s %d", junk, &areyoutheone );
	
	if ( areyoutheone == currentprogrambutton &&
		 strstr ( button->name, "version" ) == NULL )							// Don't auto-highlight buttons from a version menu
		nhighlighted = true;

	else
		nhighlighted = highlighted;

	// Draw the button

	glBegin ( GL_QUADS );

		if		( clicked )		SetColour ( "ButtonClickedA" );
		else if ( highlighted ) SetColour ( "ButtonHighlightedA" );
		else					SetColour ( "ButtonNormalA" );
		glVertex2i ( button->x, button->y + button->height );

		if      ( clicked )		SetColour ( "ButtonClickedB" );
		else if ( highlighted ) SetColour ( "ButtonHighlightedB" );
		else					SetColour ( "ButtonNormalB" );
		glVertex2i ( button->x, button->y );

		if		( clicked )		SetColour ( "ButtonClickedA" );
		else if ( highlighted ) SetColour ( "ButtonHighlightedA" );
		else					SetColour ( "ButtonNormalA" );
		glVertex2i ( button->x + button->width, button->y );

		if		( clicked )		SetColour ( "ButtonClickedB" );
		else if ( highlighted ) SetColour ( "ButtonHighlightedB" );
		else					SetColour ( "ButtonNormalB" );
		glVertex2i ( button->x + button->width, button->y + button->height );

	glEnd ();
	
	char softwarename [128];
	float version;
	sscanf ( button->caption, "%s v%f", softwarename, &version );

	if ( nhighlighted || clicked )	glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );
	else							glColor4f ( 1.0f, 1.0f, 1.0f, ALPHA );    

	// Write the software name
	GciDrawText ( button->x + 5, (button->y + button->height / 2) + 3, softwarename );

	// Write the version number
	GciDrawText ( button->x + button->width - 30, (button->y + button->height / 2) + 3, button->caption + strlen(softwarename) );
		
}

bool SWInterface::HasSoftwareType ( int softwareTYPE )
{

	// Look through the player's databank to see if this type of program shows up

	DataBank *db = &(game->GetWorld ()->GetPlayer ()->gateway.databank);

	for ( int di = 0; di < db->GetDataSize (); ++di ) {

		if ( db->GetDataFile (di) &&
			 db->GetDataFile (di)->TYPE == DATATYPE_PROGRAM &&
			 db->GetDataFile (di)->softwareTYPE == softwareTYPE ) 
			 return true;

	}

	return false;
		
}

void SWInterface::ToggleSubMenu ( int softwareTYPE, int x, int y )
{

	if ( !IsVisibleSubMenu ( softwareTYPE ) ) {

		// Create a list of all software in memory

		LList <char *> software;				// Bit of a hack - think about this
		LList <float> versions;

		DataBank *db = &(game->GetWorld ()->GetPlayer ()->gateway.databank);

		for ( int di = 0; di < db->GetDataSize (); ++di ) {

			if ( db->GetDataFile (di) &&
				 db->GetDataFile (di)->TYPE == DATATYPE_PROGRAM &&
				 db->GetDataFile (di)->softwareTYPE == softwareTYPE ) {

				// This sofware is the correct type.  Add it into the list if it isn't already there,
				// or if its version is higher than any existing program of the same name

				// First try to find a software item of the same name
				int existingindex = -1;
				
				for ( int si = 0; si < software.Size (); ++si ) {
					if ( strcmp ( software.GetData (si), db->GetDataFile (di)->title ) == 0 ) {
						existingindex = si;
						break;
					}
				}

				// If it already exists, add this item in only if it has a higher version number

				if ( existingindex == -1 ) {

					software.PutData ( db->GetDataFile (di)->title );
					versions.PutData ( db->GetDataFile (di)->version );

				}
				if ( existingindex != -1 &&	
					 db->GetDataFile (di)->version > versions.GetData (existingindex) ) {
				
					software.RemoveData ( existingindex );
					versions.RemoveData ( existingindex );

					software.PutDataAtIndex ( db->GetDataFile (di)->title, existingindex );
					versions.PutDataAtIndex ( db->GetDataFile (di)->version, existingindex );

				}

			}

		}

		int timesplit = 500;
		if ( software.Size () > 0 )
			timesplit /= software.Size ();				// Ensures total time = 500ms

		for ( int si = 0; si < software.Size (); ++si ) {
	
			char caption [128], tooltip [128], name [128];
			UplinkSnprintf ( caption, sizeof ( caption ), "%s v%1.1f", software.GetData (si), versions.GetData (si) );
			UplinkStrncpy ( tooltip, "Runs this software application", sizeof ( tooltip ) );
			UplinkSnprintf ( name, sizeof ( name ), "hud_software %d", si );
			EclRegisterButton ( x, y, 140, 15, caption, tooltip, name );
			EclRegisterButtonCallbacks ( name, SoftwareDraw, SoftwareClick, button_click, SoftwareHighlight );
			EclRegisterMovement ( name, x, y - si * 17, si * timesplit );

		}

		currentsubmenu = softwareTYPE;

        SgPlaySound ( RsArchiveFileOpen ( "sounds/softwaremenu.wav" ), "sounds/softwaremenu.wav", false );

	}
	else {

		// Remove the software menu
		// We don't know how many entries there could be, so keep deleting until they run out

		int i = 0;
		char name [32];
		UplinkSnprintf ( name, sizeof ( name ), "hud_software %d", i );

		while ( EclGetButton ( name ) != NULL ) {

			EclRemoveButton ( name );
			++i;
			UplinkSnprintf ( name, sizeof ( name ), "hud_software %d", i );

		}

		// Redraw the button that was selected

		char bname [32];
		UplinkSnprintf ( bname, sizeof ( bname ), "hud_swmenu %d", currentsubmenu );
		EclDirtyButton ( bname );

		currentsubmenu = SOFTWARETYPE_NONE;

	}

}

void SWInterface::SoftwareHighlight ( Button *button )
{

	// Don't do anything immediately, but set some timer events
	// up so that a list of all versions of this software item
	// appear in a new sub menu in a few milliseconds

	int index;
	sscanf ( button->name, "hud_software %d", &index );

	if ( currentprogrambutton != index ) {

		// Remove any old version menus

        if ( IsVisibleVersionMenu ( NULL ) ) {
			ToggleVersionMenu ( NULL, 0, 0 );            
        }
		else {										// Force redraw of previously selected button
			char bname [32];
			UplinkSnprintf ( bname, sizeof ( bname ), "hud_software %d", currentprogrambutton );
			EclDirtyButton ( bname );
		}

		currentprogrambutton = index;
		timesync = (int) EclGetAccurateTime ();

	}

	button_highlight ( button );

}

bool SWInterface::IsVisibleSubMenu	( int softwareTYPE )
{

	return ( EclGetButton ( "hud_software 0" ) != NULL && 
			 currentsubmenu == softwareTYPE );

}

void SWInterface::StartMenuItemDraw ( Button *button, bool highlighted, bool clicked )
{

	int softwareTYPE;
	sscanf ( button->name, "hud_swmenu %d", &softwareTYPE );

	if ( softwareTYPE == currentsubmenu )
		button_draw ( button, true, clicked );

	else
		button_draw ( button, highlighted, clicked );

}

void SWInterface::StartMenuItemDrawUnavailable ( Button *button, bool highlighted, bool clicked )
{

	// For now ;)

	button_draw ( button, highlighted, clicked );

}

void SWInterface::StartMenuItemHighlight ( Button *button )
{

	int softwareTYPE;
	sscanf ( button->name, "hud_swmenu %d", &softwareTYPE );

	// Create this menu if it doesn't exist

	if ( !IsVisibleSubMenu ( softwareTYPE ) ) {

		// Remove the old sub menu
		
		if ( IsVisibleVersionMenu ( NULL ) )
			ToggleVersionMenu ( NULL, 0, 0 );

		if ( IsVisibleSubMenu ( currentsubmenu ) )
			ToggleSubMenu ( currentsubmenu, 0, 0 );

		// Create the new sub menu

		ToggleSubMenu ( softwareTYPE, button->x + button->width + 2, button->y );

	}

	button_highlight ( button );

	// Stop any version lists from appearing

	currentprogrambutton = -1;

}

void SWInterface::StartMenuItemHighlightUnavailable ( Button *button )
{

	// Remove the old sub menus

	if ( IsVisibleVersionMenu ( NULL ) )
		ToggleVersionMenu ( NULL, 0, 0 );
	
	if ( IsVisibleSubMenu ( currentsubmenu ) )
		ToggleSubMenu ( currentsubmenu, 0, 0 );

	button_highlight ( button );

}

void SWInterface::ToggleSoftwareMenu ()
{

	if ( !IsVisibleSoftwareMenu () ) {

		// Create the "start" menu

        int screenH = app->GetOptions()->GetOptionValue("graphics_screenheight");
        int yPos = screenH - 90;

		EclRegisterButton ( 3, yPos, 100, 15, "File Utilities", "Eg file copiers, deleters, encrypters etc", "hud_swmenu 1" );
		EclRegisterButton ( 3, yPos, 100, 15, "Hardware Drivers", "Eg gateway cameras", "hud_swmenu 2" );
		EclRegisterButton ( 3, yPos, 100, 15, "Security", "Eg log deleters, Trace trackers etc", "hud_swmenu 3" );
		EclRegisterButton ( 3, yPos, 100, 15, "Crackers", "Eg password breakers, decrypters etc", "hud_swmenu 4" );
		EclRegisterButton ( 3, yPos, 100, 15, "Bypassers", "Eg proxy bypass, firewall bypass etc", "hud_swmenu 5" );
		EclRegisterButton ( 3, yPos, 100, 15, "LAN tools", "Eg LAN Scan, LAN probe etc", "hud_swmenu 6" );
		EclRegisterButton ( 3, yPos, 100, 15, "Other", "Other software programs", "hud_swmenu 10" );

		if ( HasSoftwareType ( 1 ) )	EclRegisterButtonCallbacks ( "hud_swmenu 1", StartMenuItemDraw, NULL, NULL, StartMenuItemHighlight );
		else							EclRegisterButtonCallbacks ( "hud_swmenu 1", StartMenuItemDrawUnavailable, NULL, NULL, StartMenuItemHighlightUnavailable );

		if ( HasSoftwareType ( 2 ) )	EclRegisterButtonCallbacks ( "hud_swmenu 2", StartMenuItemDraw, NULL, button_click, StartMenuItemHighlight );
		else							EclRegisterButtonCallbacks ( "hud_swmenu 2", StartMenuItemDrawUnavailable, NULL, NULL, StartMenuItemHighlightUnavailable );

		if ( HasSoftwareType ( 3 ) )	EclRegisterButtonCallbacks ( "hud_swmenu 3", StartMenuItemDraw, NULL, button_click, StartMenuItemHighlight );
		else							EclRegisterButtonCallbacks ( "hud_swmenu 3", StartMenuItemDrawUnavailable, NULL, NULL, StartMenuItemHighlightUnavailable );

		if ( HasSoftwareType ( 4 ) )	EclRegisterButtonCallbacks ( "hud_swmenu 4", StartMenuItemDraw, NULL, button_click, StartMenuItemHighlight );
		else							EclRegisterButtonCallbacks ( "hud_swmenu 4", StartMenuItemDrawUnavailable, NULL, NULL, StartMenuItemHighlightUnavailable );

		if ( HasSoftwareType ( 5 ) )	EclRegisterButtonCallbacks ( "hud_swmenu 5", StartMenuItemDraw, NULL, button_click, StartMenuItemHighlight );
		else							EclRegisterButtonCallbacks ( "hud_swmenu 5", StartMenuItemDrawUnavailable, NULL, NULL, StartMenuItemHighlightUnavailable );

		if ( HasSoftwareType ( 6 ) )	EclRegisterButtonCallbacks ( "hud_swmenu 6", StartMenuItemDraw, NULL, button_click, StartMenuItemHighlight );
		else							EclRegisterButtonCallbacks ( "hud_swmenu 6", StartMenuItemDrawUnavailable, NULL, NULL, StartMenuItemHighlightUnavailable );

		if ( HasSoftwareType ( 10 ) )	EclRegisterButtonCallbacks ( "hud_swmenu 10", StartMenuItemDraw, NULL, button_click, StartMenuItemHighlight );
		else							EclRegisterButtonCallbacks ( "hud_swmenu 10", StartMenuItemDrawUnavailable, NULL, NULL, StartMenuItemHighlightUnavailable );


		int timesplit = 500 / 6;							// Ensures total time = 500ms

		EclRegisterMovement ( "hud_swmenu 1", 3, yPos - 6 * 17, 6 * timesplit );
		EclRegisterMovement ( "hud_swmenu 2", 3, yPos - 5 * 17, 5 * timesplit );
		EclRegisterMovement ( "hud_swmenu 3", 3, yPos - 4 * 17, 4 * timesplit );
		EclRegisterMovement ( "hud_swmenu 4", 3, yPos - 3 * 17, 3 * timesplit );
		EclRegisterMovement ( "hud_swmenu 5", 3, yPos - 2 * 17, 2 * timesplit );
		EclRegisterMovement ( "hud_swmenu 6", 3, yPos - 1 * 17, 1 * timesplit );
		EclRegisterMovement ( "hud_swmenu 10", 3, yPos - 0 * 17, 0 * timesplit );

        SgPlaySound ( RsArchiveFileOpen ("sounds/softwaremenu.wav"), "sounds/softwaremenu.wav", false );

	}
	else {

		// Remove the old sub menus
		
		if ( IsVisibleVersionMenu ( NULL ) )
			ToggleVersionMenu ( NULL, 0, 0 );

		if ( IsVisibleSubMenu ( currentsubmenu ) )
			ToggleSubMenu ( currentsubmenu, 0, 0 );
		
		EclRemoveButton ( "hud_swmenu 1" );
		EclRemoveButton ( "hud_swmenu 2" );
		EclRemoveButton ( "hud_swmenu 3" );
		EclRemoveButton ( "hud_swmenu 4" );
		EclRemoveButton ( "hud_swmenu 5" );
		EclRemoveButton ( "hud_swmenu 6" );
		EclRemoveButton ( "hud_swmenu 10" );

	}

}

bool SWInterface::HasVersionMenu ( char *program )
{
	
	// Does the player have more than 1 version?

	int count = 0;

	DataBank *db = &(game->GetWorld ()->GetPlayer ()->gateway.databank);

	for ( int di = 0; di < db->GetDataSize (); ++di ) {

		if ( db->GetDataFile (di) &&
			 db->GetDataFile (di)->TYPE == DATATYPE_PROGRAM &&
			 strcmp ( db->GetDataFile (di)->title, program ) == 0 ) {
		
			 count++;
			 if ( count > 1 ) return true;

		}

	}

	return false;

}

void SWInterface::ToggleVersionMenu ( char *program, int x, int y )
{

	if ( program && 
		 !IsVisibleVersionMenu ( program ) ) {

		// Create it
		
		LList <char *> software;				// Bit of a hack - think about this
		LList <float> versions;

		DataBank *db = &(game->GetWorld ()->GetPlayer ()->gateway.databank);

		for ( int di = 0; di < db->GetDataSize (); ++di ) {

			if ( db->GetDataFile (di) &&
				 db->GetDataFile (di)->TYPE == DATATYPE_PROGRAM &&
				 strcmp ( db->GetDataFile (di)->title, program ) == 0 ) {

				software.PutData ( db->GetDataFile (di)->title );
				versions.PutData ( db->GetDataFile (di)->version );

			}

		}

		// Ensures total time = 500ms

		int timesplit = 500;
		if ( software.Size () > 0 )
			timesplit /= software.Size ();

		for ( int si = 0; si < software.Size (); ++si ) {
	
			char caption [128], tooltip [128], name [128];
			UplinkSnprintf ( caption, sizeof ( caption ), "%s v%1.1f", software.GetData (si), versions.GetData (si) );
			UplinkStrncpy ( tooltip, "Runs this software application", sizeof ( tooltip ) );
			UplinkSnprintf ( name, sizeof ( name ), "hud_version %d", si );
			EclRegisterButton ( x, y, 120, 15, caption, tooltip, name );
			EclRegisterButtonCallbacks ( name, SoftwareDraw, SoftwareClick, button_click, button_highlight );
			EclRegisterMovement ( name, x, y - si * 17, si * timesplit );

		}
		
        SgPlaySound ( RsArchiveFileOpen ("sounds/softwaremenu.wav"), "sounds/softwaremenu.wav", false );

	}
	else {

		// Remove it
		// We don't know how many entries there could be, so keep deleting until they run out

		int i = 0;
		char name [128];
		UplinkSnprintf ( name, sizeof ( name ), "hud_version %d", i );

		while ( EclGetButton ( name ) != NULL ) {

			EclRemoveButton ( name );
			++i;
			UplinkSnprintf ( name, sizeof ( name ), "hud_version %d", i );

		}	

		// Force redraw of the button that fathered this menu

		char bname [32];
		UplinkSnprintf ( bname, sizeof ( bname ), "hud_software %d", currentprogrambutton );
		EclDirtyButton ( bname );

		currentprogrambutton = -1;

	}

}

bool SWInterface::IsVisibleVersionMenu ( char *program )
{

	if ( program ) 
		return ( EclGetButton ( "hud_version 0" ) != NULL &&
			     strstr ( EclGetButton ( "hud_version 0" )->caption, program ) != NULL );

	else
		return ( EclGetButton ( "hud_version 0" ) != NULL );

}

void SWInterface::Create ()
{

	if ( !IsVisible () ) {

		ToggleSoftwareMenu ();

	}

}

void SWInterface::Remove ()
{

	if ( IsVisible () ) {

		ToggleSoftwareMenu ();

	}

}

void SWInterface::Update ()
{

	// If the user has his mouse over a software button and a few ms passes,
	// then bring up a list of all software items of the same name but all versions

	if ( currentprogrambutton != -1 &&
		timesync != -1 &&
		EclGetAccurateTime () - timesync > 1000 ) {

		// Check that the mouse is still over the button in question

		char correctname [32];
		UplinkSnprintf ( correctname, sizeof ( correctname ), "hud_software %d", currentprogrambutton );

		Button *currentbutton = getcurrentbutton ();

		if ( currentbutton &&
			strcmp ( currentbutton->name, correctname ) == 0 ) {

			// Get the name of the software item

			char swname [64];
			int version;
			sscanf ( currentbutton->caption, "%s v%1.1f", swname, &version );

			// Create a version menu if neccisary

			if ( HasVersionMenu ( swname ) &&
				!IsVisibleVersionMenu ( swname ) ) {

				ToggleVersionMenu ( swname, currentbutton->x + currentbutton->width + 2, currentbutton->y );

			}

		}

		// Stop it from updating needlessly

		timesync = -1;

	}

}
	
bool SWInterface::IsVisibleSoftwareMenu ()
{

	return ( EclGetButton ( "hud_swmenu 1" ) != NULL );

}

bool SWInterface::IsVisible ()
{

	return IsVisibleSoftwareMenu ();

}

