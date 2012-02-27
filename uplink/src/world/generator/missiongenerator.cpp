// MissionGenerator.cpp: implementation of the MissionGenerator class.
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>
//#include <vfw.h>
#endif

#include <strstream>

#include "soundgarden.h"
#include "redshirt.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/probability.h"
#include "app/serialise.h"
#include "app/miscutils.h"

#include "game/game.h"
#include "game/scriptlibrary.h"
#include "game/data/data.h"

#include "world/world.h"
#include "world/person.h"
#include "world/player.h"
#include "world/message.h"
#include "world/computer/recordbank.h"
#include "world/computer/computer.h"
#include "world/computer/bankcomputer.h"
#include "world/computer/databank.h"
#include "world/company/company.h"
#include "world/company/companyuplink.h"
#include "world/company/mission.h"
#include "world/company/news.h"

#include "world/generator/worldgenerator.h"
#include "world/generator/missiongenerator.h"
#include "world/generator/namegenerator.h"
#include "world/generator/numbergenerator.h"
#include "world/generator/consequencegenerator.h"

#include "world/scheduler/notificationevent.h"

#include "mmgr.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


Probability *MissionGenerator::prob_missiontype [NUM_UPLINKRATINGS];



void MissionGenerator::Initialise()
{

	// Assign all probabilities here
/*
	prob_missiontype.InputProbability (		MISSION_SPECIAL,			PROB_MISSION_SPECIAL			);
	prob_missiontype.InputProbability (		MISSION_STEALFILE,			PROB_MISSION_STEALFILE			);
	prob_missiontype.InputProbability (		MISSION_DESTROYFILE,		PROB_MISSION_DESTROYFILE		);
	prob_missiontype.InputProbability (		MISSION_FINDDATA,			PROB_MISSION_FINDDATA			);
	prob_missiontype.InputProbability (		MISSION_CHANGEDATA,			PROB_MISSION_CHANGEDATA			);
	prob_missiontype.InputProbability (		MISSION_FRAMEUSER,			PROB_MISSION_FRAMEUSER			);
	prob_missiontype.InputProbability (		MISSION_TRACEUSER,			PROB_MISSION_TRACEUSER			);
	prob_missiontype.InputProbability (		MISSION_CHANGEACCOUNT,		PROB_MISSION_CHANGEACCOUNT		);
	prob_missiontype.InputProbability (		MISSION_REMOVECOMPUTER,		PROB_MISSION_REMOVECOMPUTER		);
	prob_missiontype.InputProbability (		MISSION_REMOVECOMPANY,		PROB_MISSION_REMOVECOMPANY		);
	prob_missiontype.InputProbability (		MISSION_REMOVEUSER,			PROB_MISSION_REMOVEUSER			);

	UplinkAssert ( prob_missiontype.Validate () );
*/

    for ( int i = 0; i < NUM_UPLINKRATINGS; ++i ) {

        prob_missiontype [i] = new Probability ();

	    prob_missiontype [i]->InputProbability (		MISSION_SPECIAL,			PROB_MISSION_SPECIAL [i]		);
	    prob_missiontype [i]->InputProbability (		MISSION_STEALFILE,			PROB_MISSION_STEALFILE [i]		);
	    prob_missiontype [i]->InputProbability (		MISSION_DESTROYFILE,		PROB_MISSION_DESTROYFILE [i]	);
	    prob_missiontype [i]->InputProbability (		MISSION_FINDDATA,			PROB_MISSION_FINDDATA [i]		);
	    prob_missiontype [i]->InputProbability (		MISSION_CHANGEDATA,			PROB_MISSION_CHANGEDATA	[i] 	);
	    prob_missiontype [i]->InputProbability (		MISSION_FRAMEUSER,			PROB_MISSION_FRAMEUSER [i]		);
	    prob_missiontype [i]->InputProbability (		MISSION_TRACEUSER,			PROB_MISSION_TRACEUSER [i]		);
	    prob_missiontype [i]->InputProbability (		MISSION_CHANGEACCOUNT,		PROB_MISSION_CHANGEACCOUNT [i]	);
	    prob_missiontype [i]->InputProbability (		MISSION_REMOVECOMPUTER,		PROB_MISSION_REMOVECOMPUTER	[i]	);
	    prob_missiontype [i]->InputProbability (		MISSION_REMOVECOMPANY,		PROB_MISSION_REMOVECOMPANY [i]	);
	    prob_missiontype [i]->InputProbability (		MISSION_REMOVEUSER,			PROB_MISSION_REMOVEUSER	[i]		);

	    UplinkAssert ( prob_missiontype [i]->Validate () );

    }

}

void MissionGenerator::Shutdown ()
{

    for ( int i = 0; i < NUM_UPLINKRATINGS; ++i ) {

        Probability *p = prob_missiontype [i];
        if ( p ) delete p;

    }

}

Mission *MissionGenerator::GenerateMission ()
{

    int playerRating = game->GetWorld ()->GetPlayer ()->rating.uplinkrating;
    UplinkAssert ( playerRating >= 0 );
    UplinkAssert ( playerRating < NUM_UPLINKRATINGS );

	return GenerateMission ( prob_missiontype [playerRating]->GetValue () );

}

Mission *MissionGenerator::GenerateMission ( int type )
{

	Company *employer = WorldGenerator::GetRandomCompany ();

	return GenerateMission ( type, employer );

}

Mission *MissionGenerator::GenerateMission ( int type, Company *employer )
{

	Computer *source = NULL;
	Computer *target = NULL;

	switch ( type ) {

		case MISSION_STEALFILE		:

			return Generate_StealFile ( employer );
			break;

		case MISSION_DESTROYFILE    :

			return Generate_DestroyFile ( employer );
			break;

		case MISSION_FINDDATA		:

			return Generate_FindData ( employer );
			break;

		case MISSION_CHANGEDATA		:

			return Generate_ChangeData ( employer );
			break;

#ifndef DEMOGAME

		case MISSION_CHANGEACCOUNT  :

			source = WorldGenerator::GetRandomComputer ( COMPUTER_TYPE_PUBLICBANKSERVER );
			target = WorldGenerator::GetRandomComputer ( COMPUTER_TYPE_PUBLICBANKSERVER );
			return Generate_ChangeAccount ( employer, source, target );
			break;

		case MISSION_FRAMEUSER		:

			return Generate_FrameUser ( employer );
			break;

		case MISSION_TRACEUSER		:

			return Generate_TraceUser ( employer );
			break;

		case MISSION_REMOVECOMPUTER :

			target = WorldGenerator::GetRandomComputer ( COMPUTER_TYPE_CENTRALMAINFRAME );
			return Generate_RemoveComputer ( employer, target );
			break;

		case MISSION_REMOVEUSER		:

			return Generate_RemoveUser ( employer );
			break;

#endif

		default:

			return NULL;
			break;

	};

}

Mission *MissionGenerator::Generate_StealFile ( Company *employer )
{

	UplinkAssert (employer);

	int type = NumberGenerator::RandomNumber ( 2 ) + 1;

	Mission *m = NULL;

	switch ( type ) {

		case 1 :
		{
			Computer *target = WorldGenerator::GetRandomLowSecurityComputer ( COMPUTER_TYPE_INTERNALSERVICESMACHINE );
			UplinkAssert (target);
			if ( strcmp ( employer->name, target->companyname ) == 0 ) return NULL;
			m = Generate_StealSingleFile ( employer, target );
			break;
		}

		case 2 :
		{
			Computer *target = WorldGenerator::GetRandomComputer ( COMPUTER_TYPE_INTERNALSERVICESMACHINE |
                                                                   COMPUTER_TYPE_CENTRALMAINFRAME |
                                                                   COMPUTER_TYPE_LAN );
			UplinkAssert (target);
			if ( strcmp ( employer->name, target->companyname ) == 0 ) return NULL;
			m = Generate_StealAllFiles   ( employer, target );
			break;
		}

	};

	return m;

}

Mission *MissionGenerator::Generate_StealSingleFile ( Company *employer, Computer *target )
{

	UplinkAssert (employer);
	UplinkAssert (target);


	if ( strcmp ( employer->name, target->companyname ) == 0 ) return NULL;

	int difficulty = (int) NumberGenerator::RandomNormalNumber ( MINDIFFICULTY_MISSION_STEALFILE, DIFFICULTY_MISSION_VARIANCE );
	if ( difficulty < MINDIFFICULTY_MISSION_STEALFILE ) difficulty = MINDIFFICULTY_MISSION_STEALFILE;

	// Set up the basic variables of the mission

	int payment			= (int) NumberGenerator::RandomNormalNumber ( (float) ( difficulty * PAYMENT_MISSION_STEALFILE ),
	                                                                  (float) ( difficulty * PAYMENT_MISSION_STEALFILE
	                                                                                       * PAYMENT_MISSION_VARIANCE ) );
	int minrating		= difficulty;

	int acceptrating	= minrating + NumberGenerator::RandomNumber ( 2 );
	int datasize		= (int) ( NumberGenerator::RandomNormalNumber ( (float) difficulty, difficulty / 3.0f ) + 1 );

	if ( minrating > 10 ) minrating = 10;
	if ( acceptrating > 10 ) acceptrating = 10;

	payment = int ( payment / 100 ) * 100;				// Rounds payment to the nearest 100


	// Create the data to be stolen and add it to the target computer's memory

	char *datatitle = NameGenerator::GenerateDataName ( target->name, DATATYPE_DATA );

	int encrypted = difficulty < 3 ? 0 : (int) NumberGenerator::RandomNormalNumber ( (float) difficulty, (float) ( difficulty / 2 ) );
    if ( encrypted > 7 ) encrypted = 7;

	int compressed = 0;

	Data *data = new Data ();
	data->SetTitle ( datatitle );
	data->SetDetails ( DATATYPE_DATA, datasize, encrypted, compressed );

	if ( target->databank.FindValidPlacement ( data ) == -1 )
		target->databank.SetSize ( target->databank.GetSize () + data->size + 2 );

    target->databank.InsertData ( data );
	//target->databank.PutData ( data );

	if ( strcmp ( game->GetWorld ()->GetPlayer ()->GetRemoteHost ()->ip, target->ip ) != 0 )
		target->databank.RandomizeDataPlacement ();

	// Infer the internal services contact address

	char personname [SIZE_PERSON_NAME];
	UplinkSnprintf ( personname, sizeof ( personname ), "internal@%s.net", employer->name );


	// Generate the fields of the mission

	char description [SIZE_MISSION_DESCRIPTION];
	std::ostrstream details;
	std::ostrstream fulldetails;

	switch ( NumberGenerator::RandomNumber ( 3 ) + 1 ) {

		case 1 :		UplinkStrncpy ( description,  "Steal important data from a rival company's file server", sizeof ( description ) );		break;
		case 2 :		UplinkStrncpy ( description,  "Access a remote system and copy important data files", sizeof ( description ) );			break;
		case 3 :		UplinkStrncpy ( description,  "Gain access to a rival system and steal research files", sizeof ( description ) );			break;

	}

	details << "Payment for this job is " << payment << " credits.\n"
			<< "You will require " << datasize << " GigaQuads of storage space.\n"
			<< "This job has been assigned an Uplink difficulty of " << difficulty << ".\n"
			<< "An UplinkRating of " << Rating::GetUplinkRatingString ( acceptrating ) << " or above will be sufficient for automatic acceptance.\n\n"
			<< '\x0';


	fulldetails << "Thankyou for working for " << employer->name << ".\n"
				<< "\n"
				<< "TARGET COMPUTER DATA :\n"
				<< "   LOCATION: " << target->name << "\n"
				<< "   IP      : " << target->ip << "\n"
				<< "   FILENAME: " << datatitle << "\n"
				<< "   FILESIZE: " << datasize << " GigaQuads\n"
				<< "\n";

	if ( encrypted > 0 && encrypted < 5 )	fulldetails << "\nThe file will be encrypted.\n";
	else if ( encrypted >= 5 )				fulldetails << "\nThe file will be heavily encrypted.\n";

	if ( compressed > 0 && compressed < 5 )	fulldetails << "\nThe file will be compressed.\n";
	else if ( compressed >= 5 )				fulldetails << "\nThe file will be heavily compressed.\n";


	fulldetails	<< "\nReturn the file via email to\n"
				<< personname << "\n"
				<< "\n"
				<< "END"
				<< '\x0';

	Date postdate;
	postdate.SetDate ( &(game->GetWorld ()->date) );
	postdate.AdvanceHour ( NumberGenerator::RandomNumber ( 96 ) * -1 );
	postdate.AdvanceMinute ( NumberGenerator::RandomNumber ( 60 ) * -1 );

	// Insert the mission
	Mission *mission = new Mission ();
	mission->SetTYPE		 ( MISSION_STEALFILE );
	mission->SetCompletion   ( target->ip, datatitle, NULL, NULL, NULL );
	mission->SetEmployer     ( employer->name );
	mission->SetContact      ( personname );
	mission->SetPayment      ( payment, (int) ( payment * 1.1 ) );
	mission->SetDifficulty   ( difficulty );
	mission->SetMinRating    ( minrating );
	mission->SetAcceptRating ( acceptrating );
	mission->SetDescription  ( description );
	mission->SetDetails		 ( details.str () );
	mission->SetFullDetails  ( fulldetails.str () );
	if ( difficulty > 3 )		mission->SetWhySoMuchMoney ( "The files contain sensitive data." );
	if ( !game->IsRunning () )	mission->SetCreateDate   ( &postdate );
	mission->GiveLink ( target->ip );

	details.rdbuf()->freeze( 0 );
	fulldetails.rdbuf()->freeze( 0 );
	//delete [] details.str ();
	//delete [] fulldetails.str ();

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );
	cu->CreateMission ( mission );

	return mission;

}

Mission *MissionGenerator::Generate_StealAllFiles ( Company *employer, Computer *target )
{

	UplinkAssert (employer);
	UplinkAssert (target);

	int difficulty = (int) NumberGenerator::RandomNormalNumber ( MINDIFFICULTY_MISSION_STEALALLFILES, DIFFICULTY_MISSION_VARIANCE );
	if ( difficulty < MINDIFFICULTY_MISSION_STEALALLFILES ) difficulty = MINDIFFICULTY_MISSION_STEALALLFILES;

    if ( target->TYPE == COMPUTER_TYPE_CENTRALMAINFRAME )   difficulty += 1;
    if ( target->TYPE == COMPUTER_TYPE_LAN )                difficulty += 2;

	// Set up the basic variables of the mission

	int payment			= (int) NumberGenerator::RandomNormalNumber ( (float) ( difficulty * PAYMENT_MISSION_STEALALLFILES ),
	                                                                  (float) ( difficulty * PAYMENT_MISSION_STEALALLFILES
	                                                                                       * PAYMENT_MISSION_VARIANCE ) );

	int minrating		= difficulty;

	int acceptrating	= minrating + NumberGenerator::RandomNumber ( 2 );

	if ( minrating > 10 ) minrating = 10;
	if ( acceptrating > 10 ) acceptrating = 10;

	payment = int ( payment / 100 ) * 100;				// Rounds payment to the nearest 100


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

	int missiontype = NumberGenerator::RandomNumber ( 4 ) + 1;

	/*  ===================================================
		type 1		:		Steal all scientific research
		type 2		:		Steal all corporate data
		type 3		:		Steal customer records
		type 4		:		Steal software in development
		===================================================

		*/

	char *missiontypestring [4] = { "rsrch", "corp", "custmr", "softw" };

	//
	// Generate around 10 files to be stolen,
	// All fairly large
	//

	int numfiles = (int) NumberGenerator::RandomNormalNumber ( 10, 5 );
	int encrypted = (int) NumberGenerator::RandomNormalNumber ( (float) difficulty, (float) difficulty );
    if ( encrypted > 7 ) encrypted = 7;
	int totalsize = 0;

	for ( int i = 0; i < numfiles; ++i ) {

		char datatitle [64];
		UplinkSnprintf ( datatitle, sizeof ( datatitle ), "%c%c%c%c-%s %d.dat", target->companyname [0], target->companyname [1],
																	   target->companyname [2], target->companyname [3],
																		missiontypestring [missiontype-1], i );

		int size = (int) NumberGenerator::RandomNormalNumber ( 10, 5 );
		totalsize += size;

		Data *file = new Data ();
		file->SetTitle ( datatitle );
		file->SetDetails ( DATATYPE_DATA, size, encrypted, 0 );

		while ( target->databank.FindValidPlacement (file) == -1 )
			target->databank.SetSize ( target->databank.GetSize () + file->size );

		target->databank.PutData ( file );

	}

	if ( strcmp ( game->GetWorld ()->GetPlayer ()->GetRemoteHost ()->ip, target->ip ) != 0 )
		target->databank.RandomizeDataPlacement ();

	/********** Patch by François Gagné **********/
    //
    // Make sure there is enough space on the new computer to store the mission files

    if ( totalsize > ourcomp->databank.GetSize() ) {
        ourcomp->databank.SetSize( totalsize );
    }

    //
	// Generate the fields of the mission

	char description [SIZE_MISSION_DESCRIPTION];
	std::ostrstream details;
	std::ostrstream fulldetails;


	details << "Payment for this job is " << payment << " credits.\n"
			<< "This job has been assigned an Uplink difficulty of " << difficulty << ".\n"
			<< "An UplinkRating of " << Rating::GetUplinkRatingString ( acceptrating )
			<< " or above will be sufficient for automatic acceptance.\n"
			<< "You will need around " << 10 * int (totalsize / 10) << " gigaquads of space.";

    if ( target->TYPE == COMPUTER_TYPE_LAN )
        details << "\nThe data is stored on a LAN.\n";

    details << '\x0';


	char completionA [SIZE_VLOCATION_IP];								// IP
	char completionB [SIZE_DATA_TITLE];									// Data title or ALL
	char completionC [SIZE_VLOCATION_IP];								// IP of our dump machine
	char completionD [8];												// Numfiles TotalSize
	char completionE [16];												// TYPE of data

	char whoisthetarget [128];
	UplinkSnprintf ( whoisthetarget, sizeof ( whoisthetarget ), "The data is owned by %s", target->companyname );

	UplinkStrncpy ( completionA, target->ip, sizeof ( completionA ) );
	UplinkStrncpy ( completionB, "ALL", sizeof ( completionB ) );
	UplinkStrncpy ( completionC, ourcomp->ip, sizeof ( completionC ) );
	UplinkSnprintf ( completionD, sizeof ( completionD ), "%d %d", numfiles, totalsize );

	if ( missiontype == 1 ) {

		UplinkStrncpy ( description, "Steal valuable scientific research documents", sizeof ( description ) );

		UplinkStrncpy ( completionE, "SCIENTIFIC", sizeof ( completionE ) );

		fulldetails << "Due to a lack of enthusiasm on the part of our recruitment department, "
					   "one of our market competitors has pulled several months ahead with "
					   "their research program.  Given our current situation, we have concluded "
					   "that the best way to close this gap is for us to steal the research data "
					   "they have gathered.  This will be your mission.\n\n"
					   "Their research files are stored on the system below, and will be "
					   "heavily protected.  You will need to copy the files, break the security "
					   "on them in any way you see fit, then copy the files onto our own fileserver.\n\n";

	}
	else if ( missiontype == 2 ) {

		UplinkStrncpy ( description, "Copy large and secure corporate database", sizeof ( description ) );

		fulldetails << "Our establishment is very anxious to find out more about the inner workings "
					   "of one of our rivals, and we have determined after several weeks of investigation "
					   "that their primary corporate database is stored at the location shown below.  We "
					   "want you to enter this system and steal all of the data files.\n\n"
					   "These files will be well protected and encrypted - you will need to break this "
					   "security, before copying the stolen database to our own fileserver, location below.\n\n";

		UplinkStrncpy ( completionE, "CORPORATE", sizeof ( completionE ) );

	}
	else if ( missiontype == 3 ) {

		UplinkStrncpy ( completionE, "CUSTOMER", sizeof ( completionE ) );

		UplinkStrncpy ( description, "Break into High Security System and steal customer records", sizeof ( description ) );

		fulldetails << "It has come to our attention that one of our competitors has amassed an extensive "
					   "customer database which would prove to be of great help to our market research team.  "
					   "We want you to compromise the security of the system below, copy all of the customer data "
					   "and decrypt it.  Then copy the files onto our own temporary file server.\n\n";

	}
	else if ( missiontype == 4 ) {

		UplinkStrncpy ( completionE, "SOFTWARE", sizeof ( completionE ) );

		UplinkStrncpy ( description, "Copy proprietary source code database", sizeof ( description ) );

		fulldetails << "As you may be aware we are currently in direct competition with another rival corporation "
					   "to develop a software product which will no doubt dominate the market once released.  "
					   "Unfortunately, due to circumstances beyond our control, they have managed to pull several months "
					   "ahead.  We believe the best course of action is for us to copy their program source code and use "
					   "it to help our product provide adequate competition.\n"
					   "Break into the computer system below, copy anything that looks like source code and break any "
					   "security systems in place on the data files.  Then copy those files onto our own file server.\n\n";

	}

	fulldetails << "Target computer : " << target->name << "\n"
				<< "IP : " << target->ip << "\n\n"
				<< "Once complete, dump the files you have to this computer:\n"
				<< ourcomp->name << "\n"
				<< "IP : " << ourcomp->ip << "\n\n"
				<< "USERNAME : " << username << "\n"
				<< "PASSWORD : " << password << "\n\n"
				<< "END"
				<< '\x0';


	Date postdate;
	postdate.SetDate ( &(game->GetWorld ()->date) );
	postdate.AdvanceHour ( NumberGenerator::RandomNumber ( 96 ) * -1 );
	postdate.AdvanceMinute ( NumberGenerator::RandomNumber ( 60 ) * -1 );

	// Insert the mission
	Mission *mission = new Mission ();
	mission->SetTYPE		 ( MISSION_STEALFILE );
	mission->SetCompletion   ( completionA, completionB, completionC, completionD, completionE );
	mission->SetEmployer     ( employer->name );
	mission->SetContact      ( personname );
	mission->SetPayment      ( payment, (int) ( payment * 1.1 ) );
	mission->SetDifficulty   ( difficulty );
	mission->SetMinRating    ( minrating );
	mission->SetAcceptRating ( acceptrating );
	mission->SetDescription  ( description );
	mission->SetDetails		 ( details.str () );
	mission->SetFullDetails  ( fulldetails.str () );
	mission->SetWhySoMuchMoney ( "There will be a lot of data to copy" );
	if ( !game->IsRunning () )	mission->SetCreateDate   ( &postdate );
	mission->SetWhoIsTheTarget ( whoisthetarget );
	mission->GiveLink ( target->ip );
	mission->GiveLink ( ourcomp->ip );
	mission->GiveCode ( ourcomp->ip, code );

	details.rdbuf()->freeze( 0 );
	fulldetails.rdbuf()->freeze( 0 );
	//delete [] details.str ();
	//delete [] fulldetails.str ();

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );
	cu->CreateMission ( mission );

	return mission;

}

Mission *MissionGenerator::Generate_DestroyFile ( Company *employer )
{

	UplinkAssert (employer);

	int type = NumberGenerator::RandomNumber ( 2 ) + 1;

	Mission *m = NULL;

	switch ( type ) {

		case 1 :
		{
			Computer *target = WorldGenerator::GetRandomLowSecurityComputer ( COMPUTER_TYPE_INTERNALSERVICESMACHINE );
			UplinkAssert (target);
			if ( strcmp ( employer->name, target->companyname ) == 0 ) return NULL;
			m = Generate_DestroySingleFile ( employer, target );
			break;
		}

		case 2 :
		{
			Computer *target = WorldGenerator::GetRandomComputer ( COMPUTER_TYPE_INTERNALSERVICESMACHINE |
                                                                   COMPUTER_TYPE_CENTRALMAINFRAME |
                                                                   COMPUTER_TYPE_LAN );
			UplinkAssert (target);
			if ( strcmp ( employer->name, target->companyname ) == 0 ) return NULL;
			m = Generate_DestroyAllFiles ( employer, target );
			break;
		}

	};

	return m;

}

Mission *MissionGenerator::Generate_DestroySingleFile ( Company *employer, Computer *target )
{

	UplinkAssert (employer);
	UplinkAssert (target);

	int difficulty = (int) NumberGenerator::RandomNormalNumber ( MINDIFFICULTY_MISSION_DESTROYFILE, DIFFICULTY_MISSION_VARIANCE );
	if ( difficulty < MINDIFFICULTY_MISSION_DESTROYFILE ) difficulty = MINDIFFICULTY_MISSION_DESTROYFILE;

    if ( target->TYPE == COMPUTER_TYPE_CENTRALMAINFRAME ) ++difficulty;


	// Set up the basic variables of the mission

	int payment			= (int) NumberGenerator::RandomNormalNumber ( (float) ( difficulty * PAYMENT_MISSION_DESTROYFILE ),
	                                                                  (float) ( difficulty * PAYMENT_MISSION_DESTROYFILE
	                                                                                       * PAYMENT_MISSION_VARIANCE ) );

	int minrating		= difficulty;
	int acceptrating	= minrating + NumberGenerator::RandomNumber ( 2 );
	int datasize		= (int) NumberGenerator::RandomNormalNumber ( (float) difficulty, difficulty / 3.0f ) + 1;

	if ( minrating > 10 ) minrating = 10;
	if ( acceptrating > 10 ) acceptrating = 10;

	payment = int ( payment / 100 ) * 100;				// Rounds payment to the nearest 100


	// Create the data to be stolen and add it to the target computer's memory

	char *datatitle = NameGenerator::GenerateDataName ( target->name, DATATYPE_DATA );

	Data *data = new Data ();
	data->SetTitle ( datatitle );
	data->SetDetails ( DATATYPE_DATA, datasize, 0, 0 );

	if ( target->databank.FindValidPlacement ( data ) == -1 )
		target->databank.SetSize ( target->databank.GetSize () + data->size + 2 );

    target->databank.InsertData ( data );
	//target->databank.PutData ( data );

	if ( strcmp ( game->GetWorld ()->GetPlayer ()->GetRemoteHost ()->ip, target->ip ) != 0 )
		target->databank.RandomizeDataPlacement ();

	// Infer the internal services contact address

	char personname [SIZE_PERSON_NAME];
	UplinkSnprintf ( personname, sizeof ( personname ), "internal@%s.net", employer->name );


	// Generate the fields of the mission

	char description [SIZE_MISSION_DESCRIPTION];
	std::ostrstream details;
	std::ostrstream fulldetails;


	switch ( NumberGenerator::RandomNumber ( 3 ) + 1 ) {

		case 1 :		UplinkStrncpy ( description, "Find and destroy crucial data on a mainframe", sizeof ( description ) );				break;
		case 2 :		UplinkStrncpy ( description, "Break into a rival computer system and sabotage files", sizeof ( description ) );		break;
		case 3 :		UplinkStrncpy ( description, "Hack into a computer and delete key files", sizeof ( description ) );					break;

	}

	details << "Payment for this job is " << payment << " credits.\n"
			<< "This job has been assigned an Uplink difficulty of " << difficulty << ".\n"
			<< "An UplinkRating of " << Rating::GetUplinkRatingString ( acceptrating ) << " or above will be sufficient for automatic acceptance.\n\n"
			<< '\x0';


	fulldetails << "Thankyou for working for " << employer->name << ".\n"
				<< "\n"
				<< "TARGET COMPUTER DATA :\n"
				<< "   LOCATION: " << target->name << "\n"
				<< "   IP      : " << target->ip << "\n"
				<< "   FILENAME: " << datatitle << "\n"
				<< "   FILESIZE: " << datasize << " GigaQuads\n"
				<< "\n"
				<< "Destroy this data and all backups.\n"
				<< "Send a notice of completion to\n"
				<< personname << "\n"
				<< "\n"
				<< "END"
				<< '\x0';

	char completionA [SIZE_VLOCATION_IP];
	char completionB [SIZE_DATA_TITLE];
	UplinkStrncpy ( completionA, target->ip, sizeof ( completionA ) );
	UplinkStrncpy ( completionB, datatitle, sizeof ( completionB ) );

	Date postdate;
	postdate.SetDate ( &(game->GetWorld ()->date) );
	postdate.AdvanceHour ( NumberGenerator::RandomNumber ( 96 ) * -1 );
	postdate.AdvanceMinute ( NumberGenerator::RandomNumber ( 60 ) * -1 );

	// Insert the mission
	Mission *mission = new Mission ();
	mission->SetTYPE		 ( MISSION_DESTROYFILE );
	mission->SetCompletion   ( completionA, completionB, NULL, NULL, NULL );
	mission->SetEmployer     ( employer->name );
	mission->SetContact      ( personname );
	mission->SetPayment      ( payment, (int) ( payment * 1.1 ) );
	mission->SetDifficulty   ( difficulty );
	mission->SetMinRating    ( minrating );
	mission->SetAcceptRating ( acceptrating );
	mission->SetDescription  ( description );
	mission->SetDetails		 ( details.str () );
	mission->SetFullDetails  ( fulldetails.str () );
	if ( difficulty > 4 )		mission->SetWhySoMuchMoney ( "The data will be well protected." );
	if ( !game->IsRunning () )	mission->SetCreateDate   ( &postdate );
	mission->GiveLink ( target->ip );

	details.rdbuf()->freeze( 0 );
	fulldetails.rdbuf()->freeze( 0 );
	//delete [] details.str ();
	//delete [] fulldetails.str ();

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );
	cu->CreateMission ( mission );

	return mission;

}

Mission *MissionGenerator::Generate_DestroyAllFiles ( Company *employer, Computer *target )
{

	UplinkAssert (employer);
	UplinkAssert (target);

	int difficulty = (int) NumberGenerator::RandomNormalNumber ( MINDIFFICULTY_MISSION_DESTROYALLFILES, DIFFICULTY_MISSION_VARIANCE );
	if ( difficulty < MINDIFFICULTY_MISSION_DESTROYALLFILES ) difficulty = MINDIFFICULTY_MISSION_DESTROYALLFILES;

    if ( target->TYPE == COMPUTER_TYPE_CENTRALMAINFRAME )   difficulty += 1;
    if ( target->TYPE == COMPUTER_TYPE_LAN )                difficulty += 2;

	// Set up the basic variables of the mission

	int payment			= (int) NumberGenerator::RandomNormalNumber ( (float) ( difficulty * PAYMENT_MISSION_DESTROYALLFILES ),
	                                                                  (float) ( difficulty * PAYMENT_MISSION_DESTROYALLFILES
	                                                                                       * PAYMENT_MISSION_VARIANCE ) );

	int minrating		= difficulty;

	int acceptrating	= minrating + NumberGenerator::RandomNumber ( 2 );

	if ( minrating > 10 ) minrating = 10;
	if ( acceptrating > 10 ) acceptrating = 10;

	payment = int ( payment / 100 ) * 100;				// Rounds payment to the nearest 100


	// Infer the internal services contact address

	char personname [SIZE_PERSON_NAME];
	UplinkSnprintf ( personname, sizeof ( personname ), "internal@%s.net", employer->name );


	// Generate the fields of the mission

	char description [SIZE_MISSION_DESCRIPTION];
	std::ostrstream details;
	std::ostrstream fulldetails;


	details << "Payment for this job is " << payment << " credits.\n"
			<< "This job has been assigned an Uplink difficulty of " << difficulty << ".\n"
			<< "An UplinkRating of " << Rating::GetUplinkRatingString ( acceptrating ) << " or above will be sufficient for automatic acceptance.\n\n";

    if ( target->TYPE == COMPUTER_TYPE_LAN )
        details << "\nThe data is stored on a LAN.\n";

	details	<< '\x0';


	char completionA [SIZE_VLOCATION_IP];								// IP
	char completionB [SIZE_DATA_TITLE];									// Data title or ALL
	char completionC [16];												// Type of file
	UplinkStrncpy ( completionA, target->ip, sizeof ( completionA ) );
	UplinkStrncpy ( completionB, "ALL", sizeof ( completionB ) );

	char whoisthetarget [128];
	UplinkSnprintf ( whoisthetarget, sizeof ( whoisthetarget ), "The data is owned by %s", target->companyname );

	int type = NumberGenerator::RandomNumber ( 4 ) + 1;


	//
	// Generate around 10 files to be stolen,
	// All fairly large
	//

	char *missiontypestring [4] = { "rsrch", "corp", "custmr", "softw" };
	int numfiles = (int) NumberGenerator::RandomNormalNumber ( 10, 5 );
	int encrypted = (int) NumberGenerator::RandomNormalNumber ( (float) difficulty, (float) difficulty );
	int totalsize = 0;

	for ( int i = 0; i < numfiles; ++i ) {

		char datatitle [64];
		UplinkSnprintf ( datatitle, sizeof ( datatitle ), "%c%c%c%c-%s %d.dat", target->companyname [0], target->companyname [1],
																			   target->companyname [2], target->companyname [3],
																				missiontypestring [type-1], i );

		int size = (int) NumberGenerator::RandomNormalNumber ( 10, 5 );
		totalsize += size;

		Data *file = new Data ();
		file->SetTitle ( datatitle );
		file->SetDetails ( DATATYPE_DATA, size, encrypted, 0 );

		while ( target->databank.FindValidPlacement (file) == -1 )
			target->databank.SetSize ( target->databank.GetSize () + file->size );

		target->databank.PutData ( file );

	}

	if ( strcmp ( game->GetWorld ()->GetPlayer ()->GetRemoteHost ()->ip, target->ip ) != 0 )
		target->databank.RandomizeDataPlacement ();

	/*
		type 1		:		Destroy all scientific research
		type 2		:		Destroy all corporate data
		type 3		:		Destroy customer records
		type 4		:		Destroy software in development

		*/

	if ( type == 1 ) {

		UplinkStrncpy ( description, "Destroy scientific research currently in progress", sizeof ( description ) );

		fulldetails << "One of our competitors has unfortunately gained a lead "
					   "and has nearly completed some scientific research which will "
					   "be damaging to our corporate agenda.\n"
					   "Break into the computer system below and destroy ALL files "
					   "you find on their file server.  You must also ensure all backups "
					   "are totally wiped out.\n";

		UplinkStrncpy ( completionC, "SCIENTIFIC", sizeof ( completionC ) );

	}
	else if ( type == 2 ) {

		UplinkStrncpy ( description, "Destroy corporate data of a rival company", sizeof ( description ) );

		fulldetails << "We are becoming tired of the competition provided by a "
					   "rival firm and we have decided to put an end to their activities.\n"
					   "Break into their main computer system (ip below) and destroy ALL files "
					   "on their system.\n";

		UplinkStrncpy ( completionC, "CORPORATE", sizeof ( completionC ) );

	}
	else if ( type == 3 ) {

		UplinkStrncpy ( description, "Destroy customer records on a sales system", sizeof ( description ) );

		fulldetails << "We are in direct competition with another rival sales system and "
					   "wish to bring our conflict to a quick end.  Break into their primary "
					   "sales system shown below and delete all files on their computer.\n";

		UplinkStrncpy ( completionC, "CUSTOMER", sizeof ( completionC ) );

	}
	else if ( type == 4 ) {

		UplinkStrncpy ( description, "Destroy rival software in development", sizeof ( description ) );

		fulldetails << "As you may be aware we are trying to promote our new software system "
					   "but another corporation has developed a rival tool which we feel may "
					   "harm our sales.  Gain access to their main development server (below) "
					   "and destroy ALL files you find.  You must also ensure all backups are "
					   "totally destroyed.\n";

		UplinkStrncpy ( completionC, "SOFTWARE", sizeof ( completionC ) );

	}

	fulldetails <<	"It would be in your interests to remove any traces of your actions, "
					"as the company will not be pleased when they discover what you have done."
					"\n\n"
					"TARGET COMPUTER : \n"
				 << "LOCATION: " << target->name << "\n"
				 << "IP      : " << target->ip << "\n"
				 << "\n"
				 << "Send a notice of completion to\n"
				 << personname << "\n"
				 << "\n"
				 << "END"
				 << '\x0';


	Date postdate;
	postdate.SetDate ( &(game->GetWorld ()->date) );
	postdate.AdvanceHour ( NumberGenerator::RandomNumber ( 96 ) * -1 );
	postdate.AdvanceMinute ( NumberGenerator::RandomNumber ( 60 ) * -1 );

	// Insert the mission
	Mission *mission = new Mission ();
	mission->SetTYPE		 ( MISSION_DESTROYFILE );
	mission->SetCompletion   ( completionA, completionB, completionC, NULL, NULL );
	mission->SetEmployer     ( employer->name );
	mission->SetContact      ( personname );
	mission->SetPayment      ( payment, (int) ( payment * 1.1 ) );
	mission->SetDifficulty   ( difficulty );
	mission->SetMinRating    ( minrating );
	mission->SetAcceptRating ( acceptrating );
	mission->SetDescription  ( description );
	mission->SetDetails		 ( details.str () );
	mission->SetFullDetails  ( fulldetails.str () );
	mission->SetWhySoMuchMoney ( "The data will be well protected." );
	if ( !game->IsRunning () )	mission->SetCreateDate   ( &postdate );
	mission->SetWhoIsTheTarget ( whoisthetarget );
	mission->GiveLink ( target->ip );

	details.rdbuf()->freeze( 0 );
	fulldetails.rdbuf()->freeze( 0 );
	//delete [] details.str ();
	//delete [] fulldetails.str ();

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );
	cu->CreateMission ( mission );

	return mission;

}

Mission *MissionGenerator::Generate_FindData ( Company *employer )
{

	int missiontype = 0;

	Mission *mission = NULL;
	Computer *target = NULL;

	switch ( missiontype ) {

		case 0  :		target = WorldGenerator::GetRandomComputer ( COMPUTER_TYPE_PUBLICBANKSERVER );
						mission = Generate_FindData_FinancialRecord ( employer, target );
						break;

	}

	return mission;

}

Mission *MissionGenerator::Generate_FindData_FinancialRecord ( Company *employer, Computer *target )
{

	UplinkAssert ( employer );
	UplinkAssert ( target );
	UplinkAssert ( target->GetOBJECTID () == OID_BANKCOMPUTER );

	int difficulty = (int) NumberGenerator::RandomNormalNumber ( MINDIFFICULTY_MISSION_FINDDATA_FINANCIAL, DIFFICULTY_MISSION_VARIANCE );
	if ( difficulty < MINDIFFICULTY_MISSION_FINDDATA_FINANCIAL ) difficulty = MINDIFFICULTY_MISSION_FINDDATA_FINANCIAL;

	//
	// Set up the basic variables of the mission
	//

	int payment			= (int) NumberGenerator::RandomNormalNumber ( (float) ( difficulty * PAYMENT_MISSION_FINDDATA_FINANCIAL ),
	                                                                  (float) ( difficulty * PAYMENT_MISSION_FINDDATA_FINANCIAL
	                                                                                       * PAYMENT_MISSION_VARIANCE ) );

	int minrating		= difficulty;
	int acceptrating	= minrating + NumberGenerator::RandomNumber ( 2 );

	if ( minrating > 10 ) minrating = 10;
	if ( acceptrating > 10 ) acceptrating = 10;

	payment = int ( payment / 100 ) * 100;				// Rounds payment to the nearest 100


	char personname [SIZE_PERSON_NAME];					// Person to send completion email to
	UplinkSnprintf ( personname, sizeof ( personname ), "internal@%s.net", employer->name );

	//
	// Get the account of a person at this bank computer
	//

	BankAccount *taccount = ((BankComputer *) target)->GetRandomAccount ();
	UplinkAssert ( taccount );

	// Make sure we don't target the player's Uplink bank account

	if ( strcmp ( taccount->name, game->GetWorld ()->GetPlayer ()->handle ) == 0 ) return NULL;

	//
	// Fill in the fields of the mission
	//

	char completionA [128];							// bankIP accountNum
    char completionB [32];                          // Field required
    char completionC [128];                         // Name of person responsible (only type3)

    UplinkSnprintf ( completionA, sizeof ( completionA ), "%s %d", target->ip, taccount->accountnumber );
    UplinkStrncpy ( completionC, "Unused", sizeof ( completionC ) );

	char description [SIZE_MISSION_DESCRIPTION];
	std::ostrstream details;
	std::ostrstream fulldetails;

	details << "Payment for this job is " << payment << " credits.\n"
			<< "This job has been assigned an Uplink difficulty of " << difficulty << ".\n"
			<< "An UplinkRating of " << Rating::GetUplinkRatingString ( acceptrating ) << " or above will be sufficient for automatic acceptance.\n\n"
			<< '\x0';

	fulldetails << "Break into the following well known financial institute:\n"
				<< "TARGET COMPUTER DATA :\n"
				<< "   LOCATION: " << target->name << "\n"
				<< "   IP      : " << target->ip << "\n"
				<< "\n\n";

	int missiontype = NumberGenerator::RandomNumber ( 3 ) + 1;

		/* =========================================================================
				type 1		:		Find bank balance
				type 2		:		Find size of loan
				type 3		:		Find receiver of recent payment
		   =========================================================================	*/

	if ( missiontype == 1 ) {

		UplinkStrncpy ( description, "Find financial details of one of our associates", sizeof ( description ) );

		fulldetails << "We are interested in this man's bank balance.\n"
					<< "Open his account and send his balance to us.\n"
					<< "NAME : " << taccount->name << "\n"
					<< "ACCNO: " << taccount->accountnumber << "\n";

//		char *balance = NULL;
//		balance = rec->GetField ( "Balance" );
//		if ( !balance ) return NULL;

		UplinkStrncpy ( completionB, "Balance", sizeof ( completionB ) );

	}
	else if ( missiontype == 2 ) {

		UplinkStrncpy ( description, "Client interested in financial data on an enemy", sizeof ( description ) );

		fulldetails << "We are interested in this man's financial situation.\n"
					<< "Open his account and determine the size of his loan. Send this information to us.\n"
					<< "NAME : " << taccount->name << "\n"
					<< "ACCNO: " << taccount->accountnumber << "\n";

//		char *loan = NULL;
//		loan = rec->GetField ( "Loan" );
//		if ( !loan ) return NULL;

		UplinkStrncpy ( completionB, "Loan", sizeof ( completionB ) );

	}
	else if ( missiontype == 3 ) {

		// Perform the transfer
		BankComputer *receiver = (BankComputer *) WorldGenerator::GetRandomComputer ( COMPUTER_TYPE_PUBLICBANKSERVER );
		UplinkAssert ( receiver );
		BankAccount *raccount = receiver->GetRandomAccount ();
		UplinkAssert ( raccount );

		if ( raccount == taccount )		return NULL;				// That would be silly ;)

		char taccountno [32];
		char raccountno [32];
		UplinkSnprintf ( taccountno, sizeof ( taccountno ), "%d", taccount->accountnumber );
		UplinkSnprintf ( raccountno, sizeof ( raccountno ), "%d", raccount->accountnumber );

		if ( game->GetWorld ()->GetPlayer ()->IsPlayerAccount (receiver->ip, raccountno) ) return NULL;
		if ( game->GetWorld ()->GetPlayer ()->IsPlayerAccount (target->ip, taccountno) ) return NULL;

		int amount = (int) NumberGenerator::RandomNormalNumber ( 1000000, 700000 );
		amount = 100000 * ( amount / 100000 );
        taccount->balance += amount;

		Person *transferer = game->GetWorld ()->GetPerson ( taccount->name );
		UplinkAssert (transferer);

		bool success = taccount->TransferMoney ( target->ip, taccountno, receiver->ip, raccountno, amount, transferer );

		UplinkStrncpy ( description, "Trace a recent balance transfer", sizeof ( description ) );

		fulldetails << "This individual is the subject of an internal investigation into corruption:\n"
					<< "NAME : " << taccount->name << "\n"
					<< "ACCNO: " << taccount->accountnumber << "\n"
					<< "He recently transfered a sizable amount of money to another account."
					<< "Trace this account and find the name of the receiver of this money.";

        UplinkStrncpy ( completionB, "TraceTransfer", sizeof ( completionB ) );
		UplinkStrncpy ( completionC, raccount->name, sizeof ( completionC ) );

	}
	else {

		UplinkAbort ( "Unrecognised mission type" );

	}

	//
	// Generate the remaining fields of the mission
	//

	fulldetails	<< "\nSend a notice of completion to\n"
				<< personname << "\n"
				<< "\n"
				<< "END"
				<< '\x0';

	char whoisthetarget [128];
	UplinkStrncpy ( whoisthetarget, "Nobody you would know.", sizeof ( whoisthetarget ) );

	Date postdate;
	postdate.SetDate ( &(game->GetWorld ()->date) );
	postdate.AdvanceHour ( NumberGenerator::RandomNumber ( 96 ) * -1 );
	postdate.AdvanceMinute ( NumberGenerator::RandomNumber ( 60 ) * -1 );

	//
	// Insert the mission
	//

	Mission *mission = new Mission ();
	mission->SetTYPE		 ( MISSION_FINDDATA );
	mission->SetCompletion   ( completionA, completionB, completionC, NULL, NULL );
	mission->SetEmployer     ( employer->name );
	mission->SetContact      ( personname );
	mission->SetPayment      ( payment, (int) ( payment * 1.1 ) );
	mission->SetDifficulty   ( difficulty );
	mission->SetMinRating    ( minrating );
	mission->SetAcceptRating ( acceptrating );
	mission->SetDescription  ( description );
	mission->SetDetails		 ( details.str () );
	mission->SetFullDetails  ( fulldetails.str () );
	mission->SetWhySoMuchMoney ( "The data is highly personal, and well protected" );
	if ( !game->IsRunning () ) mission->SetCreateDate   ( &postdate );
	mission->SetWhoIsTheTarget ( whoisthetarget );
	mission->GiveLink ( target->ip );

	details.rdbuf()->freeze( 0 );
	fulldetails.rdbuf()->freeze( 0 );
	//delete [] details.str ();
	//delete [] fulldetails.str ();

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );
	cu->CreateMission ( mission );

	return mission;

}

Mission *MissionGenerator::Generate_ChangeData ( Company *employer )
{

	// Decide which type of changedata mission to make this

	int missiontype = NumberGenerator::RandomNumber ( 3 );

	Mission *mission = NULL;

	switch ( missiontype ) {
		case 0	:		mission = Generate_ChangeData_AcademicRecord ( employer );		break;
		case 1  :		mission = Generate_ChangeData_SocialSecurity ( employer );		break;
		case 2  :		mission = Generate_ChangeData_CriminalRecord ( employer );		break;
	}

	return mission;

}

Mission *MissionGenerator::Generate_ChangeData_AcademicRecord ( Company *employer )
{

	UplinkAssert ( employer );

	int difficulty = (int) NumberGenerator::RandomNormalNumber ( MINDIFFICULTY_MISSION_CHANGEDATA_ACADEMIC, DIFFICULTY_MISSION_VARIANCE );
	if ( difficulty < MINDIFFICULTY_MISSION_CHANGEDATA_ACADEMIC ) difficulty = MINDIFFICULTY_MISSION_CHANGEDATA_ACADEMIC;

	Computer *target = game->GetWorld ()->GetComputer ( "International Academic Database" );
	UplinkAssert (target);

	//
	// Set up the basic variables of the mission
	//

	int payment			= (int) NumberGenerator::RandomNormalNumber ( (float) ( difficulty * PAYMENT_MISSION_CHANGEDATA_ACADEMIC ),
	                                                                  (float) ( difficulty * PAYMENT_MISSION_CHANGEDATA_ACADEMIC
	                                                                                       * PAYMENT_MISSION_VARIANCE ) );

	int minrating		= difficulty;
	int acceptrating	= minrating + NumberGenerator::RandomNumber ( 2 );

	if ( minrating > 10 ) minrating = 10;
	if ( acceptrating > 10 ) acceptrating = 10;

	payment = int ( payment / 100 ) * 100;				// Rounds payment to the nearest 100


	char personname [SIZE_PERSON_NAME];					// Person to send completion email to
	UplinkSnprintf ( personname, sizeof ( personname ), "internal@%s.net", employer->name );

	char description [SIZE_MISSION_DESCRIPTION];
	std::ostrstream details;
	std::ostrstream fulldetails;

	details << "Payment for this job is " << payment << " credits.\n"
			<< "This job has been assigned an Uplink difficulty of " << difficulty << ".\n"
			<< "An UplinkRating of " << Rating::GetUplinkRatingString ( acceptrating ) << " or above will be sufficient for automatic acceptance.\n\n"
			<< '\x0';

	fulldetails << "Gain access to the International Academic Database:\n"
				<< "TARGET COMPUTER DATA :\n"
				<< "   LOCATION: " << target->name << "\n"
				<< "   IP      : " << target->ip << "\n"
				<< "\n\n";

	Person *person = NULL;							// Person whom mission affects

	char completionA [SIZE_VLOCATION_IP];			// IP
	char completionB [SIZE_PERSON_NAME];			// Target person
	char completionC [64];							// Field to be changed
	char completionD [64];							// Word that must appear in the field
	char completionE [64];							// Word that must appear in the field

    char whysomuchmoney [256];

	int missiontype = NumberGenerator::RandomNumber ( 4 ) + 1;

		/* =========================================================================
			type 1		:		Add a degree
			type 2		:		Improve a degree rating
			type 3		:		Decrease a degree rating
			type 4		:		Add a random qualification
		   ========================================================================= */

	if ( missiontype == 1 ) {		// Add a degree =====================================================

		// Find someone without a degree

		Record *record = target->recordbank.GetRandomRecord ( "University = None" );
		if ( !record ) return NULL;

		char *personname = record->GetField ( RECORDBANK_NAME );
		UplinkAssert (personname);

		if ( strcmp ( personname, game->GetWorld ()->GetPlayer ()->handle ) == 0 ) return NULL;

		person = game->GetWorld ()->GetPerson ( personname );
		UplinkAssert (person);

		// Generate a University degree that this person wants

		int degreelevel = 1;
		char degreesubject [32];

		switch ( NumberGenerator::RandomNumber ( 10 ) ) {

			case 0 :		UplinkStrncpy ( degreesubject, "Computing", sizeof ( degreesubject ) );						break;
			case 1 :		UplinkStrncpy ( degreesubject, "Physics", sizeof ( degreesubject ) );						break;
			case 2 :		UplinkStrncpy ( degreesubject, "Electrical Engineering", sizeof ( degreesubject ) );		break;
			case 3 :		UplinkStrncpy ( degreesubject, "Mechanical Engineering", sizeof ( degreesubject ) );		break;
			case 4 :		UplinkStrncpy ( degreesubject, "Software Engineering", sizeof ( degreesubject ) );			break;
			case 5 :		UplinkStrncpy ( degreesubject, "Systems Engineering", sizeof ( degreesubject ) );			break;
			case 6 :		UplinkStrncpy ( degreesubject, "Management", sizeof ( degreesubject ) );					break;
			case 7 :		UplinkStrncpy ( degreesubject, "Artificial Intelligence", sizeof ( degreesubject ) );		break;
			case 8 :		UplinkStrncpy ( degreesubject, "Philosophy", sizeof ( degreesubject ) );					break;
			case 9 :		UplinkStrncpy ( degreesubject, "Media studies", sizeof ( degreesubject ) );					break;

			default:
				UplinkAbort ( "MissionGenerator::Generate_ChangeData_EmploymentRecord, error in Degree Type" );

		}

		UplinkStrncpy ( description, "Generate a University Degree for a friend", sizeof ( description ) );

		fulldetails	<< "Provide this person with the following University Degree:\n"
					<< "UNIVERSITY DEGREE :\n"
					<< "   SUBJECT : " << degreesubject << "\n"
					<< "   CLASS   : " << degreelevel << "\n";
		UplinkStrncpy ( completionC, "University", sizeof ( completionC ) );
		UplinkStrncpy ( completionD, degreesubject, sizeof ( completionD ) );
		UplinkSnprintf ( completionE, sizeof ( completionE ), "%d", degreelevel );

        UplinkStrncpy ( whysomuchmoney, "We really need this qualification", sizeof ( whysomuchmoney ) );

	}
	else if ( missiontype == 2 ) {	// Improve a degree rating ==========================================

		// Find someone with a degree that is not a 1st

		Record *record = target->recordbank.GetRandomRecord ( "University ! None ; University - 1" );
		if ( !record ) return NULL;

		char *personname = record->GetField ( RECORDBANK_NAME );
		UplinkAssert (personname);

		if ( strcmp ( personname, game->GetWorld ()->GetPlayer ()->handle ) == 0 ) return NULL;

		person = game->GetWorld ()->GetPerson ( personname );
		UplinkAssert (person);

		UplinkStrncpy ( description, "Improve our associates' academic standing", sizeof ( description ) );
		fulldetails << "This person requires a first class degree.\n";
		UplinkStrncpy ( completionC, "University", sizeof ( completionC ) );
		UplinkSnprintf ( completionD, sizeof ( completionD ), "%d", 1 );
		UplinkSnprintf ( completionE, sizeof ( completionE ), "%d", 1 );						// Always needs a 1st

        UplinkStrncpy ( whysomuchmoney, "This qualification will be very useful to us", sizeof ( whysomuchmoney ) );

	}
	else if ( missiontype == 3 ) {	// Decrease a degree rating ==========================================

		// Find someone with a 1st

		Record *record = target->recordbank.GetRandomRecord ( "University ! None ; University + 1" );
		if ( !record ) return NULL;

		char *personname = record->GetField ( RECORDBANK_NAME );
		UplinkAssert (personname);

		if ( strcmp ( personname, game->GetWorld ()->GetPlayer ()->handle ) == 0 ) return NULL;

		person = game->GetWorld ()->GetPerson ( personname );
		UplinkAssert (person);

		UplinkStrncpy ( description, "Help us prove a fellow employee is over-rated", sizeof ( description ) );
		fulldetails << "We feel this person scored slightly too highly at his degree."
					<< "Give him a 3rd class replacement.\n";
		UplinkStrncpy ( completionC, "University", sizeof ( completionC ) );
		UplinkSnprintf ( completionD, sizeof ( completionD ), "%d", 3 );
		UplinkSnprintf ( completionE, sizeof ( completionE ), "%d", 3 );						// Always needs a 3rd

        UplinkStrncpy ( whysomuchmoney, "We really dont like the target", sizeof ( whysomuchmoney ) );

	}
	else if ( missiontype == 4 ) {	// Add a random qualification =======================================

		char qualification [64];

		switch ( NumberGenerator::RandomNumber ( 5 ) ) {

			case 0 :		UplinkStrncpy ( qualification, "Diploma in Computing", sizeof ( qualification ) );			break;
			case 1 :		UplinkStrncpy ( qualification, "IEEE Accreditation", sizeof ( qualification ) );			break;
			case 2 :		UplinkStrncpy ( qualification, "Certified systems engineer", sizeof ( qualification ) );	break;
			case 3 :		UplinkStrncpy ( qualification, "Btec in IT", sizeof ( qualification ) );					break;
			case 4 :		UplinkStrncpy ( qualification, "Registered Uplink Agent", sizeof ( qualification ) );		break;

			default:
				UplinkAbort ( "MissionGenerator::Generate_ChangeData_EmploymentRecord, error in Qualification Type" );

		}

		// Find someone who doesn't have this qualification

		char query [64];
		UplinkSnprintf ( query, sizeof ( query ), "Other - %s", qualification );
		Record *record = target->recordbank.GetRandomRecord ( query );
		if ( !record ) return NULL;

		char *personname = record->GetField ( RECORDBANK_NAME );
		UplinkAssert (personname);

		if ( strcmp ( personname, game->GetWorld ()->GetPlayer ()->handle ) == 0 ) return NULL;

		person = game->GetWorld ()->GetPerson ( personname );
		UplinkAssert (person);

		UplinkStrncpy ( description, "Qualifications required for wealthy professional", sizeof ( description ) );
		fulldetails << "This individual urgently requires a \n"
					<< "'" << qualification << "' Qualification adding to his record.";
		UplinkStrncpy ( completionC, "Other", sizeof ( completionC ) );
		UplinkStrncpy ( completionD, qualification, sizeof ( completionD ) );
		UplinkStrncpy ( completionE, qualification, sizeof ( completionE ) );

        UplinkStrncpy ( whysomuchmoney, "This qualification is needed to gain entry", sizeof ( whysomuchmoney ) );

	}
	else {

		UplinkAbort ( "Unrecognised mission type" );

	}


	//
	// Generate the remaining fields of the mission
	//

	UplinkStrncpy ( completionA, target->ip, sizeof ( completionA ) );
	UplinkStrncpy ( completionB, person->name, sizeof ( completionB ) );

	fulldetails	<< "\nTARGET INDIVIDUAL :\n"
				<< "   NAME    : " << person->name << "\n"
				<< "\n\n"
				<< "Send a notice of completion to\n"
				<< personname << "\n"
				<< "\n"
				<< "END"
				<< '\x0';

	char whoisthetarget [128];
	UplinkStrncpy ( whoisthetarget, "He's just a friend of ours.", sizeof ( whoisthetarget ) );

	Date postdate;
	postdate.SetDate ( &(game->GetWorld ()->date) );
	postdate.AdvanceHour ( NumberGenerator::RandomNumber ( 96 ) * -1 );
	postdate.AdvanceMinute ( NumberGenerator::RandomNumber ( 60 ) * -1 );

	//
	// Insert the mission
	//

	Mission *mission = new Mission ();
	mission->SetTYPE		 ( MISSION_CHANGEDATA );
	mission->SetCompletion   ( completionA, completionB, completionC, completionD, completionE );
	mission->SetEmployer     ( employer->name );
	mission->SetContact      ( personname );
	mission->SetPayment      ( payment, (int) ( payment * 1.1 ) );
	mission->SetDifficulty   ( difficulty );
	mission->SetMinRating    ( minrating );
	mission->SetAcceptRating ( acceptrating );
	mission->SetDescription  ( description );
	mission->SetDetails		 ( details.str () );
	mission->SetFullDetails  ( fulldetails.str () );
	mission->SetWhySoMuchMoney ( whysomuchmoney );
	if ( !game->IsRunning () )	mission->SetCreateDate   ( &postdate );
	mission->SetWhoIsTheTarget ( whoisthetarget );
	mission->GiveLink ( target->ip );

	details.rdbuf()->freeze( 0 );
	fulldetails.rdbuf()->freeze( 0 );
	//delete [] details.str ();
	//delete [] fulldetails.str ();

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );
	cu->CreateMission ( mission );

	return mission;

}

Mission *MissionGenerator::Generate_ChangeData_SocialSecurity ( Company *employer )
{

	UplinkAssert ( employer );

	int difficulty = (int) NumberGenerator::RandomNormalNumber ( MINDIFFICULTY_MISSION_CHANGEDATA_SOCIAL, DIFFICULTY_MISSION_VARIANCE );
	if ( difficulty < MINDIFFICULTY_MISSION_CHANGEDATA_SOCIAL ) difficulty = MINDIFFICULTY_MISSION_CHANGEDATA_SOCIAL;

	Computer *target = game->GetWorld ()->GetComputer ( "International Social Security Database" );
	UplinkAssert (target);

	//
	// Set up the basic variables of the mission
	//

	int payment			= (int) NumberGenerator::RandomNormalNumber ( (float) ( difficulty * PAYMENT_MISSION_CHANGEDATA_SOCIAL ),
	                                                                  (float) ( difficulty * PAYMENT_MISSION_CHANGEDATA_SOCIAL
	                                                                                       * PAYMENT_MISSION_VARIANCE ) );

	int minrating		= difficulty;
	int acceptrating	= minrating + NumberGenerator::RandomNumber ( 2 );

	if ( minrating > 10 ) minrating = 10;
	if ( acceptrating > 10 ) acceptrating = 10;

	payment = int ( payment / 100 ) * 100;				// Rounds payment to the nearest 100


	char personname [SIZE_PERSON_NAME];					// Person to send completion email to
	UplinkSnprintf ( personname, sizeof ( personname ), "internal@%s.net", employer->name );

	char description [SIZE_MISSION_DESCRIPTION];
	std::ostrstream details;
	std::ostrstream fulldetails;

	details << "Payment for this job is " << payment << " credits.\n"
			<< "This job has been assigned an Uplink difficulty of " << difficulty << ".\n"
			<< "An UplinkRating of " << Rating::GetUplinkRatingString ( acceptrating ) << " or above will be sufficient for automatic acceptance.\n\n"
			<< '\x0';

	fulldetails << "Break into the International Social Security Database:\n"
				<< "TARGET COMPUTER DATA :\n"
				<< "   LOCATION: " << target->name << "\n"
				<< "   IP      : " << target->ip << "\n"
				<< "\n\n";

	Person *person = NULL;							// Person whom mission affects

	char completionA [SIZE_VLOCATION_IP];			// IP
	char completionB [SIZE_PERSON_NAME];			// Target person
	char completionC [64];							// Field to be changed
	char completionD [64];							// Word that must appear in the field
	char completionE [64];							// Word that must appear in the field


	int missiontype = NumberGenerator::RandomNumber ( 2 ) + 1;

	/* =========================================================================
		type 1		:		Make someone deceased
		type 2		:		Resurrect a dead person
	   ========================================================================= */

	if ( missiontype == 1 ) {

		// Find someone who isn't dead

		Record *record = target->recordbank.GetRandomRecord ( "Personal Status ! Deceased" );
		if ( !record ) return NULL;

		char *personname = record->GetField ( RECORDBANK_NAME );
		UplinkAssert (personname);

		if ( strcmp ( personname, game->GetWorld ()->GetPlayer ()->handle ) == 0 ) return NULL;

		person = game->GetWorld ()->GetPerson ( personname );
		UplinkAssert (person);

		UplinkStrncpy ( description, "Falsify a Social Security document", sizeof ( description ) );
		fulldetails << "The man below is living too easily.  Change his Personal Status to read 'Deceased'.\n";
		UplinkStrncpy ( completionC, "Personal Status", sizeof ( completionC ) );
		UplinkStrncpy ( completionD, "Deceased", sizeof ( completionD ) );
		UplinkStrncpy ( completionE, "Deceased", sizeof ( completionE ) );

	}
	else if ( missiontype == 2 ) {

		// Find someone who is dead

		Record *record = target->recordbank.GetRandomRecord ( "Personal Status = Deceased" );
		if ( !record ) return NULL;

		char *personname = record->GetField ( RECORDBANK_NAME );
		UplinkAssert (personname);

		if ( strcmp ( personname, game->GetWorld ()->GetPlayer ()->handle ) == 0 ) return NULL;

		person = game->GetWorld ()->GetPerson ( personname );
		UplinkAssert (person);

		UplinkStrncpy ( description, "Create part of a new identity", sizeof ( description ) );
		fulldetails << "The man below recently passed away and we wish to use his identity ourselves. "
					   "Change his Personal Status to 'Employed'.\n";
		UplinkStrncpy ( completionC, "Personal Status", sizeof ( completionC ) );
		UplinkStrncpy ( completionD, "Employed", sizeof ( completionD ) );
		UplinkStrncpy ( completionE, "Employed", sizeof ( completionE ) );

	}

	//
	// Generate the remaining fields of the mission
	//

	UplinkStrncpy ( completionA, target->ip, sizeof ( completionA ) );
	UplinkStrncpy ( completionB, person->name, sizeof ( completionB ) );

	fulldetails	<< "\nTARGET INDIVIDUAL :\n"
				<< "   NAME    : " << person->name << "\n"
				<< "\n\n"
				<< "Send a notice of completion to\n"
				<< personname << "\n"
				<< "\n"
				<< "END"
				<< '\x0';

	char whoisthetarget [128];
	UplinkStrncpy ( whoisthetarget, "You wouldn't recognise the name.", sizeof ( whoisthetarget ) );

	Date postdate;
	postdate.SetDate ( &(game->GetWorld ()->date) );
	postdate.AdvanceHour ( NumberGenerator::RandomNumber ( 96 ) * -1 );
	postdate.AdvanceMinute ( NumberGenerator::RandomNumber ( 60 ) * -1 );

	//
	// Insert the mission
	//

	Mission *mission = new Mission ();
	mission->SetTYPE		 ( MISSION_CHANGEDATA );
	mission->SetCompletion   ( completionA, completionB, completionC, completionD, completionE );
	mission->SetEmployer     ( employer->name );
	mission->SetContact      ( personname );
	mission->SetPayment      ( payment, (int) ( payment * 1.1 ) );
	mission->SetDifficulty   ( difficulty );
	mission->SetMinRating    ( minrating );
	mission->SetAcceptRating ( acceptrating );
	mission->SetDescription  ( description );
	mission->SetDetails		 ( details.str () );
	mission->SetFullDetails  ( fulldetails.str () );
	mission->SetWhySoMuchMoney ( "We need to use the Social Security record very soon" );
	mission->SetHowSecure ( "You will need to bypass the Proxy server." );
	if ( !game->IsRunning () )	mission->SetCreateDate   ( &postdate );
	mission->SetWhoIsTheTarget ( whoisthetarget );
	mission->GiveLink ( target->ip );

	details.rdbuf()->freeze( 0 );
	fulldetails.rdbuf()->freeze( 0 );
	//delete [] details.str ();
	//delete [] fulldetails.str ();

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );
	cu->CreateMission ( mission );

	return mission;

}

Mission *MissionGenerator::Generate_ChangeData_CriminalRecord ( Company *employer )
{

	UplinkAssert ( employer );

	int difficulty = (int) NumberGenerator::RandomNormalNumber ( MINDIFFICULTY_MISSION_CHANGEDATA_CRIMINAL, DIFFICULTY_MISSION_VARIANCE );
	if ( difficulty < MINDIFFICULTY_MISSION_CHANGEDATA_CRIMINAL ) difficulty = MINDIFFICULTY_MISSION_CHANGEDATA_CRIMINAL;

	Computer *target = game->GetWorld ()->GetComputer ( "Global Criminal Database" );
	UplinkAssert (target);

	//
	// Set up the basic variables of the mission
	//

	int payment			= (int) NumberGenerator::RandomNormalNumber ( (float) ( difficulty * PAYMENT_MISSION_CHANGEDATA_CRIMINAL ),
	                                                                  (float) ( difficulty * PAYMENT_MISSION_CHANGEDATA_CRIMINAL
	                                                                                       * PAYMENT_MISSION_VARIANCE ) );

	int minrating		= difficulty;
	int acceptrating	= minrating + NumberGenerator::RandomNumber ( 2 );

	if ( minrating > 10 ) minrating = 10;
	if ( acceptrating > 10 ) acceptrating = 10;

	payment = int ( payment / 100 ) * 100;				// Rounds payment to the nearest 100



	char personname [SIZE_PERSON_NAME];					// Person to send completion email to
	UplinkSnprintf ( personname, sizeof ( personname ), "internal@%s.net", employer->name );

	char description [SIZE_MISSION_DESCRIPTION];
	std::ostrstream details;
	std::ostrstream fulldetails;

	details << "Payment for this job is " << payment << " credits.\n"
			<< "This job has been assigned an Uplink difficulty of " << difficulty << ".\n"
			<< "An UplinkRating of " << Rating::GetUplinkRatingString ( acceptrating ) << " or above will be sufficient for automatic acceptance.\n\n"
			<< '\x0';

	fulldetails << "Break into the Global Criminal Database:\n"
				<< "TARGET COMPUTER DATA :\n"
				<< "   LOCATION: " << target->name << "\n"
				<< "   IP      : " << target->ip << "\n"
				<< "\n\n";

	Person *person = NULL;							// Person whom mission affects

	char completionA [SIZE_VLOCATION_IP];			// IP
	char completionB [SIZE_PERSON_NAME];			// Target person
	char completionC [64];							// Field to be changed
	char completionD [64];							// Word that must appear in the field
	char completionE [64];							// Word that must appear in the field

	char whoisthetarget [128];

	int missiontype = NumberGenerator::RandomNumber ( 3 ) + 1;

	/* =========================================================================
		type 1		:		Give standard conviction
		type 2		:		Give computer-related conviction
		type 3		:		Remove a computer-related conviction
	   ========================================================================= */

	if ( missiontype == 1 ) {

		UplinkStrncpy ( description, "Help to discredit one of our rivals", sizeof ( description ) );

		Record *record = target->recordbank.GetRandomRecord ( "Convictions = None" );
		if ( !record ) return NULL;

		char *personname = record->GetField ( RECORDBANK_NAME );
		UplinkAssert (personname);

		if ( strcmp ( personname, game->GetWorld ()->GetPlayer ()->handle ) == 0 ) return NULL;

		person = game->GetWorld ()->GetPerson ( personname );
		UplinkAssert (person);

		fulldetails << "We need " << personname << " discredited so that any "
					<< "future checks on his background will come up positive.\n"
					<< "Open his Criminal Record and give him a conviction for "
					<< "Robbery.";

		UplinkStrncpy ( completionC, "Convictions", sizeof ( completionC ) );
		UplinkStrncpy ( completionD, "Robbery", sizeof ( completionD ) );
		UplinkStrncpy ( completionE, "Robbery", sizeof ( completionE ) );

		UplinkStrncpy ( whoisthetarget, "We wouldn't want to give his name out just yet.", sizeof ( whoisthetarget ) );

	}
	else if ( missiontype == 2 ) {

		UplinkStrncpy ( description, "Help to stop a hacker from ever working again", sizeof ( description ) );

		Record *record = target->recordbank.GetRandomRecord ( "Convictions = None" );
		if ( !record ) return NULL;

		char *personname = record->GetField ( RECORDBANK_NAME );
		UplinkAssert (personname);

		if ( strcmp ( personname, game->GetWorld ()->GetPlayer ()->handle ) == 0 ) return NULL;

		person = game->GetWorld ()->GetPerson ( personname );
		UplinkAssert (person);

		fulldetails << "We have suspected that " << personname
					<< " was guilty of hacking one of our systems for some time now, "
					<< "but have been unable to aquire sufficient proof.\n"
					<< "We wish to discredit him by giving him a computer related "
					<< "criminal record, preventing him from ever working in the industry again.\n"
					<< "Open his Criminal Record and give him a new conviction for "
					<< "High tech fraud.";

		UplinkStrncpy ( completionC, "Convictions", sizeof ( completionC ) );
		UplinkStrncpy ( completionD, "High tech fraud", sizeof ( completionD ) );
		UplinkStrncpy ( completionE, "High tech fraud", sizeof ( completionE ) );

		UplinkStrncpy ( whoisthetarget, "A hacker who has attacked us before.", sizeof ( whoisthetarget ) );

	}
	else if ( missiontype == 3 ) {

		UplinkStrncpy ( description, "Clear a criminal record", sizeof ( description ) );

		Record *record = target->recordbank.GetRandomRecord ( "Convictions ! None" );
		if ( !record ) return NULL;

		char *personname = record->GetField ( RECORDBANK_NAME );
		UplinkAssert (personname);

		if ( strcmp ( personname, game->GetWorld ()->GetPlayer ()->handle ) == 0 ) return NULL;

		person = game->GetWorld ()->GetPerson ( personname );
		UplinkAssert (person);

		fulldetails << "One of our friends in the industry is having some trouble "
					<< "finding employment, due to his criminal record.  We would be "
					<< "grateful if you could remove all record of his criminal past "
					<< "\n\n"
					<< "Target individual : " << personname;

		UplinkStrncpy ( completionC, "Convictions", sizeof ( completionC ) );
		UplinkStrncpy ( completionD, "None", sizeof ( completionD ) );
		UplinkStrncpy ( completionE, "None", sizeof ( completionE ) );

		UplinkStrncpy ( whoisthetarget, "Just a friend of ours who needs help.", sizeof ( whoisthetarget ) );

	}

	//
	// Generate the remaining fields of the mission
	//

	UplinkStrncpy ( completionA, target->ip, sizeof ( completionA ) );
	UplinkStrncpy ( completionB, person->name, sizeof ( completionB ) );

	fulldetails	<< "\nSend a notice of completion to\n"
				<< personname << "\n"
				<< "\n"
				<< "END"
				<< '\x0';

	Date postdate;
	postdate.SetDate ( &(game->GetWorld ()->date) );
	postdate.AdvanceHour ( NumberGenerator::RandomNumber ( 96 ) * -1 );
	postdate.AdvanceMinute ( NumberGenerator::RandomNumber ( 60 ) * -1 );

	//
	// Insert the mission
	//

	Mission *mission = new Mission ();
	mission->SetTYPE		 ( MISSION_CHANGEDATA );
	mission->SetCompletion   ( completionA, completionB, completionC, completionD, completionE );
	mission->SetEmployer     ( employer->name );
	mission->SetContact      ( personname );
	mission->SetPayment      ( payment, (int) ( payment * 1.1 ) );
	mission->SetDifficulty   ( difficulty );
	mission->SetMinRating    ( minrating );
	mission->SetAcceptRating ( acceptrating );
	mission->SetDescription  ( description );
	mission->SetDetails		 ( details.str () );
	mission->SetFullDetails  ( fulldetails.str () );
	if ( missiontype == 1 || missiontype == 2 ) mission->SetWhySoMuchMoney ( "The target may have friends." );
	if ( missiontype == 3 ) mission->SetWhySoMuchMoney ( "The police may come after you." );
	mission->SetHowSecure ( "The GCD uses a Proxy security system." );
	if ( !game->IsRunning () ) mission->SetCreateDate   ( &postdate );
	mission->SetWhoIsTheTarget ( whoisthetarget );
	mission->GiveLink ( target->ip );

	details.rdbuf()->freeze( 0 );
	fulldetails.rdbuf()->freeze( 0 );
	//delete [] details.str ();
	//delete [] fulldetails.str ();

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );
	cu->CreateMission ( mission );

	return mission;

}

Mission *MissionGenerator::Generate_FrameUser ( Company *employer )
{

	UplinkAssert (employer);

	//
	// Choose a random person to frame
	//

	Person *poorsod = WorldGenerator::GetRandomPerson ();

	return Generate_FrameUser ( employer, poorsod );

}

Mission *MissionGenerator::Generate_TraceUser ( Company *employer )
{

	/*
		This mission is never randomly generated.
		It always occurs as a consequence of somebody doing something like
		hacking a bank etc.

		*/

	return NULL;

}

Mission *MissionGenerator::Generate_TraceUser_MoneyTransfer ( Company *employer, BankComputer *source, BankComputer *target,
															 BankAccount *sacc, BankAccount *tacc, int amount, Person *hacker )
{

	UplinkAssert (employer);
	UplinkAssert (source);
	UplinkAssert (target);
	UplinkAssert (sacc);
	UplinkAssert (tacc);
	UplinkAssert (hacker);

	//
	// Set up the basic variables of the mission
	//

	int difficulty = (int) NumberGenerator::RandomNormalNumber ( MINDIFFICULTY_MISSION_TRACEUSER_BANKFRAUD, DIFFICULTY_MISSION_VARIANCE );
	if ( difficulty < MINDIFFICULTY_MISSION_TRACEUSER_BANKFRAUD ) difficulty = MINDIFFICULTY_MISSION_TRACEUSER_BANKFRAUD;

	int payment			= (int) NumberGenerator::RandomNormalNumber ( (float) ( difficulty * PAYMENT_MISSION_TRACEUSER ),
	                                                                  (float) ( difficulty * PAYMENT_MISSION_TRACEUSER
	                                                                                       * PAYMENT_MISSION_VARIANCE ) );

	int minrating		= difficulty;
	int acceptrating	= minrating + NumberGenerator::RandomNumber ( 3 );

	if ( minrating > 10 ) minrating = 10;
	if ( acceptrating > 10 ) acceptrating = 10;

	payment = int ( payment / 100 ) * 100;				// Rounds payment to the nearest 100


	char personname [SIZE_PERSON_NAME];					// Person to send completion email to
	UplinkSnprintf ( personname, sizeof ( personname ), "internal@%s.net", employer->name );


	char description [SIZE_MISSION_DESCRIPTION];
	std::ostrstream details;
	std::ostrstream fulldetails;

	UplinkStrncpy ( description, "Trace an unauthorised money transfer", sizeof ( description ) );

	details << "Payment for this job is " << payment << " credits.\n"
			<< "This job has been assigned an Uplink difficulty of " << difficulty << ".\n"
			<< "An UplinkRating of " << Rating::GetUplinkRatingString ( acceptrating ) << " or above will be sufficient for automatic acceptance.\n\n"
			<< '\x0';

	fulldetails << "One of our bank accounts was recently hacked, and funds were transfered to an unknown account."
				<< "Use any methods necessary to trace this hacker.\n"
				<< "Our compromised account is at:\n"
				<< source->name << "\n"
				<< " IP    : " << source->ip << "\n"
				<< " ACCNO : " << sacc->accountnumber << "\n"
				<< "The access code to the hacked account is " << sacc->password << "\n"
				<< "(The username is the account number)\n"
				<< "The amount transfered was " << amount << " credits.\n"
				<< "This occured around " << game->GetWorld ()->date.GetShortString ()
				<< "\n\n"
				<< "You are authorised to access the compromised account."
				<< "\n\n"
				<< "Once you have traced the hacker, send his name to us at\n"
				<< personname << " and we will take matters further if necessary.\n"
				<< "\n"
				<< "END"
				<< '\x0';

	char accesscode [128];
    Computer::GenerateAccessCode( sacc->accountnumber, sacc->password, accesscode, sizeof ( accesscode ) );

	Date postdate;
	postdate.SetDate ( &(game->GetWorld ()->date) );
	postdate.AdvanceHour ( NumberGenerator::RandomNumber ( 96 ) * -1 );
	postdate.AdvanceMinute ( NumberGenerator::RandomNumber ( 60 ) * -1 );

	char whoisthetarget [128];
	UplinkStrncpy ( whoisthetarget, "That's for you to find out.", sizeof ( whoisthetarget ) );

	//
	// Insert the mission
	//

	Mission *mission = new Mission ();
	mission->SetTYPE		 ( MISSION_TRACEUSER );
	mission->SetCompletion   ( hacker->name, source->ip, tacc->name, NULL, NULL );
	mission->SetEmployer     ( employer->name );
	mission->SetContact      ( personname );
	mission->SetPayment      ( payment, (int) ( payment * 1.1 ) );
	mission->SetDifficulty   ( difficulty );
	mission->SetMinRating    ( minrating );
	mission->SetAcceptRating ( acceptrating );
	mission->SetDescription  ( description );
	mission->SetDetails		 ( details.str () );
	mission->SetFullDetails  ( fulldetails.str () );
	if ( !game->IsRunning () ) mission->SetCreateDate   ( &postdate );
	mission->GiveLink ( source->ip );
	mission->GiveCode ( source->ip, accesscode );
	mission->SetWhySoMuchMoney ( "Bank records are notoriously hard to crack" );
    mission->SetHowSecure ( "Banks are always well defended." );
	mission->SetWhoIsTheTarget ( whoisthetarget );

	if ( hacker == game->GetWorld ()->GetPlayer () ) mission->SetNpcPriority ( true );

	details.rdbuf()->freeze( 0 );
	fulldetails.rdbuf()->freeze( 0 );
	//delete [] details.str ();
	//delete [] fulldetails.str ();

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );
	cu->CreateMission ( mission );

	return mission;

}

Mission *MissionGenerator::Generate_PayFine ( Person *person, Company *company, int amount, Date *duedate, char *reason )
{

	UplinkAssert (person);
	UplinkAssert (company);
	UplinkAssert (duedate);

	//
	// Locate a target bank account
	//

	BankComputer *bank = (BankComputer *) WorldGenerator::GetRandomComputer ( COMPUTER_TYPE_PUBLICBANKSERVER );
	UplinkAssert (bank);
	BankAccount *account = bank->GetRandomAccount ();
	UplinkAssert (account);

	//
	// Look up a contact at the company
	//

	char personname [SIZE_PERSON_NAME];					// Person to send completion email to
	UplinkSnprintf ( personname, sizeof ( personname ), "internal@%s.net", company->name );

	//
	// Fill in the fields of the mission
	//

	char completionA [64];							// Person name who pays the fine
	char completionB [64];							// (Target account) IP ACCNO
	char completionC [16];							// Amount to transfer

	UplinkStrncpy ( completionA, person->name, sizeof ( completionA ) );
	if ( strcmp ( person->name, "PLAYER" ) == 0 ) UplinkStrncpy ( completionA, game->GetWorld ()->GetPlayer ()->handle, sizeof ( completionA ) );

	UplinkSnprintf ( completionB, sizeof ( completionB ), "%s %d", bank->ip, account->accountnumber );
	UplinkSnprintf ( completionC, sizeof ( completionC ), "%d", amount );

	//
	// Create the descriptive text
	//

	char description [128];
	UplinkSnprintf ( description, sizeof ( description ), "Pay a %dc fine", amount );

	std::ostrstream fulldetails;
	fulldetails << reason
				<< "\n\n"
				<< "As a result of this action you are legally required to pay a fine to the following account. "
				<< "Failure to pay this fine before the due date could result in imprisonment."
				<< "\n\n"
				<< "BANK : " << bank->name << "\n"
				<< "IP : " << bank->ip << "\n"
				<< "ACCNO : " << account->accountnumber << "\n"
				<< "FINE : " << amount << " credits" << "\n"
				<< "BEFORE : " << duedate->GetLongString () << "\n"
				<< "\n\n"
                << "To pay this fine, access your financial screen and click on the account you wish to transfer from.\n"
                << "This will connect you to that account.\n"
                << "Then select 'transfer' from the menu and fill in the details as above.\n\n"
				<< "Once you have paid this fine, send a mail to "
				<< personname << " to confirm receipt.\n"
				<< "END." << '\x0';

	Date postdate;
	postdate.SetDate ( &(game->GetWorld ()->date) );
	postdate.AdvanceHour ( NumberGenerator::RandomNumber ( 96 ) * -1 );
	postdate.AdvanceMinute ( NumberGenerator::RandomNumber ( 60 ) * -1 );

	//
	// Build the mission
	//


	Mission *mission = new Mission ();
	mission->SetTYPE		 ( MISSION_PAYFINE );
	mission->SetCompletion   ( completionA, completionB, completionC, NULL, NULL );
	mission->SetDueDate		 ( duedate );
	mission->SetEmployer     ( company->name );
	mission->SetContact      ( personname );
	mission->SetPayment      ( 0 );
	mission->SetDifficulty   ( 0 );
	mission->SetMinRating    ( 0 );
	mission->SetAcceptRating ( 0 );
	mission->SetDescription  ( description );
	mission->SetDetails		 ( reason );
	mission->SetFullDetails  ( fulldetails.str () );
	if ( !game->IsRunning () ) mission->SetCreateDate   ( &postdate );

	fulldetails.rdbuf()->freeze( 0 );
	//delete [] fulldetails.str ();

	return mission;

}

Mission *MissionGenerator::Generate_FrameUser ( Company *employer, Person *person )
{

	UplinkAssert (employer);
	UplinkAssert (person);

	int difficulty = (int) NumberGenerator::RandomNormalNumber ( MINDIFFICULTY_MISSION_FRAMEUSER, DIFFICULTY_MISSION_VARIANCE );
	if ( difficulty < MINDIFFICULTY_MISSION_FRAMEUSER )
		difficulty = MINDIFFICULTY_MISSION_FRAMEUSER;
	else if ( difficulty > 10 )
		difficulty = 10;


	//
	// Set up the basic variables of the mission
	//

	int payment			= (int) NumberGenerator::RandomNormalNumber ( (float) ( difficulty * PAYMENT_MISSION_FRAMEUSER ),
	                                                                  (float) ( difficulty * PAYMENT_MISSION_FRAMEUSER
	                                                                                       * PAYMENT_MISSION_VARIANCE ) );

	int minrating		= difficulty;
	int acceptrating	= minrating + NumberGenerator::RandomNumber ( 3 );

	if ( minrating > 10 ) minrating = 10;
	if ( acceptrating > 10 ) acceptrating = 10;

	payment = int ( payment / 100 ) * 100;				// Rounds payment to the nearest 100


	char personname [SIZE_PERSON_NAME];					// Person to send completion email to
	UplinkSnprintf ( personname, sizeof ( personname ), "internal@%s.net", employer->name );


	char description [SIZE_MISSION_DESCRIPTION];
	std::ostrstream details;
	std::ostrstream fulldetails;

	char completionA [SIZE_MISSION_COMPLETION];									// Target name
	char completionB [SIZE_MISSION_COMPLETION];									// Type of crime
	char completionC [SIZE_MISSION_COMPLETION];									// IP of target (if applicable)


	UplinkStrncpy ( completionA, person->name, sizeof ( completionA ) );

	details << "Payment for this job is " << payment << " credits.\n"
			<< "This job has been assigned an Uplink difficulty of " << difficulty << ".\n"
			<< "An UplinkRating of " << Rating::GetUplinkRatingString ( acceptrating ) << " or above will be sufficient for automatic acceptance.\n\n"
			<< '\x0';


	int missiontype = NumberGenerator::RandomNumber ( 3 ) + 1;

	/* =================================================================
		  type 1	:		Frame a user for hacking
		  type 2	:		Frame a user for financial fraud
		  type 3	:		Frame a user for destruction of data
       ================================================================= */

	if ( missiontype == 1 ) {									// Frame a user for hacking

		UplinkStrncpy ( description, "Frame a user for computer crimes", sizeof ( description ) );

		fulldetails << "A member of the public has been interfering with our plans recently "
					   "and we believe his arrest and conviction for computer crimes will put "
					   "his operation at an end.\n"
					   "Perform a break in on a high-priority computer system and ensure that "
					   "this man is charged with the crime.  He must be jailed for several years.\n";

		UplinkStrncpy ( completionB, "HACKING", sizeof ( completionB ) );

	}
	else if ( missiontype == 2 ) {

		UplinkStrncpy ( description, "Frame a man for bank fraud", sizeof ( description ) );

		BankComputer *bank = (BankComputer *) WorldGenerator::GetRandomComputer ( COMPUTER_TYPE_PUBLICBANKSERVER );
		UplinkAssert (bank);

		fulldetails << "We need this man to be arrested and charged for breaking into a bank and "
					   "attempting some kind of financial fraud.\n"
					   "We would like you to use the bank server specified below.\n\n"
					   "Perform some form of illegal operation (such as disabling some of their security systems "
                       "and making sure they notice you) then modify the records "
					   "so that he is incriminated.\n"
					   "\n\nTARGET BANK:\n"
					   << bank->name << "\n"
					   "IP: " << bank->ip << "\n";

		UplinkStrncpy ( completionB, "FINANCIAL", sizeof ( completionB ) );
		UplinkStrncpy ( completionC, bank->ip, sizeof ( completionC ) );

		payment = (int) ( payment * 1.2 );

	}
	else if ( missiontype == 3 ) {

		UplinkStrncpy ( description, "Frame a user for destruction of data.", sizeof ( description ) );

		Computer *target = WorldGenerator::GetRandomComputer ( COMPUTER_TYPE_INTERNALSERVICESMACHINE );
		UplinkAssert (target);

		fulldetails << "Our corporation has recently found a way to kill two birds with one "
					   "proverbial stone.  We wish to destroy some valuable research data, "
					   "whilst at the same time incriminating one of our long standing enemies for the crime.\n\n"
					   "Break into the computer below and destroy all data files.  Then modify the logs "
					   "so that this man is charged with the crime and sent to jail.\n"
					   "\n\nTARGET COMPUTER:\n"
					   << target->name << "\n"
					   "IP: " << target->ip << "\n";

		UplinkStrncpy ( completionB, "DESTROYALL", sizeof ( completionB ) );
		UplinkStrncpy ( completionC, target->ip, sizeof ( completionC ) );

		payment = (int) ( payment * 1.2 );

	}

	//
	// Fill in the rest of the mission
	//

	fulldetails << "\n"
				<< "Target individual :\n"
				<< person->name << "\n"
                << "His local system : \n"
                << person->localhost << "\n\n"
				<< "Once you have completed your mission and his arrest is in the news, send a mail "
				<< "to " << personname << " and we will transfer the payment.\n"
				<< "You will only receive payment if he is charged for the crime we have outlined.\n"
				<< "Good luck.\n"
				<< '\x0';


	Date postdate;
	postdate.SetDate ( &(game->GetWorld ()->date) );
	postdate.AdvanceHour ( NumberGenerator::RandomNumber ( 96 ) * -1 );
	postdate.AdvanceMinute ( NumberGenerator::RandomNumber ( 60 ) * -1 );

	//
	// Build the mission
	//

	Mission *m = new Mission ();
	m->SetTYPE		   ( MISSION_FRAMEUSER );
	m->SetCompletion   ( completionA, completionB, missiontype == 1 ? NULL : completionC, NULL, NULL );
	m->SetEmployer     ( employer->name );
	m->SetContact      ( personname );
	m->SetPayment      ( payment, (int) ( payment * 1.1 ) );
	m->SetDifficulty   ( difficulty );
	m->SetMinRating    ( minrating );
	m->SetAcceptRating ( acceptrating );
	m->SetDescription  ( description );
	m->SetDetails	   ( details.str () );
	m->SetFullDetails  ( fulldetails.str () );
	m->SetWhySoMuchMoney ( "His employer and friends may come after you." );
	m->SetWhoIsTheTarget ( "Someone we don't like very much" );

	if ( missiontype == 1 ) m->SetHowSecure ( "That depends on what you frame him for." );
	else if ( missiontype == 2 ) m->SetHowSecure ( "Banks always have very high security." );

    m->GiveLink ( person->localhost );
	if ( missiontype == 2 || missiontype == 3 ) m->GiveLink ( completionC );

	details.rdbuf()->freeze( 0 );
	fulldetails.rdbuf()->freeze( 0 );
	//delete [] details.str ();
	//delete [] fulldetails.str ();

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );
	cu->CreateMission ( m );

	return m;

}

Mission *MissionGenerator::Generate_TraceHacker ( Mission *completedmission, Person *hacker )
{

	UplinkAssert ( completedmission );
	UplinkAssert ( hacker );

	//
	// Look up the computer that was hacked
	//

	char *hackedip = completedmission->links.GetData (0);
	VLocation *vl = game->GetWorld ()->GetVLocation ( hackedip );
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);

	/*

		Generate a follow up mission to trace the bastard that
		did the hack

		*/

	return Generate_TraceHacker ( comp, hacker );

}

Mission *MissionGenerator::Generate_TraceHacker	( Computer *hacked, Person *hacker )
{

	UplinkAssert (hacked);
	UplinkAssert (hacker);

	int difficulty = (int) NumberGenerator::RandomNormalNumber ( MINDIFFICULTY_MISSION_TRACEUSER, DIFFICULTY_MISSION_VARIANCE );
	if ( difficulty < MINDIFFICULTY_MISSION_TRACEUSER ) difficulty = MINDIFFICULTY_MISSION_TRACEUSER;

	int payment			= (int) NumberGenerator::RandomNormalNumber ( (float) ( difficulty * PAYMENT_MISSION_TRACEUSER ),
	                                                                  (float) ( difficulty * PAYMENT_MISSION_TRACEUSER
	                                                                                       * PAYMENT_MISSION_VARIANCE ) );

	int minrating = difficulty;
	int acceptrating	= minrating + NumberGenerator::RandomNumber ( 3 );

	if ( minrating > 10 ) minrating = 10;
	if ( acceptrating > 10 ) acceptrating = 10;

	payment = int ( payment / 100 ) * 100;				// Rounds payment to the nearest 100

	bool provideaccount = (hacked->TYPE == COMPUTER_TYPE_INTERNALSERVICESMACHINE);

	char *password = NULL;
	char username [12];

	if ( provideaccount ) {

		//
		// Add in a new account for the player to use
		//

        password = NameGenerator::GeneratePassword ();
		UplinkSnprintf ( username, sizeof ( username ), "temp%c%c%c%c", 'a' + NumberGenerator::RandomNumber ( 26 ),
																		'a' + NumberGenerator::RandomNumber ( 26 ),
																		'a' + NumberGenerator::RandomNumber ( 26 ),
																		'a' + NumberGenerator::RandomNumber ( 26 ) );

		Record *record = new Record ();
		record->AddField ( RECORDBANK_NAME, username );
		record->AddField ( RECORDBANK_PASSWORD, password );
		record->AddField ( RECORDBANK_SECURITY, "3" );
		hacked->recordbank.AddRecord ( record );

	}

	//
	// Look up the owner
	//

	char *companyname = hacked->companyname;

	char contact [SIZE_PERSON_NAME];
	UplinkSnprintf ( contact, sizeof ( contact ), "internal@%s.net", companyname );

	//
	// Fill in the text fields
	//

	char description [SIZE_MISSION_DESCRIPTION];
	std::ostrstream details;
	std::ostrstream fulldetails;

	UplinkStrncpy ( description, "Trace a hacker who recently broke into our systems", sizeof ( description ) );

	details << "Payment for this job is " << payment << " credits.\n"
			<< "This job has been assigned an Uplink difficulty of " << difficulty << ".\n"
			<< "An UplinkRating of " << Rating::GetUplinkRatingString ( acceptrating ) <<
			   " or above will be sufficient for automatic acceptance.\n\n"
			<< '\x0';

	fulldetails << "Our computer systems were recently compromised by a computer hacker.\n"
				   "Find this hacker and send his name to us.  We will deal with him in our "
				   "own way.\n\n"
				   "The compromised system is :\n"
				   << hacked->name << "\n"
				   << "IP : " << hacked->ip << "\n\n"
				   "The hack occured around " << game->GetWorld ()->date.GetShortString () << ".\n";

	if ( provideaccount ) {

		   fulldetails <<	"We have provided you with level 3 clearence to our system.\n"
							"Your username is " << username << "\n"
							"And your password is " << password << "\n\n";

    }

    fulldetails << "Once completed, send the name of the hacker to "
				<< contact << " and payment will be delivered.\n"
				   "END"
				<< '\x0';

	Mission *m = new Mission ();
	m->SetTYPE		 ( MISSION_TRACEUSER );
	m->SetCompletion   ( hacker->name, hacked->ip, NULL, NULL, NULL );
	m->SetEmployer     ( companyname );
	m->SetContact      ( contact );
	m->SetPayment      ( payment, (int) ( payment * 1.1 ) );
	m->SetDifficulty   ( difficulty );
	m->SetMinRating    ( minrating );
	m->SetAcceptRating ( acceptrating );
	m->SetDescription  ( description );
	m->SetDetails	   ( details.str () );
	m->SetFullDetails  ( fulldetails.str () );
	m->GiveLink        ( hacked->ip );

	m->SetWhySoMuchMoney ( "The hacker will certainly have covered his tracks" );
    m->SetHowSecure ( "You may have to hack several targets." );
	m->SetWhoIsTheTarget ( "That is for you to determine." );

    if ( provideaccount ) {
	    char code [128];
        Computer::GenerateAccessCode( username, password, code, sizeof ( code ) );
	    m->GiveCode ( hacked->ip, code );
    }

	if ( hacker == game->GetWorld ()->GetPlayer () ) m->SetNpcPriority ( true );

	details.rdbuf()->freeze( 0 );
	fulldetails.rdbuf()->freeze( 0 );
	//delete [] details.str ();
	//delete [] fulldetails.str ();

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );
	cu->CreateMission ( m );

	return m;

}

Mission *MissionGenerator::Generate_ChangeAccount ( Company *employer, Computer *source, Computer *target )
{

	int difficulty = (int) NumberGenerator::RandomNormalNumber ( MINDIFFICULTY_MISSION_CHANGEACCOUNT, DIFFICULTY_MISSION_VARIANCE );
	if ( difficulty < MINDIFFICULTY_MISSION_CHANGEACCOUNT ) difficulty = MINDIFFICULTY_MISSION_CHANGEACCOUNT;

	//
	// The employer wants you to transfer money between the two bank computers;
	// source and target.  Both account numbers are random, but obviously can't be the same.
	//

	UplinkAssert ( employer );
	UplinkAssert ( source );
	UplinkAssert ( target );
	UplinkAssert ( source->GetOBJECTID () == OID_BANKCOMPUTER );
	UplinkAssert ( target->GetOBJECTID () == OID_BANKCOMPUTER );

	//
	// Set up the basic variables of the mission
	//

	int payment			= (int) NumberGenerator::RandomNormalNumber ( (float) ( difficulty * PAYMENT_MISSION_CHANGEACCOUNT ),
	                                                                  (float) ( difficulty * PAYMENT_MISSION_CHANGEACCOUNT
	                                                                                       * PAYMENT_MISSION_VARIANCE ) );

	int minrating		= difficulty;
	int acceptrating	= minrating + NumberGenerator::RandomNumber ( 3 );

	if ( minrating > 10 ) minrating = 10;
	if ( acceptrating > 10 ) acceptrating = 10;

	payment = int ( payment / 100 ) * 100;				// Rounds payment to the nearest 100


	char personname [SIZE_PERSON_NAME];					// Person to send completion email to
	UplinkSnprintf ( personname, sizeof ( personname ), "internal@%s.net", employer->name );

	//
	// Choose random bank accounts on the
	// source and target machines
	//

	BankAccount *sourceaccount = ((BankComputer *) source)->GetRandomAccount ();
	BankAccount *targetaccount = ((BankComputer *) target)->GetRandomAccount ();

	UplinkAssert ( sourceaccount );
	UplinkAssert ( targetaccount );

	//
	// Make sure they aren't player accounts
	//

	if ( strcmp ( sourceaccount->name, game->GetWorld ()->GetPlayer ()->handle ) == 0 ) return NULL;
	if ( strcmp ( targetaccount->name, game->GetWorld ()->GetPlayer ()->handle ) == 0 ) return NULL;


	int amount_to_transfer = (int) NumberGenerator::RandomNormalNumber ( sourceaccount->balance / 2.0f, sourceaccount->balance * 0.4f );
	amount_to_transfer = int ( amount_to_transfer / 500 ) * 500;

    if ( amount_to_transfer <= 0 ) return NULL;
    if ( amount_to_transfer > sourceaccount->balance ) amount_to_transfer = (int) ( sourceaccount->balance * 0.9 );

	//
	// Fill in the fields of the mission
	//

	char completionA [64];							// (source account) IP AccNo
	char completionB [64];							// (Target account) IP ACCNO
	char completionC [16];							// Amount to transfer
	char completionD [16];							// Amount in source account
	char completionE [16];							// Amount in target account

	UplinkSnprintf ( completionA, sizeof ( completionA ), "%s %d", source->ip, sourceaccount->accountnumber );
	UplinkSnprintf ( completionB, sizeof ( completionB ), "%s %d", target->ip, targetaccount->accountnumber );
	UplinkSnprintf ( completionC, sizeof ( completionC ), "%d", amount_to_transfer );
	UplinkSnprintf ( completionD, sizeof ( completionD ), "%d", sourceaccount->balance );
	UplinkSnprintf ( completionE, sizeof ( completionE ), "%d", targetaccount->balance );
	
	char description [SIZE_MISSION_DESCRIPTION];
	std::ostrstream details;
	std::ostrstream fulldetails;

	UplinkStrncpy ( description, "Our esteemed colleague wishes to make a donation.", sizeof ( description ) );

	details << "Payment for this job is " << payment << " credits.\n"
			<< "This job has been assigned an Uplink difficulty of " << difficulty << ".\n"
			<< "An UplinkRating of " << Rating::GetUplinkRatingString ( acceptrating ) << " or above will be sufficient for automatic acceptance.\n\n"
			<< '\x0';

	fulldetails << "Break into the following well known financial institute:\n"
				<< "TARGET COMPUTER DATA :\n"
				<< "   LOCATION: " << source->name << "\n"
				<< "   IP      : " << source->ip << "\n"
				<< "\n\n"
				<< "Gain access to account number " << sourceaccount->accountnumber << ",\n"
				<< "which is in the name of " << sourceaccount->name << ".\n"
				<< "Transfer " << amount_to_transfer << " credits to the following account:\n"
				<< "   LOCATION: " << target->name << "\n"
				<< "   IP      : " << target->ip << "\n"
				<< "   ACCNO   : " << targetaccount->accountnumber << "\n"
				<< "\n\n"
				<< "Send a notice of completion to\n"
				<< personname << "\n"
				<< "\n"
				<< "END"
				<< '\x0';

	Date postdate;
	postdate.SetDate ( &(game->GetWorld ()->date) );
	postdate.AdvanceHour ( NumberGenerator::RandomNumber ( 96 ) * -1 );
	postdate.AdvanceMinute ( NumberGenerator::RandomNumber ( 60 ) * -1 );

	char whoisthetarget [128];
	UplinkStrncpy ( whoisthetarget, source->name, sizeof ( whoisthetarget ) );

	//
	// Insert the mission
	//

	Mission *mission = new Mission ();
	mission->SetTYPE		 ( MISSION_CHANGEACCOUNT );
	mission->SetCompletion   ( completionA, completionB, completionC, completionD, completionE );
	mission->SetEmployer     ( employer->name );
	mission->SetContact      ( personname );
	mission->SetPayment      ( payment, (int) ( payment * 1.1 ) );
	mission->SetDifficulty   ( difficulty );
	mission->SetMinRating    ( minrating );
	mission->SetAcceptRating ( acceptrating );
	mission->SetDescription  ( description );
	mission->SetDetails		 ( details.str () );
	mission->SetFullDetails  ( fulldetails.str () );
	mission->SetWhySoMuchMoney ( "Financial systems are always well guarded." );
	mission->SetHowSecure ( "The bank will be using Proxys and Monitors." );
	if ( !game->IsRunning () ) mission->SetCreateDate   ( &postdate );
	mission->GiveLink ( source->ip );
	mission->SetWhoIsTheTarget ( whoisthetarget );

	details.rdbuf()->freeze( 0 );
	fulldetails.rdbuf()->freeze( 0 );
	//delete [] details.str ();
	//delete [] fulldetails.str ();

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );
	cu->CreateMission ( mission );

	return mission;

}

Mission *MissionGenerator::Generate_RemoveComputer ( Company *employer, Computer *target )
{

	int difficulty = (int) NumberGenerator::RandomNormalNumber ( MINDIFFICULTY_MISSION_REMOVECOMPUTER, DIFFICULTY_MISSION_VARIANCE );
	if ( difficulty < MINDIFFICULTY_MISSION_REMOVECOMPUTER ) difficulty = MINDIFFICULTY_MISSION_REMOVECOMPUTER;

	UplinkAssert ( employer );
	UplinkAssert ( target );

	//
	// Set up the basic variables of the mission
	//

	int payment			= (int) NumberGenerator::RandomNormalNumber ( (float) ( difficulty * PAYMENT_MISSION_REMOVECOMPUTER ),
	                                                                  (float) ( difficulty * PAYMENT_MISSION_REMOVECOMPUTER
	                                                                                       * PAYMENT_MISSION_VARIANCE ) );

	int minrating		= difficulty;
	int acceptrating	= minrating + NumberGenerator::RandomNumber ( 3 );

	if ( minrating > 10 ) minrating = 10;
	if ( acceptrating > 10 ) acceptrating = 10;

	payment = int ( payment / 100 ) * 100;				// Rounds payment to the nearest 100


	char personname [SIZE_PERSON_NAME];					// Person to send completion email to
	UplinkSnprintf ( personname, sizeof ( personname ), "internal@%s.net", employer->name );

	//
	// Fill in the fields
	//

	char description [SIZE_MISSION_DESCRIPTION];
	std::ostrstream details;
	std::ostrstream fulldetails;

	UplinkStrncpy ( description, "Elite agent required for destruction of computer system", sizeof ( description ) );

	details << "Payment for this job is " << payment << " credits.\n"
			<< "This job has been assigned an Uplink difficulty of " << difficulty << ".\n"
			<< "An UplinkRating of " << Rating::GetUplinkRatingString ( acceptrating ) << " or above will be sufficient for automatic acceptance.\n\n"
			<< '\x0';

	fulldetails << "One of our competitors has made the critical mistake of becoming over dependent "
				   "on one of their primary computer systems, and we intend to take advantage of this oversight.\n"
				   "We want you to put the computer system below out of action, permenantly.\n"
				   "Use any means necessary, including virus attacks or system resets, but all data must be "
				   "destroyed and the system itself must be shut down.\n\n"
				   "Target : " << target->name << "\n"
				   "IP : " << target->ip << "\n"
				   "Once you have completed the task, send a mail to "
				   << personname << " and we will determine if you have "
				   "been successful.\n"
				   "END" <<
				   '\x0';


	char completionA [SIZE_MISSION_COMPLETION];
	UplinkStrncpy ( completionA, target->ip, sizeof ( completionA ) );

	Date postdate;
	postdate.SetDate ( &(game->GetWorld ()->date) );
	postdate.AdvanceHour ( NumberGenerator::RandomNumber ( 96 ) * -1 );
	postdate.AdvanceMinute ( NumberGenerator::RandomNumber ( 60 ) * -1 );

	char whoisthetarget [128];
	UplinkSnprintf ( whoisthetarget, sizeof ( whoisthetarget ), "The system is owned by %s", target->companyname );

	//
	// Insert the mission
	//

	Mission *mission = new Mission ();
	mission->SetTYPE		 ( MISSION_REMOVECOMPUTER );
	mission->SetCompletion   ( completionA, NULL, NULL, NULL, NULL );
	mission->SetEmployer     ( employer->name );
	mission->SetContact      ( personname );
	mission->SetPayment      ( payment, (int) ( payment * 1.1 ) );
	mission->SetDifficulty   ( difficulty );
	mission->SetMinRating    ( minrating );
	mission->SetAcceptRating ( acceptrating );
	mission->SetDescription  ( description );
	mission->SetDetails		 ( details.str () );
	mission->SetFullDetails  ( fulldetails.str () );
	mission->SetWhySoMuchMoney ( "The computer system will be heavily defended." );
	mission->GiveLink ( target->ip );
	mission->SetWhoIsTheTarget ( whoisthetarget );
	if ( !game->IsRunning () ) mission->SetCreateDate   ( &postdate );

	details.rdbuf()->freeze( 0 );
	fulldetails.rdbuf()->freeze( 0 );
	//delete [] details.str ();
	//delete [] fulldetails.str ();

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );
	cu->CreateMission ( mission );

	return mission;


}

Mission *MissionGenerator::Generate_RemoveCompany ( Company *employer )
{
	return NULL;
}

Mission *MissionGenerator::Generate_RemoveUser ( Company *employer )
{

	int difficulty = (int) NumberGenerator::RandomNormalNumber ( MINDIFFICULTY_MISSION_REMOVEUSER, DIFFICULTY_MISSION_VARIANCE );
	if ( difficulty < MINDIFFICULTY_MISSION_REMOVEUSER ) difficulty = MINDIFFICULTY_MISSION_REMOVEUSER;


	UplinkAssert ( employer );

	//
	// Set up the basic variables of the mission
	//

	int payment			= (int) NumberGenerator::RandomNormalNumber ( (float) ( difficulty * PAYMENT_MISSION_REMOVEUSER ),
	                                                                  (float) ( difficulty * PAYMENT_MISSION_REMOVEUSER
	                                                                                       * PAYMENT_MISSION_VARIANCE ) );

	int minrating		= difficulty;
	int acceptrating	= minrating + NumberGenerator::RandomNumber ( 3 );

	if ( minrating > 10 ) minrating = 10;
	if ( acceptrating > 10 ) acceptrating = 10;

	payment = int ( payment / 100 ) * 100;				// Rounds payment to the nearest 100


	char personname [SIZE_PERSON_NAME];					// Person to send completion email to
	UplinkSnprintf ( personname, sizeof ( personname ), "internal@%s.net", employer->name );

	//
	// Find the target person
	// (Never the player)
	//

	Person *target = WorldGenerator::GetRandomPerson ();
	if ( target == game->GetWorld ()->GetPlayer () ) return NULL;

	//
	// Fill in the fields
	//

	char description [SIZE_MISSION_DESCRIPTION];
	std::ostrstream details;
	std::ostrstream fulldetails;


	UplinkStrncpy ( description, "Highly skilled Agent required for removal job", sizeof ( description ) );

	details << "Payment for this job is " << payment << " credits.\n"
			<< "This job has been assigned an Uplink difficulty of " << difficulty << ".\n"
			<< "An UplinkRating of " << Rating::GetUplinkRatingString ( acceptrating ) << " or above will be sufficient for automatic acceptance.\n\n"
			<< '\x0';

	fulldetails << "Recently this man has become quite an annoyance and "
				   "we would like him removed.  We do not care how you do it, "
				   "as long as his life is destroyed by the time you have "
				   "finished.  \n\n"
				   "TARGET : " << target->name << "\n\n"
				   "Once you have completed the task, send a mail to "
				   << personname << " and we will determine if you have "
				   "been successful.\n"
				   "END" <<
				   '\x0';


	char completionA [SIZE_MISSION_COMPLETION];
	UplinkStrncpy ( completionA, target->name, sizeof ( completionA ) );

	Date postdate;
	postdate.SetDate ( &(game->GetWorld ()->date) );
	postdate.AdvanceHour ( NumberGenerator::RandomNumber ( 96 ) * -1 );
	postdate.AdvanceMinute ( NumberGenerator::RandomNumber ( 60 ) * -1 );

	//
	// Insert the mission
	//

	Mission *mission = new Mission ();
	mission->SetTYPE		 ( MISSION_REMOVEUSER );
	mission->SetCompletion   ( completionA, NULL, NULL, NULL, NULL );
	mission->SetEmployer     ( employer->name );
	mission->SetContact      ( personname );
	mission->SetPayment      ( payment, (int) ( payment * 1.1 ) );
	mission->SetDifficulty   ( difficulty );
	mission->SetMinRating    ( minrating );
	mission->SetAcceptRating ( acceptrating );
	mission->SetDescription  ( description );
	mission->SetDetails		 ( details.str () );
	mission->SetFullDetails  ( fulldetails.str () );
	mission->SetWhySoMuchMoney ( "A great deal of skill will be required." );
	mission->SetHowSecure ( "That depends on the method you use." );
	if ( !game->IsRunning () ) mission->SetCreateDate   ( &postdate );

	details.rdbuf()->freeze( 0 );
	fulldetails.rdbuf()->freeze( 0 );
	//delete [] details.str ();
	//delete [] fulldetails.str ();

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );
	cu->CreateMission ( mission );

	return mission;

}

bool MissionGenerator::IsMissionComplete ( Mission *mission, Person *person, Message *message )
{

	UplinkAssert (mission);

	switch ( mission->TYPE ) {

		case MISSION_STEALFILE:				return IsMissionComplete_StealFile	    ( mission, person, message );		break;
		case MISSION_DESTROYFILE:			return IsMissionComplete_DestroyFile    ( mission, person, message );		break;
		case MISSION_FINDDATA:				return IsMissionComplete_FindFinancial  ( mission, person, message );		break;
		case MISSION_CHANGEDATA:			return IsMissionComplete_ChangeData		( mission, person, message );		break;
		case MISSION_FRAMEUSER:				return IsMissionComplete_FrameUser		( mission, person, message );		break;
		case MISSION_TRACEUSER:				return IsMissionComplete_TraceUser		( mission, person, message );		break;
		case MISSION_CHANGEACCOUNT:			return IsMissionComplete_ChangeAccount  ( mission, person, message );		break;
		case MISSION_REMOVECOMPUTER:		return IsMissionComplete_RemoveComputer ( mission, person, message );		break;
		case MISSION_REMOVEUSER:			return IsMissionComplete_RemoveUser		( mission, person, message );		break;
		case MISSION_PAYFINE:				return IsMissionComplete_PayFine		( mission, person, message );		break;

        case MISSION_SPECIAL:               return IsMissionComplete_Special        ( mission, person, message );       break;

		default:
			UplinkAbort ( "Unknown Mission Type" );

	}

	return false;

}

bool MissionGenerator::IsMissionComplete_StealFile ( Mission *mission, Person *person, Message *message )
{

	UplinkAssert (mission);

	char *ip		= mission->completionA;
	char *datatitle = mission->completionB;					// Required data title

	if ( strcmp ( datatitle, "ALL" ) == 0 )
		return IsMissionComplete_StealAllFiles ( mission, person, message );


	// Data must be uncompressed and not encrypted

	Data *msgdata = message->GetData ();

	if ( msgdata
		 && strcmp ( msgdata->title, datatitle ) == 0
		 && msgdata->encrypted == 0 ) {

		MissionCompleted ( mission, person, message );
		return true;

	}
	else if ( msgdata
		 && strcmp ( msgdata->title, datatitle ) == 0
		 && msgdata->encrypted > 0 ) {

		// Message is encrypted

		std::ostrstream reason;
		reason << "Your mission is not yet complete.\n"
			   << "The data you sent us is encrypted with level " << msgdata->encrypted
			   << " encryption, which we are unable to break.\n"
			   << "\nPlease break the encryption on the file and send it too us again.\n"
			   << "You will not be paid until this has been accomplished."
			   << '\x0';

		MissionNotCompleted ( mission, person, message, reason.str () );

		reason.rdbuf()->freeze( 0 );
		//delete [] reason.str ();
		return false;

	}
/*
	else if ( msgdata
		 && strcmp ( msgdata->title, datatitle ) == 0
		 && msgdata->compressed > 0 ) {

		// Message is compressed

		std::ostrstream reason;
		reason << "Your mission is not yet complete.\n"
			   << "The data you sent us is compressed with level " << msgdata->compressed
			   << " compression, which we are unable to decode.\n"
			   << "\nPlease decompress the file and send it too us again.\n"
			   << "You will not be paid until this has been accomplished."
			   << '\x0';

		MissionNotCompleted ( mission, person, message, reason.str () );

		reason.rdbuf()->freeze( 0 );
		//delete [] reason.str ();
		return false;

	}
*/
	else {

		MissionNotCompleted ( mission, person, message, "You don't appear to have attached the file." );
		return false;

	}

}

bool MissionGenerator::IsMissionComplete_StealAllFiles ( Mission *mission, Person *person, Message *message )
{

	UplinkAssert (mission);
	UplinkAssert (person);
	UplinkAssert (message);

	char *ip		= mission->completionA;
	char *datatitle = mission->completionB;					// ALL
	char *ourcompip = mission->completionC;
	int numfiles, datasize;
	sscanf ( mission->completionD, "%d %d", &numfiles, &datasize );
	char *datatype	= mission->completionE;

	//
	// Look up the target computer
	//

	VLocation *vltarget = game->GetWorld ()->GetVLocation ( ip );
	UplinkAssert ( vltarget );
	Computer *target = vltarget->GetComputer ();
	UplinkAssert (target);

	//
	// Work out the filenames that should be here
	//

	char missiontypestring [8];

	if      ( strcmp ( datatype, "SCIENTIFIC" ) == 0 ) {       UplinkStrncpy ( missiontypestring, "rsrch", sizeof ( missiontypestring ) );
	} else if	( strcmp ( datatype, "RESEARCH" ) == 0 ) {     UplinkStrncpy ( missiontypestring, "rsrch", sizeof ( missiontypestring ) );          // Not needed anymore, but older user files may have this
	} else if ( strcmp ( datatype, "CORPORATE" ) == 0 ) {      UplinkStrncpy ( missiontypestring, "corp", sizeof ( missiontypestring ) );
	} else if ( strcmp ( datatype, "CUSTOMER" ) == 0 ) {       UplinkStrncpy ( missiontypestring, "custmr", sizeof ( missiontypestring ) );
	} else if ( strcmp ( datatype, "SOFTWARE" ) == 0 ) {       UplinkStrncpy ( missiontypestring, "softw", sizeof ( missiontypestring ) );
	} else {	UplinkAbort ( "Unrecognised data type" );
	}

	char stolendatatitle [64];
	UplinkSnprintf ( stolendatatitle, sizeof ( stolendatatitle ), "%c%c%c%c-%s", target->companyname [0], target->companyname [1],
																  target->companyname [2], target->companyname [3],
																  missiontypestring );

	//
	// Lookup the dump computer
	//

	VLocation *vlourcomp = game->GetWorld ()->GetVLocation ( ourcompip );
	UplinkAssert (vlourcomp);
	Computer *ourcomp = vlourcomp->GetComputer ();
	UplinkAssert (ourcomp);

	//
	// Work out how much of the data has been successfully stolen
	// Ignore duplicate files or anything that doesn't have the
	// correct filename
	//

	int foundsize = 0;
	bool foundencrypted = false;

	bool *filefound = new bool [numfiles];
	for ( int fi = 0; fi < numfiles; ++fi )
		filefound [fi] = false;

	for ( int i = 0; i < ourcomp->databank.GetDataSize (); ++i ) {
		if ( ourcomp->databank.GetDataFile (i) ) {

			Data *thisfile = ourcomp->databank.GetDataFile (i);
			UplinkAssert (thisfile);

			if ( strstr ( thisfile->title, stolendatatitle ) != NULL ) {

				if ( thisfile->encrypted == 0 ) {

					int thisfileindex;

					char *lastspace = strrchr ( thisfile->title, ' ' );
					if ( lastspace ) {

						sscanf ( lastspace, " %d.dat", &thisfileindex );

						if ( thisfileindex >= 0 && thisfileindex < numfiles &&
							 filefound [thisfileindex] == false ) {

							foundsize += thisfile->size;
							filefound [thisfileindex] = true;

						}

					}

				}
				else {

					foundencrypted = true;

				}

			}

		}
	}

	//
	// If they've got less than half, they've failed
	// Otherwise, base their remaining payment on the
	// fraction that they successfully downloaded.
	//

	if ( foundsize == 0 ) {

		if ( foundencrypted )
			MissionNotCompleted ( mission, person, message, "We can see some files on the fileserver but they "
															"appear to be encrypted.  You need to decrypt them "
															"before we make payment." );

		else
			MissionNotCompleted ( mission, person, message, "We can't find any of the files on the "
															"file server we specified." );
		return false;

	}
	else if ( foundsize < datasize / 2 ) {

		if ( foundencrypted )
			MissionNotCompleted ( mission, person, message, "You haven't recovered very much data and a lot of it "
															"is encrypted - please copy more and decrypt all of it." );

		else
			MissionNotCompleted ( mission, person, message, "You haven't copied very much of the data - "
															"we'll need more before we pay you." );

		return false;

	}
	else if ( foundsize == datasize ) {

		MissionCompleted ( mission, person, message );

		// Shut down the file server
		// TODO

		return true;

	}
	else {

		float fractionfound = (float) foundsize / (float) datasize;
		mission->payment = int (fractionfound * mission->payment);
		mission->payment = 100 * int (mission->payment / 100);
		if ( mission->payment < 100 ) mission->payment = 100;

		char newmessage [256];
		UplinkSnprintf ( newmessage, sizeof ( newmessage ), "We've reviewed your recent mission to copy data files.\n"
															  "It looks like you got most of the files but some are missing.\n"
															  "We've decided to reduce your remaining payment to %dc.", mission->payment );

		Message *m = new Message ();
		m->SetTo ( "PLAYER" );
		m->SetFrom ( person->name );
		m->SetSubject ( "Your recent mission" );
		m->SetBody ( newmessage );
		m->Send ();

		MissionCompleted ( mission, person, message );

		// Shut down the file server
		// TODO

		return true;

	}

}

bool MissionGenerator::IsMissionComplete_DestroyFile ( Mission *mission, Person *person, Message *message )
{

	UplinkAssert (mission);

	char *ip = mission->completionA;
	char *filename = mission->completionB;

	VLocation *vl = game->GetWorld ()->GetVLocation (ip);
	UplinkAssert (vl);

	if ( strcmp ( filename, "ALL" ) == 0 ) {

		CompanyUplink *uplink = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
		UplinkAssert (uplink);

		for ( int in = 0; in < uplink->news.Size (); ++in ) {

			News *news = uplink->news.GetData (in);
			UplinkAssert (news);

			// method 1 - Delete all files

			if ( news->NEWSTYPE == NEWS_TYPE_DELETED && news->data1 && strcmp ( news->data1, ip ) == 0 ) {

				MissionCompleted ( mission, person, message );
				return true;

			}

			// method 2 - Destroy the entire computer

			if ( news->NEWSTYPE == NEWS_TYPE_COMPUTERDESTROYED && news->data1 && strcmp ( news->data1, ip ) == 0 ) {

				MissionCompleted ( mission, person, message );
				return true;

			}

		}

		MissionNotCompleted ( mission, person, message, "We have not heard word yet that the damage has been done." );
		return false;

	}
	else {

		Data *data = vl->GetComputer ()->databank.GetData ( filename );

		if ( !data ) {

			MissionCompleted ( mission, person, message );
			return true;

		}
		else {

			MissionNotCompleted ( mission, person, message, "According to our sources, the file has not yet been deleted." );
			return false;

		}

	}

}

bool MissionGenerator::IsMissionComplete_FindData ( Mission *mission, Person *person, Message *message )
{

	UplinkAssert (mission);

	char *value1 = LowerCaseString ( mission->completionA );
	char *value2 = LowerCaseString ( mission->completionB );
	char *value3 = LowerCaseString ( mission->completionC );
	char *value4 = LowerCaseString ( mission->completionD );
	char *value5 = LowerCaseString ( mission->completionE );

	// All 5 values need to be in the message
	// (Unless they are NULL)

	char *msgbody = LowerCaseString ( message->GetBody () );
	bool success = true;

	if ( value1 && !( strstr ( msgbody, value1 ) ) ) success = false;
	if ( value2 && !( strstr ( msgbody, value2 ) ) ) success = false;
	if ( value3 && !( strstr ( msgbody, value3 ) ) ) success = false;
	if ( value4 && !( strstr ( msgbody, value4 ) ) ) success = false;
	if ( value5 && !( strstr ( msgbody, value5 ) ) ) success = false;

	delete [] value1;
	delete [] value2;
	delete [] value3;
	delete [] value4;
	delete [] value5;
	delete [] msgbody;

	if ( success ) {

		MissionCompleted ( mission, person, message );
		return true;

	}
	else {

		MissionNotCompleted ( mission, person, message, "This data appears to be invalid.  Please verify it." );
		return false;

	}

}

bool MissionGenerator::IsMissionComplete_FindFinancial  ( Mission *mission, Person *person, Message *message )
{

    UplinkAssert (mission);
    UplinkAssert (message);

    // CompletionA = sourceIP accountNum
    // CompletionB = Field (balance, loan etc)
    // CompletionC = name of person responsible (if type == find source of transfer)

	char source_ip [SIZE_VLOCATION_IP];
	int source_acc;
	sscanf ( mission->completionA, "%s %d", source_ip, &source_acc );
	char source_acc_s [16];
	UplinkSnprintf ( source_acc_s, sizeof ( source_acc_s ), "%d", source_acc );

    char *field = mission->completionB;

	BankAccount *source_account = BankAccount::GetAccount ( source_ip, source_acc_s );
	UplinkAssert (source_account);

    if ( strcmp ( field, "Balance" ) == 0 ) {

        int actualbalance = source_account->balance;
        char sbalance [64];
        UplinkSnprintf ( sbalance, sizeof ( sbalance ), "%d", actualbalance );
        if ( strstr ( message->GetBody (), sbalance ) != NULL ) {

            MissionCompleted ( mission, person, message );
            return true;

        }
        else {

            MissionNotCompleted ( mission, person, message, "Could you please verify your results.  We have reason to believe they are not accurate." );
            return false;

        }

    }
    else if ( strcmp ( field, "Loan" ) == 0 ) {

        int actualloan = source_account->loan;
        char sloan [64];
        UplinkSnprintf ( sloan, sizeof ( sloan ), "%d", actualloan );
        if ( strstr ( message->GetBody (), sloan ) != NULL ) {

            MissionCompleted ( mission, person, message );
            return true;

        }
        else {

            MissionNotCompleted ( mission, person, message, "Could you please verify your results.  We have reason to believe they are not accurate." );
            return false;

        }

    }
    else if ( strcmp ( field, "TraceTransfer" ) == 0 ) {

        char *personresponsible = LowerCaseString (mission->completionC);
        UplinkAssert (personresponsible);

		char *messagebody = LowerCaseString(message->GetBody ());

		bool ispersonresponsible = ( strstr ( messagebody, personresponsible ) != NULL );

		delete [] personresponsible;
		delete [] messagebody;

		if ( ispersonresponsible ) {

            MissionCompleted ( mission, person, message );
            return true;

        }
        else {

            MissionNotCompleted ( mission, person, message, "Could you please verify your results.  We have reason to believe that person is not responsible." );
            return false;

        }

    }
    else {

        UplinkAbort ( "Error in IsMissionComplete_FindFinancial" );
        return false;

    }

}

bool MissionGenerator::IsMissionComplete_ChangeData	( Mission *mission, Person *person, Message *message )
{

	UplinkAssert (mission);

	char *ip			= mission->completionA;
	char *personname	= mission->completionB;
	char *fieldname		= mission->completionC;

	VLocation *vl = game->GetWorld ()->GetVLocation (ip);
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);

	Record *rec = comp->recordbank.GetRecordFromName ( personname );

	if ( rec ) {

		// This is the record that should have been jimmied
		bool foundStrings = false;
		char *fieldvalue = rec->GetField ( fieldname );

		if ( fieldvalue ) {

			char *string1 = mission->completionD ? LowerCaseString ( mission->completionD ) : NULL;      
			char *string2 = mission->completionE ? LowerCaseString ( mission->completionE ) : NULL;

			char *fieldvaluelower = LowerCaseString ( fieldvalue );
			foundStrings = ( strstr ( fieldvaluelower, string1 ) && strstr ( fieldvaluelower, string2 ) );
			delete [] fieldvaluelower;

			if ( string1 )
				delete [] string1;
			if ( string2 )
				delete [] string2;

		}

		if ( foundStrings ) {

			// Mission Accomplished
			MissionCompleted ( mission, person, message );
			return true;

		}
		else {

			MissionNotCompleted ( mission, person, message, "You haven't changed the data yet" );
			return false;

		}

	}

	// Oh dear.  The record hasn't been found, but it should be there

	UplinkAbort ( "MissionGenerator::IsMissionComplete_ChangeData - could not find record" );
	return false;				// Keeps the compiler happy

}

bool MissionGenerator::IsMissionComplete_FrameUser ( Mission *mission, Person *person, Message *message )
{

	UplinkAssert (mission);
	UplinkAssert (person);
	UplinkAssert (message);

	char *personname = mission->completionA;
	char *framedfor  = mission->completionB;
	char *ip		 = mission->completionC;

	//
	// Has the person been arrested?
	// Look through recent news articles
	//

	bool arrested = false;

	CompanyUplink *uplink = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (uplink);

	for ( int i = 0; i < uplink->news.Size (); ++i ) {

		News *news = uplink->news.GetData (i);
		UplinkAssert (news);

		if ( news->NEWSTYPE == NEWS_TYPE_ARREST && news->data1 && strcmp ( news->data1, personname ) == 0 ) {

			if ( strcmp ( framedfor, "FINANCIAL" ) == 0 || strcmp ( framedfor, "DESTROYALL" ) == 0 ) {

				// Make sure he was arrested for hacking this computer
				arrested = (news->data2 && ip && strcmp ( news->data2, ip ) == 0);
				break;

			}
			else {

				// Could have hacked anywhere - ip not important
				arrested = true;
				break;

			}

		}

	}

	if ( !arrested ) {

		MissionNotCompleted ( mission, person, message, "That person has not yet been arrested.  We will not make payment until then." );
		return false;

	}
	else {

		// They have been arrested - but was the crime commited?

		if ( strcmp ( framedfor, "HACKING" ) == 0 ) {								// Bog standard hack

			if ( strcmp ( message->from, "PLAYER" ) == 0 )
				game->GetWorld ()->GetPlayer ()->score_peoplefucked ++;

			MissionCompleted ( mission, person, message );
			return true;

		}
		else if ( strcmp ( framedfor, "FINANCIAL" ) == 0 ) {						// Financial fraud

			for ( int in = 0; in < uplink->news.Size (); ++in ) {

				News *news = uplink->news.GetData (in);
				UplinkAssert (news);

				if ( news->NEWSTYPE == NEWS_TYPE_HACKED && news->data1 && ip && strcmp ( news->data1, ip ) == 0 ) {

					if ( strcmp ( message->from, "PLAYER" ) == 0 )
						game->GetWorld ()->GetPlayer ()->score_peoplefucked ++;

					MissionCompleted ( mission, person, message );
					return true;

				}

			}

		}
		else if ( strcmp ( framedfor, "DESTROYALL" ) == 0 ) {						// Destroying all files

			for ( int in = 0; in < uplink->news.Size (); ++in ) {

				News *news = uplink->news.GetData (in);
				UplinkAssert (news);

				if ( news->NEWSTYPE == NEWS_TYPE_DELETED && news->data1 && ip && strcmp ( news->data1, ip ) == 0 ) {

					if ( strcmp ( message->from, "PLAYER" ) == 0 ) {
						game->GetWorld ()->GetPlayer ()->score_peoplefucked ++;
						game->GetWorld ()->GetPlayer ()->score_systemsfucked ++;
					}

					MissionCompleted ( mission, person, message );
					return true;

				}

			}

		}

		// If we get here then the crime wasn't committed

		MissionFailed ( mission, person, "The person was arrested but not for the crimes we specifically asked for.  As such we will not be making payment." );
		return false;

	}

}

bool MissionGenerator::IsMissionComplete_TraceUser ( Mission *mission, Person *person, Message *message )
{

	char *personname    = LowerCaseString (mission->completionA);
	char *targetaccount = mission->completionC ?
                            LowerCaseString (mission->completionC) :
                            NULL;                                       // NULL unless this is a TraceUser_MoneyTransfer
    char *msgbody       = LowerCaseString ( message->GetBody () );

	// Check the person's name is in the mail

	bool success = false;

	if ( strstr ( msgbody, personname ) != NULL ) {

		if ( strcmp ( message->from, "PLAYER" ) == 0 )
			game->GetWorld ()->GetPlayer ()->score_peoplefucked ++;

		MissionCompleted ( mission, person, message );
		success = true;

	}
	else {

        if ( targetaccount && strstr ( msgbody, targetaccount ) != NULL ) {

            MissionNotCompleted ( mission, person, message, "We've determined that the money was transfered into "
                                                            "an account owned by that person, but we do not believe "
                                                            "he was personally responsible.\n\n"
                                                            "We want the name of the Hacker who performed the transfer." );
        }
        else
		    MissionNotCompleted ( mission, person, message, "This person was not responsible." );

		success = false;

	}

	delete [] personname;
	if ( targetaccount )
		delete [] targetaccount;
	delete [] msgbody;

	return success;

}

bool MissionGenerator::IsMissionComplete_ChangeAccount ( Mission *mission, Person *person, Message *message )
{

	char source_ip [SIZE_VLOCATION_IP];
	char target_ip [SIZE_VLOCATION_IP];
	int source_acc;
	int target_acc;
	int amount;
	int source_balance;
	int target_balance;

	UplinkAssert (mission);

	sscanf ( mission->completionA, "%s %d", source_ip, &source_acc );
	sscanf ( mission->completionB, "%s %d", target_ip, &target_acc );
	sscanf ( mission->completionC, "%d", &amount );
	sscanf ( mission->completionD, "%d", &source_balance );
	sscanf ( mission->completionE, "%d", &target_balance );

	char source_acc_s [16];
	char target_acc_s [16];
	UplinkSnprintf ( source_acc_s, sizeof ( source_acc_s ), "%d", source_acc );
	UplinkSnprintf ( target_acc_s, sizeof ( target_acc_s ), "%d", target_acc );

	// Check the money has been transferred

	BankAccount *source_account = BankAccount::GetAccount ( source_ip, source_acc_s );
	UplinkAssert (source_account);

	BankAccount *target_account = BankAccount::GetAccount ( target_ip, target_acc_s );
	UplinkAssert (target_account);

	bool hasLogs = source_account->HasTransferOccured ( source_ip, target_ip, target_acc, amount, true );

	if ( hasLogs || ( ( source_balance - amount ) >= source_account->balance && ( target_balance + amount ) <= target_account->balance ) ) {

		MissionCompleted ( mission, person, message );
		return true;

	}
	else if ( source_balance != source_account->balance && target_balance != target_account->balance ) {

		MissionNotCompleted ( mission, person, message, "A transfer seem to have occured, verify the amount transfered." );

	}
	else {

		MissionNotCompleted ( mission, person, message, "The transfer has not occured." );

	}

	return false;

}

bool MissionGenerator::IsMissionComplete_RemoveComputer ( Mission *mission, Person *person, Message *message )
{

	UplinkAssert (mission);
	UplinkAssert (person);
	UplinkAssert (message);

	char *sourceip = mission->completionA;
	VLocation *vl = game->GetWorld ()->GetVLocation ( sourceip );
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);


	//
	// look for word that the system was shut down
	//

	CompanyUplink *uplink = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (uplink);

	for ( int in = 0; in < uplink->news.Size (); ++in ) {

		News *news = uplink->news.GetData (in);
		UplinkAssert (news);

		if ( news->NEWSTYPE == NEWS_TYPE_COMPUTERDESTROYED &&
			 news->data1 && strcmp ( news->data1, sourceip ) == 0 ) {

			// Data bank formatted
			// System shut down

			if ( strcmp ( message->from, "PLAYER" ) == 0 )
				game->GetWorld ()->GetPlayer ()->score_systemsfucked ++;

			MissionCompleted ( mission, person, message );
			return true;

		}
		else if ( news->NEWSTYPE == NEWS_TYPE_COMPUTERSHUTDOWN &&
			  	  news->data1 && strcmp ( news->data1, sourceip ) == 0 ) {

			// System shut down
			// But files not deleted

			Message *msg = new Message ();
			msg->SetTo ( message->from );
			msg->SetFrom ( person->name );
			msg->SetSubject ( "Problems with mission completion" );
			msg->SetBody ( "It appears the target system was shut down, but the files were not deleted.\n"
						   "This means the company will most likely be able to recover the data.\n\n"
						   "As such, we will only be paying you half the remaining money.\n\n"
						   "In future, you should delete all files before shutting the system down." );
			msg->Send ();

			mission->payment /= 2;
			MissionCompleted ( mission, person, message );
			return true;

		}

	}

	MissionNotCompleted ( mission, person, message, "We have not heard word yet that the system has been destroyed." );
	return false;

}

bool MissionGenerator::IsMissionComplete_RemoveCompany ( Mission *mission, Person *person, Message *message )
{
	return false;
}

bool MissionGenerator::IsMissionComplete_RemoveUser	( Mission *mission, Person *person, Message *message )
{

	UplinkAssert (mission);
	UplinkAssert (person);
	UplinkAssert (message);

	char *personname = mission->completionA;

	//
	// Look through recent news articles
	//

	CompanyUplink *uplink = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (uplink);

	for ( int i = 0; i < uplink->news.Size (); ++i ) {

		News *news = uplink->news.GetData (i);
		UplinkAssert (news);

		if ( news->NEWSTYPE == NEWS_TYPE_ARREST && news->data1 && strcmp ( news->data1, personname ) == 0 ) {

			if ( strcmp ( message->from, "PLAYER" ) == 0 )
				game->GetWorld ()->GetPlayer ()->score_peoplefucked ++;

			MissionCompleted ( mission, person, message );
			return true;

		}

	}

	MissionNotCompleted ( mission, person, message, "We don't believe you have done sufficient damage yet." );
	return false;

}

bool MissionGenerator::IsMissionComplete_PayFine ( Mission *mission, Person *person, Message *message )
{

	UplinkAssert (mission);
	UplinkAssert (person);
	UplinkAssert (message);

	char *finedperson = mission->completionA;

	char target_ip [SIZE_VLOCATION_IP];
	char target_accno [16];
	sscanf ( mission->completionB, "%s %s", target_ip, target_accno );

	int amount;
	sscanf ( mission->completionC, "%d", &amount );

	//
	// Get the target account that should have the money in
	//

	BankAccount *target_account = BankAccount::GetAccount ( target_ip, target_accno );
	UplinkAssert (target_account);

	//
	// Check the money has been transfered
	//

	bool success = target_account->HasTransferOccured ( finedperson, amount );
	if ( !success ) {
		Player *pl = game->GetWorld()->GetPlayer();
		if ( strcmp ( finedperson, pl->handle ) == 0 ) {
			for ( int i = 0; i < pl->accounts.Size (); ++i ) {
				char ip [SIZE_VLOCATION_IP];
				char accno [16];
				sscanf ( pl->accounts.GetData (i), "%s %s", ip, accno );

				BankAccount *ba = BankAccount::GetAccount ( ip, accno );
				if ( ba && ( success = target_account->HasTransferOccured ( ba->name, amount ) ) )
					break;
			}
		}
	}

	if ( success ) {

		std::ostrstream body;

		//body << "Payment of your fine has been received for the following offence: \n"
		//	 << mission->GetDetails () << "\n\n"
		//	 << "We hope this has been a sufficient deterent to hacking "
		//		"into our systems in future." << '\x0';
		
		body << "Payment of your fine has been received for the following offence: \n"
			 << mission->GetDetails () << "\n" << '\x0';

		Message *m = new Message ();
		m->SetTo ( message->from );
		m->SetFrom ( person->name );
		m->SetSubject ( "Fine paid" );
		m->SetBody ( body.str () );
		m->Send ();

		body.rdbuf()->freeze( 0 );
		//delete [] body.str ();


	}
	else {

		std::ostrstream body;
		body << "We have still not received payment for the following fine: \n"
			 << mission->GetDetails () << "\n\n"
			 << "Please ensure this payment reaches us soon to avoid further action."
			 << '\x0';

		Message *m = new Message ();
		m->SetTo ( message->from );
		m->SetFrom ( person->name );
		m->SetSubject ( "Outstanding fine still not paid" );
		m->SetBody ( body.str () );
		m->Send ();

		body.rdbuf()->freeze( 0 );
		//delete [] body.str ();

	}

	return success;

}

bool MissionGenerator::IsMissionComplete_Special ( Mission *mission, Person *person, Message *message )
{

    UplinkAssert (mission);

    if ( strcmp ( mission->description, PlotGenerator::SpecialMissionDescription ( SPECIALMISSION_MOLE ) ) == 0 ) {

        // First - has the player already done this?

        if ( game->GetWorld ()->plotgenerator.PlayerCompletedSpecialMission (SPECIALMISSION_MOLE) ) {

	        Message *m = new Message ();
	        m->SetTo ( message->from );
	        m->SetFrom ( person->name );
	        m->SetSubject ( "Mission withdrawn" );
	        m->SetBody ( "It appears you have already sold the information.\n\n"
                         "Since it is now in the public domain, we have withdrawn the mission offer." );
	        m->Send ();

            return true;

        }

        char *ourcompip = mission->completionA;

	    //
	    // Lookup the dump computer
	    //

	    VLocation *vlourcomp = game->GetWorld ()->GetVLocation ( ourcompip );
	    UplinkAssert (vlourcomp);
	    Computer *ourcomp = vlourcomp->GetComputer ();
	    UplinkAssert (ourcomp);

	    //
	    // Work out how much of the data has been successfully stolen
	    // Ignore duplicate files or anything that doesn't have the
	    // correct filename
	    //

	    int foundsize = 0;
        bool foundprogram = false;
        int numfiles = 10;

	    bool *filefound = new bool [numfiles];
	    for ( int fi = 0; fi < numfiles; ++fi )
		    filefound [fi] = false;

	    for ( int i = 0; i < ourcomp->databank.GetDataSize (); ++i ) {
		    if ( ourcomp->databank.GetDataFile (i) ) {

			    Data *thisfile = ourcomp->databank.GetDataFile (i);
			    UplinkAssert (thisfile);

			    if ( strstr ( thisfile->title, "Uplink_Agent_Data" ) != NULL &&
				     thisfile->encrypted == 0 ) {

				    char unused [64];
				    int thisfileindex;

				    sscanf ( thisfile->title, "%s %d.dat", unused, &thisfileindex );

				    if ( thisfileindex >= 0 && thisfileindex < numfiles &&
					     filefound [thisfileindex] == false ) {

					    foundsize += thisfile->size;
					    filefound [thisfileindex] = true;

				    }

			    }

                if ( strcmp ( thisfile->title, "Uplink_Agent_List" ) == 0 &&
                    thisfile->encrypted == 0 ) {

                    foundprogram = true;

                }

		    }
	    }

        //
        // Respond based on the data size found
        //

        if ( foundsize == 0 && foundprogram == false ) {

            MissionNotCompleted ( mission, person, message, "We don't see any data on our fileserver that contains "
                                                            "the names of any Uplink Agents.  Did you upload and "
                                                            "decrypt the data?" );
            return false;

        }
        else if ( foundsize == 0 && foundprogram == true ) {

            MissionNotCompleted ( mission, person, message, "Looks like you've uploaded the Uplink Agent List program, "
                                                            "but we don't see any data on our fileserver that contains "
                                                            "the names of any Uplink Agents.  The program is useless "
                                                            "without the data.\n\n"
                                                            "Did you upload and decrypt the data?" );
            return false;

        }
        else if ( foundsize > 0 ) {

            int foundpercent = (int) ( 100 * ((float) foundsize / (float) 50) );
            int payment = (int) ( PAYMENT_SPECIALMISSION_MOLE * 0.75 * ((float) foundpercent / (float) 100) );
            if ( foundprogram ) payment += (int) ( PAYMENT_SPECIALMISSION_MOLE * 0.25 );
            payment = 100 * (int) (payment / 100);

            std::ostrstream body;
            body << "Weve just checked our fileserver, and we can see lots of data containing the handles of "
                    "Agents and their real-world names.  This data will come in very handy.\n";

            if ( foundpercent == 100 )
                body << "In the end we were able to achieve almost all of the Uplink Agent names.\n";

            else
                body << "In the end we were able to achieve roughly " << 10 * (int) (foundpercent / 10)
                     << " percent of the Uplink Agent names.\n";

            if ( foundprogram )
                body << "We also noticed you uploaded the List program used by Uplink Corporation.\n";
            else
                body << "Unfortunately you didnt upload the List program used to access the data. This will make "
                        "recovery of the source data more difficult.\n";

            if ( payment == PAYMENT_SPECIALMISSION_MOLE )
                body << "We will therefore be paying you the full " << payment << " credits.\n\n";

            else
                body << "Based on this, we will only be paying you " << payment << " credits for the mission, "
                        "out of an available " << PAYMENT_SPECIALMISSION_MOLE << ".\n\n";

            body << "Once again, congratulations on an incredible hack.\n\nEND";
            body << '\x0';

	        Message *m = new Message ();
	        m->SetTo ( message->from );
	        m->SetFrom ( person->name );
	        m->SetSubject ( "Mission completed" );
	        m->SetBody ( body.str() );
	        m->Send ();

			body.rdbuf()->freeze( 0 );
			//delete [] body.str ();

            //
            // Money, ratings etc
            //

            game->GetWorld ()->GetPlayer ()->ChangeBalance ( payment, "Services Rendered" );
            game->GetWorld ()->GetPlayer ()->rating.ChangeNeuromancerScore ( -200 );
            game->GetWorld ()->plotgenerator.specialmissionscompleted |= (1 << SPECIALMISSION_MOLE);


            //
            // Schedule the murders for a week from now
            //

            Date rundate;
            rundate.SetDate ( &game->GetWorld ()->date );
            rundate.AdvanceDay ( 7 );

	        NotificationEvent *ne = new NotificationEvent ();
	        ne->SetTYPE ( NOTIFICATIONEVENT_TYPE_AGENTSONLISTDIE );
	        ne->SetRunDate ( &rundate );
	        game->GetWorld ()->scheduler.ScheduleEvent ( ne );

            return true;

        }

    }

    return false;

}

void MissionGenerator::MissionCompleted ( Mission *mission, Person *person, Message *message )
{

    UplinkAssert (mission);
    UplinkAssert (person);
    UplinkAssert (message);

	std::ostrstream body;
	body << "Congratulations on your success.\n";

	if		( mission->paymentmethod == MISSIONPAYMENT_HALFATSTART )
		body << "Other half of the funds transfered : " << mission->payment / 2 << "c\n";
	else if ( mission->paymentmethod == MISSIONPAYMENT_AFTERCOMPLETION )
		body << "Funds transfered : " << mission->payment << "c\n";

	body << "\n"
		 << "Thankyou for working for " << mission->employer << ".\n\n"
         << "THIS IS AN AUTOMATED RESPONSE.  PLEASE DO NOT REPLY."
		 << '\x0';

	Message *m = new Message ();
	m->SetTo ( message->from );
	m->SetFrom ( person->name );
	m->SetSubject ( "Mission completed" );
	m->SetBody ( body.str () );
	m->Send ();

	body.rdbuf()->freeze( 0 );
	//delete [] body.str ();

	if		( mission->paymentmethod == MISSIONPAYMENT_HALFATSTART )		game->GetWorld ()->GetPlayer ()->ChangeBalance ( mission->payment / 2, "Anonymous benefactor" );
	else if ( mission->paymentmethod == MISSIONPAYMENT_AFTERCOMPLETION )	game->GetWorld ()->GetPlayer ()->ChangeBalance ( mission->payment, "Anonymous benefactor" );

	game->GetWorld ()->GetPlayer ()->rating.ChangeUplinkScore ( mission->difficulty );
	game->GetWorld ()->GetPlayer ()->rating.ChangeNeuromancerScore ( NEUROMANCERCHANGE [mission->TYPE] );

	ConsequenceGenerator::MissionCompleted ( mission, game->GetWorld ()->GetPlayer () );

#ifdef DEMOGAME
    game->GetWorld ()->demoplotgenerator.PlayerCompletedMission ( mission );
#endif

    // Unselect all of the links on the world map

    for ( int i = 0; i < mission->links.Size (); ++i ) {
        char *ip = mission->links.GetData (i);
        UplinkAssert (ip);
        VLocation *vl = game->GetWorld ()->GetVLocation (ip);
        UplinkAssert (vl);
        Computer *comp = vl->GetComputer ();
        UplinkAssert (comp);
        if ( comp->istargetable )
            vl->SetDisplayed ( false );
    }

    SgPlaySound ( RsArchiveFileOpen ("sounds/success.wav"), "sounds/success.wav", false );

}

void MissionGenerator::MissionNotCompleted ( Mission *mission, Person *person, Message *message, char *reason )
{

    std::ostrstream body;
    body << reason;
    body << "\n\nTHIS IS AN AUTOMATED RESPONSE.  PLEASE DO NOT REPLY.";
    body << '\x0';

	Message *m = new Message ();
	m->SetTo ( message->from );
	m->SetFrom ( person->name );
	m->SetSubject ( "Mission NOT completed" );
	m->SetBody ( body.str () );
	m->Send ();

	body.rdbuf()->freeze( 0 );
	//delete [] body.str ();
}

void MissionGenerator::MissionFailed ( Mission *mission, Person *person, char *reason )
{

	UplinkAssert (mission);
	UplinkAssert (person);

	// Knock off some points from the person's score
	game->GetWorld ()->GetPlayer ()->rating.ChangeUplinkScore ( mission->difficulty * -2 );

    if ( mission->TYPE == MISSION_PAYFINE ) {

        ConsequenceGenerator::DidntPayFine ( person, mission );

        Message *m = new Message ();
        m->SetTo ( "PLAYER" );
        m->SetFrom ( mission->employer );
        m->SetSubject ( "Failure to pay fine" );
        m->SetBody ( "You have chosen not to pay a fine. As such, "
                     "legal proceedings will begin against you shortly.\n\n"
                     "You are advised to seek legal assistance." );
        m->Send ();

    }
    else {

	    std::ostrstream body;
	    body << "You have failed the following mission: \n"
		     << mission->description
		     << "\n"
		     << reason
		     << "\n\n"
		     << "In future we will employ someone more qualified.\n\n"
             << "THIS IS AN AUTOMATED RESPONSE.  PLEASE DO NOT REPLY."
             << '\x0';

	    // Send a mail to the person
	    Message *m = new Message ();
	    m->SetTo ( "PLAYER" );
	    m->SetFrom ( mission->employer );
	    m->SetSubject ( "Mission FAILED" );
	    m->SetBody ( body.str () );
	    m->Send ();

		body.rdbuf()->freeze( 0 );
		//delete [] body.str ();

    }

}

