// NetworkOptionsInterface.cpp: implementation of the NetworkOptionsInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "eclipse.h"

#include "app/app.h"
#include "app/opengl_interface.h"
#include "app/globals.h"

#include "game/game.h"

#include "mainmenu/mainmenu.h"
#include "mainmenu/networkoptions_interface.h"

#include "network/network.h"
#include "network/clientconnection.h"

#include "options/options.h"

#include "mmgr.h"


int NetworkOptionsInterface::oldNetworkStatus = NETWORK_NONE;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NetworkOptionsInterface::NetworkOptionsInterface()
{

}

NetworkOptionsInterface::~NetworkOptionsInterface()
{

}

void NetworkOptionsInterface::CloseClick ( Button *button )
{

	app->GetMainMenu ()->RunScreen ( MAINMENU_OPTIONS );

}

void NetworkOptionsInterface::ServerButtonClick ( Button *button )
{

	if ( app->GetNetwork ()->STATUS == NETWORK_NONE ) {
		app->GetNetwork ()->StartServer ();

		if ( app->GetNetwork ()->STATUS == NETWORK_SERVER )
			app->GetNetwork ()->GetServer ()->Listen ();

	}
	else
		app->GetNetwork ()->StopServer ();

}

void NetworkOptionsInterface::ClientButtonClick ( Button *button )
{

	if ( app->GetNetwork ()->STATUS == NETWORK_SERVER ) {

		printf ( "NetworkOptionsInterface::ClientButtonClick, cannot connect to a server when already running as a server\n" );
		return;

	}

	if ( app->GetNetwork ()->STATUS == NETWORK_NONE ) {
		
		UplinkAssert ( EclGetButton ( "network_clienttarget" ) );
		app->GetNetwork ()->StartClient ( EclGetButton ( "network_clienttarget" )->caption );
	
	}
	else
		app->GetNetwork ()->StopClient ();

}

void NetworkOptionsInterface::StatusButtonDraw ( Button *button, bool highlighted, bool clicked )
{

	if ( app->GetNetwork ()->STATUS == NETWORK_NONE ) {

		button->SetCaption ( "Status : Not connected" );

	}
	else if ( app->GetNetwork ()->STATUS == NETWORK_CLIENT ) {

		button->SetCaption ( "Status : Connected to Server" );

	}
	else if ( app->GetNetwork ()->STATUS == NETWORK_SERVER ) {

		button->SetCaption ( "Status : Server Running" );

	}

	textbutton_draw ( button, highlighted, clicked );

}

void NetworkOptionsInterface::ServerButtonDraw ( Button *button, bool highlighted, bool clicked )
{

	if ( app->GetNetwork ()->STATUS == NETWORK_NONE ) {

		button->SetCaption ( "Start Server" );
		button_draw ( button, highlighted, clicked );

	}
	else if ( app->GetNetwork ()->STATUS == NETWORK_CLIENT ) {

		clear_draw ( button->x, button->y, button->width, button->height );

	}
	else if ( app->GetNetwork ()->STATUS == NETWORK_SERVER ) {

		button->SetCaption ( "Stop Server" );
		button_draw ( button, highlighted, clicked );

	}

}

void NetworkOptionsInterface::ClientButtonDraw ( Button *button, bool highlighted, bool clicked )
{

	if ( app->GetNetwork ()->STATUS == NETWORK_NONE ) {

		button->SetCaption ( "Connect to Server" );
		button_draw ( button, highlighted, clicked );

	}
	else if ( app->GetNetwork ()->STATUS == NETWORK_CLIENT ) {

		button->SetCaption ( "Disconnect" );
		button_draw ( button, highlighted, clicked );

	}
	else if ( app->GetNetwork ()->STATUS == NETWORK_SERVER ) {

		clear_draw ( button->x, button->y, button->width, button->height );

	}

}

bool NetworkOptionsInterface::ReturnKeyPressed ()
{

	return true;

}

void NetworkOptionsInterface::Create ()
{	

	// Switch on the listener

	if ( app->GetNetwork ()->STATUS == NETWORK_SERVER )
		app->GetNetwork ()->GetServer ()->Listen ();

	if ( !IsVisible () ) {

		MainMenuScreen::Create ();

		int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
		int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");

		// Title bar

		EclRegisterButton ( screenw - 240, screenh - 40, 230,15, "MULTIMONITOR options", "Close this options screen", "network_title" );
		EclRegisterButtonCallback ( "network_title", CloseClick );
		EclRegisterMovement ( "network_title", screenw - 240, screenh - 250, 500 );

		// Status of network

		EclRegisterButton ( screenw - 240, screenh - 40, 230, 30, "checking...", "", "network_details" );
		EclRegisterButtonCallbacks ( "network_details", textbutton_draw, NULL, NULL, NULL );	
		EclRegisterMovement ( "network_details", screenw - 240, screenh - 230, 500 );

		EclRegisterButton ( screenw - 240, screenh - 40, 230, 15, "checking...", "", "network_status" );
		EclRegisterButtonCallbacks ( "network_status", StatusButtonDraw, NULL, NULL, NULL );
		EclRegisterMovement ( "network_status", screenw - 240, screenh - 195, 500 );

		// Client / Server buttons

		EclRegisterButton ( screenw - 240, screenh - 40, 110, 30, "...", "...", "network_client" );
		EclRegisterButtonCallbacks ( "network_client", ClientButtonDraw, ClientButtonClick, button_click, button_highlight );
		EclRegisterMovement ( "network_client", screenw - 240, screenh - 170, 500 );

		EclRegisterButton ( screenw - 120, screenh - 40, 110, 30, "...", "...", "network_server" );
		EclRegisterButtonCallbacks ( "network_server", ServerButtonDraw, ServerButtonClick, button_click, button_highlight );
		EclRegisterMovement ( "network_server", screenw - 120, screenh - 170, 500 );

		// Client IP address

		EclRegisterButton ( screenw - 240, screenh - 40, 230, 15, "localhost", "", "network_clienttarget" );
		EclRegisterButtonCallbacks ( "network_clienttarget", textbutton_draw, NULL, NULL, button_highlight );
		EclRegisterMovement ( "network_clienttarget", screenw - 240, screenh - 135, 500 );
		EclMakeButtonEditable ( "network_clienttarget" );

		// Network connections

		EclRegisterButton ( screenw - 240, screenh - 40, 230, 60, "", "", "network_connections" );
		EclRegisterButtonCallbacks ( "network_connections", textbutton_draw, NULL, NULL, NULL );		
		EclRegisterMovement ( "network_connections", screenw - 240, screenh - 110, 500 );

		// Close button

		EclRegisterButton ( screenw - 60, screenh - 40, 50, 15, "Close", "Return to Options menu", "network_close" );
		EclRegisterButtonCallback ( "network_close", CloseClick );

	}

}
	
void NetworkOptionsInterface::Remove ()
{

	// Switch off the listener

	if ( app->GetNetwork ()->STATUS == NETWORK_SERVER )
		app->GetNetwork ()->GetServer ()->StopListening ();


	if ( IsVisible () ) {

		MainMenuScreen::Remove ();

		EclRemoveButton ( "network_title" );
		EclRemoveButton ( "network_close" );
		EclRemoveButton ( "network_status" );
		EclRemoveButton ( "network_details" );
		EclRemoveButton ( "network_server" );
		EclRemoveButton ( "network_client" );
		EclRemoveButton ( "network_clienttarget" );
		EclRemoveButton ( "network_connections" );

	}

}

void NetworkOptionsInterface::Update ()
{

	//
	// Update the network details
	//

	char details [512];
	UplinkSnprintf ( details, sizeof ( details ), "Localhost : %s\n"
												   "Local IP  : %s", app->GetNetwork ()->GetLocalHost (), 
																	 app->GetNetwork ()->GetLocalIP () );

	EclGetButton ( "network_details" )->SetCaption ( details );

	//
	// If network status has changed, refresh the buttons
	//

	if ( app->GetNetwork ()->STATUS != oldNetworkStatus ) {

		EclDirtyButton ( "network_status" );
		EclDirtyButton ( "network_client" );
		EclDirtyButton ( "network_server" );

		EclGetButton ( "network_connections" )->SetCaption ( " " );

		oldNetworkStatus = app->GetNetwork ()->STATUS;

	}

	//
	// Update current connections (if we are a server)
	//

	if ( app->GetNetwork ()->STATUS == NETWORK_SERVER ) {

		int i;
		
		char cdetails [512];
		UplinkSnprintf ( cdetails, sizeof ( cdetails ), "Active connections : %d", app->GetNetwork ()->GetServer ()->clients.NumUsed () );
		
		for ( i = 0; i < app->GetNetwork ()->GetServer ()->clients.Size (); ++i ) {
			if ( app->GetNetwork ()->GetServer ()->clients.ValidIndex (i) ) {

				char thisconnection [128];
				UplinkSnprintf ( thisconnection, sizeof ( thisconnection ), "\nFrom %s, ip %s, active %d secs", 
																			app->GetNetwork ()->GetServer ()->GetRemoteHost (i),
						  													app->GetNetwork ()->GetServer ()->GetRemoteIP (i),
																			app->GetNetwork ()->GetServer ()->clients.GetData (i)->TimeActive () );
				UplinkStrncat ( cdetails, sizeof ( cdetails ), thisconnection );

			}
		}
		
		EclGetButton ( "network_connections" )->SetCaption ( cdetails );

	}

}

bool NetworkOptionsInterface::IsVisible ()
{
	
	return ( EclGetButton ( "network_title" ) != NULL );

}

int NetworkOptionsInterface::ScreenID ()
{

	return MAINMENU_NETWORKOPTIONS;

}

