// -*- tab-width:4 c-file-style:"cc-mode" -*-

// WorldGenerator.cpp: implementation of the WorldGenerator class.
//
//////////////////////////////////////////////////////////////////////

#include <strstream>

#include "gucci.h"
#include "redshirt.h"

#include "game/game.h"
#include "game/data/data.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/miscutils.h"
#include "app/serialise.h"

#include "interface/interface.h"
#include "interface/localinterface/localinterface.h"
#include "interface/localinterface/hud_interface.h"
#include "interface/remoteinterface/remoteinterface.h"

#include "options/options.h"

#include "app/dos2unix.h"
#include "app/opengl_interface.h"

#include "world/world.h"
#include "world/player.h"
#include "world/vlocation.h"
#include "world/generator/worldgenerator.h"
#include "world/generator/missiongenerator.h"
#include "world/generator/recordgenerator.h"
#include "world/generator/namegenerator.h"
#include "world/generator/numbergenerator.h"
#include "world/generator/langenerator.h"
#include "world/company/companyuplink.h"
#include "world/company/sale.h"
#include "world/company/mission.h"
#include "world/computer/computer.h"
#include "world/computer/bankcomputer.h"
#include "world/computer/logbank.h"
#include "world/computer/recordbank.h"
#include "world/computer/securitysystem.h"
#include "world/computer/lancomputer.h"

#include "world/computer/computerscreen/genericscreen.h"
#include "world/computer/computerscreen/messagescreen.h"
#include "world/computer/computerscreen/menuscreen.h"
#include "world/computer/computerscreen/dialogscreen.h"
#include "world/computer/computerscreen/passwordscreen.h"
#include "world/computer/computerscreen/useridscreen.h"
#include "world/computer/computerscreen/logscreen.h"
#include "world/computer/computerscreen/bbsscreen.h"
#include "world/computer/computerscreen/linksscreen.h"
#include "world/computer/computerscreen/shareslistscreen.h"
#include "world/computer/computerscreen/cypherscreen.h"
#include "world/computer/computerscreen/highsecurityscreen.h"
#include "world/computer/computerscreen/disconnectedscreen.h"

#include "mmgr.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Image *WorldGenerator::worldmapmask = NULL;

void WorldGenerator::Initialise()
{

	worldmapmask = new Image ();
	char *filename;
	if ( game->GetWorldMapType () == Game::defconworldmap )
		filename = app->GetOptions ()->ThemeFilename ( "worldmaplarge_mask_defcon.tif" );
	else
		filename = app->GetOptions ()->ThemeFilename ( "worldmaplarge_mask_new.tif" );
	worldmapmask->LoadTIF ( RsArchiveFileOpen ( filename ) );
	worldmapmask->Scale ( VIRTUAL_WIDTH, VIRTUAL_HEIGHT );
	worldmapmask->FlipAroundH ();
    delete [] filename;

}

void WorldGenerator::Shutdown ()
{

    if ( worldmapmask ) delete worldmapmask;

}

void WorldGenerator::GenerateAll ()
{

	GenerateSpecifics ();
	GeneratePlayer ( "NEWAGENT" );
	GenerateRandomWorld ();								// Must come after GeneratePlayer
	LoadDynamics ();

}

void WorldGenerator::GenerateValidMapPos ( int &x, int &y )
{

	UplinkAssert (worldmapmask);

	/********** Start code by François Gagné **********/
	int retryDiffLoc = 0;
	DArray <VLocation *> *vls = game->GetWorld ()->locations.ConvertToDArray (); 
	/********** End code by François Gagné **********/

	while ( 1 ) {

		int tX = NumberGenerator::RandomNumber ( VIRTUAL_WIDTH - 1 );
		int tY = NumberGenerator::RandomNumber ( VIRTUAL_HEIGHT - 1 ) ;

		UplinkAssert ( tX >= 0 && tX < VIRTUAL_WIDTH );
		UplinkAssert ( tY >= 0 && tY < VIRTUAL_HEIGHT );

		if ( worldmapmask->GetPixelR ( tX, tY ) != 0 ) {

			/********** Start code by François Gagné **********/
			int found = 0;
			for ( int i = vls->Size () - 1; i >= 0; i-- ) {
				if ( vls->ValidIndex ( i ) ) {
					VLocation *vl = vls->GetData (i);

					//Don't put 2 locations in the same square
					if ( abs ( vl->x - tX ) < 2 && abs ( vl->y - tY ) < 2 ) {
						found = 1;
						break;
					}
				}
			}

			//If there is no free place on the map, put it anyway after ~32 tries
			if ( !found || retryDiffLoc >= 32 ) {
				x = tX;
				y = tY;

				delete vls;
				return;
			}

			retryDiffLoc++;
		}
	}
 
	delete vls;
	/********** End code by François Gagné **********/
}

void WorldGenerator::GenerateRandomWorld ()
{

    int i;

	// Generate some companies

	for ( i = 0; i < NUM_STARTING_COMPANIES; ++i )
		GenerateCompany ();

	// Generate some banks

	for ( i = 0; i < NUM_STARTING_BANKS; ++i )
		GenerateCompany_Bank ();

	// Generate some people

	for ( i = 0; i < NUM_STARTING_PEOPLE; ++i )
		GeneratePerson ();

	// Generate some agents

	for ( i = 0; i < NUM_STARTING_AGENTS; ++i )
		GenerateAgent ();

	// Generate some missions

	for ( i = 0; i < NUM_STARTING_MISSIONS; ++i )
		MissionGenerator::GenerateMission ();


	// Generate employment data

	DArray <Company *> *companies = game->GetWorld ()->companies.ConvertToDArray ();
	UplinkAssert (companies);

	for ( i = 0; i < companies->Size (); ++i ) {

		if ( companies->ValidIndex ( i ) ) {

			Company *company = companies->GetData (i);
			UplinkAssert (company);

			if ( strcmp ( company->name, "Government" ) != 0 &&
				 strcmp ( company->name, "Uplink" ) != 0 &&
				 strcmp ( company->name, "Player" ) != 0 ) {

				Person *ceo = GetRandomPerson ();
				Person *admin = GetRandomPerson ();

				UplinkAssert (ceo);
				UplinkAssert (admin);

				company->SetBoss ( ceo->name );
				company->SetAdmin ( admin->name );

			}
		}

	}

    delete companies;

}

void WorldGenerator::GenerateSimpleStartingMissionA ()
{

	//
	// Generate two very easy missions for the player at the start
	//

	Company *employer1 = NULL;
	Computer *target1 = NULL;

	while ( !employer1 || !target1 ||
		strcmp ( employer1->name, target1->companyname ) == 0 ) {

		employer1 = WorldGenerator::GetRandomCompany ();
		UplinkAssert (employer1);
		target1 = WorldGenerator::GetRandomLowSecurityComputer ( COMPUTER_TYPE_INTERNALSERVICESMACHINE );
		UplinkAssert (target1);

	}

	Mission *mission1 = MissionGenerator::Generate_StealSingleFile ( employer1, target1 );
	if ( mission1 ) mission1->SetMinRating ( 2 );


}

void WorldGenerator::GenerateSimpleStartingMissionB ()
{

	//
	// Generate a delete file mission
	//

	Company *employer2 = NULL;
	Computer *target2 = NULL;

	while ( !employer2 || !target2 ||
		strcmp ( employer2->name, target2->companyname ) == 0 ) {

		employer2 = WorldGenerator::GetRandomCompany ();
		UplinkAssert (employer2);
		target2 = WorldGenerator::GetRandomLowSecurityComputer ( COMPUTER_TYPE_INTERNALSERVICESMACHINE );
		UplinkAssert (target2);

	}

	Mission *mission2 = MissionGenerator::Generate_DestroySingleFile ( employer2, target2 );
	if ( mission2 ) mission2->SetMinRating ( 2 );

}

Player *WorldGenerator::GeneratePlayer ( char *handle )
{

	// Initialise the player
	Player *player = new Player ();

	player->SetName						( "PLAYER" );
	player->SetAge						( 21 );
	player->SetHandle					( handle );
 	player->SetLocalHost				( IP_LOCALHOST );
	player->SetIsTargetable				( false );
	player->rating.SetUplinkRating		( PLAYER_START_UPLINKRATING );
	player->rating.SetNeuromancerRating ( PLAYER_START_NEUROMANCERRATING );
	player->rating.SetCreditRating      ( PLAYER_START_CREDITRATING );
	player->SetCurrentAccount           ( 0 );

	player->gateway.GiveStartingHardware	();
	player->gateway.GiveStartingSoftware	();

	game->GetWorld ()->CreatePerson ( (Person *) player );

	player->GetConnection ()->Reset ();
	player->GetConnection ()->AddVLocation ( IP_UPLINKPUBLICACCESSSERVER );
	player->GetConnection ()->Connect ();

	player->GiveLink ( IP_UPLINKPUBLICACCESSSERVER );
	player->GiveLink ( IP_INTERNIC );

#ifdef TESTGAME
	player->GiveLink ( IP_GLOBALCRIMINALDATABASE );
	player->GiveLink ( IP_SOCIALSECURITYDATABASE );
	player->GiveLink ( IP_ACADEMICDATABASE );
//	player->GiveLink ( IP_CENTRALMEDICALDATABASE );
	player->GiveLink ( IP_STOCKMARKETSYSTEM );

    Computer *comp = GetRandomComputer ( COMPUTER_TYPE_LAN );
    player->GiveLink( comp->ip );

#endif

	return player;

}

void WorldGenerator::GenerateLocalMachine ()
{

	// Create a local machine for players to fall back to
	// Which can also act as a links screen
	//

	VLocation *localaddress = game->GetWorld ()->CreateVLocation ( IP_LOCALHOST, 284, 73 );
	Computer  *localmachine = game->GetWorld ()->CreateComputer ( "Gateway", "Player", IP_LOCALHOST );

	localaddress->SetListed ( false );
	localmachine->SetIsTargetable ( false );

	// Screen 0			:			Links

	LinksScreen *links = new LinksScreen ();
	links->SetScreenType ( LINKSSCREENTYPE_PLAYERLINKS );
	links->SetMainTitle ( "Gateway" );
	links->SetSubTitle ( "Click on a link to quick-connect to that site" );
	localmachine->AddComputerScreen ( links, 0 );

	// Screen 1			:			Connection disconnected locally

	DisconnectedScreen *msg1 = new DisconnectedScreen ();
	msg1->SetMainTitle ( "" );
	msg1->SetSubTitle ( "" );
	msg1->SetTextMessage ( "Connection terminated by Gateway" );
	msg1->SetNextPage ( 0 );
	localmachine->AddComputerScreen ( msg1, 1 );

	// Screen 2			:			Connection disconnected remotely (naughty!!)

	DisconnectedScreen *msg2 = new DisconnectedScreen ();
	msg2->SetMainTitle ( "" );
	msg2->SetSubTitle ( "" );
	msg2->SetTextMessage ( "Connection terminated by remote host" );
	msg2->SetNextPage ( 0 );
	localmachine->AddComputerScreen ( msg2, 2 );

	// Screen 3			:			Welcome to Gateway - time since last log in

	DialogScreen *dlg3 = new DialogScreen ();
	dlg3->SetMainTitle ( "Gateway" );
	dlg3->SetSubTitle ( "Log on complete" );
	dlg3->AddWidget ( "text2", WIDGET_CAPTION, 170, 210, 270, 15, "Welcome back to your Gateway", "" );
	dlg3->AddWidget ( "okbutton", WIDGET_NEXTPAGE, 300, 350, 50, 20, "OK", "Click to continue", 0, 0, NULL, NULL );
	localmachine->AddComputerScreen ( dlg3, 3 );


	// Screen 5			:			Welcome to Gateway

	DialogScreen *dlg5 = new DialogScreen ();
	localmachine->AddComputerScreen ( dlg5, 5 );

	// Screen 6						Successfully started

	DialogScreen *dlg6 = new DialogScreen ();
	dlg6->AddWidget ( "text2", WIDGET_CAPTION, 100, 210, 340, 15, "UPLINK Operating System successfully started on Gateway", "" );
	dlg6->AddWidget ( "okbutton", WIDGET_NEXTPAGE, 300, 350, 50, 20, "OK", "Click to continue", 7, 0, NULL, NULL );
	localmachine->AddComputerScreen ( dlg6, 6 );

	// Screen 7						All done!

	DialogScreen *dlg7 = new DialogScreen ();
	dlg7->SetMainTitle ( "Gateway" );
	dlg7->SetSubTitle ( "Set up complete" );
	dlg7->AddWidget ( "caption", WIDGET_CAPTION, 50, 120, 400, 280,
						   "Your Gateway computer has been successfully set up and will accept only your username and password "
						   "in the future.  When you next start Uplink on your home computer, you will be connected automatically "
						   "to this Gateway.\n\n"
						   "In your email you will find a message from Uplink Corporation, with some useful IP addresses.\n"
						   "You will also find the Uplink Test Mission, which will allow you to practice hacking.\n\n"
						   "WOULD YOU LIKE TO RUN THE TUTORIAL?\n"
						   "(Recommended for first time users)"
						   , "" );
	dlg7->AddWidget ( "runtutorial", WIDGET_SCRIPTBUTTON, 300, 350, 50, 20, "Yes", "Run the tutorial", 43, 0, NULL, NULL );
	dlg7->AddWidget ( "dontruntutorial", WIDGET_NEXTPAGE, 200, 350, 50, 20, "No", "Skip the tutorial and return to the main screen", 0, 0, NULL, NULL );
	localmachine->AddComputerScreen ( dlg7, 7 );

	// Screen 8						404 page not found error ;)

	MessageScreen *ms8 = new MessageScreen ();
	ms8->SetMainTitle ( "Error 404" );
	ms8->SetSubTitle ( "System not found" );
	ms8->SetTextMessage ( "The IP address you specified did not \nhave a valid computer host.\n\n"
						  "The system may be temporarily out of action\nand may return shortly." );
	ms8->SetButtonMessage ( "OK" );
	ms8->SetNextPage ( 0 );
	localmachine->AddComputerScreen ( ms8, 8 );

	// Screen 9						Cannot connect to remotehost - they are not externally targetable

	MessageScreen *ms9 = new MessageScreen ();
	ms9->SetMainTitle ( "Failed to establish connection" );
	ms9->SetSubTitle ( "External connections not permitted" );
	ms9->SetTextMessage ( "The computer you tried to access will not accept external connections.\n"
						  "Only connections from trusted internal systems will be allowed to connect.\n\n"
                          "If you wish to establish a connection to this system,\n"
                          "you must first route your connection through a trusted system owned by the same company." );
	ms9->SetButtonMessage ( "OK" );
	ms9->SetNextPage ( 0 );
	localmachine->AddComputerScreen ( ms9, 9 );

	// Screen 10					Welcome to your new gateway

	MessageScreen *ms10 = new MessageScreen ();
	ms10->SetMainTitle ( "Gateway" );
	ms10->SetSubTitle ( "Upgrade complete" );
	ms10->SetTextMessage ( "Congratulations agent - your are now using your new gateway and your old one is already "
						   "being dismantled.\n\n"
						   "If you view your hardware profile you should see the new gateway in place.\n"
						   "If you experience any problems with this new system, please get in contact with us at\n"
						   "Problems@UplinkCorporation.net." );
	ms10->SetButtonMessage ( "OK" );
	ms10->SetNextPage ( 0 );
	localmachine->AddComputerScreen ( ms10, 10 );

}

void WorldGenerator::GenerateSpecifics ()
{

	// Create yourself as a company
	//

	game->GetWorld ()->CreateCompany ( "Player" );


	// Generate some specific companies and computers
	//

	GenerateLocalMachine ();

	GenerateCompanyGovernment ();
	GenerateGlobalCriminalDatabase ();
	GenerateInternationalSocialSecurityDatabase ();
//	GenerateCentralMedicalDatabase ();
	GenerateGlobalIntelligenceAgency ();
	GenerateInternationalAcademicDatabase ();
	GenerateInterNIC ();
	GenerateStockMarket ();
	GenerateProtoVision ();

#ifndef DEMOGAME
    GenerateOCP ();
    GenerateSJGames ();
    GenerateIntroversion ();
#endif

	GenerateCompanyUplink ();

}

void WorldGenerator::LoadDynamics ()
{

    //
    // Load all LANs from file
    //

    DArray <char *> *files = ListDirectory( "data/lans/", ".txt" );

    for ( int k = 0; k< files->Size(); ++k ) {
        if ( files->ValidIndex(k) ) {

            char *filename = files->GetData(k);
            UplinkAssert (filename);
            LanGenerator::LoadLAN( filename );

        }
    }
	DeleteDArrayData ( files );
	delete files;

}

void WorldGenerator::LoadDynamicsGatewayDefs ()
{

    //
    // Load additional gateways from the directory
    //

    int numgateways = game->GetWorld()->gatewaydefs.Size();
    DArray <char *> *extraGateways = ListDirectory ( "data/gateways/", ".txt" );
    for ( int j = 0; j < extraGateways->Size(); ++j ) {
        if ( extraGateways->ValidIndex(j) ) {

            char *gatewayFilename = extraGateways->GetData(j);
	        char *rsFilename = RsArchiveFileOpen ( gatewayFilename );
	        UplinkAssert (rsFilename);
	        idos2unixstream thisFile ( rsFilename );

            GatewayDef *def = new GatewayDef ();
            def->LoadGatewayDefinition ( thisFile );

            char filename [256];
            char thumbnail [256];
            UplinkSnprintf ( filename, sizeof ( filename ), "gateway/%s", strrchr(gatewayFilename, '/')+1 );
            *(strchr( filename, '.' )) = '\x0';
            UplinkStrncpy ( thumbnail, filename, sizeof ( thumbnail ) );
            UplinkStrncat ( filename, sizeof ( filename ), ".tif" );
            UplinkStrncat ( thumbnail, sizeof ( thumbnail ), "_t.tif" );

            def->SetFilename ( filename );
            def->SetThumbnail ( thumbnail );

            game->GetWorld()->CreateGatewayDef ( def );

            thisFile.close ();
            RsArchiveFileClose ( extraGateways->GetData(j) );

        }
    }
    DeleteDArrayData ( extraGateways );
    delete extraGateways;

}

void WorldGenerator::GenerateCompanyUplink ()
{

	// Create the BIG company
	CompanyUplink *cu = new CompanyUplink ();
	game->GetWorld ()->CreateCompany ( cu );
	GenerateUplinkPublicAccessServer ();
	GenerateUplinkInternalServicesSystem ();
	GenerateUplinkTestMachine ();
	GenerateUplinkCreditsMachine ();
	Computer *ubank = GeneratePublicBankServer ( "Uplink" );
	ubank->SetIsTargetable ( false );

	Person *pinternal = game->GetWorld ()->CreatePerson ( "internal@Uplink.net", IP_UPLINKINTERNALSERVICES );
	pinternal->SetIsTargetable	( false );

	// Generate the software items for sale

	Sale *previoussale = NULL;

	for ( int is = 0; is < NUM_STARTINGSOFTWAREUPGRADES; ++is ) {

		if ( previoussale && strcmp ( previoussale->title, SOFTWARE_UPGRADES [is].name ) == 0 ) {

			// New version of an existing item

			previoussale->AddVersion ( SOFTWARE_UPGRADES [is].description,
									   SOFTWARE_UPGRADES [is].cost,
									   SOFTWARE_UPGRADES [is].size,
									   SOFTWARE_UPGRADES [is].data );

		}
		else {

			// New sale item

			Sale *sale = new Sale ();
			sale->SetTitle			( SOFTWARE_UPGRADES [is].name );
			sale->SetSaleTYPE		( SALETYPE_SOFTWARE );
			sale->SetSwhwTYPE		( SOFTWARE_UPGRADES [is].TYPE );

			sale->AddVersion		( " ", -1, -1, -1 );
			sale->AddVersion		( SOFTWARE_UPGRADES [is].description,
									  SOFTWARE_UPGRADES [is].cost,
									  SOFTWARE_UPGRADES [is].size,
									  SOFTWARE_UPGRADES [is].data );

			cu->CreateSWSale ( sale );

			previoussale = sale;

		}

	}

	// Generate the hardware items for sale

	for ( int ih = 0; ih < NUM_STARTINGHARDWAREUPGRADES; ++ih ) {

		if ( previoussale && strcmp ( previoussale->title, HARDWARE_UPGRADES [ih].name ) == 0 ) {

			// New version of an existing item

			previoussale->AddVersion ( HARDWARE_UPGRADES [ih].description,
									   HARDWARE_UPGRADES [ih].cost,
									   HARDWARE_UPGRADES [ih].size,
									   HARDWARE_UPGRADES [ih].data );

		}
		else {

			// New sale item

			Sale *sale = new Sale ();
			sale->SetTitle			( HARDWARE_UPGRADES [ih].name );
			sale->SetSaleTYPE		( SALETYPE_HARDWARE );
			sale->SetSwhwTYPE		( HARDWARE_UPGRADES [ih].TYPE );

			sale->AddVersion		( " ", -1, -1, -1 );
			sale->AddVersion		( HARDWARE_UPGRADES [ih].description,
									  HARDWARE_UPGRADES [ih].cost,
									  HARDWARE_UPGRADES [ih].size,
									  HARDWARE_UPGRADES [ih].data );

			cu->CreateHWSale ( sale );

			previoussale = sale;

		}

	}

}

void WorldGenerator::GenerateUplinkPublicAccessServer ()
{

	int x, y;
	GenerateValidMapPos ( x, y );
    //char *compName = strdup( NameGenerator::GeneratePublicAccessServerName("Uplink") );
	char computername[MAX_COMPUTERNAME];
	strncpy( computername, NameGenerator::GeneratePublicAccessServerName( "Uplink" ),
		     MAX_COMPUTERNAME );
	if ( computername[MAX_COMPUTERNAME - 1] != '\0' ) {
		computername[MAX_COMPUTERNAME - 1] = '\0';
	}

	game->GetWorld ()->CreateVLocation ( IP_UPLINKPUBLICACCESSSERVER, x, y );
    game->GetWorld ()->CreateComputer ( computername, "Uplink", IP_UPLINKPUBLICACCESSSERVER );

	Computer *comp = game->GetWorld ()->GetComputer ( computername );
	UplinkAssert (comp);
	comp->SetIsTargetable ( false );
    comp->SetTraceSpeed ( TRACESPEED_UPLINK_PUBLICACCESSSERVER );

    //delete compName;

	// Screen 0						Intro
	MessageScreen *msg1 = new MessageScreen ();
	msg1->SetMainTitle ( "Uplink" );
	msg1->SetSubTitle ( "About us" );
	msg1->SetNextPage ( 1 );
	msg1->SetButtonMessage ( "OK" );
	std::ostrstream body;
	body << "Welcome to the Uplink Public Access Server.\n\n"
			"Uplink Corporation maintains the largest list of freelance agents in the world, and we "
			"have operated for the last decade with a flawless record of satisfied customers and "
			"successful agents.  Our company acts as an anonymous job centre, bringing corporations "
			"together with agents who can work for them.  Our company also provides rental of essential "
			"gateway computers to all agents, which allow unparalleled security in a high risk environment."
			"\n\nYou are here because you wish to join this company." << '\x0';

	msg1->SetTextMessage ( body.str () );
	body.rdbuf()->freeze( 0 );
	//delete [] body.str ();
	comp->AddComputerScreen ( msg1, 0 );

	// Screen 1						Main menu

	MenuScreen *menu0 = new MenuScreen ();
	menu0->SetMainTitle ( "Uplink" );
	menu0->SetSubTitle ( "Public server Main Menu" );
	menu0->AddOption ( "About Us", "Find out who we are and what we do", 0 );
	menu0->AddOption ( "Register as an Agent", "Click to register yourself as a new Uplink Agent", 2 );
	comp->AddComputerScreen ( menu0, 1 );

	// Screen 2						Registration info

	DialogScreen *dlg2 = new DialogScreen ();
	dlg2->SetMainTitle ( "Uplink" );
	dlg2->SetSubTitle ( "Registration" );
	std::ostrstream body2;
	body2 << "Your Uplink membership package includes : \n\n"
			 "- A Gateway computer at a secure location.  You will connect to this machine from your home "
			 "computer when you are working for Uplink.  You can have it upgraded at a later stage if necessary.\n\n"
			 "- A low interest loan of " << PLAYER_START_BALANCE << " credits with Uplink International Bank, "
			 "to get you started.\n\n"
			 "- Access to our Bulletin Board system - the usual place for Uplink Agents to find work.\n\n"
			 "- You will be officially rated as an Uplink Agent, and we will monitor your progress.  As your "
			 "rating increases you will find new avenues of work become available." << '\x0';

	dlg2->AddWidget ( "caption1", WIDGET_CAPTION, 100, 110, 380, 270, body2.str (), "" );
	body2.rdbuf()->freeze( 0 );
	//delete [] body2.str ();
	dlg2->AddWidget ( "cancel", WIDGET_NEXTPAGE, 160, 380, 100, 20, "Cancel", "Cancel registration", 1, 0, NULL, NULL );
	dlg2->AddWidget ( "continue", WIDGET_NEXTPAGE, 270, 380, 100, 20, "Continue", "Continue registration", 3, 0, NULL, NULL );
	dlg2->SetReturnKeyButton ( "continue" );
	comp->AddComputerScreen ( dlg2, 2 );

	// Screen 3						Username / password

	DialogScreen *dlg3 = new DialogScreen ();
	dlg3->SetMainTitle ( "Uplink Registration" );
	dlg3->SetSubTitle ( "Create your agent profile" );
	dlg3->AddWidget ( "name",      WIDGET_BASIC, 80, 140, 170, 20, "Enter your name", "Enter the name you wish to use" );
	dlg3->AddWidget ( "password",  WIDGET_BASIC, 80, 170, 170, 20, "Enter your password", "You will need this to access the Uplink Services Machine" );
	dlg3->AddWidget ( "password2", WIDGET_BASIC, 80, 200, 170, 20, "Re-type your password", "For verification purposes" );

	dlg3->AddWidget ( "nametext",      WIDGET_TEXTBOX,     270, 140, 170, 20, "Fill this in", "Enter your name here" );
	dlg3->AddWidget ( "passwordtext",  WIDGET_PASSWORDBOX, 270, 170, 170, 20, "", "Enter your password here" );
	dlg3->AddWidget ( "passwordtext2", WIDGET_PASSWORDBOX, 270, 200, 170, 20, "", "Re-type your password here" );

	dlg3->AddWidget ( "moretext", WIDGET_CAPTION, 80, 270, 360, 100, "Uplink Corporation will not ask for any more personal details.  "
																	 "In the event of you being charged with illegal operations, our "
																	 "corporation will be forced to disavow all knowledge of your actions, "
																	 "however you will be safe from arrest as your real world address will never be stored.",
																	 "" );

	dlg3->AddWidget ( "continue", WIDGET_SCRIPTBUTTON, 270, 400, 100, 20, "Done", "Click here when finished", 33, -1, NULL, NULL );
	dlg3->SetReturnKeyButton ( "continue" );
	comp->AddComputerScreen ( dlg3, 3 );

	// Screen 4						World map - select nearby gateway

	GenericScreen *gs4 = new GenericScreen ();
	gs4->SetMainTitle ( "Uplink" );
	gs4->SetSubTitle ( "Local gateway selection" );
	gs4->SetScreenType ( SCREEN_NEARESTGATEWAY );

	gs4->SetNextPage ( 9 );        // code card id

	comp->AddComputerScreen ( gs4, 4 );

	// Screen 5						Introduction to your Gateway

	DialogScreen *dlg5 = new DialogScreen ();
	dlg5->SetMainTitle ( "Uplink Registration" );
	dlg5->SetSubTitle ( "Your Gateway computer" );
	std::ostrstream body3;
	body3 <<  "Registration is now taking place.\n\n"
			  "As part of your membership, we will assign you a Gateway computer system in your chosen "
			  "server room.  This will act as your jumping off point to the rest of the Net.  When you next "
			  "log in you will connect directly from your home computer to this gateway machine, and from there "
			  "to the rest of the world.\n\n"
			  "Should any of your actions be traced back to your Gateway, Uplink Corporation will disavow any "
			  "knowledge of your actions and will destroy your account.  Your Gateway machine will also be destroyed.\n\n"
			  "Rental of your Gateway computer will cost " << COST_UPLINK_PERMONTH << " credits a month." << '\x0';
	dlg5->AddWidget ( "caption1", WIDGET_CAPTION, 100, 130, 380, 270, body3.str (), "" );
	body3.rdbuf()->freeze( 0 );
	//delete [] body3.str ();

#ifdef TESTGAME
	dlg5->AddWidget ( "continue", WIDGET_SCRIPTBUTTON, 270, 380, 100, 20, "Done", "Click here when finished", 36, 6, NULL, NULL );
#else
    dlg5->AddWidget ( "continue", WIDGET_SCRIPTBUTTON, 270, 380, 100, 20, "Done", "Click here when finished", 34, 6, NULL, NULL );
#endif

	dlg5->SetReturnKeyButton ( "continue" );
	comp->AddComputerScreen ( dlg5, 5 );

	// Screen 6						Connecting to Gateway

	DialogScreen *dlg6 = new DialogScreen ();

	int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
	int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");

	dlg6->AddWidget ( "connecting", WIDGET_CAPTION, screenw - 370, screenh - 30, 370, 20, "", "" );
	comp->AddComputerScreen ( dlg6, 6 );

    // Screen 9                     Code card verification

	GenericScreen *gs9 = new GenericScreen ();
	gs9->SetMainTitle ( "Uplink" );
	gs9->SetSubTitle ( "Code card verification" );
	gs9->SetScreenType ( SCREEN_CODECARD );
	gs9->SetNextPage ( 5 );
	comp->AddComputerScreen ( gs9, 9 );

	// Screen 10					Your Gateway is Gone!

	DialogScreen *dlg10 = new DialogScreen ();
	dlg10->SetMainTitle ( "Uplink" );
	dlg10->SetSubTitle ( "Gateway connection failed" );
	dlg10->AddWidget ( "caption1", WIDGET_CAPTION, 100, 130, 400, 270,
							"We have been unable to connect you to your Gateway computer.\n\n"
							"This may be due to a fault in out communications lines, and if this is the case then we "
							"apologise and ask you to try again later.\n\n"
							"We are aware that Uplink Agents occasionally suffer 'difficulties' with their Gateway "
							"systems and sometimes require a new one as a result.  If this is the case, click YES below "
							"and we will assign you a new gateway.  (There will be an administrative charge of 1000 credits "
							"if you take this action.)  Otherwise, click NO to log off and try again later.\n\n"
							"Rent a new Gateway computer?", "", 0, 0, NULL, NULL );
	dlg10->AddWidget ( "yes", WIDGET_SCRIPTBUTTON, 170, 380, 100, 20, "YES", "Click here to rent a new Gateway", 50, 4, NULL, NULL );
	dlg10->AddWidget ( "no", WIDGET_SCRIPTBUTTON, 300, 380, 100, 20, "NO", "Click here to log out", 51, -1, NULL, NULL );
	comp->AddComputerScreen ( dlg10, 10 );

}

void WorldGenerator::GenerateUplinkInternalServicesSystem ()
{

	int x, y;
	GenerateValidMapPos ( x, y );
	game->GetWorld ()->CreateVLocation ( IP_UPLINKINTERNALSERVICES, x, y );
	game->GetWorld ()->CreateComputer ( NAME_UPLINKINTERNALSERVICES, "Uplink", IP_UPLINKINTERNALSERVICES );

	Computer *comp = game->GetWorld ()->GetComputer ( NAME_UPLINKINTERNALSERVICES );
	UplinkAssert (comp);

	comp->SetTraceSpeed ( TRACESPEED_UPLINK_INTERNALSERVICESMACHINE );
	comp->SetTraceAction ( COMPUTER_TRACEACTION_DISCONNECT |
            			   COMPUTER_TRACEACTION_LEGAL );
	comp->security.AddSystem ( SECURITY_TYPE_PROXY, 5 );
	comp->security.AddSystem ( SECURITY_TYPE_FIREWALL, 5 );
	comp->security.AddSystem ( SECURITY_TYPE_MONITOR, 5 );
	comp->SetIsTargetable ( false );

	// Screen 0			( ID )

	UserIDScreen *uid = new UserIDScreen ();
	uid->SetMainTitle ( "Uplink" );
	uid->SetSubTitle ( "Internal Services System" );
	uid->SetNextPage ( 1 );
	uid->SetDifficulty ( HACKDIFFICULTY_UPLINK_INTERNALSERVICESMACHINE );
	comp->AddComputerScreen ( uid, 0 );

	// Screen 1			( Main menu )

	MenuScreen *menu2 = new MenuScreen ();
	menu2->SetMainTitle ( "Uplink" );
	menu2->SetSubTitle ( "Internal Services Main Menu" );
	menu2->AddOption ( "News Server",       "Read the latest news", 11 );
	menu2->AddOption ( "Mission list",      "View a list of available missions", 4 );
	menu2->AddOption ( "Software upgrades", "Buy new Software", 6 );
	menu2->AddOption ( "Hardware upgrades", "Buy new Hardware", 7 );
	menu2->AddOption ( "Gateway upgrades",  "Exchange your gateway for a better model", 8 );
	menu2->AddOption ( "Rankings",			"View the scores of the finest Uplink agents", 12 );
	menu2->AddOption ( "Help",				"Read documents on a wide range of skills", 20 );
	menu2->AddOption ( "Admin",				"Access Administrator services (security level 1 required)", 13, 1 );
	comp->AddComputerScreen ( menu2, 1 );

	// Screen 2			( Admin menu )

	MenuScreen *menu3 = new MenuScreen ();
	menu3->SetMainTitle ( "Uplink" );
	menu3->SetSubTitle ( "Administrator services" );
	menu3->AddOption ( "Access fileserver", "Upload or Download files", 14, 1 );
    menu3->AddOption ( "Access security", "View settings for security systems", 9, 1 );
	menu3->AddOption ( "Access logs",       "View all activity logs for this computer", 10, 1 );
	menu3->AddOption ( "Main Menu",			"Return to the Main Menu", 1 );
	comp->AddComputerScreen ( menu3, 2 );

	// Screen 4			( Bulletin board )

	BBSScreen *bbs = new BBSScreen ();
	bbs->SetNextPage ( 1 );
	bbs->SetContactPage ( 5 );
	comp->AddComputerScreen ( bbs, 4 );

	// Screen 5			( Contact screen )

	GenericScreen *gs5 = new GenericScreen ();
	gs5->SetScreenType ( SCREEN_CONTACTSCREEN );
	gs5->SetNextPage ( 4 );
	comp->AddComputerScreen ( gs5, 5 );

	// Screen 6			( Software market )

	GenericScreen *sss = new GenericScreen ();
	sss->SetScreenType ( SCREEN_SWSALESSCREEN );
	sss->SetMainTitle ( "Uplink" );
	sss->SetSubTitle ( "Software sales" );
	sss->SetNextPage ( 1 );
	comp->AddComputerScreen ( sss, 6 );

	// Screen 7			( Hardware market )

	GenericScreen *hss = new GenericScreen ();
	hss->SetScreenType ( SCREEN_HWSALESSCREEN );
	hss->SetMainTitle ( "Uplink" );
	hss->SetSubTitle ( "Hardware sales" );
	hss->SetNextPage ( 1 );
	comp->AddComputerScreen ( hss, 7 );

	// Screen 8			( Change Gateway )

	GenericScreen *gs8 = new GenericScreen ();
	gs8->SetScreenType ( SCREEN_CHANGEGATEWAY );
	gs8->SetMainTitle ( "Uplink" );
	gs8->SetSubTitle ( "Exchange gateway" );
	gs8->SetNextPage ( 1 );
	comp->AddComputerScreen ( gs8, 8 );

	// Screen 9			( Access security )

	GenericScreen *rs = new GenericScreen ();
	rs->SetScreenType ( SCREEN_SECURITYSCREEN );
	rs->SetMainTitle ( "Uplink" );
	rs->SetSubTitle ( "Security Systems" );
	rs->SetNextPage ( 2 );
	comp->AddComputerScreen ( rs, 9 );

	// Screen 10			( Access logs )

	LogScreen *ls = new LogScreen ();
	ls->SetTARGET ( LOGSCREEN_TARGET_ACCESSLOGS );
	ls->SetMainTitle ( "Uplink" );
	ls->SetSubTitle ( "Access Logs" );
	ls->SetNextPage ( 2 );
	comp->AddComputerScreen ( ls, 10 );

	// Screen 11		( News server )

	GenericScreen *gs11 = new GenericScreen ();
	gs11->SetMainTitle ( "Uplink" );
	gs11->SetSubTitle ( "News server" );
	gs11->SetScreenType ( SCREEN_NEWSSCREEN );
	gs11->SetNextPage ( 1 );
	comp->AddComputerScreen ( gs11, 11 );

	// Screen 12		( Rankings screen )

	GenericScreen *gs12 = new GenericScreen ();
	gs12->SetMainTitle ( "Uplink" );
	gs12->SetSubTitle ( "Agent Rankings" );
	gs12->SetScreenType ( SCREEN_RANKINGSCREEN );
	gs12->SetNextPage ( 1 );
	comp->AddComputerScreen ( gs12, 12 );

    // Screen 13        ( Decypher )

	CypherScreen *cs13 = new CypherScreen ();
	cs13->SetMainTitle ( "Uplink" );
	cs13->SetSubTitle ( "Enter Elliptic-Curve encryption cypher" );
	cs13->SetDifficulty ( HACKDIFFICULTY_UPLINK_INTERNALSERVICESMACHINE );
	cs13->SetNextPage ( 2 );
	comp->AddComputerScreen ( cs13, 13 );

    // Screen 14        ( File Server )

	GenericScreen *fss14 = new GenericScreen ();
	fss14->SetScreenType ( SCREEN_FILESERVERSCREEN );
	fss14->SetMainTitle ( "Uplink" );
	fss14->SetSubTitle ( "File server" );
	fss14->SetNextPage ( 2 );
	comp->AddComputerScreen ( fss14, 14 );


	// Screen 20		( Help menu )

	MenuScreen *ms20 = new MenuScreen ();
	ms20->SetMainTitle ( "Uplink" );
	ms20->SetSubTitle ( "Help/" );
	ms20->AddOption ( "Getting Started", "If you're new to this, you should read this section", 40 );
	ms20->AddOption ( "Secure Connections", "Info on establishing secure, untraceable connections", 21 );
	ms20->AddOption ( "Gateway Computers", "Info on the usage of your Gateway computer", 28 );
	ms20->AddOption ( "Security Systems", "Info on the various security systems in use", 35 );
	ms20->AddOption ( "Main Menu", "Return to main menu", 1 );
	comp->AddComputerScreen ( ms20, 20 );

	// Screen 21		( Secure connections help menu )

	MenuScreen *ms21 = new MenuScreen ();
	ms21->SetMainTitle ( "Uplink Help" );
	ms21->SetSubTitle ( "Help/Secure Connections" );
	ms21->AddOption ( "Introduction", "Introduction", 22 );
	ms21->AddOption ( "Call bouncing", "Info on routing your call through multiple servers", 23 );
	ms21->AddOption ( "Active Traces", "Info on the Active Traces that can be employed to track you", 24 );
	ms21->AddOption ( "Passive Traces", "Info on the Passive Traces that can be employed to track you", 25 );
	ms21->AddOption ( "Trace tracking", "Info on ways to monitor traces on your connection", 26 );
	ms21->AddOption ( "Active Tips", "How to avoid being traced by active traces", 48 );
    ms21->AddOption ( "Passive Tips", "How to avoid being traced by passive traces", 27 );
	ms21->AddOption ( "Help Menu", "Return to help menu", 20 );
	comp->AddComputerScreen ( ms21, 21 );

	// Screen 22		( Intro to secure connections )

	MessageScreen *ms22 = new MessageScreen ();
	ms22->SetMainTitle ( "Secure connections" );
	ms22->SetSubTitle ( "Help/Secure Connections/Introduction" );
	ms22->SetTextMessage ( "When you connect to a computer from your links screen, a direct connection is established between  "
						   "your Gateway and the computer.  This type of connection is very easy to trace back to your Gateway.  "
						   "If a connection is traced back to your Gateway, you will receive blame for the crime and risk legal action.  "
						   "\n\nThis section of the help files will teach you to establish safe connections that cannot be traced back to you." );
	ms22->SetButtonMessage ( "Return" );
	ms22->SetNextPage ( 21 );
	comp->AddComputerScreen ( ms22, 22 );

	// Screen 23		( Call bouncing help )

	MessageScreen *ms23 = new MessageScreen ();
	ms23->SetMainTitle ( "Secure connections" );
	ms23->SetSubTitle ( "Help/Secure Connections/Call bouncing" );
	ms23->SetTextMessage ( "The first step in establishing a secure untracable connection is to route your connection through "
						   "multiple servers all over the world.  In order to trace you, the agents on the target machine will "
						   "be forced to trace each link individually, which takes time.  They will get you eventually, but it "
						   "will take much longer and you will have warning before they finally get back to you.  "
						   "\n\nCall bouncing can be set up from the main map screen - simply click on each location to route the "
						   "call through, clicking finally on the target as the final node in the chain.  Then click on the connect "
						   "button to establish the connection." );
	ms23->SetButtonMessage ( "Return" );
	ms23->SetNextPage ( 21 );
	comp->AddComputerScreen ( ms23, 23 );

	// screen 24		( Active traces )

	MessageScreen *ms24 = new MessageScreen ();
	ms24->SetMainTitle ( "Secure connections" );
	ms24->SetSubTitle ( "Help/Secure Connections/Active traces" );
	ms24->SetTextMessage ( "Whenever you begin to act suspiciously, the target machine will begin to trace you.  Many computers "
						   "are running security software which begins a trace on certain key actions - such as trying hundreds "
						   "of passwords very fast (eg a Dictionary Hacker), deleting logs or use of the admin account.  "
						   "\n\nWhilst you are connected you will be traced with an Active Trace - the agents on the target machine "
						   "trace the physical lines that you are using.  This method can result in a trace back to your Gateway in "
						   "as little as one minute.  You can see an active trace as it happens on your mini-map, which should provide "
						   "ample warning before the trace completes.  You are strongly advised to disconnect before an active trace is completed." );
	ms24->SetButtonMessage ( "Return" );
	ms24->SetNextPage ( 21 );
	comp->AddComputerScreen ( ms24, 24 );

	// Screen 25		( Passive traces )

	MessageScreen *ms25 = new MessageScreen ();
	ms25->SetMainTitle ( "Secure connections" );
	ms25->SetSubTitle ( "Help/Secure Connections/Passive traces" );
	ms25->SetTextMessage ( "Whenever you use call bouncing to connect to a computer, you will leave logs behind on each computer "
						   "used.  Normally this is not a problem, but this information can be used against you.  If you disconnect "
						   "before an active trace has completed, the company that owned that computer may begin a Passive Trace. "
						   "\n\nThey will dig up any logs left by you and trace you that way.  It can take longer - usually several hours, "
						   "but the results can be just as severe.  Because of this, you should make every effort to delete the logs you leave."
						   "\n\nYou never need to delete all the logs on every system you bounce through.  Deleting the logs on one system "
						   "will be sufficient to break the chain and prevent traces from proceding." );
	ms25->SetButtonMessage ( "Return" );
	ms25->SetNextPage ( 21 );
	comp->AddComputerScreen ( ms25, 25 );

	// Screen 26		( Trace tracking )

	MessageScreen *ms26 = new MessageScreen ();
	ms26->SetMainTitle ( "Secure connections" );
	ms26->SetSubTitle ( "Help/Secure Connections/Trace tracking" );
	ms26->SetTextMessage ( "Whilst you are hacking, it is inevitable that people will attempt to trace you.  It is very important "
						   "that you avoid being traced - this of course requires that you know how close you are to being traced "
						   "at any point."
						   "\n\nIf you purchase the relevent HUD upgrade (from the software sales screen), the mini-map will show you "
						   "the progress of any Active Traces visually.  You can also purchase software "
						   "such as the Trace Tracker which will sit in the corner of your screen and will report on the progress "
						   "of any active traces.  Some later versions even report the time you have remaining before you are traced. " );
	ms26->SetButtonMessage ( "Return" );
	ms26->SetNextPage ( 21 );
	comp->AddComputerScreen ( ms26, 26 );

	// Screen 27		( Tips on secure connections )

	MessageScreen *ms27 = new MessageScreen ();
	ms27->SetMainTitle ( "Secure connections" );
	ms27->SetSubTitle ( "Help/Secure Connections/Passive Tips" );
	ms27->SetTextMessage ( "There are several methods of preventing Passive traces from coming back to you.  Here are some suggestions. \n"
						   "First, it is best to delete any logs left behind on the target machine.  This is not always possible though, "
						   "(you may be forced to disconnect quickly) and it is not always successful.\n\n"
						   "If you can establish one link in the chain on a computer that you can access easily, you can erase the logs "
						   "on that machine after every call bounce.  This requires admin access on a low security system.  Academic "
						   "systems are ideal for this due to the low consequence if caught.\n\n"
						   "Another common trick is to route through a financial or government system.  These companies are typically very "
						   "resistive to opening their logs to other agents, due to the sensitivity of their data. " );
	ms27->SetButtonMessage ( "Return" );
	ms27->SetNextPage ( 21 );
	comp->AddComputerScreen ( ms27, 27 );

	// Screen 28		( Gateway info )

	MenuScreen *ms28 = new MenuScreen ();
	ms28->SetMainTitle ( "Uplink help" );
	ms28->SetSubTitle ( "Help/Gateway Computers" );
	ms28->AddOption ( "Introduction", "Introduction to the use of Gateway computers", 29 );
	ms28->AddOption ( "Why use Gateways", "Info on the purpose of the Gateway", 30 );
	ms28->AddOption ( "Physical specs", "Info on the physical specifications of a Gateway computer", 31 );
	ms28->AddOption ( "Upgrading your Gateway", "Info on how to maintain your Gateway computer", 32 );
	ms28->AddOption ( "Return to Help Menu", "Return to the help menu", 20 );
	comp->AddComputerScreen ( ms28, 28 );

	// Screen 29		( Intro to gateway )

	MessageScreen *ms29 = new MessageScreen ();
	ms29->SetMainTitle ( "Gateway computers" );
	ms29->SetSubTitle ( "Help/Gateway Computers/Introduction" );
	ms29->SetTextMessage ( "When an Uplink agent logs on to his local machine, the Uplink software on his computer establishes a direct "
						   "connection to his Gateway computer and runs the Uplink Operating System on that Gateway.  This allows each "
						   "agent to retain a safe distance from his jumping off point onto the Web.  This gives Uplink Agents virtual "
						   "anonymity in the world - there is no way to trace an agent from a Gateway.\n\n"
						   "A gateway also provides the processor power, storage and high bandwidth connection necessary for an Uplink "
						   "agent to complete his tasks.\n"
						   "Gateway computers are rented to Uplink Agents as part of the contract and are stored in secure server rooms "
						   "around the world." );
	ms29->SetButtonMessage ( "Return" );
	ms29->SetNextPage ( 28 );
	comp->AddComputerScreen ( ms29, 29 );

	// Screen 30		( Why use a gateway )

	MessageScreen *ms30 = new MessageScreen ();
	ms30->SetMainTitle ( "Gateway computers" );
	ms30->SetSubTitle ( "Help/Gateway Computers/Why use Gateways" );
	ms30->SetTextMessage ( "A gateway computer gives total physical anonymity to all Uplink Agents who use them.  A gateway computer is "
						   "programmed to automatically disconnect any connection that has been traced back to the Gateway, thus ensuring "
						   "that an agents physical location can never be determined.  Gateway computers keep no logs of access.  Further more, "
						   "there is no record of which real person uses which username.  This means that, in a worse case scenario when a connection "
						   "is traced back to a Gateway, any federal agents inspecting the Gateway will be able to determine the username but "
						   "not the real person who uses that username.\n"
						   "Unfortunately, when this happens it is often necessary for Uplink Corporation to disavow the agent in question, "
						   "in order to avoid any legal consequences.  That user's account and rating will be deleted, and his gateway will be "
						   "given to another agent." );
	ms30->SetButtonMessage ( "Return" );
	ms30->SetNextPage ( 28 );
	comp->AddComputerScreen ( ms30, 30 );

	// Screen 31		( Physical specs of gateway )

	MessageScreen *ms31 = new MessageScreen ();
	ms31->SetMainTitle ( "Gateway computers" );
	ms31->SetSubTitle ( "Help/Gateway Computers/Physical specifications" );
	ms31->SetTextMessage ( "Uplink Corporation owns several large offices around the world, which are used to house hundreds of Gateway computers.  "
						   "These computers are openly connected to the phone network, allowing Agents to connect to them from any local computer.  "
						   "The buildings are under 24 hour guard.  Engineering teams from Uplink Corporation routinely service the computers inside, "
						   "installing new hardware when requested.\n\n"
						   "As such, the physical specifications of each machine can vary drastically.  With enough money, an Uplink Agent could upgrade "
						   "his Gateway to enormous power.  Typically a Gateway contains an inbound modem (to allow the agent to dial in), a high speed "
						   "processor and large quantities of memory storage, and a high speed broadband connection to the Net." );
	ms31->SetButtonMessage ( "Return" );
	ms31->SetNextPage ( 28 );
	comp->AddComputerScreen ( ms31, 31 );

	// Screen 32		( Upgrading your gateway )

	MessageScreen *ms32 = new MessageScreen ();
	ms32->SetMainTitle ( "Gateway Computers" );
	ms32->SetSubTitle ( "Help/Gateway Computers/Upgrading your gateway" );
	ms32->SetTextMessage ( "You can order new hardware upgrades for your Gateway from the Hardware Menu on the Uplink Internal Services Machine.  "
						   "All hardware will be installed within 24 hours by a certified Uplink technician.  Available upgrades include faster "
						   "processors, more memory capacity, higher speed connections to the Net, and many more.\n\n"
						   "You can also add extra security to your Gateway by buying security camera's, laser trip wires etc, which will inform you "
						   "if your Gateway is about to be seized by Federal Agents.  This can give you warning, allowing you to destroy any incriminating "
						   "evidence.  You can even fit your Gateway to self-destruct on your command.  In this case, Federal Agents have no way of "
						   "finding out who was using the Gateway - allowing you to keep your username and established reputation.  " );
	ms32->SetButtonMessage ( "Return" );
	ms32->SetNextPage ( 28 );
	comp->AddComputerScreen ( ms32, 32 );


	// Screen 35		( Security systems )

	MenuScreen *ms35 = new MenuScreen ();
	ms35->SetMainTitle ( "Uplink Help" );
	ms35->SetSubTitle ( "Help/Security Systems" );
	ms35->AddOption ( "Introduction", "Introduction to security systems", 36 );
	ms35->AddOption ( "the Proxy", "Information on the Proxy security system", 37 );
	ms35->AddOption ( "the Firewall", "Information on the Firewall security system", 38 );
    ms35->AddOption ( "the Monitor", "Information on the Monitor security system", 49 );
    ms35->AddOption ( "Voice print ID Systems", "Information on voice print security", 50 );
    ms35->AddOption ( "Elliptic Curve Systems", "Information on the Elliptic Curve system", 51 );
	ms35->AddOption ( "Help Menu", "Return to the help menu", 20 );
	comp->AddComputerScreen ( ms35, 35 );

	// Screen 36		( Introduction to security systems )

	MessageScreen *ms36 = new MessageScreen ();
	ms36->SetMainTitle ( "Security Systems Help" );
	ms36->SetSubTitle ( "Help/Security Systems/Introduction" );
	ms36->SetTextMessage ( "As you accept more work from Uplink Corporation you will encounter more and more security systems of different types and strengths.  "
						   "All security systems are designed with one primary goal - to keep you out, and as an Agent you must learn to bypass them.\n\n"
						   "Most systems can be simply disabled once Administrative access has been gained, but this method is considered unprofessional and is "
						   "'noisy' - most systems will detect this act and will begin tracing your connection immediately.\n"
						   "In nearly all cases a better method exists - most systems can be bypassed or fooled in some way." );
	ms36->SetButtonMessage ( "Return" );
	ms36->SetNextPage ( 35 );
	comp->AddComputerScreen ( ms36, 36 );

	// Screen 37		( Proxy systems )

	MessageScreen *ms37 = new MessageScreen ();
	ms37->SetMainTitle ( "Security Systems Help" );
	ms37->SetSubTitle ( "Help/Security Systems/the Proxy" );
	ms37->SetTextMessage ( "The primary purpose of the Proxy server is to stop unauthorised users from modifying records or logs.  A system with a good Proxy server "
						   "will permit you to examine records and logs but will not allow you to change them.  As such, you will not be able to remove any logs of "
						   "your actions, nor will you be able to make any alterations to any records on the target system.\n\n"
						   "Proxy servers are extremely popular on government data systems (such as the social security database) and other systems were the data "
						   "itself is sensitive.  They can be bypassed simply by disabling them on the target computer (which will typically require admin access) "
						   "or by running a Proxy_Disable tool.  Both of these methods will cause you to get noticed.  You can also bypass them silently with more "
						   "advanced software tools and a Connection Analyser." );
	ms37->SetButtonMessage ( "Return" );
	ms37->SetNextPage ( 35 );
	comp->AddComputerScreen ( ms37, 37 );

	// Screen 38		( Firewall systems )

	MessageScreen *ms38 = new MessageScreen ();
	ms38->SetMainTitle ( "Security Systems Help" );
	ms38->SetSubTitle ( "Help/Security Systems/the Firewall" );
	ms38->SetTextMessage ( "The primary purpose of the firewall is to prevent unauthorised usage of files on the target system.  A good Firewall will stop you from "
						   "copying, deleting or downloading any files in existence on the target computer.\n\n"
						   "Firewalls are common on File Servers or any system where the contents of the files are sensitive or valuable. They can be bypassed "
						   "simply by disabling them on the target computer (which will typically require admin access) or by running a Firewall_Disable tool.  "
						   "Both of these methods will cause you to get noticed.  You can also bypass them silently with more advanced software tools and a "
						   "Connection Analyser." );
	ms38->SetButtonMessage ( "Return" );
	ms38->SetNextPage ( 35 );
	comp->AddComputerScreen ( ms38, 38 );

	// Screen 40		( Getting started menu )

	MenuScreen *ms40 = new MenuScreen ();
	ms40->SetMainTitle ( "Uplink Help" );
	ms40->SetSubTitle ( "Help/Getting Started" );
	ms40->AddOption ( "Introduction", "What this section is about", 41 );
	ms40->AddOption ( "Basic tools", "Information on the first software tools to buy", 42 );
	ms40->AddOption ( "the Connection", "Information on opening a connection to a target", 43 );
	ms40->AddOption ( "Password Breaking", "Information on breaking the opening password", 44 );
	ms40->AddOption ( "Stealing", "Information on stealing the files you need", 45 );
	ms40->AddOption ( "Hide", "Information on covering your tracks", 46 );
	ms40->AddOption ( "Missions", "Information on getting new mission contracts", 47 );
	ms40->AddOption ( "Help Menu", "Return to the help menu", 20 );
	comp->AddComputerScreen ( ms40, 40 );

	// Screen 41		( Intro to getting started )

	MessageScreen *ms41 = new MessageScreen ();
	ms41->SetMainTitle ( "Getting Started" );
	ms41->SetSubTitle ( "Help/Getting Started/Introduction" );
	ms41->SetTextMessage ( "This section of the help documents is designed to help new Uplink Agents with their first few hours online.\n\n"
						   "We will start with the Uplink Test Mission.  You will notice this mission has already been given to you (it is given to all new agents) "
						   "and you can read the details by clicking on its icon at the bottom right of the screen.  This mission is the perfect opportunity to "
						   "test yourself against real computer defenses, without risking jail time.\n\n"
						   "We will go over the basic equipment needed, how to do the mission, and how to aquire more missions afterwards." );
	ms41->SetButtonMessage ( "Return" );
	ms41->SetNextPage ( 40 );
	comp->AddComputerScreen ( ms41, 41 );

	// Screen 42		( Basic tools )

	MessageScreen *ms42 = new MessageScreen ();
	ms42->SetMainTitle ( "Getting Started" );
	ms42->SetSubTitle ( "Help/Getting Started/Basic Tools" );
	ms42->SetTextMessage ( "The basic tools that you will require for the Uplink Test mission will also be required for almost every other type of mission, so you "
						   "will not be wasting your money when you buy these.  We recommend you purchase these tools immediately as they will be essential.\n\n"
						   "Password Breaker (V1.0) - You will need this to break through password protection screens.\n"
						   "Log Deleter (V1.0) - For removing traces of your actions.\n"
						   "Trace Tracker (V2.0) - Will allow you to monitor your communication lines, so you will know when you are about to be traced.  "
						   "We recommend version 2.0 for beginners as it will show you the percentage of your connection that has "
						   "been traced.\n\n"
						   "You can purchase all of these items with your starting loan, from the 'Software Sales' section of this site." );
	ms42->SetButtonMessage ( "Return" );
	ms42->SetNextPage ( 40 );
	ms42->SetMailThisToMe ( true );
	comp->AddComputerScreen ( ms42, 42 );

	// Screen 43		( the Connection )

	MessageScreen *ms43 = new MessageScreen ();
	ms43->SetMainTitle ( "Getting Started" );
	ms43->SetSubTitle ( "Help/Getting Started/The Connection" );
	ms43->SetTextMessage ( "The Uplink Test Mission requires you to connect to the Uplink Test Machine and steal a file.  You should not connect directly to it - "
						   "they would be able to trace you very quickly.  Instead, you should bounce our call around the world.\n"
						   "Open the Communications map by clicking on the world map, top right.  Click on a few other nodes to bounce through them.  Bounce through "
						   "at least 3 other nodes before the Uplink Test Machine.  Click on the connect button to connect to the target computer.\n\n"
						   "Now would be a good time to start up your TraceTracker  (click the software menu bottom left).  This tool will tell you if you are being traced.  "
						   "Once you start hacking, you'll have a couple of minutes before they trace you and cut you off.  In the real world, this would get you in trouble." );
	ms43->SetButtonMessage ( "Return" );
	ms43->SetNextPage ( 40 );
	ms43->SetMailThisToMe ( true );
	comp->AddComputerScreen ( ms43, 43 );

	// Screen 44		( The hack )

	MessageScreen *ms44 = new MessageScreen ();
	ms44->SetMainTitle ( "Getting Started" );
	ms44->SetSubTitle ( "Help/Getting Started/Password breaking" );
	ms44->SetTextMessage ( "Once connected to the Uplink Test Machine, you will see the main log in screen.  Since you do not have security clearence to use this machine, "
						   "you will need to hack through this screen.  Passwords can be broken in one of two ways - either using a dictionary hacker (which tries every english "
						   "word from a large dictionary) or a password cracker, which takes longer but can break non-english-word passwords.  \n\n"
						   "Run your Password Breaker and target the password box.  It will take around 10 seconds to break this password (security is low) and they "
						   "will begin tracing you immediately." );
	ms44->SetButtonMessage ( "Return" );
	ms44->SetNextPage ( 40 );
	ms44->SetMailThisToMe ( true );
	comp->AddComputerScreen ( ms44, 44 );

	// Screen 45		( Stealing the file )

	MessageScreen *ms45 = new MessageScreen ();
	ms45->SetMainTitle ( "Getting Started" );
	ms45->SetSubTitle ( "Help/Getting Started/Stealing the file" );
	ms45->SetTextMessage ( "Once you are in the system, you should have no trouble accessing the file server and downloading the file you need.  (Use the file copier and download "
						   "the file 'uplink test data' into your memory banks.)  In many cases this might not be so easy - you may find that Firewall or Proxy systems are running, "
						   "which will prevent you from just copying files easily.  You will learn to bypass these in time.\n\n"
						   "Once you've got the file, disconnect from the system as soon as possible.\n"
						   "You can now send it to the employer (by clicking on 'reply' in the mission screen) and receive your payment, which in this case "
						   "will be zero (it was only a test after all)." );
	ms45->SetButtonMessage ( "Return" );
	ms45->SetNextPage ( 40 );
	ms45->SetMailThisToMe ( true );
	comp->AddComputerScreen ( ms45, 45 );

	// Screen 46		( Hide )

	MessageScreen *ms46 = new MessageScreen ();
	ms46->SetMainTitle ( "Getting Started" );
	ms46->SetSubTitle ( "Help/Getting Started/Covering your tracks" );
	ms46->SetTextMessage ( "The final step in any hack is to cover your tracks.  "
						   "Access the log banks and run your log deleter.  Remember to keep an eye on the trace tracker.  Locate the logs that you have created from "
						   "your actions (especially the ones that detail where your connection came from) and target your log deleter on them.  The logs will be "
						   "deleted but will leave a clear marker behind.  More advanced versions of the log deleter can cover your tracks better, but this will be "
						   "sufficient for now.\n\n"
						   "In the real world of hacking, you would probably not delete the logs on the target machine - you would not have time.  Instead, you would "
						   "bounce your connection through a system on which you have admin access.  After the hack, you would connect to that system and remove those "
						   "logs in safety - breaking the chain of logs between yourself and the target." );
	ms46->SetButtonMessage ( "Return" );
	ms46->SetNextPage ( 40 );
	ms46->SetMailThisToMe ( true );
	comp->AddComputerScreen ( ms46, 46 );

	// Screen 47		( Missions )

	MessageScreen *ms47 = new MessageScreen ();
	ms47->SetMainTitle ( "Getting Started" );
	ms47->SetSubTitle ( "Help/Getting Started/Future missions" );
	ms47->SetTextMessage ( "The Uplink test mission is the easiest mission you will ever attempt.  Once you feel good enough, connect to the Uplink Internal Services "
						   "Machine and go to the Missions section.  You will see some missions that are already available.  You may be able to auto-accept some of them, "
						   "and you will need to contact the employer to accept others.  As your rating increases, more and more missions will appear here.  You will find "
						   "that you will need to upgrade your hardware and software before the more complex missions can be attempted." );
	ms47->SetButtonMessage ( "Return" );
	ms47->SetNextPage ( 40 );
	ms47->SetMailThisToMe ( true );
	comp->AddComputerScreen ( ms47, 47 );

	// Screen 48		( Tips on secure connections (active) )

	MessageScreen *ms48 = new MessageScreen ();
	ms48->SetMainTitle ( "Secure connections" );
	ms48->SetSubTitle ( "Help/Secure Connections/Active Tips" );
	ms48->SetTextMessage ( "There are several important points to keep in mind when setting up connections. If done correctly, you can give yourself the extra few "
                           "seconds need to get the job done without being caught.\n\n"
                           "Firstly, bouncing through a random computer will not work against any reasonably secure system. They will be able to trace through that link in seconds.\n"
                           "If you bounce through a system that you have already compromised, and have an access account available, the trace will take much longer.\n"
                           "If you bounce through a system on which you have Admin access, the trace will take longer still.\n"
                           "In general, the higher security the system you bounce through, the longer it will take to trace through that link.\n\n"
                           "It is worth keeping in mind that you can lose your username and password login on a system if you route through it too many times." );
	ms48->SetButtonMessage ( "Return" );
	ms48->SetNextPage ( 21 );
	comp->AddComputerScreen ( ms48, 48 );

	// Screen 49		( Monitor systems )

	MessageScreen *ms49 = new MessageScreen ();
	ms49->SetMainTitle ( "Security Systems Help" );
	ms49->SetSubTitle ( "Help/Security Systems/the Monitor" );
	ms49->SetTextMessage ( "Monitor systems are designed to detect unauthorised activity. They are usually triggered when a user attempts to hack a password, "
                           "or makes use of the Admin account, or starts deleting files. Once they have detected illegal activity they will sound the alarm and "
                           "start an Active Trace on your connection. They will also flag your logs as suspicious.\n\n"
                           "Monitor systems will also periodically check the other security systems running locally, to see if they have been disabled.\n\n"
						   "You can bypass a monitor system with a connection Analyser HUD upgrade and a Monitor Bypass.  This will allow you to hack without being noticed." );
	ms49->SetButtonMessage ( "Return" );
	ms49->SetNextPage ( 35 );
	comp->AddComputerScreen ( ms49, 49 );

    // Screen 50        ( Voice print ID systems )

    MessageScreen *ms50 = new MessageScreen ();
    ms50->SetMainTitle ( "Security Systems Help" );
    ms50->SetSubTitle ( "Help/Security Systems/Voice Print ID" );
    ms50->SetTextMessage ( "Many systems that require a specific person to log in use Voice Print technology.  The users voice is first sampled and analysed "
                           "by the company, then digitised and encrypted onto their system.  When that person logs in, he reproduces a certain phrase or set "
                           "of words.  These are then analysed and compared.\n\n"
                           "Recent advances in this area have made it very difficult to crack.  For many years Digital Approximation software was capable of "
                           "defeating these systems, but this is no longer the case.  In most modern systems, the only real way is to obtain a sample of the users "
                           "voice directly.  Agents can then use Voice Analysis software to reproduce the phrase digitally from that sample." );
    ms50->SetButtonMessage ( "Return" );
    ms50->SetNextPage ( 35 );
    comp->AddComputerScreen ( ms50, 50 );

    // Screen 51        ( Elliptic Curve systems )

    MessageScreen *ms51 = new MessageScreen ();
    ms51->SetMainTitle ( "Security Systems Help" );
    ms51->SetSubTitle ( "Help/Security Systems/Elliptic Curve Security" );
    ms51->SetTextMessage ( "Faced with the growing threat of hackers, many companies have begun using Elliptic Curve Cryptography.  A user on a system must "
                           "input a large block of data in sequence, synchronised with the system.  This cannot be done by hand - the timings must be perfect. "
                           "Elliptic Curve Cryptography is popular due to the large amount of CPU power needed to force crack it.\n\n"
                           "However, as with all cryptography, even this system can be broken given enough time and CPU cycles.  Agents can use the Decypher "
                           "software tool to force crack one of these systems, but a fast CPU rig will be required." );
    ms51->SetButtonMessage ( "Return" );
    ms51->SetNextPage ( 35 );
    comp->AddComputerScreen ( ms51, 51 );

	// Add in the admin account

	Record *record = new Record ();
	record->AddField ( RECORDBANK_NAME, RECORDBANK_ADMIN );
	record->AddField ( RECORDBANK_PASSWORD, NameGenerator::GenerateComplexPassword () );
	record->AddField ( RECORDBANK_SECURITY, "1" );
	comp->recordbank.AddRecord ( record );

    // Add in some interesting data

    comp->databank.SetSize ( 100 );

    Data *program = new Data ();
    program->SetTitle ( "Uplink_Agent_List" );
    program->SetDetails ( DATATYPE_PROGRAM, 2, 0, 0, 1.0, SOFTWARETYPE_OTHER );
    comp->databank.PutData ( program );

	for ( int i = 0; i < 10; ++i ) {

		char datatitle [64];
		UplinkSnprintf ( datatitle, sizeof ( datatitle ), "Uplink_Agent_Data %d.dat", i );
        int size = 5;
        int encrypted = 7;

        if ( i != 4 ) {                                             // Miss out 4 to make some of the names Unknown

		    Data *file = new Data ();
		    file->SetTitle ( datatitle );
		    file->SetDetails ( DATATYPE_DATA, size, encrypted, 0 );

            comp->databank.PutData ( file );

        }

    }

}

void WorldGenerator::GenerateUplinkTestMachine ()
{

	int x, y;
	GenerateValidMapPos ( x, y );
	game->GetWorld ()->CreateVLocation ( IP_UPLINKTESTMACHINE, x, y );

	// Generate a computer with some data on it

	game->GetWorld ()->CreateComputer ( "Uplink Test Machine", "Uplink", IP_UPLINKTESTMACHINE );
	Computer *comp = game->GetWorld ()->GetComputer ( "Uplink Test Machine" );
	UplinkAssert ( comp );
	comp->SetIsTargetable ( false );

	comp->SetTraceSpeed ( TRACESPEED_UPLINK_TESTMACHINE );
	comp->SetTraceAction ( COMPUTER_TRACEACTION_DISCONNECT );
	comp->security.AddSystem ( SECURITY_TYPE_MONITOR, 1 );
	comp->databank.SetSize ( 20 );

	Data *data1 = new Data ();
	data1->SetTitle ( "Uplink test data" );
	data1->SetDetails ( DATATYPE_DATA, 6, 0, 0 );
	comp->databank.PutData ( data1 );

	Data *data2 = new Data ();
	data2->SetTitle ( "Random file" );
	data2->SetDetails ( DATATYPE_DATA, 3, 4, 1 );
	comp->databank.PutData ( data2 );

	Data *data3 = new Data ();
	data3->SetTitle ( "Sample program" );
	data3->SetDetails ( DATATYPE_PROGRAM, 2, 0, 0 );
	comp->databank.PutData ( data3 );

	// Generate an admin log on

	Record *record = new Record ();
	record->AddField ( RECORDBANK_NAME, RECORDBANK_ADMIN );
	record->AddField ( RECORDBANK_PASSWORD, "rosebud" );
	record->AddField ( RECORDBANK_SECURITY, "1" );
	comp->recordbank.AddRecord ( record );

	// Generate an interface

	// Screen 0			( Opening message )

	MessageScreen *ms = new MessageScreen ();
	ms->SetMainTitle ( "Uplink" );
	ms->SetSubTitle ( "Welcome to the Uplink Test Machine" );
	ms->SetTextMessage ( "This will be your first opportunity to hack into a real computer system with real defenses. "
						 "There is no point in trying to hack a real computer system until you can do the Uplink Test in your sleep.\n"
						 "\nYou will be traced during your attempts, so remember to monitor the communications lines, however no action will be taken against you should you be traced.\n"
						 "\nHINT : Never connect directly to your target server.  Bounce your connection around the world - "
						 "it will make you much harder to track and give you longer to complete your work.\n"
						 "Remember that you will leave logs on the computers you use as relays (which can be traced back to you) - ideally your connection should use a computer which allows you easy and risk free log access, "
						 "so you can erase your tracks without fear of punishment.\n" );
	ms->SetButtonMessage ( "Start the test" );
	ms->SetNextPage ( 1 );
	comp->AddComputerScreen ( ms, 0 );

	// Screen 1			( User ID )

	UserIDScreen *uid = new UserIDScreen ();
	uid->SetMainTitle ( "Uplink" );
	uid->SetSubTitle ( "Test machine" );
	uid->SetDifficulty ( HACKDIFFICULTY_UPLINK_TESTMACHINE );
	uid->SetNextPage ( 2 );
	comp->AddComputerScreen ( uid, 1 );

	// Screen 2			( Main menu )

	MenuScreen *menu = new MenuScreen ();
	menu->SetMainTitle ( "Uplink" );
	menu->SetSubTitle ( "Test machine Main Menu" );
	menu->AddOption ( "Access fileserver", "", 4 );
	menu->AddOption ( "View Logs", "", 3 );
	menu->AddOption ( "Console", "", 5 );
	comp->AddComputerScreen ( menu, 2 );

	// Screen 3			( Logs )

	LogScreen *ls = new LogScreen ();
	ls->SetTARGET ( LOGSCREEN_TARGET_ACCESSLOGS );
	ls->SetMainTitle ( "Uplink Test Machine" );
	ls->SetSubTitle ( "Access logs" );
	ls->SetNextPage ( 2 );
	comp->AddComputerScreen ( ls, 3 );

	// Screen 4			( file server )

	GenericScreen *gs4 = new GenericScreen ();
	gs4->SetScreenType ( SCREEN_FILESERVERSCREEN );
	gs4->SetMainTitle ( "Uplink Test Machine" );
	gs4->SetSubTitle ( "File Server" );
	gs4->SetNextPage ( 2 );
	comp->AddComputerScreen ( gs4, 4 );

	// Screen 5			( Console )

	GenericScreen *gs5 = new GenericScreen ();
	gs5->SetScreenType ( SCREEN_CONSOLESCREEN );
	gs5->SetMainTitle ( "Uplink Test Machine" );
	gs5->SetSubTitle ( "Console" );
	gs5->SetNextPage ( 2 );
	comp->AddComputerScreen ( gs5, 5 );

}

void WorldGenerator::GenerateUplinkCreditsMachine ()
{

	int x, y;
	GenerateValidMapPos ( x, y );
	game->GetWorld ()->CreateVLocation ( IP_UPLINKCREDITSMACHINE, x, y );

	// Generate a computer with some data on it

	game->GetWorld ()->CreateComputer ( "Uplink Credits Machine", "Uplink", IP_UPLINKCREDITSMACHINE );
	Computer *comp = game->GetWorld ()->GetComputer ( "Uplink Credits Machine" );
	UplinkAssert ( comp );
	comp->SetIsTargetable ( false );

	MessageScreen *ms = new MessageScreen ();
	ms->SetMainTitle ( "Uplink" );
	ms->SetSubTitle ( "The Credits Machine" );
	ms->SetTextMessage ( "Produced by INTROVERSION SOFTWARE\n\n"
                         "Chris 'Abandon' Delay\n"
                         "Mark 'Monkey' Morris\n"
                         "Tom 'Blissfully-ignorant' Arundel\n\n"
                         "WITH HELP FROM\n"
                         "John 'Rotton-smell' Knottenbelt'\n"
                         "Gareth 'D-V-D' Sidebottom'\n\n"
                         "DEVELOPMENT TIME : 816 days\n"
                         "SOURCE CODE SIZE : 93102 lines\n\n"
                         "A-N-D  C-O-U-N-T-I-N-G" );
	comp->AddComputerScreen ( ms, 0 );

}

void WorldGenerator::GenerateCompanyGovernment ()
{

	Company *company = GenerateCompany ( "Government", 5, 1, 10, -20 );

//	Person *admin = GetRandomPerson ();
//	company->SetAdmin ( admin->name );

}

void WorldGenerator::GenerateGlobalCriminalDatabase ()
{

	// Create the VLocation

	int x, y;
	GenerateValidMapPos ( x, y );
	game->GetWorld ()->CreateVLocation ( IP_GLOBALCRIMINALDATABASE, x, y );

	// Create the computer

	Computer *comp = new Computer ();
	comp->SetTYPE ( COMPUTER_TYPE_UNKNOWN );
	comp->SetName ( "Global Criminal Database" );
	comp->SetCompanyName ( "Government" );
	comp->SetIP ( IP_GLOBALCRIMINALDATABASE );
	comp->SetTraceSpeed ( TRACESPEED_GLOBALCRIMINALDATABASE );
	comp->SetTraceAction ( COMPUTER_TRACEACTION_DISCONNECT |
						   COMPUTER_TRACEACTION_LEGAL );
	comp->SetIsTargetable ( false );
	comp->security.AddSystem ( SECURITY_TYPE_PROXY, 1 );
	comp->security.AddSystem ( SECURITY_TYPE_MONITOR, 1 );
	game->GetWorld ()->CreateComputer ( comp );

	// Screen 0					( Opening message screen )

	MessageScreen *ms = new MessageScreen ();
	ms->SetMainTitle ( "Global Criminal Database" );
	ms->SetSubTitle ( "Unauthorised Access will be punished" );
	ms->SetTextMessage ( "This is a Government owned computer system - all attempts at\n"
						 "illegal access will be severely punished to the full extent of the law.\n\n"
						 "An Elliptic-Curve encryption cypher will be required for full system access.\n\n"
						 "Valid User Accounts\n"
						 "====================\n\n"
						 "admin     - Complete system wide access\n"
						 "readwrite  - Read/Write access to all records\n"
						 "readonly   - Read only access to all records\n" );
	ms->SetButtonMessage ( "Log in" );
	ms->SetNextPage ( 1 );
	comp->AddComputerScreen ( ms, 0 );

	// Screen 1					( High Security Screen )

	HighSecurityScreen *hs = new HighSecurityScreen ();
	hs->SetMainTitle ( "Global Criminal Database" );
	hs->SetSubTitle ( "Authorisation required" );
	hs->AddSystem ( "UserID / password verification", 2 );
	hs->AddSystem ( "Elliptic-Curve Encryption Cypher", 3 );
	hs->SetNextPage ( 4 );
	comp->AddComputerScreen ( hs, 1 );

	// Screen 2					( User ID Screen )

	UserIDScreen *uid = new UserIDScreen ();
	uid->SetMainTitle ( "Global Criminal Database" );
	uid->SetSubTitle ( "Log in" );
	uid->SetDifficulty ( HACKDIFFICULTY_GLOBALCRIMINALDATABASE );
	uid->SetNextPage ( 1 );
	comp->AddComputerScreen ( uid, 2 );

	// Screen 3					( Cypher screen )

	CypherScreen *cs = new CypherScreen ();
	cs->SetMainTitle ( "Global Criminal Database" );
	cs->SetSubTitle ( "Enter elliptic-curve encryption cypher" );
	cs->SetDifficulty ( HACKDIFFICULTY_GLOBALCRIMINALDATABASE );
	cs->SetNextPage ( 1 );
	comp->AddComputerScreen ( cs, 3 );

	// Screen 4					( Main Menu )

	MenuScreen *menu = new MenuScreen ();
	menu->SetMainTitle ( "Global Criminal Database" );
	menu->SetSubTitle ( "Main Menu" );
	menu->AddOption ( "Search Records", "", 7 );
#ifdef TESTGAME
	menu->AddOption ( "Edit Records", "", 6, 1 );
#endif
	menu->AddOption ( "View Logs", "", 6, 1 );
	menu->AddOption ( "Security", "", 9, 1 );
	comp->AddComputerScreen ( menu, 4 );

	// Screen 5					( Records )

	GenericScreen *rs = new GenericScreen ();
	rs->SetScreenType ( SCREEN_RECORDSCREEN );
	rs->SetMainTitle ( "Global Criminal Database" );
	rs->SetSubTitle ( "Criminal Records" );
	rs->SetNextPage ( 4 );
	comp->AddComputerScreen ( rs, 5 );

	// Screen 6					( Logs )

	LogScreen *ls = new LogScreen ();
	ls->SetTARGET ( LOGSCREEN_TARGET_ACCESSLOGS );
	ls->SetMainTitle ( "Global Criminal Database" );
	ls->SetSubTitle ( "Access logs" );
	ls->SetNextPage ( 4 );
	comp->AddComputerScreen ( ls, 6 );

	// Screen 7					( Search )

	DialogScreen *ds7 = new DialogScreen ();
	ds7->SetMainTitle ( "Global Criminal Database" );
	ds7->SetSubTitle ( "Search for Criminal Record" );
	ds7->AddWidget ( "name", WIDGET_TEXTBOX, 150, 200, 200, 15, "Enter name here", "Type the name of the person to search for here" );
	ds7->AddWidget ( "OK", WIDGET_SCRIPTBUTTON, 150, 240, 80, 15, "OK", "Click here when done", 15, -1, NULL, NULL );
	ds7->AddWidget ( "Cancel", WIDGET_NEXTPAGE, 270, 240, 80, 15, "Cancel", "Click here to return to the menu", 4, -1, NULL, NULL );

	ds7->AddWidget ( "border1", WIDGET_BASIC, 120, 170, 260, 4, "", "" );
	ds7->AddWidget ( "border2", WIDGET_BASIC, 120, 266, 260, 4, "", "" );
	ds7->AddWidget ( "border3", WIDGET_BASIC, 120, 170, 4, 100, "", "" );
	ds7->AddWidget ( "border4", WIDGET_BASIC, 380, 170, 4, 100, "", "" );

    ds7->SetReturnKeyButton ( "OK" );
    ds7->SetEscapeKeyButton ( "Cancel" );

	comp->AddComputerScreen ( ds7, 7 );

	// Screen 8					( Search )

	GenericScreen *gs8 = new GenericScreen ();
	gs8->SetScreenType ( SCREEN_CRIMINALSCREEN );
	gs8->SetMainTitle ( "Global Criminal Database" );
	gs8->SetSubTitle ( "View Criminal Records" );
	gs8->SetNextPage ( 4 );
	comp->AddComputerScreen ( gs8, 8 );

	// Screen 9					( Security )

	GenericScreen *gs9 = new GenericScreen ();
	gs9->SetScreenType ( SCREEN_SECURITYSCREEN );
	gs9->SetMainTitle ( "Global Criminal Database" );
	gs9->SetSubTitle ( "Security settings" );
	gs9->SetNextPage ( 4 );
	comp->AddComputerScreen ( gs9, 9 );


	// Add in the user accounts

	Record *record = new Record ();
	record->AddField ( RECORDBANK_NAME, RECORDBANK_ADMIN );
	record->AddField ( RECORDBANK_PASSWORD, NameGenerator::GenerateComplexPassword () );
	record->AddField ( RECORDBANK_SECURITY, "1" );
	comp->recordbank.AddRecord ( record );

	Record *record2 = new Record ();
	record2->AddField ( RECORDBANK_NAME, RECORDBANK_READWRITE );
	record2->AddField ( RECORDBANK_PASSWORD, NameGenerator::GenerateComplexPassword () );
	record2->AddField ( RECORDBANK_SECURITY, "2" );
	comp->recordbank.AddRecord ( record2 );

	Record *record3 = new Record ();
	record3->AddField ( RECORDBANK_NAME, RECORDBANK_READONLY );
	record3->AddField ( RECORDBANK_PASSWORD, NameGenerator::GeneratePassword () );
	record3->AddField ( RECORDBANK_SECURITY, "4" );
	comp->recordbank.AddRecord ( record3 );

}

void WorldGenerator::GenerateInternationalSocialSecurityDatabase ()
{

	// Create the VLocation

	int x, y;
	GenerateValidMapPos ( x, y );
	game->GetWorld ()->CreateVLocation ( IP_SOCIALSECURITYDATABASE, x, y );

	// Create the computer

	Computer *comp = new Computer ();
	comp->SetTYPE ( COMPUTER_TYPE_UNKNOWN );
	comp->SetName ( "International Social Security Database" );
	comp->SetCompanyName ( "Government" );
	comp->SetIP ( IP_SOCIALSECURITYDATABASE );
	comp->SetTraceSpeed ( TRACESPEED_INTERNATIONALSOCIALSECURITYDATABASE );
	comp->SetTraceAction ( COMPUTER_TRACEACTION_DISCONNECT |
						   COMPUTER_TRACEACTION_FINE );
	comp->SetIsTargetable ( false );
	comp->security.AddSystem ( SECURITY_TYPE_PROXY, 1 );
	comp->security.AddSystem ( SECURITY_TYPE_MONITOR, 1 );
	game->GetWorld ()->CreateComputer ( comp );

	// Screen 0					( Opening message screen )

	MessageScreen *ms = new MessageScreen ();
	ms->SetMainTitle ( "Social Security Database" );
	ms->SetSubTitle ( "Unauthorised Access will be punished" );
	ms->SetTextMessage ( "This is a Government owned computer system - all attempts at\n"
						 "illegal access will be severely punished to the full extent of the law.\n\n"
						 "Valid User Accounts\n"
						 "====================\n\n"
						 "admin    - Complete system wide access\n"
						 "readwrite  - Read/Write access to all records\n"
						 "readonly  - Read only access to all records\n" );
	ms->SetButtonMessage ( "Log in" );
	ms->SetNextPage ( 1 );
	comp->AddComputerScreen ( ms, 0 );

	// Screen 1					( User ID Screen )

	UserIDScreen *uid = new UserIDScreen ();
	uid->SetMainTitle ( "Social Security Database" );
	uid->SetSubTitle ( "Log in" );
	uid->SetDifficulty ( HACKDIFFICULTY_INTERNATIONALSOCIALSECURITYDATABASE );
	uid->SetNextPage ( 2 );
	comp->AddComputerScreen ( uid, 1 );

	// Screen 2					( Main Menu )

	MenuScreen *menu = new MenuScreen ();
	menu->SetMainTitle ( "Social Security Database" );
	menu->SetSubTitle ( "Main Menu" );
	menu->AddOption ( "Search Records", "", 5, 3 );
#ifdef TESTGAME
	menu->AddOption ( "View Records", "", 3 );
#endif
	menu->AddOption ( "View Logs", "", 4, 1 );
	comp->AddComputerScreen ( menu, 2 );

	// Screen 3					( Records )

	GenericScreen *rs = new GenericScreen ();
	rs->SetScreenType ( SCREEN_RECORDSCREEN );
	rs->SetMainTitle ( "Social Security Database" );
	rs->SetSubTitle ( "Records" );
	rs->SetNextPage ( 2 );
	comp->AddComputerScreen ( rs, 3 );

	// Screen 4					( Logs )

	LogScreen *ls = new LogScreen ();
	ls->SetTARGET ( LOGSCREEN_TARGET_ACCESSLOGS );
	ls->SetMainTitle ( "Social Security Database" );
	ls->SetSubTitle ( "Access logs" );
	ls->SetNextPage ( 2 );
	comp->AddComputerScreen ( ls, 4 );

	// Screen 5					( Search )

	DialogScreen *ds5 = new DialogScreen ();
	ds5->SetMainTitle ( "Social Security Database" );
	ds5->SetSubTitle ( "Search for social security record" );
	ds5->AddWidget ( "name", WIDGET_TEXTBOX, 150, 200, 200, 15, "Enter name here", "Type the name of the person to search for here" );
	ds5->AddWidget ( "OK", WIDGET_SCRIPTBUTTON, 150, 240, 80, 15, "OK", "Click here when done", 17, -1, NULL, NULL );
	ds5->AddWidget ( "Cancel", WIDGET_NEXTPAGE, 270, 240, 80, 15, "Cancel", "Click here to return to the menu", 2, -1, NULL, NULL );

	ds5->AddWidget ( "border1", WIDGET_BASIC, 120, 170, 260, 4, "", "" );
	ds5->AddWidget ( "border2", WIDGET_BASIC, 120, 266, 260, 4, "", "" );
	ds5->AddWidget ( "border3", WIDGET_BASIC, 120, 170, 4, 100, "", "" );
	ds5->AddWidget ( "border4", WIDGET_BASIC, 380, 170, 4, 100, "", "" );

    ds5->SetReturnKeyButton ( "OK" );
    ds5->SetEscapeKeyButton ( "Cancel" );

	comp->AddComputerScreen ( ds5, 5 );

	// Screen 6					( View record )

	GenericScreen *gs6 = new GenericScreen ();
	gs6->SetMainTitle ( "Social Security Database" );
	gs6->SetSubTitle ( "View record" );
	gs6->SetNextPage ( 2 );
	gs6->SetScreenType ( SCREEN_SOCSECSCREEN );
	comp->AddComputerScreen ( gs6, 6 );

	// Add in the accounts

	Record *record = new Record ();
	record->AddField ( RECORDBANK_NAME, RECORDBANK_ADMIN );
	record->AddField ( RECORDBANK_PASSWORD, NameGenerator::GeneratePassword () );
	record->AddField ( RECORDBANK_SECURITY, "1" );
	record->AddField ( "Social Security", -1 );
	comp->recordbank.AddRecord ( record );

	Record *record2 = new Record ();
	record2->AddField ( RECORDBANK_NAME, RECORDBANK_READWRITE );
	record2->AddField ( RECORDBANK_PASSWORD, NameGenerator::GeneratePassword () );
	record2->AddField ( RECORDBANK_SECURITY, "2" );
	record2->AddField ( "Social Security", -1 );
	comp->recordbank.AddRecord ( record2 );

	Record *record3 = new Record ();
	record3->AddField ( RECORDBANK_NAME, RECORDBANK_READONLY );
	record3->AddField ( RECORDBANK_PASSWORD, NameGenerator::GeneratePassword () );
	record3->AddField ( RECORDBANK_SECURITY, "4" );
	record3->AddField ( "Social Security", -1 );
	comp->recordbank.AddRecord ( record3 );

}

void WorldGenerator::GenerateCentralMedicalDatabase ()
{

	// Create the VLocation

	int x, y;
	GenerateValidMapPos ( x, y );
	game->GetWorld ()->CreateVLocation ( IP_CENTRALMEDICALDATABASE, x, y );

	// Create the computer

	Computer *comp = new Computer ();
	comp->SetTYPE ( COMPUTER_TYPE_UNKNOWN );
	comp->SetName ( "Central Medical Database" );
	comp->SetCompanyName ( "Government" );
	comp->SetIP ( IP_CENTRALMEDICALDATABASE );
	comp->SetTraceSpeed ( TRACESPEED_CENTRALMEDICALDATABASE );
	comp->SetTraceAction ( COMPUTER_TRACEACTION_DISCONNECT |
						   COMPUTER_TRACEACTION_WARNINGMAIL );
	comp->SetIsTargetable ( false );
	comp->security.AddSystem ( SECURITY_TYPE_PROXY, 1 );
	comp->security.AddSystem ( SECURITY_TYPE_MONITOR, 1 );
	game->GetWorld ()->CreateComputer ( comp );

	// Screen 0					( Opening message screen )

	MessageScreen *ms = new MessageScreen ();
	ms->SetMainTitle ( "Central Medical Database" );
	ms->SetSubTitle ( "No Unauthorised Access" );
	ms->SetTextMessage ( "admin - Complete read/write/delete access on all records" );
	ms->SetButtonMessage ( "Log in" );
	ms->SetNextPage ( 1 );
	comp->AddComputerScreen ( ms, 0 );

	// Screen 1					( User ID Screen )

	UserIDScreen *uid = new UserIDScreen ();
	uid->SetMainTitle ( "Central Medical Database" );
	uid->SetSubTitle ( "Log in" );
	uid->SetDifficulty ( HACKDIFFICULTY_CENTRALMEDICALDATABASE );
	uid->SetNextPage ( 2 );
	comp->AddComputerScreen ( uid, 1 );

	// Screen 2					( Main Menu )

	MenuScreen *menu = new MenuScreen ();
	menu->SetMainTitle ( "Central Medical Database" );
	menu->SetSubTitle ( "Main Menu" );
	menu->AddOption ( "View Records", "", 3 );
	menu->AddOption ( "View Logs", "", 4, 3 );
	comp->AddComputerScreen ( menu, 2 );

	// Screen 3					( Records )

	GenericScreen *rs = new GenericScreen ();
	rs->SetScreenType ( SCREEN_RECORDSCREEN );
	rs->SetMainTitle ( "Central Medical Database" );
	rs->SetSubTitle ( "Medical Records" );
	rs->SetNextPage ( 2 );
	comp->AddComputerScreen ( rs, 3 );

	// Screen 4					( Logs )

	LogScreen *ls = new LogScreen ();
	ls->SetTARGET ( LOGSCREEN_TARGET_ACCESSLOGS );
	ls->SetMainTitle ( "Central Medical Database" );
	ls->SetSubTitle ( "Access logs" );
	ls->SetNextPage ( 2 );
	comp->AddComputerScreen ( ls, 4 );

	// Add in the admin account

	Record *record = new Record ();
	record->AddField ( RECORDBANK_NAME, RECORDBANK_ADMIN );
	record->AddField ( "Handle", RECORDBANK_ADMIN );
	record->AddField ( RECORDBANK_PASSWORD, NameGenerator::GenerateComplexPassword () );
	record->AddField ( RECORDBANK_SECURITY, "1" );
	comp->recordbank.AddRecord ( record );

}

void WorldGenerator::GenerateGlobalIntelligenceAgency ()
{
}

void WorldGenerator::GenerateInternationalAcademicDatabase ()
{

	// Create the VLocation

	int x, y;
	GenerateValidMapPos ( x, y );
	game->GetWorld ()->CreateVLocation ( IP_ACADEMICDATABASE, x, y );

	// Create the computer

	Computer *comp = new Computer ();
	comp->SetTYPE ( COMPUTER_TYPE_UNKNOWN );
	comp->SetName ( "International Academic Database" );
	comp->SetCompanyName ( "Government" );
	comp->SetIP ( IP_ACADEMICDATABASE );
	comp->SetTraceSpeed ( TRACESPEED_INTERNATIONALACADEMICDATABASE );
	comp->SetTraceAction ( COMPUTER_TRACEACTION_DISCONNECT |
						   COMPUTER_TRACEACTION_WARNINGMAIL );
	comp->SetIsTargetable ( false );
	comp->security.AddSystem ( SECURITY_TYPE_MONITOR, 1 );
	game->GetWorld ()->CreateComputer ( comp );

	// Screen 0					( Opening message screen )

	MessageScreen *ms = new MessageScreen ();
	ms->SetMainTitle ( "International Academic Database" );
	ms->SetSubTitle ( "Unauthorised Access will be punished" );
	ms->SetTextMessage ( "This is a Government owned computer system - all attempts at\n"
						 "illegal access will be severely punished to the full extent of the law.\n\n"
						 "Valid User Accounts\n"
						 "====================\n\n"
						 "admin    - Complete system wide access\n"
						 "readwrite  - Read/Write access to all records\n"
						 "readonly  - Read only access to all records\n" );
	ms->SetButtonMessage ( "Log in" );
	ms->SetNextPage ( 1 );
	comp->AddComputerScreen ( ms, 0 );

	// Screen 1					( User ID Screen )

	UserIDScreen *uid = new UserIDScreen ();
	uid->SetMainTitle ( "International Academic Database" );
	uid->SetSubTitle ( "Log in" );
	uid->SetDifficulty ( HACKDIFFICULTY_INTERNATIONALACADEMICDATABASE );
	uid->SetNextPage ( 2 );
	comp->AddComputerScreen ( uid, 1 );

	// Screen 2					( Main Menu )

	MenuScreen *menu = new MenuScreen ();
	menu->SetMainTitle ( "International Academic Database" );
	menu->SetSubTitle ( "Main Menu" );
	menu->AddOption ( "Search Records", "", 5, 3 );
#ifdef TESTGAME
	menu->AddOption ( "View Records", "", 3 );
#endif
	menu->AddOption ( "View Logs", "", 4, 1 );
	comp->AddComputerScreen ( menu, 2 );

	// Screen 3					( Records )

	GenericScreen *rs = new GenericScreen ();
	rs->SetScreenType ( SCREEN_RECORDSCREEN );
	rs->SetMainTitle ( "International Academic Database" );
	rs->SetSubTitle ( "Academic Records" );
	rs->SetNextPage ( 2 );
	comp->AddComputerScreen ( rs, 3 );

	// Screen 4					( Logs )

	LogScreen *ls = new LogScreen ();
	ls->SetTARGET ( LOGSCREEN_TARGET_ACCESSLOGS );
	ls->SetMainTitle ( "International Academic Database" );
	ls->SetSubTitle ( "Access logs" );
	ls->SetNextPage ( 2 );
	comp->AddComputerScreen ( ls, 4 );

	// Screen 5					( Search )

	DialogScreen *ds5 = new DialogScreen ();
	ds5->SetMainTitle ( "International Academic Database" );
	ds5->SetSubTitle ( "Search for Academic record" );
	ds5->AddWidget ( "name", WIDGET_TEXTBOX, 150, 200, 200, 15, "Enter name here", "Type the name of the person to search for here" );
	ds5->AddWidget ( "OK", WIDGET_SCRIPTBUTTON, 150, 240, 80, 15, "OK", "Click here when done", 16, -1, NULL, NULL );
	ds5->AddWidget ( "Cancel", WIDGET_NEXTPAGE, 270, 240, 80, 15, "Cancel", "Click here to return to the menu", 2, -1, NULL, NULL );

	ds5->AddWidget ( "border1", WIDGET_BASIC, 120, 170, 260, 4, "", "" );
	ds5->AddWidget ( "border2", WIDGET_BASIC, 120, 266, 260, 4, "", "" );
	ds5->AddWidget ( "border3", WIDGET_BASIC, 120, 170, 4, 100, "", "" );
	ds5->AddWidget ( "border4", WIDGET_BASIC, 380, 170, 4, 100, "", "" );

    ds5->SetReturnKeyButton ( "OK" );
    ds5->SetEscapeKeyButton ( "Cancel" );

	comp->AddComputerScreen ( ds5, 5 );

	// Screen 6					( View record )

	GenericScreen *gs6 = new GenericScreen ();
	gs6->SetMainTitle ( "International Academic Database" );
	gs6->SetSubTitle ( "View record" );
	gs6->SetNextPage ( 2 );
	gs6->SetScreenType ( SCREEN_ACADEMICSCREEN );
	comp->AddComputerScreen ( gs6, 6 );

	// Add in the accounts

	Record *record = new Record ();
	record->AddField ( RECORDBANK_NAME, RECORDBANK_ADMIN );
	record->AddField ( RECORDBANK_PASSWORD, NameGenerator::GeneratePassword () );
	record->AddField ( RECORDBANK_SECURITY, "1" );
	comp->recordbank.AddRecord ( record );

	Record *record2 = new Record ();
	record2->AddField ( RECORDBANK_NAME, RECORDBANK_READWRITE );
	record2->AddField ( RECORDBANK_PASSWORD, NameGenerator::GeneratePassword () );
	record2->AddField ( RECORDBANK_SECURITY, "2" );
	comp->recordbank.AddRecord ( record2 );

	Record *record3 = new Record ();
	record3->AddField ( RECORDBANK_NAME, RECORDBANK_READONLY );
	record3->AddField ( RECORDBANK_PASSWORD, NameGenerator::GeneratePassword () );
	record3->AddField ( RECORDBANK_SECURITY, "4" );
	comp->recordbank.AddRecord ( record3 );


}

void WorldGenerator::GenerateInterNIC ()
{

	// Create the VLocation

	int x, y;
	GenerateValidMapPos ( x, y );
	game->GetWorld ()->CreateVLocation ( IP_INTERNIC, x, y );

	// Create the computer

	Computer *comp = new Computer ();
	comp->SetTYPE ( COMPUTER_TYPE_UNKNOWN );
	comp->SetName ( "InterNIC" );
	comp->SetCompanyName ( "Government" );
	comp->SetIP ( IP_INTERNIC );
	comp->SetTraceSpeed ( TRACESPEED_INTERNIC );
	comp->SetTraceAction ( COMPUTER_TRACEACTION_DISCONNECT |
						   COMPUTER_TRACEACTION_WARNINGMAIL );
	comp->SetIsTargetable ( false );
	game->GetWorld ()->CreateComputer ( comp );

	// Screen 0						-			Intro screen

	MessageScreen *ms0 = new MessageScreen ();
	ms0->SetMainTitle ( "InterNIC" );
	ms0->SetSubTitle ( "InterNet Information Center" );
	ms0->SetNextPage ( 1 );
	ms0->SetTextMessage ( "Welcome to the Internet Information Center.\n\n"
						  "Use of this service is free and open to all." );
	ms0->SetButtonMessage ( "OK" );
	comp->AddComputerScreen ( ms0, 0 );


	// Screen 1						-			Main menu

	MenuScreen *ms1 = new MenuScreen ();
	ms1->SetMainTitle ( "InterNIC" );
	ms1->SetSubTitle ( "Main menu" );
	ms1->AddOption ( "Browse/Search", "Click here to use a list of all known IP's", 2 );
	ms1->AddOption ( "Admin", "Click here to modify the database", 3 );
	comp->AddComputerScreen  ( ms1, 1 );


	// Screen 2						-			Full list

	LinksScreen *ls2 = new LinksScreen ();
	ls2->SetMainTitle ( "InterNIC" );
	ls2->SetSubTitle ( "Search list" );
	ls2->SetScreenType ( LINKSSCREENTYPE_ALLLINKS );
	ls2->SetNextPage ( 1 );
	comp->AddComputerScreen ( ls2, 2 );

	// Screen 3						-			Admin password

	PasswordScreen *ps3 = new PasswordScreen ();
	ps3->SetMainTitle ( "InterNIC" );
	ps3->SetSubTitle ( "Password authorisation required" );
	ps3->SetNextPage ( 4 );
	ps3->SetPassword ( NameGenerator::GeneratePassword () );
	ps3->SetDifficulty ( HACKDIFFICULTY_INTERNIC );
	comp->AddComputerScreen ( ps3, 3 );

	// Screen 4						-			Admin menu

	MenuScreen *ms4 = new MenuScreen ();
	ms4->SetMainTitle ( "InterNIC" );
	ms4->SetSubTitle ( "Admin menu" );
	ms4->AddOption ( "Access Logs", "Click here to view access logs", 5 );
	ms4->AddOption ( "Exit", "Return to the main menu", 1 );
	comp->AddComputerScreen ( ms4, 4 );

	// Screen 5						-			Logs

	LogScreen *ls5 = new LogScreen ();
	ls5->SetMainTitle ( "InterNIC" );
	ls5->SetSubTitle ( "Log Screen" );
	ls5->SetTARGET ( LOGSCREEN_TARGET_ACCESSLOGS );
	ls5->SetNextPage ( 4 );
	comp->AddComputerScreen ( ls5, 5 );


}

void WorldGenerator::GenerateStockMarket ()
{

	// Create the VLocation

	int x, y;
	GenerateValidMapPos ( x, y );
	game->GetWorld ()->CreateVLocation ( IP_STOCKMARKETSYSTEM, x, y );

	// Create the computer

	Computer *comp = new Computer ();
	comp->SetTYPE ( COMPUTER_TYPE_UNKNOWN );
	comp->SetName ( "International Stock Market System" );
	comp->SetCompanyName ( "Government" );
	comp->SetIP ( IP_STOCKMARKETSYSTEM );
	comp->SetTraceSpeed ( TRACESPEED_STOCKMARKET );
	comp->SetTraceAction ( COMPUTER_TRACEACTION_DISCONNECT |
						   COMPUTER_TRACEACTION_LEGAL );
	comp->SetIsTargetable ( false );
	comp->security.AddSystem ( SECURITY_TYPE_PROXY, 1 );
	comp->security.AddSystem ( SECURITY_TYPE_MONITOR, 1 );
	game->GetWorld ()->CreateComputer ( comp );


	// Screen 0									( Intro )

    std::ostrstream txtmsg;
    txtmsg << "Welcome to the International Stock Market trading System.\n"
			  "This system is open to all, however you will need an access "
			  "account if you wish to trade in shares.\n\n"
			  "Select 'new user' from the menu to create a new account.\n"
			  "Log in as 'guest', password 'guest' to browse share prices.\n"
			  "Otherwise, use your provided log in name and password.\n\n";

#ifdef DEMOGAME
    txtmsg << "Demo Uplink clients will not be permitted to trade shares.";
#endif

    txtmsg << '\x0';

	MessageScreen *ms0 = new MessageScreen ();
	ms0->SetMainTitle ( comp->name );
	ms0->SetSubTitle ( "Welcome" );
	ms0->SetTextMessage ( txtmsg.str () );
	ms0->SetButtonMessage ( "OK" );
	ms0->SetNextPage ( 1 );
	comp->AddComputerScreen ( ms0, 0 );

    //delete [] txtmsg.str ();
	txtmsg.rdbuf()->freeze( 0 );

	// Screen 1									( Main Menu )

	MenuScreen *ms1 = new MenuScreen ();
	ms1->SetMainTitle ( comp->name );
	ms1->SetSubTitle ( "Main Menu" );
	ms1->AddOption ( "Log in", "Log in using an existing username and password", 4 );

#ifndef DEMOGAME
    ms1->AddOption ( "New User", "Create a new user account to trade in shares", 2 );
#endif

	ms1->AddOption ( "Information", "Click for help information about this computer", 0 );
	comp->AddComputerScreen ( ms1, 1 );

	// Screen 2									( New user )

	DialogScreen *ds2 = new DialogScreen ();
	ds2->SetMainTitle ( comp->name );
	ds2->SetSubTitle ( "New User" );

	ds2->AddWidget ( "name",      WIDGET_BASIC, 80, 180, 170, 20, "Enter your name", "This is your name" );
	ds2->AddWidget ( "password",  WIDGET_BASIC, 80, 210, 170, 20, "Enter your password", "You'll need this to access your account" );
	ds2->AddWidget ( "password2", WIDGET_BASIC, 80, 240, 170, 20, "Re-type your password", "For verification purposes" );

	ds2->AddWidget ( "nametext",      WIDGET_TEXTBOX,     270, 180, 170, 20, "Fill this in", "Enter your name here" );
	ds2->AddWidget ( "passwordtext",  WIDGET_PASSWORDBOX, 270, 210, 170, 20, "", "Enter your password here" );
	ds2->AddWidget ( "passwordtext2", WIDGET_PASSWORDBOX, 270, 240, 170, 20, "", "Re-type your password here" );

	ds2->AddWidget ( "create", WIDGET_SCRIPTBUTTON, 250, 330, 80, 20, "Create", "Create this account", 12, -1, "", "" );
	ds2->AddWidget ( "cancel", WIDGET_NEXTPAGE, 350, 330, 80, 20, "Cancel", "Cancel", 1, -1, "", "" );

	comp->AddComputerScreen ( ds2, 2 );

	// Screen 3									( New user created )

	MessageScreen *ms3 = new MessageScreen ();
	ms3->SetMainTitle ( comp->name );
	ms3->SetSubTitle ( "New user created" );
	ms3->SetTextMessage ( "Your new user account has been created successfully.\n"
						  "You are now authorised to trade shares on this system." );
	ms3->SetButtonMessage ( "OK" );
	ms3->SetNextPage ( 5 );
	comp->AddComputerScreen ( ms3, 3 );

	// Screen 4									( Log in )

	UserIDScreen *uid4 = new UserIDScreen ();
	uid4->SetMainTitle ( comp->name );
	uid4->SetSubTitle ( "Enter username and password" );
	uid4->SetDifficulty ( HACKDIFFICULTY_STOCKMARKET );
	uid4->SetNextPage ( 5 );
	comp->AddComputerScreen ( uid4, 4 );

	// Screen 5									( User menu )

	MenuScreen *ms5 = new MenuScreen ();
	ms5->SetMainTitle ( comp->name );
	ms5->SetSubTitle ( "User Menu" );
	ms5->AddOption ( "Browse shares", "Browse a list of all shares available", 7 );
	ms5->AddOption ( "Admin", "View the admin menu", 8, 1 );
	comp->AddComputerScreen ( ms5, 5 );

	// Screen 6									( View my portfolio )

	// Screen 7									( Browse shares )

	SharesListScreen *sls7 = new SharesListScreen ();
	sls7->SetMainTitle ( comp->name );
	sls7->SetSubTitle ( "Browse Shares" );
	sls7->SetScreenType ( SHARESLISTSCREENTYPE_ALLSHARES );
	sls7->SetNextPage ( 5 );
	sls7->SetViewPage ( 11 );
	comp->AddComputerScreen ( sls7, 7 );

	// Screen 8									( Admin menu )

	MenuScreen *ms8 = new MenuScreen ();
	ms8->SetMainTitle ( comp->name );
	ms8->SetSubTitle ( "Admin menu" );
#ifdef TESTGAME
	ms8->AddOption ( "View records", "View all records on this system", 9 );
#endif
	ms8->AddOption ( "View logs", "View all access logs on this system", 10 );
	ms8->AddOption ( "Done", "Return to the user menu", 5 );
	comp->AddComputerScreen ( ms8, 8 );

	// Screen 9									( Records )

	GenericScreen *rs9 = new GenericScreen ();
	rs9->SetScreenType ( SCREEN_RECORDSCREEN );
	rs9->SetMainTitle ( comp->name );
	rs9->SetSubTitle ( "Records" );
	rs9->SetNextPage ( 8 );
	comp->AddComputerScreen ( rs9, 9 );

	// Screen 10								( Logs )

	LogScreen *ls10 = new LogScreen ();
	ls10->SetTARGET ( LOGSCREEN_TARGET_ACCESSLOGS );
	ls10->SetMainTitle ( comp->name );
	ls10->SetSubTitle ( "Access logs" );
	ls10->SetNextPage ( 8 );
	comp->AddComputerScreen ( ls10, 10 );

	// Screen 11								( View a share )

	GenericScreen *gs11 = new GenericScreen ();
	gs11->SetScreenType ( SCREEN_SHARESVIEWSCREEN );
	gs11->SetMainTitle ( comp->name );
	gs11->SetSubTitle ( "View share details" );
	gs11->SetNextPage ( 7 );
	comp->AddComputerScreen ( gs11, 11 );


	// Create some logins

	Record *record = new Record ();
	record->AddField ( RECORDBANK_NAME, RECORDBANK_ADMIN );
	record->AddField ( RECORDBANK_PASSWORD, NameGenerator::GeneratePassword () );
	record->AddField ( RECORDBANK_SECURITY, "1" );
	comp->recordbank.AddRecord ( record );

	Record *record2 = new Record ();
	record2->AddField ( RECORDBANK_NAME, "guest" );
	record2->AddField ( RECORDBANK_PASSWORD, "guest" );
	record2->AddField ( RECORDBANK_SECURITY, "10" );
	comp->recordbank.AddRecord ( record2 );

}

void WorldGenerator::GenerateProtoVision ()
{

	// Create the VLocation

	int x, y;
	GenerateValidMapPos ( x, y );
	game->GetWorld ()->CreateVLocation ( IP_PROTOVISION, x, y );

	// Create the company

	Company *company = GenerateCompany ( "Protovision", 20, COMPANYTYPE_COMMERCIAL, 10, 0 );
	company->SetBoss ( "Dr. Falken" );

	// Create the computer

	Computer *comp = new Computer ();
	comp->SetTYPE ( COMPUTER_TYPE_UNKNOWN );
	comp->SetName ( "Protovision Game Server" );
	comp->SetCompanyName ( "Protovision" );
	comp->SetIP ( IP_PROTOVISION );
	comp->SetTraceSpeed ( TRACESPEED_PROTOVISION );
	comp->SetTraceAction ( COMPUTER_TRACEACTION_DISCONNECT |
						   COMPUTER_TRACEACTION_LEGAL );
	comp->SetIsTargetable ( false );
	game->GetWorld ()->CreateComputer ( comp );

	// Screen 0					Password ID

	PasswordScreen *ps0 = new PasswordScreen ();
	ps0->SetMainTitle ( "Protovision" );
	ps0->SetSubTitle ( "Game Server" );
	ps0->SetDifficulty ( HACKDIFFICULTY_PROTOVISION );
	ps0->SetPassword ( "joshua" );
	ps0->SetNextPage ( 1 );
	comp->AddComputerScreen ( ps0 );

	// Screen 1					Main menu

	MenuScreen *ms1 = new MenuScreen ();
	ms1->SetMainTitle ( "Protovision" );
	ms1->SetSubTitle ( "Main Menu" );
	ms1->AddOption ( "WarGames", " ", 2, 1 );
	ms1->AddOption ( "Hints", " ", 3, 1 );
	comp->AddComputerScreen ( ms1 );

	// Screen 2					WarGames

	GenericScreen *ms2 = new GenericScreen ();
	ms2->SetScreenType ( SCREEN_PROTOVISION );
    ms2->SetMainTitle ( "Protovision" );
	ms2->SetSubTitle ( "WarGames" );
	ms2->SetNextPage ( 1 );
	comp->AddComputerScreen ( ms2 );

	// Screen 3					New Games

	MessageScreen *ms3 = new MessageScreen ();
	ms3->SetMainTitle ( "Protovision" );
	ms3->SetSubTitle ( "Hints" );
	ms3->SetTextMessage	( "Getting warmmmmm ;)\n"
                          "What was the IP of Introversion Software again?" );
	ms3->SetButtonMessage ( "OK" );
	ms3->SetNextPage ( 1 );
	comp->AddComputerScreen ( ms3 );

    // Screen 4                 Global Thermonuclear War

    GenericScreen *gs4 = new GenericScreen ();
    gs4->SetScreenType ( SCREEN_NUCLEARWAR );
    gs4->SetMainTitle ( "Protovision" );
    gs4->SetSubTitle ( "Global Thermonuclear War" );
    gs4->SetNextPage ( 1 );
    comp->AddComputerScreen ( gs4 );

}

void WorldGenerator::GenerateOCP ()
{

	// Create the VLocation

	int x, y;
	GenerateValidMapPos ( x, y );
	game->GetWorld ()->CreateVLocation ( IP_OCP, x, y );

	// Create the company

    Company *company = new Company ();
	company->SetName ( "OCP" );
	company->SetSize ( 20 );
	company->SetTYPE ( COMPANYTYPE_COMMERCIAL );
	company->SetGrowth ( 10 );
	company->SetAlignment ( -30 );
	game->GetWorld ()->CreateCompany ( company );
    company->Grow(0);

	// Create the computer

	Computer *comp = new Computer ();
	comp->SetTYPE ( COMPUTER_TYPE_UNKNOWN );
	comp->SetName ( "OCP remote monitoring system" );
	comp->SetCompanyName ( "OCP" );
	comp->SetIP ( IP_OCP );
	comp->SetIsTargetable ( false );
	game->GetWorld ()->CreateComputer ( comp );

	// Screen 1					Message

    MessageScreen *ms0 = new MessageScreen ();
    ms0->SetMainTitle ( "OCP" );
    ms0->SetSubTitle ( "Remote Monitoring System" );
    ms0->SetTextMessage ( "\n\n\n\n\n\n\nServe the public trust\n"
                          "Protect the innocent\n"
                          "Uphold the law\n"
                          "(Classified)" );

    comp->AddComputerScreen ( ms0 );

	// Generate contact addresses for this company

	char contactcomputer [SIZE_COMPUTER_NAME];
	UplinkStrncpy ( contactcomputer, "OCP remote monitoring system", sizeof ( contactcomputer ) );
	Computer *contact = game->GetWorld ()->GetComputer ( contactcomputer );
	UplinkAssert (contact);

	char personname [SIZE_PERSON_NAME];
	UplinkStrncpy ( personname, "internal@OCP.net", sizeof ( personname ) );
	Person *pinternal = game->GetWorld ()->CreatePerson ( personname, contact->ip );
	pinternal->SetIsTargetable ( false );

}

void WorldGenerator::GenerateSJGames ()
{

	// Create the VLocation

	int x, y;
	GenerateValidMapPos ( x, y );
	game->GetWorld ()->CreateVLocation ( IP_SJGAMES, x, y );

	// Create the company

	Company *company = new Company ();
	company->SetName ( "Steve Jackson Games" );
	company->SetSize ( 3 );
	company->SetTYPE ( COMPANYTYPE_COMMERCIAL );
	company->SetGrowth ( 1 );
	company->SetAlignment ( 0 );
	game->GetWorld ()->CreateCompany ( company );
    company->Grow(0);

	// Create the computer

	Computer *comp = new Computer ();
	comp->SetTYPE ( COMPUTER_TYPE_UNKNOWN );
	comp->SetName ( "Steve Jackson Games Server" );
	comp->SetCompanyName ( "Steve Jackson Games" );
	comp->SetIP ( IP_SJGAMES );
	comp->SetIsTargetable ( false );
	game->GetWorld ()->CreateComputer ( comp );

	// Screen 1					Message

    MessageScreen *ms0 = new MessageScreen ();
    ms0->SetMainTitle ( "Steve Jackson Games" );
    ms0->SetSubTitle ( "Public Access Server" );
    ms0->SetTextMessage ( "ATTENTION\n\n"
                          "This computer system has been seized\n"
                          "by the United States Secret Service\n"
                          "in the interests of National Security.\n\n\n"
                          "Your IP has been logged." );

    comp->AddComputerScreen ( ms0 );

	// Generate contact addresses for this company

	char contactcomputer [SIZE_COMPUTER_NAME];
	UplinkStrncpy ( contactcomputer, "Steve Jackson Games Server", sizeof ( contactcomputer ) );
	Computer *contact = game->GetWorld ()->GetComputer ( contactcomputer );
	UplinkAssert (contact);

	char personname [SIZE_PERSON_NAME];
	UplinkStrncpy ( personname, "internal@Steve Jackson Games.net", sizeof ( personname ) );
	Person *pinternal = game->GetWorld ()->CreatePerson ( personname, contact->ip );
	pinternal->SetIsTargetable ( false );

}

void WorldGenerator::GenerateIntroversion ()
{

	// Create the VLocation

	int x, y;
	GenerateValidMapPos ( x, y );
	VLocation *vl = game->GetWorld ()->CreateVLocation ( IP_INTROVERSION, x, y );
    vl->SetListed ( false );

	// Create the company

	Company *company = new Company ();
	company->SetName ( "Introversion Software" );
	company->SetSize ( 10 );
	company->SetTYPE ( COMPANYTYPE_COMMERCIAL );
	company->SetGrowth ( 50 );
	company->SetAlignment ( 0 );
	game->GetWorld ()->CreateCompany ( company );
    company->Grow(0);

	// Create the computer

	Computer *comp = new Computer ();
	comp->SetTYPE ( COMPUTER_TYPE_UNKNOWN );
	comp->SetName ( "Introversion Software" );
	comp->SetCompanyName ( "Introversion Software" );
	comp->SetIP ( IP_INTROVERSION );
	comp->SetIsTargetable ( false );
	game->GetWorld ()->CreateComputer ( comp );

	// Screen 0					Welcome Message

    MessageScreen *ms0 = new MessageScreen ();
    ms0->SetMainTitle ( "Introversion Software" );
    ms0->SetSubTitle ( "Internal Services" );
    ms0->SetTextMessage ( "Welcome, employee\n"
                          "All design documents relating to the forthcoming Introversion title\n"
                          "codenamed 'Uplink2: TERMINAL' are to be copied to this site\n"
                          "for easy access by all employees.\n\n"
                          "All data on this site is strictly confidential." );
    ms0->SetButtonMessage ( "OK" );
    ms0->SetNextPage ( 1 );
    comp->AddComputerScreen ( ms0, 0 );

    // Screen 1                 Password

    PasswordScreen *ps1 = new PasswordScreen ();
    ps1->SetMainTitle ( "Introversion Software" );
    ps1->SetSubTitle ( "Enter password" );
    ps1->SetPassword ( NameGenerator::GenerateComplexPassword () );
    ps1->SetDifficulty ( HACKDIFFICULTY_UPLINK_TESTMACHINE );
    ps1->SetNextPage ( 2 );
    comp->AddComputerScreen ( ps1, 1 );

    // Screen 2                 Menu

    MenuScreen *ms2 = new MenuScreen ();
    ms2->SetMainTitle ( "Introversion Software" );
    ms2->SetSubTitle ( "Project Uplink2 : TERMINAL" );
    ms2->AddOption ( "Introduction", "Introduction", 3 );
    ms2->AddOption ( "Uplink", "Uplink", 4 );
    ms2->AddOption ( "Mission: Impossible", "Mission: Impossible", 5 );
    ms2->AddOption ( "Johnny Mnemonic", "Johnny Mnemonic", 6 );
    ms2->AddOption ( "The Holy Grail", "The Holy Grail", 8 );
    ms2->AddOption ( "Other", "Other", 7 );
    comp->AddComputerScreen ( ms2, 2 );

    // Screen 3                 Introduction

    MessageScreen *ms3 = new MessageScreen ();
    ms3->SetMainTitle ( "Introversion Software" );
    ms3->SetSubTitle ( "Introduction" );
    ms3->SetTextMessage ( "The aim of this site is to serve as a repository for "
                          "all ideas and documents relating to the design of the next "
                          "Uplink product, to be released by Introversion Software.\n\n"
                          "Currently nothing is finalised, other than the sequel to Uplink "
                          "must be a hacking tool of a similar nature to Uplink itself.\n\n"
                          "If any employees have any good ideas, please email them to the "
                          "company designer.  Please use his real-world address, as his "
                          "Uplink email address is under almost constant surveillance from "
                          "federal agents and hackers, and any good ideas will no doubt be stolen.\n\n"
                          "Contact Chris at chris@introversion.co.uk" );
    ms3->SetButtonMessage ( "OK" );
    ms3->SetNextPage ( 2 );
    comp->AddComputerScreen ( ms3, 3 );

    // Screen 4                 Uplink

    MessageScreen *ms4 = new MessageScreen ();
    ms4->SetMainTitle ( "Introversion Software" );
    ms4->SetSubTitle ( "Uplink" );
    ms4->SetTextMessage ( "At some point I would like one of our members to perform a "
                          "postmortem on Uplink itself, and come up with a list of possible improvements.\n\n"
                          "<todo>" );
    ms4->SetButtonMessage ( "OK" );
    ms4->SetNextPage ( 2 );
    comp->AddComputerScreen ( ms4, 4 );

    // Screen 5                  Mission: Impossible

    MessageScreen *ms5 = new MessageScreen ();
    ms5->SetMainTitle ( "Introversion Software" );
    ms5->SetSubTitle ( "Mission: Impossible" );
    ms5->SetTextMessage ( "One possible direction for Uplink to go is shown in the film Mission:Impossible, "
                          "starring Tom Cruise when he was a much younger actor and long before his terrible accident.\n\n"
                          "This film came out in the late 20th century, but the basic idea was that a squad of real people "
                          "would enter the target site (eg a bank, corporation HQ etc) and they would be assisted by "
                          "a tech guy on the outside.  The tech guy would be running Uplink2, and he would be able to see "
                          "the agents moving around the target site.  He would have voice control over his squad.  He would "
                          "need to open doors, block elevator shafts, override building security, distract guards etc.\n\n"
                          "Success would attract agents with better skills who could rob more advanced buildings and sites. "
                          "Failure would lead to the arrest of the Agents under the guys command." );
    ms5->SetButtonMessage ( "OK" );
    ms5->SetNextPage ( 2 );
    comp->AddComputerScreen ( ms5, 5 );

    // Screen 6                 Johnny Mnemonic

    MessageScreen *ms6 = new MessageScreen ();
    ms6->SetMainTitle ( "Introversion Software" );
    ms6->SetSubTitle ( "Johnny Mnemonic" );
    ms6->SetTextMessage ( "Around 15 years ago a film was released in what was then called Hollywood, called Johnny Mnemonic. "
                          "The film was almost universally hated, however it did contain some fantastic visulisations of the "
                          "Internet of 2010.  It is frequently hilarious to see how inaccurate those predictions were.\n\n"
                          "Nevertheless, Uplink2 would look fantastic with a similar look. The Internet would be rendered in 3D, "
                          "with major corporate computer systems appearing as huge 3D sky-scrapers.  Every tool and action would "
                          "be extremely visual.\n\n"
                          "It is thought that this feel would give Uplink2 a broader appeal, but would alienate the hard-core Uplink "
                          "player who values its (semi) realism. In addition, it would require a team of artists to produce the 3d models." );
    ms6->SetButtonMessage ( "OK" );
    ms6->SetNextPage ( 2 );
    comp->AddComputerScreen ( ms6, 6 );

    // Screen 7                 Other

    MessageScreen *ms7 = new MessageScreen ();
    ms7->SetMainTitle ( "Introversion Software" );
    ms7->SetSubTitle ( "Other ideas" );
    ms7->SetTextMessage ( "Assasination mission. The target is flying on a chartered flight. Agent has to hack into airport "
                          "system and find out which flight the target is on. Then hack into air-traffic control and cause "
                          "the plane to crash.\n\n"
                          "Power grids. A terrorist group is planning to attack a bank, and will cut you in for 10 percent "
                          "if you can cut the power to the bank vault at just the right moment.\n\n"
                          "Remote control security drone.  The agent takes control of a security drone, viewing the world "
                          "through a black-and-white CCTV camera in the room. \n\n"
                          "The agent takes control of a government owned satellite weapon, and uses it to destroy a target." );
    ms7->SetButtonMessage ( "OK ");
    ms7->SetNextPage ( 2 );
    comp->AddComputerScreen ( ms7, 7 );

    // Screen 8                 The Holy Grail

    MessageScreen *ms8 = new MessageScreen ();
    ms8->SetMainTitle ( "Introversion Software" );
    ms8->SetSubTitle ( "The Holy Grail" );
    ms8->SetTextMessage ( "The founder of Introversion Software believes the definitive version of Uplink could only be achieved "
                          "as an online massively-muliplayer universe.  When a player launches Uplink2, it would connect to a global "
                          "network of Uplink2 clients.  Each agent would be able to attack other Agent computer systems and steal "
                          "things from them.  A number of 'core' systems would be run by Introversion Software, including most of the "
                          "Government systems.  Agents would need to work in teams to properly hack the well guarded systems.\n\n"
                          "Agents would also need to defend their own systems from hackers.  In effect, it would be similar in design "
                          "to Napster - everyone can see everyone else on the Uplink Network.\n\n"
                          "It is not clear what the legal situation would be - could anyone actually be prosecuted under this system?" );
    ms8->SetButtonMessage ( "Dream on" );
    ms8->SetNextPage ( 2 );
    comp->AddComputerScreen ( ms8, 8 );


	// Generate contact addresses for this company

	char contactcomputer [SIZE_COMPUTER_NAME];
	UplinkStrncpy ( contactcomputer, "Introversion Software", sizeof ( contactcomputer ) );
	Computer *contact = game->GetWorld ()->GetComputer ( contactcomputer );
	UplinkAssert (contact);

	char personname [SIZE_PERSON_NAME];
	UplinkStrncpy ( personname, "internal@Introversion Software.net", sizeof ( personname ) );
	Person *pinternal = game->GetWorld ()->CreatePerson ( personname, contact->ip );
	pinternal->SetIsTargetable ( false );

}

VLocation *WorldGenerator::GenerateLocation ()
{

	int x, y;
	GenerateValidMapPos ( x, y );

	char ip [SIZE_VLOCATION_IP];
	UplinkSnprintf ( ip, sizeof ( ip ), "%d.%d.%d.%d", NumberGenerator::RandomNumber (1000), NumberGenerator::RandomNumber (1000), NumberGenerator::RandomNumber (1000), NumberGenerator::RandomNumber (1000) );

	VLocation *vl = new VLocation ();
	vl->SetPLocation ( x, y );
	vl->SetIP ( ip );
	game->GetWorld ()->CreateVLocation ( vl );

	return vl;

}

Company *WorldGenerator::GenerateCompany ( char *companyname, int size, int TYPE, int growth, int alignment )
{

	Company *company = new Company ();
	company->SetName ( companyname );
	company->SetSize ( size );
	company->SetTYPE ( TYPE );
	company->SetGrowth ( growth );
	company->SetAlignment ( alignment );

	game->GetWorld ()->CreateCompany ( company );

	// Generate the computers owned by this company

	GeneratePublicAccessServer ( companyname );
	GenerateInternalServicesMachine ( companyname );
	GenerateCentralMainframe ( companyname );
    GenerateLAN ( companyname );

	for ( int i = 0; i < size/20; ++i )
		GenerateComputer ( companyname );

	// Generate contact addresses for this company

    char *contactcomputer = NameGenerator::GeneratePublicAccessServerName( companyname );
	Computer *comp = game->GetWorld ()->GetComputer ( contactcomputer );
	UplinkAssert (comp);

	char personname [SIZE_PERSON_NAME];
	UplinkSnprintf ( personname, sizeof ( personname ), "internal@%s.net", companyname );
	Person *pinternal = game->GetWorld ()->CreatePerson ( personname, comp->ip );
	pinternal->SetIsTargetable ( false );

	// Set up our share prices for the start

	company->Grow ( 0 );

	return company;

}

Company *WorldGenerator::GenerateCompany ()
{

	// Generate the company
	char companyname [SIZE_COMPANY_NAME];
	UplinkStrncpy ( companyname, NameGenerator::GenerateCompanyName (), sizeof ( companyname ) );

	int size      = (int) NumberGenerator::RandomNormalNumber ( COMPANYSIZE_AVERAGE,      COMPANYSIZE_RANGE );
	int growth    = (int) NumberGenerator::RandomNormalNumber ( COMPANYGROWTH_AVERAGE,    COMPANYGROWTH_RANGE );
	int alignment = (int) NumberGenerator::RandomNormalNumber ( COMPANYALIGNMENT_AVERAGE, COMPANYALIGNMENT_RANGE );

	Company *company = GenerateCompany ( companyname, size, COMPANYTYPE_COMMERCIAL, growth, alignment );

	return company;

}

Company *WorldGenerator::GenerateCompany_Bank ()
{

	char companyname [SIZE_COMPANY_NAME];
    UplinkStrncpy ( companyname, NameGenerator::GenerateBankName (), sizeof ( companyname ) );

	int size      = (int) NumberGenerator::RandomNormalNumber ( 4, 4 );
	int growth    = (int) NumberGenerator::RandomNormalNumber ( 10, 20 );
	int alignment = (int) NumberGenerator::RandomNormalNumber ( 0, 50 );

	Company *company = GenerateCompany ( companyname, size, COMPANYTYPE_FINANCIAL, growth, alignment );

	GeneratePublicBankServer ( company->name );

	return company;

}

Computer *WorldGenerator::GenerateComputer ( char *companyname )
{

    //char *computername = strdup( NameGenerator::GenerateAccessTerminalName( companyname ) );
	char computername[MAX_COMPUTERNAME];
	strncpy( computername, NameGenerator::GenerateAccessTerminalName( companyname ),
		     MAX_COMPUTERNAME );
	if ( computername[MAX_COMPUTERNAME - 1] != '\0' ) {
		computername[MAX_COMPUTERNAME - 1] = '\0';
	}

	VLocation *vl = GenerateLocation ();

	Computer *comp = new Computer ();
	comp->SetName ( computername );
	comp->SetCompanyName ( companyname );
	comp->SetTraceSpeed ( 20 );
	comp->SetTraceAction ( COMPUTER_TRACEACTION_DISCONNECT );
	comp->SetIsTargetable ( false );
	comp->SetIP ( vl->ip );

	MessageScreen *ms = new MessageScreen ();
	ms->SetMainTitle ( companyname );
	ms->SetSubTitle ( "Access Terminal" );
	ms->SetTextMessage ( "No public log-ons permitted" );
	comp->AddComputerScreen ( ms );

	game->GetWorld ()->CreateComputer ( comp );

    //delete computername;
	return comp;

}

Computer  *WorldGenerator::GenerateComputer ( char *companyName, int TYPE )
{

    if ( TYPE & COMPUTER_TYPE_PUBLICACCESSSERVER )                  return GeneratePublicAccessServer ( companyName );
    else if ( TYPE & COMPUTER_TYPE_INTERNALSERVICESMACHINE )        return GenerateInternalServicesMachine ( companyName );
    else if ( TYPE & COMPUTER_TYPE_CENTRALMAINFRAME )               return GenerateCentralMainframe ( companyName );
    else if ( TYPE & COMPUTER_TYPE_PUBLICBANKSERVER )               return GeneratePublicBankServer ( companyName );
    else if ( TYPE & COMPUTER_TYPE_PERSONALCOMPUTER )               return GeneratePersonalComputer ( companyName );
    else if ( TYPE & COMPUTER_TYPE_VOICEPHONESYSTEM )               return GenerateVoicePhoneSystem ( companyName );
    else if ( TYPE & COMPUTER_TYPE_LAN )                            return GenerateLAN ( companyName );
    else
        return NULL;

}

Computer *WorldGenerator::GeneratePublicAccessServer ( char *companyname )
{

    //char *computername = strdup( NameGenerator::GeneratePublicAccessServerName( companyname ) );
	char computername[MAX_COMPUTERNAME];
	strncpy( computername, NameGenerator::GeneratePublicAccessServerName( companyname ),
		     MAX_COMPUTERNAME );
	if ( computername[MAX_COMPUTERNAME - 1] != '\0' ) {
		computername[MAX_COMPUTERNAME - 1] = '\0';
	}


	VLocation *vl = GenerateLocation ();

	Computer *comp = new Computer ();
	comp->SetTYPE ( COMPUTER_TYPE_PUBLICACCESSSERVER );
	comp->SetName ( computername );
	comp->SetCompanyName ( companyname );
	comp->SetTraceSpeed ( (int) NumberGenerator::RandomNormalNumber ( TRACESPEED_PUBLICACCESSSERVER,
	                                                                  (float) ( TRACESPEED_PUBLICACCESSSERVER * TRACESPEED_VARIANCE ) ) );
	comp->SetIP ( vl->ip );

	MessageScreen *ms = new MessageScreen ();
	ms->SetMainTitle ( companyname );
	ms->SetSubTitle ( "Public Access Server" );
	ms->SetTextMessage ( computername );
	ms->SetButtonMessage ( "OK" );
	ms->SetNextPage ( 1 );
	comp->AddComputerScreen ( ms, 0 );

	GenericScreen *gs = new GenericScreen ();
	gs->SetMainTitle ( companyname );
	gs->SetSubTitle ( "Company data" );
	gs->SetScreenType ( SCREEN_COMPANYINFO );
	comp->AddComputerScreen ( gs, 1 );

	game->GetWorld ()->CreateComputer ( comp );

    //free( computername );

	return comp;

}

Computer *WorldGenerator::GenerateInternalServicesMachine ( char *companyname )
{

    //char *computername = strdup( NameGenerator::GenerateInternalServicesServerName( companyname ) );
	char computername[MAX_COMPUTERNAME];
	strncpy( computername, NameGenerator::GenerateInternalServicesServerName( companyname ),
		     MAX_COMPUTERNAME );
	if ( computername[MAX_COMPUTERNAME - 1] != '\0' ) {
		computername[MAX_COMPUTERNAME - 1] = '\0';
	}

	Company *company = game->GetWorld ()->GetCompany ( companyname );
	UplinkAssert (company);

	// Create the location

	VLocation *vl = GenerateLocation ();

	// Create the computer

	Computer *comp = new Computer ();
	comp->SetTYPE ( COMPUTER_TYPE_INTERNALSERVICESMACHINE );
	comp->SetName ( computername );
	comp->SetCompanyName ( companyname );
	comp->SetTraceSpeed ( (int) NumberGenerator::RandomNormalNumber ( TRACESPEED_INTERNALSERVICESMACHINE,
	                                                                  (float) ( TRACESPEED_INTERNALSERVICESMACHINE * TRACESPEED_VARIANCE ) ) );
	comp->SetIP ( vl->ip );

    //delete computername;

    if ( company->size > MINCOMPANYSIZE_MONITOR ) {
        int monitorstrength = (company->size - MINCOMPANYSIZE_MONITOR) / 3;
        if ( monitorstrength > 5 ) monitorstrength = 5;
        if ( monitorstrength < 1 ) monitorstrength = 1;
        comp->security.AddSystem ( SECURITY_TYPE_MONITOR, monitorstrength );
    }

    if ( company->size > MINCOMPANYSIZE_PROXY ) {
        int proxystrength = (company->size - MINCOMPANYSIZE_PROXY) / 3;
        if ( proxystrength > 5 ) proxystrength = 5;
        if ( proxystrength < 1 ) proxystrength = 1;
        comp->security.AddSystem ( SECURITY_TYPE_PROXY, proxystrength );
    }

    if ( company->size > MINCOMPANYSIZE_FIREWALL ) {
        int firewallstrength = (company->size - MINCOMPANYSIZE_FIREWALL) / 3;
        if ( firewallstrength > 5 ) firewallstrength = 5;
        if ( firewallstrength < 1 ) firewallstrength = 1;
        comp->security.AddSystem ( SECURITY_TYPE_FIREWALL, firewallstrength );
    }


    if ( company->size < MAXCOMPANYSIZE_WARNINGMAIL )
        comp->SetTraceAction ( COMPUTER_TRACEACTION_DISCONNECT |
                               COMPUTER_TRACEACTION_WARNINGMAIL );

    else if ( company->size < MAXCOMPANYSIZE_FINE )
        comp->SetTraceAction ( COMPUTER_TRACEACTION_DISCONNECT |
                               COMPUTER_TRACEACTION_FINE );

    else
       	comp->SetTraceAction ( COMPUTER_TRACEACTION_DISCONNECT |
            				   COMPUTER_TRACEACTION_LEGAL );


	if ( strcmp ( companyname, "Government" ) == 0 )
		comp->SetIsTargetable (false);

	game->GetWorld ()->CreateComputer ( comp );

	// Create the screens

	UserIDScreen *uid = new UserIDScreen ();
	uid->SetMainTitle ( companyname );
	uid->SetSubTitle ( "Log in" );
	uid->SetDifficulty ( (int) NumberGenerator::RandomNormalNumber ( HACKDIFFICULTY_INTERNALSERVICESMACHINE,
	                                                                 (float) ( HACKDIFFICULTY_INTERNALSERVICESMACHINE * HACKDIFFICULTY_VARIANCE ) ) );
	uid->SetNextPage ( 1 );
	comp->AddComputerScreen ( uid, 0 );

	MenuScreen *ms = new MenuScreen ();
	ms->SetMainTitle ( companyname );
	ms->SetSubTitle ( "Internal Services Main Menu" );
	ms->AddOption ( "File Server", "Access the file server", 2 );
    ms->AddOption ( "View records", "View the records stored on this system", 4 );
	ms->AddOption ( "View links", "View all links available on this system", 6 );
	ms->AddOption ( "Admin", "Enter administrative mode", 7, 1 );
	comp->AddComputerScreen ( ms, 1 );

	GenericScreen *fss = new GenericScreen ();
	fss->SetScreenType ( SCREEN_FILESERVERSCREEN );
	fss->SetMainTitle ( companyname );
	fss->SetSubTitle ( "File server" );
	fss->SetNextPage ( 1 );
	comp->AddComputerScreen ( fss, 2 );

	LogScreen *ls = new LogScreen ();
	ls->SetTARGET ( LOGSCREEN_TARGET_ACCESSLOGS );
	ls->SetMainTitle ( companyname );
	ls->SetSubTitle ( "Access Logs" );
	ls->SetNextPage ( 7 );
	comp->AddComputerScreen ( ls, 3 );

	GenericScreen *rs = new GenericScreen ();
	rs->SetScreenType ( SCREEN_RECORDSCREEN );
	rs->SetMainTitle ( companyname );
	rs->SetSubTitle ( "Records" );
	rs->SetNextPage ( 1 );
	comp->AddComputerScreen ( rs, 4 );

	GenericScreen *gs5 = new GenericScreen ();
	gs5->SetScreenType ( SCREEN_SECURITYSCREEN );
	gs5->SetMainTitle ( companyname );
	gs5->SetSubTitle ( "Security" );
	gs5->SetNextPage ( 7 );
	comp->AddComputerScreen ( gs5, 5 );

	LinksScreen *ls6 = new LinksScreen ();
	ls6->SetMainTitle ( companyname );
	ls6->SetSubTitle ( "Links" );
	ls6->SetNextPage ( 1 );
	ls6->SetScreenType ( LINKSSCREENTYPE_LOCALLINKS );
	comp->AddComputerScreen ( ls6, 6 );

	MenuScreen *ms7 = new MenuScreen ();
	ms7->SetMainTitle ( companyname );
	ms7->SetSubTitle ( "Admin menu" );
	ms7->AddOption ( "View logs", "View the access logs on this system", 3, 1 );
	ms7->AddOption ( "Security", "Enable and disable security systems", 5, 1 );
	ms7->AddOption ( "Console", "Run a console", 8, 1 );
	ms7->AddOption ( "Exit", "Return to the main menu", 1 );
	comp->AddComputerScreen ( ms7, 7 );

	GenericScreen *gs8 = new GenericScreen ();
	gs8->SetScreenType ( SCREEN_CONSOLESCREEN );
	gs8->SetMainTitle ( companyname );
	gs8->SetSubTitle ( "Console" );
	gs8->SetNextPage ( 7 );
	comp->AddComputerScreen ( gs8, 8 );

	// Create the files

	int i;

	comp->databank.SetSize ( (int) NumberGenerator::RandomNormalNumber ( 100, 40 ) );

	int numfiles = (int) NumberGenerator::RandomNormalNumber ( 10, 5 );

	for ( i = 0; i < numfiles; ++i ) {

		int TYPE = NumberGenerator::RandomNumber ( 2 ) + 1;
		int size = (int) NumberGenerator::RandomNormalNumber ( 6, 6 );
		int encrypted = NumberGenerator::RandomNumber ( 2 );
		int compressed = NumberGenerator::RandomNumber ( 2 );
		char *datatitle = NameGenerator::GenerateDataName ( companyname, TYPE );

		Data *data = new Data ();
		data->SetTitle ( datatitle );
		data->SetDetails ( TYPE, size, (encrypted ? NumberGenerator::RandomNumber ( 5 ) : 0),
									   (compressed ? NumberGenerator::RandomNumber ( 5 ) : 0) );
		comp->databank.PutData ( data );

	}

	// Create some logs

	int numlogs = NumberGenerator::RandomNumber ( 10 );

	for ( i = 0; i < numlogs; ++i ) {

		AccessLog *al = new AccessLog ();
		al->SetProperties ( &(game->GetWorld ()->date), WorldGenerator::GetRandomLocation ()->ip, " " );
		al->SetData1 ( "Accessed File" );
		comp->logbank.AddLog (al);

	}

	// Create some log-in's

	Record *record = new Record ();
	record->AddField ( RECORDBANK_NAME, RECORDBANK_ADMIN );
	record->AddField ( RECORDBANK_PASSWORD, NameGenerator::GeneratePassword () );
	record->AddField ( RECORDBANK_SECURITY, "1" );
	comp->recordbank.AddRecord ( record );


	return comp;

}

Computer *WorldGenerator::GenerateCentralMainframe ( char *companyname )
{

    //char *computername = strdup( NameGenerator::GenerateCentralMainframeName(companyname) );
	char computername[MAX_COMPUTERNAME];
	strncpy( computername, NameGenerator::GenerateCentralMainframeName( companyname ),
		     MAX_COMPUTERNAME );
	if ( computername[MAX_COMPUTERNAME - 1] != '\0' ) {
		computername[MAX_COMPUTERNAME - 1] = '\0';
	}

    Company *company = game->GetWorld ()->GetCompany (companyname);
    UplinkAssert (company);

	VLocation *vl = GenerateLocation ();
	vl->SetListed ( false );

	Computer *comp = new Computer ();
	comp->SetTYPE ( COMPUTER_TYPE_CENTRALMAINFRAME );
	comp->SetName ( computername );
	comp->SetCompanyName ( companyname );
	comp->SetIsExternallyOpen ( false );
	comp->SetTraceSpeed ( (int) NumberGenerator::RandomNormalNumber ( TRACESPEED_CENTRALMAINFRAME,
	                                                                  (float) ( TRACESPEED_CENTRALMAINFRAME * TRACESPEED_VARIANCE ) ) );
	comp->SetTraceAction ( COMPUTER_TRACEACTION_DISCONNECT |
						   COMPUTER_TRACEACTION_LEGAL );

	comp->SetIP ( vl->ip );

    //delete computername;

    int monitorstrength = (company->size - MINCOMPANYSIZE_MONITOR) / 3;
    if ( monitorstrength > 5 ) monitorstrength = 5;
    if ( monitorstrength < 1 ) monitorstrength = 1;
    comp->security.AddSystem ( SECURITY_TYPE_MONITOR, monitorstrength );

    int proxystrength = (company->size - MINCOMPANYSIZE_PROXY) / 3;
    if ( proxystrength > 5 ) proxystrength = 5;
    if ( proxystrength < 1 ) proxystrength = 1;
    comp->security.AddSystem ( SECURITY_TYPE_PROXY, proxystrength );

    int firewallstrength = (company->size - MINCOMPANYSIZE_FIREWALL) / 3;
    if ( firewallstrength > 5 ) firewallstrength = 5;
    if ( firewallstrength < 1 ) firewallstrength = 1;
    comp->security.AddSystem ( SECURITY_TYPE_FIREWALL, firewallstrength );

	if ( strcmp ( companyname, "Government" ) == 0 )
		comp->SetIsTargetable (false);


	game->GetWorld ()->CreateComputer ( comp );

	// Create the screens

	HighSecurityScreen *hs = new HighSecurityScreen ();
	hs->SetMainTitle ( companyname );
	hs->SetSubTitle ( "Authorisation required" );
	hs->AddSystem ( "UserID / password verification", 1 );
	hs->AddSystem ( "Voice Print Identification", 2 );
	hs->SetNextPage ( 3 );
	comp->AddComputerScreen ( hs, 0 );

	UserIDScreen *uid = new UserIDScreen ();
	uid->SetMainTitle ( companyname );
	uid->SetSubTitle ( "Log in" );
	uid->SetDifficulty ( (int) NumberGenerator::RandomNormalNumber ( HACKDIFFICULTY_CENTRALMAINFRAME,
	                                                                 (float) ( HACKDIFFICULTY_CENTRALMAINFRAME * HACKDIFFICULTY_VARIANCE ) ) );
	uid->SetNextPage ( 0 );
	comp->AddComputerScreen ( uid, 1 );

	GenericScreen *gs2 = new GenericScreen ();
	gs2->SetMainTitle ( companyname );
	gs2->SetSubTitle ( "Voice print analysis required" );
	gs2->SetScreenType ( SCREEN_VOICEANALYSIS );
	gs2->SetNextPage ( 0 );
	comp->AddComputerScreen ( gs2, 2 );

	MenuScreen *ms = new MenuScreen ();
	ms->SetMainTitle ( companyname );
	ms->SetSubTitle ( "Central Mainframe Main Menu" );
	ms->AddOption ( " File Server", "Access the file server", 4, 3 );
	ms->AddOption ( "View logs", "View the access logs on this system", 5, 1 );
	ms->AddOption ( "Console", "Use a console on this system", 6, 1 );
	comp->AddComputerScreen ( ms, 3 );

	GenericScreen *fss = new GenericScreen ();
	fss->SetScreenType ( SCREEN_FILESERVERSCREEN );
	fss->SetMainTitle ( companyname );
	fss->SetSubTitle ( "File server" );
	fss->SetNextPage ( 3 );
	comp->AddComputerScreen ( fss, 4 );

	LogScreen *ls = new LogScreen ();
	ls->SetTARGET ( LOGSCREEN_TARGET_ACCESSLOGS );
	ls->SetMainTitle ( companyname );
	ls->SetSubTitle ( "Access Logs" );
	ls->SetNextPage ( 3 );
	comp->AddComputerScreen ( ls, 5 );

	GenericScreen *gs6 = new GenericScreen ();
	gs6->SetScreenType ( SCREEN_CONSOLESCREEN );
	gs6->SetMainTitle ( companyname );
	gs6->SetSubTitle ( "Console" );
	gs6->SetNextPage ( 3 );
	comp->AddComputerScreen ( gs6, 6 );

    // 7 is taken up on some companies such as ARC and Arunmor

	// Create the files

	int i;

	comp->databank.SetSize ( (int) NumberGenerator::RandomNormalNumber ( 100, 40 ) );

	int numfiles = (int) NumberGenerator::RandomNormalNumber ( 10, 5 );

	for ( i = 0; i < numfiles; ++i ) {

		int TYPE = NumberGenerator::RandomNumber ( 2 ) + 1;
		int size = (int) NumberGenerator::RandomNormalNumber ( 6, 4 );
		int encrypted = (int) NumberGenerator::RandomNumber ( 2 );
		int compressed = NumberGenerator::RandomNumber ( 2 );
		char *datatitle = NameGenerator::GenerateDataName ( companyname, TYPE );

		Data *data = new Data ();
		data->SetTitle ( datatitle );
		data->SetDetails ( TYPE, size, (encrypted ? NumberGenerator::RandomNumber ( 5 ) : 0),
									   (compressed ? NumberGenerator::RandomNumber ( 5 ) : 0) );
		comp->databank.PutData ( data );

	}

	// Create some logs

	int numlogs = NumberGenerator::RandomNumber ( 10 );

	for ( i = 0; i < numlogs; ++i ) {

		AccessLog *al = new AccessLog ();
		al->SetProperties ( &(game->GetWorld ()->date), WorldGenerator::GetRandomLocation ()->ip, " " );
		al->SetData1 ( "Accessed File" );
		comp->logbank.AddLog (al);

	}

	// Create some log-in's

	Record *record = new Record ();
	record->AddField ( RECORDBANK_NAME, RECORDBANK_ADMIN );
	record->AddField ( RECORDBANK_PASSWORD, NameGenerator::GenerateComplexPassword () );
	record->AddField ( RECORDBANK_SECURITY, "1" );
	comp->recordbank.AddRecord ( record );


	return comp;

}

Computer *WorldGenerator::GeneratePublicBankServer ( char *companyname )
{

    //char *computername = strdup( NameGenerator::GenerateInternationalBankName( companyname ) );
	char computername[MAX_COMPUTERNAME];
	strncpy( computername, NameGenerator::GenerateInternationalBankName( companyname ),
		     MAX_COMPUTERNAME );
	if ( computername[MAX_COMPUTERNAME - 1] != '\0' ) {
		computername[MAX_COMPUTERNAME - 1] = '\0';
	}

	VLocation *vl = GenerateLocation ();

	BankComputer *comp = new BankComputer ();
	comp->SetTYPE ( COMPUTER_TYPE_PUBLICBANKSERVER );
	comp->SetName ( computername );
	comp->SetCompanyName ( companyname );
	comp->SetTraceSpeed ( TRACESPEED_PUBLICBANKSERVER );
	comp->SetTraceAction ( COMPUTER_TRACEACTION_DISCONNECT |
						   COMPUTER_TRACEACTION_LEGAL );
	comp->security.AddSystem ( SECURITY_TYPE_PROXY, 5 );
	comp->security.AddSystem ( SECURITY_TYPE_MONITOR, 5 );
	comp->SetIP ( vl->ip );

    //delete computername;

	// Screen 0										(Main Menu)

	MenuScreen *s0 = new MenuScreen ();
	s0->SetMainTitle ( companyname );
	s0->SetSubTitle ( "Welcome to our Online Banking Server" );
	s0->AddOption ( "About Us", "About us", 1 );
	s0->AddOption ( "Create New Account", "Create new account", 4 );
	s0->AddOption ( "Manage Existing Account", "Manage existing account", 2 );
	s0->AddOption ( "Administration", "Access admin features", 6 );
	comp->AddComputerScreen ( s0, 0 );

	// Screen 1										(About us)

	GenericScreen *gs = new GenericScreen ();
	gs->SetMainTitle ( companyname );
	gs->SetSubTitle ( "Company data" );
	gs->SetScreenType ( SCREEN_COMPANYINFO );
    gs->SetNextPage ( 0 );
	comp->AddComputerScreen ( gs, 1 );

	// Screen 2										(Login)

	UserIDScreen *s2 = new UserIDScreen ();
	s2->SetMainTitle ( companyname );
	s2->SetSubTitle ( "Log in" );
	s2->SetNextPage ( 3 );
	s2->SetDifficulty ( HACKDIFFICULTY_PUBLICBANKSERVER );
	comp->AddComputerScreen ( s2, 2 );

	// Screen 3										(Manage existing account)

	MenuScreen *s3 = new MenuScreen ();
	s3->SetMainTitle ( companyname );
	s3->SetSubTitle ( "Manage Existing Account" );
	s3->AddOption ( "View Account", "View account", 12 );
	s3->AddOption ( "View Statement", "View Statement", 11 );
	s3->AddOption ( "Transfer money", "Transfer money", 7 );
	s3->AddOption ( "Loans", "Manage money loans", 10 );
//	s3->AddOption ( "Close account", "Close account", 19 );
	s3->AddOption ( "Done", "Return to the main menu", 0 );
	comp->AddComputerScreen ( s3, 3 );

	// Screen 4										(Create new account)

	DialogScreen *ds4 = new DialogScreen ();
	ds4->SetMainTitle ( companyname );
	ds4->SetSubTitle ( "Create new account" );
	ds4->AddWidget ( "name",      WIDGET_BASIC, 80, 180, 170, 20, "Enter your name", "This is your name" );
	ds4->AddWidget ( "password",  WIDGET_BASIC, 80, 210, 170, 20, "Enter your password", "You'll need this to access your account" );
	ds4->AddWidget ( "password2", WIDGET_BASIC, 80, 240, 170, 20, "Re-type your password", "For verification purposes" );

	ds4->AddWidget ( "nametext",      WIDGET_TEXTBOX,     270, 180, 170, 20, "Fill this in", "Enter your name here" );
	ds4->AddWidget ( "passwordtext",  WIDGET_PASSWORDBOX, 270, 210, 170, 20, "", "Enter your password here" );
	ds4->AddWidget ( "passwordtext2", WIDGET_PASSWORDBOX, 270, 240, 170, 20, "", "Re-type your password here" );

	ds4->AddWidget ( "create", WIDGET_SCRIPTBUTTON, 250, 330, 80, 20, "Create", "Create this account", 11, -1, "", "" );
	ds4->AddWidget ( "cancel", WIDGET_NEXTPAGE, 350, 330, 80, 20, "Cancel", "Cancel", 0, -1, "", "" );

    ds4->SetReturnKeyButton ( "create" );
	comp->AddComputerScreen ( ds4, 4 );

	// Screen 5										(New account created)

	MessageScreen *ms5 = new MessageScreen ();
	ms5->SetMainTitle ( companyname );
	ms5->SetSubTitle ( "Create new account" );
	ms5->SetTextMessage ( "Your new account has been created." );
	ms5->SetButtonMessage ( "OK" );
	ms5->SetNextPage ( 12 );
	comp->AddComputerScreen ( ms5, 5 );

	// Screen 6										(Admin security verification)

	HighSecurityScreen *hs = new HighSecurityScreen ();
	hs->SetMainTitle ( companyname );
	hs->SetSubTitle ( "Admin Authorisation required" );
	hs->AddSystem ( "Admin Password verification", 13 );
	hs->AddSystem ( "Voice print Identification", 14 );
	hs->AddSystem ( "Elliptic-Curve Encryption Cypher", 15 );
	hs->SetNextPage ( 16 );
	comp->AddComputerScreen ( hs, 6 );

	// Screen 7										(Money transfer)

	DialogScreen *ds7 = new DialogScreen ();
	ds7->SetMainTitle ( companyname );
	ds7->SetSubTitle ( "Transfer money" );

	ds7->AddWidget ( "s_accno", WIDGET_BASIC, 100, 150, 100, 15, "Acc No. (From)", "Shows the source account number" );
	ds7->AddWidget ( "t_ip",    WIDGET_BASIC, 100, 200, 100, 15, "Bank IP (To)", "Enter the IP of the target bank here" );
	ds7->AddWidget ( "t_accno", WIDGET_BASIC, 100, 230, 100, 15, "Acc No. (To)", "Enter the target account number here" );
	ds7->AddWidget ( "amount",  WIDGET_BASIC, 100, 280, 100, 15, "Amount", "Enter the value (in credits) to transfer" );

	ds7->AddWidget ( "s_accno_value", WIDGET_FIELDVALUE, 250, 150, 200, 15, "", "", -1, -1, "Account Number", "" );
	ds7->AddWidget ( "t_ip_value",    WIDGET_TEXTBOX,    250, 200, 200, 15, "Fill this in", "Fill this in" );
	ds7->AddWidget ( "t_accno_value", WIDGET_TEXTBOX,    250, 230, 200, 15, "Fill this in", "Fill this in" );
	ds7->AddWidget ( "amount_value",  WIDGET_TEXTBOX,    250, 280, 200, 15, "Fill this in", "Fill this in" );

	ds7->AddWidget ( "transfer", WIDGET_SCRIPTBUTTON, 250, 330, 80, 15, "Transfer", "Perform the transaction", 10, -1, "", "" );
	ds7->AddWidget ( "cancel", WIDGET_NEXTPAGE, 350, 330, 80, 15, "Cancel", "Cancel the transfer", 3, -1, "", "" );

	comp->AddComputerScreen ( ds7, 7 );

	// Screen 8										(Transfer successfull)

	MessageScreen *ms8 = new MessageScreen ();
	ms8->SetMainTitle ( companyname );
	ms8->SetSubTitle ( "Transfer completed" );
	ms8->SetTextMessage ( "The transfer was completed successfully." );
	ms8->SetButtonMessage ( "OK" );
	ms8->SetNextPage ( 3 );
	comp->AddComputerScreen ( ms8, 8 );

	// Screen 9										(Transfer not successfull)

	MessageScreen *ms9 = new MessageScreen ();
	ms9->SetMainTitle ( companyname );
	ms9->SetSubTitle ( "Transfer failed" );
	ms9->SetTextMessage ( "The transfer was not completed successfully.\nPlease contact the bank administrator." );
	ms9->SetButtonMessage ( "OK" );
	ms9->SetNextPage ( 3 );
	comp->AddComputerScreen ( ms9, 9 );

	// Screen 10

	GenericScreen *gs10 = new GenericScreen ();
	gs10->SetScreenType ( SCREEN_LOANSSCREEN );
	gs10->SetMainTitle ( companyname );
	gs10->SetSubTitle ( "Loans Management" );
	gs10->SetNextPage ( 3 );
	comp->AddComputerScreen ( gs10, 10 );

	// Screen 11

	LogScreen *log11 = new LogScreen ();
	log11->SetTARGET ( LOGSCREEN_TARGET_ACCOUNTLOGS );
	log11->SetMainTitle ( companyname );
	log11->SetSubTitle ( "Account Statement" );
	log11->SetNextPage ( 3 );
	comp->AddComputerScreen ( log11, 11 );

	// Screen 12									(View account)

	GenericScreen *view12 = new GenericScreen ();
	view12->SetScreenType ( SCREEN_ACCOUNTSCREEN );
	view12->SetMainTitle ( companyname );
	view12->SetSubTitle ( "View account" );
	view12->SetNextPage ( 3 );
	comp->AddComputerScreen ( view12, 12 );


	// Screen 13									(Admin password screen)

	PasswordScreen *ps13 = new PasswordScreen ();
	ps13->SetMainTitle ( companyname );
	ps13->SetSubTitle ( "Access code required" );
	ps13->SetPassword ( NameGenerator::GenerateComplexPassword () );
	ps13->SetDifficulty ( HACKDIFFICULTY_PUBLICBANKSERVER_ADMIN );
	ps13->SetNextPage ( 6 );
	comp->AddComputerScreen ( ps13, 13 );

	// Screen 14									(Voice print analysis)

	GenericScreen *gs14 = new GenericScreen ();
	gs14->SetMainTitle ( companyname );
	gs14->SetSubTitle ( "Voice print analysis required" );
	gs14->SetScreenType ( SCREEN_VOICEANALYSIS );
	gs14->SetNextPage ( 6 );
	comp->AddComputerScreen ( gs14, 14 );

	// Screen 15									(Elliptic-Curve encryption cypher)

	CypherScreen *cs = new CypherScreen ();
	cs->SetMainTitle ( companyname );
	cs->SetSubTitle ( "Enter Elliptic-Curve encryption cypher" );
	cs->SetDifficulty ( HACKDIFFICULTY_PUBLICBANKSERVER_ADMIN );
	cs->SetNextPage ( 6 );
	comp->AddComputerScreen ( cs, 15 );


	// Screen 16									(Admin menu)

	MenuScreen *ms16 = new MenuScreen ();
	ms16->SetMainTitle ( companyname );
	ms16->SetSubTitle ( "Admin Menu" );
	ms16->AddOption ( "View All Accounts", "View all accounts", 17 );
	ms16->AddOption ( "View access logs", "View activity logs", 18 );
	ms16->AddOption ( "Done", "Close admin options menu", 0 );
	comp->AddComputerScreen ( ms16, 16 );

	// Screen 17									(View all records)

	GenericScreen *rs17 = new GenericScreen ();
	rs17->SetScreenType ( SCREEN_RECORDSCREEN );
	rs17->SetMainTitle ( companyname );
	rs17->SetSubTitle ( "Bank accounts" );
	rs17->SetNextPage ( 16 );
	comp->AddComputerScreen ( rs17, 17 );

	// Screen 18									(Access logs)

	LogScreen *ls18 = new LogScreen ();
	ls18->SetTARGET ( LOGSCREEN_TARGET_ACCESSLOGS );
	ls18->SetMainTitle ( companyname );
	ls18->SetSubTitle ( "Access logs" );
	ls18->SetNextPage ( 16 );
	comp->AddComputerScreen ( ls18, 18 );

    // Screen 19                                    (Close account)

	DialogScreen *ds19 = new DialogScreen ();
	ds19->SetMainTitle ( companyname );
	ds19->SetSubTitle ( "Close account" );
	ds19->AddWidget ( "text", WIDGET_CAPTION, 100, 150, 200, 50, "Are you sure you wish to close your account?", "Shows the source account number" );
	ds19->AddWidget ( "close", WIDGET_SCRIPTBUTTON, 250, 330, 80, 15, "Close", "Close your account", 13, -1, "", "" );
	ds19->AddWidget ( "cancel", WIDGET_NEXTPAGE, 350, 330, 80, 15, "Cancel", "Do not close the account", 3, -1, "", "" );

    comp->AddComputerScreen ( ds19, 19 );

    // Screen 20                                    (Account closed)

	MessageScreen *ms20 = new MessageScreen ();
	ms20->SetMainTitle ( companyname );
	ms20->SetSubTitle ( "Account closed" );
	ms20->SetTextMessage ( "Your account has been closed.\n"
                           "Click OK to return to the main menu." );
	ms20->SetButtonMessage ( "OK" );
	ms20->SetNextPage ( 0 );
	comp->AddComputerScreen ( ms20, 20 );

	game->GetWorld ()->CreateComputer ( comp );

	return comp;

}

Computer *WorldGenerator::GenerateEmptyFileServer ( char *companyname )
{

    //char *computername = strdup( NameGenerator::GenerateFileServerName( companyname ) );
	char computername[MAX_COMPUTERNAME];
	strncpy( computername, NameGenerator::GenerateFileServerName( companyname ),
		     MAX_COMPUTERNAME );
	if ( computername[MAX_COMPUTERNAME - 1] != '\0' ) {
		computername[MAX_COMPUTERNAME - 1] = '\0';
	}

    Computer *existing = game->GetWorld ()->GetComputer ( computername );
    if ( existing ) return existing;

	VLocation *vl = GenerateLocation ();
	vl->SetListed ( false );

	Computer *comp = new Computer ();
	comp->SetTYPE ( COMPUTER_TYPE_UNKNOWN );
	comp->SetName ( computername );
	comp->SetCompanyName ( companyname );
	comp->SetTraceSpeed ( 0 );
	comp->SetTraceAction ( COMPUTER_TRACEACTION_DISCONNECT );
	comp->SetIsTargetable ( false );
	comp->SetIP ( vl->ip );
	comp->databank.SetSize ( 100 );

    //delete computername;

	// Screen 0			User ID

	UserIDScreen *uid0 = new UserIDScreen ();
	uid0->SetMainTitle ( companyname );
	uid0->SetSubTitle ( "File Server" );
	uid0->SetNextPage ( 1 );
	comp->AddComputerScreen ( uid0, 0 );

	// Screen 1			File Server

	GenericScreen *gs1 = new GenericScreen ();
	gs1->SetMainTitle ( companyname );
	gs1->SetSubTitle ( "File Server" );
	gs1->SetScreenType ( SCREEN_FILESERVERSCREEN );
	comp->AddComputerScreen ( gs1, 1 );

	game->GetWorld ()->CreateComputer ( comp );

	return comp;

}

Computer  *WorldGenerator::GenerateLAN ( char *companyname )
{

	Company *company = game->GetWorld()->GetCompany( companyname );
	if ( company ) {

		int difficulty;

		if ( company->size > 40 )	difficulty = 4;
		else                    	difficulty = company->size / 10;

		return LanGenerator::GenerateLAN( companyname, difficulty );

	}

	return NULL;
}

Computer *WorldGenerator::GeneratePersonalComputer ( char *personname )
{

    //char *computername = strdup( NameGenerator::GeneratePersonalComputerName( personname ) );
	char computername[MAX_COMPUTERNAME];
	strncpy( computername, NameGenerator::GeneratePersonalComputerName( personname ),
		     MAX_COMPUTERNAME );
	if ( computername[MAX_COMPUTERNAME - 1] != '\0' ) {
		computername[MAX_COMPUTERNAME - 1] = '\0';
	}

	VLocation *vl = GenerateLocation ();
	vl->SetListed ( false );
	game->GetWorld ()->CreateCompany ( personname );

	Computer *comp = new Computer ();
	comp->SetTYPE ( COMPUTER_TYPE_PERSONALCOMPUTER );
	comp->SetName ( computername );
	comp->SetCompanyName ( personname );
	comp->SetTraceSpeed ( 0 );
	comp->SetTraceAction ( COMPUTER_TRACEACTION_DISCONNECT );
	comp->SetIsTargetable ( false );
	comp->SetIP ( vl->ip );

    //delete computername;

	MessageScreen *ms0 = new MessageScreen ();
	ms0->SetMainTitle ( personname );
	ms0->SetSubTitle ( "Personal Computer" );
	ms0->SetTextMessage ( "No external log-ins permitted.\nPlease leave now." );
	comp->AddComputerScreen ( ms0 );


	game->GetWorld ()->CreateComputer ( comp );

	return comp;

}

Computer *WorldGenerator::GenerateVoicePhoneSystem ( char *personname )
{

    //char *computername = strdup( NameGenerator::GenerateVoicePhoneSystemName( personname ) );
	char computername[MAX_COMPUTERNAME];
	strncpy( computername, NameGenerator::GenerateVoicePhoneSystemName( personname ),
		     MAX_COMPUTERNAME );
	if ( computername[MAX_COMPUTERNAME - 1] != '\0' ) {
		computername[MAX_COMPUTERNAME - 1] = '\0';
	}

	int x, y;
	GenerateValidMapPos ( x, y );
	char ip [SIZE_VLOCATION_IP];
	UplinkSnprintf ( ip, sizeof ( ip ), "(0%d%d%d%d)%d%d%d%d%d%d", NumberGenerator::RandomNumber (10), NumberGenerator::RandomNumber (10), NumberGenerator::RandomNumber (10), NumberGenerator::RandomNumber (10),
																	NumberGenerator::RandomNumber (10), NumberGenerator::RandomNumber (10), NumberGenerator::RandomNumber (10), NumberGenerator::RandomNumber (10), NumberGenerator::RandomNumber (10), NumberGenerator::RandomNumber (10) );
	VLocation *vl = new VLocation ();
	vl->SetPLocation ( x, y );
	vl->SetIP ( ip );
	game->GetWorld ()->CreateVLocation ( vl );
	vl->SetListed ( false );

	Computer *comp = new Computer ();
	comp->SetTYPE ( COMPUTER_TYPE_VOICEPHONESYSTEM );
	comp->SetName ( computername );
	comp->SetCompanyName ( personname );
	comp->SetTraceSpeed ( 0 );
	comp->SetTraceAction ( COMPUTER_TRACEACTION_DISCONNECT );
	comp->SetIsTargetable ( false );
	comp->SetIP ( vl->ip );

    //delete computername;

	GenericScreen *gs0 = new GenericScreen ();
	gs0->SetMainTitle ( personname );
	gs0->SetSubTitle ( ip );
	gs0->SetScreenType ( SCREEN_VOICEPHONE );
	comp->AddComputerScreen ( gs0, 0 );

	game->GetWorld ()->CreateComputer ( comp );

	return comp;

}

Person *WorldGenerator::GeneratePerson ()
{

	//char *name = strdup( NameGenerator::GeneratePersonName () );
	char name[MAX_PERSONNAME];
	strncpy( name, NameGenerator::GeneratePersonName(),
		     MAX_PERSONNAME );
	if ( name[MAX_PERSONNAME - 1] != '\0' ) {
		name[MAX_PERSONNAME - 1] = '\0';
	}

	Computer *comp = GeneratePersonalComputer ( name );
	UplinkAssert (comp);

	Computer *phone = GenerateVoicePhoneSystem ( name );
	UplinkAssert (phone);

	int age = (int) NumberGenerator::RandomNormalNumber ( 30, 20 );						// 10 - 50
	int photo = NumberGenerator::RandomNumber ( NUM_STARTING_PHOTOS + 1 );

    int voice = 1;
#ifndef DEMOGAME
    voice = NumberGenerator::RandomNumber ( NUM_STARTING_VOICES ) + 1;
#endif

	Person *person = new Person ();
	person->SetName ( name );
	person->SetAge ( age );
	person->SetPhotoIndex ( photo );
    person->SetVoiceIndex ( voice );
	person->SetLocalHost ( comp->ip );
	person->SetRemoteHost ( comp->ip );
	person->SetPhoneNumber ( phone->ip );

	game->GetWorld ()->CreatePerson ( person );

	person->connection.Reset ();

	// Create some records for this person, spread around the world-wide databases

	RecordGenerator::GenerateRecords ( name );

    //delete name;

	return person;

}

Agent *WorldGenerator::GenerateAgent ()
{

	char name [SIZE_PERSON_NAME];
	char handle [SIZE_AGENT_HANDLE];

    UplinkStrncpy ( name, NameGenerator::GeneratePersonName (), sizeof ( name ) );
    UplinkStrncpy ( handle, NameGenerator::GenerateAgentAlias (), sizeof ( handle ) );

	Computer *comp = GeneratePersonalComputer ( name );
	UplinkAssert (comp);

	Computer *phone = GenerateVoicePhoneSystem ( name );
	UplinkAssert (phone);

	int age = (int) NumberGenerator::RandomNormalNumber ( 30, 10 );						// 20 - 40
	int photo = NumberGenerator::RandomNumber ( NUM_STARTING_PHOTOS + 1 );

    int voice = 1;
#ifndef DEMOGAME
    voice = NumberGenerator::RandomNumber ( NUM_STARTING_VOICES ) + 1;
#endif

	Agent *person = new Agent ();
	person->SetName ( name );
	person->SetHandle ( handle );
	person->SetAge ( age );
	person->SetPhotoIndex ( photo );
    person->SetVoiceIndex ( voice );
	person->SetLocalHost ( comp->ip );
	person->SetRemoteHost ( comp->ip );
    person->SetPhoneNumber ( phone->ip );
	person->rating.SetUplinkRating ( (int) NumberGenerator::RandomNormalNumber ( AGENT_UPLINKRATINGAVERAGE, AGENT_UPLINKRATINGVARIANCE ) );


	game->GetWorld ()->CreatePerson ( person );

	// Create some records for this person, spread around the world-wide databases

	RecordGenerator::GenerateRecords ( name );

	return person;

}

void WorldGenerator::UpdateSoftwareUpgrades ( )
{

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );

	if ( !cu )
		return;

	// Update the software items for sale

	for ( int i = 0; i < cu->sw_sales.Size (); i++ )
		if ( cu->sw_sales.ValidIndex ( i ) ) {
			Sale *sale = cu->sw_sales.GetData ( i );
			if ( sale )
				delete sale;
	}
	cu->sw_sales.Empty ();

	Sale *previoussale = NULL;

	for ( int is = 0; is < NUM_STARTINGSOFTWAREUPGRADES; ++is ) {

		if ( previoussale && strcmp ( previoussale->title, SOFTWARE_UPGRADES [is].name ) == 0 ) {

			// New version of an existing item

			previoussale->AddVersion ( SOFTWARE_UPGRADES [is].description,
			                           SOFTWARE_UPGRADES [is].cost,
			                           SOFTWARE_UPGRADES [is].size,
			                           SOFTWARE_UPGRADES [is].data );

		}
		else {

			// New sale item

			Sale *sale = new Sale ();
			sale->SetTitle         ( SOFTWARE_UPGRADES [is].name );
			sale->SetSaleTYPE      ( SALETYPE_SOFTWARE );
			sale->SetSwhwTYPE      ( SOFTWARE_UPGRADES [is].TYPE );

			sale->AddVersion      ( " ", -1, -1, -1 );
			sale->AddVersion      ( SOFTWARE_UPGRADES [is].description,
			                        SOFTWARE_UPGRADES [is].cost,
			                        SOFTWARE_UPGRADES [is].size,
			                        SOFTWARE_UPGRADES [is].data );

			cu->CreateSWSale ( sale );

			previoussale = sale;

		}

	}

}

VLocation *WorldGenerator::GetRandomLocation ()
{

	DArray <char *> *vls = game->GetWorld ()->locations.ConvertIndexToDArray ();
	UplinkAssert ( vls->Size () > 0 );

	int index = NumberGenerator::RandomNumber ( vls->Size () );
	UplinkAssert ( vls->ValidIndex (index) );

	VLocation *vl = game->GetWorld ()->GetVLocation ( vls->GetData (index) );
	delete vls;
	return vl;

}

Company *WorldGenerator::GetRandomCompany ()
{

	DArray <char *> *comps = game->GetWorld ()->companies.ConvertIndexToDArray ();
	UplinkAssert ( comps->Size () > 0 );

	while (1) {

		int index = NumberGenerator::RandomNumber ( comps->Size () );
		UplinkAssert ( comps->ValidIndex ( index ) );

		Company *comp = game->GetWorld ()->GetCompany ( comps->GetData (index) );
		UplinkAssert (comp);

		// Make sure we haven't picked up the player's computer
		// Or a persons private company
		// Or a Government computer
        // Or an Uplink computer
		// Or a plot company (because it cause problems when the plot as begun)

		if ( strcmp ( comp->name, "Player" ) != 0 &&
			 strcmp ( comp->name, "Government" ) != 0 &&
			 strcmp ( comp->name, "Uplink" ) != 0 &&
             strcmp ( comp->name, "Sample Company" ) != 0 &&
			 !game->GetWorld()->plotgenerator.IsPlotCompany( comp->name ) &&
			 comp->TYPE != COMPANYTYPE_UNKNOWN ) {
			delete comps;
			return comp;
		}

	}

}

Computer *WorldGenerator::GetRandomComputer ()
{

/*
	DArray <char *> *comps = game->GetWorld ()->computers.ConvertIndexToDArray ();
	UplinkAssert ( comps->Size () > 0 );

	while (1) {

		int index = NumberGenerator::RandomNumber ( comps->Size () );
		UplinkAssert ( comps->ValidIndex ( index ) );

		Computer *comp = game->GetWorld ()->GetComputer ( comps->GetData (index) );
		UplinkAssert (comp);

		if ( comp->istargetable && comp->isrunning ) {
			delete comps;
			return comp;
		}

	}
*/

    return GetRandomComputer ( COMPUTER_TYPE_ANY );

}

Computer *WorldGenerator::GetRandomComputer ( int TYPE )
{

	DArray <char *> *comps = game->GetWorld ()->computers.ConvertIndexToDArray ();
	UplinkAssert ( comps->Size () > 0 );

	// Create a new array of all valid computers

	DArray <char *> *typecomps = new DArray <char *> ();

	for ( int i = 0; i < comps->Size (); ++i ) {

		if ( comps->ValidIndex ( i ) ) {

			Computer *comp = game->GetWorld ()->GetComputer ( comps->GetData (i) );
			UplinkAssert ( comp );

			if ( (comp->TYPE & TYPE) && comp->istargetable && comp->isrunning &&
				 strcmp(comp->companyname, "Government" ) != 0 )
				typecomps->PutData ( comps->GetData (i) );

		}

	}

    if ( typecomps->Size () == 0 ) {

        delete comps;
        delete typecomps;

        // We couldn't find a valid computer
        // Generate one now

        printf ( "WorldGenerator WARNING: Couldn't find any computers - generated a new one\n" );

        Company *company = GetRandomCompany();
        UplinkAssert (company);
        return GenerateComputer ( company->name, TYPE );

    }
    else {

	    int index = NumberGenerator::RandomNumber ( typecomps->Size () );
	    UplinkAssert ( typecomps->ValidIndex ( index ) );

	    Computer *comp = game->GetWorld ()->GetComputer ( typecomps->GetData (index) );
	    UplinkAssert (comp);

	    delete comps;
	    delete typecomps;

	    return comp;

    }

}

Computer *WorldGenerator::GetRandomLowSecurityComputer ( int TYPE )
{

	Computer *result = NULL;

	while ( !result ||
			result->security.IsRunning_Proxy () ||
			result->security.IsRunning_Firewall () ||
            !(result->traceaction < COMPUTER_TRACEACTION_LEGAL) ) {

		result = GetRandomComputer ( TYPE );

	}

	return result;

}

Person *WorldGenerator::GetRandomPerson ()
{

/*
    // OLD CODE
    // Could get into endless loop

	DArray <char *> *people = game->GetWorld ()->people.ConvertIndexToDArray ();
	UplinkAssert ( people->Size () > 0 );

	while ( 1 ) {

		int index = NumberGenerator::RandomNumber ( people->Size () );
		UplinkAssert ( people->ValidIndex (index) );

		Person *person = game->GetWorld ()->GetPerson ( people->GetData (index) );
		UplinkAssert (person);

		if ( person->istargetable &&
			 person->STATUS == PERSON_STATUS_NONE ) {
			delete people;
			return person;
		}

	}
*/

    //
    // Build a list of all valid People

	DArray <char *> *people = game->GetWorld ()->people.ConvertIndexToDArray ();
	UplinkAssert ( people->Size () > 0 );

    DArray <Person *> validPeople;

    for ( int i = 0; i < people->Size(); ++i ) {

        UplinkAssert ( people->ValidIndex(i) );

		Person *person = game->GetWorld ()->GetPerson ( people->GetData(i) );
		UplinkAssert (person);

		if ( person->istargetable &&
			 person->GetStatus () == PERSON_STATUS_NONE )

			validPeople.PutData( person );

    }

    delete people;


    if ( validPeople.Size() == 0 ) {

        // No person fits the criteria
        // Create a new one and return him

		Person *person = WorldGenerator::GeneratePerson();
		return person;

    }
    else {

	    int index = NumberGenerator::RandomNumber ( validPeople.Size () );
		UplinkAssert ( validPeople.ValidIndex (index) );
        return (Person *) validPeople.GetData ( index );

    }

}

Agent *WorldGenerator::GetRandomAgent ()
{

	DArray <char *> *people = game->GetWorld ()->people.ConvertIndexToDArray ();
	UplinkAssert ( people->Size () > 0 );

/*

    OLD CODE
    Serious bug - can get caught in a endless loop
    if there are no matching agents to find

	while ( 1 ) {

		int index = NumberGenerator::RandomNumber ( people->Size () );
		UplinkAssert ( people->ValidIndex (index) );

		Person *person = game->GetWorld ()->GetPerson ( people->GetData (index) );
		UplinkAssert (person);

		if ( person->istargetable &&
			 person->rating.uplinkrating > 0 &&
			 person->STATUS == 0 ) {
			delete people;
			return (Agent *) person;
		}

	}
*/

    //
    // Build a list of all valid Agents

    DArray <Person *> validPeople;

    for ( int i = 0; i < people->Size(); ++i ) {

        UplinkAssert ( people->ValidIndex(i) );

		Person *person = game->GetWorld ()->GetPerson ( people->GetData(i) );
		UplinkAssert (person);

		if ( person->istargetable &&
			 person->rating.uplinkrating > 0 &&
			 person->GetStatus () == PERSON_STATUS_NONE )

			validPeople.PutData( person );

    }

    delete people;


    if ( validPeople.Size() == 0 ) {

        // No agent fits the criteria
        // Create a new one and return him

        Agent *agent = WorldGenerator::GenerateAgent();
        return agent;

    }
    else {

	    int index = NumberGenerator::RandomNumber ( validPeople.Size () );
		UplinkAssert ( validPeople.ValidIndex (index) );
        return (Agent *) validPeople.GetData ( index );

    }

}

Mission *WorldGenerator::GetRandomMission  ()
{

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );

	LList <Mission *> *missions = &(cu->missions);
	UplinkAssert ( missions->Size () > 0 );

	int index = NumberGenerator::RandomNumber ( missions->Size () );
	UplinkAssert ( missions->ValidIndex (index) );

	Mission *mission = missions->GetData (index);
	UplinkAssert ( mission );
	return mission;

}

void WorldGenerator::ReplaceAdminCompanies ( Person *person )
{

	DArray<class Company*> *comps = game->GetWorld ()->companies.ConvertToDArray ();

	for ( int i = comps->Size () - 1; i >= 0; i-- ) {

		if ( comps->ValidIndex ( i ) ) {
	
			Company *cp = comps->GetData ( i );

			if ( cp->administrator && strcmp( person->name, cp->administrator ) == 0 ) {

				Person *newadmin = WorldGenerator::GetRandomPerson ();
	            UplinkAssert ( newadmin );
				cp->SetAdmin ( newadmin->name );

			}

		}

	}

	delete comps;

}

void WorldGenerator::ReplaceInvalidCompanyAdmins ( )
{

	World *world = game->GetWorld ();
	DArray<class Company*> *comps = world->companies.ConvertToDArray ();

	for ( int i = comps->Size () - 1; i >= 0; i-- ) {

		if ( comps->ValidIndex ( i ) ) {

			Company *cp = comps->GetData ( i );
			
			if ( cp->administrator ) {
			
				Person *admin = world->GetPerson ( cp->administrator );

				// Some administrator ( like Unlisted ) are valid but not a person
				
				if ( admin && admin->GetStatus () != PERSON_STATUS_NONE ) {
				
					Person *newadmin = WorldGenerator::GetRandomPerson ();
					UplinkAssert ( newadmin );
					cp->SetAdmin ( newadmin->name );
				
				}

			}

		}

	}

	delete comps;

}

/*
bool WorldGenerator::VerifyPlayerGateway ()
{

	// Check if the player use a custom gateway and it's no more available.

	Gateway *plgateway = &game->GetWorld ()->GetPlayer ()->gateway;
	GatewayDef *plgatewaydef = plgateway->curgatewaydef;
	UplinkAssert ( plgatewaydef );
	GatewayDef *realgatewaydef = NULL;
	
	for ( int i = game->GetWorld ()->gatewaydefs.Size () - 1; i >= 0; i-- )
		if ( game->GetWorld ()->gatewaydefs.ValidIndex ( i ) ) {
			GatewayDef *tempgd = game->GetWorld ()->gatewaydefs.GetData ( i );
			if ( strcmp ( plgatewaydef->name, tempgd->name ) == 0 ) {
				realgatewaydef = tempgd;
				break;
			}
		}

	if ( !realgatewaydef ) {

		char message [ 256 ];
		UplinkSnprintf ( message, sizeof ( message ), "This agent is using the following custom gateway '%s', but Uplink cannot find a definition for it.", plgatewaydef->name );
		printf ( "WARNING : WorldGenerator::VerifyPlayerGateway : %s\n", message );
        //create_msgbox ( "Critical Error", message, WorldGenerator::VerifyPlayerGatewayCloseDialog );
        create_msgbox ( "Critical Error", message );
		return false;

	}

	return true;

}
*/

/*
void WorldGenerator::VerifyPlayerGatewayCloseDialog ( Button *button )
{

	remove_msgbox ();

	HUDInterface::CloseGame ();

}
*/
