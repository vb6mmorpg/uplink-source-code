// -*- tab-width:4 c-file-style:"cc-mode" -*-

#include <strstream>

#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"

#include "game/game.h"
#include "game/data/data.h"

#include "world/world.h"
#include "world/message.h"
#include "world/computer/gateway.h"
#include "world/computer/gatewaydef.h"

#include "mmgr.h"


Gateway::Gateway ()
{

	curgatewaydef = NULL;

	id = 0;
	newgatewaydef = NULL;
	nuked = false;
	proximity = 0;

	UplinkStrncpy ( modemtype, " ", sizeof ( modemtype ) );
	modemspeed = 0;
	memorysize = 0;

	hudupgrades = 0;

}

Gateway::~Gateway ()
{

    DeleteLListData ( &hardware );
	if ( curgatewaydef )
		delete curgatewaydef;
	if ( newgatewaydef )
		delete newgatewaydef;

}

void Gateway::GenerateNewID ()
{

	id++;

}

void Gateway::Nuke ()
{

	//
	// Reset software
	//

	databank.Format ();
	databank.SetSize ( PLAYER_START_MEMORYSIZE );

	GiveStartingSoftware ();

	hudupgrades = 0;

	//
	// Reset all of the hardware items
	//

	SetGatewayStart ();

	DeleteLListData ( &hardware );
	hardware.Empty ();

	GiveStartingHardware ();

	//
	// Change the Gateway ID  (so any SeizeGatewayEvents will now fail)
	//

	GenerateNewID ();

	//
	// Set it to be nuked, to remind us next time we log in
	//

	proximity = 0;
	nuked = true;

}

void Gateway::IncrementProximity ( int n )
{

	proximity += n;

}

void Gateway::DecrementProximity ( int n )
{

	proximity -= n;
    if ( proximity < 0 ) proximity = 0;

}

void Gateway::SetGatewayStart ()
{

	GatewayDef *newgateway = game->GetWorld ()->GetGatewayDef ( PLAYER_START_GATEWAYNAME );

	if ( !newgateway )
		for ( int i = 0; i < game->GetWorld ()->gatewaydefs.Size (); i++ )
			if ( game->GetWorld ()->gatewaydefs.ValidIndex ( i ) ) {
				newgateway = game->GetWorld ()->gatewaydefs.GetData ( i );
				break;
			}

    UplinkAssert (newgateway);

	curgatewaydef = new GatewayDef ( *newgateway );

}

void Gateway::SetGateway ( GatewayDef *newgd )
{

	UplinkAssert ( newgd );
	curgatewaydef = new GatewayDef ( *newgd );

}

void Gateway::ExchangeGateway ( GatewayDef *newgd )
{

	if ( newgatewaydef )
		delete newgatewaydef;

	if ( newgd )
		newgatewaydef = new GatewayDef ( *newgd );
	else
		newgatewaydef = NULL;

}

void Gateway::ExchangeGatewayComplete ()
{

	UplinkAssert (curgatewaydef);
	GatewayDef *oldgateway = new GatewayDef ( *curgatewaydef );

	if ( !newgatewaydef )
		printf ( "WARNING: Gateway::ExchangeGatewayComplete, cannot find exchange gateway\n" );
	else
		SetGateway ( newgatewaydef );

	if ( newgatewaydef ) {
		delete newgatewaydef;
		newgatewaydef = NULL;
	}
	nuked = false;
	GenerateNewID ();

	// Added to hopefully fix the problem with the motion sensor still red after the player nuked his gateway.
	proximity = 0;


	//
	// Handle loss of hardware or software
	//

	GatewayDef *newgateway = curgatewaydef;
	UplinkAssert (newgateway);

	LList <char *> removedItems;


	//
	// NumCPUS - remove the slowest until we have the right number
	//

	while ( newgateway->maxcpus < GetNumCPUs () ) {

		int indexSlowest = -1;
		const ComputerUpgrade *upgradeSlowest = NULL;

		for ( int i = 0; i < hardware.Size (); ++i ) {

			const ComputerUpgrade *cu = GetHardwareUpgrade ( hardware.GetData (i) );
			UplinkAssert (cu);

			if ( cu->TYPE == GATEWAYUPGRADETYPE_CPU ) {
				if ( indexSlowest == -1 || upgradeSlowest == NULL ) {
					indexSlowest = i;
					upgradeSlowest = cu;
				}
				else if ( cu->data < upgradeSlowest->data ) {
					indexSlowest = i;
					upgradeSlowest = cu;
				}
			}

		}

		if ( indexSlowest != -1 ) {
			UplinkAssert (upgradeSlowest != NULL);
			removedItems.PutData ( upgradeSlowest->name );
			hardware.RemoveData (indexSlowest);
		}
		else {
			UplinkAbort ( "Gateway::ExchangeGatewayComplete, there should be a slowest CPU" );
		}

	}


	//
	// Memory - remove the last N GQs until the right amount is reached
	//

	if ( newgateway->maxmemory * 8 < databank.GetSize () ) {

		for ( int i = newgateway->maxmemory * 8; i < memorysize; ++i )
			databank.RemoveData (i);

		size_t removedMemorysize = 64;
		char *removedMemory = new char [removedMemorysize];
		UplinkSnprintf ( removedMemory, removedMemorysize, "%dGq of memory", memorysize - (newgateway->maxmemory*8) );
		removedItems.PutData (removedMemory);

        SetMemorySize ( newgateway->maxmemory * 8 );

	}


	//
	// Security - Remove N security items, first come first serve ;)
	//

    int numSecurity = GetNumSecurity ();

	while ( newgateway->maxsecurity < numSecurity ) {

		for ( int i = 0; i < hardware.Size (); ++i ) {

			const ComputerUpgrade *cu = GetHardwareUpgrade ( hardware.GetData (i) );
			UplinkAssert (cu);

			if ( cu->TYPE == GATEWAYUPGRADETYPE_SECURITY ) {
				removedItems.PutData ( cu->name );
				hardware.RemoveData (i);
			}

		}

        numSecurity = GetNumSecurity ();
	}


	//
	// Tell the player what we removed
	//

	if ( removedItems.Size () > 0 ) {

		std::ostrstream removedText;
		removedText << "Unfortunately, your new gateway does not have enough space to contain "
					   "all of your old hardware.  As such, we have been forced to remove the "
					   "following items from your new system :\n\n";

		for ( int i = 0; i < removedItems.Size (); ++i )
			removedText << removedItems.GetData(i) << "\n";

		removedText << '\x0';

		Message *msg = new Message ();
		msg->SetTo ( "PLAYER" );
		msg->SetFrom ( "Uplink Corporation" );
		msg->SetSubject ( "Removed hardware items" );
		msg->SetBody ( removedText.str () );
		msg->Send ();

		removedText.rdbuf()->freeze( 0 );
		//delete [] removedText.str ();

	}

	if ( oldgateway )
		delete oldgateway;

}

void Gateway::SetModemType ( char *newmodem, int newmodemspeed )
{

	UplinkAssert ( strlen(newmodem) < 64 );
	UplinkStrncpy ( modemtype, newmodem, sizeof ( modemtype ) );

	modemspeed = newmodemspeed;

}

void Gateway::SetMemorySize ( int newsize )
{

	memorysize = newsize;
	databank.SetSize ( memorysize );

}

void Gateway::ChangeMemorySize ( int amount )
{

	SetMemorySize ( memorysize + amount );

}

void Gateway::GiveHUDUpgrade ( char upgrade )
{

	hudupgrades = hudupgrades | upgrade;

}

bool Gateway::HasHUDUpgrade ( char upgrade )
{

	return ( hudupgrades & upgrade );

}

char Gateway::GetHudUpgrades ()
{

	return hudupgrades;

}

bool Gateway::HasAnyHUDUpgrade ()
{

	return ( hudupgrades != 0 );

}

void Gateway::GiveCPU ( char *CPUName )
{

	GatewayDef *gatewaydef = curgatewaydef;
	UplinkAssert (gatewaydef);

    char *cpucopy = new char [strlen(CPUName)+1];
    UplinkSafeStrcpy ( cpucopy, CPUName );

	if ( GetNumCPUs () < gatewaydef->maxcpus ) {

		hardware.PutData ( cpucopy );

	}
	else {

		// The player already has a full set of CPUs
		// So replace his slowest

		int indexSlowest = -1;
		const ComputerUpgrade *upgradeSlowest = NULL;

		for ( int i = 0; i < hardware.Size (); ++i ) {

			const ComputerUpgrade *cu = GetHardwareUpgrade ( hardware.GetData (i) );
			UplinkAssert (cu);

			if ( cu->TYPE == GATEWAYUPGRADETYPE_CPU ) {
				if ( indexSlowest == -1 || upgradeSlowest == NULL ) {
					indexSlowest = i;
					upgradeSlowest = cu;
				}
				else if ( cu->data < upgradeSlowest->data ) {
					indexSlowest = i;
					upgradeSlowest = cu;
				}
			}

		}

		if ( indexSlowest != -1 ) {
			char *existing = hardware.GetData (indexSlowest);
            hardware.RemoveData (indexSlowest);
            delete [] existing;
			hardware.PutDataAtIndex ( cpucopy, indexSlowest);
		}
		else {
			UplinkAbort ( "Gateway::GiveCPU, there should be a slowest CPU" );
		}

	}

}

int Gateway::GetNumCPUs ()
{

	int numCPUs = 0;

	for ( int i = 0; i < hardware.Size (); ++i ) {

		const ComputerUpgrade *cu = GetHardwareUpgrade ( hardware.GetData (i) );
		UplinkAssert (cu);

		if ( cu->TYPE == GATEWAYUPGRADETYPE_CPU )
			++numCPUs;

	}

	return numCPUs;

}

int Gateway::GetCPUSpeed ()
{

	int speed = 0;

	for ( int i = 0; i < hardware.Size (); ++i ) {
		const ComputerUpgrade *cu = GetHardwareUpgrade ( hardware.GetData (i) );
		UplinkAssert (cu);
		if ( cu->TYPE == GATEWAYUPGRADETYPE_CPU )
			speed += cu->data;
	}

	return speed;

}

LList <char *> *Gateway::GetCPUs ()
{

	LList <char *> *list = new LList <char *> ();

	for ( int i = 0; i < hardware.Size (); ++i ) {

		const ComputerUpgrade *cu = GetHardwareUpgrade ( hardware.GetData (i) );
		UplinkAssert (cu);

		if ( cu->TYPE == GATEWAYUPGRADETYPE_CPU )
			list->PutData ( cu->name );

	}

	return list;

}

int Gateway::GetBandwidth ()
{

	GatewayDef *gatewaydef = curgatewaydef;
	UplinkAssert (gatewaydef);

	if ( modemspeed < gatewaydef->bandwidth )
		return modemspeed;

	else
		return gatewaydef->bandwidth;

}

int Gateway::GetNumSecurity ()
{

	int numsec = 0;

	for ( int i = 0; i < hardware.Size (); ++i ) {

		const ComputerUpgrade *cu = GetHardwareUpgrade ( hardware.GetData (i) );
		UplinkAssert (cu);

		if ( cu->TYPE == GATEWAYUPGRADETYPE_SECURITY )
			++numsec;

	}

	return numsec;

}

LList <char *> *Gateway::GetSecurity ()
{

	LList <char *> *list = new LList <char *> ();

	for ( int i = 0; i < hardware.Size (); ++i ) {

		const ComputerUpgrade *cu = GetHardwareUpgrade ( hardware.GetData (i) );
		UplinkAssert (cu);

		if ( cu->TYPE == GATEWAYUPGRADETYPE_SECURITY )
			list->PutData ( cu->name );

	}

	return list;

}

void Gateway::GiveStartingHardware ()
{

	SetGatewayStart ();

	// CPU

	GiveCPU ( PLAYER_START_CPUTYPE );

	// Modem

	char modemname [64];
	UplinkSnprintf ( modemname, sizeof ( modemname ), "Modem (%d Ghz)", PLAYER_START_MODEMSPEED );
	SetModemType  ( modemname, PLAYER_START_MODEMSPEED );

	// Memory

	SetMemorySize ( PLAYER_START_MEMORYSIZE );

}

void Gateway::GiveHardware ( char *newhardware )
{

	char *hwtitle = new char [strlen(newhardware)+1];
	UplinkSafeStrcpy ( hwtitle, newhardware );

	hardware.PutData ( hwtitle );

}

bool Gateway::IsHWInstalled ( char *name )
{

	for ( int i = 0; i < hardware.Size (); ++i )
		if ( hardware.GetData (i) )
			if ( strcmp ( hardware.GetData (i), name ) == 0 )
				return true;

	return false;

}

void Gateway::GiveStartingSoftware ()
{

	Data *copier = new Data ();
	copier->SetTitle ( "File_Copier" );
	copier->SetDetails ( DATATYPE_PROGRAM, 1, 0, 0, 1.0, SOFTWARETYPE_FILEUTIL );
	databank.PutData ( copier, 14 );

	Data *deleter = new Data ();
	deleter->SetTitle ( "File_Deleter" );
	deleter->SetDetails ( DATATYPE_PROGRAM, 1, 0, 0, 1.0, SOFTWARETYPE_FILEUTIL );
	databank.PutData ( deleter, 15 );

	Data *tutorial = new Data ();
	tutorial->SetTitle ( "Tutorial" );
	tutorial->SetDetails ( DATATYPE_PROGRAM, 4, 0, 0, 1.0, SOFTWARETYPE_OTHER );
	databank.PutData ( tutorial, 17 );

#ifdef TESTGAME
    Data *defrag = new Data ();
    defrag->SetTitle ( "Defrag" );
    defrag->SetDetails ( DATATYPE_PROGRAM, 2, 0, 0, 1.0, SOFTWARETYPE_FILEUTIL );
    databank.PutData ( defrag, 4 );
#endif

}

void Gateway::GiveAllHardware ()
{

	// Best gateway

	GatewayDef *bestgatewayDef = NULL;
	for ( int i = game->GetWorld ()->gatewaydefs.Size () - 1; i >= 0; i-- )
		if ( game->GetWorld ()->gatewaydefs.ValidIndex ( i ) ) {
			bestgatewayDef = game->GetWorld ()->gatewaydefs.GetData ( i );
			break;
		}

	UplinkAssert (bestgatewayDef);
	SetGateway ( bestgatewayDef );


	// Some big CPUs

	for ( int i = 0; i < 4; ++i )
		GiveCPU ( "CPU ( 150 Ghz )" );


	// Max all stats

	SetMemorySize ( bestgatewayDef->maxmemory * 8 );
	SetModemType ( "Fast fucker", bestgatewayDef->bandwidth );

}

void Gateway::GiveAllSoftware ()
{

	for ( int i = 0; i < NUM_STARTINGSOFTWAREUPGRADES; ++i ) {

		Data *prog = new Data ();
		prog->SetTitle ( SOFTWARE_UPGRADES [i].name );
		prog->SetDetails ( DATATYPE_PROGRAM,
						   SOFTWARE_UPGRADES [i].size,
						   0, 0,
						   (float) SOFTWARE_UPGRADES [i].data,
						   SOFTWARE_UPGRADES [i].TYPE );
		databank.PutData ( prog );

	}

	// Give all upgrades

	hudupgrades = 255;

}


bool Gateway::Load  ( FILE *file )
{

	LoadID ( file );

	curgatewaydef = NULL;
	newgatewaydef = NULL;

	if ( !databank.Load ( file ) ) return false;

	if ( !LoadLList ( &hardware, file ) ) return false;

	int old_type = -1;
	int old_newtype = -1;
	char old_name [SIZE_GATEWAY_NAME] = {0};

	if ( strcmp( game->GetLoadedSavefileVer(), "SAV61" ) >= 0 ) {
		curgatewaydef = new GatewayDef ();
		if ( !curgatewaydef->Load ( file ) ) return false;

		int exists;
		if ( !FileReadData ( &exists, sizeof(exists), 1, file ) ) return false;
		if ( exists != 0 ) {
			newgatewaydef = new GatewayDef ();
			if ( !newgatewaydef->Load ( file ) ) return false;
		}
	}
	else {
		if ( !FileReadData ( &old_type,		 sizeof(old_type),		 1, file ) ) return false;
		if ( !FileReadData ( &old_newtype,	 sizeof(old_newtype),	 1, file ) ) return false;
	}

	if ( !FileReadData ( &id,		 sizeof(id),		 1, file ) ) return false;
	if ( !FileReadData ( &nuked,		 sizeof(nuked),		 1, file ) ) return false;
	if ( !FileReadData ( &proximity,  sizeof(proximity),	 1, file ) ) return false;

	if ( !FileReadData ( &modemspeed, sizeof(modemspeed), 1, file ) ) return false;
	if ( !FileReadData ( &memorysize, sizeof(memorysize), 1, file ) ) return false;
	if ( !FileReadData ( &modemtype,  sizeof(modemtype),  1, file ) ) return false;

	if ( !FileReadData ( &hudupgrades, sizeof(hudupgrades), 1, file ) ) return false;

	if ( strcmp( game->GetLoadedSavefileVer(), "SAV61" ) < 0 ) {

		if ( old_type == -1 ) {
			if ( strcmp( game->GetLoadedSavefileVer(), "SAV59" ) >= 0 ) {
				if ( !LoadDynamicStringStatic ( old_name, sizeof(old_name), file ) ) return false;
			}
			else {
				if ( !FileReadData ( old_name, sizeof(old_name), 1, file ) ) {
					old_name [ sizeof(old_name) - 1 ] = '\0';
					return false;
				}
				old_name [ sizeof(old_name) - 1 ] = '\0';
			}

			GatewayDef *gd = game->GetWorld ()->GetGatewayDef ( old_name );
			if ( !gd ) {
				UplinkPrintAbortArgs ( "WARNING: Gateway::Load, cannot find gateway '%s'", old_name );
				delete gd;
				return false;
			}
			SetGateway ( gd );
		}
		else {
			if ( game->GetWorld ()->gatewaydefs.ValidIndex ( old_type ) ) {
				SetGateway ( game->GetWorld ()->gatewaydefs.GetData ( old_type ) );
			}
			else {
				UplinkPrintAbortArgs ( "WARNING: Gateway::Load, cannot find gateway at index %d", old_type );
				return false;
			}
		}

		if ( old_newtype != -1 ) {
			if ( game->GetWorld ()->gatewaydefs.ValidIndex ( old_newtype ) )
				ExchangeGateway ( game->GetWorld ()->gatewaydefs.GetData ( old_newtype ) );
		}
	}

	LoadID_END ( file );

	return true;

}

void Gateway::Save  ( FILE *file )
{

	SaveID ( file );

	databank.Save ( file );

	SaveLList ( &hardware, file );

	UplinkAssert ( curgatewaydef );
	curgatewaydef->Save ( file );

	int exists;
	if ( newgatewaydef ) {
		exists = 1;
		fwrite ( &exists, sizeof(exists), 1, file );
		newgatewaydef->Save ( file );
	}
	else {
		exists = 0;
		fwrite ( &exists, sizeof(exists), 1, file );
	}

	fwrite ( &id,		  sizeof(id),         1, file );
	fwrite ( &nuked,	  sizeof(nuked),      1, file );
	fwrite ( &proximity,  sizeof(proximity),  1, file );

	fwrite ( &modemspeed, sizeof(modemspeed), 1, file );
	fwrite ( &memorysize, sizeof(memorysize), 1, file );
	fwrite ( &modemtype,  sizeof(modemtype),  1, file );

	fwrite ( &hudupgrades, sizeof(hudupgrades), 1, file );

	SaveID_END ( file );

}

void Gateway::Print ()
{

	printf ( "Gateway : name='%s', id=%d\n"
	         "nuked=%d, proximity=%d\n", (curgatewaydef)? curgatewaydef->name : "NULL", id, nuked, proximity );
	printf ( "Upgrades : %c\n", hudupgrades );

	databank.Print ();

	PrintLList ( &hardware );

	printf ( "Modem Speed = %d, MemorySize = %d\n",
			 modemspeed, memorysize );

}


void Gateway::Update ()
{

	databank.Update ();

}


char *Gateway::GetID ()
{

	return "GATEWAY";

}
