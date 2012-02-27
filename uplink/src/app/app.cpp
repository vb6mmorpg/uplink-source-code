// -*- tab-width:4 c-file-style:"cc-mode" -*-
// App source file
// By Christopher Delay

#include "stdafx.h"

#ifdef WIN32
#include <direct.h>
#include <io.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#endif

#include "gucci.h"
#include "eclipse.h"
#include "redshirt.h"
#include "soundgarden.h"
#include "vanbakel.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/miscutils.h"

#include "options/options.h"

#include "game/game.h"
#include "game/data/data.h"
#include "game/gameobituary.h"

#include "mainmenu/mainmenu.h"
#include "network/network.h"

#include "world/world.h"
#include "world/player.h"
#include "world/generator/worldgenerator.h"

#include "interface/interface.h"
#include "interface/localinterface/localinterface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/localinterface/phonedialler.h"

#include "mmgr.h"

App :: App ()
{

    UplinkStrncpy ( path, "c:/", sizeof ( path ) );
    UplinkStrncpy ( userpath, path, sizeof ( userpath ) );
    UplinkStrncpy ( version, "1.31c", sizeof ( version ) );
    UplinkStrncpy ( type, "RELEASE", sizeof ( type ) );
    UplinkStrncpy ( date, "01/01/97", sizeof ( date ) );
    UplinkStrncpy ( title, "NewApp", sizeof ( title ) );
    UplinkStrncpy ( release, "Version 1.0 (RELEASE), Compiled on 01/01/97", sizeof ( release ) );

    starttime = 0;

	options = NULL;
	network = NULL;
	mainmenu = NULL;
	phoneDial = NULL;

	nextLoadGame = NULL;

    closed = false;

#ifdef CODECARD_ENABLED
	askCodeCard = true;
#else
	askCodeCard = false;
#endif

#ifdef _DEBUG
//    SlInitialise ();
#endif

}

App :: ~App ()
{
	
	// All destructor information should go in App::Close ()

	if ( !Closed() )
		Close();

}

void App :: Initialise ()
{

	options = new Options ();	
	options->Load ( NULL );
	options->CreateDefaultOptions ();

    starttime = (int)EclGetAccurateTime ();

	network = new Network ();
	mainmenu = new MainMenu ();

}

void App :: Set ( char *newpath, char *newversion, char *newtype, char *newdate, char *newtitle )
{
                
	UplinkAssert ( strlen ( newpath ) < SIZE_APP_PATH );
	UplinkAssert ( strlen ( newversion ) < SIZE_APP_VERSION );
	UplinkAssert ( strlen ( newtype ) < SIZE_APP_TYPE );
	UplinkAssert ( strlen ( newdate ) < SIZE_APP_DATE );
	UplinkAssert ( strlen ( newtitle ) < SIZE_APP_TITLE );

    UplinkStrncpy ( path, newpath, sizeof ( path ) );
    UplinkStrncpy ( version, newversion, sizeof ( version ) );
    UplinkStrncpy ( type, newtype, sizeof ( type ) );
    UplinkStrncpy ( date, newdate, sizeof ( date ) );
    UplinkStrncpy ( title, newtitle, sizeof ( title ) );
    UplinkSnprintf ( release, sizeof ( release ), "Version %s (%s)\nCompiled on %s\n", version, type, date );    

#ifdef WIN32
	// Under Windows, the user-path is %app-path%/users
	UplinkSnprintf( userpath, sizeof ( userpath ), "%susers/", path );
	UplinkSnprintf( usertmppath, sizeof ( usertmppath ), "%suserstmp/", path );
	UplinkSnprintf( userretirepath, sizeof ( userretirepath ), "%susersold/", path );
#else
	// Under Linux, the user-path is ~/.uplink 
	// (or %app-path%/users if no HOME environment variable)
	char *homedir = getenv("HOME");
	if (homedir != NULL) {
		UplinkSnprintf( userpath, sizeof ( userpath ), "%s/.uplink/", homedir);
		UplinkSnprintf( usertmppath, sizeof ( usertmppath ), "%s/.uplink/userstmp/", homedir);
		UplinkSnprintf( userretirepath, sizeof ( userretirepath ), "%s/.uplink/usersold/", homedir);
	}
	else {
		UplinkSnprintf( userpath, sizeof ( userpath ), "%susers/", path );
		UplinkSnprintf( usertmppath, sizeof ( usertmppath ), "%suserstmp/", path );
		UplinkSnprintf( userretirepath, sizeof ( userretirepath ), "%susersold/", path );
	}
#endif // WIN32
}

Options *App::GetOptions ()
{

	UplinkAssert (options);
	return options;

}

Network *App::GetNetwork ()
{

	UplinkAssert (network);
	return network;

}

MainMenu *App::GetMainMenu ()
{

	UplinkAssert (mainmenu);
	return mainmenu;

}

void App::RegisterPhoneDialler ( PhoneDialler *phoneDiallerScreen )
{

	UplinkAssert ( phoneDiallerScreen );
	UplinkAssert ( phoneDial != phoneDiallerScreen );

	if ( phoneDial )
		UnRegisterPhoneDialler ( phoneDial );

	phoneDial = phoneDiallerScreen;

}

void App::UnRegisterPhoneDialler ( PhoneDialler *phoneDiallerScreen )
{

	if ( phoneDial == phoneDiallerScreen ) {

		phoneDial->Remove ();
		delete phoneDial;
		phoneDial = NULL;

	}

}

void CopyGame ( char *username, char *filename )
{

	char filenametmp [256];
	UplinkSnprintf ( filenametmp, sizeof ( filenametmp ), "%scuragent.usr", app->usertmppath);

	//char filenametmpUplink [256];
	//UplinkSnprintf ( filenametmpUplink, sizeof ( filenametmpUplink ), "%scuragent_clear.bin", app->usertmppath );

	EmptyDirectory ( app->usertmppath );
	CopyFilePlain ( filename, filenametmp );
	//CopyFileUplink ( filename, filenametmpUplink );

}

void App::SetNextLoadGame ( const char *username )
{

	UplinkAssert ( username );

	if ( nextLoadGame )
		delete [] nextLoadGame;

	nextLoadGame = new char [ strlen ( username ) + 1 ];
	UplinkSafeStrcpy ( nextLoadGame, username );

}

void App::LoadGame ( )
{

	UplinkAssert ( nextLoadGame );

	LoadGame ( nextLoadGame );

	delete [] nextLoadGame;
	nextLoadGame = NULL;

}

void App::LoadGame ( char *username )
{

	UplinkAssert ( game );

	// Try to load from the local dir

	char filename [256];
	UplinkSnprintf ( filename, sizeof ( filename ), "%s%s.usr", app->userpath, username );

	if ( !RsFileEncrypted ( filename ) ) {
		char filenametmp [256];
		UplinkSnprintf ( filenametmp, sizeof ( filenametmp ), "%s%s.tmp", app->userpath, username );
		if ( RsFileEncrypted ( filenametmp ) ) {
			UplinkSafeStrcpy ( filename, filenametmp );
		}
	}

	CopyGame ( username, filename );

	printf ( "Loading profile from %s...", filename );
	
	FILE *file = RsFileOpen ( filename );

	if ( file ) {

		GetMainMenu ()->Remove ();

		bool success = game->LoadGame ( file );
		RsFileClose ( filename, file );

        if ( !success ) {
			EmptyDirectory ( app->usertmppath );
            printf ( "App::LoadGame, Failed to load user profile\n" );

			delete game;
			game = new Game();

			return;
        }

		printf ( "success\n" );

	}
	else {

		EmptyDirectory ( app->usertmppath );
		printf ( "failed\n" );
		printf ( "App::LoadGame, Failed to load user profile\n" );

		EclReset ( GetOptions ()->GetOptionValue ("graphics_screenwidth"),
		           GetOptions ()->GetOptionValue ("graphics_screenheight") );
		GetMainMenu ()->RunScreen ( MAINMENU_LOGIN );

		return;

	}
	
	if ( !(game->GameSpeed () == GAMESPEED_GAMEOVER) ) {

        WorldGenerator::LoadDynamics();

		// Not needed anymore since the Gateway store the GatewayDef
		//WorldGenerator::VerifyPlayerGateway();

		game->GetWorld ()->GetPlayer ()->GetConnection ()->Disconnect ();
		game->GetWorld ()->GetPlayer ()->GetConnection ()->Reset ();

		if ( game->GetWorld ()->GetPlayer ()->gateway.newgatewaydef ) {

			// The player has upgraded his gateway and now wishes to 
			// log into the new system

			game->GetWorld ()->GetPlayer ()->gateway.ExchangeGatewayComplete ();
			
			// "Welcome to your new gateway"

			game->GetInterface ()->GetRemoteInterface ()->RunNewLocation ();
			game->GetInterface ()->GetRemoteInterface ()->RunScreen ( 10 );
			

		}
		else if ( game->GetWorld ()->GetPlayer ()->gateway.nuked ) {

			// The player has nuked his gateway, so set him up
			// with a new one           
            game->GetWorld ()->GetPlayer ()->gateway.nuked = false;

			game->GetWorld ()->GetPlayer ()->GetConnection ()->AddVLocation ( IP_UPLINKPUBLICACCESSSERVER );
			game->GetWorld ()->GetPlayer ()->GetConnection ()->Connect ();

            game->GetInterface ()->GetLocalInterface ()->Remove ();
            EclReset ( app->GetOptions ()->GetOptionValue ("graphics_screenwidth"),
				       app->GetOptions ()->GetOptionValue ("graphics_screenheight") );
			game->GetInterface ()->GetRemoteInterface ()->RunNewLocation ();
			game->GetInterface ()->GetRemoteInterface ()->RunScreen ( 10 );

		}
		else {
			
			// "Welcome back to your gateway"

			game->GetInterface ()->GetRemoteInterface ()->RunNewLocation ();
			game->GetInterface ()->GetRemoteInterface ()->RunScreen ( 3 );

		}

	}
	else {											// This is a Game Over game

		game->SetGameSpeed ( GAMESPEED_PAUSED );
        EclReset ( app->GetOptions ()->GetOptionValue ("graphics_screenwidth"),
				   app->GetOptions ()->GetOptionValue ("graphics_screenheight") );
		mainmenu->RunScreen ( MAINMENU_OBITUARY );

	}

}

void App::SaveGame ( char *username )
{

	if ( strcmp ( username, "NEWAGENT" ) == 0 ) return;

	UplinkAssert ( game );

	// Try to save to the local dir

	MakeDirectory ( userpath );

	char filename [256];
	//UplinkSnprintf ( filename, sizeof ( filename ), "%s%s.usr", userpath, username );
	UplinkSnprintf ( filename, sizeof ( filename ), "%s%s.tmp", userpath, username );
	char filenamereal [256];
	UplinkSnprintf ( filenamereal, sizeof ( filenamereal ), "%s%s.usr", userpath, username );

	printf ( "Saving profile to %s...", filename );

	FILE *file = fopen ( filename, "wb" );

	if ( file ) {

	
		UplinkAssert ( file );

		game->Save ( file );
		fclose ( file );

#ifndef TESTGAME
		RsEncryptFile ( filename );
#endif

		printf ( "success. Moving profile to %s...", filenamereal );

		if ( !CopyFilePlain ( filename, filenamereal ) ) {
			printf ( "failed\n" );
			printf ( "App::SaveGame, Failed to copy user profile from %s to %s\n", filename, filenamereal );
		}
		else {
			printf ( "success\n" );
			CopyGame ( username, filenamereal );
		}

	}
	else {

		printf ( "failed\n" );
		printf ( "App::SaveGame, Failed to save user profile\n" );
	
	}

}

void App::RetireGame ( char *username )
{

	char filenamereal [256];
	UplinkSnprintf ( filenamereal, sizeof ( filenamereal ), "%s%s.usr", userpath, username );
	char filenametmp [256];
	UplinkSnprintf ( filenametmp, sizeof ( filenametmp ), "%s%s.tmp", userpath, username );

	char filenameretirereal [256];
	UplinkSnprintf ( filenameretirereal, sizeof ( filenameretirereal ), "%s%s.usr", userretirepath, username );
	char filenameretiretmp [256];
	UplinkSnprintf ( filenameretiretmp, sizeof ( filenameretiretmp ), "%s%s.tmp", userretirepath, username );

	printf ( "Retire profile %s ...", username );

	CopyFilePlain ( filenametmp, filenameretiretmp );
	if ( !CopyFilePlain ( filenamereal, filenameretirereal ) ) {
		printf ( "failed\n" );
		printf ( "App::RetireGame, Failed to copy user profile from %s to %s\n", filenamereal, filenameretirereal );
	}
	else {
		printf ( "success\n" );
		RemoveFile ( filenametmp );
		RemoveFile ( filenamereal );
	}

}

DArray <char *> *App::ListExistingGames ()
{

	DArray <char *> *existing = new DArray <char *> ();

#ifdef WIN32

	char searchstring [_MAX_PATH + 1];
	UplinkSnprintf ( searchstring, sizeof ( searchstring ), "%s*.usr", app->userpath );

	_finddata_t thisfile;
	intptr_t fileindex = _findfirst ( searchstring, &thisfile );

	int exitmeplease = 0;

	while ( fileindex != -1 && !exitmeplease ) {

		size_t newnamesize = _MAX_PATH + 1;
		char *newname = new char [newnamesize];
		UplinkStrncpy ( newname, thisfile.name, newnamesize );
		char *p = strchr ( newname, '.' );
		if ( p ) *p = '\x0';

		existing->PutData ( newname );
		exitmeplease = _findnext ( fileindex, &thisfile );

	}

	if ( fileindex != -1 )
		_findclose ( fileindex );

#else
	char userdir [256];
	UplinkStrncpy ( userdir, app->userpath, sizeof ( userdir ) );
	DIR *dir = opendir( userdir );
	if (dir != NULL) {
	    struct dirent *entry = readdir ( dir );

	    while (entry != NULL) {
	    
		char *p = strstr(entry->d_name, ".usr");
		if ( p ) {
		    *p = '\x0';
	      
		    size_t newnamesize = 256;
			char *newname = new char [newnamesize];
		    UplinkStrncpy ( newname, entry->d_name, newnamesize );
		    existing->PutData ( newname );
		}
	    
		entry = readdir ( dir );
	    
	    }
	  
	    closedir( dir );
	}
#endif

	return existing;

}

void App::Close ()
{

	UplinkAssert ( !closed );

    closed = true;

    EclReset ( app->GetOptions ()->GetOptionValue ("graphics_screenwidth"),
			   app->GetOptions ()->GetOptionValue ("graphics_screenheight") );

	if ( game ) game->ExitGame ();

    options->ApplyShutdownChanges ();
    options->Save ( NULL );
    
    SvbReset ();
    GciDeleteAllTrueTypeFonts ();
    RsCleanUp ();
	SgShutdown ();

	delete mainmenu;
	mainmenu = NULL;
	delete options;
	options = NULL;
	delete network;
	network = NULL;
	if ( game ) {
		delete game;
		game = NULL;
	}
	if ( phoneDial ) {
		delete phoneDial;
		phoneDial = NULL;
	}

	if ( nextLoadGame ) {
		delete [] nextLoadGame;
		nextLoadGame = NULL;
	}

#ifdef _DEBUG
    char filename [256];
    UplinkSnprintf ( filename, sizeof ( filename ), "%smemtemp", app->userpath );
//    SlPrintMemoryLeaks ( filename );
#endif

	//exit(0);

}

bool App::Closed ()
{

    return closed;

}

bool App::Load ( FILE *file )
{
	return true;
}

void App::Save ( FILE *file )
{
}

void App::CoreDump ()
{

#ifdef WIN32
    MessageBox ( NULL, "A Fatal Error occured in Uplink.\n\n"
                       "Please report this on the Uplink forums at\n"
					   "http://www.introversion.co.uk/\n\n"
                       "Uplink will now shut down.", 
                       "Uplink Error", MB_ICONEXCLAMATION | MB_OK );    

#endif
    
  	printf ( "============== B E G I N  C O R E  D U M P =================\n" );
    PrintStackTrace ();
   	printf ( "============== E N D  C O R E  D U M P =====================\n" );

}

void App::Print ()
{
    
  	printf ( "============== A P P =======================================\n" );

	if ( game )     game->Print ();     else printf ( "game == NULL\n" );
    if ( mainmenu ) mainmenu->Print (); else printf ( "mainmenu == NULL\n" );
	if ( options )  options->Print ();  else printf ( "options == NULL\n" );
	if ( network )  network->Print ();  else printf ( "network == NULL\n" );

    printf ( "============== E N D  O F  A P P ===========================\n" );

}

void App::Update ()
{

	UplinkAssert ( game );

	if ( game->GameSpeed () == GAMESPEED_GAMEOVER || 
		( game->IsRunning () &&
		  game->GetWorld ()->GetPlayer ()->gateway.nuked ) ) {

		if ( phoneDial )
			UnRegisterPhoneDialler ( phoneDial );

		// The game has ended and now needs shutting down
		
		app->SaveGame ( game->GetWorld ()->GetPlayer ()->handle );				
		game->SetGameSpeed ( GAMESPEED_PAUSED );

        EclReset ( app->GetOptions ()->GetOptionValue ("graphics_screenwidth"),
				   app->GetOptions ()->GetOptionValue ("graphics_screenheight") );

		if ( game->GetWorld ()->GetPlayer ()->gateway.nuked )
			mainmenu->RunScreen ( MAINMENU_CONNECTIONLOST );

        else if ( game->GetObituary ()->demogameover ) 
            mainmenu->RunScreen ( MAINMENU_DEMOGAMEOVER );

        else if ( game->GetObituary ()->warezgameover )
            mainmenu->RunScreen ( MAINMENU_WAREZGAMEOVER );

        else if ( game->GetWorld ()->plotgenerator.revelation_releaseuncontrolled )
            mainmenu->RunScreen ( MAINMENU_REVELATIONWON );

        else if ( game->GetWorld ()->plotgenerator.revelation_releasefailed )
            mainmenu->RunScreen ( MAINMENU_REVELATIONLOST );

		else
			mainmenu->RunScreen ( MAINMENU_DISAVOWED );

	}

	if ( !Closed () && game->IsRunning () ) {	
		game->Update ();
	}

	if ( !Closed () && mainmenu->IsVisible () ) {
		mainmenu->Update ();
	}

	if ( !Closed () && ( game->IsRunning () || mainmenu->InScreen () == MAINMENU_FIRSTLOAD ) )
		if ( phoneDial )
			if ( phoneDial->UpdateSpecial () )
				UnRegisterPhoneDialler ( phoneDial );

	if ( !Closed () && network->IsActive () ) {
		network->Update ();
	}

}

char *App::GetID ()
{

	return "APP";

}


App *app = NULL;



