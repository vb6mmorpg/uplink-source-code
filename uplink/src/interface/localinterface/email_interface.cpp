// EmailInterface.cpp: implementation of the EmailInterface class.
//
//////////////////////////////////////////////////////////////////////

#include <strstream>

#include "gucci.h"
#include "soundgarden.h"
#include "redshirt.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"

#include "game/game.h"

#include "options/options.h"

#include "interface/interface.h"
#include "interface/localinterface/localinterface.h"
#include "interface/localinterface/email_interface.h"

#include "world/date.h"
#include "world/world.h"
#include "world/player.h"
#include "world/message.h"
#include "world/computer/computer.h"

#include "mmgr.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void EmailInterface::TitleClick ( Button *button )
{

	game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_NONE );

}

void EmailInterface::EmailClose ( Button *button )
{

	game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_NONE );

}

void EmailInterface::EmailReply ( Button *button )
{

	EmailInterface *em = (EmailInterface *) game->GetInterface ()->GetLocalInterface ()->GetInterfaceScreen ();
	UplinkAssert (em);
	UplinkAssert (em->message);

	// Get the mission at the supposed index

	Message *realmessage = game->GetWorld ()->GetPlayer ()->messages.GetData (em->index);

	// Check it matches (ie the player hasn't lost the email

	if ( em->message != realmessage ) 
		game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_NONE );


	char contact [SIZE_PERSON_NAME];
	UplinkStrncpy ( contact, em->message->from, sizeof ( contact ) );

	std::ostrstream body;
	body << "RE your email\n"
         << "'" 
         << em->message->GetSubject ()
         << "'"
         << "\n\n"
         << '\x0';

	game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_SENDMAIL );
	game->GetInterface ()->GetLocalInterface ()->Update ();

	EclRegisterCaptionChange ( "sendmail_to", contact, 1 );						// Should occur instantly
	EclRegisterCaptionChange ( "sendmail_subject", "RE your email" );
	EclRegisterCaptionChange ( "sendmail_body box", body.str (), 1000 );

	body.rdbuf()->freeze( 0 );
    //delete [] body.str ();

}

void EmailInterface::EmailDelete ( Button *button )
{

	EmailInterface *em = (EmailInterface *) game->GetInterface ()->GetLocalInterface ()->GetInterfaceScreen ();
	UplinkAssert (em);
	UplinkAssert (em->message);

	// Get the mission at the supposed index

	Message *realmessage = game->GetWorld ()->GetPlayer ()->messages.GetData (em->index);

	// Check it matches (ie the player hasn't lost the email

	if ( em->message == realmessage ) {

		Message *m = game->GetWorld ()->GetPlayer ()->messages.GetData (em->index);
		if (m) {
			delete m;
			game->GetWorld ()->GetPlayer ()->messages.RemoveData ( em->index );
		}

	}

	EmailClose ( button );

}

EmailInterface::EmailInterface()
{

	index = -1;
	timesync = 0;
	message = NULL;

}

EmailInterface::~EmailInterface()
{

}

void EmailInterface::SetMessage ( int newindex )
{

	index = newindex;
	message = game->GetWorld ()->GetPlayer ()->messages.GetData (index);
	UplinkAssert (message);

}

void EmailInterface::Create ()
{	

	UplinkAssert ( message );

	if ( !IsVisible () ) {

		int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
		int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");
		int paneltop = (int)(100.0 * ( screenw * PANELSIZE / 188.0 )) + 30;
		int panelwidth = (int)(screenw * PANELSIZE);

		int boxheight = screenh - ((paneltop + 3) + 110) - 70;

		LocalInterfaceScreen::CreateHeight ( 3 + (110 + boxheight + 5) + 15 + 3 );

		//EclRegisterButton ( screenw - panelwidth - 3, paneltop, panelwidth, 15, "EMAIL", "Remove the email screen", "email_title" );
		EclRegisterButton ( screenw - panelwidth, paneltop + 3, panelwidth - 7, 15, "EMAIL", "Remove the email screen", "email_title" );
		EclRegisterButtonCallback ( "email_title", TitleClick );
		
		char from [SIZE_PERSON_NAME + 8];		
		char date [SIZE_DATE_LONG + 8];
		size_t subjectsize = strlen(message->GetSubject()) + 12;
		char *subject = new char [subjectsize];
		std::ostrstream body;

		UplinkSnprintf ( from, sizeof ( from ), "From : %s", message->from );
		UplinkSnprintf ( date, sizeof ( date ), "Date : %s", message->date.GetShortString () );
		UplinkSnprintf ( subject, subjectsize, "Subject : %s", message->GetSubject () );

		body << message->GetBody ();

		// Concatenate any links onto the end of the message

		if ( message->links.Size () > 0 ) {
			
			body << "\n\nLinks included : \n";

			for ( int i = 0; i < message->links.Size (); ++i ) {
				
				VLocation *vl = game->GetWorld ()->GetVLocation ( message->links.GetData (i) );				

				if ( !vl ) {

					body << "- " << message->links.GetData (i) << "(Invalid)\n";

				}
				else {

					Computer *computer = game->GetWorld ()->GetComputer ( vl->computer );

					if ( !computer )
						body << "- " << message->links.GetData (i) << "(Invalid)\n";

					else 
						body << "- " << computer->name << "\n";					

				}

			}

		}

		// Concatenate any codes onto the end of the message

		if ( message->codes.Size () > 0 ) {
			
			DArray <char *> *darray = message->codes.ConvertToDArray ();
			DArray <char *> *darray_index = message->codes.ConvertIndexToDArray ();

			body << "\n\nCodes included : \n";

			for ( int i = 0; i < darray->Size (); ++i ) {
				
				UplinkAssert ( darray->ValidIndex ( i ) );

				VLocation *vl = game->GetWorld ()->GetVLocation ( darray_index->GetData (i) );				

				if ( !vl ) {
					
					body << "[" << message->links.GetData (i) << "(Invalid)]\n";					

				}
				else {

					Computer *computer = game->GetWorld ()->GetComputer ( vl->computer );

					if ( !computer ) 
						body << "[" << message->links.GetData (i) << "(Invalid)]\n";					
					
					else 
						body << "[" << computer->name << "]\n";
											
				}

				body << "- " << darray->GetData (i) << "\n";

			}

            delete darray;
            delete darray_index;

		}

		// Concatenate the data file onto the message if it exists

		Data *data = message->GetData ();

		if ( data ) {
	
			body << "\n\nData included : \n";
			body << data->title;			

		}

		body << '\x0';

		//EclRegisterButton ( screenw - panelwidth - 3, paneltop + 20, panelwidth, 30, "", "email_from" );
		//EclRegisterButton ( screenw - panelwidth - 3, paneltop + 55, panelwidth, 15, "", "email_date" );
		//EclRegisterButton ( screenw - panelwidth - 3, paneltop + 75, panelwidth, 30, "", "email_subject" );

		EclRegisterButton ( screenw - panelwidth, (paneltop + 3) + 20, (panelwidth - 7), 30, "", "email_from" );
		EclRegisterButton ( screenw - panelwidth, (paneltop + 3) + 55, (panelwidth - 7), 15, "", "email_date" );
		EclRegisterButton ( screenw - panelwidth, (paneltop + 3) + 75, (panelwidth - 7), 30, "", "email_subject" );

		EclRegisterButtonCallbacks ( "email_from", textbox_draw, NULL, NULL, NULL );
		EclRegisterButtonCallbacks ( "email_date", textbox_draw, NULL, NULL, NULL );
		EclRegisterButtonCallbacks ( "email_subject", textbox_draw, NULL, NULL, NULL );

		EclRegisterCaptionChange ( "email_from", from, 100 );
		EclRegisterCaptionChange ( "email_date", date, 100 );
		EclRegisterCaptionChange ( "email_subject", subject, 100 );

		//int boxheight = screenh - (paneltop + 110) - 70;
		//create_stextbox ( screenw - panelwidth - 3, paneltop + 110, panelwidth, boxheight, body.str (), "email_body" );
		create_stextbox ( screenw - panelwidth, (paneltop + 3) + 110, (panelwidth - 7), boxheight, body.str (), "email_body" );

		//int x1 = screenw - panelwidth - 2;
		//int x2 = x1 + panelwidth / 3 + 2;
		//int x3 = x2 + panelwidth / 3 + 2;
        //int y = paneltop + 110 + boxheight + 5;
        //int w = panelwidth / 3 - 2;

        int w = ((panelwidth - 7) - 4) / 3;
		int x1 = screenw - panelwidth;
        int x3 = (screenw - 7) - w;
		int x2 = x1 + w + ( (x3 - (x1 + w)) - w ) / 2;
        int y = (paneltop + 3) + 110 + boxheight + 5;

		EclRegisterButton ( x1, y, w, 15, "Close", "Close and store this email", "email_close" );
        EclRegisterButton ( x2, y, w, 15, "Reply", "Reply to this email", "email_reply" );
		EclRegisterButton ( x3, y, w, 15, "Delete", "Close and delete this email", "email_delete" );

		EclRegisterButtonCallback ( "email_close", EmailClose );
        EclRegisterButtonCallback ( "email_reply", EmailReply );
		EclRegisterButtonCallback ( "email_delete", EmailDelete );		

		delete [] subject;

		body.rdbuf()->freeze( 0 );
		//delete [] body.str ();

	}

}

void EmailInterface::Remove ()
{

	if ( IsVisible () ) {

		LocalInterfaceScreen::Remove ();

		EclRemoveButton ( "email_title" );
		EclRemoveButton ( "email_from" );
		EclRemoveButton ( "email_date" );
		EclRemoveButton ( "email_subject" );
		
		remove_stextbox ( "email_body" );

		EclRemoveButton ( "email_close" );
        EclRemoveButton ( "email_reply" );
		EclRemoveButton ( "email_delete" );

	}

}

void EmailInterface::Update ()
{

	if ( time(NULL) > timesync + 1 ) {

		// Get the mission at the supposed index

		Message *realmessage = game->GetWorld ()->GetPlayer ()->messages.GetData (index);

		// Check it matches (ie the player hasn't lost the email

		if ( message != realmessage ) 
			game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_NONE );

		timesync = time(NULL);

	}

}

bool EmailInterface::IsVisible ()
{

	return ( EclGetButton ("email_title") != 0 );

}

int EmailInterface::ScreenID ()
{

	return SCREEN_EMAIL;

}
