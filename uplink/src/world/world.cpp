// World.cpp: implementation of the World class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <time.h>
#include <fstream>

#include "redshirt.h"

#include "game/data/data.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"
#include "app/miscutils.h"
#include "app/dos2unix.h"

#include "game/game.h"

#include "world/world.h"
#include "world/vlocation.h"
#include "world/person.h"
#include "world/player.h"

#include "world/company/company.h"
#include "world/company/mission.h"
#include "world/computer/computer.h"
#include "world/computer/gatewaydef.h"

#include "world/generator/worldgenerator.h"
#include "world/generator/missiongenerator.h"
#include "world/generator/recordgenerator.h"
#include "world/generator/numbergenerator.h"
#include "world/generator/namegenerator.h"
#include "world/generator/consequencegenerator.h"
#include "world/generator/newsgenerator.h"
#include "world/generator/plotgenerator.h"
#include "world/generator/demoplotgenerator.h"
#include "world/generator/langenerator.h"

#include "world/scheduler/eventscheduler.h"
#include "world/scheduler/notificationevent.h"

#include "mmgr.h"


World::World()
{

	date.SetDate ( WORLD_START_DATE );
	date.Activate ();							// Make it update with the game
	
	//
	// Initialise the static generators
	//
	
	WorldGenerator::Initialise ();
	MissionGenerator::Initialise ();
	RecordGenerator::Initialise ();
	NumberGenerator::Initialise ();
	NameGenerator::Initialise ();
	ConsequenceGenerator::Initialise ();
	NewsGenerator::Initialise ();
	LanGenerator::Initialise ();
	
	//
	// Load the password list from a disk file
	//

	passwords.SetStepSize ( 100 );
	FILE *file = RsArchiveFileOpen ( "data/wordlist.txt", "rt" );
	UplinkAssert ( file );
	
	while ( !feof (file) ) {

		char line [64];
		fscanf ( file, "%s\n", line );
		CreatePassword ( line );

	}

	RsArchiveFileClose ( "data/wordlist.txt", file );

	//
	// Load the gateway list from a disk file
	//

	char *filename = RsArchiveFileOpen ( "data/gatewaydefs.txt" );
	UplinkAssert (filename);
	idos2unixstream thefile ( filename );
	
	char unused [256];
	int numgateways;
	thefile.getline ( unused, 256 );
	thefile >> unused >> numgateways >> ws;

	for ( int i = 0; i < numgateways; ++i ) {
	
		GatewayDef *def = new GatewayDef ();
		def->LoadGatewayDefinition ( thefile );

        char filename[256];
        char thumbnail[256];
        UplinkSnprintf ( filename, sizeof ( filename ), "gateway/gateway%d.tif", i );
        UplinkSnprintf ( thumbnail, sizeof ( thumbnail ), "gateway/gateway_t%d.tif", i );
        def->SetFilename ( filename );
        def->SetThumbnail ( thumbnail );
        
		CreateGatewayDef ( def );

	}

	thefile.close ();
	RsArchiveFileClose ( "data/gatewaydefs.txt", NULL );

}

World::~World()
{

    WorldGenerator::Shutdown ();
    NameGenerator::Shutdown ();
    MissionGenerator::Shutdown ();

	DeleteBTreeData ( (BTree <UplinkObject *> *) &locations );
	DeleteBTreeData ( (BTree <UplinkObject *> *) &companies );
	DeleteBTreeData ( (BTree <UplinkObject *> *) &computers );						
	DeleteBTreeData ( (BTree <UplinkObject *> *) &people );							

    DeleteDArrayData ( &passwords );

    for ( int i = 0; i < gatewaydefs.Size (); ++i ) 
        if ( gatewaydefs.ValidIndex (i) )
            if ( gatewaydefs.GetData (i) )
                delete gatewaydefs.GetData (i);

}

VLocation *World::CreateVLocation ( char *ip, int phys_x, int phys_y )
{

	if ( locations.LookupTree ( ip ) != NULL ) {

		char warning [128];
		UplinkSnprintf ( warning, sizeof ( warning ), "Duplicate IP created : %s", ip );
		UplinkWarning ( warning );

	}

	VLocation *vl = new VLocation ();
	locations.PutData ( ip, vl );
	
	vl->SetIP ( ip );
	vl->SetPLocation ( phys_x, phys_y );

	return vl;

}

bool World::VerifyVLocation ( char *ip, int phys_x, int phys_y )
{

	if ( !ip || !VLocation::VerifyIP ( ip ) || !VLocation::VerifyPLocation ( phys_x, phys_y ) ) {

		return false;

	}

	return true;

}

Company *World::CreateCompany ( char *name )
{

	Company *company = new Company ();
	companies.PutData ( name, company );
	
	company->SetName ( name );
	
	return company;

}

Computer *World::CreateComputer ( char *name, char *companyname, char *ip )
{

	Computer *computer = new Computer ();
	computers.PutData ( name, computer );

	computer->SetName ( name );
	computer->SetCompanyName ( companyname );
	computer->SetIP ( ip );

	UplinkAssert ( GetVLocation ( ip ) );
	GetVLocation ( ip )->SetComputer ( name );

	return computer;

}

Person *World::CreatePerson ( char *name, char *localhost )
{

	Person *person = new Person ();
	people.PutData ( name, person );

	person->SetName ( name );
	person->SetLocalHost ( localhost );	
	person->SetRemoteHost ( localhost );

	return person;

}

void World::CreateVLocation ( VLocation *vlocation )
{
	
	UplinkAssert (vlocation);
	UplinkAssert (vlocation->ip);

	locations.PutData ( vlocation->ip, vlocation );

}

void World::CreateCompany ( Company *company )
{

	UplinkAssert ( company );
	UplinkAssert ( company->name );

	companies.PutData ( company->name, company );

}

void World::CreateComputer ( Computer *computer )
{

	UplinkAssert (computer);
	UplinkAssert (computer->name);

	computers.PutData ( computer->name, computer );	

	UplinkAssert ( GetVLocation ( computer->ip ) );
	GetVLocation ( computer->ip )->SetComputer ( computer->name );

}

void World::CreatePerson ( Person *person )
{

	UplinkAssert (person);
	UplinkAssert (person->name);

	people.PutData ( person->name, person );

}

void World::CreatePassword  ( char *password )
{

	char *newpassword = new char [ strlen ( password ) + 1 ];
	UplinkSafeStrcpy ( newpassword, password );
	passwords.PutData ( newpassword );

}

void World::CreateGatewayDef ( GatewayDef *newdef )
{

	UplinkAssert (newdef);

	if ( !newdef->VerifyCorrectness () )
		return;

	gatewaydefs.PutData ( newdef );

	gatewaydefs.Sort ( GatewayDef::GatewayDefComparator );

}

VLocation *World::GetVLocation  ( char *ip )
{

	BTree <VLocation *> *vl = locations.LookupTree ( ip );

	if ( vl ) return vl->data;
	else	  return NULL;

}

Company *World::GetCompany ( char *name )
{

	BTree <Company *> *company = companies.LookupTree ( name );

	if ( company ) return company->data;
	else		   return NULL;

}

Computer *World::GetComputer ( char *name )
{

	BTree <Computer *> *computer = computers.LookupTree ( name );

	if ( computer ) return computer->data;
	else			return NULL;

}

Person *World::GetPerson ( char *name )
{

	if ( people.LookupTree("PLAYER") && strcmp ( name, GetPlayer ()->handle ) == 0 ) {
	
		// We're looking for the player
		return game->GetWorld ()->GetPlayer ();

	}
	else {
		
		// We're not looking for the player

		BTree <Person *> *person = people.LookupTree ( name );

		if ( person ) return person->data;
		else		  return NULL;

	}

}

char *World::GetPassword ( int index )				
{

	if ( passwords.ValidIndex ( index ) )
		return passwords.GetData (index);

	else
		return NULL;

}

GatewayDef *World::GetGatewayDef ( char *name )
{

	if ( name ) {
		char *nameTrim = TrimSpaces ( name );

		for ( int i = 0; i < gatewaydefs.Size (); ++i ) {
			if ( gatewaydefs.ValidIndex ( i ) && gatewaydefs.GetData ( i ) ) {

				char *gatewayNameTrim = TrimSpaces ( gatewaydefs.GetData ( i )->name );
				bool equalsname = ( strcmp ( nameTrim, gatewayNameTrim ) == 0 );
				delete [] gatewayNameTrim;
				if ( equalsname ) {
					delete [] nameTrim;
					return gatewaydefs.GetData ( i );
				}

			}
		}

		delete [] nameTrim;
	}

	return NULL;

}

Player *World::GetPlayer ()
{

	BTree <Person *> *player = people.LookupTree ( "PLAYER" );

	UplinkAssert ( player );

	return (Player *) player->data;

}

bool World::Load ( FILE *file )
{
	
	LoadID ( file );

	if ( !date.Load ( file ) ) return false;
	if ( !scheduler.Load ( file ) ) return false;
	if ( !plotgenerator.Load ( file ) ) return false;
    if ( !demoplotgenerator.Load ( file ) ) return false;

	if ( !LoadBTree ( (BTree <UplinkObject *> *) &locations, file ) ) return false;
	if ( !LoadBTree ( (BTree <UplinkObject *> *) &companies, file ) ) return false;
	if ( !LoadBTree ( (BTree <UplinkObject *> *) &computers, file ) ) return false;
	if ( !LoadBTree ( (BTree <UplinkObject *> *) &people,    file ) ) return false;

	// Fix for dead or jailed people talking on the phone or administering companies
	// If the person is in charge of administering a company, replace him with a new person
	// Else it will be impossible to capture his voice and thus breaking in the servers of the company

	WorldGenerator::ReplaceInvalidCompanyAdmins ( );

	// Get new software versions

	WorldGenerator::UpdateSoftwareUpgrades ( ); 

	LoadID_END ( file );

	return true;

}

void World::Save ( FILE *file )
{

	SaveID ( file );

	date.Save ( file );
	scheduler.Save ( file );
	plotgenerator.Save ( file );
    demoplotgenerator.Save ( file );

	SaveBTree ( (BTree <UplinkObject *> *) &locations, file );
	SaveBTree ( (BTree <UplinkObject *> *) &companies, file );
	SaveBTree ( (BTree <UplinkObject *> *) &computers, file );
	SaveBTree ( (BTree <UplinkObject *> *) &people,    file );
	
	SaveID_END ( file );

}

void World::Print ()
{

	printf ( "============== W O R L D ===================================\n" );

	date.Print ();
	scheduler.Print ();
	plotgenerator.Print ();
    demoplotgenerator.Print ();

	PrintBTree ( (BTree <UplinkObject *> *) &locations );
	PrintBTree ( (BTree <UplinkObject *> *) &companies );
	PrintBTree ( (BTree <UplinkObject *> *) &computers );
	PrintBTree ( (BTree <UplinkObject *> *) &people    );

	printf ( "============== E N D  O F  W O R L D =======================\n" );

}

char *World::GetID ()
{

	return "WORLD";

}

void World::Update ()
{

	date.Update ();

	if ( date.After ( &nextupdate ) ) {

		UpdateBTree ( (BTree <UplinkObject *> *) &locations );
		UpdateBTree ( (BTree <UplinkObject *> *) &companies );
		UpdateBTree ( (BTree <UplinkObject *> *) &computers );
		UpdateBTree ( (BTree <UplinkObject *> *) &people    );
		
		scheduler.Update ();
        plotgenerator.Update ();

#ifdef DEMOGAME
        demoplotgenerator.Update ();
#endif
		
#ifdef WAREZRELEASE
    
        int timePlaying = EclGetAccurateTime() - app->starttime;
        if ( timePlaying > WAREZ_MAXPLAYTIME ) {

            Date rundate;
            rundate.SetDate ( &game->GetWorld ()->date );
            rundate.AdvanceMinute ( TIME_TOWAREZGAMEOVER );

            NotificationEvent *ne = new NotificationEvent ();
            ne->SetTYPE ( NOTIFICATIONEVENT_TYPE_WAREZGAMEOVER );
            ne->SetRunDate ( &rundate );
        
            game->GetWorld ()->scheduler.ScheduleEvent ( ne );

        }
#endif

		// Calculate time of next update

		nextupdate.SetDate ( &date );
		nextupdate.AdvanceSecond ( 2 );

	}

}

