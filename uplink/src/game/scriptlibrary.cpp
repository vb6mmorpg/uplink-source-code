// ScriptLibrary.cpp: implementation of the ScriptLibrary class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <GL/gl.h>

#include <GL/glu.h>

#include "eclipse.h"
#include "soundgarden.h"
#include "redshirt.h"
#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"
#include "app/opengl_interface.h"

#include "game/game.h"
#include "game/scriptlibrary.h"
#include "game/data/data.h"

#include "options/options.h"

#include "mainmenu/mainmenu.h"

#include "interface/interface.h"
#include "interface/localinterface/localinterface.h"
#include "interface/localinterface/phonedialler.h"
#include "interface/taskmanager/taskmanager.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/criminalscreen_interface.h"
#include "interface/remoteinterface/academicscreen_interface.h"
#include "interface/remoteinterface/socialsecurityscreen_interface.h"
#include "interface/remoteinterface/radiotransmitterscreen_interface.h"

#include "world/world.h"
#include "world/player.h"
#include "world/message.h"
#include "world/company/news.h"
#include "world/company/mission.h"
#include "world/company/companyuplink.h"
#include "world/computer/computer.h"
#include "world/computer/bankcomputer.h"
#include "world/computer/lancomputer.h"
#include "world/computer/lanmonitor.h"
#include "world/computer/recordbank.h"
#include "world/computer/computerscreen/passwordscreen.h"
#include "world/computer/computerscreen/dialogscreen.h"
#include "world/computer/bankaccount.h"
#include "world/generator/missiongenerator.h"
#include "world/generator/recordgenerator.h"
#include "world/generator/worldgenerator.h"
#include "world/generator/namegenerator.h"
#include "world/generator/numbergenerator.h"
#include "world/scheduler/notificationevent.h"
#include "world/computer/gateway.h"
#include "mmgr.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////185bpmcopyright TJ 19th feb 2000 init (I love you!)XXX


void ScriptLibrary::RunScript ( int scriptindex )
{

	switch ( scriptindex ) {

		case 1			:			Script1 ();			break;

		case 10			:			Script10 ();		break;
		case 11			:			Script11 ();		break;
		case 12			:			Script12 ();		break;
        case 13         :           Script13 ();        break;
			
		case 15			:			Script15 ();		break;
		case 16			:			Script16 ();		break;
		case 17			:			Script17 ();		break;

		case 30			:			Script30 ();		break;
		case 31			:			Script31 ();		break;
		case 32			:			Script32 ();		break;
		case 33			:			Script33 ();		break;
		case 34			:			Script34 ();		break;
		case 35			:			Script35 ();		break;
		case 36			:			Script36 ();		break;

		case 40			:			Script40 ();		break;
		case 41			:			Script41 ();		break;
		case 42			:			Script42 ();		break;
		case 43			:			Script43 ();		break;

        case 45         :           Script45 ();        break;
        case 46         :           Script46 ();        break;
        case 47         :           Script47 ();        break;

        case 50			:			Script50 ();		break;
		case 51			:			Script51 ();		break;

		case 60			:			Script60 ();		break;
		case 61			:			Script61 ();		break;
		case 62			:			Script62 ();		break;
		case 63			:			Script63 ();		break;

        case 70         :           Script70 ();        break;
        case 71         :           Script71 ();        break;
        case 72         :           Script72 ();        break;

		case 80			:			Script80 ();		break;
		case 81			:			Script81 ();		break;
        case 82         :           Script82 ();        break;
        case 83         :           Script83 ();        break;

        case 85         :           Script85 ();        break;

        case 90         :           Script90 ();        break;
        case 91         :           Script91 ();        break;

        case 92         :           Script92 ();        break;
        case 93         :           Script93 ();        break;
	
		default:	
			UplinkWarning ( "Unknown script index" );
			break;
			
	}

}

void ScriptLibrary::Script1 ()
{

	/* 
		PURPOSE : A test script to make sure this Script Library class works.
		Sends an email to the player.

		OCCURS  : Removed from the game
		
		*/

	Message *msg = new Message ();
	msg->SetTo		( "PLAYER" );
	msg->SetFrom	( "Script Library" );
	msg->SetSubject ( "Test message from Script Library" );
	msg->SetBody	( "Body of message.  Looks like it works!" );
	msg->Send		();

}

void ScriptLibrary::Script10 ()
{

	/*
		PURPOSE : To perform a money transfer from one account to another

		OCCURS  : When the player confirms a transformation on screen
					ie All neccisary data is on current remote interface

		*/

	// Get the source bank computer

	Computer *comp = game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ()->GetComputer ();
	UplinkAssert (comp);

	// Get the source and target values

	char *source_ip    = comp->ip;
	char *target_ip    = EclGetButton ( "t_ip_value 0 0" )->caption;
	char *source_accno = EclGetButton ( "s_accno_value -1 -1" )->caption;	
	char *target_accno = EclGetButton ( "t_accno_value 0 0" )->caption;
	char *value        = EclGetButton ( "amount_value 0 0" )->caption;

	if ( strlen( target_ip ) >= SIZE_VLOCATION_IP )
		target_ip [ SIZE_VLOCATION_IP - 1 ] = '\0';

	if ( strlen( source_accno ) >= 16 )
		source_accno [ 16 - 1 ] = '\0';

	if ( strlen( target_accno ) >= 16 )
		target_accno [ 16 - 1 ] = '\0';

	if ( strlen( value ) >= 16 )
		value [ 16 - 1 ] = '\0';

	int i_value;
	sscanf ( value, "%d", &i_value );

	// Check the player has filled in the correct values
	
	if ( strcmp ( target_accno, "Fill this in" ) == 0 
	  || strcmp ( target_accno, "Fill this in" ) == 0 
	  || strcmp ( value,		"Fill this in" ) == 0 )
		return;

	// Prevent if it's not his account and a proxy is running
	
	if ( !game->GetWorld ()->GetPlayer ()->IsPlayerAccount ( source_ip, source_accno ) &&
		  comp->security.IsRunning_Proxy () ) {
		create_msgbox ( "Error", "Denied access by Proxy server" );
		return;
	}

	// Do the transfer

	bool success = BankAccount::TransferMoney ( source_ip, source_accno, target_ip, target_accno, 
												i_value, game->GetWorld ()->GetPlayer () );

	if ( success ) {

		// Change to a success screen
		
		game->GetInterface ()->GetRemoteInterface ()->RunScreen ( 8, comp );

	}
	else {

		// Transfer failed
		game->GetInterface ()->GetRemoteInterface ()->RunScreen ( 9, comp );

	}

}

void ScriptLibrary::Script11 ()
{

	/*
		PURPOSE : To create a new account
 				  at a bank
		(Based on the captions in the onscreen dialog)

		*/

	
	// Get the inputted properties of the new account

	char *name    = EclGetButton ( "nametext 0 0" )->caption;
	char *passwd  = EclGetButton ( "passwordtext 0 0" )->caption;
	char *passwd2 = EclGetButton ( "passwordtext2 0 0" )->caption;

    if ( strcmp ( name, "Fill this in" ) == 0 || strlen ( name ) == 0 ) {
        create_msgbox ( "Error", "You must enter an username" );
        return;
    }

    if ( strlen ( passwd ) == 0 ) {
        create_msgbox ( "Error", "You must enter a password" );
        return;
    }

	if ( strcmp ( passwd, passwd2 ) != 0 ) {
		create_msgbox ( "Error", "The two passwords must be identical" );
		return;
	}

	if ( strlen( name ) >= 33 )
		name [ 33 - 1 ] = '\0';

	if ( strlen ( passwd ) >= 33 )
		passwd [ 33 - 1 ] = '\0';

	// Get the bank computer

	Computer *comp = game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ()->GetComputer ();
	UplinkAssert (comp);
	BankComputer *bank = (BankComputer *) comp;

	// Verify the name is unique

	DArray <BankAccount *> *baccounts = bank->accounts.ConvertToDArray ();

	for ( int i = 0; i < baccounts->Size(); i++ )
		if ( baccounts->ValidIndex ( i ) )
			if ( strcmp ( name, baccounts->GetData ( i )->name ) == 0 ) {
				delete baccounts;
				create_msgbox ( "Error", "You must enter an unique username" );
				return;
			}

	delete baccounts;

	// Create the account

	int accno = game->GetWorld ()->GetPlayer ()->CreateNewAccount ( comp->ip, name, passwd, 0, 0 );
	game->GetWorld ()->GetPlayer ()->GiveLink ( comp->ip );

	// Log the player in as this person

	char saccno [16];
	UplinkSnprintf( saccno, sizeof ( saccno ), "%d", accno );
	game->GetInterface ()->GetRemoteInterface ()->SetSecurity ( saccno, 3 );

	
	// Change to success screen

	game->GetInterface ()->GetRemoteInterface ()->RunScreen ( 5, comp );

}

void ScriptLibrary::Script12 ()
{

	/*
		PURPOSE : To create a new account
 				  at the stock market
		(Based on the captions in the onscreen dialog)

		*/

	
	// Get the inputted properties of the new account

	char *name    = EclGetButton ( "nametext 0 0" )->caption;
	char *passwd  = EclGetButton ( "passwordtext 0 0" )->caption;
	char *passwd2 = EclGetButton ( "passwordtext2 0 0" )->caption;

	if ( strcmp ( passwd, passwd2 ) != 0 ) {
		create_msgbox ( "Error", "The two passwords must be identical" );
		return;
	}

	// Get the stock market

	Computer *comp = game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ()->GetComputer ();
	UplinkAssert (comp);
	
	// Create the account

	if ( strlen( name ) >= 33 )
		name [ 33 - 1 ] = '\0';

	if ( strlen ( passwd ) >= 33 )
		passwd [ 33 - 1 ] = '\0';

	Record *record = new Record ();
	record->AddField ( RECORDBANK_NAME, name );
    record->AddField ( RECORDBANK_PASSWORD, passwd );
	record->AddField ( RECORDBANK_SECURITY, "3" );
	comp->recordbank.AddRecord ( record );
	
	// Give the player his new access codes

	char code [128];
    Computer::GenerateAccessCode( name, passwd, code, sizeof ( code ) );

	game->GetWorld ()->GetPlayer ()->GiveLink ( comp->ip );
	game->GetWorld ()->GetPlayer ()->GiveCode ( comp->ip, code );

	// Log the player in as this person

	game->GetInterface ()->GetRemoteInterface ()->SetSecurity ( name, 3 );
	
	// Change to success screen

	game->GetInterface ()->GetRemoteInterface ()->RunScreen ( 3, comp );

}

void ScriptLibrary::Script13 ()
{

	// Look up the BankAccount object and store it
	VLocation *vl = game->GetWorld ()->GetPlayer ()->GetRemoteHost ();
    UplinkAssert (vl);
    Computer *comp = vl->GetComputer ();
    UplinkAssert (comp);
	UplinkAssert (comp->GetOBJECTID () == OID_BANKCOMPUTER );
	BankComputer *bank = (BankComputer *) comp;

	// Look up the account number based on the log in name
	Record *rec = bank->recordbank.GetRecordFromName (game->GetInterface ()->GetRemoteInterface ()->security_name);
    UplinkAssert (rec);

	char *accno = rec->GetField ( RECORDBANK_ACCNO );
    UplinkAssert (accno);

	// Now look up the account based on that account number
	
    BankAccount *account = bank->accounts.GetData ( accno );
    UplinkAssert (account);

    // Only allow close if empty
    // TODO finish this

    if ( account->balance != 0 ) {

        create_msgbox ( "Error", "Your account cannot be closed.\n"
                                 "The balance must be zero." );

        game->GetInterface ()->GetRemoteInterface ()->RunScreen ( 3, comp );
        return;

    }
    else if ( account->loan != 0 ) {

        create_msgbox ( "Error", "Your account cannot be closed.\n"
                                 "The loan must be zero." );

        game->GetInterface ()->GetRemoteInterface ()->RunScreen ( 3, comp );
        return;

    }
    else {

        int iaccno;
        sscanf ( accno, "%d", &iaccno );        
        bank->CloseAccount ( iaccno );

        game->GetInterface ()->GetRemoteInterface ()->SetSecurity ( "Guest", 10 );
        game->GetInterface ()->GetRemoteInterface ()->RunScreen ( 3, comp );

    }
        
}

void ScriptLibrary::Script15 ()
{

	/*
		PURPOSE : To search for a Criminal Record and display it on 
		a criminal record screen.

		OCCURS : The player is logged onto the Global Criminal Database
		and clicks on SEARCH

		*/

	// Get the search string

	char name [SIZE_PERSON_NAME];
	Button *button = EclGetButton ( "name 0 0" );
	UplinkAssert ( button );
	strncpy ( name, button->caption, sizeof ( name ) );
	name [ sizeof ( name ) - 1 ] = '\0';

	// Run the Criminal Record Screen

	game->GetInterface ()->GetRemoteInterface ()->RunScreen ( 8, game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ()->GetComputer () );
	
	// Start the search going

	CriminalScreenInterface *csi = (CriminalScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert ( csi );
	UplinkAssert ( csi->ScreenID () == SCREEN_CRIMINALSCREEN );
	csi->SetSearchName ( name );

}

void ScriptLibrary::Script16 ()
{

	/*
		PURPOSE : To search for an Academic Record and display it on 
		a criminal record screen.

		OCCURS : The player is logged onto the International Academic Database
		and clicks on SEARCH

		*/

	// Get the search string

	char name [SIZE_PERSON_NAME];
	Button *button = EclGetButton ( "name 0 0" );
	UplinkAssert ( button );
	strncpy ( name, button->caption, sizeof ( name ) );
	name [ sizeof ( name ) - 1 ] = '\0';

	// Run the Academic Record Screen

	game->GetInterface ()->GetRemoteInterface ()->RunScreen ( 6, game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ()->GetComputer () );
	
	// Start the search going

	AcademicScreenInterface *asi = (AcademicScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert ( asi );
	UplinkAssert ( asi->ScreenID () == SCREEN_ACADEMICSCREEN );
	asi->SetSearchName ( name );

}

void ScriptLibrary::Script17 ()
{

	/*
		PURPOSE : To search for a Social Security record and display it on 
		a record screen.

		OCCURS : The player is logged onto the Social Security Database
		and clicks on SEARCH

		*/

	// Get the search string

	char name [SIZE_PERSON_NAME];
	Button *button = EclGetButton ( "name 0 0" );
	UplinkAssert ( button );
	strncpy ( name, button->caption, sizeof ( name ) );
	name [ sizeof ( name ) - 1 ] = '\0';

	// Run the Record Screen

	game->GetInterface ()->GetRemoteInterface ()->RunScreen ( 6, game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ()->GetComputer () );
	
	// Start the search going

	SocialSecurityScreenInterface *sssi = (SocialSecurityScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert ( sssi );
	UplinkAssert ( sssi->ScreenID () == SCREEN_SOCSECSCREEN );
	sssi->SetSearchName ( name );

}

void ScriptLibrary::Script30 ()
{

	/*
		PURPOSE : Script 3X are used in the first time users start sequence
		They handle the timing of certain events such as game loading

		*/

	// Bring up first caption
	
	EclRegisterCaptionChange ( "firsttimeloading_text", "Establishing connection to UPLINK public access server...", 1500 );
	GciTimerFunc ( 1500, RunScript, 31 );

}

void ScriptLibrary::Script31 ()
{

	if ( !EclGetButton ( "start_localhost" ) ) {
	
		// First time through

		EclRegisterButton ( 230, 200, 64, 78, " ", " ", "start_localhost" );
		
#ifdef WIN32		
		button_assignbitmap ( "start_localhost", "start/localhost_win32.tif" );
#else
		button_assignbitmap ( "start_localhost", "start/localhost_linux.tif" );	
#endif

		EclRegisterButton ( 330, 220, 32, 32, " ", " ", "start_key" );
		button_assignbitmap ( "start_key", "analyser/nonsecure.tif" );

		Button *key = EclGetButton ( "start_key" );
		UplinkAssert (key);

		EclRegisterButton ( key->x - 43, key->y + key->height/2 - 1, 42, 4, " ", " ", "start_keylink" );
		EclRegisterButtonCallbacks ( "start_keylink", DrawConnection, NULL, NULL, NULL );
		
		GciTimerFunc ( 800, RunScript, 31 );

	}
	else if ( !EclGetButton ( "start_target" ) ) {

		// Second time through

		EclRegisterButton ( 690, 320, 100, 100, " ", " ", "start_target" );
		button_assignbitmap ( "start_target", "start/publicaccessserver.tif" );
		
		EclRegisterButton ( 650, 340, 32, 32, " ", " ", "start_lock" );
		button_assignbitmap ( "start_lock", "analyser/secure.tif" );

		Button *lock = EclGetButton ( "start_lock" );
		UplinkAssert (lock);

		int lockX = 450 + lock->width;
		int lockY = 340 + lock->height/2 - 1;

		EclRegisterButton ( lockX + 200, lockY, 31, 4, " ", " ", "start_locklink" );
		EclRegisterButtonCallbacks ( "start_locklink", DrawConnection, NULL, NULL, NULL );

		EclRegisterMovement ( "start_target", 490, 320, 500 );
		EclRegisterMovement ( "start_lock", 450, 340, 500 );
		EclRegisterMovement ( "start_locklink", lockX, lockY, 500 );

        SgPlaySound ( RsArchiveFileOpen ( "sounds/newmail.wav" ), "sounds/newmail.wav", true );

		GciTimerFunc ( 800, RunScript, 31 );

	}
	else {

		// Third time through

	    PhoneDialler *pd = new PhoneDialler ();
		pd->DialNumber ( 400, 170, IP_UPLINKPUBLICACCESSSERVER, 1 );    

		// See the next step in script 92

	}
	
}


void ScriptLibrary::DrawConnection ( Button *button, bool highlighted, bool clicked )
{

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
	glEnable ( GL_SCISSOR_TEST );

	glColor4f ( 0.3f, 0.5f, 1.0f, 1.0f );
	glLineWidth ( 2.0 );
//    glLineStipple ( 2, 0x1111 );
//    glEnable ( GL_LINE_STIPPLE );

	if ( button->height == 4 ) {

		// Horizontal button
		glBegin ( GL_LINES );
			glVertex2i ( button->x + 1, button->y + 1 );
			glVertex2i ( button->x + 1 + button->width - 1, button->y + 1 );
		glEnd ();

	}
	else if ( button->width == 4 ) {
		
		// Vertical button
		glBegin ( GL_LINES );
			glVertex2i ( button->x + 1, button->y + 1 );
			glVertex2i ( button->x + 1, button->y + button->height - 1 );
		glEnd ();

	}
	else {

		// Diagonal button
		glBegin ( GL_LINES );
			glVertex2i ( button->x + 1, button->y + 1 );
			glVertex2i ( button->x + button->width - 1, button->y + button->height - 1 );
		glEnd ();

	}

	glLineWidth ( 1.0 );
//    glDisable ( GL_LINE_STIPPLE );
	glDisable ( GL_SCISSOR_TEST );

}

void ScriptLibrary::Script32 ()
{

	// Load world
	// then start game running

	game->NewGame ();	

    SgPlaySound ( RsArchiveFileOpen ( "sounds/phonepickup.wav" ), "sounds/phonepickup.wav", true );

	// Remove the connection buttons

	EclRemoveButton ( "start_localhost" );
	EclRemoveButton ( "start_key" );
	EclRemoveButton ( "start_keylink" );
//	EclRemoveButton ( "start_target" );
	EclRemoveButton ( "start_lock" );
	EclRemoveButton ( "start_locklink" );
	EclRemoveButton ( "start_link" );

	app->GetMainMenu ()->Remove ();

}

void ScriptLibrary::Script33 ()
{

	/*
		PURPOSE : To register the player as an Uplink Agent after he has filled in
		all of his details.

		OCCURS  : When clicking on the OK button during Uplink Registration.

		*/

	char name [SIZE_AGENT_HANDLE];
	char password [33];
	char password2 [33];
	char accesscode [SIZE_AGENT_HANDLE + 32 + 32];

	strncpy ( name, EclGetButton ( "nametext 0 0" )->caption, sizeof( name ) );
	name [ sizeof( name ) - 1 ] = '\0';

	strncpy ( password, EclGetButton ( "passwordtext 0 0" )->caption, sizeof( password ) );
	password [ sizeof( password ) - 1 ] = '\0';

	strncpy ( password2, EclGetButton ( "passwordtext2 0 0" )->caption, sizeof( password2 ) );
	password2 [ sizeof( password2 ) - 1 ] = '\0';
	
    Computer::GenerateAccessCode( name, password, accesscode, sizeof ( accesscode ) );

	if ( strcmp ( game->GetWorld ()->GetPlayer ()->handle, "NEWAGENT" ) != 0 ) {
		create_msgbox ( "Error", "Our records show you are already\n"
                                 "registered as an Uplink agent." );
		return;
	}

    if ( strcmp ( name, "Fill this in" ) == 0 ) {
        create_msgbox ( "Error", "You must first enter a username" );
        return;
    }

	if ( strchr ( name, ':' ) || strchr ( name, '/' ) || strchr ( name, '\\' ) ||
		 strchr ( name, '?' ) || strchr ( name, '.' ) || strchr ( name, ',' ) ||
		 strchr ( name, '"' ) || strchr ( name, '<' ) || strchr ( name, '>' ) ||
		 strchr ( name, '|' ) || strchr ( name, '*' ) ) {

		create_msgbox ( "Error", "Usernames cannot contain\n"
                                 "any of these characters:\n"
								 " : / \\ ? . , \" < > | * " );
		return;

	}

	if ( strcmp ( name, RECORDBANK_ADMIN ) == 0 || 
	     strcmp ( name, RECORDBANK_READWRITE ) == 0 || 
	     strcmp ( name, RECORDBANK_READONLY ) == 0 ) {

		create_msgbox ( "Error", "Usernames cannot be\n"
                                 RECORDBANK_ADMIN ", " RECORDBANK_READWRITE " or " RECORDBANK_READONLY "\n" );
		return;
	}

    if ( password[0] == '\x0' ) {
        create_msgbox ( "Error", "You must first enter a password" );
        return;
    }

	if ( strcmp ( password, password2 ) != 0 ) {
		create_msgbox ( "Error", "The two passwords must be identical" );
		return;
	}

	game->GetWorld ()->GetPlayer ()->SetHandle ( name );
	
	// Open a new account with Uplink International Bank

    Computer *bank = game->GetWorld ()->GetComputer ( NameGenerator::GenerateInternationalBankName("Uplink") );
	UplinkAssert (bank);
	int accno = game->GetWorld ()->GetPlayer ()->CreateNewAccount ( bank->ip, name, password, 
																	0, PLAYER_START_BALANCE );
	game->GetWorld ()->GetPlayer ()->GiveLink ( bank->ip );

	// Store the new player's account on Uplink's computer

	Record *record = new Record ();
	record->AddField ( RECORDBANK_NAME, name );
	record->AddField ( RECORDBANK_ACCNO, accno );
	record->AddField ( RECORDBANK_PASSWORD, password );
	record->AddField ( RECORDBANK_SECURITY, "5" );
	record->AddField ( "Created", game->GetWorld ()->date.GetLongString () );
	Computer *comp = game->GetWorld ()->GetComputer ( NAME_UPLINKINTERNALSERVICES );
	comp->recordbank.AddRecord ( record );

	// Create the rest of your records

	RecordGenerator::GenerateRecords_Player ( name );

	// Change his uplink rating to beginner
	
	game->GetWorld ()->GetPlayer ()->rating.SetUplinkRating ( 1 );
	
	
	// Send notification to the player
	//

	//Message *msg = new Message ();
	//msg->SetTo		( "PLAYER" );
	//msg->SetFrom	( "Uplink public access system" );
	//msg->SetSubject ( "Welcome to Uplink" );
	//msg->SetBody	( "Your details have been entered into our account.  Congratulations, you are now "
	//				  "an officially rated Uplink Agent.\n"
	//				  "We recommend you complete the Uplink Test mission before attempting any real work.\n"
	//				  "Your first stopping off point should be the Uplink Internal Services System.  Once there, "
	//				  "log in and click on 'help', followed by 'getting started'.\n"
	//				  "Good luck." );
	//msg->GiveLink   ( IP_UPLINKINTERNALSERVICES );
	//msg->GiveCode   ( IP_UPLINKINTERNALSERVICES, accesscode );
	//msg->Send		();
	
	Message *msg = new Message ();
	msg->SetTo ( "PLAYER" );
	msg->SetFrom ( "Benpowerman" );
	msg->SetSubject ("Welcome to my mod");
	msg->SetBody (" Welcome to my mod hope enjoy the free uplink software i put on the uplink test machine"
					" also thank Dna Stormchild and myself Benpowerman i have disable the trace on the test machine password is rosebud type in ben as a uplink agent to get the cheat menu" );
	game->GetWorld ()->GetPlayer ()->rating.SetUplinkRating ( game->GetWorld ()->GetPlayer ()->rating.uplinkrating + 1 );
    game->GetWorld ()->GetPlayer ()->rating.ChangeUplinkScore ( 1 );msg->GiveLink   ( IP_UPLINKINTERNALSERVICES );
	msg->GiveCode   ( IP_UPLINKINTERNALSERVICES, accesscode ); game->GetWorld ()->GetPlayer ()->gateway.hudupgrades = 255; msg->Send ();


	// Send the player a test mission as a tutorial
	//

	//Mission *mission = new Mission ();
	//mission->SetTYPE ( MISSION_STEALFILE );
	//mission->SetCompletion ( IP_UPLINKINTERNALSERVICES, "Uplink test data", NULL, NULL, NULL );
	//mission->SetEmployer ( "Uplink" );
	//mission->SetContact ( "internal@Uplink.net" );
	//mission->SetPayment ( 0 );
	//mission->SetDifficulty ( 1 );
	//mission->SetDescription ( "Uplink Test Mission -\nSteal data from a file server" );
	//mission->SetFullDetails ( "Gain access to the Uplink Test Machine by breaking the outer security layer.\n"
	//						  "Access the fileserver and download the target file 'Uplink Test Data'.\n"
	//						  "Remove any traces of your hack.\n"
	//						  "Return the data to us at address internal@Uplink.net" );
	//mission->GiveLink ( IP_UPLINKTESTMACHINE );
						 
//	game->GetWorld ()->GetPlayer ()->GiveMission (mission);

	// Run the next screen

#ifdef TESTGAME
	game->GetInterface ()->GetRemoteInterface ()->RunScreen ( 5 );
#else
	game->GetInterface ()->GetRemoteInterface ()->RunScreen ( 4 );    
#endif

}

void ScriptLibrary::Script34 ()
{

	// Animation of opening connection to gateway

	if ( !EclGetButton ( "start_localhost" ) ) {

		// First time through - rebuild the connection graphics

		EclRegisterButton ( 230, 200, 64, 78, " ", " ", "start_localhost" );

#ifdef WIN32		
		button_assignbitmap ( "start_localhost", "start/localhost_win32.tif" );
#else
		button_assignbitmap ( "start_localhost", "start/localhost_linux.tif" );
#endif

		EclRegisterButton ( 330, 220, 32, 32, " ", " ", "start_key" );
		button_assignbitmap ( "start_key", "analyser/nonsecure.tif" );

		Button *key = EclGetButton ( "start_key" );
		UplinkAssert (key);

		EclRegisterButton ( key->x - 43, key->y + key->height/2 - 1, 42, 4, " ", " ", "start_keylink" );
		EclRegisterButtonCallbacks ( "start_keylink", DrawConnection, NULL, NULL, NULL );

		EclRegisterButton ( 450, 340, 32, 32, " ", " ", "start_lock" );
		button_assignbitmap ( "start_lock", "analyser/secure.tif" );

		Button *lock = EclGetButton ( "start_lock" );
		UplinkAssert (lock);

		int lockX = lock->x + lock->width;
		int lockY = lock->y + lock->height/2 - 1;

		EclRegisterButton ( lockX, lockY, 31, 4, " ", " ", "start_locklink" );
		EclRegisterButtonCallbacks ( "start_locklink", DrawConnection, NULL, NULL, NULL );

		EclRegisterButton ( key->x + key->width/2 + 8, key->y + key->height/2 + 8, lock->x - key->x - 16, lock->x - key->x - 16, " ", " ", "start_link" );
		EclRegisterButtonCallbacks ( "start_link", DrawConnection, NULL, NULL, NULL );

		EclRegisterCaptionChange ( "connecting 0 0", "Disconnecting from Uplink Public access server...", 700 );

		GciTimerFunc ( 800, RunScript, 34 );

	}
	else if ( EclGetButton ( "start_link" ) ) {

		// Second time through - remove connection line

		EclRemoveButton ( "start_link" );
		
		//Removed, wasn't heard when using mikmod
        //SgPlaySound ( RsArchiveFileOpen ( "sounds/menuitemclick.wav" ), "sounds/menuitemclick.wav", true );

		GciTimerFunc ( 600, RunScript, 35 );

	}

}

void ScriptLibrary::Script35 () 
{

	if ( EclGetButton ( "start_target" ) && EclGetButton ("start_target")->x < 640 ) {

		// First time through - scroll target off screen

		Button *lock = EclGetButton ( "start_lock" );
		UplinkAssert (lock);

		int lockX = lock->x + lock->width;
		int lockY = lock->y + lock->height/2 - 1;

		EclRegisterMovement ( "start_target", 690, 320, 500 );
		EclRegisterMovement ( "start_lock", 650, 340, 500 );
		EclRegisterMovement ( "start_locklink", lockX + 200, lockY, 500 );

        SgPlaySound ( RsArchiveFileOpen ( "sounds/newmailreverse.wav" ), "sounds/newmailreverse.wav", true );

		EclRegisterCaptionChange ( "connecting 0 0", "Establishing connection to your GATEWAY computer...", 600 );

		GciTimerFunc ( 1000, RunScript, 35 );

	}
	else if ( EclGetButton ( "start_target" ) && EclGetButton ( "start_target")->x > 640 ) {

		// Second time through - replace target with Gateway, scroll back

		EclRemoveButton ( "start_target" );

		EclRegisterButton ( 706, 320, 64, 80, " ", " ", "start_target2" );
		button_assignbitmap ( "start_target2", "start/gateway.tif" );
		EclButtonSendToBack ( "start_target2" );

		Button *lock = EclGetButton ( "start_lock" );
		UplinkAssert (lock);
		int lockX = 450 + lock->width;
		int lockY = 340 + lock->height/2 - 1;
		
		EclRegisterMovement ( "start_target2", 506, 320, 500 );
		EclRegisterMovement ( "start_lock", 450, 340, 500 );
		EclRegisterMovement ( "start_locklink", lockX, lockY, 500 );

        SgPlaySound ( RsArchiveFileOpen ( "sounds/newmail.wav" ), "sounds/newmail.wav", true );

		GciTimerFunc ( 1000, RunScript, 35 );

	}
	else if ( !EclGetButton ( "start_target" ) ) {

		// Third time through - connect to gateway

	    PhoneDialler *pd = new PhoneDialler ();
		pd->DialNumber ( 400, 170, IP_LOCALHOST, 2 );    

		// See the next step in script 93

	}

}

void ScriptLibrary::Script36 ()
{

	/*
		
		PURPOSE : Connects the user to his Gateway for the first time
		OCCURS  : Right after registration

		*/

	EclRemoveButton ( "start_localhost" );
	EclRemoveButton ( "start_key" );
	EclRemoveButton ( "start_keylink" );
	EclRemoveButton ( "start_target2" );
	EclRemoveButton ( "start_lock" );
	EclRemoveButton ( "start_locklink" );
	EclRemoveButton ( "start_link" );


	game->GetWorld ()->GetPlayer ()->GetConnection ()->Disconnect ();
	game->GetWorld ()->GetPlayer ()->GetConnection ()->Reset ();

	game->GetInterface ()->GetRemoteInterface ()->RunNewLocation ();
	game->GetInterface ()->GetRemoteInterface ()->RunScreen ( 5 );		

#ifdef TESTGAME
	GciTimerFunc ( 100, RunScript, 42 );
#else
	GciTimerFunc ( 100, RunScript, 40 );
#endif

}

void ScriptLibrary::Script40 ()
{
	
	/*

		PURPOSE : Simulates the downloading of the Uplink Operating System
		OCCURS : During first-time start sequence

		*/

	if ( !EclGetButton ( "downloadingUOS" ) ) {

		// Button doesn't exist yet

		EclRegisterButton ( 180, 180, 300, 20, "Downloading UPLINK Operating System...", "", "downloadingUOS_title" );
		EclRegisterButtonCallbacks ( "downloadingUOS_title", textbutton_draw, NULL, NULL, NULL );
		
		EclRegisterButton ( 190, 210, 0, 15, "", "Shows the progress of the download", "downloadingUOS" );
		EclRegisterResize ( "downloadingUOS", 200, 15, 5000 );		

		EclRegisterButton ( 150, 210, 40, 15, "", "0 %", "downloadingUOS_prog" );
		EclRegisterButtonCallbacks ( "downloadingUOS_prog", textbutton_draw, NULL, NULL, NULL );
		
		EclRegisterButton ( 190, 230, 300, 15, " ", " ", "downloadingUOS_current" );
		EclRegisterButtonCallbacks ( "downloadingUOS_current", textbutton_draw, NULL, NULL, NULL );

		GciTimerFunc ( 100, RunScript, 40 );

	}
	else {

		// Download is occuring
		// Set the percentage done so far

		Button *button = EclGetButton ( "downloadingUOS" );
		UplinkAssert (button);

		int progress = (int) (100.0 * (float) button->width / 200.0);

		char caption [16];
		UplinkSnprintf ( caption, sizeof ( caption ), "%d %%", progress );
		
		Button *prog = EclGetButton ( "downloadingUOS_prog" );
		UplinkAssert (prog);
		prog->SetCaption ( caption );

		// Put down some description of what we're downloading...

		Button *current = EclGetButton ( "downloadingUOS_current" );
		UplinkAssert (current);

		if      ( progress < 5  )		current->SetCaption ( "Downloading Core services" );
		else if ( progress < 10 )		current->SetCaption ( "Downloading Kernel" );
		else if ( progress < 15 )		current->SetCaption ( "Downloading Loader package" );
		else if ( progress < 20 )		current->SetCaption ( "Downloading Gateway package" );
		else if ( progress < 25 )		current->SetCaption ( "Downloading Memory package" );
		else if ( progress < 30 )		current->SetCaption ( "Downloading Status package" );
		else if ( progress < 35 )		current->SetCaption ( "Downloading Finance package" );
		else if ( progress < 40 )		current->SetCaption ( "Downloading Mail package" );
		else if ( progress < 45 )		current->SetCaption ( "Downloading Mission package" );
		else if ( progress < 50 )		current->SetCaption ( "Downloading Clock package" );
		else if ( progress < 55 )		current->SetCaption ( "Downloading Multitasking package" );
		else if ( progress < 60 )		current->SetCaption ( "Downloading Map package" );
		else if ( progress < 75 )		current->SetCaption ( "Downloading Global communications package" );
		else if ( progress < 80 )		current->SetCaption ( "Downloading Security package" );
		else if ( progress < 85 )		current->SetCaption ( "Downloading File utilities" );
		else if ( progress < 90 )		current->SetCaption ( "Downloading Tutorial daemon" );
		else if ( progress < 95 )		current->SetCaption ( "Downloading Remote renderer package" );
		else if ( progress < 100 )		current->SetCaption ( "Downloading Site connect package" );

		if ( progress < 100 ) 
			GciTimerFunc ( 100, RunScript, 40 );

		else
			GciTimerFunc ( 500, RunScript, 41 );

	}

}

void ScriptLibrary::Script41 ()
{
	
	/*

		PURPOSE : Runs the newly downloaded Uplink OS
				(pretends to)

	*/

	if ( EclGetButton ( "syscheckUOS_13" ) ) {
	
		// All finished - create the hud 
		GciTimerFunc ( 250, RunScript, 42 );

	}
	else {
	
		// Run each system check on the new Operating System

		for ( int i = 0; i < 14; ++i ) {

			char buttonname [32];
			UplinkSnprintf ( buttonname, sizeof ( buttonname ), "syscheckUOS_%d", i );

			if ( !EclGetButton ( buttonname ) ) {

				// We are running test #i

				if ( i == 0 ) {

					// First time round - remove previous screen

					EclRemoveButton ( "downloadingUOS_title" );
					EclRemoveButton ( "downloadingUOS_prog" );
					EclRemoveButton ( "downloadingUOS_current" );
					EclRemoveButton ( "downloadingUOS" );

				}
				else {

					// Not first time round - create 'OK' button
					char okname [32];
					UplinkSnprintf ( okname, sizeof ( okname ), "syscheckUOS_OK_%d", (i-1) );
					EclRegisterButton ( 450, 130 + 20 * (i-1), 30, 15, "[OK]", "", okname );
					EclRegisterButtonCallbacks ( okname, textbutton_draw, NULL, NULL, NULL );

				}

				// Now create current test button

				char caption [128];
				
				switch ( i ) {
					case 0		:		UplinkStrncpy ( caption, "Checking gateway HARDWARE : Trinity Ax086 2GQs cpu...", sizeof ( caption ) );			break;
					case 1		:		UplinkStrncpy ( caption, "Checking gateway HARDWARE : K256 512KQds modem...", sizeof ( caption ) );				break;
					case 2		:		UplinkStrncpy ( caption, "Checking gateway HARDWARE : 24 GQds DR-RAM memory...", sizeof ( caption ) );			break;
					case 3		:		UplinkStrncpy ( caption, "Verifying integrity of Uplink Operating System...", sizeof ( caption ) );				break;
					case 4		:		UplinkStrncpy ( caption, "Verifying security of Uplink Operating System...", sizeof ( caption ) );				break;
					case 5		:		UplinkStrncpy ( caption, "Installing UPLINK Operating system files...", sizeof ( caption ) );						break;
					case 6		:		UplinkStrncpy ( caption, "Initialising TSR programs in memory...", sizeof ( caption ) );							break;
					case 7		:		UplinkStrncpy ( caption, "Sending RUN signal to core Uplink Kernel...", sizeof ( caption ) );						break;
					case 8		:		UplinkStrncpy ( caption, "Checking Kernel Initilisation...", sizeof ( caption ) );								break;
					case 9		:		UplinkStrncpy ( caption, "Establishing Network communications protocols...", sizeof ( caption ) );				break;
					case 10		:		UplinkStrncpy ( caption, "Installing File-Copier...", sizeof ( caption ) );										break;
					case 11		:		UplinkStrncpy ( caption, "Installing File-Deleter...", sizeof ( caption ) );										break;
					case 12		:		UplinkStrncpy ( caption, "Installing Task-Manager you can read fast...", sizeof ( caption ) );					break;
					case 13		:		UplinkStrncpy ( caption, "Running Graphical User Interface...", sizeof ( caption ) );								break;
				}

				EclRegisterButton ( 20, 130 + 20 * i, 400, 15, caption, "", buttonname );
				EclRegisterButtonCallbacks ( buttonname, textbutton_draw, NULL, NULL, NULL );

				SgPlaySound ( RsArchiveFileOpen ( "sounds/done.wav" ), "sounds/done.wav" );

				GciTimerFunc ( 250, RunScript, 41 );

				// End now (to avoid creating other buttons)
				return;

			}

		}

	}

}

void ScriptLibrary::Script42 ()
{

	/*

		PURPOSE : Removes start-up sequence
				  + Creates the HUD

	*/

	if ( EclGetButton ( "syscheckUOS_0" ) ) {

		for ( int i = 0; i < 14; ++i ) {
			
			// Remove existing start-up sequence
			char buttonname [32];
			UplinkSnprintf ( buttonname, sizeof ( buttonname ), "syscheckUOS_%d", i );
			EclRemoveButton ( buttonname );
			char okname [32];
			UplinkSnprintf ( okname, sizeof ( okname ), "syscheckUOS_OK_%d", i );
			EclRemoveButton ( okname );

		}

	}

	// Create the HUD

	if ( !game->GetInterface ()->GetLocalInterface ()->IsVisible () ) {

		game->GetInterface ()->GetLocalInterface ()->Reset ();
		game->GetInterface ()->GetLocalInterface ()->Create ();

	}

	// Put this line in to disable "first time" after one game		

	app->GetOptions ()->SetOptionValue ( "game_firsttime", 0 );

	game->GetInterface ()->GetRemoteInterface ()->RunScreen ( 6 );
	
}

void ScriptLibrary::Script43 ()
{

	/*

		PURPOSE : To start the tutorial program running

	*/

	game->GetInterface ()->GetTaskManager ()->RunSoftware ( "Tutorial", 1.0 );
	;
}

void ScriptLibrary::Script45 ()
{
	
	/*

		PURPOSE : Installs a new patch
        OCCURS : When a new version EXE is detected for the first time

	*/

	if ( EclGetButton ( "newpatch_3" ) ) {
	
		// All finished - create the hud 
		GciTimerFunc ( 250, RunScript, 46 );

	}
	else {
	
		// Run a bunch of fake tests

		for ( int i = 0; i < 4; ++i ) {

			char buttonname [32];
			UplinkSnprintf ( buttonname, sizeof ( buttonname ), "newpatch_%d", i );

			if ( !EclGetButton ( buttonname ) ) {

                if ( i == 0 ) {

                    EclRegisterButton ( 120, 40, 405, 52, " ", " ", "newpatch_title" );
                    button_assignbitmap ( "newpatch_title", "mainmenu/newpatch.tif" );

                    //EclRegisterButton ( 150, 90, 330, 12, " ", " ", "newpatch_version" );
                    //button_assignbitmap ( "newpatch_version", "mainmenu/versionnew.tif" );

                }
                else {

				    char okname [32];
				    UplinkSnprintf ( okname, sizeof ( okname ), "newpatch_OK_%d", (i-1) );
				    EclRegisterButton ( 500, 140 + 20 * (i-1), 30, 15, "[OK]", "", okname );
				    EclRegisterButtonCallbacks ( okname, textbutton_draw, NULL, NULL, NULL );

                }

				// Now create current test button

				char caption [128];
				
				switch ( i ) {
					case 0		:		UplinkStrncpy ( caption, "Reading new patch data...", sizeof ( caption ) );			break;
					case 1		:		UplinkStrncpy ( caption, "Modifying Uplink executable...", sizeof ( caption ) );				break;
					case 2		:		UplinkStrncpy ( caption, "Modifying Uplink runtime data...", sizeof ( caption ) );			break;
					case 3		:		UplinkStrncpy ( caption, "Validating patch authorisation with Uplink Corporation...", sizeof ( caption ) );				break;
				}

				EclRegisterButton ( 100, 140 + 20 * i, 400, 15, caption, "", buttonname );
				EclRegisterButtonCallbacks ( buttonname, textbutton_draw, NULL, NULL, NULL );

                SgPlaySound ( RsArchiveFileOpen ( "sounds/done.wav" ), "sounds/done.wav" );

				GciTimerFunc ( 400, RunScript, 45 );

				// End now (to avoid creating other buttons)
				return;

			}

		}

	}

}

void ShowRecommendationsClick ( Button *button )
{

    ScriptLibrary::RunScript ( 47 );

}

void ExitGameClick ( Button *button )
{

	app->Close ();

}

void NewPatchOKClick ( Button *button )
{

//    EclReset ( app->GetOptions ()->GetOptionValue ("graphics_screenwidth"),
//			   app->GetOptions ()->GetOptionValue ("graphics_screenheight") );
//
//    app->GetMainMenu ()->RunScreen ( MAINMENU_LOGIN );

    bool setScreenRes = EclGetButton ( "newpatch_screenrestoggle" )->caption[0] == '1';
    bool newUser = EclGetButton ( "newpatch_newusertoggle" )->caption[0] == '1';

    EclReset ( app->GetOptions ()->GetOptionValue ("graphics_screenwidth"),
			   app->GetOptions ()->GetOptionValue ("graphics_screenheight") );

    create_msgbox ( "Shutdown", "Patch changes complete.\nUplink must now be restarted." );		
    EclRegisterButtonCallback ( "msgbox_close", ExitGameClick );
    EclRegisterButtonCallback ( "msgbox_title", ExitGameClick );

    if ( setScreenRes ) {
////        app->GetOptions ()->SetOptionValue ( "graphics_screenwidth", 800 );
////        app->GetOptions ()->SetOptionValue ( "graphics_screenheight", 600 );
//        app->GetOptions ()->RequestShutdownChange ( "graphics_screenwidth", 800 );
//        app->GetOptions ()->RequestShutdownChange ( "graphics_screenheight", 600 );

		GciScreenMode *mode = GciGetClosestScreenMode ( 1024, 768 );

        app->GetOptions ()->RequestShutdownChange ( "graphics_screenwidth", mode->w );
        app->GetOptions ()->RequestShutdownChange ( "graphics_screenheight", mode->h );

		delete mode;

    }

    if ( newUser ) {
        app->GetOptions ()->SetOptionValue ( "game_firsttime", 1 );
    }

}

void ScriptLibrary::Script46 ()
{

	char okname [32];
	UplinkSnprintf ( okname, sizeof ( okname ), "newpatch_OK_%d", 3 );
	EclRegisterButton ( 500, 140 + 20 * 3, 30, 15, "[OK]", "", okname );
	EclRegisterButtonCallbacks ( okname, textbutton_draw, NULL, NULL, NULL );

    EclRegisterButton ( 50, 270, 550, 190, " ", " ", "newpatch_text"  );

    EclRegisterButtonCallbacks ( "newpatch_text", textbutton_draw, NULL, NULL, NULL );
    EclRegisterCaptionChange ( "newpatch_text", "UPLINK VERSION 1.3 PATCH\n"
                                                "Release notes\n\n"
                                                "Any problems with this patch should be reported to "
                                                "Introversion Software at www.introversion.co.uk.\n\n"
                                                "This patch is backwards-compatible with your user files, however many "
                                                "of the new features will not become active until you create a "
                                                "new user from scratch.\n\n"
                                                "Uplink Corporation recommends you create a new user "
                                                "as soon as possible.",
                                                6000 );

    EclRegisterButton ( 450, 440, 80, 15, "OK", "Click once you have read the information", "newpatch_ok" );
    EclRegisterButtonCallback ( "newpatch_ok", ShowRecommendationsClick );

}

void DrawMainTitle ( Button *button, bool highlighted, bool clicked )
{
	GciDrawText ( button->x, button->y, button->caption, HELVETICA_18 );
}

void ToggleBoxDraw ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert (button);

	if ( button->caption [0] == '0' )
		imagebutton_draw ( button, false, false );

	else
		imagebutton_draw ( button, true, true );

}

void ToggleBoxClick ( Button *button )
{

	UplinkAssert (button);

	// Read the value from the button
	
	if ( button->caption [0] == '0' ) 
		button->SetCaption ( "1" );

	else
		button->SetCaption ( "0" );

}

void ScriptLibrary::Script47 ()
{

    EclReset ( app->GetOptions ()->GetOptionValue ( "graphics_screenwidth" ),
               app->GetOptions ()->GetOptionValue ( "graphics_screenheight" ) );


    EclRegisterButton ( 120, 40, 405, 52, " ", " ", "newpatch_title" );
    button_assignbitmap ( "newpatch_title", "mainmenu/newpatch.tif" );

    //EclRegisterButton ( 150, 90, 330, 12, " ", " ", "newpatch_version" );
    //button_assignbitmap ( "newpatch_version", "mainmenu/versionnew.tif" );

    //
    // Show recommendations to the player

    EclRegisterButton ( 100, 200, 300, 20, "Recommendations", " ", "newpatch_recommendtitle" );
    EclRegisterButtonCallbacks ( "newpatch_recommendtitle", DrawMainTitle, NULL, NULL, NULL );

    //
    // Screen resolution to 1024x768

	GciScreenMode *mode = GciGetClosestScreenMode ( 1024, 768 );
	char messagesetresolution [ 256 ];
	UplinkSnprintf ( messagesetresolution, sizeof ( messagesetresolution ), "Set the in-game screen resolution to %dx%d", mode->w, mode->h );
	delete mode;

    EclRegisterButton ( 150, 250, 300, 15, messagesetresolution, "This is now the recommended in-game resolution.", "newpatch_screenres" );
    EclRegisterButtonCallbacks ( "newpatch_screenres", textbutton_draw, NULL, button_click, button_highlight );

	EclRegisterButton ( 450, 250, 15, 15, "1", "click to Enable or Disable this option", "newpatch_screenrestoggle" );
	button_assignbitmaps ( "newpatch_screenrestoggle", "mainmenu/optionno.tif", "mainmenu/optionyes.tif", "mainmenu/optionyes.tif" );
	EclRegisterButtonCallbacks ( "newpatch_screenrestoggle", ToggleBoxDraw, ToggleBoxClick, button_click, button_highlight );
    
    // 
    // New user profile

    EclRegisterButton ( 150, 280, 300, 15, "Start a new Agent profile", "Your old profiles will work but many new features won't be available", "newpatch_newuser" );
    EclRegisterButtonCallbacks ( "newpatch_newuser", textbutton_draw, NULL, button_click, button_highlight );

	EclRegisterButton ( 450, 280, 15, 15, "1", "click to Enable or Disable this option", "newpatch_newusertoggle" );
	button_assignbitmaps ( "newpatch_newusertoggle", "mainmenu/optionno.tif", "mainmenu/optionyes.tif", "mainmenu/optionyes.tif" );
	EclRegisterButtonCallbacks ( "newpatch_newusertoggle", ToggleBoxDraw, ToggleBoxClick, button_click, button_highlight );

    // OK button

    EclRegisterButton ( 450, 440, 80, 15, "OK", "Click once you have chosen your options", "newpatch_ok" );
    EclRegisterButtonCallback ( "newpatch_ok", NewPatchOKClick );

}

void ScriptLibrary::Script50 ()
{

	/*

		PURPOSE : To bill the player because he blew his last Gateway up, 
		and now wants another

		OCCURS : After Gateway Nuke, log in again, select "Yes" to hire new gateway

		*/

	game->GetWorld ()->GetPlayer ()->ChangeBalance ( COST_UPLINK_NEWGATEWAY * -1, "Uplink Corporation : New gateway" );

    // Recreate some of the missing interface

	EclRegisterButton ( 490, 320, 100, 100, " ", " ", "start_target" );
	button_assignbitmap ( "start_target", "start/publicaccessserver.tif" );
	
}

void ScriptLibrary::Script51 ()
{

	/*

		PURPOSE : To allow a player to say "NO" to hiring a new Gateway
		by logging him out, remembering that his Gateway is still Nuked for next time

		OCCURS : After Gateway Nuke, log in again, select "NO" to hire a new gateway

		*/

	game->GetWorld ()->GetPlayer ()->gateway.nuked = true;

	app->SaveGame ( game->GetWorld ()->GetPlayer ()->handle );				
	game->SetGameSpeed ( GAMESPEED_PAUSED );

    EclReset ( app->GetOptions ()->GetOptionValue ("graphics_screenwidth"),
			   app->GetOptions ()->GetOptionValue ("graphics_screenheight") );

	app->GetMainMenu ()->RunScreen ( MAINMENU_LOGIN );
	

}

void ScriptLibrary::Script60 ()
{

	/*

		PURPOSE : to remove the first email from Act1Scene4agents computer

		OCCURS : When the player is on that agents computer and clicks to remove it

		*/

	DialogScreen *ds = (DialogScreen *) game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
	UplinkAssert (ds);
	UplinkAssert (ds->GetOBJECTID () == OID_DIALOGSCREEN );

	ds->RemoveWidget ( "email1" );
	ds->RemoveWidget ( "view1" );
	ds->RemoveWidget ( "cancel1" );

	/*
	EclRemoveButton ( "email1 0 0" );
	EclRemoveButton ( "view1 62 -1" );
	EclRemoveButton ( "cancel1 60 -1" );
	*/

}

void ScriptLibrary::Script61 ()
{

	/*

		PURPOSE : to remove the second email from Act1Scene4agents computer

		OCCURS : When the player is on that agents computer and clicks to remove it

		*/

	DialogScreen *ds = (DialogScreen *) game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
	UplinkAssert (ds);
	UplinkAssert (ds->GetOBJECTID () == OID_DIALOGSCREEN );

	ds->RemoveWidget ( "email2" );
	ds->RemoveWidget ( "view2" );
	ds->RemoveWidget ( "cancel2" );

	/*
	EclRemoveButton ( "email2 0 0" );
	EclRemoveButton ( "view2 63 -1" );
	EclRemoveButton ( "cancel2 61 -1" );
	*/

	game->GetWorld ()->plotgenerator.PlayerCancelsMail ();

}

void ScriptLibrary::Script62 ()
{

	/*
		
		PURPOSE : To view an email on Act1Scene4agents computer

		OCCURS : When he clicks on the view button for the first scheduled email (subscription)

		*/

	EclRegisterCaptionChange ( "mailtext 0 0", 
							   "I would very much like to continue my subscription to "
							   "Hacker monthly.  Please continue to bill my account and "
							   "send me each issue for the next year.", 2000 );

}

void ScriptLibrary::Script63 ()
{

	/*
		
		PURPOSE : To view an email on Act1Scene4agents computer

		OCCURS : When he clicks on the view button for the second scheduled email (ARC warning)
		We need to resize the button so the whole text will fit in as well

		*/

	Button *b = EclGetButton ( "mailtext 0 0" );
	UplinkAssert (b);

	b->y = 150;
	b->height = 220;

	EclRegisterCaptionChange ( "mailtext 0 0",
			"Agents,\n"
			"If you are reading this email then it means I am dead.  I have been working for "
			"Andromeda for the past month on a top secret project - and until yesterday I did not "
			"even know what exactly I was coding.\n"
			"This story hit the news and I decided that I should try to find out what was happening "
			"with project Revelation - and yesterday I managed to gain access to their stand-alone "
			"server room and copy the files that we were working on.\n"
			"Unfortunately they saw what I was doing and I had to leave quick, and I haven't had time "
			"to analyse the project files yet.  If you read this then they have already caught me.\n"
			"ARC cannot be trusted.  We must come together as a group and find out what they are up to "
			"before something terrible happens.  I don't know if it's already too late.", 2000 );

}


void ScriptLibrary::Script70 ()
{

    /*
        
          PURPOSE : To make a company offer the player money for his agent list

          OCCURS : After the player has downloaded the agent list from Uplink Internal Services

        */


    // First - has the player already done this?

    if ( game->GetWorld ()->plotgenerator.PlayerCompletedSpecialMission (SPECIALMISSION_MOLE) )
        return;


    // No he hasn't.

    Date rundate;
    rundate.SetDate ( &game->GetWorld ()->date );
    rundate.AdvanceDay ( 7 );
    
	NotificationEvent *ne = new NotificationEvent ();
	ne->SetTYPE ( NOTIFICATIONEVENT_TYPE_BUYAGENTLIST );
	ne->SetRunDate ( &rundate );
	game->GetWorld ()->scheduler.ScheduleEvent ( ne );

}

void ScriptLibrary::Script71 ()
{

    // First - has the player already done this?

    if ( game->GetWorld ()->plotgenerator.PlayerCompletedSpecialMission (SPECIALMISSION_MOLE) )
        return;

    // Does the player already have this offer?

    for ( int i = 0; i < game->GetWorld ()->GetPlayer()->missions.Size(); ++i ) {

        Mission *m = game->GetWorld ()->GetPlayer ()->missions.GetData(i);
        UplinkAssert (m);

        if ( strcmp ( m->description, PlotGenerator::SpecialMissionDescription ( SPECIALMISSION_MOLE ) ) == 0 ) 
            return;       

    }

    // No he hasn't.

    Company *employer = WorldGenerator::GetRandomCompany ();
    if ( !employer ) return;

	// Infer the internal services contact address

	char personname [SIZE_PERSON_NAME];
	UplinkSnprintf ( personname, sizeof ( personname ), "internal@%s.net", employer->name );	

	// 
	// Generate a new computer to dump the files to
	//

	Computer *ourcomp = WorldGenerator::GenerateEmptyFileServer ( employer->name );
	UplinkAssert (ourcomp);

	//
	// Add in a new account for the player to use
	//

	char *password = NameGenerator::GeneratePassword ();
	char username [12];
	UplinkSnprintf ( username, sizeof ( username ), "temp%c%c%c%c", 'a' + NumberGenerator::RandomNumber ( 26 ),
															'a' + NumberGenerator::RandomNumber ( 26 ),
															'a' + NumberGenerator::RandomNumber ( 26 ),
															'a' + NumberGenerator::RandomNumber ( 26 ) );

	Record *record = new Record ();
	record->AddField ( RECORDBANK_NAME, username );
	record->AddField ( RECORDBANK_PASSWORD, password );
	record->AddField ( RECORDBANK_SECURITY, "3" );
	ourcomp->recordbank.AddRecord ( record );

	char code [128];
    Computer::GenerateAccessCode( username, password, code, sizeof ( code ) );

    // 
    // Generate the text
    //

    char *description = PlotGenerator::SpecialMissionDescription ( SPECIALMISSION_MOLE );

    std::ostrstream details;
    details << "We have recently heard a rumour going around that says the complete Uplink Agent "
               "List was stolen from their very own Internal Services System, and that this list includes " 
               "a mapping of agent Handles to real-world name.  If true, this data would be of extreme "
               "interest to us and our operations.\n\n"
               "We have also heard a rumour, totally unsubstantiated of course, that you might be the Agent who stole "
               "the data.  Certainly there are only a handful of agents capable, and you are indeed one.\n\n"
               "We have set up a file Server at the address below, and supplied you with a login.  If the rumour is "
               "true, we will be willing to pay you "
            << PAYMENT_SPECIALMISSION_MOLE << " for all of the data files and the Agent List program.  "
               "Payment will be up to the amount specified - if we "
               "dont receive all of the data we will reduce payment accordingly.\n\n"
               "If we have made a mistake and you are not the person with whom we should be talking, then we apologise.  "
               "Your ratings will not change if you choose to abandon this mission - it is totally optional.\n";

    details << "Our fileserver: \n" 
            << ourcomp->ip << "\n"
            << "Login:\n"
            << code << "\n\n"
            << "END"
            << '\x0';

    int payment = 10;

	//
    // Generate the mission
    //

	Mission *mission = new Mission ();
	mission->SetTYPE		 ( MISSION_SPECIAL );
	mission->SetCompletion   ( ourcomp->ip, NULL, NULL, NULL, NULL );    
	mission->SetEmployer     ( employer->name );
	mission->SetContact      ( personname );
	mission->SetPayment      ( payment, payment );
	mission->SetDescription  ( description );	
	mission->SetFullDetails  ( details.str () );
	mission->GiveLink ( ourcomp->ip );
	mission->GiveCode ( ourcomp->ip, code );

	details.rdbuf()->freeze( 0 );
	//delete [] details.str ();

    game->GetWorld ()->GetPlayer ()->GiveMission (mission);

}

void ScriptLibrary::Script72 ()
{

    //
    // Kill a few agents
    //

    for ( int a = 0; a < 5; ++a ) {

        Agent *agent = WorldGenerator::GetRandomAgent();
        UplinkAssert (agent);

        agent->SetStatus (PERSON_STATUS_DEAD);

    }

    //
    // Write a news story
    //

	News *news = new News ();
	news->SetHeadline ( "Multiple Uplink Agent murders reported" );
	news->SetDetails ( "Federal Agents today announced they were investigating a series of murders and "
                       "suicides, all involving active or retired Uplink Agents.  Within the last two days, "
                       "eight Uplink Agents of various skill levels have been executed or have committed suicide.\n\n"
                       "Federal agents are keeping quiet about the identities of those agents killed, and have not "
                       "suggested any connecting factor other than the fact that they all worked freelance for Uplink Corporation.\n\n"
                       "Murders of Uplink Agents are rare - Uplink Corporation is very careful at making sure the agent handles "
                       "are never converted into real-world people, effectively providing anonymity for their agents."
                       );
   
	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (cu);
	cu->CreateNews ( news );

}

void ScriptLibrary::Script80 ()
{

	//
	// Are we on a LAN?

	VLocation *vl = game->GetWorld()->GetPlayer()->GetRemoteHost();
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);

	if ( comp->GetOBJECTID () != OID_LANCOMPUTER ) 
		return;
	
	LanComputer *lan = (LanComputer *) comp;


	//
	// Are we connected to an Authentication Server?
	// If so, do the locking

	int currentIndex = LanMonitor::currentSystem;
	
	if ( lan->systems.ValidIndex(currentIndex) ) {

		LanComputerSystem *system = lan->systems.GetData(currentIndex);
		UplinkAssert (system);

		if ( system->TYPE == LANSYSTEM_AUTHENTICATION ) {

			int unlockIndex1 = system->data1;
			if ( lan->systems.ValidIndex(unlockIndex1) ) {
				LanComputerSystem *lock1 = lan->systems.GetData(unlockIndex1);
				UplinkAssert (lock1);
				lock1->data1 = 1;
			}

			int unlockIndex2 = system->data2;
			if ( lan->systems.ValidIndex(unlockIndex2) ) {
				LanComputerSystem *lock2 = lan->systems.GetData(unlockIndex2);
				UplinkAssert (lock2);
				lock2->data1 = 1;
			}

			int unlockIndex3 = system->data3;
			if ( lan->systems.ValidIndex(unlockIndex3) ) {
				LanComputerSystem *lock3 = lan->systems.GetData(unlockIndex3);
				UplinkAssert (lock3);
				lock3->data3 = 1;
			}

            if ( LanMonitor::IsInConnection( unlockIndex1 ) ||
                 LanMonitor::IsInConnection( unlockIndex2 ) ||
                 LanMonitor::IsInConnection( unlockIndex3 ) ) {

                int currentSystem = LanMonitor::currentSystem;

                char *remotehost = game->GetWorld()->GetPlayer()->remotehost;
  	            VLocation *vl = game->GetWorld ()->GetVLocation ( remotehost );
	            UplinkAssert (vl);
	            Computer *comp = vl->GetComputer ();
	            UplinkAssert (comp);

                if ( comp->TYPE == COMPUTER_TYPE_LAN )
                {

                    LanComputer *lc = (LanComputer *) comp;
					if ( lc->systems.ValidIndex ( currentSystem ) ) {
						LanComputerSystem *system = lc->systems.GetData(currentSystem);
						game->GetInterface()->GetRemoteInterface()->RunScreen ( system->screenIndex, lc );

						game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_NONE );
						game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_LAN );
					}

                }

            }

			create_msgbox( "Success", "Lock System ENABLED" );
            
		}

	}

    //
    // Awaken the sys admin
    //LanMonitor::SysAdminAwaken();

}

void ScriptLibrary::Script81 ()
{

	//
	// Are we on a LAN?

	VLocation *vl = game->GetWorld()->GetPlayer()->GetRemoteHost();
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);

	if ( comp->GetOBJECTID () != OID_LANCOMPUTER ) 
		return;
	
	LanComputer *lan = (LanComputer *) comp;


	//
	// Are we connected to an Authentication Server?
	// If so, do the unlocking

	int currentIndex = LanMonitor::currentSystem;
	
	if ( lan->systems.ValidIndex(currentIndex) ) {

		LanComputerSystem *system = lan->systems.GetData(currentIndex);
		UplinkAssert (system);

		if ( system->TYPE == LANSYSTEM_AUTHENTICATION ) {

			int unlockIndex1 = system->data1;
			if ( lan->systems.ValidIndex(unlockIndex1) ) {
				LanComputerSystem *lock1 = lan->systems.GetData(unlockIndex1);
				UplinkAssert (lock1);
				lock1->data1 = 0;
			}

			int unlockIndex2 = system->data2;
			if ( lan->systems.ValidIndex(unlockIndex2) ) {
				LanComputerSystem *lock2 = lan->systems.GetData(unlockIndex2);
				UplinkAssert (lock2);
				lock2->data1 = 0;
			}

			int unlockIndex3 = system->data3;
			if ( lan->systems.ValidIndex(unlockIndex3) ) {
				LanComputerSystem *lock3 = lan->systems.GetData(unlockIndex3);
				UplinkAssert (lock3);
				lock3->data1 = 0;
			}
		
			create_msgbox( "Success", "Lock System DISABLED" );			

		}

	}


    //
    // Awaken the sys admin
    //LanMonitor::SysAdminAwaken();

}

void ScriptLibrary::Script82 ()
{

	//
	// Are we on a LAN?

	VLocation *vl = game->GetWorld()->GetPlayer()->GetRemoteHost();
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);

	if ( comp->GetOBJECTID () != OID_LANCOMPUTER ) 
		return;
	
	LanComputer *lan = (LanComputer *) comp;


	//
	// Are we connected to an Isolation bridge
	// If so, do the locking

	int currentIndex = LanMonitor::currentSystem;
	
	if ( lan->systems.ValidIndex(currentIndex) ) {

		LanComputerSystem *system = lan->systems.GetData(currentIndex);
		UplinkAssert (system);

		if ( system->TYPE == LANSYSTEM_ISOLATIONBRIDGE ) {

			int unlockIndex1 = system->data1;
			if ( lan->systems.ValidIndex(unlockIndex1) ) {
				LanComputerSystem *lock1 = lan->systems.GetData(unlockIndex1);
				UplinkAssert (lock1);
				lock1->data1 = 1;
			}

			int unlockIndex2 = system->data2;
			if ( lan->systems.ValidIndex(unlockIndex2) ) {
				LanComputerSystem *lock2 = lan->systems.GetData(unlockIndex2);
				UplinkAssert (lock2);
				lock2->data1 = 0;
			}
			
            // If our connection ran through the newly locked system
            // Then reset the bastard

            if ( LanMonitor::IsInConnection( unlockIndex1 ) ) {

                LanMonitor::BeginAttack ();

                int currentSystem = LanMonitor::currentSystem;

                char *remotehost = game->GetWorld()->GetPlayer()->remotehost;
  	            VLocation *vl = game->GetWorld ()->GetVLocation ( remotehost );
	            UplinkAssert (vl);
	            Computer *comp = vl->GetComputer ();
	            UplinkAssert (comp);

                if ( comp->TYPE == COMPUTER_TYPE_LAN )
                {

                    LanComputer *lc = (LanComputer *) comp;
					if ( lc->systems.ValidIndex ( currentSystem ) ) {
						LanComputerSystem *system = lc->systems.GetData(currentSystem);
						game->GetInterface()->GetRemoteInterface()->RunScreen ( system->screenIndex, lc );

						game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_NONE );
						game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_LAN );
					}

                }

            }

			create_msgbox( "Success", "Isolation Bridge ENABLED" );

		}

	}


    //
    // Awaken the sys admin
    //LanMonitor::SysAdminAwaken();

}

void ScriptLibrary::Script83 ()
{


	//
	// Are we on a LAN?

	VLocation *vl = game->GetWorld()->GetPlayer()->GetRemoteHost();
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);

	if ( comp->GetOBJECTID () != OID_LANCOMPUTER ) 
		return;
	
	LanComputer *lan = (LanComputer *) comp;


	//
	// Are we connected to an Isolation bridge
	// If so, do the locking

	int currentIndex = LanMonitor::currentSystem;
	
	if ( lan->systems.ValidIndex(currentIndex) ) {

		LanComputerSystem *system = lan->systems.GetData(currentIndex);
		UplinkAssert (system);

		if ( system->TYPE == LANSYSTEM_ISOLATIONBRIDGE ) {

			int unlockIndex1 = system->data1;
			if ( lan->systems.ValidIndex(unlockIndex1) ) {
				LanComputerSystem *lock1 = lan->systems.GetData(unlockIndex1);
				UplinkAssert (lock1);
				lock1->data1 = 0;
			}

			int unlockIndex2 = system->data2;
			if ( lan->systems.ValidIndex(unlockIndex2) ) {
				LanComputerSystem *lock2 = lan->systems.GetData(unlockIndex2);
				UplinkAssert (lock2);
				lock2->data1 = 1;
			}
			
            // If our connection ran through the newly locked system
            // Then reset the bastard

            if ( LanMonitor::IsInConnection( unlockIndex2 ) ) {

                LanMonitor::BeginAttack ();

                int currentSystem = LanMonitor::currentSystem;

                char *remotehost = game->GetWorld()->GetPlayer()->remotehost;
  	            VLocation *vl = game->GetWorld ()->GetVLocation ( remotehost );
	            UplinkAssert (vl);
	            Computer *comp = vl->GetComputer ();
	            UplinkAssert (comp);

                if ( comp->TYPE == COMPUTER_TYPE_LAN )
                {

                    LanComputer *lc = (LanComputer *) comp;
					if ( lc->systems.ValidIndex ( currentSystem ) ) {
						LanComputerSystem *system = lc->systems.GetData(currentSystem);
						game->GetInterface()->GetRemoteInterface()->RunScreen ( system->screenIndex, lc );

						game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_NONE );
						game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_LAN );
					}

                }

            }

			create_msgbox( "Success", "Isolation Bridge DISABLED" );

		}

	}


    //
    // Awaken the sys admin
    //LanMonitor::SysAdminAwaken();

}

void ScriptLibrary::Script85 ()
{

    int NUMCAPTIONS = 14;

    char *TEXT [] = {   "Not Connected",
                        "Aligning radio transmitter",
                        "Complete",
                        "Starting remote connect services",
                        "Powering up transmitter",
                        "Verifying transmitter power output",
                        "Verified",
                        "Preparing ping message",
                        "Transmitting ping signal",
                        "Sent",
                        "Listening for echo reply",
                        "Listening for echo reply .",
                        "Listening for echo reply ..",
                        "Listening for echo reply ...",     };

    int TIME [] =   {   0,
                        700,
                        200,
                        100,
                        100,
                        100,
                        100,
                        100,
                        500,
                        200,
                        250,
                        250,
                        250,
                        250         };


    Button *status = EclGetButton ( "radioscreen_statustext" );

    if ( status ) {

        //
        // Which caption is currently up?

        int captionIndex = -1;
        for ( int i = 0; i < NUMCAPTIONS; ++i ) {
            if ( strcmp ( status->caption, TEXT[i] ) == 0 ) {
                captionIndex = i;
                break;
            }
        }

        if ( captionIndex == -1 ) return;               // Something went wrong

        //
        // Show the next caption

        if ( captionIndex == NUMCAPTIONS - 1 ) {

            RadioTransmitterScreenInterface *rsi = (RadioTransmitterScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
            rsi->Connect ();

        }
        else {

            status->SetCaption ( TEXT[captionIndex+1] );
            GciTimerFunc ( TIME[captionIndex+1], RunScript, 85 );

        }

    }

}

void ScriptLibrary::Script90 ()
{

	// Called from login_interface.cpp to create a new game from the login screen

	game->NewGame ();
	app->GetMainMenu ()->Remove ();

	
}

void ScriptLibrary::Script91 ()
{

	// Called from login_interface.cpp to load a game from the login screen

	app->LoadGame ( );

}

void ScriptLibrary::Script92 ()
{

	Button *key = EclGetButton ( "start_key" );
	Button *lock = EclGetButton ( "start_lock" );
	UplinkAssert (key);
	UplinkAssert (lock);
	
	EclRegisterButton ( key->x + key->width/2 + 8, key->y + key->height/2 + 8, lock->x - key->x - 16, lock->x - key->x - 16, " ", " ", "start_link" );
	EclRegisterButtonCallbacks ( "start_link", DrawConnection, NULL, NULL, NULL );

	EclRegisterCaptionChange ( "firsttimeloading_text", "Awaiting connection acknowledgment from UPLINK...", 800 );

    SgPlaySound ( RsArchiveFileOpen ( "sounds/ringout.wav" ), "sounds/ringout.wav", false );

	GciTimerFunc ( 900, RunScript, 32 );

}

void ScriptLibrary::Script93 ()
{

	Button *key = EclGetButton ( "start_key" );
	Button *lock = EclGetButton ( "start_lock" );
	UplinkAssert (key);
	UplinkAssert (lock);
	
	EclRegisterButton ( key->x + key->width/2 + 8, key->y + key->height/2 + 8, lock->x - key->x - 16, lock->x - key->x - 16, " ", " ", "start_link" );
	EclRegisterButtonCallbacks ( "start_link", DrawConnection, NULL, NULL, NULL );

	EclRegisterCaptionChange ( "connecting 0 0", "Awaiting connection acknowledgment from GATEWAY...", 700 );
	GciTimerFunc ( 3500, RunScript, 36 );

}
