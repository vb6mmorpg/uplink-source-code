// -*- tab-width:4 c-file-style:"cc-mode" -*-

#include <strstream>

#include <fstream>

#include "redshirt.h"

#include "app/app.h"
#include "app/dos2unix.h"
#include "app/serialise.h"

#include "game/game.h"
#include "game/data/data.h"

#include "world/world.h"
#include "world/generator/worldgenerator.h"
#include "world/generator/langenerator.h"
#include "world/generator/namegenerator.h"
#include "world/generator/numbergenerator.h"

#include "world/computer/computer.h"
#include "world/computer/lancomputer.h"
#include "world/computer/securitysystem.h"
#include "world/computer/computerscreen/computerscreen.h"
#include "world/computer/computerscreen/messagescreen.h"
#include "world/computer/computerscreen/passwordscreen.h"
#include "world/computer/computerscreen/useridscreen.h"
#include "world/computer/computerscreen/cypherscreen.h"
#include "world/computer/computerscreen/highsecurityscreen.h"
#include "world/computer/computerscreen/genericscreen.h"
#include "world/computer/computerscreen/logscreen.h"
#include "world/computer/computerscreen/menuscreen.h"
#include "world/computer/computerscreen/dialogscreen.h"

#include "interface/remoteinterface/remoteinterface.h"

#include "mmgr.h"


class dos2unix : public filebuf {
};

void LanGenerator::Initialise ()
{
}

Computer  *LanGenerator::LoadLAN ( char *filename )
{
	idos2unixstream thefile(RsArchiveFileOpen ( filename) );

	if ( thefile.eof() ) {

		thefile.close ();
		RsArchiveFileClose ( filename );
		return NULL;

	}

	//
	// Load the header information

	char computerName[SIZE_COMPUTER_NAME];
	char companyName[SIZE_COMPANY_NAME];
	char ip [SIZE_VLOCATION_IP];
	int x, y;

	char buffer [256];

    thefile >> buffer >> ws;
    thefile.getline( computerName, SIZE_COMPUTER_NAME );

	thefile >> buffer >> ws;
    thefile.getline( companyName, SIZE_COMPANY_NAME );

    thefile >> buffer >> ws;
    thefile.getline( ip, SIZE_VLOCATION_IP );

	thefile.getline( buffer, 256 );
	sscanf ( buffer, "XPOS %d", &x );

	thefile.getline( buffer, 256 );
	sscanf ( buffer, "YPOS %d", &y );

    printf ( "Loading LAN from %s...", filename );

    if ( game->GetWorld ()->GetVLocation (ip) ) {
        printf ( "[Already Loaded]\n", filename );
		thefile.close ();
		RsArchiveFileClose ( filename );
        return NULL;
    }

	//
	// Create blank LAN

	if ( !game->GetWorld()->VerifyVLocation( ip, x, y ) ) {
		printf ( "LanGenerator::LoadLAN WARNING: Location is invalid, ip(%s), x(%d), y(%d).(%s)\n", (ip)?ip:"NULL", x, y, filename );
		thefile.close ();
		RsArchiveFileClose ( filename );
        return NULL;
	}

	game->GetWorld()->CreateVLocation( ip, x, y );
//	game->GetWorld()->CreateCompany( companyName );
	int size      = (int) NumberGenerator::RandomNormalNumber ( COMPANYSIZE_AVERAGE,      COMPANYSIZE_RANGE );
	int growth    = (int) NumberGenerator::RandomNormalNumber ( COMPANYGROWTH_AVERAGE,    COMPANYGROWTH_RANGE );
	int alignment = (int) NumberGenerator::RandomNormalNumber ( COMPANYALIGNMENT_AVERAGE, COMPANYALIGNMENT_RANGE );
    Company *company = WorldGenerator::GenerateCompany ( companyName, size, COMPANYTYPE_COMMERCIAL, growth, alignment );

    Person *ceo = WorldGenerator::GetRandomPerson ();
    Person *admin = WorldGenerator::GetRandomPerson ();
    UplinkAssert (ceo);
    UplinkAssert (admin);
	company->SetBoss ( ceo->name );
	company->SetAdmin ( admin->name );

	LanComputer *comp = new LanComputer ();
	comp->SetTYPE ( COMPUTER_TYPE_LAN );
	comp->SetTraceSpeed ( TRACESPEED_LAN );
	comp->SetTraceAction ( COMPUTER_TRACEACTION_DISCONNECT |
						   COMPUTER_TRACEACTION_LEGAL );
	comp->security.AddSystem ( SECURITY_TYPE_PROXY, 5 );
	comp->security.AddSystem ( SECURITY_TYPE_MONITOR, 5 );

	comp->SetName ( computerName );
	comp->SetCompanyName ( companyName );
	comp->SetIP ( ip );
	comp->SetIsTargetable( false );
    game->GetWorld ()->CreateComputer ( comp );

    LanCluster *cluster = new LanCluster ();

	//
	// Load each LAN system

	while ( thefile )
	{

		char fullLine [256];
		thefile.getline( fullLine, 256 );

		if ( fullLine[0] == '.' )       break;
        if ( strlen(fullLine) < 2 )     continue;
		if ( fullLine[0] == ';' )       continue;

		char systemName[128];
		int index, x, y, security, data1, data2, data3;
		bool haveModemPhoneNumber = true;
        char dataX[256];
		std::istrstream thisLine ( fullLine );
		thisLine >> index >> systemName >> x >> y >> security >> data1 >> data2 >> data3 >> ws;
        if ( strstr(systemName, "MODEM") ) {
            thisLine.getline(dataX, 256);
            if ( strlen(dataX) < 3 ) {
				haveModemPhoneNumber = false;
                printf ( "LanGenerator::LoadLAN WARNING: Modem found without phone number.(%s)\n", filename );
            }
        }

		int indexUsed = -1;

		if		( strstr(systemName, "ROUTER") )			indexUsed = GenerateRouter              (comp, cluster, x, y, security);
		else if ( strstr(systemName, "HUB") )				indexUsed = GenerateHUB                 (comp, cluster, x, y, security);
		else if ( strstr(systemName, "TERMINAL") )			indexUsed = GenerateTerminal            (comp, cluster, x, y, security);
		else if ( strstr(systemName, "MAINSERVER") )		indexUsed = GenerateMainServer          (comp, cluster, x, y, security);
		else if ( strstr(systemName, "AUTHENTICATION") )	indexUsed = GenerateAuthentication      (comp, cluster, x, y, security);
		else if ( strstr(systemName, "LOCK") )				indexUsed = GenerateLock                (comp, cluster, x, y, security);
        else if ( strstr(systemName, "ISOLATIONBRIDGE") )   indexUsed = GenerateIsolationBridge     (comp, cluster, x, y, security);
        else if ( strstr(systemName, "SESSIONKEYSERVER") )  indexUsed = GenerateSessionKeyServer    (comp, cluster, x, y, security);
        else if ( strstr(systemName, "RADIOTRANSMITTER") )  indexUsed = GenerateRadioTransmitter    (comp, cluster, x, y, security);
        else if ( strstr(systemName, "RADIORECEIVER") )     indexUsed = GenerateRadioReceiver       (comp, cluster, x, y, security);
        else if ( strstr(systemName, "FAXPRINTER") )        indexUsed = GenerateFaxPrinter          (comp, cluster, x, y, security);
		else if ( strstr(systemName, "MODEM" ) ) {
			if ( haveModemPhoneNumber )
															indexUsed = GenerateModem               (comp, cluster, x, y, security, dataX, sizeof ( dataX ) );
			else
															indexUsed = GenerateModem               (comp, cluster, x, y, security);
		}
        else if ( strstr(systemName, "LOGSERVER" ) )        indexUsed = GenerateLogServer           (comp, cluster, x, y, security);

		else
			printf ( "LanGenerator::LoadLAN WARNING: Unrecognised System TYPE %s.(%s)\n", systemName, filename );

		if ( indexUsed != -1 ) {
			UplinkAssert ( cluster->systems.ValidIndex ( indexUsed ) );
			LanComputerSystem *system = cluster->systems.GetData(indexUsed);
			system->data1 = data1;
			system->data2 = data2;
			system->data3 = data3;
		}

	}

	//
	// Load all links

	while ( thefile )
	{

		//LanComputerLink *link = new LanComputerLink();

		char fullLine [256];
		thefile.getline( fullLine, 256 );

		if ( fullLine[0] == '.' )       break;
        if ( strlen(fullLine) < 2 )     continue;
		if ( fullLine[0] == ';' )       continue;

		char linkName [256];
		int from, to, security;
		float fromX, fromY, toX, toY;

		std::istrstream thisLine ( fullLine );
		thisLine >> linkName >> from >> fromX >> fromY >> to >> toX >> toY >> security >> ws;
		linkName[ sizeof(linkName) - 1 ] = '\0';

		if ( !cluster->VerifyLanLink ( from, fromX, fromY, to, toX, toY, security ) ) {
            printf ( "LanGenerator::LoadLAN WARNING: Invalid link, linkName(%s), from(%d), fromX(%f), fromY(%f), to(%d), toX(%f), toY(%f), security(%d).(%s)\n", 
			         linkName, from, fromX, fromY, to, toX, toY, security, filename );
		}
		else {
			cluster->AddLanLink( from, fromX, fromY, to, toX, toY, security );
		}

	}

    //
    // Load Valid Subnets

	while ( thefile )
	{

		char fullLine [256];
		thefile.getline( fullLine, 256 );

		if ( fullLine[0] == '.' )       break;
        if ( strlen(fullLine) < 2 )     continue;
		if ( fullLine[0] == ';' )       continue;

		char name[128];
		int systemIndex;
		int validIndex;

		std::istrstream thisLine ( fullLine );
		thisLine >> name >> systemIndex >> validIndex >> ws;

		if ( cluster->systems.ValidIndex ( systemIndex ) ) {
			LanComputerSystem *system = cluster->systems.GetData(systemIndex);
			if ( system )
				system->validSubnets.PutData( validIndex );
		}

    }

    //
    // Merge all of those systems into place

    cluster->Merge( comp );
	delete cluster;
	cluster = NULL;

    //
    // Load hidden data values

	while ( thefile )
	{

		char fullLine [256];
		thefile.getline( fullLine, 256 );

		if ( fullLine[0] == '.' )       break;
        if ( strlen(fullLine) < 2 )     continue;
		if ( fullLine[0] == ';' )       continue;

		char dataName[128];
		int index;
		char allData[256];

		std::istrstream thisLine ( fullLine );
		thisLine >> dataName >> index >> ws;
        thisLine.getline( allData, 256 );

        LanGenerator::HideData( comp, index, allData );

    }


    //
	// Create the files
    //

	int i;

	comp->databank.SetSize ( (int) NumberGenerator::RandomNormalNumber ( 100, 40 ) );

	int numfiles = (int) NumberGenerator::RandomNormalNumber ( 10, 5 );

	for ( i = 0; i < numfiles; ++i ) {

		int TYPE = NumberGenerator::RandomNumber ( 2 ) + 1;
		int size = (int) NumberGenerator::RandomNormalNumber ( 6, 4 );
		int encrypted = NumberGenerator::RandomNumber ( 2 );
		int compressed = NumberGenerator::RandomNumber ( 2 );
		char *datatitle = NameGenerator::GenerateDataName ( companyName, TYPE );

		Data *data = new Data ();
		data->SetTitle ( datatitle );
		data->SetDetails ( TYPE, size, (encrypted ? NumberGenerator::RandomNumber ( 5 ) : 0),
									   (compressed ? NumberGenerator::RandomNumber ( 5 ) : 0) );
		comp->databank.PutData ( data );

	}

    //
	// Create some logs
    //

	int numlogs = NumberGenerator::RandomNumber ( 10 );

	for ( i = 0; i < numlogs; ++i ) {

		AccessLog *al = new AccessLog ();
		al->SetProperties ( &(game->GetWorld ()->date), WorldGenerator::GetRandomLocation ()->ip, " " );
		al->SetData1 ( "Accessed File" );
		comp->logbank.AddLog (al);

	}

    //
	// Create some log-in's
    //

	Record *record = new Record ();
	record->AddField ( RECORDBANK_NAME, RECORDBANK_ADMIN );
	record->AddField ( RECORDBANK_PASSWORD, NameGenerator::GenerateComplexPassword () );
	record->AddField ( RECORDBANK_SECURITY, "1" );
	comp->recordbank.AddRecord ( record );

    printf ( "done\n" );

	thefile.close ();
	RsArchiveFileClose ( filename );

	return comp;

}

Computer  *LanGenerator::GenerateLAN ( char *companyname, int difficulty )
{

    //
    // Generate the location and computer
    //

    //char *computername = strdup( NameGenerator::GenerateLANName( companyname ) );
	char computername[MAX_COMPUTERNAME];
	strncpy( computername, NameGenerator::GenerateLANName( companyname ),
		     MAX_COMPUTERNAME );
	if ( computername[MAX_COMPUTERNAME - 1] != '\0' ) {
		computername[MAX_COMPUTERNAME - 1] = '\0';
	}

	VLocation *vl = WorldGenerator::GenerateLocation ();
    vl->SetListed ( false );

	LanComputer *comp = new LanComputer ();
	comp->SetTYPE ( COMPUTER_TYPE_LAN );
	comp->SetName ( computername );
	comp->SetCompanyName ( companyname );
	comp->SetTraceSpeed ( TRACESPEED_LAN );
	comp->SetTraceAction ( COMPUTER_TRACEACTION_DISCONNECT |
						   COMPUTER_TRACEACTION_LEGAL );
	comp->security.AddSystem ( SECURITY_TYPE_PROXY, 5 );
	comp->security.AddSystem ( SECURITY_TYPE_MONITOR, 5 );
	comp->SetIP ( vl->ip );
	game->GetWorld ()->CreateComputer ( comp );

    //delete computername;


    //
    // Generate the LAN cluster
    //

    GenerateLANCluster ( comp, difficulty );


    //
	// Create the files
    //

	comp->databank.SetSize ( (int) NumberGenerator::RandomNormalNumber ( 100, 40 ) );

	int numfiles = (int) NumberGenerator::RandomNormalNumber ( 10, 5 );
	int i;

	for ( i = 0; i < numfiles; ++i ) {

		int TYPE = NumberGenerator::RandomNumber ( 2 ) + 1;
		int size = (int) NumberGenerator::RandomNormalNumber ( 6, 4 );
		int encrypted = NumberGenerator::RandomNumber ( 2 );
		int compressed = NumberGenerator::RandomNumber ( 2 );
		char *datatitle = NameGenerator::GenerateDataName ( companyname, TYPE );

		Data *data = new Data ();
		data->SetTitle ( datatitle );
		data->SetDetails ( TYPE, size, (encrypted ? NumberGenerator::RandomNumber ( 5 ) : 0),
									   (compressed ? NumberGenerator::RandomNumber ( 5 ) : 0) );
		comp->databank.PutData ( data );

	}

    //
	// Create some logs
    //

	int numlogs = NumberGenerator::RandomNumber ( 10 );

	for ( i = 0; i < numlogs; ++i ) {

		AccessLog *al = new AccessLog ();
		al->SetProperties ( &(game->GetWorld ()->date), WorldGenerator::GetRandomLocation ()->ip, " " );
		al->SetData1 ( "Accessed File" );
		comp->logbank.AddLog (al);

	}

    //
	// Create some log-in's
    //

	Record *record = new Record ();
	record->AddField ( RECORDBANK_NAME, RECORDBANK_ADMIN );
	record->AddField ( RECORDBANK_PASSWORD, NameGenerator::GenerateComplexPassword () );
	record->AddField ( RECORDBANK_SECURITY, "1" );
	comp->recordbank.AddRecord ( record );
    return comp;

}

void LanGenerator::SanityCheckLAN ( LanComputer *comp )
{

    //
    // Sanity check those screens
    //

    for ( int i = 0; i < comp->screens.Size(); ++i ) {
        if ( comp->screens.ValidIndex (i) ) {

            ComputerScreen *cs = comp->screens.GetData(i);
            UplinkAssert (cs);
            if ( cs->GetOBJECTID () == OID_HIGHSECURITYSCREEN ) {

                HighSecurityScreen *hss = (HighSecurityScreen *) cs;

                for ( int j = 0; j < hss->systems.Size(); ++j ) {
                    if ( hss->systems.ValidIndex(j) ) {

                        MenuScreenOption *mso = hss->systems.GetData(j);
                        UplinkAssert (mso);
                        if ( comp->screens.ValidIndex(mso->nextpage) ) {

                            ComputerScreen *nextPage = comp->screens.GetData(mso->nextpage);
                            if ( nextPage->GetOBJECTID () == OID_MENUSCREEN ) {

                                printf ( "ERROR Screen %d Points to %d at %s", i, mso->nextpage, comp->companyname );

                            }

                        }

                    }
                }

            }

        }
    }

}

void LanGenerator::GenerateLANCluster ( LanComputer *comp, int difficulty )
{

    LanCluster *cluster = new LanCluster ();

	LList <int> usedRadioFrenquencies;

    switch ( difficulty ) {

		case 0 :		GenerateLAN_Level0Cluster ( comp, cluster, &usedRadioFrenquencies, 10, 100, 200, 200 );       break;
		case 1 :		GenerateLAN_Level1Cluster ( comp, cluster, &usedRadioFrenquencies, 10, 100, 275, 275 );       break;
        case 2 :        GenerateLAN_Level2Cluster ( comp, cluster, &usedRadioFrenquencies );                          break;
        case 3 :        GenerateLAN_Level3Cluster ( comp, cluster, &usedRadioFrenquencies );                          break;
        case 4 :        GenerateLAN_Level4Cluster ( comp, cluster, &usedRadioFrenquencies );                          break;

	}

	UplinkAssert ( cluster->systems.ValidIndex ( cluster->input ) );
	UplinkAssert ( cluster->systems.ValidIndex ( cluster->output ) );
    LanComputerSystem *inputSys = cluster->systems.GetData( cluster->input );
    LanComputerSystem *outputSys = cluster->systems.GetData( cluster->output );

    int routerIndex = GenerateRouter ( comp, cluster, inputSys->x - 70, inputSys->y - 100, 1 );
    cluster->AddLanLink( routerIndex, 0.5, 1.0, cluster->input, 0.5, 0.0, 1 );

    if ( difficulty == 0 || difficulty == 1 ) {
        int mainServerIndex = GenerateMainServer ( comp, cluster, outputSys->x + 70, outputSys->y + 120, 1 );
        cluster->AddLanLink( cluster->output, 0.5, 1.0, mainServerIndex, 0.5, 0.0, 1 );
    }

    int rotateBy = NumberGenerator::RandomNumber( 4 );
    cluster->Rotate( rotateBy );
    cluster->Merge( comp );

	delete cluster;

}

void LanGenerator::GenerateLAN_Level0Cluster ( LanComputer *comp, LanCluster *cluster, LList <int> *usedRadioFrenquencies, int x, int y, int width, int height )
{

    cluster->centreX = x + width / 2;
    cluster->centreY = y + height / 2;

    int type = NumberGenerator::RandomNumber(7) + 1;

    if ( type == 1 ) {

        int hubIndex = GenerateHUB ( comp, cluster, x, y, 1 );
        int t1Index = GenerateTerminal ( comp, cluster, x + width / 4 * 0, y + 100, 1 );
        int t2Index = GenerateTerminal ( comp, cluster, x + width / 4 * 1, y + 100, 1 );
        int t3Index = GenerateTerminal ( comp, cluster, x + width / 4 * 2, y + 100, 1 );
        int t4Index = GenerateTerminal ( comp, cluster, x + width / 4 * 3, y + 100, 1 );

        cluster->AddLanLink ( hubIndex, 0.2f, 1.0f, t1Index, 0.5f, 0.0f, 1 );
        cluster->AddLanLink ( hubIndex, 0.4f, 1.0f, t2Index, 0.5f, 0.0f, 1 );
        cluster->AddLanLink ( hubIndex, 0.6f, 1.0f, t3Index, 0.5f, 0.0f, 1 );
        cluster->AddLanLink ( hubIndex, 0.8f, 1.0f, t4Index, 0.5f, 0.0f, 1 );

        cluster->SetInput ( hubIndex );

        switch ( NumberGenerator::RandomNumber(4) )
        {
            case 0  :       cluster->SetOutput ( t1Index );             break;
            case 1  :       cluster->SetOutput ( t2Index );             break;
            case 2  :       cluster->SetOutput ( t3Index );             break;
            case 3  :       cluster->SetOutput ( t4Index );             break;
        };

    }
    else if ( type == 2 ) {

        int hubIndex = GenerateHUB ( comp, cluster, x + width / 2, y, 1 );
        int lockIndex = GenerateLock ( comp, cluster, x + width / 2 - 60, y + height / 2, 2 );
        int authIndex = GenerateAuthentication ( comp, cluster, x + width - 60, y + height / 2, 2 );

		UplinkAssert ( cluster->systems.ValidIndex ( authIndex ) );
        LanComputerSystem *auth = cluster->systems.GetData( authIndex );
        auth->data1 = lockIndex;

        cluster->AddLanLink ( hubIndex, 0.5, 1.0, lockIndex, 0.5, 0.0, 1 );
        cluster->AddLanLink ( hubIndex, 1.0, 0.5, authIndex, 0.5, 0.0, 1 );

        cluster->SetInput ( hubIndex );
        cluster->SetOutput ( lockIndex );

    }
    else if ( type == 3 ) {

        int hubIndex = GenerateHUB ( comp, cluster, x, y, 1 );
        int lockIndex = GenerateLock ( comp, cluster, x + width - 20, y + height - 40, 2 );
        int authIndex = GenerateAuthentication ( comp, cluster, x, y + height - 60, 2 );

		UplinkAssert ( cluster->systems.ValidIndex ( authIndex ) );
        LanComputerSystem *auth = cluster->systems.GetData( authIndex );
        auth->data1 = lockIndex;

        cluster->AddLanLink ( hubIndex, 1.0, 0.5, lockIndex, 0.5, 0.0, 1 );
        cluster->AddLanLink ( lockIndex, 0.0, 0.5, authIndex, 1.0, 0.5, 1 );
        cluster->AddLanLink ( hubIndex, 0.5, 1.0, authIndex, 0.5, 0.0, 1 );

        cluster->SetInput ( hubIndex );
        cluster->SetOutput ( lockIndex );

    }
    else if ( type == 4 ) {

        int hubIndex = GenerateHUB ( comp, cluster, x + width / 2, y, 1 );
        int lock1Index = GenerateLock ( comp, cluster, x + width - 20, y + height / 2, 1 );
        int lock2Index = GenerateLock ( comp, cluster, x + width / 2, y + height - 20, 2 );
        int isolIndex = GenerateIsolationBridge ( comp, cluster, x + width - 13, y + height - 27, 2 );
        int termIndex = GenerateTerminal ( comp, cluster, x, y + height / 2, 1 );

		UplinkAssert ( cluster->systems.ValidIndex ( isolIndex ) );
        LanComputerSystem *isol = cluster->systems.GetData( isolIndex );
        isol->data1 = lock1Index;
        isol->data2 = lock2Index;

		UplinkAssert ( cluster->systems.ValidIndex ( lock1Index ) );
        LanComputerSystem *lock1 = cluster->systems.GetData( lock1Index );
        lock1->data1 = 0;

        cluster->AddLanLink ( hubIndex, 1.0, 0.5, lock1Index, 0.5, 0.0, 1 );
        cluster->AddLanLink ( lock1Index, 0.5, 1.0, isolIndex, 0.5, 0.0, 1 );
        cluster->AddLanLink ( hubIndex, 0.0, 0.5, termIndex, 0.5, 0.0, 1 );
        cluster->AddLanLink ( termIndex, 0.5, 1.0, lock2Index, 0.0, 0.5, 1 );
        cluster->AddLanLink ( lock2Index, 1.0, 0.5, isolIndex, 0.0, 0.5, 1 );

        cluster->SetInput ( hubIndex );
        cluster->SetOutput ( lock2Index );

    }
    else if ( type == 5 ) {

        int hubIndex = GenerateHUB ( comp, cluster, x, y, 1 );
        int authIndex = GenerateAuthentication ( comp, cluster, x + width / 2, y, 2 );
        int lockIndex = GenerateLock ( comp, cluster, x + width - 20, y, 2 );
        int termIndex = GenerateTerminal ( comp, cluster, x, y + height - 40, 1 );

		UplinkAssert ( cluster->systems.ValidIndex ( authIndex ) );
        LanComputerSystem *auth = cluster->systems.GetData( authIndex );
        auth->data1 = lockIndex;

        cluster->AddLanLink ( hubIndex, 1.0, 0.5, authIndex, 0.0, 0.5, 1 );
        cluster->AddLanLink ( authIndex, 1.0, 0.5, lockIndex, 0.0, 0.5, 1 );
        cluster->AddLanLink ( hubIndex, 0.5, 1.0, termIndex, 0.5, 0.0, 1 );

        cluster->SetInput ( hubIndex );
        cluster->SetOutput ( lockIndex );

    }
    else if ( type == 6 ) {

        int hubIndex = GenerateHUB ( comp, cluster, x + width/2, y, 1 );
        int t1Index = GenerateTerminal ( comp, cluster, x, y + height / 3 * 1, 1 );
        int t2Index = GenerateTerminal ( comp, cluster, x, y + height / 3 * 2, 1 );
        int t3Index = GenerateTerminal ( comp, cluster, x, y + height / 3 * 3, 1 );
        int termIndex = GenerateTerminal ( comp, cluster, x + width - 40, y + height / 2, 1 );

		UplinkAssert ( cluster->systems.ValidIndex ( termIndex ) );
        LanComputerSystem *terminal = cluster->systems.GetData(termIndex);
        terminal->validSubnets.PutData( t1Index );
        terminal->validSubnets.PutData( t2Index );
        terminal->validSubnets.PutData( t3Index );

        cluster->AddLanLink ( hubIndex, 0.25, 1.0, t1Index, 1.0, 0.5, 1 );
        cluster->AddLanLink ( hubIndex, 0.5, 1.0, t2Index, 1.0, 0.5, 1 );
        cluster->AddLanLink ( hubIndex, 0.75, 1.0, t3Index, 1.0, 0.5, 1 );
        cluster->AddLanLink ( hubIndex, 1.0, 0.5, termIndex, 0.5, 0.0, 1 );

        cluster->SetInput ( hubIndex );
        cluster->SetOutput ( termIndex );

    }
    else if ( type == 7 ) {

        int lock1Index = GenerateLock ( comp, cluster, x, y, 1 );
        int isolIndex = GenerateIsolationBridge ( comp, cluster, x, y + height / 2, 2 );
        int lock2Index = GenerateLock ( comp, cluster, x + width - 20, y + height / 2, 2 );
        int modemIndex = GenerateModem ( comp, cluster, x + width/2, y + height - 40, 2 );

		UplinkAssert ( cluster->systems.ValidIndex ( isolIndex ) );
        LanComputerSystem *isol = cluster->systems.GetData( isolIndex );
        isol->data1 = lock1Index;
        isol->data2 = lock2Index;

		UplinkAssert ( cluster->systems.ValidIndex ( lock1Index ) );
        LanComputerSystem *lock1 = cluster->systems.GetData( lock1Index );
        lock1->data1 = 0;

        cluster->AddLanLink ( lock1Index, 0.5, 1.0, isolIndex, 0.5, 0.0, 1 );
        cluster->AddLanLink ( isolIndex, 1.0, 0.5, lock2Index, 0.0, 0.5, 1 );
        cluster->AddLanLink ( isolIndex, 0.5, 1.0, modemIndex, 0.0, 0.5, 1 );

        cluster->SetInput ( lock1Index );
        cluster->SetOutput ( lock2Index );

    }

}

void LanGenerator::GenerateLAN_Level1Cluster ( LanComputer *comp, LanCluster *cluster, LList <int> *usedRadioFrenquencies, int x, int y, int width, int height )
{

    cluster->centreX = x + width / 2;
    cluster->centreY = y + height / 2;

    int type = NumberGenerator::RandomNumber(6) + 1;

    int x1 = x + width / 4 * 1;
    int x2 = x + width / 4 * 2;
    int x3 = x + width / 4 * 3;

    int y1 = y + height / 4 * 1;
    int y2 = y + height / 4 * 2;
    int y3 = y + height / 4 * 3;

    if ( type == 1 ) {

        int hub1Index = GenerateHUB ( comp, cluster, x2, y, 1 );
        int hub2Index = GenerateHUB ( comp, cluster, x, y2, 1 );
        int authIndex = GenerateAuthentication ( comp, cluster, x2, y + height - 60, 2 );
        int lockIndex = GenerateLock ( comp, cluster, x + width - 20, y + height - 60, 2 );

        int t1Index = GenerateTerminal ( comp, cluster, x2, y2 - 70, 1 );
        int t2Index = GenerateTerminal ( comp, cluster, x2, y2 - 20, 1 );
        int t3Index = GenerateTerminal ( comp, cluster, x2, y2 + 30, 1 );

		UplinkAssert ( cluster->systems.ValidIndex ( authIndex ) );
        LanComputerSystem *auth = cluster->systems.GetData(authIndex);
        auth->validSubnets.PutData( t1Index );
        auth->validSubnets.PutData( t2Index );
        auth->validSubnets.PutData( t3Index );
        auth->data1 = lockIndex;

        cluster->AddLanLink ( hub1Index, 0.0, 0.5, hub2Index, 0.5, 0.0, 1 );
        cluster->AddLanLink ( hub2Index, 0.5, 1.0, authIndex, 0.0, 0.5, 1 );
        cluster->AddLanLink ( hub1Index, 1.0, 0.5, lockIndex, 0.5, 0.0, 1 );
        cluster->AddLanLink ( lockIndex, 0.0, 0.5, authIndex, 1.0, 0.5, 1 );

        cluster->AddLanLink ( hub2Index, 1.0, 0.25, t1Index, 0.0, 0.5, 1 );
        cluster->AddLanLink ( hub2Index, 1.0, 0.5, t2Index, 0.0, 0.5, 1 );
        cluster->AddLanLink ( hub2Index, 1.0, 0.75, t3Index, 0.0, 0.5, 1 );

        cluster->SetInput ( hub1Index );
        cluster->SetOutput ( lockIndex );

    }
    else if ( type == 2 ) {

        int hubIndex = GenerateHUB ( comp, cluster, x2, y, 1 );
        int lock1Index = GenerateLock ( comp, cluster, x, y2, 1 );
        int authIndex = GenerateAuthentication ( comp, cluster, x2, y2, 2 );
        int lock2Index = GenerateLock ( comp, cluster, x + width - 20, y + height - 40, 1 );
        int isoIndex = GenerateIsolationBridge ( comp, cluster, x2, y + height - 40, 2 );

		UplinkAssert ( cluster->systems.ValidIndex ( authIndex ) );
        LanComputerSystem *auth = cluster->systems.GetData( authIndex );
        auth->data1 = lock1Index;

		UplinkAssert ( cluster->systems.ValidIndex ( isoIndex ) );
        LanComputerSystem *iso = cluster->systems.GetData( isoIndex );
        iso->data1 = lock1Index;
        iso->data2 = lock2Index;

        cluster->AddLanLink ( hubIndex, 0.2f, 1.0f, lock1Index, 0.5f, 0.0f, 1 );
        cluster->AddLanLink ( hubIndex, 0.5f, 1.0f, authIndex, 0.5f, 0.0f, 1 );
        cluster->AddLanLink ( lock1Index, 0.5f, 1.0f, isoIndex, 0.0f, 0.5f, 1 );
        cluster->AddLanLink ( hubIndex, 1.0f, 0.5f, lock2Index, 0.5f, 0.0f, 1 );
        cluster->AddLanLink ( lock2Index, 0.0f, 0.5f, isoIndex, 1.0f, 0.5f, 1 );

        cluster->SetInput ( hubIndex );
        cluster->SetOutput ( lock2Index );

    }
    else if ( type == 3 ) {

        int hubIndex = GenerateHUB ( comp, cluster, x, y1, 1 );
        int auth1Index = GenerateAuthentication ( comp, cluster, x1, y1, 1 );
        int lock1Index = GenerateLock ( comp, cluster, x2, y1, 1 );
        int term1Index = GenerateTerminal ( comp, cluster, x3, y1, 1 );
        int term2Index = GenerateTerminal ( comp, cluster, x2, y, 1 );
        int auth2Index = GenerateAuthentication ( comp, cluster, x2, y2, 2 );
        int lock2Index = GenerateLock ( comp, cluster, x2, y3, 2 );
        int term3Index = GenerateTerminal ( comp, cluster, x1, y3, 2 );

		UplinkAssert ( cluster->systems.ValidIndex ( auth1Index ) );
        LanComputerSystem *auth1 = cluster->systems.GetData( auth1Index );
        auth1->data1 = lock1Index;

		UplinkAssert ( cluster->systems.ValidIndex ( auth2Index ) );
        LanComputerSystem *auth2 = cluster->systems.GetData( auth2Index );
        auth2->data1 = lock2Index;
        auth2->validSubnets.PutData( term1Index );

        cluster->AddLanLink ( hubIndex, 1.0, 0.5, auth1Index, 0.0, 0.5, 1 );
        cluster->AddLanLink ( auth1Index, 1.0, 0.5, lock1Index, 0.0, 0.5, 1 );
        cluster->AddLanLink ( lock1Index, 1.0, 0.5, term1Index, 0.0, 0.5, 1 );
        cluster->AddLanLink ( lock1Index, 0.5, 0.0, term2Index, 0.5, 1.0, 1 );
        cluster->AddLanLink ( lock1Index, 0.5, 1.0, auth2Index, 0.5, 0.0, 1 );
        cluster->AddLanLink ( auth2Index, 0.5, 1.0, lock2Index, 0.5, 0.0, 1 );
        cluster->AddLanLink ( lock2Index, 0.0, 0.5, term3Index, 1.0, 0.5, 1 );

        cluster->SetInput ( hubIndex );
        cluster->SetOutput ( lock2Index );

    }
    else if ( type == 4 ) {

        int hubIndex = GenerateHUB ( comp, cluster, x1, y, 1 );
        int termIndex = GenerateTerminal ( comp, cluster, x, y, 1 );
        int authIndex = GenerateAuthentication ( comp, cluster, x, y2, 2 );
        int lockIndex = GenerateLock ( comp, cluster, x1 + 20, y2, 2 );
        int transIndex = GenerateRadioTransmitter ( comp, cluster, x1, y3, 2 );
        int receiIndex = GenerateRadioReceiver ( comp, cluster, x3, y1, 3 );

		UplinkAssert ( cluster->systems.ValidIndex ( authIndex ) );
        LanComputerSystem *auth = cluster->systems.GetData( authIndex );
        auth->data1 = lockIndex;

		UplinkAssert ( cluster->systems.ValidIndex ( receiIndex ) );
        LanComputerSystem *recei = cluster->systems.GetData( receiIndex );
        recei->data1 = LanGenerator::GetRadioFrequency ( usedRadioFrenquencies );
        recei->data2 = NumberGenerator::RandomNumber ( 10 );

        UplinkAssert ( cluster->systems.ValidIndex ( termIndex ) );
        LanComputerSystem *term = cluster->systems.GetData( termIndex );
        char freqData [128];
        UplinkSnprintf ( freqData, sizeof ( freqData ), "Radio Receiver Frequency : %d.%d Ghz", recei->data1, recei->data2 );
        HideData ( comp, term, freqData );

        cluster->AddLanLink ( hubIndex, 0.0f, 0.5f, termIndex, 1.0f, 0.5f, 1 );
        cluster->AddLanLink ( hubIndex, 0.3f, 1.0f, authIndex, 0.5f, 0.0f, 1 );
        cluster->AddLanLink ( hubIndex, 0.7f, 1.0f, lockIndex, 0.5f, 0.0f, 1 );
        cluster->AddLanLink ( lockIndex, 0.5f, 1.0f, transIndex, 0.5f, 0.0f, 1 );

        cluster->SetInput ( hubIndex );
        cluster->SetOutput ( receiIndex );

    }
    else if ( type == 5 ) {

        int hub1Index = GenerateHUB ( comp, cluster, x, y, 1 );
        int transIndex = GenerateRadioTransmitter ( comp, cluster, x3, y, 1 );
        int authIndex = GenerateAuthentication ( comp, cluster, x, y2 + 40, 2 );
        int lockIndex = GenerateLock ( comp, cluster, x2, y2 + 40, 2 );
        int hub2Index = GenerateHUB ( comp, cluster, x2, y1, 3 );
        int t1Index = GenerateTerminal ( comp, cluster, x1, y2 - 80, 3 );
        int t2Index = GenerateTerminal ( comp, cluster, x1, y2 - 40, 3 );
        int t3Index = GenerateTerminal ( comp, cluster, x1, y2 + 0, 3 );
        int receiIndex = GenerateRadioReceiver ( comp, cluster, x2, y + height - 40, 3 );

        UplinkAssert ( cluster->systems.ValidIndex ( authIndex ) );
        LanComputerSystem *auth = cluster->systems.GetData( authIndex );
        auth->data1 = lockIndex;

        UplinkAssert ( cluster->systems.ValidIndex ( receiIndex ) );
        LanComputerSystem *recei = cluster->systems.GetData( receiIndex );
        recei->data1 = LanGenerator::GetRadioFrequency ( usedRadioFrenquencies );
        recei->data2 = NumberGenerator::RandomNumber ( 10 );

        char freqData [128];
        UplinkSnprintf ( freqData, sizeof ( freqData ), "Radio Receiver Frequency : %d.%d Ghz", recei->data1, recei->data2 );
        LanComputerSystem *term = NULL;
        switch ( NumberGenerator::RandomNumber(3) )
        {
            case 0 : UplinkAssert ( cluster->systems.ValidIndex ( t1Index ) ); term = cluster->systems.GetData( t1Index ); break;
            case 1 : UplinkAssert ( cluster->systems.ValidIndex ( t2Index ) ); term = cluster->systems.GetData( t2Index ); break;
            case 2 : UplinkAssert ( cluster->systems.ValidIndex ( t3Index ) ); term = cluster->systems.GetData( t3Index ); break;
            default: UplinkAssert ( cluster->systems.ValidIndex ( t3Index ) ); term = cluster->systems.GetData( t3Index );
        }
        HideData ( comp, term, freqData );

        cluster->AddLanLink ( hub1Index, 0.5, 1.0, authIndex, 0.5, 0.0, 1 );
        cluster->AddLanLink ( authIndex, 1.0, 0.5, lockIndex, 0.0, 0.5, 1 );
        cluster->AddLanLink ( hub1Index, 1.0, 0.5, transIndex, 0.0, 0.5, 1 );
        cluster->AddLanLink ( transIndex, 0.5, 1.0, lockIndex, 1.0, 0.5, 1 );
        cluster->AddLanLink ( lockIndex, 0.5, 0.0, hub2Index, 0.5, 1.0, 1 );
        cluster->AddLanLink ( hub2Index, 0.0, 0.25, t1Index, 1.0, 0.5, 1 );
        cluster->AddLanLink ( hub2Index, 0.0, 0.5, t2Index, 1.0, 0.5, 1 );
        cluster->AddLanLink ( hub2Index, 0.0, 0.75, t3Index, 1.0, 0.5, 1 );

        cluster->SetInput ( hub1Index );
        cluster->SetOutput ( receiIndex );

    }
    else if ( type == 6 ) {

        char phoneNum[32];
        UplinkStrncpy( phoneNum, "UnInitialised", sizeof ( phoneNum ) );

        int hubIndex = GenerateHUB ( comp, cluster, x, y, 1 );
        int term1Index = GenerateTerminal ( comp, cluster, x, y3, 1 );
        int authIndex = GenerateAuthentication ( comp, cluster, x2, y, 2 );
        int lockIndex = GenerateLock ( comp, cluster, x2, y2, 2 );
        int term2Index = GenerateTerminal ( comp, cluster, x2, y3, 2 );
        int modemIndex = GenerateModem ( comp, cluster, x3, y, 3, phoneNum, sizeof ( phoneNum ) );

        UplinkAssert ( cluster->systems.ValidIndex ( authIndex ) );
        LanComputerSystem *auth = cluster->systems.GetData( authIndex );
        auth->validSubnets.PutData ( term1Index );
        auth->data1 = lockIndex;

        UplinkAssert ( cluster->systems.ValidIndex ( term2Index ) );
        LanComputerSystem *term2 = cluster->systems.GetData( term2Index );
        char data [128];
        UplinkSnprintf ( data, sizeof ( data ), "Local Dial-up number : %s", phoneNum );
        HideData ( comp, term2, data );

        cluster->AddLanLink ( hubIndex, 0.25, 1.0, term1Index, 0.5, 0.0, 1 );
        cluster->AddLanLink ( hubIndex, 1.0, 0.5, authIndex, 0.0, 0.5, 1 );
        cluster->AddLanLink ( hubIndex, 0.5, 1.0, lockIndex, 0.5, 0.0, 1 );
        cluster->AddLanLink ( lockIndex, 0.5, 1.0, term2Index, 0.5, 0.0, 1 );

        cluster->SetInput ( hubIndex );
        cluster->SetOutput ( modemIndex );

    }

}

void LanGenerator::GenerateLAN_Level2Cluster ( LanComputer *comp, LanCluster *cluster, LList <int> *usedRadioFrenquencies )
{

    int type = NumberGenerator::RandomNumber(4) + 1;

    if ( type == 1 ) {

        // Left

        GenerateLAN_Level0Cluster ( comp, cluster, usedRadioFrenquencies, 0, 50, 200, 200 );
        int input1 = cluster->input;
        int output1 = cluster->output;
        int lockIndex = GenerateLock ( comp, cluster, 100, 350, 3 );
        int msIndex = GenerateMainServer ( comp, cluster, 200, 350, 3 );
        cluster->AddLanLink ( output1, 0.5, 1.0, lockIndex, 0.5, 0.0, 1 );
        cluster->AddLanLink ( lockIndex, 1.0, 0.5, msIndex, 0.0, 0.5, 1 );

        // Right

        GenerateLAN_Level1Cluster ( comp, cluster, usedRadioFrenquencies, 250, 50, 275, 275 );
        int input2 = cluster->input;
        int output2 = cluster->output;
        int authIndex = GenerateAuthentication ( comp, cluster, 600, 200, 3 );
        cluster->AddLanLink ( output2, 1.0, 0.5, authIndex, 0.0, 0.5, 1 );

        // Remaining

        int hubIndex = GenerateHUB ( comp, cluster, 200, 0, 1 );
        cluster->AddLanLink ( hubIndex, 0.0, 0.5, input1, 0.5, 0.0, 1 );
        cluster->AddLanLink ( hubIndex, 1.0, 0.5, input2, 0.5, 0.0, 1 );

        UplinkAssert ( cluster->systems.ValidIndex ( authIndex ) );
        LanComputerSystem *auth = cluster->systems.GetData( authIndex );
        auth->data1 = lockIndex;

        cluster->centreX = 600 / 2;
        cluster->centreY = 300 / 2;

        cluster->SetInput ( hubIndex );
        cluster->SetOutput ( msIndex );

    }
    else if ( type == 2 ) {

        GenerateLAN_Level1Cluster ( comp, cluster, usedRadioFrenquencies, 0, 0, 275, 275 );
        int input1 = cluster->input;
        int output1 = cluster->output;

        LanCluster *cluster1 = new LanCluster ();
        GenerateLAN_Level0Cluster ( comp, cluster1, usedRadioFrenquencies, 325, 0, 200, 200 );
        cluster1->Rotate ( 3 );
        cluster1->Merge ( cluster, comp );
        int input2 = cluster1->input;
        int output2 = cluster1->output;
		delete cluster1;

        int msIndex = GenerateMainServer ( comp, cluster, 600, 0, 3 );

        cluster->AddLanLink ( output1, 1.0, 0.5, input2, 0.0, 0.5, 1 );
        cluster->AddLanLink ( output2, 1.0, 0.5, msIndex, 0.0, 0.5, 1 );

        cluster->centreX = 700 / 2;
        cluster->centreY = 300 / 2;

        cluster->SetInput ( input1 );
        cluster->SetOutput ( output2 );

    }
    else if ( type == 3 || type == 4 ) {

        GenerateLAN_Level0Cluster ( comp, cluster, usedRadioFrenquencies, 0, 150, 200, 200 );
        int input1 = cluster->input;
        int output1 = cluster->output;
        int l1Index = GenerateLock ( comp, cluster, 150, 400, 3 );
        cluster->AddLanLink ( output1, 0.5, 1.0, l1Index, 0.5, 0.0, 1 );

        GenerateLAN_Level0Cluster ( comp, cluster, usedRadioFrenquencies, 250, 150, 200, 200 );
        int input2 = cluster->input;
        int output2 = cluster->output;
        int l2Index = GenerateLock ( comp, cluster, 400, 400, 3 );
        cluster->AddLanLink ( output2, 0.5, 1.0, l2Index, 0.5, 0.0, 1 );

        GenerateLAN_Level0Cluster ( comp, cluster, usedRadioFrenquencies, 500, 150, 200, 200 );
        int input3 = cluster->input;
        int output3 = cluster->output;
        int l3Index = GenerateLock ( comp, cluster, 650, 400, 3 );
        cluster->AddLanLink ( output3, 0.5, 1.0, l3Index, 0.5, 0.0, 1 );


        int hubIndex = GenerateHUB ( comp, cluster, 350, 0, 1 );
        int msIndex = GenerateMainServer ( comp, cluster, 450, 0, 1 );

        UplinkAssert ( cluster->systems.ValidIndex ( msIndex ) );
        LanComputerSystem *ms = cluster->systems.GetData( msIndex );
        ms->data1 = l1Index;
        ms->data2 = l2Index;
        ms->data3 = l3Index;

        cluster->AddLanLink ( hubIndex, 0.1f, 1.0f, input1, 0.5f, 0.0f, 1 );
        cluster->AddLanLink ( hubIndex, 0.3f, 1.0f, input2, 0.5f, 0.0f, 1 );
        cluster->AddLanLink ( hubIndex, 0.5f, 1.0f, input3, 0.5f, 0.0f, 1 );
        cluster->AddLanLink ( hubIndex, 1.0f, 0.5f, msIndex, 0.0f, 0.5f, 1 );

        cluster->centreX = 350;
        cluster->centreY = 400 / 2;

        cluster->SetInput ( hubIndex );
        cluster->SetOutput ( msIndex );

    }

}

void LanGenerator::GenerateLAN_Level3Cluster ( LanComputer *comp, LanCluster *cluster, LList <int> *usedRadioFrenquencies )
{
    // TODO : Fill this in
    GenerateLAN_Level4Cluster ( comp, cluster, usedRadioFrenquencies );

}

void LanGenerator::GenerateLAN_Level4Cluster ( LanComputer *comp, LanCluster *cluster, LList <int> *usedRadioFrenquencies )
{

    int type = NumberGenerator::RandomNumber( 4 ) + 1;

    if ( type == 1 ) {

        // Part 1

        GenerateLAN_Level1Cluster ( comp, cluster, usedRadioFrenquencies, 0, 0, 275, 275 );
        int input1 = cluster->input;
        int output1 = cluster->output;

        // Part 2

        LanCluster *part2 = new LanCluster ();
        GenerateLAN_Level0Cluster ( comp, part2, usedRadioFrenquencies, 325, 0, 200, 200 );
        part2->Rotate( 3 );
        part2->Merge( cluster, comp );
        int input2 = part2->input;
        int output2 = part2->output;
		delete part2;

        // Part 3

        GenerateLAN_Level0Cluster ( comp, cluster, usedRadioFrenquencies, 550, 200, 200, 200 );
        int input3 = cluster->input;
        int output3 = cluster->output;

        // Part 4

        LanCluster *part4 = new LanCluster ();
        GenerateLAN_Level1Cluster ( comp, part4, usedRadioFrenquencies, 800, 350, 275, 275 );
        part4->Rotate( 3 );
        part4->Merge( cluster, comp );
        int input4 = part4->input;
        int output4 = part4->output;
		delete part4;

        // Main Server

        int msIndex = GenerateMainServer ( comp, cluster, 1100, 150, 3 );

        cluster->AddLanLink ( output1, 1.0, 0.5, input2, 0.0, 0.5, 1 );
        cluster->AddLanLink ( output2, 1.0, 0.5, input3, 0.5, 0.0, 1 );
        cluster->AddLanLink ( output3, 0.5, 1.0, input4, 0.0, 0.5, 1 );
        cluster->AddLanLink ( output4, 1.0, 0.5, msIndex, 0.5, 1.0, 1 );

        cluster->centreX = 1200 / 2;
        cluster->centreY = 600 / 2;

        cluster->SetInput ( input1 );
        cluster->SetOutput ( output4 );

    }
    else if ( type == 2 ) {

        // Left system

        GenerateLAN_Level1Cluster ( comp, cluster, usedRadioFrenquencies, 0, 50, 275, 275 );
        int input1 = cluster->input;
        int output1 = cluster->output;
        int termIndex = GenerateTerminal ( comp, cluster, 275, 400, 3 );
        cluster->AddLanLink ( output1, 0.5, 1.0, termIndex, 0.5, 0.0, 1 );

        // Central system

        GenerateLAN_Level1Cluster ( comp, cluster, usedRadioFrenquencies, 330, 300, 300, 300 );
        int input2 = cluster->input;
        int output2 = cluster->output;
        char phoneNum[64];
        UplinkStrncpy( phoneNum, "UnInitialised", sizeof ( phoneNum ) );
        int modemIndex = GenerateModem ( comp, cluster, 480, 100, 3, phoneNum, sizeof ( phoneNum ) );
        int lockIndex = GenerateLock ( comp, cluster, 480, 200, 3 );
        cluster->AddLanLink ( modemIndex, 0.5, 1.0, lockIndex, 0.5, 0.0, 1 );
        cluster->AddLanLink ( lockIndex, 0.5, 1.0, input2, 0.5, 0.0, 1 );

        int msIndex = GenerateMainServer ( comp, cluster, 630, 650, 3 );
        cluster->AddLanLink ( output2, 0.5, 1.0, msIndex, 0.5, 0.0, 1 );

        // Right system

        GenerateLAN_Level1Cluster ( comp, cluster, usedRadioFrenquencies, 660, 50, 275, 275 );
        int input3 = cluster->input;
        int output3 = cluster->output;
        int authIndex = GenerateAuthentication ( comp, cluster, 960, 400, 3 );
        cluster->AddLanLink ( output3, 0.5, 1.0, authIndex, 0.5, 0.0, 1 );

        // Hub system

        int hubIndex = GenerateHUB ( comp, cluster, 480, 0, 1 );
        cluster->AddLanLink ( hubIndex, 0.0, 0.5, input1, 0.5, 0.0, 1 );
        cluster->AddLanLink ( hubIndex, 1.0, 0.5, input3, 0.5, 0.0, 1 );

        // Remaining

        UplinkAssert ( cluster->systems.ValidIndex ( authIndex ) );
        LanComputerSystem *auth = cluster->systems.GetData( authIndex );
        auth->data1 = lockIndex;

        UplinkAssert ( cluster->systems.ValidIndex ( termIndex ) );
        LanComputerSystem *term = cluster->systems.GetData( termIndex );
        char data[256];
        UplinkSnprintf ( data, sizeof ( data ), "Local Dial up number: %s", phoneNum );
        HideData ( comp, term, data );

        cluster->centreX = 1000 / 2;
        cluster->centreY = 700 / 2;

        cluster->SetInput ( hubIndex );
        cluster->SetOutput ( msIndex );

    }
    if ( type == 3 ) {

        // Left authentication

        LanCluster *leftCluster = new LanCluster ();
        int rec1Index = GenerateRadioReceiver ( comp, leftCluster, 100, 0, 1 );
        GenerateLAN_Level1Cluster ( comp, leftCluster, usedRadioFrenquencies, 0, 100, 300, 300 );
        int auth1Index = GenerateAuthentication ( comp, leftCluster, 350, 400, 1 );
        leftCluster->AddLanLink ( rec1Index, 0.5, 1.0, leftCluster->input, 0.5, 0.0, 1 );
        leftCluster->AddLanLink ( leftCluster->output, 1.0, 0.5, auth1Index, 0.0, 0.5, 1 );
        leftCluster->SetOutput( auth1Index );
        UplinkAssert ( leftCluster->systems.ValidIndex ( rec1Index ) );
        LanComputerSystem *recei1 = leftCluster->systems.GetData( rec1Index );
        recei1->data1 = LanGenerator::GetRadioFrequency ( usedRadioFrenquencies );
        recei1->data2 = NumberGenerator::RandomNumber ( 10 );
        leftCluster->Rotate( 1 );
        leftCluster->Merge( cluster, comp );
        int output1 = leftCluster->output;
		delete leftCluster;

        // Right authentication

        LanCluster *rightCluster = new LanCluster ();
        int rec2Index = GenerateRadioReceiver ( comp, rightCluster, 1000, 0, 1 );
        GenerateLAN_Level1Cluster ( comp, rightCluster, usedRadioFrenquencies, 900, 60, 300, 300 );
        int auth2Index = GenerateAuthentication ( comp, rightCluster, 1250, 400, 1 );
        rightCluster->AddLanLink ( rec2Index, 0.5, 1.0, rightCluster->input, 0.5, 0.0, 1 );
        rightCluster->AddLanLink ( rightCluster->output, 1.0, 0.5, auth2Index, 0.0, 0.5, 1 );
        rightCluster->SetOutput ( auth2Index );
        UplinkAssert ( rightCluster->systems.ValidIndex ( rec2Index ) );
        LanComputerSystem *recei2 = rightCluster->systems.GetData( rec2Index );
        recei2->data1 = LanGenerator::GetRadioFrequency ( usedRadioFrenquencies );
        recei2->data2 = NumberGenerator::RandomNumber ( 10 );
        rightCluster->Rotate( 3 );
        rightCluster->Merge( cluster, comp );
        int output2 = rightCluster->output;
		delete rightCluster;

        // Lower authentication

        LanCluster *lowerCluster = new LanCluster ();
        int rec3Index = GenerateRadioReceiver ( comp, lowerCluster, 575, 400, 1 );
        GenerateLAN_Level1Cluster ( comp, lowerCluster, usedRadioFrenquencies, 460, 500, 300, 300 );
        int auth3Index = GenerateAuthentication ( comp, lowerCluster, 800, 900, 1 );
        lowerCluster->AddLanLink ( rec3Index, 0.5, 1.0, lowerCluster->input, 0.5, 0.0, 1 );
        lowerCluster->AddLanLink ( lowerCluster->output, 1.0, 0.5, auth3Index, 0.0, 0.5, 1 );
        lowerCluster->SetOutput ( auth3Index );
        UplinkAssert ( lowerCluster->systems.ValidIndex ( rec3Index ) );
        LanComputerSystem *recei3 = lowerCluster->systems.GetData( rec3Index );
        recei3->data1 = LanGenerator::GetRadioFrequency ( usedRadioFrenquencies );
        recei3->data2 = NumberGenerator::RandomNumber ( 10 );
        lowerCluster->Merge( cluster, comp );
        int output3 = lowerCluster->output;
		delete lowerCluster;

        // Central bit

        int x1 = 460;
        int y1 = 0;
        int w = 300;
        int h = 300;

        int hubIndex = GenerateHUB ( comp, cluster, x1 + w / 2, y1, 1 );
        int t1Index = GenerateTerminal ( comp, cluster, x1, y1 + h / 2, 1 );
        int t2Index = GenerateTerminal ( comp, cluster, x1 + w, y1 + h / 2, 1 );
        int transIndex = GenerateRadioTransmitter ( comp, cluster, x1 + w / 2, y1 + h, 1 );
        int l1Index = GenerateLock ( comp, cluster, x1 + w / 4 - 20, y1 + h / 3, 1 );
        int l2Index = GenerateLock ( comp, cluster, x1 + w - 40, y1 + h / 3, 1 );
        int l3Index = GenerateLock ( comp, cluster, x1 + w / 2, y1 + h - 60, 1 );
        int msIndex = GenerateMainServer ( comp, cluster, x1 + w / 2, y1 + h / 2, 1 );

        UplinkAssert ( cluster->systems.ValidIndex ( msIndex ) );
        LanComputerSystem *ms = cluster->systems.GetData( msIndex );
        ms->data1 = l1Index;
        ms->data2 = l2Index;
        ms->data3 = l3Index;

        UplinkAssert ( cluster->systems.ValidIndex ( output1 ) );
        LanComputerSystem *auth1 = cluster->systems.GetData( output1 );
        auth1->data1 = l1Index;

        UplinkAssert ( cluster->systems.ValidIndex ( output2 ) );
        LanComputerSystem *auth2 = cluster->systems.GetData( output2 );
        auth2->data1 = l2Index;

        UplinkAssert ( cluster->systems.ValidIndex ( output3 ) );
        LanComputerSystem *auth3 = cluster->systems.GetData( output3 );
        auth3->data1 = l3Index;

        UplinkAssert ( cluster->systems.ValidIndex ( t1Index ) );
        LanComputerSystem *t1 = cluster->systems.GetData( t1Index );
        char data [512];
        UplinkSnprintf ( data, sizeof ( data ), "Known Radio Receiver Frequencies :\n\n"
												"Receiver A : %d.%d Ghz\n"
												"Receiver B : %d.%d Ghz\n"
												"Receiver C : %d.%d Ghz",
												recei1->data1, recei1->data2,
												recei2->data1, recei2->data2,
												recei3->data1, recei3->data2 );
        HideData ( comp, t1, data );

        cluster->AddLanLink ( hubIndex, 0.0f, 0.3f, t1Index, 0.5f, 0.0f, 1 );
        cluster->AddLanLink ( t1Index, 0.5f, 1.0f, transIndex, 0.0f, 0.5f, 1 );
        cluster->AddLanLink ( hubIndex, 1.0f, 0.3f, t2Index, 0.5f, 0.0f, 1 );
        cluster->AddLanLink ( t2Index, 0.5f, 1.0f, transIndex, 1.0f, 0.5f, 1 );

        cluster->AddLanLink ( hubIndex, 0.0f, 0.7f, l1Index, 0.5f, 0.0f, 1 );
        cluster->AddLanLink ( hubIndex, 1.0f, 0.7f, l2Index, 0.5f, 0.0f, 1 );
        cluster->AddLanLink ( hubIndex, 0.5f, 1.0f, msIndex, 0.5f, 0.0f, 1 );
        cluster->AddLanLink ( l1Index, 1.0f, 0.5f, msIndex, 0.0f, 0.5f, 1 );
        cluster->AddLanLink ( l2Index, 0.0f, 0.5f, msIndex, 1.0f, 0.5f, 1 );
        cluster->AddLanLink ( transIndex, 0.5f, 0.0f, l3Index, 0.5f, 1.0f, 1 );
        cluster->AddLanLink ( l3Index, 0.5f, 0.0f, msIndex, 0.5f, 1.0f, 1 );

        cluster->SetInput ( hubIndex );
        cluster->SetOutput ( transIndex );

        cluster->centreX = x1 + w / 2;
        cluster->centreY = y1 + h / 2;

    }
    else if ( type == 4 ) {

        GenerateLAN_Level0Cluster ( comp, cluster, usedRadioFrenquencies, 0, 0, 200, 200 );
        int input1 = cluster->input;
        int output1 = cluster->output;

        GenerateLAN_Level1Cluster ( comp, cluster, usedRadioFrenquencies, 0, 275, 300, 300 );
        int input2 = cluster->input;
        int output2 = cluster->output;

        LanCluster *part3 = new LanCluster ();
        GenerateLAN_Level1Cluster ( comp, part3, usedRadioFrenquencies, 340, 275, 300, 300 );
        part3->Rotate( 3 );
        part3->Merge( cluster, comp );
        int input3 = part3->input;
        int output3 = part3->output;
		delete part3;

        int msIndex = GenerateMainServer ( comp, cluster, 500, 150, 3 );

        cluster->AddLanLink ( output1, 0.5, 1.0, input2, 0.5, 0.0, 1 );
        cluster->AddLanLink ( output2, 1.0, 0.5, input3, 0.0, 0.5, 1 );
        cluster->AddLanLink ( output3, 0.5, 0.0, msIndex, 0.5, 1.0, 1 );

        cluster->SetInput ( input1 );
        cluster->SetOutput ( msIndex );

        cluster->centreX = 640 / 2;
        cluster->centreY = 540 / 2;

    }

}

void LanGenerator::GenerateLAN_BoundingBox ( LanComputer *comp, LanCluster *cluster, int x, int y, int width, int height )
{

#ifdef TESTGAME

    int l1Index = GenerateLock ( comp, cluster, x - 16, y - 16, 1 );
    int l2Index = GenerateLock ( comp, cluster, x + width - 16, y - 16, 1 );
    int l3Index = GenerateLock ( comp, cluster, x + width - 16, y + height - 16, 1 );
    int l4Index = GenerateLock ( comp, cluster, x - 16, y + height, 1 );

    cluster->AddLanLink ( l1Index, 1.0, 0.5, l2Index, 0.0, 0.5, 1 );
    cluster->AddLanLink ( l2Index, 0.5, 1.0, l3Index, 0.5, 0.0, 1 );
    cluster->AddLanLink ( l3Index, 0.0, 0.5, l4Index, 1.0, 0.5, 1 );
    cluster->AddLanLink ( l4Index, 0.5, 0.0, l1Index, 0.5, 1.0, 1 );

#endif

}

int LanGenerator::GenerateRouter( LanComputer *comp, LanCluster *cluster, int x, int y, int security )
{

	UplinkAssert (comp);
    UplinkAssert (cluster);

	//
	// Create the screen

	MessageScreen *ms = new MessageScreen ();
	ms->SetMainTitle ( comp->companyname );
	ms->SetSubTitle ( "Local Area Network" );
	ms->SetTextMessage ( "EXTERNAL ROUTER ACCESSED\n\n"
						 "You have accessed the Local Area Network of our company.\n"
                         "This system is not open to external connections from the Net.\n\n"
                         "If you reached this page in error, please disconnect now.\n\n\n"
                         "Your IP has been logged.\n"
                         "If you continue to connect to this private network\n"
                         "our system administrators will begin tracing you,\n"
                         "which could result in legal action." );

	int screenIndex = comp->AddComputerScreen ( ms );

	//
	// Add the LAN system

	int systemIndex = cluster->AddLanSystem( LANSYSTEM_ROUTER, x, y, security, screenIndex, screenIndex );

	return systemIndex;

}

int LanGenerator::GenerateHUB ( LanComputer *comp, LanCluster *cluster, int x, int y, int security )
{

	UplinkAssert (comp);
    UplinkAssert (cluster);

	//
	// Create the screen

	MessageScreen *ms = new MessageScreen ();
	ms->SetMainTitle ( comp->companyname );
	ms->SetSubTitle ( "Local Area Network" );
	ms->SetTextMessage ( "INTERNAL HUB ACCESSED\n\n"
						 "HUB Internal Operating system Active\n"
                         "Incoming connections Active\n"
                         "Outgoing connections Active\n\n"
                         "Maximum HUB bandwidth : 1000 Gq/s\n"
                         "Peak latency : 10ms\n"
                         "Packet collisions detected : None\n\n"
                         "All systems operational" );

	int screenIndex = comp->AddComputerScreen ( ms );

	//
	// Add the LAN system

	int systemIndex = cluster->AddLanSystem( LANSYSTEM_HUB, x, y, security, screenIndex, screenIndex );

	return systemIndex;

}

int LanGenerator::GenerateTerminal ( LanComputer *comp, LanCluster *cluster, int x, int y, int security )
{

	UplinkAssert (comp);
    UplinkAssert (cluster);

	// Opening message screen

	MessageScreen *ms = new MessageScreen ();
	ms->SetMainTitle ( comp->companyname );
	ms->SetSubTitle ( "Local Area Network" );
	ms->SetTextMessage ( "TERMINAL ACCESSED\n\n"
						 "Password verification will be required\n"
						 "for further systems access.  All activity through these\n"
                         "terminals is logged and analysed on a daily basis.\n\n"
                         "If you are not a registered employee of this company\n"
                         "then you must disconnect now, or legal charges may be\n"
                         "brought against you." );
	ms->SetButtonMessage( "OK" );

	// Password screen

	PasswordScreen *ps = new PasswordScreen ();
	ps->SetMainTitle ( "LAN Terminal" );
	ps->SetSubTitle ( "Password verification required" );
    ps->SetDifficulty ( HACKDIFFICULTY_LANTERMINAL );

	if		( security == 1 )	ps->SetPassword ( NameGenerator::GenerateEasyPassword() );
	else if ( security == 2 )	ps->SetPassword ( NameGenerator::GeneratePassword() );
	else						ps->SetPassword ( NameGenerator::GenerateComplexPassword() );

	// You're in screen

	MessageScreen *ms2 = new MessageScreen();
	ms2->SetMainTitle ( comp->companyname );
	ms2->SetSubTitle ( "ACCESS GRANTED" );
	ms2->SetTextMessage ( "All security verification checks out OK.\n"
                          "You are now authorised to use this Terminal system.\n\n"
                          "Please respect the wishes and needs of other users,\n"
                          "and report any suspected illegal activity to the\n"
                          "system administrator." );


	int msIndex = comp->AddComputerScreen ( ms );
	int psIndex = comp->AddComputerScreen ( ps );
	int ms2Index = comp->AddComputerScreen( ms2 );

	ms->SetNextPage ( psIndex );
	ps->SetNextPage ( ms2Index );

	//
	// Add the LAN system

	int systemIndex = cluster->AddLanSystem( LANSYSTEM_TERMINAL, x, y, security, msIndex, ms2Index );

	return systemIndex;

}

int LanGenerator::GenerateLock ( LanComputer *comp, LanCluster *cluster, int x, int y, int security )
{

	UplinkAssert (comp);
    UplinkAssert (cluster);

	//
	// Create the screen

	MessageScreen *ms = new MessageScreen ();
	ms->SetMainTitle ( comp->companyname );
	ms->SetSubTitle ( "Local Area Network" );
	ms->SetTextMessage ( "LOCK ACCESSED\n\n"
						 "Full systems check: OK\n"
                         "Maximum simultanious clients: 16\n"
                         "Maximum available bandwidth: 200 Gq/s"
                         "Security checks valid\n\n"
                         "This lock is DISABLED" );

	int screenIndex = comp->AddComputerScreen ( ms );

	//
	// Add the LAN system

	int systemIndex = cluster->AddLanSystem( LANSYSTEM_LOCK, x, y, security, screenIndex, screenIndex );

    UplinkAssert ( cluster->systems.ValidIndex ( systemIndex ) );
	LanComputerSystem *lock = cluster->systems.GetData(systemIndex);
	UplinkAssert (lock);
	lock->data1 = 1;

	return systemIndex;

}

int LanGenerator::GenerateAuthentication ( LanComputer *comp, LanCluster *cluster, int x, int y, int security )
{

	UplinkAssert (comp);
    UplinkAssert (cluster);

	//
	// Opening message screen

	MessageScreen *ms = new MessageScreen ();
	ms->SetMainTitle ( comp->companyname );
	ms->SetSubTitle ( "Local Area Network" );
	ms->SetTextMessage ( "AUTHENTICATION SERVER ACCESSED\n\n"
						 "Authentication Server dedicated server software running\n"
                         "One user currently detected online\n"
                         "Incoming ports check OK\n"
                         "Outgoing ports check OK\n"
                         "Target Lock systems check OK\n\n"
                         "If you are not the system administrator you must disconnect now\n"
                         "or you will be punished to the full extent of the law.\n"
                         "All access to this system is logged and checked on a daily basis." );
	ms->SetButtonMessage ( "OK" );
	int msIndex = comp->AddComputerScreen( ms );

	// Password Screen

	PasswordScreen *ps = new PasswordScreen ();
	ps->SetMainTitle ( "Authentication Server" );
	ps->SetSubTitle ( "Password verification required" );
	ps->SetDifficulty ( HACKDIFFICULTY_LANAUTHENTICATIONSERVER );

	if		( security == 1 )	ps->SetPassword ( NameGenerator::GenerateEasyPassword() );
	else if ( security == 2 )	ps->SetPassword ( NameGenerator::GeneratePassword() );
	else						ps->SetPassword ( NameGenerator::GenerateComplexPassword() );
	int psIndex = comp->AddComputerScreen ( ps );

	// Cypher screen

	CypherScreen *cs = new CypherScreen ();
	cs->SetMainTitle ( "Authentication Server" );
	cs->SetSubTitle ( "Enter elliptic-curve encryption cypher" );
	cs->SetDifficulty ( HACKDIFFICULTY_GLOBALCRIMINALDATABASE );
	int csIndex = comp->AddComputerScreen ( cs );

	// High Security Screen

	HighSecurityScreen *hs = new HighSecurityScreen ();
	hs->SetMainTitle ( "Authentication Server" );
	hs->SetSubTitle ( "Authorisation required" );
	hs->AddSystem ( "Password verification", psIndex );
	hs->AddSystem ( "Elliptic-Curve Encryption Cypher", csIndex );
	int hsIndex = comp->AddComputerScreen ( hs );

	ms->SetNextPage ( hsIndex );
	ps->SetNextPage ( hsIndex );
	cs->SetNextPage ( hsIndex );

	// All done screen

	DialogScreen *ds = new DialogScreen ();
	int dsIndex = comp->AddComputerScreen( ds );
	ds->SetMainTitle ( comp->companyname );
	ds->SetSubTitle ( "Authentication Server" );
	ds->AddWidget( "text", WIDGET_CAPTION, 50, 100, 200, 100, "Access to authentication Server systems granted.\n"
														 "Please proceed with caution.", " " );
	ds->AddWidget( "enable", WIDGET_SCRIPTBUTTON, 50, 200, 150, 20, "Enable Security Locks",
							"Enable the locks on this Local Area Network", 80, dsIndex, NULL, NULL );
	ds->AddWidget( "disable", WIDGET_SCRIPTBUTTON, 50, 240, 150, 20, "Disable security locks",
							"Disable the locks on this Local Area Network", 81, dsIndex, NULL, NULL );

	hs->SetNextPage( dsIndex );


    // Generate the system

	int systemIndex = cluster->AddLanSystem( LANSYSTEM_AUTHENTICATION, x, y, security, msIndex );
	return systemIndex;

}

int LanGenerator::GenerateMainServer ( LanComputer *comp, LanCluster *cluster, int x, int y, int security )
{

	UplinkAssert (comp);
    UplinkAssert (cluster);

	//
	// Opening message screen

	MessageScreen *ms = new MessageScreen ();
	ms->SetMainTitle ( comp->companyname );
	ms->SetSubTitle ( "Local Area Network" );
	ms->SetTextMessage ( "MAIN SERVER ACCESSED\n\n"
						 "External connection to Main Server granted.\n"
                         "This connection is being logged.\n\n"
                         "If you are not authorised to have access to this system\n"
                         "then you will be caught and prosecuted under the full force\n"
                         "of the International Data Security Treaty, 2006.\n\n" );
	ms->SetButtonMessage ( "OK" );
	int msIndex = comp->AddComputerScreen( ms );

	// Password Screen

	PasswordScreen *ps = new PasswordScreen ();
	ps->SetMainTitle ( "Main Server" );
	ps->SetSubTitle ( "Password verification required" );
    ps->SetDifficulty ( HACKDIFFICULTY_LANMAINSERVER );

	if		( security == 1 )	ps->SetPassword ( NameGenerator::GenerateEasyPassword() );
	else if ( security == 2 )	ps->SetPassword ( NameGenerator::GeneratePassword() );
	else						ps->SetPassword ( NameGenerator::GenerateComplexPassword() );
	int psIndex = comp->AddComputerScreen ( ps );


	// Voice print screen

	GenericScreen *gs = new GenericScreen ();
	gs->SetMainTitle ( "Main Server" );
	gs->SetSubTitle ( "Voice print analysis required" );
	gs->SetScreenType ( SCREEN_VOICEANALYSIS );
	int gsIndex = comp->AddComputerScreen ( gs );

	// High Security Screen

	HighSecurityScreen *hs = new HighSecurityScreen ();
	hs->SetMainTitle ( "Authentication Server" );
	hs->SetSubTitle ( "Authorisation required" );
	hs->AddSystem ( "UserID / password verification", psIndex );
	hs->AddSystem ( "Voice Print Analysis", gsIndex );
	int hsIndex = comp->AddComputerScreen ( hs );

	ms->SetNextPage ( hsIndex );
	ps->SetNextPage ( hsIndex );
	gs->SetNextPage ( hsIndex );

	// File server

	GenericScreen *fss = new GenericScreen ();
	fss->SetScreenType ( SCREEN_FILESERVERSCREEN );
	fss->SetMainTitle ( "Main Server" );
	fss->SetSubTitle ( "File server" );
	int fssIndex = comp->AddComputerScreen ( fss );

    // Log screen

	LogScreen *ls = new LogScreen ();
	ls->SetTARGET ( LOGSCREEN_TARGET_ACCESSLOGS );
	ls->SetMainTitle ( "Log Server" );
	ls->SetSubTitle ( "Access Logs" );
	int logIndex = comp->AddComputerScreen ( ls );

	// Console

	GenericScreen *gs6 = new GenericScreen ();
	gs6->SetScreenType ( SCREEN_CONSOLESCREEN );
	gs6->SetMainTitle ( "Main Server" );
	gs6->SetSubTitle ( "Console" );
	int gs6Index = comp->AddComputerScreen ( gs6 );

	// Menu

	MenuScreen *menu = new MenuScreen ();
	menu->SetMainTitle ( "Main Server" );
	menu->SetSubTitle ( "Main Menu" );
	menu->AddOption ( "File Server", "Access the file server", fssIndex, 3 );
    menu->AddOption ( "Log Server", "Access logs on this system", logIndex, 1 );
	menu->AddOption ( "Console", "Use a console on this system", gs6Index, 1 );
	int menuIndex = comp->AddComputerScreen ( menu );

    hs->SetNextPage( menuIndex );
	gs6->SetNextPage( menuIndex );
	fss->SetNextPage( menuIndex );
    ls->SetNextPage( menuIndex );

    // Generate the system

	int systemIndex = cluster->AddLanSystem( LANSYSTEM_MAINSERVER, x, y, security, msIndex );
	return systemIndex;

}

int LanGenerator::GenerateLogServer ( LanComputer *comp, LanCluster *cluster, int x, int y, int security )
{

    UplinkAssert ( comp );
    UplinkAssert ( cluster );

    // Message screen

	MessageScreen *ms = new MessageScreen ();
	ms->SetMainTitle ( comp->companyname );
	ms->SetSubTitle ( "Log Server" );
	ms->SetTextMessage ( "External access to LOG server granted.\n\n"
                         "Checking Operating System : OK\n"
                         "Checking File system : OK\n"
                         "Checking LOG integrity : OK\n\n"
                         "Warning: Only the Sys admin has access to this system.\n"
                         "All attempted hacks will be logged ;)" );
	ms->SetButtonMessage ( "OK" );
	int msIndex = comp->AddComputerScreen( ms );

	// Password Screen

	PasswordScreen *ps = new PasswordScreen ();
	ps->SetMainTitle ( "Log Server" );
	ps->SetSubTitle ( "Password verification required" );
    ps->SetDifficulty ( HACKDIFFICULTY_LANLOGSERVER );

	if		( security == 1 )	ps->SetPassword ( NameGenerator::GenerateEasyPassword() );
	else if ( security == 2 )	ps->SetPassword ( NameGenerator::GeneratePassword() );
	else						ps->SetPassword ( NameGenerator::GenerateComplexPassword() );
	int psIndex = comp->AddComputerScreen ( ps );


    // Log screen

	LogScreen *ls = new LogScreen ();
	ls->SetTARGET ( LOGSCREEN_TARGET_ACCESSLOGS );
	ls->SetMainTitle ( "Log Server" );
	ls->SetSubTitle ( "Access Logs" );
	int lsIndex = comp->AddComputerScreen ( ls );

    ms->SetNextPage( psIndex );
    ps->SetNextPage( lsIndex );

    int systemIndex = cluster->AddLanSystem( LANSYSTEM_LOGSERVER, x, y, security, msIndex );
    return systemIndex;

}


int LanGenerator::GenerateIsolationBridge ( LanComputer *comp, LanCluster *cluster, int x, int y, int security )
{

    UplinkAssert (comp);
    UplinkAssert (cluster);

    //
    // Main control panel


	DialogScreen *ds = new DialogScreen ();
	int dsIndex = comp->AddComputerScreen( ds );
	ds->SetMainTitle ( comp->companyname );
	ds->SetSubTitle ( "ISOLATION BRIDGE" );
	ds->AddWidget( "text", WIDGET_CAPTION, 50, 100, 200, 100, "Isolation bridge accessed", " " );
	ds->AddWidget( "enable", WIDGET_SCRIPTBUTTON, 50, 200, 150, 20, "Enable Isolation Bridge",
							"Enable the bridge (disconnecting Router)", 82, -1, NULL, NULL );
	ds->AddWidget( "disable", WIDGET_SCRIPTBUTTON, 50, 240, 150, 20, "Disable Isolation Bridge",
							"Disable the bridge (enable Router)", 83, -1, NULL, NULL );


    int systemIndex = cluster->AddLanSystem( LANSYSTEM_ISOLATIONBRIDGE, x, y, security, dsIndex );
    return systemIndex;

}

int LanGenerator::GenerateSessionKeyServer ( LanComputer *comp, LanCluster *cluster, int x, int y, int security )
{

    UplinkAssert (comp);
    UplinkAssert (cluster);

    MessageScreen *ms = new MessageScreen ();
    ms->SetMainTitle ( comp->companyname );
    ms->SetSubTitle ( "Session key server" );
    ms->SetTextMessage ( "This is a session key server" );
    int msIndex = comp->AddComputerScreen ( ms );

    int systemIndex = cluster->AddLanSystem( LANSYSTEM_SESSIONKEYSERVER, x, y, security, msIndex );
    return systemIndex;

}

int LanGenerator::GenerateModem ( LanComputer *comp, LanCluster *cluster, int x, int y, int security, char *phoneNum, size_t phoneNumsize )
{

    UplinkAssert (comp);
    UplinkAssert (cluster);

    //
    // New modem virtual location

	char computername [SIZE_COMPUTER_NAME];
	UplinkSnprintf ( computername, sizeof ( computername ), "%s LAN Dial Up access", comp->companyname );

	int mapx, mapy;
    WorldGenerator::GenerateValidMapPos ( mapx, mapy );
	char ip [SIZE_VLOCATION_IP];

	if ( phoneNum && phoneNum[0] == '0' ) {
        UplinkStrncpy ( ip, phoneNum, sizeof ( ip ) );

	} else {
        UplinkSnprintf ( ip, sizeof ( ip ), "0%d%d%d%d-%d%d%d%d%d%d", NumberGenerator::RandomNumber (10), NumberGenerator::RandomNumber (10), NumberGenerator::RandomNumber (10), NumberGenerator::RandomNumber (10),
		    									NumberGenerator::RandomNumber (10), NumberGenerator::RandomNumber (10), NumberGenerator::RandomNumber (10), NumberGenerator::RandomNumber (10), NumberGenerator::RandomNumber (10), NumberGenerator::RandomNumber (10) );
	}

	VLocationSpecial *vl = new VLocationSpecial ();
	vl->SetPLocation ( mapx, mapy );
	vl->SetIP ( ip );
	game->GetWorld ()->CreateVLocation ( vl );
	vl->SetListed ( false );
    vl->SetComputer ( comp->name );

    if ( phoneNum ) UplinkStrncpy ( phoneNum, ip, phoneNumsize );

	//
	// Password Screen

	PasswordScreen *ps = new PasswordScreen ();
	ps->SetMainTitle ( "Modem" );
	ps->SetSubTitle ( "Password verification required" );
    ps->SetDifficulty ( HACKDIFFICULTY_LANMODEM );

	if		( security == 1 )	ps->SetPassword ( NameGenerator::GenerateEasyPassword() );
	else if ( security == 2 )	ps->SetPassword ( NameGenerator::GeneratePassword() );
	else						ps->SetPassword ( NameGenerator::GenerateComplexPassword() );
	int psIndex = comp->AddComputerScreen ( ps );


	//
	// Opening message screen

    char textMsg[512];
    UplinkSnprintf ( textMsg, sizeof ( textMsg ),  "LAN DIAL-UP ACCESSED\n\n"
													 "Phone charges incurred will be your own.\n"
													 "Our company is not responsible for your misuse of this system.\n"
													 "Welcome to our LAN.\n"
													 "If you are not an authorised user you must disconnect now or risk\n"
													 "legal action and jail time.\n\n"
													 "The local dial-up number is %s.",
													 ip );

	MessageScreen *ms = new MessageScreen ();
	ms->SetMainTitle ( comp->companyname );
	ms->SetSubTitle ( "Local Area Network" );
	ms->SetTextMessage ( textMsg );
	int msIndex = comp->AddComputerScreen( ms );

    ps->SetNextPage( msIndex );
    int systemIndex = cluster->AddLanSystem( LANSYSTEM_MODEM, x, y, security, psIndex, msIndex );

    vl->SetScreenIndex ( msIndex );
    vl->SetSecuritySystemIndex ( systemIndex );

    return systemIndex;

}


int LanGenerator::GenerateRadioTransmitter ( LanComputer *comp, LanCluster *cluster, int x, int y, int security )
{

    UplinkAssert (comp);
    UplinkAssert (cluster);

    GenericScreen *gs = new GenericScreen ();
    gs->SetMainTitle ( comp->companyname );
    gs->SetSubTitle ( "Wireless LAN Transmitter" );
    gs->SetScreenType ( SCREEN_RADIOTRANSMITTER );
    int gsIndex = comp->AddComputerScreen ( gs );

    int systemIndex = cluster->AddLanSystem( LANSYSTEM_RADIOTRANSMITTER, x, y, security, gsIndex );
    return systemIndex;

}

int LanGenerator::GenerateRadioReceiver ( LanComputer *comp, LanCluster *cluster, int x, int y, int security )
{

    UplinkAssert (comp);
    UplinkAssert (cluster);

    MessageScreen *ms = new MessageScreen ();
    ms->SetMainTitle ( comp->companyname );
    ms->SetSubTitle ( "Wireless LAN Receiver" );
    ms->SetTextMessage ( "Wireless connection successfully established.\n"
                         "Checking receiver systems : OK\n"
                         "Checking frequency range : OK\n"
                         "Incoming signal strength : 90%\n\n"
                         "Maximum system bandwidth : 50Gq/s\n"
                         "Maximum system latency : 400ms" );
    int msIndex = comp->AddComputerScreen ( ms );

    int systemIndex = cluster->AddLanSystem( LANSYSTEM_RADIORECEIVER, x, y, security, msIndex, msIndex );
    return systemIndex;

}

int LanGenerator::GenerateFaxPrinter ( LanComputer *comp, LanCluster *cluster, int x, int y, int security )
{


    UplinkAssert (comp);
    UplinkAssert (cluster);

    MessageScreen *ms = new MessageScreen ();
    ms->SetMainTitle ( comp->companyname );
    ms->SetSubTitle ( "Fax printer" );
    ms->SetTextMessage ( "This is a fax printer" );
    int msIndex = comp->AddComputerScreen ( ms );

    int systemIndex = cluster->AddLanSystem( LANSYSTEM_FAXPRINTER, x, y, security, msIndex, msIndex );
    return systemIndex;

}

bool LanGenerator::HideData ( LanComputer *comp, int systemIndex, char *data )
{

    UplinkAssert (comp);

    if ( comp->systems.ValidIndex(systemIndex) ) {
        LanComputerSystem *system = comp->systems.GetData( systemIndex );
        return HideData ( comp, system, data );
    }

    return false;

}

bool LanGenerator::HideData ( LanComputer *comp, LanComputerSystem *system, char *data )
{

    UplinkAssert (comp);
    UplinkAssert (system);
    UplinkAssert (data);

    //
    // Try to insert the data into a message screen

    if ( system->dataScreenIndex != -1 ) {

        ComputerScreen *cs = comp->GetComputerScreen( system->dataScreenIndex );
        if ( cs && cs->GetOBJECTID() == OID_MESSAGESCREEN ) {

            MessageScreen *ms = (MessageScreen *) cs;
			size_t newCaptionsize = strlen(ms->textmessage) + strlen(data) + 5;
            char *newCaption = new char [ newCaptionsize ];
            UplinkSnprintf ( newCaption, newCaptionsize, "%s\n\n%s", ms->textmessage, data );
            ms->SetTextMessage ( newCaption );
            delete [] newCaption;
            return true;

        }

    }

    return false;

}

int LanGenerator::GetRadioFrequency ( LList <int> *usedRadioFrenquencies )
{

	bool foundFrequency = false;
	int newFrequency = -1;
	do {
		newFrequency = RADIOTRANSMITTER_MINRANGE + NumberGenerator::RandomNumber( RADIOTRANSMITTER_MAXRANGE - RADIOTRANSMITTER_MINRANGE );

		foundFrequency = false;
		for ( int i = 0; i < usedRadioFrenquencies->Size (); i++ )
			if ( usedRadioFrenquencies->GetData ( i ) == newFrequency ) {
				foundFrequency = true;
				break;
			}
	} while ( foundFrequency );

	usedRadioFrenquencies->PutData ( newFrequency );
	return newFrequency;

}

LanCluster::LanCluster ()
{
    input = -1;
    output = -1;
    centreX = 0;
    centreY = 0;
}

LanCluster::~LanCluster ()
{
	DeleteDArrayData ( (DArray <UplinkObject *> *) &links );
}

int LanCluster::AddLanSystem ( LanComputerSystem *system )
{
    return systems.PutData( system );
}

int LanCluster::AddLanSystem ( int TYPE, int x, int y, int security, int screenIndex, int dataScreenIndex )
{

    LanComputerSystem *system = new LanComputerSystem ();
    system->TYPE = TYPE;
    system->x = x;
    system->y = y;
	system->subnet = NumberGenerator::RandomNumber ( LAN_SUBNETRANGE );
	system->security = security;
	system->screenIndex = screenIndex;
    system->dataScreenIndex = dataScreenIndex;
    if ( TYPE == LANSYSTEM_ROUTER ) system->visible = LANSYSTEMVISIBLE_TYPE;
    return AddLanSystem ( system );

}

int LanCluster::AddLanLink ( int from, float fromX, float fromY,
                             int to, float toX, float toY, int security )
{
    LanComputerLink *link = new LanComputerLink ();
    link->from = from;
    link->to = to;
    link->fromX = fromX;
    link->fromY = fromY;
    link->toX = toX;
    link->toY = toY;
	link->port = NumberGenerator::RandomNumber ( LAN_LINKPORTRANGE );
	link->security = security;
    return links.PutData ( link );
}

bool LanCluster::VerifyLanLink ( int from, float fromX, float fromY, 
                                 int to, float toX, float toY, int security )
{
	if ( !systems.ValidIndex ( from ) || !systems.ValidIndex ( to ) ) {
		return false;
	}

	if ( 0.0f > fromX || fromX > 1.0f ||
	     0.0f > fromY || fromY > 1.0f ||
	     0.0f > toX || toX > 1.0f ||
		 0.0f > toY || toY > 1.0f ) {
		return false;
	}

	if ( 0 > security ) {
		return false;
	}

	return true;
}

void LanCluster::Rotate ( int angle )
{

    //
    // Rotate all systems 90 degrees * angle

    for ( int i = 0; i < angle; ++i ) {

        for ( int s = 0; s < systems.Size(); ++s ) {
            if ( systems.ValidIndex(s) ) {

                LanComputerSystem *system = systems.GetData(s);
                UplinkAssert (system);

                int tempX = system->x - centreX;
                int tempY = system->y - centreY;

                system->x = -tempY + centreX;
                system->y = tempX + centreY;

            }
        }

    }


    //
    // Rotate all links 90 degrees * angle

    for ( int j = 0; j < angle; ++j ) {

        for ( int l = 0; l < links.Size(); ++l ) {
            if ( links.ValidIndex(l) ) {

                LanComputerLink *link = links.GetData(l);
                UplinkAssert (link);

                float tempX = link->fromX;
                float tempY = link->fromY;
                link->fromX = 1.0f - tempY;
                link->fromY = tempX;

                tempX = link->toX;
                tempY = link->toY;
                link->toX = 1.0f - tempY;
                link->toY = tempX;

            }
        }

    }

}

void LanCluster::Merge ( LanCluster *target, LanComputer *comp )
{

    UplinkAssert (target);

    //
    // Make a store for the newly assigned indexes

    int *newIndexes = new int [ systems.Size() ];
    for ( int k = 0; k < systems.Size(); ++k )
        newIndexes[k] = -1;

    //
    // Transfer all systems

    for ( int i = 0; i < systems.Size (); ++i ) {
        if ( systems.ValidIndex (i) ) {

            LanComputerSystem *system = systems.GetData( i );
            newIndexes[i] = target->AddLanSystem ( system );

            if ( system->TYPE == LANSYSTEM_MODEM ) {

				if ( comp->screens.ValidIndex ( system->screenIndex ) ) {
					int targetScreenIndex = ((UserIDScreen *) comp->screens.GetData( system->screenIndex ))->nextpage;
					DArray <VLocation *> *locations = game->GetWorld ()->locations.ConvertToDArray();
					for ( int i = 0; i < locations->Size(); ++i ) {
						if ( locations->ValidIndex(i) ) {
							if ( locations->GetData(i)->GetOBJECTID() == OID_VLOCATIONSPECIAL ) {

								VLocationSpecial *vl = (VLocationSpecial *) locations->GetData(i);
								if ( strcmp ( vl->computer, comp->name ) == 0 &&
									 vl->screenIndex == targetScreenIndex &&
									 vl->securitySystemIndex < systems.Size () ) {

									vl->SetSecuritySystemIndex( newIndexes[vl->securitySystemIndex] );
									break;

								}

							}
						}
					}
					delete locations;
				}

            }

        }
    }

    //
    // Transfer all links

    for ( int j = 0; j < links.Size (); ++j ) {
        if ( links.ValidIndex (j) ) {

            LanComputerLink *link = links.GetData( j );
            target->AddLanLink ( newIndexes[link->from], link->fromX, link->fromY,
                                 newIndexes[link->to], link->toX, link->toY, link->security );

            // TODO : Memory leak with link here

        }
    }

    //
    // Update all data values

    for ( int l = 0; l < systems.Size(); ++l ) {
        if ( systems.ValidIndex(l) ) {

            LanComputerSystem *system = systems.GetData(l);

            if ( system->TYPE == LANSYSTEM_AUTHENTICATION ||
                 system->TYPE == LANSYSTEM_ISOLATIONBRIDGE ||
                 system->TYPE == LANSYSTEM_MAINSERVER ) {

                if ( system->data1 != -1 ) system->data1 = newIndexes[system->data1];
                if ( system->data2 != -1 ) system->data2 = newIndexes[system->data2];
                if ( system->data3 != -1 ) system->data3 = newIndexes[system->data3];

            }

            for ( int m = 0; m < system->validSubnets.Size(); ++m ) {
                if ( system->validSubnets.ValidIndex(m) ) {

                    int oldIndex = system->validSubnets.GetData(m);
                    system->validSubnets.PutData( newIndexes[oldIndex], m );

                }
            }

        }
    }

    if ( input != -1 ) input = newIndexes[input];
    if ( output != -1 ) output = newIndexes[output];

	delete [] newIndexes;

}

void LanCluster::Merge ( LanComputer *target )
{

    UplinkAssert (target);

    //
    // Make a store for the newly assigned indexes

    int *newIndexes = new int [ systems.Size() ];
    for ( int k = 0; k < systems.Size(); ++k )
        newIndexes[k] = -1;

    //
    // Transfer all systems

    for ( int i = 0; i < systems.Size (); ++i ) {
        if ( systems.ValidIndex (i) ) {

            LanComputerSystem *system = systems.GetData( i );
            newIndexes[i] = target->AddLanSystem ( system );

            if ( system->TYPE == LANSYSTEM_MODEM ) {

				if ( target->screens.ValidIndex ( system->screenIndex ) ) {
					int targetScreenIndex = ((UserIDScreen *) target->screens.GetData( system->screenIndex ))->nextpage;
					DArray <VLocation *> *locations = game->GetWorld ()->locations.ConvertToDArray();
					for ( int i = 0; i < locations->Size(); ++i ) {
						if ( locations->ValidIndex(i) ) {
							if ( locations->GetData(i)->GetOBJECTID() == OID_VLOCATIONSPECIAL ) {

								VLocationSpecial *vl = (VLocationSpecial *) locations->GetData(i);
								if ( strcmp ( vl->computer, target->name ) == 0 &&
									 vl->screenIndex == targetScreenIndex &&
									 vl->securitySystemIndex < systems.Size () ) {

									vl->SetSecuritySystemIndex( newIndexes[vl->securitySystemIndex] );
									break;

								}

							}
						}
					}
					delete locations;
				}

            }

        }
    }

    //
    // Transfer all links

    for ( int j = 0; j < links.Size (); ++j ) {
        if ( links.ValidIndex (j) ) {

            LanComputerLink *link = links.GetData( j );
            target->AddLanLink ( newIndexes[link->from], link->fromX, link->fromY,
                                 newIndexes[link->to], link->toX, link->toY, link->security );

            // TODO : Memory leak with link here

        }
    }

    //
    // Update all data values

    for ( int l = 0; l < systems.Size(); ++l ) {
        if ( systems.ValidIndex(l) ) {

            LanComputerSystem *system = systems.GetData(l);

            if ( system->TYPE == LANSYSTEM_AUTHENTICATION ||
                 system->TYPE == LANSYSTEM_ISOLATIONBRIDGE ||
                 system->TYPE == LANSYSTEM_MAINSERVER ) {

                if ( system->data1 != -1 ) system->data1 = newIndexes[system->data1];
                if ( system->data2 != -1 ) system->data2 = newIndexes[system->data2];
                if ( system->data3 != -1 ) system->data3 = newIndexes[system->data3];

            }

            for ( int m = 0; m < system->validSubnets.Size(); ++m ) {
                if ( system->validSubnets.ValidIndex(m) ) {

                    int oldIndex = system->validSubnets.GetData(m);
                    system->validSubnets.PutData( newIndexes[oldIndex], m );

                }
            }

        }
    }

    if ( input != -1 ) input = newIndexes[input];
    if ( output != -1 ) output = newIndexes[output];

	delete [] newIndexes;

}
