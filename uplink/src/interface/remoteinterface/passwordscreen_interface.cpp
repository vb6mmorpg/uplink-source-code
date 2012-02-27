
#ifdef WIN32
#include <windows.h>
#endif

#ifndef HAVE_GLES
#include <GL/gl.h>
#include <GL/glu.h> /* glu extention library */
#else
#include <GLES/gl.h>
#include <GLES/glues.h>
#endif

#include <string.h>

#include "eclipse.h"
#include "soundgarden.h"
#include "gucci.h"
#include "redshirt.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/miscutils.h"
#include "app/opengl_interface.h"

#include "game/game.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/localinterface/keyboardinterface.h"
#include "interface/remoteinterface/passwordscreen_interface.h"
#include "interface/taskmanager/taskmanager.h"

#include "world/world.h"
#include "world/player.h"
#include "world/computer/computer.h"
#include "world/computer/logbank.h"
#include "world/computer/computerscreen/passwordscreen.h"

#include "mmgr.h"


void PasswordScreenInterface::CursorFlash ()
{

	if ( strcmp ( EclGetButton ( "passwordscreen_password" )->caption, "_" ) == 0 )
		EclRegisterCaptionChange ( "passwordscreen_password", " ", 1000, CursorFlash );

	else if ( strcmp ( EclGetButton ( "passwordscreen_password" )->caption, " " ) == 0 )
		EclRegisterCaptionChange ( "passwordscreen_password", "_", 1000, CursorFlash );

}

void PasswordScreenInterface::PasswordClick ( Button *button )
{

	UplinkAssert (button);

	RemoteInterfaceScreen *ris = game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (ris);
	PasswordScreenInterface *ps = (PasswordScreenInterface *) ris;

	game->GetInterface ()->GetTaskManager ()->SetProgramTarget ( ps->GetComputerScreen (), button->name, -1 );

}

void PasswordScreenInterface::AccessCodeClick ( Button *button )
{

	UplinkAssert (button);

	char *fullcode = new char [strlen(button->caption) + 1];
	UplinkSafeStrcpy ( fullcode, button->caption );

	char *code, *code2;

	code = strchr ( fullcode, '\'' );
	if ( code ) {
		code += 1;
		code2 = strchr ( code, '\'' );
		if ( code2 ) {
			*code2 = '\0';
			EclRegisterCaptionChange ( "passwordscreen_password", code );
		}
	}

	delete [] fullcode;

}

void PasswordScreenInterface::BypassClick ( Button *button )
{


//	game->GetWorld ()->GetPlayer ()->GetConnection ()->BeginTrace ();
	
	RemoteInterfaceScreen *ris = game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	PasswordScreenInterface *ps = (PasswordScreenInterface *) ris;
	if ( game->GetWorld ()->GetPlayer ()->IsConnected () )
		ps->NextPage ();

}

void PasswordScreenInterface::CodeButtonDraw ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert (button);

	// Draw a background colour

	SetColour ( "PasswordBoxBackground" );
	
#ifndef HAVE_GLES
	glBegin ( GL_QUADS );

		glVertex2i ( button->x, button->y );
		glVertex2i ( button->x + button->width - 1, button->y );
		glVertex2i ( button->x + button->width - 1, button->y + button->height );
		glVertex2i ( button->x, button->y + button->height );

	glEnd ();
#else
	GLfloat verts[] = {
		button->x, button->y,
		button->x + button->width - 1, button->y,
		button->x + button->width - 1, button->y + button->height,
		button->x, button->y + button->height
	};

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, verts);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
#endif

	// Print the text

    SetColour ( "DefaultText" );
	
	char *caption = new char [strlen(button->caption) + 1];
	for ( size_t i = 0; i < strlen(button->caption); ++i )
		caption [i] = '*';

	caption [strlen(button->caption)] = '\x0';
	GciDrawText ( button->x + 10, button->y + 10, caption, BITMAP_15 );

	delete [] caption;

	// Draw a box around the text if highlighted

	if ( highlighted || clicked ) {

#ifndef HAVE_GLES
		glBegin ( GL_LINE_LOOP );

			glVertex2i ( button->x, button->y );
			glVertex2i ( button->x + button->width, button->y );
			glVertex2i ( button->x + button->width, button->y + button->height );
			glVertex2i ( button->x, button->y + button->height );

		glEnd ();
#else
		glEnableClientState(GL_VERTEX_ARRAY);
		glDrawArrays(GL_LINE_LOOP, 0, 4);
		glDisableClientState(GL_VERTEX_ARRAY);

#endif
	}

}

void PasswordScreenInterface::NextPage ()
{

	// Give the user max security clearance (only 1 access code for password screen)
	
	game->GetInterface ()->GetRemoteInterface ()->SetSecurity ( "Admin", 1 );

	// Add this into the computer's logs

	AccessLog *log = new AccessLog ();
	log->SetProperties ( &(game->GetWorld ()->date), 
						 game->GetWorld ()->GetPlayer ()->GetConnection ()->GetGhost (), "PLAYER" );
	log->SetData1 ( "Password authentication accepted" );
	GetComputerScreen ()->GetComputer ()->logbank.AddLog ( log );

	if ( GetComputerScreen ()->nextpage != -1 )
		game->GetInterface ()->GetRemoteInterface ()->RunScreen ( GetComputerScreen ()->nextpage, GetComputerScreen ()->GetComputer () );

}

bool PasswordScreenInterface::ReturnKeyPressed ()
{

	return false;

}

void PasswordScreenInterface::Create ( ComputerScreen *newcs )
{

	UplinkAssert ( newcs );
	cs = newcs;

	if ( !IsVisible () ) {

		EclRegisterButton ( 150, 120, 350, 25, GetComputerScreen ()->maintitle, "", "passwordscreen_maintitle" );
		EclRegisterButtonCallbacks ( "passwordscreen_maintitle", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButton ( 150, 140, 350, 20, GetComputerScreen ()->subtitle, "", "passwordscreen_subtitle" );
		EclRegisterButtonCallbacks ( "passwordscreen_subtitle", DrawSubTitle, NULL, NULL, NULL );
		
		EclRegisterButton ( 168, 185, 220, 110, "", "", "passwordscreen_message" );
		button_assignbitmap ( "passwordscreen_message", "passwordscreen.tif" );

		EclRegisterButton ( 206, 251, 145, 14, "", "Target this password box", "passwordscreen_password" );
		EclRegisterButtonCallbacks ( "passwordscreen_password", CodeButtonDraw, PasswordClick, button_click, button_highlight );
		
		EclMakeButtonEditable ( "passwordscreen_password" );

		// Create the box that will show the currently known codes for this screen
		UplinkAssert ( cs->GetComputer () );
		BTree <char *> *btree = game->GetWorld ()->GetPlayer ()->codes.LookupTree ( cs->GetComputer ()->ip );

		if ( btree ) {

			EclRegisterButton ( 200, 310, 250, 15, "Known Access Codes", "", "passwordscreen_codestitle" );
			EclRegisterButtonCallbacks ( "passwordscreen_codestitle", textbutton_draw, NULL, NULL, NULL );

			EclRegisterButton ( 200, 330, 250, 15, btree->data, "Use this code", "passwordscreen_codes" );
			EclRegisterButtonCallbacks ( "passwordscreen_codes", textbutton_draw, AccessCodeClick, button_click, button_highlight );

		}

#ifdef CHEATMODES_ENABLED
		// Create a symbol for quick entry into this screen
		EclRegisterButton ( 3, 20, 30, 15, "#", "Click here to bypass this screen (DEBUG MODE ONLY)", "passwordscreen_bypass" );
		EclRegisterButtonCallbacks ( "passwordscreen_bypass", textbutton_draw, BypassClick, button_click, button_highlight );
#endif						
		
	}

}

void PasswordScreenInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "passwordscreen_maintitle" );
		EclRemoveButton ( "passwordscreen_subtitle" );
		EclRemoveButton ( "passwordscreen_message" );
		EclRemoveButton ( "passwordscreen_password" );
		EclRemoveButton ( "passwordscreen_codestitle" );
		EclRemoveButton ( "passwordscreen_codes" );
	
#ifdef CHEATMODES_ENABLED
		EclRemoveButton ( "passwordscreen_bypass" );
#endif

	}

}

void PasswordScreenInterface::Update ()
{

	if ( game->GetWorld ()->GetPlayer ()->IsConnected () && 
	     strcmp ( EclGetButton ( "passwordscreen_password" )->caption, GetComputerScreen ()->password ) == 0 ) {

		SgPlaySound ( RsArchiveFileOpen ( "sounds/login.wav" ), "sounds/login.wav" );
		NextPage ();

	}

}

bool PasswordScreenInterface::IsVisible ()
{

	return ( EclGetButton ( "passwordscreen_message" ) != NULL );

}

int PasswordScreenInterface::ScreenID ()
{

	return SCREEN_PASSWORDSCREEN;

}

PasswordScreen *PasswordScreenInterface::GetComputerScreen ()
{

	UplinkAssert (cs);
	return (PasswordScreen *) cs;

}

