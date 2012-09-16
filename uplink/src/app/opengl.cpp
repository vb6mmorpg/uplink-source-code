// -*- tab-width:4; c-file-style:"cc-mode" -*- 
/*

	All generic OpenGL code goes in here
	i.e mouse handler, keyboard handler, idle function etc

  */

#include "stdafx.h"

#include <GL/gl.h>

#include <GL/glu.h>

#include "tosser.h"
#include "eclipse.h"
#include "vanbakel.h"
#include "gucci.h"
#include "soundgarden.h"
#include "redshirt.h"

#include "options/options.h"

#include "app/app.h"
#include "app/opengl.h"
#include "app/globals.h"
#include "app/opengl_interface.h"
#include "app/miscutils.h"

#include "mainmenu/mainmenu.h"
#include "mainmenu/mainmenuscreen.h"

#include "view/view.h"

#include "game/game.h"

#include "interface/interface.h"
#include "interface/scrollbox.h"
#include "interface/localinterface/localinterface.h"
#include "interface/localinterface/hud_interface.h"
#include "interface/localinterface/irc_interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/taskmanager/taskmanager.h"
#include "interface/taskmanager/uplinktask.h"

#include "world/world.h"
#include "world/player.h"

#include "mmgr.h"

//#define  DRAWMOUSE

// ============================================================================
// Local function definitions


local void init(void);
local void keyboard(unsigned char, int, int);
local void specialkeyboard(int, int, int);
local void mouse(int,int,int,int);
local void mousemove(int,int);
local void passivemouse(int,int);
local void resize(int, int);
local void drawcube(int, int, int);
local void idle(void);

local int lastidleupdate = 0;
local int mouseX = 0;
local int mouseY = 0;

// ============================================================================


void opengl_initialise (int argc, char **argv)
{

	bool debugging = false;
	if ( app->GetOptions ()->IsOptionEqualTo ( "game_debugstart", 1 ) )
		debugging = true;

	int screenWidth = app->GetOptions ()->GetOptionValue ( "graphics_screenwidth" );
	int screenHeight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	bool runFullScreen = app->GetOptions ()->IsOptionEqualTo ( "graphics_fullscreen", 1 ) &&
						!app->GetOptions ()->IsOptionEqualTo ( "graphics_safemode", 1 );
	int screenDepth = app->GetOptions ()->GetOptionValue ( "graphics_screendepth" );
    int screenRefresh = app->GetOptions ()->GetOptionValue ( "graphics_screenrefresh" );

	int graphics_flags = GCI_DOUBLE | 
	                     GCI_RGB | 
	                     (runFullScreen ? GCI_FULLSCREEN : 0) |
	                     (debugging ? GCI_DEBUGSTART : 0);

	char *errorMessageInitLib = GciInitGraphicsLibrary ( graphics_flags );
	if ( errorMessageInitLib ) {
		UplinkAbort ( errorMessageInitLib );
	}

	GciScreenMode *mode = GciGetClosestScreenMode ( screenWidth, screenHeight );
	if ( mode->w != screenWidth || mode->h != screenHeight ) {
		app->GetOptions ()->SetOptionValue ( "graphics_screenwidth", mode->w );
		app->GetOptions ()->SetOptionValue ( "graphics_screenheight", mode->h );
		screenWidth = mode->w;
		screenHeight = mode->h;
	}
	delete mode;

	char *errorMessageInit = GciInitGraphics( "uplink",
			 graphics_flags, 
			 screenWidth, screenHeight,
			 screenDepth, screenRefresh,
			 argc, argv );
	if ( errorMessageInit ) {
		UplinkAbort ( errorMessageInit );
	}

	if ( debugging ) printf ( "Initialising OpenGL...\n" );
	init();
	if ( debugging ) printf ( "Finished initialising OpenGL.\n" );
	

	if ( debugging ) printf ( "Now registering callback functions..." );
    setcallbacks ();
    if ( debugging ) printf ( "done\n ");

}

void opengl_run ()
{

	GciMainLoop ();

}

void opengl_close ()
{

	GciRestoreScreenSize ();

}

#ifdef WIN32

bool opengl_isSoftwareRendering ()
{

    char path_opengl32_dll[256];
    UplinkSnprintf ( path_opengl32_dll, sizeof ( path_opengl32_dll ), "%sopengl32.dll", app->path );

	return DoesFileExist ( path_opengl32_dll );

}

bool opengl_setSoftwareRendering ( bool softwareRendering )
{

    char path_opengl32_dll[256];
    UplinkSnprintf ( path_opengl32_dll, sizeof ( path_opengl32_dll ), "%sopengl32.dll", app->path );
    char path_opengl32_dll_bak[256];
    UplinkSnprintf ( path_opengl32_dll_bak, sizeof ( path_opengl32_dll_bak ), "%sopengl32.dll.bak", app->path );

    char new_path_opengl32_dll[256];
    UplinkSnprintf ( new_path_opengl32_dll, sizeof ( new_path_opengl32_dll ), "%sopengl32.dll", app->userpath );

	if ( softwareRendering ) {
		if ( !DoesFileExist ( path_opengl32_dll ) ) {
			if ( DoesFileExist ( path_opengl32_dll_bak ) && CopyFilePlain ( path_opengl32_dll_bak, path_opengl32_dll ) ) {
				return true;
			}
		}
		else {
			return true;
		}
	}
	else {
		if ( !DoesFileExist ( path_opengl32_dll ) ) {
			return true;
		}
		else {
			if ( !DoesFileExist ( path_opengl32_dll_bak ) ) {
				CopyFilePlain ( path_opengl32_dll, path_opengl32_dll_bak );
			}
			if ( RemoveFile( path_opengl32_dll ) ) {
				return true;
			}
			else {
				RemoveFile( new_path_opengl32_dll );
				if ( MoveFile( path_opengl32_dll, new_path_opengl32_dll ) != 0 ) {
					return true;
				}
			}
		}
	}

	return false;

}

#endif

local void init(void)
{

/*
	// First things first - show the OpenGL version in use
	printf ( "\n" );
	printf ( "OpenGL Driver Information\n" );
	printf ( "Vendor   : %s\n", glGetString ( GL_VENDOR ) );
	printf ( "Renderer : %s\n", glGetString ( GL_RENDERER ) );
	printf ( "Version  : %s\n", glGetString ( GL_VERSION ) );
	printf ( "GL Utils : %s\n", gluGetString ( (GLenum) GLU_VERSION ) );
	printf ( "\n" );
*/

	glClearColor(0.0, 0.0, 0.0, 0.0);

	// ====================================================================== 
	// Fix for Riva TNT cards (these don't automatically clear the background
	// UPDATE : Should now 	be covered by the GLUT_NORMAL_DAMAGED code in display()
	// int screenwidth = app->GetOptions ()->GetOptionValue ( "graphics_screenwidth" );
	// int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	// clear_draw ( 0, 0, screenwidth, screenheight );
	// ======================================================================

//	glEnable(GL_DEPTH_TEST);
	glDisable ( GL_DEPTH_TEST );
		
	glMatrixMode(GL_MODELVIEW);

	glTexEnvf ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );	

        glDisable(GL_ALPHA_TEST);        
        glDisable(GL_FOG);        
		glDisable(GL_LIGHTING);
        glDisable(GL_LOGIC_OP);        
		glDisable(GL_STENCIL_TEST);
        glDisable(GL_TEXTURE_1D);      
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		
        glPixelTransferi(GL_MAP_COLOR, GL_FALSE);
        glPixelTransferi(GL_RED_SCALE, 1);
        glPixelTransferi(GL_RED_BIAS, 0);
        glPixelTransferi(GL_GREEN_SCALE, 1);
        glPixelTransferi(GL_GREEN_BIAS, 0);
        glPixelTransferi(GL_BLUE_SCALE, 1);
        glPixelTransferi(GL_BLUE_BIAS, 0);
        glPixelTransferi(GL_ALPHA_SCALE, 1);
        glPixelTransferi(GL_ALPHA_BIAS, 0);

	GLuint texName;
	glGenTextures( 1, &texName );
	glBindTexture ( GL_TEXTURE_2D, texName );	

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glHint ( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
	glHint ( GL_LINE_SMOOTH_HINT,	 GL_NICEST );
	glHint ( GL_POINT_SMOOTH_HINT,	 GL_NICEST );

    EclReset ( app->GetOptions ()->GetOptionValue ("graphics_screenwidth"),
			   app->GetOptions ()->GetOptionValue ("graphics_screenheight") );
	EclRegisterClearDrawFunction      ( clear_draw );
	EclRegisterDefaultButtonCallbacks ( button_draw, NULL, button_click, button_highlight );
	EclRegisterSuperHighlightFunction ( 3, superhighlight_draw );

	if ( app->GetOptions ()->GetOption ("graphics_buttonanimations") &&
		 app->GetOptions ()->GetOption ("graphics_buttonanimations")->value == 0 ) {

		EclDisableAnimations ();

	}

	if ( app->GetOptions ()->GetOption ("graphics_fasterbuttonanimations") &&
		 app->GetOptions ()->GetOption ("graphics_fasterbuttonanimations")->value != 0 ) {

		EclEnableFasterAnimations ();

	}

}

void display(void)
{

    if ( app->Closed () ) return;

	if ( !GciAppVisible () ) return;

	/*

	// 
	// If the display was damaged (eg by other window activity)
	// Then we need to blank and both buffers now
	//

	static bool firsttime = true;
	if (firsttime) {
		firsttime = false;
		EclDirtyRectangle ( 0, 0, app->GetOptions ()->GetOptionValue ( "graphics_screenwidth" ), 
								  app->GetOptions ()->GetOptionValue ( "graphics_screenheight" ) );
	}

	if (GciLayerDamaged())
		EclDirtyRectangle ( 0, 0, app->GetOptions ()->GetOptionValue ( "graphics_screenwidth" ), 
								  app->GetOptions ()->GetOptionValue ( "graphics_screenheight" ) );

	//
	// If safe mode redraw EVERYTHING
	//

	int numFlips = 2;
	if ( //numFlips == 1 ||
		app->GetOptions ()->IsOptionEqualTo ( "graphics_safemode", 1 ) ) {
		EclDirtyRectangle ( 0, 0, app->GetOptions ()->GetOptionValue ( "graphics_screenwidth" ), 
								  app->GetOptions ()->GetOptionValue ( "graphics_screenheight" ) );
	}

	//
	// Draw the world into both buffers
	//


	for ( int i = 0; i < numFlips; ++i ) {

	*/

		//  Draw the Eclipse buttons

		////For speed testing
		//static int counter = 0;
		//int inter1, inter2;
		//inter1 = (int) ( EclGetAccurateTime () * 100 );

		glPushMatrix ();
		glLoadIdentity ();
		glMatrixMode ( GL_PROJECTION );
		glPushMatrix ();
		glLoadIdentity ();
		glPushAttrib ( GL_ALL_ATTRIB_BITS );
        
		glOrtho ( 0.0, app->GetOptions ()->GetOptionValue ( "graphics_screenwidth" ), 
					   app->GetOptions ()->GetOptionValue ( "graphics_screenheight" ), 0.0, -1.0, 1.0 );

		glTranslatef ( 0.375f, 0.375f, 0.0f );

		// Added by François for testing new display
		EclClearRectangle ( 0, 0, app->GetOptions ()->GetOptionValue ( "graphics_screenwidth" ), 
								  app->GetOptions ()->GetOptionValue ( "graphics_screenheight" ) );
        
		EclDrawAllButtons ();

		glPopAttrib ();
		glPopMatrix ();
		glMatrixMode ( GL_MODELVIEW );
		glPopMatrix ();

		//  Swap the buffers, do it all again to the new back-buffer
		GciSwapBuffers();
	/*
	}
	*/
	glFinish();

	////For speed testing
	//inter2 = (int) ( EclGetAccurateTime () * 100 );
	//counter++;
	////if ( counter % 5 == 0 ) {
	//	FILE *debugfile = fopen("c:/uplink_time.txt", "a");
	//	if ( debugfile ) {
	//		fprintf(debugfile, "Total Draw: %d\n", inter2 - inter1);
	//		fclose(debugfile);
	//	}
	////}

	/*

	//
	// Finished drawing to the two buffers - clear all dirty areas
	//

	EclDirtyClear ();

	*/

}


void keyboard(unsigned char key, int x, int y)
{

    if ( app->Closed () ) return;

	if ( key == 13 ) {									// ======== Return key

      	bool returned = false;

        if ( game->IsRunning () ) 
        {
            if ( game->GetInterface ()->GetLocalInterface ()->InScreen() != SCREEN_NONE &&
                 game->GetInterface ()->GetLocalInterface ()->GetInterfaceScreen ()->ScreenID () == SCREEN_IRC )
                returned = ((IRCInterface *) game->GetInterface ()->GetLocalInterface ()->GetInterfaceScreen ())->ReturnKeyPressed ();
            else
			    returned = game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ()->ReturnKeyPressed ();
        }
		else if ( app->GetMainMenu ()->InScreen () != MAINMENU_UNKNOWN )
			returned = app->GetMainMenu ()->GetMenuScreen ()->ReturnKeyPressed ();

	    if ( !returned ) {

		    // Pass the key press on to the box under the mouse
		    char *name = EclGetHighlightedButton ();
		    if ( EclIsButtonEditable (name) ) 
			    textbutton_keypress ( EclGetButton (name), key );

	    }

	}
    else if ( key == 27 ) {                             // ======== Esc key

		char *name = EclGetHighlightedButton ();

        if ( EclIsButtonEditable (name) ) {

            textbutton_keypress ( EclGetButton (name), key );

        }
        else {
        
            if ( game->IsRunning () )
                bool escaped = game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ()->EscapeKeyPressed ();

        }

    }
	else if ( key == 9 ) {								// ========= Tab key
		
        EclHighlightNextEditableButton ();

	}
	else if ( key == 96 ) { // backtick (`)

	    char screenpath [256];
	    UplinkSnprintf ( screenpath, sizeof ( screenpath ), "%sscreenshot.bmp", app->userpath );

		GciSaveScreenshot( screenpath );

	}
	else if ( (key >=1 && key <= 26) && key != 8 ) // Manually trap backspace
	{
		char field[10];
		UplinkSnprintf(field, sizeof(field), "Ctrl+%c", (char)key+64);

		char softwarename[128];
		UplinkSnprintf(softwarename, sizeof(softwarename), "%s", game->GetWorld ()->GetPlayer ()->gateway.functionKeys.GetField(field));

		if ( strcmp(softwarename, "**CHEATS") == 0 ) {

#ifdef CHEATMODES_ENABLED
			game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_CHEATS );
#else
#ifndef DEMOGAME
#ifndef WAREZRELEASE
			if ( game->IsRunning () && strcmp ( game->GetWorld ()->GetPlayer ()->handle, "Stormchild" ) == 0 ) {
				game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_CHEATS );
			} else {
				create_msgbox("TooManySecrets","Whose game is it anyway?");
			}
#endif
#endif
#endif
		} else if ( strcmp(softwarename, "**SCREENSHOT") == 0 ) {
			char screenpath [256];
			UplinkSnprintf ( screenpath, sizeof ( screenpath ), "%sscreenshot.bmp", app->userpath );

			GciSaveScreenshot( screenpath );
		} else if ( strcmp(softwarename, "**EXIT") == 0 ) {
			if ( game->IsRunning () ) app->SaveGame ( game->GetWorld ()->GetPlayer ()->handle );
			app->Close ();
		} else {
			bool inmemory = false;
			float version = 0.0f;
			DataBank *db = &(game->GetWorld ()->GetPlayer ()->gateway.databank);
			int numfiles = db->GetDataSize ();
			for ( int i = 0; i < numfiles; ++i ) {

				if ( db->GetDataFile (i) &&
					strcmp ( db->GetDataFile (i)->title, softwarename ) == 0 &&
					db->GetDataFile (i)->version > version ) {
					inmemory = true;
					version = db->GetDataFile (i)->version;
				}

			}

			// Run it if it is

			if ( inmemory ) {
				game->GetInterface ()->GetTaskManager ()->RunSoftware ( softwarename, version );
				SgPlaySound ( RsArchiveFileOpen ("sounds/runsoftware.wav"), "sounds/runsoftware.wav", false );
			}
		}
	}
    else {

		char *name = EclGetHighlightedButton ();
		if ( EclIsButtonEditable (name) ) 
			textbutton_keypress ( EclGetButton (name), key );

    }

	//GciPostRedisplay();

}

void specialkeyboard (int key, int x, int y)
{

    if ( app->Closed () ) return;

	switch ( key ) {

//		case GCI_KEY_F1:							// Cheat menu
//
//#ifdef CHEATMODES_ENABLED
//            game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_CHEATS );
//#else
//	#ifndef DEMOGAME
//		#ifndef WAREZRELEASE								// Prevent cheats from working in the warez release
//            if ( game->IsRunning () && strcmp ( game->GetWorld ()->GetPlayer ()->handle, "Stormchild" ) == 0 )
//                game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_CHEATS );
//		#endif
//	#endif
//#endif  
//			break;
//
//		case GCI_KEY_F9:
//
//			char screenpath [256];
//			UplinkSnprintf ( screenpath, sizeof ( screenpath ), "%sscreenshot.bmp", app->userpath );
//
//			GciSaveScreenshot( screenpath );
//
//			break;
//
//
//#ifndef DEMOGAME
//
//		case GCI_KEY_F12:							// Exit
//			if ( game->IsRunning () ) app->SaveGame ( game->GetWorld ()->GetPlayer ()->handle );
//			app->Close ();
//			break;
//
//#endif
			case GCI_KEY_F1:
			case GCI_KEY_F2:
			case GCI_KEY_F3:
			case GCI_KEY_F4:
			case GCI_KEY_F5:
			case GCI_KEY_F6:
			case GCI_KEY_F7:
			case GCI_KEY_F8:
			case GCI_KEY_F9:
			case GCI_KEY_F10:
			case GCI_KEY_F11:
			case GCI_KEY_F12:

				char field[5];
				UplinkSnprintf(field, sizeof(field), "F%d", key);
				
				char softwarename[128];
				UplinkSnprintf(softwarename, sizeof(softwarename), "%s", game->GetWorld ()->GetPlayer ()->gateway.functionKeys.GetField(field));

				if ( strcmp(softwarename, "**CHEATS") == 0 ) {

#ifdef CHEATMODES_ENABLED
            game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_CHEATS );
#else
	#ifndef DEMOGAME
		#ifndef WAREZRELEASE
					if ( game->IsRunning () && strcmp ( game->GetWorld ()->GetPlayer ()->handle, "Stormchild" ) == 0 ) {
						game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_CHEATS );
					} else {
						create_msgbox("TooManySecrets","Whose game is it anyway?");
					}
		#endif
	#endif
#endif  
				} else if ( strcmp(softwarename, "**SCREENSHOT") == 0 ) {
			char screenpath [256];
			UplinkSnprintf ( screenpath, sizeof ( screenpath ), "%sscreenshot.bmp", app->userpath );

			GciSaveScreenshot( screenpath );
				} else if ( strcmp(softwarename, "**EXIT") == 0 ) {
					if ( game->IsRunning () ) app->SaveGame ( game->GetWorld ()->GetPlayer ()->handle );
					app->Close ();
				} else {
					bool inmemory = false;
					float version = 0.0f;
					DataBank *db = &(game->GetWorld ()->GetPlayer ()->gateway.databank);
					int numfiles = db->GetDataSize ();
					for ( int i = 0; i < numfiles; ++i ) {

						if ( db->GetDataFile (i) &&
							strcmp ( db->GetDataFile (i)->title, softwarename ) == 0 &&
							db->GetDataFile (i)->version > version ) {
							inmemory = true;
							version = db->GetDataFile (i)->version;
						}

				}

				// Run it if it is

				if ( inmemory ) {
					game->GetInterface ()->GetTaskManager ()->RunSoftware ( softwarename, version );
					SgPlaySound ( RsArchiveFileOpen ("sounds/runsoftware.wav"), "sounds/runsoftware.wav", false );
				}
			break;
				}
	}

	//GciPostRedisplay ();

}

void idle ()
{

    if ( app->Closed () ) return;

	int timesincelastupdate = (int) ( EclGetAccurateTime () - lastidleupdate );

#ifdef WIN32
	if ( timesincelastupdate < 16 )
		Sleep( 16 - timesincelastupdate ); // Play nice with other apps. May cause issues on some systems.
#endif

	//int EclAnimationsRunning();

	////For speed testing
	//static int counter = 0;
	//int inter1, inter2, inter3, inter4;

	//inter1 = (int) ( EclGetAccurateTime () * 100 );
	EclUpdateAllAnimations ();		
	//inter2 = (int) ( EclGetAccurateTime () * 100 );
    SgUpdate ();
	//inter3 = (int) ( EclGetAccurateTime () * 100 );
	app->Update ();
	//inter4 = (int) ( EclGetAccurateTime () * 100 );

	//counter++;
	//if ( counter % 5 == 0 ) {
	//	FILE *debugfile = fopen("c:/uplink_time.txt", "a");
	//	if ( debugfile ) {
	//		fprintf(debugfile, "Total update: %d, %d, %d, %d\n", inter4 - inter1, inter2 - inter1, inter3 - inter2, inter4 - inter3);
	//		fclose(debugfile);
	//	}
	//}

	lastidleupdate = (int) EclGetAccurateTime ();

	//if (EclAnimationsRunning()) 
	//	GciPostRedisplay ();

}

void mouse ( int button, int state, int x, int y )
{

    if ( app->Closed () ) return;

	if ( button == GCI_LEFT_BUTTON && state == GCI_DOWN ) {

		char *name = EclGetButtonAtCoord ( x, y );

		if ( name ) {
			Button *button = EclGetButton ( name );
			if ( button ) button->MouseDown ();			
		}

	}
	else if ( button == GCI_LEFT_BUTTON && state == GCI_UP ) {

		EclUnClickButton ();
        ScrollBox::UnGrabScrollBar();
		//GciPostRedisplay ();

		char *name = EclGetButtonAtCoord ( x, y );
		
		if ( name ) {			
			Button *button = EclGetButton ( name );
			if ( button ) button->MouseUp ();
		}
		else {

			// Clicked on nothing - remove the software menu
			if ( game->IsRunning () && 
				game->GetInterface ()->GetLocalInterface ()->GetHUD ()->si.IsVisibleSoftwareMenu () ) {

				game->GetInterface ()->GetLocalInterface ()->GetHUD ()->si.ToggleSoftwareMenu ();

			}

		}

	}
	else if ( button == GCI_RIGHT_BUTTON && state == GCI_UP ) {

		// Put down any programs that are linked to the cursur

		if ( game->IsRunning () ) 
			game->GetInterface ()->GetTaskManager ()->SetTargetProgram ( -1 );

	}

}

local void mousedraw ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert (button);

	glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );

	glBegin ( GL_LINES );

		glLineWidth ( 1.0 );
		glVertex2i ( button->x, button->y );
		glVertex2i ( button->x + button->width - 1, button->y + button->height - 1 );
		
		glVertex2i ( button->x, button->y );
		glVertex2i ( (int) ( button->x + button->width/1.5 ), button->y );

		glVertex2i ( button->x, button->y );
		glVertex2i ( button->x, (int) ( button->y + button->height/1.5 ) );
	glEnd ();

	glLineWidth ( 1.0 );

}

void passivemouse ( int x, int y )
{
    if ( app->Closed () ) return;

	mouseX = x;
	mouseY = y;

    bool showSWMouse = app->GetOptions ()->IsOptionEqualTo ( "graphics_softwaremouse", 1 );

	if ( showSWMouse ) {

		// Draw the mouse

		if ( !EclGetButton ( "mouse" ) ) {
			EclRegisterButton ( 0, 0, 8, 8, "", "mouse" );
			EclRegisterButtonCallbacks ( "mouse", mousedraw, NULL, NULL, NULL );
		}

		EclButtonBringToFront ( "mouse" );
		Button *mouse = EclGetButton ( "mouse" );
		UplinkAssert (mouse);
		EclDirtyRectangle ( mouse->x, mouse->y, mouse->width, mouse->height );
		EclGetButton ( "mouse" )->x = x + 1;
		EclGetButton ( "mouse" )->y = y + 1;
		EclDirtyRectangle ( mouse->x, mouse->y, mouse->width, mouse->height );

	}

	// Look for any buttons under the mouse

	char *name = EclGetButtonAtCoord ( x, y );

	if ( name ) {
		Button *button = EclGetButton ( name );
		if ( button ) button->MouseMove ();			
	}
	else {
		EclUnHighlightButton ();
		tooltip_update ( " " );
		//GciPostRedisplay ();
	}

}

void mousemove(int x, int y)
{

    if ( app->Closed () ) return;

	mouseX = x;
	mouseY = y;

	//Removed by Francois
	//return;

    // Update scroll bars

    if ( ScrollBox::IsGrabInProgress() ) ScrollBox::UpdateGrabScroll();


    bool showSWMouse = app->GetOptions ()->IsOptionEqualTo ( "graphics_softwaremouse", 1 );

	if ( showSWMouse ) {

		// Draw the mouse

		if ( !EclGetButton ( "mouse" ) ) {
			EclRegisterButton ( 0, 0, 8, 8, "", "mouse" );
			EclRegisterButtonCallbacks ( "mouse", mousedraw, NULL, NULL, NULL );
		}

		EclButtonBringToFront ( "mouse" );
		Button *mouse = EclGetButton ( "mouse" );
		UplinkAssert (mouse);
		EclDirtyRectangle ( mouse->x, mouse->y, mouse->width, mouse->height );
		EclGetButton ( "mouse" )->x = x + 1;
		EclGetButton ( "mouse" )->y = y + 1;
		EclDirtyRectangle ( mouse->x, mouse->y, mouse->width, mouse->height );

	}

}

int get_mouseX ()
{

	return mouseX;

}

int get_mouseY ()
{

	return mouseY;

}

void resize(int width, int height)
{

	if (height == 0) height = 1;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	/* note we divide our width by our height to get the aspect ratio */
	gluPerspective(45.0, width / height, 1.0, 400.0);

	/* set initial position */
	glTranslatef(0.0, -5.0, -150.0);

	glMatrixMode(GL_MODELVIEW);

}

void setcallbacks ()
{
	GciDisplayFunc(display);	
	GciMouseFunc(mouse);
	GciMotionFunc(mousemove);
	GciPassiveMotionFunc(passivemouse);
	GciKeyboardFunc(keyboard);
	GciSpecialFunc(specialkeyboard);
	GciIdleFunc (idle);
	GciReshapeFunc(resize);  
}
