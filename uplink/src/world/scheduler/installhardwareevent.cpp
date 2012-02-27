
#include <strstream>

#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"

#include "game/game.h"

#include "world/world.h"
#include "world/player.h"
#include "world/message.h"
#include "world/company/sale.h"
#include "world/computer/gatewaydef.h"
#include "world/scheduler/installhardwareevent.h"

#include "mmgr.h"


InstallHardwareEvent::InstallHardwareEvent ()
{

	hwsale = NULL;
	version = -1;

}

InstallHardwareEvent::~InstallHardwareEvent ()
{
}

void InstallHardwareEvent::SetHWSale ( Sale *newsale, int newversion )
{

	hwsale = newsale;
	version = newversion;

	UplinkAssert ( hwsale->GetVersion ( version ) );

}

void InstallHardwareEvent::Run ()
{

	UplinkAssert ( hwsale );

	SaleVersion *sv = hwsale->GetVersion ( version );
	UplinkAssert ( sv );

	bool success = true;
	char failure [128];

	//
	// MODEM - Change the player's gateway's transfer rate
	//

	if ( hwsale->swhwTYPE == GATEWAYUPGRADETYPE_MODEM )
		game->GetWorld ()->GetPlayer ()->gateway.SetModemType ( hwsale->title, sv->data );

	//
	// CPU   - Change the player's gateway's processor speed
	//

	if ( hwsale->swhwTYPE == GATEWAYUPGRADETYPE_CPU )
		game->GetWorld ()->GetPlayer ()->gateway.GiveCPU ( hwsale->title );

	//
	// MEMORY  - Give the player's gateway more memory space
	//

	if ( hwsale->swhwTYPE == GATEWAYUPGRADETYPE_MEMORY ) {

		Gateway *gateway = &(game->GetWorld ()->GetPlayer ()->gateway);
		UplinkAssert (gateway);
		GatewayDef *gd = gateway->curgatewaydef;
		UplinkAssert (gd);

		if ( gateway->memorysize + sv->data > gd->maxmemory * 8 ) {
			success = false;
			UplinkStrncpy ( failure, "There was not enough space to fit the memory into your gateway.", sizeof ( failure ) );
			game->GetWorld ()->GetPlayer ()->ChangeBalance ( sv->cost, "Refund from Uplink Corporation" );
		}
		else
			game->GetWorld ()->GetPlayer ()->gateway.ChangeMemorySize ( sv->data );

	}


    //
    // SECURITY - check the player has room for the device before installing it
    //

    if ( hwsale->swhwTYPE == GATEWAYUPGRADETYPE_SECURITY ) {

		Gateway *gateway = &(game->GetWorld ()->GetPlayer ()->gateway);
		UplinkAssert (gateway);
		GatewayDef *gd = gateway->curgatewaydef;
		UplinkAssert (gd);

		if ( gateway->GetNumSecurity () + 1 > gd->maxsecurity ) {
            success = false;
			UplinkStrncpy ( failure, "There was not enough space to fit the security device into your gateway.", sizeof ( failure ) );
			game->GetWorld ()->GetPlayer ()->ChangeBalance ( sv->cost, "Refund from Uplink Corporation" );
		}
        else {

            game->GetWorld ()->GetPlayer ()->gateway.GiveHardware ( hwsale->title );

            Data *driver = new Data ();
		    driver->SetDetails ( DATATYPE_PROGRAM, 1, 0, 0, 1.0, SOFTWARETYPE_HWDRIVER );

            if      ( strcmp ( hwsale->title, "Gateway Self Destruct" ) == 0 )      driver->SetTitle ( "Gateway_Nuke" );
            else if ( strcmp ( hwsale->title, "Gateway Motion Sensor" ) == 0 )      driver->SetTitle ( "Motion_Sensor" );

		    game->GetWorld ()->GetPlayer ()->gateway.databank.PutData ( driver );

        }

    }

	//
	// Notify the player
	//

	std::ostrstream body;
	Message *msg = new Message ();
	msg->SetTo ( "PLAYER" );
	msg->SetFrom ( "Uplink Corporation" );

	if ( success ) {
		msg->SetSubject ( "Gateway upgrade complete" );
		body << "The following hardware item has been successfully installed "
		    "in your gateway machine : \n\n"
			<< hwsale->title
			<< '\x0';
	}
	else {
		msg->SetSubject ( "Gateway upgrade failed" );
		body << "The following hardware item was not installed "
				"in your gateway machine : \n\n"
			 << hwsale->title
			 << "\n\nReason: " << failure
			 << "\nYour money has been refunded."
			 << '\x0';
	}

	msg->SetBody ( body.str () );
	msg->Send ();

	body.rdbuf()->freeze( 0 );
	//delete [] body.str ();

	//
	// The engineer walks away
	//

	game->GetWorld ()->GetPlayer ()->gateway.DecrementProximity ();

}

void InstallHardwareEvent::RunWarning ()
{

	//
	// Notify the player
	//

	std::ostrstream body;
	body << "We are about to begin installation of the following hardware item "
		    "in your gateway machine : \n\n"
			<< hwsale->title
			<< "\n\n"
			<< "We anticipate this will take 30 - 60 minutes to complete."
			<< '\x0';
	Message *msg = new Message ();
	msg->SetTo ( "PLAYER" );
	msg->SetFrom ( "Uplink Corporation" );
	msg->SetSubject ( "Gateway upgrade commencing" );
	msg->SetBody ( body.str () );
	msg->Send ();

	body.rdbuf()->freeze( 0 );
	//delete [] body.str ();

	//
	// Someone is near the gateway
	//

	game->GetWorld ()->GetPlayer ()->gateway.IncrementProximity ();

}

char *InstallHardwareEvent::GetShortString ()
{

	size_t shortstringsize = strlen(hwsale->title) + 32;
	char *shortstring = new char [shortstringsize];
	UplinkSnprintf ( shortstring, shortstringsize, "HW Install of %s, version %d", hwsale->title, version );
	return shortstring;

}

char *InstallHardwareEvent::GetLongString ()
{

	std::ostrstream longstring;
	longstring << "Install Hardware Event\n"
			   << "Item : " << hwsale->title << "\n"
			   << "Version : " << version << "\n"
			   << '\x0';
	return longstring.str ();

}

bool InstallHardwareEvent::Load  ( FILE *file )
{

	LoadID ( file );

	hwsale = NULL;

	if ( !UplinkEvent::Load ( file ) ) return false;

	bool hwsaleavail;
	if ( !FileReadData ( &hwsaleavail, sizeof(hwsaleavail), 1, file ) ) return false;
	if ( hwsaleavail ) {
		hwsale = new Sale ();
		if ( !hwsale->Load ( file ) ) return false;
	}

	if ( !FileReadData ( &version, sizeof(version), 1, file ) ) return false;

	LoadID_END ( file );

	return true;

}

void InstallHardwareEvent::Save  ( FILE *file )
{

	SaveID ( file );

	UplinkEvent::Save ( file );

	bool hwsaleavail = (hwsale != NULL);
	fwrite ( &hwsaleavail, sizeof(hwsaleavail), 1, file );
	if ( hwsale ) hwsale->Save ( file );

	fwrite ( &version, sizeof(version), 1, file );

	SaveID_END ( file );

}

void InstallHardwareEvent::Print ()
{

	printf ( "Install Hardware Event : \n" );
	UplinkEvent::Print ();
	if ( hwsale ) hwsale->Print ();
	printf ( "Version: %d\n", version );

}

char *InstallHardwareEvent::GetID ()
{

	return "EVT_IHW";

}

int   InstallHardwareEvent::GetOBJECTID ()
{

	return OID_INSTALLHWEVENT;

}
