
#ifdef WIN32
#include <windows.h>
#endif

#include <strstream>

#include <GL/gl.h>

#include <GL/glu.h> /* glu extention library */

#include <string.h>

#include "eclipse.h"
#include "soundgarden.h"
#include "redshirt.h"
#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/miscutils.h"
#include "app/opengl_interface.h"

#include "options/options.h"

#include "game/game.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/useridscreen_interface.h"
#include "interface/taskmanager/taskmanager.h"

#include "world/world.h"
#include "world/player.h"
#include "world/agent.h"
#include "world/computer/computer.h"
#include "world/computer/logbank.h"
#include "world/computer/recordbank.h"
#include "world/computer/computerscreen/useridscreen.h"

#include "mmgr.h"

void UserIDScreenInterface::BypassClick ( Button *button )
{

	// Give admin access

	game->GetInterface ()->GetRemoteInterface ()->SetSecurity ( "Admin", 1 );

	// Add this into the computer's logs

	AccessLog *log = new AccessLog ();
	log->SetProperties ( &(game->GetWorld ()->date),
						 game->GetWorld ()->GetPlayer ()->GetConnection ()->GetGhost (), "PLAYER" );
	log->SetData1 ( "User [Admin] logged on (level 1)" );

	UserIDScreen *uid = (UserIDScreen *) game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
	UplinkAssert (uid);
	uid->GetComputer ()->logbank.AddLog ( log );

	// Start tracing

//	game->GetWorld ()->GetPlayer ()->GetConnection ()->BeginTrace ();

	// Run the next screen

	if ( uid->nextpage != -1 )
		game->GetInterface ()->GetRemoteInterface ()->RunScreen ( uid->nextpage, uid->GetComputer () );

}

void UserIDScreenInterface::AccessCodeClick ( Button *button )
{

	UplinkAssert (button);

	char *fullcode = new char [strlen(button->caption) + 1];
	UplinkSafeStrcpy ( fullcode, button->caption );

	/*
		Access Codes

		Only 2 varieties are permitted:
		1.   ... 'name' .... 'code'
		2.   ... 'code' ....

		In other words, names/codes are always enclosed in '

		*/

	//
	// Count the number of dits
	//

	int numdits = 0;
	char *currentdit = fullcode;
	while ( strchr ( currentdit, '\'' ) != NULL ) {
		currentdit = strchr ( currentdit, '\'' ) + 1;
		++numdits;
	}

	//
	// Deal with each case
	//

	if ( numdits == 2 ) {

		char *code;

		code = strchr ( fullcode, '\'' ) + 1;
		*( strchr ( code, '\'' ) ) = '\x0';

		EclRegisterCaptionChange ( "useridscreen_name", "admin" );
		EclRegisterCaptionChange ( "useridscreen_code", code );

	}
	else if ( numdits == 4 ) {

		char *name;
		char *code;

		name = strchr ( fullcode, '\'' ) + 1;
		code = strchr ( name, '\'' ) + 1;
		code = strchr ( code, '\'' ) + 1;

		*( strchr ( name, '\'' ) ) = '\x0';
		*( strchr ( code, '\'' ) ) = '\x0';

		EclRegisterCaptionChange ( "useridscreen_name", name );
		EclRegisterCaptionChange ( "useridscreen_code", code );

	}
	else {

		UplinkWarning ( "Tried to use an invalid code" );

	}

	delete [] fullcode;

}

void UserIDScreenInterface::ProceedClick ( Button *button )
{

	// If disconnected which can happend when dialling.

	if ( !game->GetWorld ()->GetPlayer ()->IsConnected () )
		return;
	
	char *name = EclGetButton ( "useridscreen_name" )->caption;
	char *code = EclGetButton ( "useridscreen_code" )->caption;

	if ( strlen ( name ) >= SIZE_PERSON_NAME )
		name [ SIZE_PERSON_NAME - 1 ] = '\0';

	if ( strlen ( code ) >= 64 )
		code [ 64 - 1 ] = '\0';

	// Look up that code in this computer's records

	UserIDScreen *uid = (UserIDScreen *) game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
	UplinkAssert (uid);
	Computer *comp = uid->GetComputer ();
	UplinkAssert (comp);

	Record *rec = comp->recordbank.GetRecordFromNamePassword ( name, code );

	if ( rec ) {

        SgPlaySound ( RsArchiveFileOpen("sounds/login.wav"), "sounds/login.wav", false );

		// This is our code

		int security;
		char *securitytext = rec->GetField ( RECORDBANK_SECURITY );
		if ( securitytext )
			sscanf ( securitytext, "%d", &security );
		else
			security = 10;

		game->GetInterface ()->GetRemoteInterface ()->SetSecurity ( name, security );

		// Add this into the computer's logs

		AccessLog *log = new AccessLog ();
		char logdetails [256];
		UplinkSnprintf ( logdetails, sizeof ( logdetails ), "User [%s] logged on (level %d)", name, security );
		log->SetProperties ( &(game->GetWorld ()->date),
							 game->GetWorld ()->GetPlayer ()->GetConnection ()->GetGhost (), "PLAYER" );
		log->SetData1 ( logdetails );

		comp->logbank.AddLog ( log );

		if ( strcmp ( name, "admin" ) == 0 )
			game->GetWorld ()->GetPlayer ()->GetConnection ()->BeginTrace ();			// Always trace admin access

		if ( uid->nextpage != -1 )
			game->GetInterface ()->GetRemoteInterface ()->RunScreen ( uid->nextpage, uid->GetComputer () );

		return;

	}

	// If we get this far then the access code is not valid

	EclRegisterCaptionChange ( "useridscreen_message", "Invalid Name or Code" );

}

void UserIDScreenInterface::UserIDButtonDraw ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert ( button );

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );
	glEnable ( GL_SCISSOR_TEST );

	// Draw a background colour

	SetColour ( "PasswordBoxBackground" );

	glBegin ( GL_QUADS );

		glVertex2i ( button->x, button->y );
		glVertex2i ( button->x + button->width - 1, button->y );
		glVertex2i ( button->x + button->width - 1, button->y + button->height );
		glVertex2i ( button->x, button->y + button->height );

	glEnd ();

	// Draw the text

	SetColour ( "DefaultText" );

	text_draw ( button, highlighted, clicked );

	// Draw a border

	if ( highlighted || clicked ) border_draw ( button );

    glDisable ( GL_SCISSOR_TEST );

}

void UserIDScreenInterface::CodeButtonDraw ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert (button);

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );
	glEnable ( GL_SCISSOR_TEST );

	// Draw a background colour

    SetColour ( "PasswordBoxBackground" );

	glBegin ( GL_QUADS );

		glVertex2i ( button->x, button->y );
		glVertex2i ( button->x + button->width - 1, button->y );
		glVertex2i ( button->x + button->width - 1, button->y + button->height );
		glVertex2i ( button->x, button->y + button->height );

	glEnd ();

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

void UserIDScreenInterface::CodeButtonClick ( Button *button )
{

	UplinkAssert ( button );

	RemoteInterfaceScreen *ris = game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (ris);
	UserIDScreenInterface *uid = (UserIDScreenInterface *) ris;

	game->GetInterface ()->GetTaskManager ()->SetProgramTarget ( uid->GetComputerScreen (), "useridscreen_code", -1 );

}

void UserIDScreenInterface::DeleteCodeClick ( Button *button )
{

	UplinkAssert ( button );

	RemoteInterfaceScreen *ris = game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (ris);
	UserIDScreenInterface *uid = (UserIDScreenInterface *) ris;
	Computer *comp = uid->GetComputerScreen ()->GetComputer ();
	UplinkAssert (comp);

	int currentcode = 0;
	sscanf ( button->name, "useridscreen_deletecode %d", &currentcode );

	char name [64];
	UplinkSnprintf ( name, sizeof ( name ), "useridscreen_code %d", currentcode );

	Button *buttonCode = EclGetButton ( name );

	if ( buttonCode ) {

		bool firstime = true;

		BTree <char *> *treeCode = NULL;

		do {

			if ( firstime ) {
				firstime = false;
				treeCode = game->GetWorld ()->GetPlayer ()->codes.LookupTree ( comp->ip );
			} else if ( treeCode ){
				if ( treeCode->Left () )
					treeCode = treeCode->Left ()->LookupTree ( comp->ip );
				else
					treeCode = NULL;
			}

			if ( treeCode ) {

				char *code = treeCode->data;

				if ( !code )
					continue;

				if ( strcmp( code, buttonCode->caption ) != 0 )
					continue;

				// Parse the access code for this IP

				char username [256];
				char password [256];
				if ( !Agent::ParseAccessCode ( code, username, sizeof ( username ), password, sizeof ( password ) ) ||
					 !comp->recordbank.GetRecordFromNamePassword ( username, password ) ) {
					
					// This code is obsolete, remove it

					game->GetWorld ()->GetPlayer ()->codes.RemoveData ( comp->ip, code );
					delete [] code;

				}
				
				break;

			}

		} while ( treeCode );

	}

	// The code may be deleted, not obsolete anymore, not found
	// Create Known Access Codes anew

	uid->RemoveKnownAccessCodes();
	uid->CreateKnownAccessCodes();

}

bool UserIDScreenInterface::ReturnKeyPressed ()
{

	ProceedClick ( NULL );
	return true;

}

void UserIDScreenInterface::CreateKnownAccessCodes ()
{

	// Create the box that will show the currently known codes for this screen

	UplinkAssert ( cs->GetComputer () );

	if ( game->GetWorld ()->GetPlayer ()->codes.LookupTree ( cs->GetComputer ()->ip ) ) {

		DArray <char *> *codes = game->GetWorld ()->GetPlayer ()->codes.ConvertToDArray ();
		DArray <char *> *ips   = game->GetWorld ()->GetPlayer ()->codes.ConvertIndexToDArray ();

		EclRegisterButton ( 200, 310, 250, 15, "Known Access Codes", "", "useridscreen_codestitle" );
		EclRegisterButtonCallbacks ( "useridscreen_codestitle", textbutton_draw, NULL, NULL, NULL );

		int currentcode = 0;

		Image *iclose_tif = get_assignbitmap ( "close.tif" );
		Image *iclose_h_tif = get_assignbitmap ( "close_h.tif" );
		Image *iclose_c_tif = get_assignbitmap ( "close_c.tif" );

		for ( int i = 0; i < codes->Size (); ++i ) {
			if ( codes->ValidIndex (i) && ips->ValidIndex (i) ) {
				if ( strcmp ( ips->GetData (i), cs->GetComputer ()->ip ) == 0 ) {

					char name [64];
					UplinkSnprintf ( name, sizeof ( name ), "useridscreen_code %d", currentcode );
					EclRegisterButton ( 200, 330 + currentcode*15, 230, 15, codes->GetData (i), "Use this code", name );
					EclRegisterButtonCallbacks ( name, textbutton_draw, AccessCodeClick, button_click, button_highlight );

					// Give the player the option to delete obsolete codes
					// Only obsolete codes, less chance to break the game (codes are used in Agent::HasAccount)

					char username [256];
					char password [256];
					if ( !Agent::ParseAccessCode ( codes->GetData (i), username, sizeof ( username ), password, sizeof ( password ) ) ||
					     !cs->GetComputer ()->recordbank.GetRecordFromNamePassword ( username, password ) ) {

						UplinkSnprintf ( name, sizeof ( name ), "useridscreen_deletecode %d", currentcode );
						EclRegisterButton ( 200 + 230 + 2, 330 + currentcode*15, 13, 13, "", "Delete this obsolete code", name );
						button_assignbitmaps ( name, iclose_tif, iclose_h_tif, iclose_c_tif );
						EclRegisterButtonCallbacks ( name, imagebutton_draw, DeleteCodeClick, button_click, button_highlight );

					}

					++currentcode;

				}
			}
		}

		if ( iclose_tif )
			delete iclose_tif;
		if ( iclose_h_tif )
			delete iclose_h_tif;
		if ( iclose_c_tif )
			delete iclose_c_tif;

		delete codes;
		delete ips;

	}

}

void UserIDScreenInterface::RemoveKnownAccessCodes ()
{

	if ( EclGetButton ( "useridscreen_codestitle" ) ) {
		EclRemoveButton ( "useridscreen_codestitle" );
	}

	int currentcode = 0;
	char name [64];
	char name2 [64];
	UplinkSnprintf ( name, sizeof ( name ), "useridscreen_code %d", currentcode );
	UplinkSnprintf ( name2, sizeof ( name2 ), "useridscreen_deletecode %d", currentcode );

	while ( EclGetButton ( name ) ) {

		EclRemoveButton ( name );
		if ( EclGetButton ( name2 ) ) {
			EclRemoveButton ( name2 );
		}
		++currentcode;
		UplinkSnprintf ( name, sizeof ( name ), "useridscreen_code %d", currentcode );
		UplinkSnprintf ( name2, sizeof ( name2 ), "useridscreen_deletecode %d", currentcode );

	}

}

void UserIDScreenInterface::Create ( ComputerScreen *newcs )
{

	UplinkAssert ( newcs );
	cs = newcs;

	if ( !IsVisible () ) {

		EclRegisterButton ( 130, 90, 320, 25, GetComputerScreen ()->maintitle, "", "useridscreen_maintitle" );
		EclRegisterButtonCallbacks ( "useridscreen_maintitle", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButton ( 130, 110, 320, 20, GetComputerScreen ()->subtitle, "", "useridscreen_subtitle" );
		EclRegisterButtonCallbacks ( "useridscreen_subtitle", DrawSubTitle, NULL, NULL, NULL );

		EclRegisterButton ( 168, 155, 220, 120, "", "", "useridscreen_image" );
		button_assignbitmap ( "useridscreen_image", "userid.tif" );
		EclRegisterButtonCallbacks ( "useridscreen_image", imagebutton_draw, CodeButtonClick, button_click, button_highlight );

		EclRegisterButton ( 227, 210, 148, 15, "", "Enter your userID here", "useridscreen_name" );
		EclRegisterButtonCallbacks ( "useridscreen_name", UserIDButtonDraw, CodeButtonClick, button_click, button_highlight );

		EclRegisterButton ( 227, 236, 148, 15, "", "Enter your access code here", "useridscreen_code" );
		EclRegisterButtonCallbacks ( "useridscreen_code", CodeButtonDraw, CodeButtonClick, button_click, button_highlight );

		EclRegisterButton ( 168, 280, 120, 15, "", "", "useridscreen_message" );
		EclRegisterButtonCallbacks ( "useridscreen_message", textbutton_draw, NULL, NULL, NULL );

		EclRegisterButton ( 308, 280, 80, 15, "Proceed", "Click here when done", "useridscreen_proceed" );
		button_assignbitmaps ( "useridscreen_proceed", "proceed.tif", "proceed_h.tif", "proceed_c.tif" );
		EclRegisterButtonCallback ( "useridscreen_proceed", ProceedClick );

		EclMakeButtonEditable ( "useridscreen_name" );
		EclMakeButtonEditable ( "useridscreen_code" );

		CreateKnownAccessCodes();

#ifdef CHEATMODES_ENABLED
		// Create a symbol for quick entry into this screen
		EclRegisterButton ( 3, 20, 30, 15, "#", "Click here to bypass this screen (DEBUG MODE ONLY)", "useridscreen_bypass" );
		EclRegisterButtonCallbacks ( "useridscreen_bypass", textbutton_draw, BypassClick, button_click, button_highlight );
#endif

	}

}

void UserIDScreenInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "useridscreen_maintitle" );
		EclRemoveButton ( "useridscreen_subtitle" );

		EclRemoveButton ( "useridscreen_image" );
		EclRemoveButton ( "useridscreen_name" );
		EclRemoveButton ( "useridscreen_code" );

		EclRemoveButton ( "useridscreen_message" );
		EclRemoveButton ( "useridscreen_proceed" );

		RemoveKnownAccessCodes ();

#ifdef CHEATMODES_ENABLED
		EclRemoveButton ( "useridscreen_bypass" );
#endif

	}

}

void UserIDScreenInterface::Update ()
{

}

bool UserIDScreenInterface::IsVisible ()
{

	return ( EclGetButton ( "useridscreen_image" ) != NULL );

}

int UserIDScreenInterface::ScreenID ()
{

	return SCREEN_USERIDSCREEN;

}

UserIDScreen *UserIDScreenInterface::GetComputerScreen ()
{

	UplinkAssert (cs);
	return (UserIDScreen *) cs;

}

