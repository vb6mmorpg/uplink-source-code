

#include "eclipse.h"
#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"

#include "game/game.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/messagescreen_interface.h"

#include "world/world.h"
#include "world/message.h"
#include "world/computer/computer.h"
#include "world/computer/computerscreen/messagescreen.h"

#include "mmgr.h"


void MessageScreenInterface::Click ( Button *button )
{

	int nextpage;
	char ip [ SIZE_VLOCATION_IP ] = {0};
	sscanf ( button->name, "messagescreen_click %d %s", &nextpage, ip );

	Computer *comp = NULL;
	if ( ip ) {
		VLocation *loc = game->GetWorld ()->GetVLocation ( ip );
		if ( loc )
			comp = loc->GetComputer ();
	}
	if ( nextpage != -1 ) game->GetInterface ()->GetRemoteInterface ()->RunScreen ( nextpage, comp );

}

void MessageScreenInterface::MailMeClick ( Button *button )
{

	MessageScreen *msi = (MessageScreen *) game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
	UplinkAssert (msi);

	Message *m = new Message ();
	m->SetTo ( "PLAYER" );
	m->SetFrom ( msi->GetComputer ()->name );	
	m->SetSubject ( msi->subtitle );
	m->SetBody ( msi->textmessage );
	m->Send ();

}

bool MessageScreenInterface::ReturnKeyPressed ()
{

	if ( GetComputerScreen ()->nextpage != -1 ) 
		game->GetInterface ()->GetRemoteInterface ()->RunScreen ( GetComputerScreen ()->nextpage, GetComputerScreen ()->GetComputer () );

	return true;

}

void MessageScreenInterface::Create ()
{

	if ( cs ) Create ( cs );
	else printf ( "MessageScreenInterface::Create, tried to create when MessageScreen==NULL\n" );

}

void MessageScreenInterface::Create ( ComputerScreen *newcs )
{

	UplinkAssert ( newcs );
	cs = newcs;

	if ( !IsVisible () ) {

		EclRegisterButton ( 80, 60, 350, 25, GetComputerScreen ()->maintitle, "", "messagescreen_maintitle" );
		EclRegisterButtonCallbacks ( "messagescreen_maintitle", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButton ( 80, 80, 350, 20, GetComputerScreen ()->subtitle, "", "messagescreen_subtitle" );
		EclRegisterButtonCallbacks ( "messagescreen_subtitle", DrawSubTitle, NULL, NULL, NULL );

		EclRegisterButton ( 50, 120, 400, 245, "", "", "messagescreen_message" );
		EclRegisterButtonCallbacks ( "messagescreen_message", textbutton_draw, NULL, NULL, NULL );
		EclRegisterCaptionChange   ( "messagescreen_message", GetComputerScreen ()->textmessage, 2000 );		

		if ( GetComputerScreen ()->buttonmessage ) {

			int width = (int) ( 20 + strlen (GetComputerScreen ()->buttonmessage) * 10 );
			char name [128 + SIZE_VLOCATION_IP + 1];
			UplinkSnprintf ( name, sizeof ( name ), "messagescreen_click %d %s", GetComputerScreen ()->nextpage, GetComputerScreen ()->GetComputer ()->ip );
			EclRegisterButton ( 320 - width/2, 370, width, 20, GetComputerScreen ()->buttonmessage, GetComputerScreen ()->buttonmessage, name );
			EclRegisterButtonCallback ( name, Click );

		}

		if ( GetComputerScreen ()->mailthistome ) {

			char name [128 + SIZE_VLOCATION_IP + 1];
			UplinkSnprintf ( name, sizeof ( name ), "messagescreen_click %d %s", GetComputerScreen ()->nextpage, GetComputerScreen ()->GetComputer ()->ip );
			Button *b = EclGetButton ( name );
			UplinkAssert (b);

			EclRegisterButton ( b->x - 120, b->y, 100, 20, "Mail this to me", "Click to send this information to yourself in an email", "messagescreen_mailme" );
			EclRegisterButtonCallback ( "messagescreen_mailme", MailMeClick );

		}

	}

}

void MessageScreenInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "messagescreen_message" );

		EclRemoveButton ( "messagescreen_maintitle" );
		EclRemoveButton ( "messagescreen_subtitle" );

		char name [128 + SIZE_VLOCATION_IP + 1];
		UplinkSnprintf ( name, sizeof ( name ), "messagescreen_click %d %s", GetComputerScreen ()->nextpage, GetComputerScreen ()->GetComputer ()->ip );
		EclRemoveButton ( name );

		EclRemoveButton ( "messagescreen_mailme" );

	}

}

bool MessageScreenInterface::IsVisible ()
{

	return ( EclGetButton ( "messagescreen_message" ) != NULL );

}

MessageScreen *MessageScreenInterface::GetComputerScreen ()
{

	UplinkAssert (cs);
	return (MessageScreen *) cs;

}

int MessageScreenInterface::ScreenID ()
{

	return SCREEN_MESSAGESCREEN;

}
