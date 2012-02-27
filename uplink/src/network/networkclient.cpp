// NetworkClient.cpp: implementation of the NetworkClient class.
//
//////////////////////////////////////////////////////////////////////

#include <strstream>

#include "eclipse.h"
#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"

#include "options/options.h"

#include "mainmenu/mainmenu.h"

#include "network/network.h"
#include "network/networkclient.h"
#include "network/interfaces/networkscreen.h"
#include "network/interfaces/clientcommsinterface.h"
#include "network/interfaces/clientstatusinterface.h"

#include "world/vlocation.h"

#include "mmgr.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

NetworkClient::NetworkClient()
{

    socket = -1;

    clienttype = CLIENT_NONE;
    currentscreencode = CLIENT_NONE;
    screen = NULL;

}

NetworkClient::~NetworkClient()
{

    if ( screen ) delete screen;

}

bool NetworkClient::StartClient ( char *ip )
{

    // Establish a connection to the server at the specified ip

    unsigned short portnum = 31337;

    int result = TcpConnect ( &socket, ip, NULL, &portnum );

    if ( result != TCP4U_SUCCESS )
	return false;

    else
	return true;

}

bool NetworkClient::StopClient ()
{

    int result = TcpClose ( &socket );

    if ( result == TCP4U_SUCCESS )
	return true;

    else
	return false;

}

void NetworkClient::SetClientType ( int newtype )
{

    // Request this mode from the server

    char message [32];
    UplinkSnprintf ( message, sizeof ( message ), "SETCLIENTTYPE %d#", newtype );

    int result = TcpSend ( socket, message, sizeof(message), false, HFILE_ERROR );

    if ( result == TCP4U_SUCCESS ) {

	clienttype = newtype;
	RunScreen ( clienttype );

    }
    else
	printf ( "NetworkClient::SetClientType, failed to convince server to co-operate\n" );

}

int NetworkClient::InScreen ()
{

    return currentscreencode;

}

void NetworkClient::RunScreen ( int SCREENCODE )
{

    // Get rid of the current interface
    if ( screen ) {
	screen->Remove ();
	delete screen;
	screen = NULL;
    }

    currentscreencode = SCREENCODE;

    switch ( currentscreencode ) {


	case CLIENT_COMMS:			screen = new ClientCommsInterface ();				break;
	case CLIENT_STATUS:			screen = new ClientStatusInterface ();				break;

	case CLIENT_NONE:			return;
	default:
	    UplinkAbort ("Tried to create a screen with unknown SCREENCODE");

    }

    screen->Create ();

}

NetworkScreen *NetworkClient::GetNetworkScreen ()
{

    UplinkAssert (screen);
    return screen;

}

bool NetworkClient::Load ( FILE *file )
{
    // not needed
	return true;
}

void NetworkClient::Save ( FILE *file )
{
    // not needed
}

void NetworkClient::Print ()
{

    printf ( "NetworkClient : SOCKET:%d\n", socket );
    printf ( "\tcurrentscreen:%d\n", currentscreencode );

}

void NetworkClient::Handle_ClientCommsData ( char *buffer )
{

    std::istrstream msgstream ( buffer );

    char msgtype [64];
    msgstream >> msgtype;

    if ( strcmp ( msgtype, "CLIENTCOMMS" ) == 0 ) {

	// This is a list of IP's making up the players connection

	UplinkAssert ( screen );
	UplinkAssert ( screen->ScreenID () == CLIENT_COMMS );

	((ClientCommsInterface *) screen)->connection.Empty ();

	int numrelays;
	msgstream >> numrelays;

	for ( int i = 0; i < numrelays; ++i ) {

	    int x, y;
	    char *ip = new char [64];

	    msgstream >> x >> y >> ip ;
/*
	    if ( ! ((ClientCommsInterface *) screen)->locations.LookupTree (ip) ) {

		// We don't have this IP yet

		VLocation *vl = new VLocation ();
		vl->SetPLocation ( x, y );
		vl->SetIP ( ip );
		vl->SetComputer ( "Accessing..." );
		((ClientCommsInterface *) screen)->locations.PutData ( ip, vl );

	    }
*/
	    UplinkAssert ( ((ClientCommsInterface *) screen)->locations.LookupTree (ip) );

	    ((ClientCommsInterface *) screen)->connection.PutData ( ip );
	}

    }
    else if ( strcmp ( msgtype, "CLIENTCOMMS-TRACEPROGRESS" ) == 0 ) {

	// This is an update on the trace progress

	int traceprogress;
	msgstream >> traceprogress;
	((ClientCommsInterface *) screen)->traceprogress = traceprogress;

	printf ( "Traceprogress is now %d\n", traceprogress );

    }
    else if ( strcmp ( msgtype, "CLIENTCOMMS-IPNAME" ) == 0 ) {

	// This is the name of a computer at one of the IP's in your connection

	char ip [SIZE_VLOCATION_IP];
	int x;
	int y;

	char compname [SIZE_COMPUTER_NAME];

	msgstream  >> ip >> x >> y >> ws;

//	compname = buffer + strlen("CLIENTCOMMS_IPNAME") + strlen(ip) + 1;
//	*(compname + strlen(compname) - 2) = '\x0';

	msgstream.getline (compname, SIZE_COMPUTER_NAME );
	cout << "'" << compname << "'\n";

	BTree <VLocation *> *locations = & (((ClientCommsInterface *) screen)->locations);
	VLocation *vl = locations->GetData (ip);
	if ( vl ) {
	    vl->SetComputer ( compname );
	    vl->SetPLocation ( x, y );
	    ((ClientCommsInterface *) screen)->LayoutLabels();
	}
	else {

	    VLocation *vl = new VLocation ();
	    vl->SetPLocation ( x, y );
	    vl->SetIP ( ip );
	    vl->SetComputer ( compname );
	    ((ClientCommsInterface *) screen)->locations.PutData ( ip, vl );
	    ((ClientCommsInterface *) screen)->LayoutLabels();

	}

    }
    else {

	UplinkWarning ( "NetworkClient::Handle_ClientCommsData, received data but did not recognise it" );

    }

}

void NetworkClient::Handle_ClientStatusData ( char *buffer )
{

    std::istrstream msgstream ( buffer );

    char msgtype [64];
    msgstream >> msgtype;

    char *data = buffer + strlen (msgtype) + 1;
    *(data + strlen(data) - 2) = '\x0';

    if ( strcmp ( msgtype, "CLIENTSTATUS-NEWS" ) == 0 ) {

	((ClientStatusInterface *) screen)->AddNewsStory ( data );

    }
    else if ( strcmp ( msgtype, "CLIENTSTATUS-RATING" ) == 0 ) {

	((ClientStatusInterface *) screen)->SetRating ( data );

    }
    else if ( strcmp ( msgtype, "CLIENTSTATUS-FINANCE" ) == 0 ) {

	((ClientStatusInterface *) screen)->SetFinancial ( data );

    }
    else if ( strcmp ( msgtype, "CLIENTSTATUS-CRIMINAL" ) == 0 ) {

	((ClientStatusInterface *) screen)->SetCriminal ( data );

    }
    else if ( strcmp ( msgtype, "CLIENTSTATUS-HUD" ) == 0 ) {

	((ClientStatusInterface *) screen)->SetHUDUpgrades ( data );

    }
    else if ( strcmp ( msgtype, "CLIENTSTATUS-HW" ) == 0 ) {

	((ClientStatusInterface *) screen)->SetHardware ( data );

    }
    else if ( strcmp ( msgtype, "CLIENTSTATUS-IP" ) == 0 ) {

	((ClientStatusInterface *) screen)->SetConnection ( data );

    }
    else {

	UplinkWarning ( "NetworkClient::Handle_ClientStatusData, received data but did not recognise it" );

    }

}

void NetworkClient::Update ()
{

    // Check for input from server

    if ( socket != -1 ) {

	char buffer [512] = "";
	UINT nLength = sizeof buffer;

	int result = TcpRecvUntilStr ( socket, buffer, &nLength, "#", 2, false, -1, HFILE_ERROR );

	switch ( result ) {

	    case TCP4U_SOCKETCLOSED:
        EclReset ( app->GetOptions ()->GetOptionValue ("graphics_screenwidth"),
			       app->GetOptions ()->GetOptionValue ("graphics_screenheight") );
		socket = -1;
		app->GetNetwork ()->SetStatus ( NETWORK_NONE );
		app->GetMainMenu ()->RunScreen ( MAINMENU_NETWORKOPTIONS );
		return;

	    case TCP4U_OVERFLOW:
		UplinkAbort ("buffer overflow" );

	    case TCP4U_ERROR:
		UplinkAbort ( "Tcp4u Error occured" );

	};

	if ( strcmp ( buffer, "" ) != 0 ) {

	    // Deal with the input


	    if		( clienttype == CLIENT_COMMS )			Handle_ClientCommsData ( buffer );
	    else if ( clienttype == CLIENT_STATUS )			Handle_ClientStatusData ( buffer );
	    else if ( clienttype == CLIENT_NONE )			printf ( "Client type not set" );
	    else
		UplinkWarning ( "Unrecognised client type" );

	}

    }

    // Update interface

    if ( screen ) screen->Update ();

}

char *NetworkClient::GetID ()
{

    return "CLIENT";

}
