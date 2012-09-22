// ClientConnection.cpp: implementation of the ClientConnection class.
//
//////////////////////////////////////////////////////////////////////

#include <strstream>

#include "gucci.h"
#include "eclipse.h"

#include "app/app.h"
#include "app/globals.h"

#include "game/game.h"
#include "game/data/data.h"

#include "world/world.h"
#include "world/player.h"
#include "world/vlocation.h"

#include "world/company/companyuplink.h"
#include "world/company/news.h"
#include "world/computer/computer.h"
#include "world/computer/bankaccount.h"
#include "world/computer/recordbank.h"
#include "world/computer/gateway.h"

#include "world/generator/recordgenerator.h"

#include "network/network.h"
#include "network/clientconnection.h"
#include "network/networkclient.h"

#include "mmgr.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ClientConnection::ClientConnection()
{

	socket = -1;

	clienttype = CLIENT_NONE;
	connectiontime = 0;
	previousupdate = 0;

	connectionsize = 0;
	traceprogress = 0;
	numlinks = 0;
	sentgateway = false;

	balance = 0;
	numaccounts = 0;
	UplinkStrncpy ( convictions, " ", sizeof ( convictions ) );

	hudupgrades = 0;
	cpumodemmemory = 0;
	numhwitems = 0;
	UplinkStrncpy ( ip, " ", sizeof ( ip ) );

}

ClientConnection::~ClientConnection()
{

}

void ClientConnection::SetSocket ( SOCKET newsocket )
{

	socket = newsocket;

}

void ClientConnection::SetClientType ( int newclienttype )
{

	clienttype = newclienttype;

}

void ClientConnection::SetConnectionTime ( int newtime )
{

	connectiontime = newtime;

}

int ClientConnection::TimeActive ()
{

	int now = (int)EclGetAccurateTime ();

	int active = now - connectiontime;

	return (int) (active/1000);

}

void ClientConnection::Handle_ClientCommsInterface ()
{

	Connection *conn = &(game->GetWorld ()->GetPlayer ()->connection);
	LList <char *> *links = &(game->GetWorld ()->GetPlayer ()->links);

	//
	// Update trace progress
	//

	if ( conn->traceprogress != traceprogress ) {

		std::ostrstream msgstream;
		msgstream << "CLIENTCOMMS-TRACEPROGRESS " << conn->traceprogress;
		msgstream << (char) 10 << (char) 13 << '#' << '\x0';

		char *message = msgstream.str ();

		int result = TcpSend ( socket, message, (unsigned int) ( strlen(message)+1 ), false, HFILE_ERROR );
		if ( result != TCP4U_SUCCESS ) 	app->GetNetwork ()->GetServer ()->clients.RemoveData (index);

		msgstream.rdbuf()->freeze( 0 );
		//delete [] msgstream.str ();

		traceprogress = conn->traceprogress;

	}

	//
	// Update gateway
	//

	if ( !sentgateway ){

	    std::ostrstream msgstream;

	    VLocation *vl = game->GetWorld ()->GetVLocation ( IP_LOCALHOST );
	    UplinkAssert (vl);
	    msgstream << "CLIENTCOMMS-IPNAME " << vl->ip
		      << " " << vl->x << " " << vl->y << " " << vl->computer;
	    msgstream << (char) 10 << (char) 13 << '#' << '\x0';

	    char *message = msgstream.str ();

	    int result = TcpSend ( socket, message, (unsigned int) ( strlen(message)+1 ), false, HFILE_ERROR );
	    if ( result != TCP4U_SUCCESS )	app->GetNetwork ()->GetServer ()->clients.RemoveData (index);

		msgstream.rdbuf()->freeze( 0 );
	    //delete [] msgstream.str ();

	    sentgateway = true;

	}

	//
	// Update known links
	//

	if ( links->Size () > numlinks ) {

	    // The player has added some new links
	    // Update the client

	    int numnewlinks = links->Size () - numlinks;

	    for ( int i = 0; i < numnewlinks; ++i ) {

		std::ostrstream msgstream;

		VLocation *vl = game->GetWorld ()->GetVLocation ( links->GetData(i) );
		UplinkAssert (vl);
		msgstream << "CLIENTCOMMS-IPNAME " << vl->ip
			  << " " << vl->x << " " << vl->y << " " << vl->computer;
		msgstream << (char) 10 << (char) 13 << '#' << '\x0';

		char *message = msgstream.str ();

		int result = TcpSend ( socket, message, (unsigned int) ( strlen(message)+1 ), false, HFILE_ERROR );
		if ( result != TCP4U_SUCCESS )	app->GetNetwork ()->GetServer ()->clients.RemoveData (index);

		msgstream.rdbuf()->freeze( 0 );
		//delete [] msgstream.str ();

	    }

	    numlinks = links->Size ();

	}
	else if ( links->Size () < numlinks ) {

	    // Player has deleted some links
	    // No need to update the client

	    numlinks = links->Size ();

	}

	//
	// Update connection
	//

	if ( conn->GetSize () != connectionsize ) {

		std::ostrstream msgstream;
		msgstream << "CLIENTCOMMS " << conn->GetSize () << " ";

		for ( int i = 0; i < conn->GetSize (); ++i ) {
			VLocation *vl = game->GetWorld ()->GetVLocation ( conn->vlocations [i] );
			UplinkAssert (vl);
			msgstream << vl->x << " " << vl->y << " " << vl->ip << " ";
		}

		msgstream << (char) 10 << (char) 13 << '#' << '\x0';

		char *message = msgstream.str ();

		int result = TcpSend ( socket, message, (unsigned int) ( strlen(message)+1 ), false, HFILE_ERROR );
		if ( result != TCP4U_SUCCESS ) 	app->GetNetwork ()->GetServer ()->clients.RemoveData (index);

		msgstream.rdbuf()->freeze( 0 );
		//delete [] msgstream.str ();


		connectionsize = conn->GetSize ();

	}

}

void ClientConnection::Handle_ClientStatusInterface ()
{

	Player *player = game->GetWorld ()->GetPlayer ();
	UplinkAssert (player);

	//
	// Send latest rating
	//

	if ( player->rating.uplinkrating != rating.uplinkrating ||
		 player->rating.neuromancerrating != rating.neuromancerrating ) {

		std::ostrstream msgstream;
		msgstream << "CLIENTSTATUS-RATING "
				  << "Uplink rating : " << player->rating.GetUplinkRating () << "\n"
				  << "Neuromancer   : " << player->rating.GetNeuromancerRating () << "\n"
				  << "Notoriety     : None";

		msgstream << (char) 10 << (char) 13 << '#' << '\x0';
		char *message = msgstream.str ();

		int result = TcpSend ( socket, message, (unsigned int) ( strlen(message)+1 ), false, HFILE_ERROR );
		if ( result != TCP4U_SUCCESS )	app->GetNetwork ()->GetServer ()->clients.RemoveData (index);

		msgstream.rdbuf()->freeze( 0 );
		//delete [] msgstream.str ();

		rating.uplinkrating = player->rating.uplinkrating;
		rating.neuromancerrating = player->rating.neuromancerrating;

	}

	//
	// Send latest balance
	//

	if ( player->GetBalance () != balance || player->accounts.Size () != numaccounts ) {

		std::ostrstream msgstream;
		msgstream << "CLIENTSTATUS-FINANCE "
				  << "Balance : " << player->GetBalance () << "c\n\n";

		for ( int i = 0; i < player->accounts.Size (); ++i ) {

			char *account = player->accounts.GetData (i);
			UplinkAssert (account);

			char ip [SIZE_VLOCATION_IP];
			char accno [16];
			sscanf ( account, "%s %s", ip, accno );
			BankAccount *ba = BankAccount::GetAccount ( ip, accno );
			UplinkAssert (ba);

			msgstream << "Account: " << accno << " Balance: " << ba->balance << "c\n";

		}

		msgstream << (char) 10 << (char) 13 << '#' << '\x0';
		char *message = msgstream.str ();

		int result = TcpSend ( socket, message, (unsigned int) ( strlen(message)+1 ), false, HFILE_ERROR );
		if ( result != TCP4U_SUCCESS )	app->GetNetwork ()->GetServer ()->clients.RemoveData (index);

		msgstream.rdbuf()->freeze( 0 );
		//delete [] msgstream.str ();


		balance = player->GetBalance ();
		numaccounts = player->accounts.Size ();

	}

	//
	// Send latest criminal record
	//

	Record *rec = RecordGenerator::GetCriminal ( "PLAYER" );
	char *crimrec = NULL;
	if ( rec ) {
		crimrec = rec->GetField ( "Convictions" );
		UplinkAssert (crimrec);
	}

	if ( crimrec && strcmp ( convictions, crimrec ) != 0 ) {

		std::ostrstream msgstream;
		msgstream << "CLIENTSTATUS-CRIMINAL "
				  << crimrec;

		msgstream << (char) 10 << (char) 13 << '#' << '\x0';
		char *message = msgstream.str ();

		int result = TcpSend ( socket, message, (unsigned int) ( strlen(message)+1 ), false, HFILE_ERROR );
		if ( result != TCP4U_SUCCESS )	app->GetNetwork ()->GetServer ()->clients.RemoveData (index);

		msgstream.rdbuf()->freeze( 0 );
		//delete [] msgstream.str ();

		UplinkStrncpy ( convictions, crimrec, sizeof ( convictions ) );

	}

	//
	// Send latest HUD upgrades
	//

	if ( player->gateway.GetHudUpgrades () != hudupgrades ) {

		std::ostrstream msgstream;
		msgstream << "CLIENTSTATUS-HUD ";

		if ( !(player->gateway.HasAnyHUDUpgrade ()) )								msgstream << "None\n";
		if ( player->gateway.HasHUDUpgrade ( HUDUPGRADE_MAPSHOWSTRACE ) )			msgstream << "Show trace on map\n";
		if ( player->gateway.HasHUDUpgrade ( HUDUPGRADE_CONNECTIONANALYSIS ) )		msgstream << "Connection analysis\n";
		if ( player->gateway.HasHUDUpgrade ( HUDUPGRADE_IRCCLIENT ) )	        	msgstream << "IRC Client\n";
        if ( player->gateway.HasHUDUpgrade ( HUDUPGRADE_LANVIEW ) )                 msgstream << "LAN View\n";

		msgstream << (char) 10 << (char) 13 << '#' << '\x0';
		char *message = msgstream.str ();

		int result = TcpSend ( socket, message, (unsigned int) ( strlen(message)+1 ), false, HFILE_ERROR );
		if ( result != TCP4U_SUCCESS )	app->GetNetwork ()->GetServer ()->clients.RemoveData (index);

		msgstream.rdbuf()->freeze( 0 );
		//delete [] msgstream.str ();

		hudupgrades = player->gateway.GetHudUpgrades ();

	}

	//
	// Send latest CPU modem memory data
	//

	if ( player->gateway.GetCPUSpeed () * player->gateway.GetBandwidth () * player->gateway.memorysize != cpumodemmemory ||
		 player->gateway.hardware.Size () != numhwitems ) {

		// Explanation of above formula
		// No need to store cpu, modem and mem seperately - multiply them together and compare
		// This will work pretty much all of the time;

		std::ostrstream msgstream;
		msgstream << "CLIENTSTATUS-HW ";
		msgstream << player->gateway.GetCPUSpeed () << "\n"
				  << player->gateway.modemtype << "\n"
				  << "Memory : " << player->gateway.memorysize << "\n";

		for ( int i = 0; i < player->gateway.hardware.Size (); ++i )
			if ( player->gateway.hardware.ValidIndex (i) )
				msgstream << player->gateway.hardware.GetData (i) << "\n";

		msgstream << (char) 10 << (char) 13 << '#' << '\x0';
		char *message = msgstream.str ();

		int result = TcpSend ( socket, message, (unsigned int) ( strlen(message)+1 ), false, HFILE_ERROR );
		if ( result != TCP4U_SUCCESS )	app->GetNetwork ()->GetServer ()->clients.RemoveData (index);

		msgstream.rdbuf()->freeze( 0 );
		//delete [] msgstream.str ();

		cpumodemmemory = player->gateway.GetCPUSpeed () * player->gateway.GetBandwidth () * player->gateway.memorysize;
		numhwitems = player->gateway.hardware.Size ();

	}

	//
	// Send latest location
	//

	if ( strcmp ( player->GetRemoteHost ()->ip, ip ) != 0 ) {

		std::ostrstream msgstream;
		msgstream << "CLIENTSTATUS-IP ";

		if ( player->IsConnected () ) {

			msgstream << "Connected to " << player->GetRemoteHost ()->ip << "\n"
					  << "(" << player->GetRemoteHost ()->GetComputer ()->name << ")\n";

		}
		else {

			msgstream << "Not connected";

		}

		msgstream << (char) 10 << (char) 13 << '#' << '\x0';
		char *message = msgstream.str ();

		int result = TcpSend ( socket, message, (unsigned int) ( strlen(message)+1 ), false, HFILE_ERROR );
		if ( result != TCP4U_SUCCESS )	app->GetNetwork ()->GetServer ()->clients.RemoveData (index);

		msgstream.rdbuf()->freeze( 0 );
		//delete [] msgstream.str ();

		UplinkStrncpy ( ip, player->GetRemoteHost ()->ip, sizeof ( ip ) );

	}

	//
	// Send latest news items
	//

	CompanyUplink *uplink = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (uplink);

	News *latest = uplink->GetNews (0);
	UplinkAssert (latest);

	if ( latest->date.After ( &lastnewsdate ) ) {

		// Find the first "new" news story

		int indexofnew = uplink->news.Size ();

		for ( int i = 1; i < uplink->news.Size (); ++i ) {
			if ( uplink->news.ValidIndex (i) ) {

				News *thisnews = uplink->news.GetData (i);
				UplinkAssert (thisnews);

				if ( ! (thisnews->date.After ( &lastnewsdate )) ) {
					indexofnew = i - 1;
					break;
				}

			}
		}

		// No need to send more than 7

		if ( indexofnew > 5 ) indexofnew = 5;

		// Send all new stories

		for ( int in = indexofnew; in >= 0; --in ) {

			News *thisnews = uplink->news.GetData (in);
			UplinkAssert (thisnews);

			std::ostrstream msgstream;
			msgstream << "CLIENTSTATUS-NEWS - " << thisnews->date.GetShortString () << ": " << thisnews->headline;
			msgstream << (char) 10 << (char) 13 << '#' << '\x0';
			char *message = msgstream.str ();

			int result = TcpSend ( socket, message, (unsigned int) ( strlen(message)+1 ), false, HFILE_ERROR );
			if ( result != TCP4U_SUCCESS )	app->GetNetwork ()->GetServer ()->clients.RemoveData (index);

			msgstream.rdbuf()->freeze( 0 );
			//delete [] msgstream.str ();

		}

		lastnewsdate.SetDate ( &(latest->date) );

	}

}


bool ClientConnection::Load ( FILE *file )
{
	// Not needed
	return true;
}

void ClientConnection::Save ( FILE *file )
{
	// Not needed
}

void ClientConnection::Print  ()
{

	// TODO: This is not right. Time is of type time_t(_cdecl *)(time_t *)
	printf ( "ClientConnection : Socket:%d, Type:%d, starttime:%d\n", socket, clienttype, time (NULL) );

}

void ClientConnection::Update ()
{

	if ( socket != -1 ) {

		// Read any input from the client

		char thebuffer [128] = "";
		UINT nLength = sizeof thebuffer;
		int result = TcpRecvUntilStr ( socket, thebuffer, &nLength, "#", 2, false, -1, HFILE_ERROR );

		switch ( result ) {

			case TCP4U_SOCKETCLOSED:
				app->GetNetwork ()->GetServer ()->clients.RemoveData (index);
				break;

			case TCP4U_OVERFLOW:
				UplinkWarning ( "Overflow on incoming data" );
				break;

			case TCP4U_ERROR:
				UplinkWarning ( "Error in incoming data" );
				break;

		}

		// Examine and act on the information

		if ( strcmp ( thebuffer, "" ) != 0 ) {

			char instruction [32];
			int data1;

			sscanf ( thebuffer, "%s %d", instruction, &data1 );

			if ( strcmp ( instruction, "SETCLIENTTYPE" ) == 0 ) {

				SetClientType ( data1 );
				printf ( "ClientConnection::Update, Successfully set CLIENTTYPE to %d\n", data1 );

			}
			else {

			    printf ( "ClientConnection::Update, incoming instruction not recognised\n" );
			    printf ( "Instruction: %s\n", instruction );
			    printf ( "Data : %d\n", data1 );

			}

		}


		// Send out the relevent data
		// (If a certain amount of time has passed and if a game is running)

		if ( game->GameSpeed () > GAMESPEED_PAUSED ) {

			if ( EclGetAccurateTime () - previousupdate > 1000 ) {

				if		( clienttype == CLIENT_COMMS )			Handle_ClientCommsInterface ();
				else if ( clienttype == CLIENT_STATUS )			Handle_ClientStatusInterface ();
				else
					UplinkWarning ( "Unrecognised Client Type" );


				previousupdate = (int)EclGetAccurateTime ();

			}

		}

	}

}

void ClientConnection::SetIndex ( int newindex )
{

	index = newindex;

}

char *ClientConnection::GetID ()
{

	return "CL_CON";

}
