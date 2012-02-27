// Bank.cpp: implementation of the Bank class.
//
//////////////////////////////////////////////////////////////////////

#include "gucci.h"

#include "app/app.h"
#include "app/serialise.h"
#include "app/globals.h"

#include "game/game.h"
#include "game/data/data.h"

#include "world/world.h"
#include "world/player.h"
#include "world/vlocation.h"
#include "world/computer/computer.h"
#include "world/computer/bankcomputer.h"
#include "world/computer/bankaccount.h"
#include "world/scheduler/eventscheduler.h"
#include "world/scheduler/bankrobberyevent.h"

#include "mmgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////



BankAccount::BankAccount ()
{

	balance = 0;
	accountnumber = 0;
	memset ( password, 0, sizeof ( password ) );

}

BankAccount::~BankAccount ()
{
}
	
BankAccount *BankAccount::GetAccount ( char *bankip, char *accno )
{

	VLocation *vl = game->GetWorld ()->GetVLocation ( bankip );
	if ( !vl ) return NULL;

	Computer *comp = vl->GetComputer ();
	if ( !comp ) return NULL;
	if ( !(comp->GetOBJECTID () == OID_BANKCOMPUTER ) ) return NULL;
	BankComputer *bank = (BankComputer *) comp;

	BankAccount *account = bank->accounts.GetData ( accno );
	return account;

}

bool BankAccount::TransferMoney ( char *source_ip, char *source_accno,
								  char *target_ip, char *target_accno,
								  int amount, Person *person )
{

	UplinkAssert (person);

	BankAccount *source_acc = BankAccount::GetAccount ( source_ip, source_accno );
	BankAccount *target_acc = BankAccount::GetAccount ( target_ip, target_accno );

	char source_data [128];
	char target_data [128];
	UplinkSnprintf ( source_data, sizeof ( source_data ), "%s %s", target_ip, target_accno );
	UplinkSnprintf ( target_data, sizeof ( target_data ), "%s %s", source_ip, source_accno );

	char s_amount [16];
	UplinkSnprintf ( s_amount, sizeof ( s_amount ), "%d", amount );

    //
    // If the player is trying to rob money,
    // Make sure we know about it

    if ( !game->GetWorld ()->GetPlayer ()->IsPlayerAccount ( source_ip, source_accno ) &&
          game->GetWorld ()->GetPlayer ()->IsPlayerAccount ( target_ip, target_accno ) &&
          person == game->GetWorld ()->GetPlayer () ) {

        Date rundate;
        rundate.SetDate ( &game->GetWorld ()->date );
        rundate.AdvanceMinute ( TIME_TOCOVERBANKROBBERY );

        BankRobberyEvent *bre = new BankRobberyEvent ();
        bre->SetDetails ( source_ip, source_accno, target_ip, target_accno, amount, &game->GetWorld ()->date );
        bre->SetRunDate ( &rundate );

        game->GetWorld ()->scheduler.ScheduleEvent ( bre );

    }

	if ( source_acc && target_acc && amount > 0 && source_acc->balance >= amount ) {

		// Change the accounts

		source_acc->balance -= amount;
		target_acc->balance += amount;

		// Log on the source account
		AccessLog *source_log = new AccessLog ();
		source_log->SetProperties ( &(game->GetWorld ()->date), source_ip, person->name, LOG_NOTSUSPICIOUS, LOG_TYPE_TRANSFERTO );
		source_log->SetData1 ( source_data );
		source_log->SetData2 ( s_amount );
		source_log->SetData3 ( target_acc->name );
		source_acc->log.AddLog ( source_log );

		// Log on the target account
		AccessLog *target_log = new AccessLog ();
		target_log->SetProperties ( &(game->GetWorld ()->date), source_ip, person->name, LOG_NOTSUSPICIOUS, LOG_TYPE_TRANSFERFROM );
		target_log->SetData1 ( target_data );
		target_log->SetData2 ( s_amount );
		target_log->SetData3 ( source_acc->name );
		target_acc->log.AddLog ( target_log );

		return true;

	}
	else {
	
		return false;

	}


}

void BankAccount::SetBalance ( int newbalance, int newloan )
{

	balance = newbalance;
	loan = newloan;

}

void BankAccount::SetOwner ( char *newname )
{
	
	UplinkAssert ( strlen(newname) < SIZE_PERSON_NAME );
	UplinkStrncpy ( name, newname, sizeof ( name ) );

}

void BankAccount::SetSecurity ( char *newpassword, int newsecurity )
{

	UplinkAssert ( strlen(newpassword) < sizeof(password) );
	UplinkStrncpy ( password, newpassword, sizeof ( password ) );

	security = newsecurity;

}

void BankAccount::SetAccNumber ( int newaccnumber )
{

	accountnumber = newaccnumber;

}

void BankAccount::ChangeBalance ( int amount, char *description )
{

	balance += amount;

	// Log this action

	AccessLog *source_log = new AccessLog ();
	source_log->SetProperties ( &(game->GetWorld ()->date), "unknown", name, LOG_NOTSUSPICIOUS, LOG_TYPE_TEXT );
	
	if ( description )  source_log->SetData1 ( description );
	else				source_log->SetData1 ( "Unknown" );

	log.AddLog ( source_log );


}

bool BankAccount::HasTransferOccured ( char *s_ip, char *t_ip, int t_accno, int amount, bool partial )
{

	// 
	// Convert integer values to strings
	//

	char amount_s [16];
	UplinkSnprintf ( amount_s, sizeof ( amount_s ), "%d", amount );

	char s_accno_s [16];
	UplinkSnprintf ( s_accno_s, sizeof ( s_accno_s ), "%d", accountnumber );

	char t_accno_s [16];
	UplinkSnprintf ( t_accno_s, sizeof ( t_accno_s ), "%d", t_accno );

	bool progress = false;
	int i;

	//
	// Check the source account (this)
	//

	for ( i = 0; i < log.internallogs.Size (); ++i ) {
		if ( log.internallogs.ValidIndex (i) ) {
		
			AccessLog *al = log.internallogs.GetData (i);
			UplinkAssert ( al );

            if ( al->TYPE == LOG_TYPE_TRANSFERTO ) {

    			UplinkAssert ( al->data1 );
	    		UplinkAssert ( al->data2 );

				if ( strstr ( al->data1, t_ip )      != 0 &&
				     strstr ( al->data1, t_accno_s ) != 0 &&
				     strcmp ( al->data2, amount_s )  == 0 ) {

				    // This is the correct log in the source account

				    progress = true;
				    break;

                }

			}
			
		}
	}

	if ( partial && progress ) return true;
	if ( !partial && !progress ) return false;
	progress = false;

	//
	// Check the target account
	//

	BankAccount *target_acc = BankAccount::GetAccount ( t_ip, t_accno_s );

	if ( !target_acc ) return false;

	for ( i = 0; i < target_acc->log.internallogs.Size (); ++i ) {
		if ( target_acc->log.internallogs.ValidIndex (i) ) {
		
			AccessLog *al = target_acc->log.internallogs.GetData (i);
			UplinkAssert ( al );


            if ( al->TYPE == LOG_TYPE_TRANSFERFROM ) {

			    UplinkAssert ( al->data1 );
			    UplinkAssert ( al->data2 );

				if ( strstr ( al->data1, s_ip )      != 0 &&
				     strstr ( al->data1, s_accno_s ) != 0 &&
				     strcmp ( al->data2, amount_s )  == 0 ) {
			
				    // This is the correct log in the target account

				    progress = true;
				    break;

                }

			}
			
		}
	}

	return progress;

}

bool BankAccount::HasTransferOccured ( char *person, int amount )
{

	char amount_s [16];
	UplinkSnprintf ( amount_s, sizeof ( amount_s ), "%d", amount );

	//
	// Go throught each transfer trying to find a match for the amount
	//

	for ( int i = 0; i < log.internallogs.Size (); ++i ) {
		if ( log.internallogs.ValidIndex (i) ) {
		
			AccessLog *al = log.internallogs.GetData (i);
			UplinkAssert ( al );

            if ( al->TYPE == LOG_TYPE_TRANSFERFROM ) {

          	    UplinkAssert ( al->data2 );
			    UplinkAssert ( al->data3 );

                if ( strcmp ( al->data2, amount_s ) == 0 &&
				     strcmp ( al->data3, person )   == 0 ) {

				    // This is it
				    return true;

                }

			}

		}
	}

	return false;

}

Person *BankAccount::GetPerson ()
{

	Person *accountowner = NULL;

	Player *pl = game->GetWorld ()->GetPlayer ();
	for ( int i = 0; i < pl->accounts.Size (); ++i ) {
		char ip [SIZE_VLOCATION_IP];
		char accno [16];
		sscanf ( pl->accounts.GetData (i), "%s %s", ip, accno );

		BankAccount *ba = BankAccount::GetAccount ( ip, accno );
		if ( ba == this ) {
			accountowner = pl;
			break;
		}
	}

	if ( !accountowner )
		accountowner = game->GetWorld ()->GetPerson ( name );

	return accountowner;

}

bool BankAccount::Load  ( FILE *file )
{

	LoadID ( file );
	
	if ( !LoadDynamicStringStatic ( name, SIZE_PERSON_NAME, file ) ) return false;

	if ( strcmp( game->GetLoadedSavefileVer(), "SAV59" ) >= 0 ) {
		if ( !LoadDynamicStringStatic ( password, sizeof(password), file ) ) return false;
	}
	else {
		if ( !FileReadData ( password, sizeof(password), 1, file ) ) {
			password [ sizeof(password) - 1 ] = '\0';
			return false;
		}
		password [ sizeof(password) - 1 ] = '\0';
	}
	if ( !FileReadData ( &security, sizeof(security), 1,file ) ) return false;
	if ( !FileReadData ( &accountnumber, sizeof(accountnumber), 1, file ) ) return false;
	if ( !FileReadData ( &balance, sizeof(balance), 1, file ) ) return false;
	if ( !FileReadData ( &loan, sizeof(loan), 1, file ) ) return false;

	if ( !log.Load ( file ) ) return false;

	LoadID_END ( file );

	return true;

}

void BankAccount::Save  ( FILE *file )
{

	SaveID ( file );

	SaveDynamicString ( name, SIZE_PERSON_NAME, file );

	SaveDynamicString ( password, sizeof(password), file );
	fwrite ( &security, sizeof(security), 1,file );
	fwrite ( &accountnumber, sizeof(accountnumber), 1, file );
	fwrite ( &balance, sizeof(balance), 1, file );
	fwrite ( &loan, sizeof(loan), 1, file );

	log.Save ( file );

	SaveID_END ( file );

}

void BankAccount::Print ()
{
	
	printf ( "Bank Account : " );
	printf ( "\tOwner    : Name:%s\n", name );
	printf ( "\tSecurity : Password:%s, level:%d\n", password, security );
	printf ( "\tAcc No   : %d\n", accountnumber );
	printf ( "\tBalance  : %d, Loan:%d\n", balance, loan );

	log.Print ();

}

	
char *BankAccount::GetID ()
{

	return "ACCOUNT";

}

int  BankAccount::GetOBJECTID ()
{

	return OID_BANKACCOUNT;

}

