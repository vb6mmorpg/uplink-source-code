#include <stdio.h>

#include <strstream>

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"

#include "game/game.h"
#include "game/data/data.h"

#include "world/world.h"
#include "world/player.h"
#include "world/computer/computer.h"
#include "world/computer/bankcomputer.h"
#include "world/computer/recordbank.h"

#include "world/generator/worldgenerator.h"
#include "world/generator/recordgenerator.h"
#include "world/generator/namegenerator.h"
#include "world/generator/numbergenerator.h"

#include "mmgr.h"


void RecordGenerator::Initialise ()
{
}

void RecordGenerator::GenerateRecords ( char *personname )
{

	Person *person = game->GetWorld ()->GetPerson ( personname );
	UplinkAssert (person);

	int age = person->age;

	GenerateRecord_SocialSecurity ( personname, age );
	GenerateRecord_Criminal		  ( personname, age );
//	GenerateRecord_Medical		  ( personname, age );
	GenerateRecord_Academic		  ( personname, age );
	GenerateRecord_Financial	  ( personname, age );

}

void RecordGenerator::GenerateRecords_Player ( char *playername )
{

	// Social Security Record

	Record *socsec = new Record ();
	socsec->AddField ( RECORDBANK_NAME, playername );
	socsec->AddField ( "D.O.B", "24 - 3 - 1989" );
	socsec->AddField ( "Social Security", "14011401" );
	socsec->AddField ( "Marital Status", "Single" );
	socsec->AddField ( "Personal Status", "Self-Employed" );

	Computer *socdb = game->GetWorld ()->GetComputer ( "International Social Security Database" );
	UplinkAssert ( socdb );
	socdb->recordbank.AddRecordSorted ( socsec );


	// Criminal Record

	Record *crim = new Record ();
	crim->AddField ( RECORDBANK_NAME, playername );
	crim->AddField ( "Convictions", "None" );

	Computer *crimdb = game->GetWorld ()->GetComputer ( "Global Criminal Database" );
	UplinkAssert ( crimdb );
	crimdb->recordbank.AddRecordSorted ( crim );


	// Academic Record

	Record *ac = new Record ();
	ac->AddField ( RECORDBANK_NAME, playername );
	ac->AddField ( "IQ", "172" );
	ac->AddField ( "College", "Maths    -   A\n"
							  "Physics    -    A\n"
							  "Electronics    -   A\n"
							  "Computing    -    A" );
	ac->AddField ( "University", "Computer Science, Class 1" );
	ac->AddField ( "Other", "Registered Uplink Agent" );


	Computer *acdb = game->GetWorld ()->GetComputer ( "International Academic Database" );
	UplinkAssert ( acdb );
	acdb->recordbank.AddRecordSorted ( ac );

}

void RecordGenerator::GenerateRecord_SocialSecurity ( char *personname, int age )
{

	// Generate a Social Security Record
	// Add it into the International Social Security Database

	char dob [64];
	UplinkSnprintf ( dob, sizeof ( dob ), "%d - %d - %d", NumberGenerator::RandomNumber ( 28 ) + 1,
														   NumberGenerator::RandomNumber ( 12 ) + 1,
														   2010 - age );

	char socialsecurity [16];
	UplinkSnprintf ( socialsecurity, sizeof ( socialsecurity ), "%d", 10000000 + NumberGenerator::RandomNumber ( 99999999 ) );

	char maritalstatus [64];

	switch ( NumberGenerator::RandomNumber ( 5 ) ) {
		case 0		:		UplinkStrncpy ( maritalstatus, "Single", sizeof ( maritalstatus ) );			break;
		case 1		:		UplinkStrncpy ( maritalstatus, "Married", sizeof ( maritalstatus ) );			break;
		case 2		:		UplinkStrncpy ( maritalstatus, "Seperated", sizeof ( maritalstatus ) );			break;
		case 3		:		UplinkStrncpy ( maritalstatus, "Divorced", sizeof ( maritalstatus ) );			break;
		case 4		:		UplinkStrncpy ( maritalstatus, "Widowed", sizeof ( maritalstatus ) );			break;
	}

	char personalstatus [64];

	switch ( NumberGenerator::RandomNumber ( 5 ) ) {
		case 0		:		UplinkStrncpy ( personalstatus, "Employed", sizeof ( personalstatus ) );			break;
		case 1		:		UplinkStrncpy ( personalstatus, "Un-Employed", sizeof ( personalstatus ) );		break;
		case 2		:		UplinkStrncpy ( personalstatus, "Student", sizeof ( personalstatus ) );			break;
		case 3		:		UplinkStrncpy ( personalstatus, "Self-Employed", sizeof ( personalstatus ) );	break;
		case 4		:		UplinkStrncpy ( personalstatus, "Deceased", sizeof ( personalstatus ) );			break;
	}

	Record *soc = new Record ();
	soc->AddField ( RECORDBANK_NAME, personname );
	soc->AddField ( "Social Security", socialsecurity );
	soc->AddField ( "D.O.B", dob );
	soc->AddField ( "Marital Status", maritalstatus );
	soc->AddField ( "Personal Status", personalstatus );

	Computer *socsec = game->GetWorld ()->GetComputer ( "International Social Security Database" );
	UplinkAssert ( socsec );

	socsec->recordbank.AddRecordSorted ( soc, "Social Security" );

}

void RecordGenerator::GenerateRecord_Criminal ( char *personname, int age )
{

	// Generate a possible criminal past
	// Only a small percentage of people have any convictions

	std::ostrstream convictions;

	Person *person = game->GetWorld ()->GetPerson ( personname );
	UplinkAssert (person);

	if ( person->rating.uplinkrating == 0 ) {

		// Ordinary person

		if ( NumberGenerator::RandomNumber (100) < PERCENTAGE_PEOPLEWITHCONVICTIONS ) {

			// Greater age = more possible convictions

			int numconv = 0;
			int numconvictions = ( age / 10 ) + NumberGenerator::RandomNumber ( 4 );

			for ( int i = 0; i < numconvictions; ++i ) {

				switch ( (int) NumberGenerator::RandomNormalNumber ( 6, 6 ) ) {

					case 0  :			convictions << "Murder\n"; numconv++;					break;
					case 1  :			convictions << "Armed Robbery\n"; numconv++;			break;
					case 2  :			convictions << "Robbery\n"; numconv++;					break;
					case 3  :			convictions << "Petty theft\n"; numconv++;				break;
					case 4  :	break;
					case 5  :	break;
					case 6  :	break;
					case 7  :	break;
					case 8  :			convictions << "Disturbing the peace\n"; numconv++;		break;
					case 9  :			convictions << "Reckless driving\n"; numconv++;			break;
					case 10 :			convictions << "Man-slaughter\n"; numconv++;			break;
					case 11 :			convictions << "G-B-H\n"; numconv++;					break;
					case 12 :			convictions << "Rape\n"; numconv++;						break;

					default :
						UplinkAbort ( "RecordGenerator::GenerateRecord_Criminal, unrecognised conviction id" );
						break;

				}

			}

			if ( numconv == 0 )
				convictions << "None";

		}
		else {

			convictions << "None";

		}

	}
	else {

		// This guy is an Uplink Agent

		if ( NumberGenerator::RandomNumber (100) < PERCENTAGE_AGENTSWITHCONVICTIONS ) {

			// Greater rating = more possible convictions

			int numconv = 0;
			int numconvictions = ( person->rating.uplinkrating ) + NumberGenerator::RandomNumber ( 2 );

			for ( int i = 0; i < numconvictions; ++i ) {

				switch ( (int) NumberGenerator::RandomNormalNumber ( 6, 6 ) ) {

					case 0  :			convictions << "High tech fraud\n"; numconv++;							break;
					case 1  :			convictions << "Willful destruction of data\n"; numconv++;				break;
					case 2  :			convictions << "Theft of sensitive data\n"; numconv++;					break;
					case 3  :			convictions << "Unlawful access of classified data\n"; numconv++;		break;
					case 4  :	break;
					case 5  :	break;
					case 6  :	break;
					case 7  :	break;
					case 8  :			convictions << "Falsifying academic results\n"; numconv++;				break;
					case 9  :			convictions << "Illegal modification of credit rating\n"; numconv++;	break;
					case 10 :			convictions << "Illegally transfering funds\n"; numconv++;				break;
					case 11 :			convictions << "Credit card fraud\n"; numconv++;						break;
					case 12 :			convictions << "Destruction of financial data\n"; numconv++;			break;

					default :
						UplinkAbort ( "RecordGenerator::GenerateRecord_Criminal, unrecognised conviction id" );
						break;

				}

			}

			if ( numconv == 0 )
				convictions << "None";

		}
		else {

			convictions << "None";

		}

	}

	convictions << '\x0';

	Record *crim = new Record ();
	crim->AddField ( RECORDBANK_NAME, personname );
	crim->AddField ( "Convictions", convictions.str () );

	Computer *crimdb = game->GetWorld ()->GetComputer ( "Global Criminal Database" );
	UplinkAssert ( crimdb );

	crimdb->recordbank.AddRecordSorted ( crim );

	convictions.rdbuf()->freeze( 0 );
	//delete [] convictions.str ();

}

void RecordGenerator::GenerateRecord_Medical ( char *personname, int age )
{

	Record *rec = new Record ();
	rec->AddField ( RECORDBANK_NAME, personname );
	rec->AddField ( "Known alergies", "None" );
	rec->AddField ( "History", "None" );

	Computer *meddb = game->GetWorld ()->GetComputer ( "Central Medical Database" );
	UplinkAssert ( meddb );

	meddb->recordbank.AddRecordSorted ( rec );

}

void RecordGenerator::GenerateRecord_Academic ( char *personname, int age )
{

	// Generate an academic history based on age

	int brains = (int) NumberGenerator::RandomNormalNumber ( 50, 50 );					// 0 = thick, 50 = average, 100 = rocket scientist

	std::ostrstream collegequals;
	std::ostrstream universityquals;
	std::ostrstream otherquals;

	if ( age >= 16 ) {

		// Has some college quals

		LList <char *> possiblequals;
		possiblequals.PutData ( "Science" );
		possiblequals.PutData ( "Maths" );
		possiblequals.PutData ( "Physics" );
		possiblequals.PutData ( "Biology" );
		possiblequals.PutData ( "Art" );
		possiblequals.PutData ( "History ");
		possiblequals.PutData ( "Psychology" );
		possiblequals.PutData ( "Electronics" );
		possiblequals.PutData ( "Computing" );
		possiblequals.PutData ( "Media studies" );
		possiblequals.PutData ( "Sociology" );

		char grades [] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G' };

		int numquals = NumberGenerator::RandomNumber ( brains / 10 );

		if ( numquals > 0 ) {

			for ( int i = 0; i < numquals; ++i ) {

				int thisqualindex = NumberGenerator::RandomNumber ( possiblequals.Size () );
				UplinkAssert ( possiblequals.ValidIndex (thisqualindex) );

				int grade = (int) ( 1 + NumberGenerator::RandomNormalNumber ( 100.0f - brains, 40.0f ) / 15 );
				if ( grade < 0 ) grade = 0;
				if ( grade > 6 ) grade = 6;

				collegequals << possiblequals.GetData (thisqualindex) << "      " << grades [grade] << "\n";

				possiblequals.RemoveData (thisqualindex);

			}

		}
		else {

			collegequals << "None";

		}

	}
	else {

		collegequals << "None";

	}

	collegequals << '\x0';

	if ( age >= 21 && brains > 50 ) {

		// Has a degree

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
				UplinkAbort ( "RecordGenerator::GenerateRecord_Academic, error in Degree Type" );

		}

		int degreelevel = 1 + ( 100 - brains ) / 25;

		universityquals << degreesubject << ", Class " << degreelevel;

	}
	else {

		// Does not have a degree
		universityquals << "None";

	}

	universityquals << '\x0';

	if ( age > 18 && brains > 50 ) {

		// Generate qualification

		char qualification [64];

		switch ( NumberGenerator::RandomNumber ( 5 ) ) {

			case 0 :		UplinkStrncpy ( qualification, "Diploma in Computing", sizeof ( qualification ) );			break;
			case 1 :		UplinkStrncpy ( qualification, "IEEE Accreditation", sizeof ( qualification ) );		    break;
			case 2 :		UplinkStrncpy ( qualification, "Certified systems engineer", sizeof ( qualification ) );	break;
			case 3 :		UplinkStrncpy ( qualification, "Btec in IT", sizeof ( qualification ) );					break;
			case 4 :		UplinkStrncpy ( qualification, "Systems Engineering diploma", sizeof ( qualification ) );	break;

			default:
				UplinkAbort ( "RecordGenerator::GenerateRecord_Academic, error in Qualification Type" );

		}

		otherquals << qualification;

	}
	else {

		otherquals << "None";

	}

	otherquals << '\x0';


	Record *rec = new Record ();
	rec->AddField ( RECORDBANK_NAME, personname );
	rec->AddField ( "IQ", brains + 50 );
	rec->AddField ( "College", collegequals.str () );
	rec->AddField ( "University", universityquals.str () );
	rec->AddField ( "Other", otherquals.str () );

	Computer *ac = game->GetWorld ()->GetComputer ( "International Academic Database" );
	UplinkAssert ( ac );

	ac->recordbank.AddRecordSorted ( rec );

	collegequals.rdbuf()->freeze( 0 );
	universityquals.rdbuf()->freeze( 0 );
	otherquals.rdbuf()->freeze( 0 );
	//delete [] collegequals.str ();
	//delete [] universityquals.str ();
	//delete [] otherquals.str ();

}

void RecordGenerator::GenerateRecord_Financial ( char *personname, int age )
{

	// Find a financial computer - owned by one of the banks of course
	// And create an account for this person

	Computer *comp = WorldGenerator::GetRandomComputer ( COMPUTER_TYPE_PUBLICBANKSERVER );
	UplinkAssert (comp);
	UplinkAssert (comp->GetOBJECTID () == OID_BANKCOMPUTER );
	BankComputer *bank = (BankComputer *) comp;

	// Base level value - used to determine this man's situation
	//

	int financial_situation = (int) NumberGenerator::RandomNormalNumber ( 10, 10 );			// 0 = fucked, 10 = average, 20 = Gates

	int balance = (int) ( (financial_situation - 6) * NumberGenerator::RandomNormalNumber ( 1000, 500 ) );
	int loan = financial_situation < 10 ? (int) ( NumberGenerator::RandomNormalNumber ( 10.0f - financial_situation, 5.0f ) * 1000 ) : 0;
	loan = ( loan / 100 ) * 100;
	if ( loan < 0 )
		loan =  0;

	// Add this account in

	Person *person = game->GetWorld ()->GetPerson ( personname );
	UplinkAssert (person);
	int accno = person->CreateNewAccount ( bank->ip, personname, NameGenerator::GeneratePassword (), balance, loan );
	person->rating.SetCreditRating ( financial_situation );

	char accno_s [16];
	UplinkSnprintf ( accno_s, sizeof ( accno_s ), "%d", accno );

	BankAccount *myaccount = bank->accounts.GetData (accno_s);
	UplinkAssert (myaccount);


	// Generate some records
	// By performing a transfer every few days
	// (This won't do anything if we have passed the game start date)

	if ( myaccount->balance > 0 ) {

		Date transferdate;
		Date nomoredate;
		transferdate.SetDate ( &(game->GetWorld ()->date) );
		nomoredate.SetDate ( GAME_START_DATE );

		while ( transferdate.Before ( &nomoredate ) ) {

			BankComputer *target_bank = (BankComputer *) WorldGenerator::GetRandomComputer ( COMPUTER_TYPE_PUBLICBANKSERVER );
			UplinkAssert ( target_bank );
			BankAccount  *target_acc = target_bank->GetRandomAccount ();

			if ( target_acc ) {

				if ( strcmp ( target_acc->name, game->GetWorld ()->GetPlayer ()->handle ) != 0 ) {		// Prevent transfers to player

					if ( target_acc != myaccount ) {			// Prevent transfers to my own account

						int amount = (int) NumberGenerator::RandomNormalNumber ( myaccount->balance * 0.3f, myaccount->balance * 0.2f );
						amount = int ( amount / 50 ) * 50;

						if ( amount > 0 ) {

							char source_data [128];
							char target_data [128];
							char s_amount [16];
							UplinkSnprintf ( source_data, sizeof ( source_data ), "%s %d", target_bank->ip, target_acc->accountnumber );
							UplinkSnprintf ( target_data, sizeof ( target_data ), "%s %d", bank->ip, myaccount->accountnumber );
							UplinkSnprintf ( s_amount, sizeof ( s_amount ), "%d", amount );

							AccessLog *source_log = new AccessLog ();
							source_log->SetProperties ( &transferdate, bank->ip, person->name, LOG_NOTSUSPICIOUS, LOG_TYPE_TRANSFERTO );
							source_log->SetData1 ( source_data );
							source_log->SetData2 ( s_amount );
							source_log->SetData3 ( person->name );
							myaccount->log.AddLog ( source_log );

							AccessLog *target_log = new AccessLog ();
							target_log->SetProperties ( &transferdate, bank->ip, person->name, LOG_NOTSUSPICIOUS, LOG_TYPE_TRANSFERFROM );
							target_log->SetData1 ( target_data );
							target_log->SetData2 ( s_amount );
							target_log->SetData3 ( person->name );
							target_acc->log.AddLog ( target_log );

						}

					}

				}

			}

			transferdate.AdvanceDay ( NumberGenerator::RandomNumber ( 14 ) );
			transferdate.AdvanceHour ( NumberGenerator::RandomNumber ( 24 ) );
			transferdate.AdvanceMinute ( NumberGenerator::RandomNumber ( 60 ) );

		}

	}

}

Record *RecordGenerator::GetSocialSecurity ( char *personname )
{

	Computer *comp = game->GetWorld ()->GetComputer ( "International Social Security Database" );
	UplinkAssert ( comp );

	char *name = ( strcmp ( personname, "PLAYER" ) == 0 ?
							game->GetWorld ()->GetPlayer ()->handle :
							personname );

	Record *rec = comp->recordbank.GetRecordFromName ( name );

	return rec;

}

Record *RecordGenerator::GetCriminal ( char *personname )
{

	Computer *comp = game->GetWorld ()->GetComputer ( "Global Criminal Database" );
	UplinkAssert ( comp );

	char *name = ( strcmp ( personname, "PLAYER" ) == 0 ?
							game->GetWorld ()->GetPlayer ()->handle :
							personname );

	Record *rec = comp->recordbank.GetRecordFromName ( name );

	return rec;

}

Record *RecordGenerator::GetMedical ( char *personname )
{

	Computer *comp = game->GetWorld ()->GetComputer ( "Central Medical Database" );
	UplinkAssert ( comp );

	char *name = ( strcmp ( personname, "PLAYER" ) == 0 ?
							game->GetWorld ()->GetPlayer ()->handle :
							personname );

	Record *rec = comp->recordbank.GetRecordFromName ( name );

	return rec;

}

Record *RecordGenerator::GetAcademic ( char *personname )
{

	Computer *comp = game->GetWorld ()->GetComputer ( "International Academic Database" );
	UplinkAssert ( comp );

	char *name = ( strcmp ( personname, "PLAYER" ) == 0 ?
							game->GetWorld ()->GetPlayer ()->handle :
							personname );

	Record *rec = comp->recordbank.GetRecordFromName ( name );

	return rec;

}

BankAccount *RecordGenerator::GetFinancial ( char *personname )
{

	Person *person = game->GetWorld ()->GetPerson ( personname );
	UplinkAssert ( person );

	char *fullcurrentaccount = person->accounts.GetData (0);
	UplinkAssert (fullcurrentaccount);

	char ip [SIZE_VLOCATION_IP];
	char accno [16];
	sscanf ( fullcurrentaccount, "%s %s", ip, accno );

	BankAccount *ba = BankAccount::GetAccount ( ip, accno );
	UplinkAssert (ba);

	return ba;

}
