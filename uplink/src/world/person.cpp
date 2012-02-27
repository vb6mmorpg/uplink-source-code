// Person.cpp: implementation of the Person class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"

#include "game/game.h"

#include "world/world.h"
#include "world/message.h"
#include "world/person.h"
#include "world/player.h"
#include "world/vlocation.h"

#include "world/company/mission.h"
#include "world/computer/bankaccount.h"
#include "world/computer/bankcomputer.h"
#include "world/generator/missiongenerator.h"
#include "world/generator/numbergenerator.h"
#include "world/generator/worldgenerator.h"

#include "mmgr.h"


Person::Person()
{

	currentaccount = 0;
	age = -1;
	photoindex = 0;
    voiceindex = 0;
	UplinkStrncpy ( name, " ", sizeof ( name ) );
	UplinkStrncpy ( localhost, " ", sizeof ( localhost ) );
	UplinkStrncpy ( remotehost, " ", sizeof ( remotehost ) );
	UplinkStrncpy ( phonenumber, " ", sizeof ( phonenumber ) );

	STATUS = PERSON_STATUS_NONE;

	istargetable = true;

}

Person::~Person()
{

	DeleteLListData ( (LList <UplinkObject *> *) &messages );
	DeleteLListData ( &accounts );

}

void Person::SetName ( char *newname )
{

	UplinkAssert ( strlen (newname) < SIZE_PERSON_NAME );
	UplinkStrncpy ( name, newname, sizeof ( name ) );

	connection.SetOwner ( name );
	rating.SetOwner ( name );

}

void Person::SetAge	( int newage )
{

	age = newage;

}

void Person::SetPhotoIndex ( int newindex )
{

	photoindex = newindex;

}

void Person::SetVoiceIndex  ( int newindex )
{

    voiceindex = newindex;

}

void Person::SetLocalHost ( char *newip )
{

	UplinkAssert ( strlen (newip) < SIZE_VLOCATION_IP );
	UplinkStrncpy ( localhost, newip, sizeof ( localhost ) );

	UplinkAssert ( game->GetWorld ()->GetVLocation ( localhost ) );

}

void Person::SetRemoteHost ( char *newip )
{

	UplinkAssert ( strlen (newip) < SIZE_VLOCATION_IP );
	UplinkStrncpy ( remotehost, newip, sizeof ( remotehost ) );

	UplinkAssert ( game->GetWorld ()->GetVLocation ( remotehost ) );

}

void Person::SetPhoneNumber ( char *newphone )
{

	UplinkAssert ( strlen(newphone) < SIZE_VLOCATION_IP );
	UplinkStrncpy ( phonenumber, newphone, sizeof ( phonenumber ) );

	UplinkAssert ( game->GetWorld ()->GetVLocation ( phonenumber ) );

}

void Person::SetStatus ( int newSTATUS )
{

	STATUS = newSTATUS;

	// If the person is in charge of administering a company, replace him with a new person
	// Else it will be impossible to capture is voice and thus breaking in the servers of the company

	if ( STATUS != PERSON_STATUS_NONE )
		WorldGenerator::ReplaceAdminCompanies ( this );

}

int Person::GetStatus ( )
{

	return STATUS;

}

void Person::GiveMessage ( Message *message )
{

	messages.PutData ( message );

}

bool Person::IsConnected ()
{

	return ( strcmp ( localhost, remotehost ) != 0 );

}

Connection *Person::GetConnection ()
{

	return &connection;

}

VLocation *Person::GetLocalHost ()
{

	VLocation *vl = game->GetWorld ()->GetVLocation ( localhost );
	UplinkAssert ( vl );
	return vl;

}

VLocation *Person::GetRemoteHost ()
{

	VLocation *vl = game->GetWorld ()->GetVLocation ( remotehost );
	UplinkAssert ( vl );
	return vl;

}

void Person::SetCurrentAccount ( int index )
{

	currentaccount = index;

}

void Person::ChangeBalance ( int amount, char *description )
{

	if ( accounts.Size () > 0 ) {

		// Change the current account

		char *fullcurrentaccount = accounts.GetData (currentaccount);
		UplinkAssert (fullcurrentaccount);

		char ip [SIZE_VLOCATION_IP];
		char accno [16];
		sscanf ( fullcurrentaccount, "%s %s", ip, accno );

		BankAccount *ba = BankAccount::GetAccount ( ip, accno );
		if ( !ba ) {
			UplinkAbortArgs( "Cannot find bank account, name='%s', ip='%s', accno='%s'", name, ip, accno );
		}
		ba->ChangeBalance ( amount, description );


        // If the person has gone overdrawn, fine them

        if ( amount < 0 && ba->balance < 0 ) {
            if ( this == game->GetWorld ()->GetPlayer () ) {

                VLocation *vl = game->GetWorld ()->GetVLocation ( ip );
                UplinkAssert (vl);
                Computer *comp = vl->GetComputer ();
                UplinkAssert (comp);
                Company *company = game->GetWorld ()->GetCompany (comp->companyname);
                UplinkAssert (company);

                std::ostrstream reason;
                reason << "It has come to our attention that your bank account below has gone "
                          "overdrawn, without authorisation.\n\n"
                       << comp->name << "\n"
                       << "AccNo: " << accno << "\n"
                       << '\x0';

			    Date *date = new Date ();
			    date->SetDate ( &(game->GetWorld ()->date) );
			    date->AdvanceMinute ( TIME_TOPAYLEGALFINE );
			    date->SetDate ( 0, 0, 0, date->GetDay (), date->GetMonth (), date->GetYear () );			// Round to midnight

                Mission *payfine = MissionGenerator::Generate_PayFine ( this, company, 500, date, reason.str() );
                game->GetWorld ()->GetPlayer ()->GiveMission ( payfine );

				delete date;

				reason.rdbuf()->freeze( 0 );
                //delete [] reason.str();

            }
        }

	}

}

int Person::CreateNewAccount ( char *bankip, char *accname, char *password, int balance, int loan )
{

	// Get the bank in question

	VLocation *vl = game->GetWorld ()->GetVLocation ( bankip );
	UplinkAssert (vl);
	Computer *comp = game->GetWorld ()->GetComputer ( vl->computer );
	UplinkAssert (comp);
	BankComputer *bank = (BankComputer *) comp;

	// Add the account in

	int accountnumber = bank->CreateBankAccount ( accname, password, 3, balance + loan, loan );

	// Add it into the person

	size_t newaccountsize = 32;
	char *newaccount = new char [newaccountsize];
	UplinkSnprintf ( newaccount, newaccountsize, "%s %d", bankip, accountnumber );
	accounts.PutData ( newaccount );

	// Return the account number

	return accountnumber;

}

int Person::GetBalance ()
{

	int result = 0;

	for ( int i = 0; i < accounts.Size (); ++i ) {

		char ip [SIZE_VLOCATION_IP];
		char accno [16];
		sscanf ( accounts.GetData (i), "%s %s", ip, accno );

		BankAccount *ba = BankAccount::GetAccount ( ip, accno );
		UplinkAssert (ba);
		result += ba->balance;

	}

	return result;

}

void Person::SetIsTargetable ( bool newvalue )
{

	istargetable = newvalue;

}

bool Person::HasMessageLink ( const char *newip )
{

	for ( int ii = 0; ii < messages.Size (); ii++ )
		if ( messages.ValidIndex ( ii ) ) {
			LList<char*> *links = &(messages.GetData ( ii )->links);
			for ( int i = 0; i < links->Size () ; i++ )
				if ( links->ValidIndex ( i ) )
					if ( strcmp ( newip, links->GetData ( i ) ) == 0 )
						return true;
		}

	return false;

}

bool Person::Load  ( FILE *file )
{

	LoadID ( file );

	if ( !LoadDynamicStringStatic ( name, SIZE_PERSON_NAME, file ) ) return false;
	if ( !LoadDynamicStringStatic ( localhost, SIZE_VLOCATION_IP, file ) ) return false;
	if ( !LoadDynamicStringStatic ( remotehost, SIZE_VLOCATION_IP, file ) ) return false;
	if ( !LoadDynamicStringStatic ( phonenumber, SIZE_VLOCATION_IP, file ) ) return false;

	if ( !game->GetWorld ()->locations.LookupTree ( localhost ) ) {
		UplinkPrintAbortArgs ( "WARNING: Person::Load, Localhost IP '%s' already existing", localhost );
		return false;
	}

	if ( !game->GetWorld ()->locations.LookupTree ( remotehost ) ) {
		UplinkPrintAbortArgs ( "WARNING: Person::Load, Remotehost IP '%s' already existing", remotehost );
		return false;
	}

	if ( !FileReadData ( &age, sizeof (age), 1, file ) ) return false;
	if ( !FileReadData ( &photoindex, sizeof (photoindex), 1, file ) ) return false;
    if ( !FileReadData ( &voiceindex, sizeof (voiceindex), 1, file ) ) return false;
    if ( !FileReadData ( &currentaccount, sizeof (currentaccount), 1, file ) ) return false;
	if ( !FileReadData ( &istargetable, sizeof (istargetable), 1, file ) ) return false;
	if ( !FileReadData ( &STATUS, sizeof(STATUS), 1, file ) ) return false;

	if ( !LoadLList ( (LList <UplinkObject *> *) &messages, file ) ) return false;
	if ( !LoadLList ( &accounts, file ) ) return false;

	if ( !connection.Load ( file ) ) return false;
	if ( !rating.Load ( file ) ) return false;

	LoadID_END ( file );

	return true;

}

void Person::Save  ( FILE *file )
{

	SaveID ( file );

	SaveDynamicString ( name, SIZE_PERSON_NAME, file );
	SaveDynamicString ( localhost, SIZE_VLOCATION_IP, file );
	SaveDynamicString ( remotehost, SIZE_VLOCATION_IP, file );
	SaveDynamicString ( phonenumber, SIZE_VLOCATION_IP, file );

	fwrite ( &age,		  sizeof (age),		    1, file );
	fwrite ( &photoindex, sizeof (photoindex),  1, file );
    fwrite ( &voiceindex, sizeof (voiceindex),  1, file );
	fwrite ( &currentaccount, sizeof (currentaccount), 1, file );
	fwrite ( &istargetable,	  sizeof (istargetable), 1, file );
	fwrite ( &STATUS, sizeof(STATUS), 1, file );

	SaveLList ( (LList <UplinkObject *> *) &messages, file );
	SaveLList ( &accounts, file );

	connection.Save ( file );
	rating.Save ( file );

	SaveID_END ( file );

}

void Person::Print ()
{

	printf ( "Person : " );
	printf ( "name = %s, Age = %d, photoindex = %d, voiceindex = %d, status = %d\n", name, age, photoindex, voiceindex, STATUS );
	printf ( "LocalHost : %s, RemoteHost : %s, Phone : %s\n", localhost, remotehost, phonenumber );

	printf ( "Messages : \n" );
	PrintLList ( (LList <UplinkObject *> *) &messages );

	PrintLList ( &accounts );
	printf ( "currentaccount:%d\n", currentaccount );

	connection.Print ();
	rating.Print ();

	printf ( "Is Targetable? %d\n", istargetable );

}

void Person::Update ()
{

	if ( strcmp ( name, "PLAYER" ) != 0 ) {

		//
		// This person reads his mail
		//

		if ( messages.Size () > 0 ) {

			Message *msg = messages.GetData (0);

			if ( strcmp ( msg->from, "PLAYER" ) == 0 ) {

				// This is a message from the player - most likely a "completed mission" email
				// But it may be a mail to a "special" company
				// But then again it might just be for the company

				bool handled = false;

				if ( strcmp ( msg->to, "internal@ARC.net" ) == 0 )
#ifdef DEMOGAME
                    handled = game->GetWorld ()->demoplotgenerator.PlayerContactsARC ( msg );
#else
					handled = game->GetWorld ()->plotgenerator.PlayerContactsARC ( msg );
#endif

				else if ( strcmp ( msg->to, "internal@Arunmor.net" ) == 0 )
					handled = game->GetWorld ()->plotgenerator.PlayerContactsARUNMOR ( msg );

				if ( !handled ) {

					// Go through each of the players missions and see if this email satisfies any of them

					for ( int i = 0; i < game->GetWorld ()->GetPlayer ()->missions.Size (); ++i ) {

						Mission *mis = game->GetWorld ()->GetPlayer ()->missions.GetData (i);
						UplinkAssert (mis);

						if ( strcmp ( mis->contact, name ) == 0 &&
                             strstr ( msg->GetBody (), mis->description ) != NULL &&                // ie the name of the mission appears in your email
                             MissionGenerator::IsMissionComplete ( mis, this, msg ) ) {

							game->GetWorld ()->GetPlayer ()->missions.RemoveData (i);
                            delete mis;

                        }

					}

				}

			}
			else {

				// Message is not from the player

			}

			// Finished dealing with this email
			delete msg;
			messages.RemoveData (0);

		}

	}

}

char *Person::GetID ()
{

	return "PERSON";

}

int Person::GetOBJECTID ()
{

	return OID_PERSON;

}
