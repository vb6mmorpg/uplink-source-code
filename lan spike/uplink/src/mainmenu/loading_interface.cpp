
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h> /*_glu_extention_library_*/

#include <stdlib.h>

#include "gucci.h"
#include "redshirt.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"
#include "app/serialise.h"

#include "options/options.h"

#include "mainmenu/mainmenu.h"
#include "mainmenu/loading_interface.h"

#include "mmgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LoadingInterface::LoadingInterface()
{

}

LoadingInterface::~LoadingInterface()
{

}

void LoadingInterface::Create ()
{	

	if ( !IsVisible () ) {

//		RsLoadArchive ( "loading.dat" );

        /*
            I don't do this dynamic loading/unloading of loading.dat here anymore
            The RsCloseArchive function makes use of BTree::RemoveData, which
            doesn't work properly.

            This results in a crash after multiple reloads
            Instead the loading.dat file is loaded at start up, along with the rest

            */

		int screenw = app->GetOptions ()->GetOptionValue ( "graphics_screenwidth" );
		int screenh = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
		
		// Decide which graphic to load up
        char *filename = app->GetOptions ()->ThemeFilename ( "loading/filenames.txt" );
		FILE *file = RsArchiveFileOpen ( filename, "rt" );		
		UplinkAssert (file);

        LList <char *> filenames;
		while ( !feof (file) ) {
			char *thisfile = new char [256];
			fscanf ( file, "%s\n", thisfile );
			// kris.tif have simply not the same level of quality of the other files
			if ( strcmp ( thisfile, "loading/kris.tif" ) != 0 )
				filenames.PutData ( thisfile );
			else
				delete [] thisfile;
		}
		float value = ( (float) rand () / (float) RAND_MAX );		
		int pictureindex = (int) ( value * filenames.Size () );
    
		EclRegisterButton ( 0, 0, screenw, screenh, "", "", "loading_background" );
		button_assignbitmap ( "loading_background", filenames [pictureindex] );
		EclGetButton ( "loading_background" )->image_standard->Scale ( 512, 512 );
		EclRegisterButtonCallbacks ( "loading_background", imagebutton_drawtextured, NULL, NULL, NULL );
		
        // Clear up memory used

        DeleteLListData ( &filenames );

		EclRegisterButton ( SX(440), SY(450), 200, 15, "Connecting to GATEWAY...", "", "loading_text" );
        EclRegisterButtonCallbacks ( "loading_text", text_draw, NULL, NULL, NULL );

        RsArchiveFileClose ( filename, file );
        delete [] filename;

//		RsCloseArchive ( "loading.dat" );

	}

}

void LoadingInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "loading_background" );
		EclRemoveButton ( "loading_text" );

	}

}

void LoadingInterface::Update ()
{
}

bool LoadingInterface::IsVisible ()
{

	return ( EclGetButton ( "loading_background" ) != 0 );

}

int LoadingInterface::ScreenID ()
{

	return MAINMENU_LOADING;

}
