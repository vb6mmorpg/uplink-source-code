// Bank.cpp: implementation of the Bank class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "gucci.h"

#include "app/app.h"
#include "app/serialise.h"
#include "app/globals.h"

#include "game/game.h"
#include "game/data/data.h"

#include "world/world.h"
#include "world/player.h"
#include "world/vlocation.h"
#include "world/message.h"
#include "world/computer/computer.h"
#include "world/computer/bankcomputer.h"
#include "world/computer/bankaccount.h"
#include "world/scheduler/eventscheduler.h"
#include "world/scheduler/bankrobberyevent.h"
#include "world/scheduler/seizegatewayevent.h"
#include "world/scheduler/arrestevent.h"
#include "world/generator/missiongenerator.h"

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

/*
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
*/
void BankAccount::ChangeBalance ( int amount, char *description )
{

	balance += amount;

	// Log this action

	// Set the log to the appropriate type
	AccessLog *source_log = new AccessLog ();
	if ( amount > 0 ) {
		source_log->SetProperties ( &(game->GetWorld ()->date), "unknown", name, LOG_NOTSUSPICIOUS, LOG_TYPE_DEPOSIT );
	} else {
		source_log->SetProperties ( &(game->GetWorld ()->date), "unknown", name, LOG_NOTSUSPICIOUS, LOG_TYPE_WITHDRAWAL );
	}
	
	if ( description )  source_log->SetData1 ( description );
	else				source_log->SetData1 ( "Unknown" );

	// Set AMOUNT correctly for DATA2 Insertion
	char *am = new char[12];
	if ( amount < 0 ) { amount = -amount; }
	sprintf(am,"%d",amount);

	// Store the value here
	source_log->SetData2(am);

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


void BankAccount::CheckBalance ( char *ip )
{
	// Is this account flagged as suspicious?
	bool suspicious = false;
	bool fraud = false;

#ifdef DEBUG_BANK_AUDIT
	printf("AUDIT %s %d\n", ip, accountnumber);
#endif

	for ( int i = 0; i < log.logs.Size(); i++ )
	{
		if ( log.logs.ValidIndex(i) )
		{
			AccessLog *thislog = log.logs.GetData(i);
			UplinkAssert ( thislog );

			if ( (thislog->TYPE == LOG_TYPE_DEPOSIT || thislog->TYPE == LOG_TYPE_WITHDRAWAL) && thislog->data3 )
			{
				if ( strcmp(thislog->data1, "Audit") == 0 && strcmp(thislog->data3,"Suspicious") == 0 ) {
					thislog->SetData3("Checked");
					suspicious = true;
#ifdef DEBUG_BANK_AUDIT
					printf("   This account has been flagged as suspicious\n");
#endif
				}
			}
		}
	}

	int cBalance = 0;

	for ( int i = 0; i < log.logs.Size(); i++ )
	{
		if ( log.logs.ValidIndex(i) )
		{
			AccessLog *thislog = log.logs.GetData(i);
			UplinkAssert ( thislog );

			bool suspiciouslog = false;
			int cTransfer = 0;
			// These two log types are always suspicious, so investigate them as a matter of cause, also undelete everything if we are suspicious of the account
			if ( log.internallogs.ValidIndex(i) && (thislog->TYPE == LOG_TYPE_WITHDRAWAL || thislog->TYPE == LOG_TYPE_DEPOSIT || suspicious ) ) {
				thislog->SetProperties( log.internallogs.GetData(i) );
			}

			if ( thislog->TYPE == LOG_TYPE_TRANSFERTO || thislog->TYPE == LOG_TYPE_WITHDRAWAL )
			{
				sscanf( thislog->data2, "%d", &cTransfer );
				cBalance -= cTransfer;
			}
			else if ( thislog->TYPE == LOG_TYPE_TRANSFERFROM || thislog->TYPE == LOG_TYPE_DEPOSIT )
			{
				sscanf( thislog->data2, "%d", &cTransfer );
				cBalance += cTransfer;
			}

			if ( cTransfer > AMOUNT_SUSPICIOUS_LOG ) { suspiciouslog = true; }

			// If we are suspicious of the account or the log, dig deeper
			if ( (suspicious || suspiciouslog) && (thislog->TYPE == LOG_TYPE_TRANSFERTO || thislog->TYPE == LOG_TYPE_TRANSFERFROM) )
			{
				// Does a matching log exist on the other machine?
			    char otherip [SIZE_VLOCATION_IP];
			    char otheraccno [16];
			    sscanf ( thislog->data1, "%s %s", otherip, otheraccno );

				int otheracc = 0;
				sscanf ( otheraccno, "%d", &otheracc );

#ifdef DEBUG_BANK_AUDIT
				printf("   Checking Transfer (%s %d) <-> (%s %d) %d\n", ip, accountnumber, otherip, otheracc, cTransfer);
#endif
				// Fraud is set to true if:
				//  * The other account is invalid
				//  * There is no matching transfer log
				BankAccount *other_acc = BankAccount::GetAccount(otherip, otheraccno);
				if ( other_acc ) {
					if ( thislog->TYPE == LOG_TYPE_TRANSFERTO && !(HasTransferOccured(ip, otherip, otheracc, cTransfer, false)) ) {
						fraud = true;
#ifdef DEBUG_BANK_AUDIT
						printf("   TransferTo: No Matching TransferFrom\n");
#endif
					}
					if ( thislog->TYPE == LOG_TYPE_TRANSFERFROM && !(other_acc->HasTransferOccured(otherip, ip, accountnumber, cTransfer, false)) ) {
						fraud = true;
#ifdef DEBUG_BANK_AUDIT
						printf("   TransferFrom: No Matching TransferTo\n");
#endif
					}
				} else { fraud = true; }
			}
		}
	}

#ifdef DEBUG_BANK_AUDIT
	printf("   cBalance: %d\n   Balance: %d\n", cBalance, balance- loan);
	if ( fraud ) { printf("   Transfer Fraud Detected\n"); }
#endif
	// A mismatched balance, or a failed fraud check will trigger these events
	if ( cBalance != (balance - loan) || fraud )
	{
        VLocation *vl = game->GetWorld ()->GetVLocation ( ip );
        UplinkAssert (vl);
        Computer *comp = vl->GetComputer ();
        UplinkAssert (comp);
        Company *company = game->GetWorld ()->GetCompany (comp->companyname);
        UplinkAssert (company);
		// Maybe do something here

		int diff = (balance - loan) - cBalance;
#ifdef DEBUG_BANK_AUDIT
		printf("   Diff: %d\n", diff);
#endif
		if ( diff > AMOUNT_TRIGGER_ARREST || fraud )
		{
			// This account appears to have more money that it should
			// because we couldnt account for it all
			// Its a large amount so prosecute the owner
			Person *person = GetPerson();
			UplinkAssert(person);
			if ( strcmp(person->name,"PLAYER") == 0 )
			{
				char *reason = new char[256];
				sprintf(reason,"attempting to defraud the %s", comp->name);

				Date duedate;
				duedate.SetDate ( &(game->GetWorld ()->date) );
				duedate.AdvanceMinute ( TIME_LEGALACTION );

				Date warningdate;
				warningdate.SetDate ( &duedate );
				warningdate.AdvanceMinute ( TIME_LEGALACTION_WARNING * -1 );

				SeizeGatewayEvent *ae = new SeizeGatewayEvent();
				ae->SetName ( person->name );
				ae->SetReason( reason );
				ae->SetRunDate( &duedate );

				game->GetWorld ()->scheduler.ScheduleWarning ( ae, &warningdate );
				game->GetWorld ()->scheduler.ScheduleEvent ( ae );
			} else {
				char *reason = new char[256];
				sprintf(reason,"attempting to defraud the %s", comp->name);

				Date duedate;
				duedate.SetDate ( &(game->GetWorld ()->date) );
				duedate.AdvanceMinute ( TIME_LEGALACTION );

				Date warningdate;
				warningdate.SetDate ( &duedate );
				warningdate.AdvanceMinute ( TIME_LEGALACTION_WARNING * -1 );

				ArrestEvent *ae = new ArrestEvent();
				ae->SetName ( person->name );
				ae->SetReason( reason );
				ae->SetIP ( ip );
				ae->SetRunDate( &duedate );

				game->GetWorld ()->scheduler.ScheduleWarning ( ae, &warningdate );
				game->GetWorld ()->scheduler.ScheduleEvent ( ae );

			}
			// Fix the account
			balance -= diff;
		} else if ( diff > 0 ) {
			if ( GetPerson() == game->GetWorld()->GetPlayer() ) {

                std::ostrstream reason;
                reason << "A recent audit of our transactions has shown that the following account has "
                          "an unaccounted balance surplus of " << diff << " credits.\n\n"
                       << comp->name << "\n"
                       << "AccNo: " << accountnumber << "\n"
                       << '\x0';

			    Date *date = new Date ();
			    date->SetDate ( &(game->GetWorld ()->date) );
			    date->AdvanceMinute ( TIME_TOPAYLEGALFINE );
			    date->SetDate ( 0, 0, 0, date->GetDay (), date->GetMonth (), date->GetYear () );			// Round to midnight

                Mission *payfine = MissionGenerator::Generate_PayFine ( GetPerson(), company, diff, date, reason.str() );
                game->GetWorld ()->GetPlayer ()->GiveMission ( payfine );

				delete date;

				reason.rdbuf()->freeze( 0 );
                //delete [] reason.str();

				// Balance the account
				AccessLog *log = new AccessLog();
				log->SetProperties(&(game->GetWorld()->date), ip, name, LOG_NOTSUSPICIOUS,LOG_TYPE_DEPOSIT);
				log->SetData1("Audit");
				log->SetData3("Suspicious");

				char *temp = new char[12];
				sprintf(temp,"%d", diff);
				log->SetData2(temp);

				this->log.AddLog(log);
			}
			else
			{
				// Assume the fine is paid, reset the balance
				balance -= diff;

				// Flag a re-audit check
				AccessLog *log = new AccessLog();
				log->SetProperties(&(game->GetWorld()->date), ip, name, LOG_NOTSUSPICIOUS,LOG_TYPE_WITHDRAWAL);
				log->SetData1("Audit");
				log->SetData2("0");
				log->SetData3("Suspicious");

				this->log.AddLog(log);
			}
		} else if ( diff < 0 ) {
			// This account appears to have less money than it should
			// because we accounted for more than we have
			AccessLog *log = new AccessLog();
			log->SetProperties(&(game->GetWorld()->date), ip, name, LOG_NOTSUSPICIOUS,LOG_TYPE_WITHDRAWAL);
			log->SetData1("Audit");
			
			char *temp = new char[12];
			sprintf(temp,"%d", -diff);
			log->SetData2(temp);

			log->SetData3("Suspicious");

			this->log.AddLog(log);

				Message *mail = new Message();
				mail->SetTo(GetPerson()->name);
				mail->SetDate(&(game->GetWorld()->date));
				mail->SetSubject("Account Audit");

				std::ostrstream reason;
                reason << "A recent audit of our transactions has shown that the following account has "
                          "an unaccounted balance shortfall of " << diff << " credits.\n\n"
						<< comp->name << "\n"
						<< "AccNo: " << accountnumber << "\n\n"
						<< "It appears as though unauthorised activity has been undertaken on your account "
						<< "and we will be investigating this more thoroughly. In the meantime, we suggest "
						<< "that you change your password and review your recent transactions.\n"
						<< '\x0';

				mail->SetBody(reason.str());
				reason.rdbuf()->freeze( 0 );
				
				char *fromname = new char[SIZE_PERSON_NAME];
				UplinkSnprintf(fromname, SIZE_PERSON_NAME, "internal@%s.net", comp->companyname);
				mail->SetFrom(fromname);

				mail->Send();
		}
		// Now force the acount to balance
		//balance = cBalance + loan;
	}

}