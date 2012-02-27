// LoginInterface.cpp: implementation of the LoginInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifndef HAVE_GLES
#include <GL/gl.h>
#include <GL/glu.h> /* glu extention library */
#else
#include <GLES/gl.h>
#include <GLES/glues.h>
#endif

#include "eclipse.h"
#include "soundgarden.h"
#include "redshirt.h"

#include "app/app.h"
#include "app/opengl_interface.h"
#include "app/globals.h"
#include "app/opengl.h"
#include "app/miscutils.h"

#include "game/game.h"
#include "game/scriptlibrary.h"

#include "interface/interface.h"
#include "interface/localinterface/localinterface.h"
#include "interface/remoteinterface/remoteinterface.h"

#include "network/network.h"
#include "network/networkclient.h"

#include "options/options.h"

#include "mainmenu/mainmenu.h"
#include "mainmenu/login_interface.h"

#include "mmgr.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LoginInterface::LoginInterface()
{

}

LoginInterface::~LoginInterface()
{

}

void LoginInterface::CreateExistingGames ()
{

	DArray <char *> *existing = App::ListExistingGames ();
	
	for ( int i = 0; i < existing->Size (); ++i ) {
		if ( existing->ValidIndex (i) ) {
			char *curAgent = existing->GetData ( i );

			char name [32];
			UplinkSnprintf ( name, sizeof ( name ), "username %d", i );
			EclRegisterButton ( SX(35), SY(300) + i * 18, SX(170), 15, curAgent, "Log in as this Agent", name );
			EclRegisterButtonCallbacks ( name, textbutton_draw, UserNameClick, button_click, button_highlight );

			delete [] curAgent;
		}
	}
			
	delete existing;

}

void LoginInterface::RemoveExistingGames ()
{

	// Remove all username buttons
	int usernameindex = 0;
	char name [32];
	UplinkSnprintf ( name, sizeof ( name ), "username %d", usernameindex );
	while ( EclGetButton ( name ) ) {
		EclRemoveButton ( name );
		++usernameindex;
		UplinkSnprintf ( name, sizeof ( name ), "username %d", usernameindex );			
	} 

}

void LoginInterface::NewGameClick ( Button *button )
{

	tooltip_update ( " " );

#ifndef TESTGAME

	// Do the start sequence

	app->GetOptions ()->SetOptionValue ( "game_firsttime", 1 ) ;
	
	app->GetMainMenu ()->RunScreen ( MAINMENU_FIRSTLOAD );
	GciTimerFunc ( 2000, ScriptLibrary::RunScript, 30 );

#else

	// Skip it

	app->GetMainMenu ()->RunScreen ( MAINMENU_LOADING );

	GciTimerFunc ( 1, ScriptLibrary::RunScript, 90 );

#endif

    if ( app->GetOptions ()->IsOptionEqualTo ( "sound_musicenabled", 1 ) ) {			
        //SgPlaylist_Play ("ambient");
        SgPlaylist_Play ("main");
	}

}

void LoginInterface::RetireAgentMsgboxClick ( Button *button )
{

	remove_msgbox ();

	UplinkAssert ( EclGetButton ( "userid_name" ) );
	char *agentfile = EclGetButton ( "userid_name" )->caption;

	if ( strlen ( agentfile ) > 0 ) {
		app->RetireGame ( agentfile );
		RemoveExistingGames ();
		CreateExistingGames ();
	}

}

void LoginInterface::RetireAgentClick ( Button *button )
{

	UplinkAssert ( EclGetButton ( "userid_name" ) );
	char agentfile [256];
	strncpy ( agentfile, EclGetButton ( "userid_name" )->caption, sizeof ( agentfile ) );
	agentfile [ sizeof ( agentfile ) - 1 ] = '\0';

	bool found = false;
	if ( strlen ( agentfile ) > 0 ) {
		DArray <char *> *existing = App::ListExistingGames ();

		int lenexisting = existing->Size ();
		for ( int i = 0; i < lenexisting; i++ )
			if ( existing->ValidIndex ( i ) ) {
				char *curAgent = existing->GetData ( i );
				if ( strcmp ( curAgent, agentfile ) == 0 )
					found = true;

				delete [] curAgent;
			}

		delete existing;
	}

	char caption [384];
	if ( found ) {
		UplinkSnprintf ( caption, sizeof ( caption ), "Are you sure you want to remove '%s' from the roster?", agentfile );
		create_yesnomsgbox ( "Retire", caption, RetireAgentMsgboxClick );
	}
	else {
		UplinkSnprintf ( caption, sizeof ( caption ), "Impossible to remove '%s' from the roster.", agentfile );
		create_msgbox ( "Retire", caption );
	}

}

void LoginInterface::ProceedClick ( Button *button )
{

	UplinkAssert ( EclGetButton ( "userid_name" ) );	
	char username [256];
	UplinkStrncpy ( username, EclGetButton ( "userid_name" )->caption, sizeof ( username ) );

	app->GetMainMenu ()->RunScreen ( MAINMENU_LOADING );

	app->SetNextLoadGame ( username );

	GciTimerFunc ( 1, ScriptLibrary::RunScript, 91 );

    if ( app->GetOptions ()->IsOptionEqualTo ( "sound_musicenabled", 1 ) ) {		
        //SgPlaylist_Play ("ambient");
        SgPlaylist_Play ("main");
	}

}

void LoginInterface::OptionsClick ( Button *button )
{

	app->GetMainMenu ()->RunScreen ( MAINMENU_OPTIONS );
//    SgPlaylist_NextSong();

}

void LoginInterface::ExitGameClick ( Button *button )
{

#ifdef DEMOGAME

    app->GetMainMenu ()->RunScreen ( MAINMENU_CLOSING );

#else

	app->Close ();

#endif

}

void LoginInterface::LargeTextBoxDraw ( Button *button, bool highlighted, bool clicked )
{

	SetColour ( "TitleText" );
	int ypos = (button->y + button->height / 2) + 5;

	GciDrawText ( button->x, ypos, button->caption, HELVETICA_18 );

}

void LoginInterface::UserIDButtonDraw ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert ( button );

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
	glEnable ( GL_SCISSOR_TEST );

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

	// Draw the text

	SetColour ( "DefaultText" );

	text_draw ( button, highlighted, clicked );

	// Draw a border

	if ( highlighted || clicked ) border_draw ( button );

	glDisable ( GL_SCISSOR_TEST );

}

void LoginInterface::CodeButtonDraw ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert (button);

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
	glEnable ( GL_SCISSOR_TEST );

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

	if ( highlighted || clicked )
		border_draw ( button );

	glDisable ( GL_SCISSOR_TEST );

}

void LoginInterface::UserNameClick ( Button *button )
{

	EclRegisterCaptionChange ( "userid_name", button->caption );
	EclRegisterCaptionChange ( "userid_code", "PASSWORD" );

}

void LoginInterface::CommsClick ( Button *button )
{

	UplinkAssert ( app->GetNetwork ()->STATUS == NETWORK_CLIENT );

    EclReset ( app->GetOptions ()->GetOptionValue ("graphics_screenwidth"),
			   app->GetOptions ()->GetOptionValue ("graphics_screenheight") );

	app->GetNetwork ()->GetClient ()->SetClientType ( CLIENT_COMMS );
	
}

void LoginInterface::StatusClick	( Button *button )
{

	UplinkAssert ( app->GetNetwork ()->STATUS == NETWORK_CLIENT );

    EclReset ( app->GetOptions ()->GetOptionValue ("graphics_screenwidth"),
			   app->GetOptions ()->GetOptionValue ("graphics_screenheight") );

	app->GetNetwork ()->GetClient ()->SetClientType ( CLIENT_STATUS );

}

bool LoginInterface::ReturnKeyPressed ()
{

	ProceedClick ( NULL );
	return true;

}

void LoginInterface::Create ()
{	

	if ( !IsVisible () ) {
		
		// Play some music

		if ( app->GetOptions ()->IsOptionEqualTo ( "sound_musicenabled", 1 ) ) {
			
            //SgPlaylist_Play ("action");
            SgPlaylist_Play ("main");

		}

		MainMenuScreen::Create ();

		int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
		int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");

		if ( app->GetNetwork ()->STATUS == NETWORK_CLIENT ) {

			EclRegisterButton ( 20, 20, 110, 20, "COMMS", "Run as a dumb client, showing the communications status", "client_comms" );
			EclRegisterButtonCallback ( "client_comms", CommsClick );
			EclRegisterButton ( 20, 50, 110, 20, "STATUS", "Run as a dumb client, showing your characters status", "client_status" );
			EclRegisterButtonCallback ( "client_status", StatusClick );

		}

		// New game

        int buttonsX = SX(600);
        int buttonsY = SY(270);

		EclRegisterButton ( buttonsX, buttonsY, 32, 32, "", "Create a new user's account", "newgame" );		
		button_assignbitmaps ( "newgame", "mainmenu/newgame.tif", "mainmenu/newgame_h.tif", "mainmenu/newgame_c.tif" );	
		EclRegisterButton ( buttonsX - 95, buttonsY + 5, 100, 20, "New User", "Create a new user's account", "newgame_text" );
		EclRegisterButtonCallbacks ( "newgame_text", LargeTextBoxDraw, NewGameClick, button_click, button_highlight );

		// Retire agent 

		EclRegisterButton ( buttonsX, buttonsY + 40, 32, 32, "", "Remove agents from the roster", "retireagent" );		
		button_assignbitmaps ( "retireagent", "mainmenu/loadgame.tif", "mainmenu/loadgame_h.tif", "mainmenu/loadgame_c.tif" );
		EclRegisterButton ( buttonsX - 68, buttonsY + 45, 50, 20, "Retire", "Remove an agent from the roster", "retireagent_text" );
		EclRegisterButtonCallbacks ( "retireagent_text", LargeTextBoxDraw, RetireAgentClick, button_click, button_highlight );

		// TODO : Look at this alpha stuff - it don't work no more
		if ( app->GetNetwork ()->STATUS == NETWORK_CLIENT ) {
			EclRegisterButtonCallbacks ( "newgame", imagebutton_draw, NULL, NULL, NULL );
			EclGetButton ( "newgame" )->image_standard->SetAlpha ( ALPHA_DISABLED );
			EclRegisterButtonCallbacks ( "retireagent", imagebutton_draw, NULL, NULL, NULL );
			EclGetButton ( "retireagent" )->image_standard->SetAlpha ( ALPHA_DISABLED );
		}
		else {
			EclRegisterButtonCallbacks ( "newgame", imagebutton_draw, NewGameClick, button_click, button_highlight );			
			EclRegisterButtonCallbacks ( "retireagent", imagebutton_draw, RetireAgentClick, button_click, button_highlight );
		}
		
		// Options

		EclRegisterButton ( buttonsX, buttonsY + 80, 32, 32, "", "Advanced options", "options" );		
		button_assignbitmaps ( "options", "mainmenu/options.tif", "mainmenu/options_h.tif", "mainmenu/options_c.tif" );
		EclRegisterButtonCallbacks ( "options", imagebutton_draw, OptionsClick, button_click, button_highlight ) ;

		EclRegisterButton ( buttonsX - 81, buttonsY + 85, 80, 20, "Options", "Advanced options", "options_text" );
		EclRegisterButtonCallbacks ( "options_text", LargeTextBoxDraw, OptionsClick, button_click, button_highlight );

		// Exit

		EclRegisterButton ( buttonsX, buttonsY + 120, 32, 32, "", "Exit Uplink", "exitgame" );		
		button_assignbitmaps ( "exitgame", "mainmenu/exitgame.tif", "mainmenu/exitgame_h.tif", "mainmenu/exitgame_c.tif" );
		EclRegisterButtonCallbacks ( "exitgame", imagebutton_draw, ExitGameClick, button_click, button_highlight );

		EclRegisterButton ( buttonsX - 45, buttonsY + 125, 50, 20, "Exit", "Exit Uplink", "exitgame_text" );
		EclRegisterButtonCallbacks ( "exitgame_text", LargeTextBoxDraw, ExitGameClick, button_click, button_highlight );

		// UserID box

        int useridX = SX(320) - 110;
        int useridY = SY(265);

		EclRegisterButton ( useridX, useridY, 220, 120, "", "", "userid_image" );
		button_assignbitmap ( "userid_image", "userid.tif" );		

		EclRegisterButton ( useridX + 59, useridY + 55, 147, 15, "", "Enter your userID here", "userid_name" );
		EclRegisterButtonCallbacks ( "userid_name", UserIDButtonDraw, NULL, button_click, button_highlight );

		EclRegisterButton ( useridX + 59, useridY + 81, 147, 15, "", "Enter your access code here", "userid_code" );
		EclRegisterButtonCallbacks ( "userid_code", CodeButtonDraw, NULL, button_click, button_highlight );
		
		EclRegisterButton ( useridX, useridY + 125, 120, 15, "", "", "userid_message" );
		EclRegisterButtonCallbacks ( "userid_message", textbutton_draw, NULL, NULL, NULL );

		EclRegisterButton ( useridX + 140, useridY + 125, 80, 15, "Proceed", "Click here when done", "userid_proceed" );
		button_assignbitmaps ( "userid_proceed", "proceed.tif", "proceed_h.tif", "proceed_c.tif" );
		EclRegisterButtonCallback ( "userid_proceed", ProceedClick );
		
		// Text help box

		EclRegisterButton ( useridX, SY(200), 240, SY(50), "", "", "texthelp" );
		EclRegisterButtonCallbacks ( "texthelp", textbutton_draw, NULL, NULL, NULL );
		EclRegisterCaptionChange ( "texthelp", "If you are registered as an Uplink Agent,\n"
											   "enter your username and password below.\n"
											   "Otherwise, click on the New User button." );

		// List of all existing games

		EclRegisterButton ( SX(35), SY(270), SX(170), SY(20), "Valid User Names", "", "usernames_title" );
		EclRegisterButtonCallbacks ( "usernames_title", LargeTextBoxDraw, NULL, NULL, NULL );

		CreateExistingGames ();

		EclMakeButtonEditable ( "userid_name" );
		EclMakeButtonEditable ( "userid_code" );
		
		// Version
       
		EclRegisterButton ( screenw - 8 * sizeof ( VERSION_NUMBER ), screenh - 15, 8 * sizeof ( VERSION_NUMBER ), 15, VERSION_NUMBER, "", "login_int_version" );
		EclRegisterButtonCallbacks ( "login_int_version", textbutton_draw, NULL, NULL, NULL );

	}

}

void LoginInterface::Remove ()
{

	if ( IsVisible () ) {

		MainMenuScreen::Remove ();

		if ( app->GetNetwork ()->STATUS == NETWORK_CLIENT ) {
			EclRemoveButton ( "client_comms" );
			EclRemoveButton ( "client_status" );
		}
		
		EclRemoveButton ( "newgame" );
		EclRemoveButton ( "newgame_text" );

		EclRemoveButton ( "retireagent" );
		EclRemoveButton ( "retireagent_text" );
		
		EclRemoveButton ( "options" );
		EclRemoveButton ( "options_text" );

		EclRemoveButton ( "exitgame" );
		EclRemoveButton ( "exitgame_text" );

		EclRemoveButton ( "userid_image" );
		EclRemoveButton ( "userid_name" );
		EclRemoveButton ( "userid_code" );
		EclRemoveButton ( "userid_message" );
		EclRemoveButton ( "userid_proceed" );

		EclRemoveButton ( "login_int_version" ); 

		EclRemoveButton ( "texthelp" );

		EclRemoveButton ( "usernames_title" );

		RemoveExistingGames ();

	}

}

void LoginInterface::Update ()
{
}

bool LoginInterface::IsVisible ()
{

	return ( EclGetButton ( "newgame" ) != NULL );

}

int LoginInterface::ScreenID ()
{

	return MAINMENU_LOGIN;

}
