// SendMailInterface.cpp: implementation of the SendMailInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "eclipse.h"
#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"

#include "game/game.h"

#include "options/options.h"

#include "interface/interface.h"
#include "interface/localinterface/localinterface.h"
#include "interface/localinterface/sendmail_interface.h"
#include "interface/taskmanager/taskmanager.h"

#include "world/world.h"
#include "world/message.h"
#include "world/player.h"
#include "world/company/mission.h"

	
Message *SendMailInterface::m = new Message();		// The message being built up
bool SendMailInterface::changed = false;	        // Has m changed?

// These have to be initialised above to stop them from being reset
// everytime a sendmailinterface is created

#include "mmgr.h"


SendMailInterface::SendMailInterface ()
{

	//if ( m )
	//	delete m;
	//m = new Message ();       // The message being built up
	//changed = false;          // Has m changed?

}

SendMailInterface::~SendMailInterface ()
{

	//if ( m ) {
	//	delete m;
	//	m = NULL;
	//}

}

Message *SendMailInterface::sGetMessage ()
{

    return m;

}

void SendMailInterface::TitleClick ( Button *button )
{

	game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_NONE );

}

void SendMailInterface::FileClick ( Button *button )
{

	ToggleFileList ( button->x - 130, button->y );

}

void SendMailInterface::AttachFileClick ( Button *button )
{

	// Update m with the captions which may have been changed

	UplinkAssert (m);

	m->SetTo      ( EclGetButton ( "sendmail_to" )->caption );
	m->SetFrom    ( "PLAYER" );
	m->SetSubject ( EclGetButton ( "sendmail_subject" )->caption );
	m->SetBody    ( EclGetButton ( "sendmail_body box" )->caption );


	// Check this is still in memory

	Data *data = NULL;
	DataBank *db = &(game->GetWorld ()->GetPlayer ()->gateway.databank);

	for ( int i = 0; i < db->GetDataSize (); ++i ) {

		if ( db->GetDataFile (i) &&
			strcmp ( db->GetDataFile (i)->title, button->caption ) == 0 ) {
			data = db->GetDataFile (i);
			break;
		}

	}
	
	if ( data ) {
	
		Data *datacopy = new Data ( data );
		m->AttachData ( datacopy );
		changed = true;

	}

	ToggleFileList ( 0, 0 );

}

bool SendMailInterface::IsVisibleFileList ()
{

	return ( EclGetButton ( "sendmail_file 0" ) != 0 );

}

void SendMailInterface::ToggleFileList ( int x, int y )
{

	if ( !IsVisibleFileList () ) {

		// Create a list of all files in memory

		LList <char *> software;				// Bit of a hack - think about this

		DataBank *db = &(game->GetWorld ()->GetPlayer ()->gateway.databank);

		for ( int di = 0; di < db->GetDataSize (); ++di ) {

			if ( db->GetDataFile (di) ) 
				software.PutData ( db->GetDataFile (di)->title );

		}

		int numfiles = software.Size ();

		if ( numfiles > 0 ) {

			int timesplit = 500 / numfiles;				// Ensures total time = 500ms

			for ( int si = 0; si < numfiles; ++si ) {

				char caption [128], tooltip [128], name [128];
				UplinkStrncpy ( caption, software.GetData (si), sizeof ( caption ) );
				UplinkStrncpy ( tooltip, "Attach this file to the mail message", sizeof ( tooltip ) );
				UplinkSnprintf ( name, sizeof ( name ), "sendmail_file %d", si );
				EclRegisterButton ( x, y, 120, 15, caption, tooltip, name );
				EclRegisterButtonCallbacks ( name, button_draw, AttachFileClick, button_click, button_highlight );
				EclRegisterMovement ( name, x, y - si * 17, si * timesplit );

			}

		}
		else {

			EclRegisterButton ( x, y, 120, 15, "No files", "", "sendmail_file 0" );

		}

	}
	else {

		int index = 0;
		char bname [128];
		UplinkSnprintf ( bname, sizeof ( bname ), "sendmail_file %d", index );

		while ( EclGetButton ( bname ) ) {

			EclRemoveButton ( bname );
			++index;
			UplinkSnprintf ( bname, sizeof ( bname ), "sendmail_file %d", index );

		}

	}

}

void SendMailInterface::ToClick ( Button *button )
{

	ToggleAddressBook ( button->x - 150, button->y );

}

bool SendMailInterface::IsVisibleAddressBook ()
{
	
	return ( EclGetButton ( "sendmail_addressbook 0" ) != 0 );

}

void SendMailInterface::ToggleAddressBook ( int x, int y )
{

	if ( !IsVisibleAddressBook () ) {

		// Create address book
		int numcontacts = game->GetWorld ()->GetPlayer ()->missions.Size ();
		
		if ( numcontacts > 0 ) {

			int timesplit = 500 / numcontacts;				// Ensures total time = 500ms

			for ( int i = 0; i < numcontacts; ++i ) {

				char name [64];
				UplinkSnprintf ( name, sizeof ( name ), "sendmail_addressbook %d", i );

				char *contact = game->GetWorld ()->GetPlayer ()->missions.GetData (i)->contact;

				EclRegisterButton ( x, y, 140, 15, contact, "Send to this recipient", name );
				EclRegisterButtonCallbacks ( name, button_draw, AddressClick, button_click, button_highlight );
				EclRegisterMovement ( name, x, y + i * 20, i * timesplit );

			}

		}
		else {

			EclRegisterButton ( x, y, 140, 15, "No contacts", "", "sendmail_addressbook 0" );
	
		}

	}
	else {

		// Remove address book

		int index = 0;
		char bname [128];
		UplinkSnprintf ( bname, sizeof ( bname ), "sendmail_addressbook %d", index );

		while ( EclGetButton ( bname ) ) {

			EclRemoveButton ( bname );
			++index;
			UplinkSnprintf ( bname, sizeof ( bname ), "sendmail_addressbook %d", index );

		}

	}

}

void SendMailInterface::AddressClick ( Button *button )
{

	// Copy the address to the TO box
	EclRegisterCaptionChange ( "sendmail_to", button->caption );

	// Remove the address book
	ToggleAddressBook ( 0, 0 );

}

void SendMailInterface::ClearClick ( Button *button )
{
	
	if ( m ) {
		delete m;	
		m = new Message ();
		changed = true;
	}
	
}

void SendMailInterface::SendClick ( Button *button )
{
 
	UplinkAssert (m);

	// Update m with the captions which may have been changed

	char *sendmail_to = EclGetButton ( "sendmail_to" )->caption;

	if ( strlen( sendmail_to ) >= SIZE_PERSON_NAME )
		sendmail_to [ SIZE_PERSON_NAME - 1 ] = '\0';

	m->SetTo      ( sendmail_to );
	m->SetFrom    ( "PLAYER" );
	m->SetSubject ( EclGetButton ( "sendmail_subject" )->caption );
	m->SetBody    ( EclGetButton ( "sendmail_body box" )->caption );

	m->Send ();

	// We can't delete the old message m - it is in use elsewhere now
	m = new Message ();
	changed = true;
	game->GetInterface ()->GetLocalInterface ()->GetInterfaceScreen ()->Update ();

	game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_NONE );

}

void SendMailInterface::Create ()
{

	if ( !IsVisible () ) {

		int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
		int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");
		int paneltop = (int) ( 100.0 * ( (screenw * PANELSIZE) / 188.0 ) + 30 );
		int panelwidth = (int) ( screenw * PANELSIZE );

		int boxheight = screenh - ((paneltop + 3) + 85) - 90;

		LocalInterfaceScreen::CreateHeight ( 3 + (85 + boxheight + 25) + 15 + 3 );

		//EclRegisterButton ( screenw - panelwidth - 3, paneltop, panelwidth, 15, "SEND MAIL", "Remove the send mail screen", "sendmail_title" );
		EclRegisterButton ( screenw - panelwidth, paneltop + 3, panelwidth - 7, 15, "SEND MAIL", "Remove the send mail screen", "sendmail_title" );
		EclRegisterButtonCallback ( "sendmail_title", TitleClick );

		//EclRegisterButton ( screenw - panelwidth - 3, paneltop + 20, 50, 15, "To", "Show a list of known recipients", "sendmail_tto" );
		//EclRegisterButton ( screenw - panelwidth - 3, paneltop + 40, 50, 40, "Subject", "sendmail_tsubject" );
		EclRegisterButton ( screenw - panelwidth, (paneltop + 3) + 20, 50, 15, "To", "Show a list of known recipients", "sendmail_tto" );
		EclRegisterButton ( screenw - panelwidth, (paneltop + 3) + 40, 50, 40, "Subject", "sendmail_tsubject" );

		EclRegisterButtonCallbacks ( "sendmail_tto", button_draw, ToClick, button_click, button_highlight );
		EclRegisterButtonCallbacks ( "sendmail_tsubject", button_draw, NULL, NULL, NULL );

		//EclRegisterButton ( screenw - panelwidth + 49, paneltop + 20, panelwidth - 52, 15, "", "Enter recipient name here", "sendmail_to" );
		//EclRegisterButton ( screenw - panelwidth + 49, paneltop + 40, panelwidth - 52, 40, "", "Enter subject here", "sendmail_subject" );
		EclRegisterButton ( screenw - panelwidth + 49 + 3, (paneltop + 3) + 20, (panelwidth - 7) - 52, 15, "", "Enter recipient name here", "sendmail_to" );
		EclRegisterButton ( screenw - panelwidth + 49 + 3, (paneltop + 3) + 40, (panelwidth - 7) - 52, 40, "", "Enter subject here", "sendmail_subject" );

		EclRegisterButtonCallbacks ( "sendmail_to", textbox_draw, NULL, NULL, button_highlight );
		EclRegisterButtonCallbacks ( "sendmail_subject", textbox_draw, NULL, NULL, button_highlight );

		//int boxheight = screenh - (paneltop + 85) - 90;
		//create_stextbox ( screenw - panelwidth - 3, paneltop + 85, panelwidth, boxheight, "", "sendmail_body" );
		create_stextbox ( screenw - panelwidth, (paneltop + 3) + 85, (panelwidth - 7), boxheight, "", "sendmail_body" );
		EclRegisterButtonCallbacks ( "sendmail_body box", draw_stextbox, NULL, NULL, button_highlight );

		//EclRegisterButton ( screenw - panelwidth - 3, paneltop + 85 + boxheight + 5, panelwidth, 15, "File: None attached", "Attach a file to this email", "sendmail_file" );
		EclRegisterButton ( screenw - panelwidth, (paneltop + 3) + 85 + boxheight + 5, (panelwidth - 7), 15, "File: None attached", "Attach a file to this email", "sendmail_file" );
		EclRegisterButtonCallback ( "sendmail_file", FileClick );
		
		//int x1 = screenw - panelwidth - 2;
		//int x2 = x1 + panelwidth / 3 + 2;
        //int x3 = x2 + panelwidth / 3 + 2;
        //int y = paneltop + 85 + boxheight + 25;
        //int width = panelwidth / 3 - 2;

        int width = ((panelwidth - 7) - 4) / 3;
		int x1 = screenw - panelwidth;
        int x3 = (screenw - 7) - width;
		int x2 = x1 + width + ( (x3 - (x1 + width)) - width ) / 2;
        int y = (paneltop + 3) + 85 + boxheight + 25;

		EclRegisterButton ( x1, y, width, 15, "Close", "Close the sendmail screen", "sendmail_close" );
		EclRegisterButton ( x2, y, width, 15, "Clear", "Clear this screen", "sendmail_clear" );
		EclRegisterButton ( x3, y, width, 15, "Send", "Send this email", "sendmail_send" );

		EclRegisterButtonCallback ( "sendmail_close", TitleClick );
        EclRegisterButtonCallback ( "sendmail_send", SendClick );
		EclRegisterButtonCallback ( "sendmail_clear", ClearClick );

		EclMakeButtonEditable ( "sendmail_to" );
		EclMakeButtonEditable ( "sendmail_subject" );
		EclMakeButtonEditable ( "sendmail_body box" );

		changed = true;

	}

}

void SendMailInterface::Remove ()
{

	UplinkAssert (m);

	if ( IsVisible () ) {

		// Update m with the captions which may have been changed

		m->SetTo      ( EclGetButton ( "sendmail_to" )->caption );
		m->SetSubject ( EclGetButton ( "sendmail_subject" )->caption );
		m->SetBody    ( EclGetButton ( "sendmail_body box" )->caption );

		LocalInterfaceScreen::Remove ();

		EclRemoveButton ( "sendmail_title" );

		EclRemoveButton ( "sendmail_tto" );
		EclRemoveButton ( "sendmail_tsubject" );

		EclRemoveButton ( "sendmail_to" );
		EclRemoveButton ( "sendmail_subject" );		
		remove_stextbox ( "sendmail_body" );

		EclRemoveButton ( "sendmail_file" );
		EclRemoveButton ( "sendmail_close" );
        EclRemoveButton ( "sendmail_clear" );
		EclRemoveButton ( "sendmail_send" );

		if ( IsVisibleAddressBook () ) ToggleAddressBook ( 0, 0 );
		if ( IsVisibleFileList () ) ToggleFileList ( 0, 0 );

	}

}

void SendMailInterface::Update ()
{
	
	if ( changed && IsVisible () ) {
		
		// Update the interface buttons
		
		UplinkAssert (m);

		EclGetButton ( "sendmail_to" )->SetCaption ( m->to );
		EclGetButton ( "sendmail_subject" )->SetCaption ( m->GetSubject () );
		EclGetButton ( "sendmail_body box" )->SetCaption ( m->GetBody () );

		char attached [128];
		if ( m->GetData () ) {    UplinkSnprintf ( attached, sizeof ( attached ), "File: %s", m->GetData ()->title );
		} else {                  UplinkStrncpy ( attached, "File: None attached", sizeof ( attached ) );
		}
		EclGetButton ( "sendmail_file" )->SetCaption ( attached );
		
		changed = false;

	}

}

bool SendMailInterface::IsVisible ()
{

	return ( EclGetButton ( "sendmail_title" ) != NULL );

}

int SendMailInterface::ScreenID ()
{
	
	return SCREEN_SENDMAIL;

}
