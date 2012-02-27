
#include <strstream>

#include "app/app.h"
#include "app/globals.h"

#include "game/game.h"
#include "game/data/data.h"

#include "options/options.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"

#include "world/world.h"
#include "world/message.h"
#include "world/person.h"
#include "world/player.h"
#include "world/computer/computer.h"
#include "world/computer/bankcomputer.h"
#include "world/company/mission.h"
#include "world/company/companyuplink.h"

#include "world/generator/missiongenerator.h"
#include "world/generator/consequencegenerator.h"
#include "world/generator/newsgenerator.h"
#include "world/generator/recordgenerator.h"
#include "world/generator/numbergenerator.h"

#include "world/scheduler/eventscheduler.h"
#include "world/scheduler/arrestevent.h"
#include "world/scheduler/shotbyfedsevent.h"
#include "world/scheduler/seizegatewayevent.h"

#include "mmgr.h"


void ConsequenceGenerator::Initialise ()
{
}

void ConsequenceGenerator::CaughtHacking ( Person *person, Computer *comp )
{

	UplinkAssert (person);
	UplinkAssert (comp);


	//
	// Get the computer to "remember" that is was hacked
	//

	comp->AddToRecentHacks ( 1 );

	//
	// Lookup the company owner
	//

	Company *company = game->GetWorld ()->GetCompany ( comp->companyname );
	UplinkAssert ( company );

	int severity = company->size / 10 + 1;

	/*
    //
    // SPECIAL CASE
    // If this is the player, and he has the VIRTUANET gateway (total anonymity)
    //

    if ( strcmp ( person->name, "PLAYER" ) == 0 &&
         game->GetWorld ()->GetPlayer ()->gateway.type == 3 ) {

		// Disconnect user if he is still connected here

		if ( person->GetConnection ()->GetTarget () &&
			 strcmp ( person->GetConnection ()->GetTarget (), comp->ip ) == 0 ) {

			person->GetConnection ()->Disconnect ();
			person->GetConnection ()->Reset ();
			game->GetInterface ()->GetRemoteInterface ()->RunNewLocation ();
			game->GetInterface ()->GetRemoteInterface ()->RunScreen ( 2);

        }

        // Don't do anything else
        return;

    }
	*/

	//
	// Add this to their criminal record
	// And change their ratings
	//

	if ( comp->traceaction > 3 ) {

		// ie not "NONE", "DISCONNECT" or "WARNINGMAIL"

		Record *rec = RecordGenerator::GetCriminal ( person->name );

		if ( rec ) {
			char *existing = rec->GetField ( "Convictions" );
			//UplinkAssert (existing);

			std::ostrstream newconvictions;

			if ( existing && strstr ( existing, "None" ) == NULL )
				newconvictions << existing << "\n";

			newconvictions << "Unauthorised systems access\n"
						   << '\x0';

			rec->ChangeField ( "Convictions", newconvictions.str () );

			newconvictions.rdbuf()->freeze( 0 );
			//delete [] newconvictions.str ();
		}

		//
		// Decrease ratings
		//

		person->rating.ChangeUplinkScore ( severity * -1 );
		person->rating.ChangeNeuromancerScore ( severity * -1 );

	}


	//
	// comp->traceaction is a bitfield - so act on each possibility
	//

	if ( comp->traceaction & COMPUTER_TRACEACTION_DISCONNECT ) {

		// Disconnect user if he is still connected here

		if ( person->GetConnection ()->GetTarget () &&
			 strcmp ( person->GetConnection ()->GetTarget (), comp->ip ) == 0 ) {

			person->GetConnection ()->Disconnect ();
			person->GetConnection ()->Reset ();

			if ( strcmp ( person->name, "PLAYER" ) == 0 ) {

				game->GetInterface ()->GetRemoteInterface ()->RunNewLocation ();
				game->GetInterface ()->GetRemoteInterface ()->RunScreen ( 2);

			}

		}

	}

	if ( comp->traceaction & COMPUTER_TRACEACTION_WARNINGMAIL ) {

		// Send warning mail

		Message *msg = new Message ();
		msg->SetTo ( person->name );
		msg->SetFrom ( comp->companyname );
		msg->SetSubject ( "You were caught hacking our computer systems" );
		msg->SetBody ( "Our system administrator has informed us that you have been using "
					   "one of our accounts to gain unauthorised access to our computer systems.\n"
					   "We have disconnected your computer from our system and have changed "
					   "the access codes to this account.  Do not attempt to gain access to "
					   "our systems again.\n"
					   "We will be notifying your employer and the police of this action.  "
					   "This was a warning - in future, more severe action will be taken." );
		msg->Send ();

       // Reset defenses

        comp->ChangeSecurityCodes ();

	}

	if ( comp->traceaction & COMPUTER_TRACEACTION_FINE ) {

		// Pay a fine

		int finesize = severity * 1000;

		// No point in fining anyone other than the player
		// As no one would ever notice

		if ( strcmp ( person->name, "PLAYER" ) == 0 ) {

			// Generate a "pay fine" mission

			char reason [128 + SIZE_COMPUTER_NAME];
			UplinkSnprintf ( reason, sizeof ( reason ), "Our system security agents have caught you making unauthorised access "
														  "to our computer system %s.", comp->name );

			// You have a few days to complete it

			Date *date = new Date ();
			date->SetDate ( &(game->GetWorld ()->date) );
			date->AdvanceMinute ( TIME_TOPAYLEGALFINE );
			date->SetDate ( 0, 0, 0, date->GetDay (), date->GetMonth (), date->GetYear () );			// Round to midnight

			Mission *mission = MissionGenerator::Generate_PayFine ( person, company, finesize, date, reason );
			game->GetWorld ()->GetPlayer ()->GiveMission ( mission );

			delete date;

		}


       // Reset defenses

        comp->ChangeSecurityCodes ();

	}

	if ( comp->traceaction & COMPUTER_TRACEACTION_LEGAL ) {

		// Legal action begins - the person is arrested
		// (In a few hours)
		// If its an agent, seize his gateway

		Date duedate;
		duedate.SetDate ( &(game->GetWorld ()->date) );
		duedate.AdvanceMinute ( TIME_LEGALACTION );

		Date warningdate;
		warningdate.SetDate ( &duedate );
		warningdate.AdvanceMinute ( TIME_LEGALACTION_WARNING * -1 );


		if ( strcmp ( person->name, "PLAYER" ) != 0 ) {

			std::ostrstream reason;
			reason << "hacking into the "
				   << comp->name << ".\n"
				   << '\x0';

			ArrestEvent *event = new ArrestEvent ();
			event->SetName ( person->name );
			event->SetReason ( reason.str () );
			event->SetIP ( comp->ip );
			event->SetRunDate ( &duedate );

			game->GetWorld ()->scheduler.ScheduleWarning ( event, &warningdate );
			game->GetWorld ()->scheduler.ScheduleEvent ( event );

			reason.rdbuf()->freeze( 0 );
			//delete [] reason.str ();

		}
		else {

			// This is the player - seize his gateway

			std::ostrstream reason;
			reason << "Hacking into the "
				   << comp->name << ".\n"
				   << '\x0';

			SeizeGatewayEvent *event = new SeizeGatewayEvent ();
			event->SetName ( person->name );
			event->SetGatewayID ( game->GetWorld ()->GetPlayer ()->gateway.id );
			event->SetReason ( reason.str () );
			event->SetRunDate ( &duedate );

			game->GetWorld ()->scheduler.ScheduleWarning ( event, &warningdate );
			game->GetWorld ()->scheduler.ScheduleEvent ( event );

			reason.rdbuf()->freeze( 0 );
			//delete [] reason.str ();

		}

	}

	if ( comp->traceaction & COMPUTER_TRACEACTION_TACTICAL ) {

		// Tactical action begins - the person is shot
		// (In a few minutes)
		// If its an agent, seize his gateway

		Date duedate;
		duedate.SetDate ( &(game->GetWorld ()->date) );
		duedate.AdvanceMinute ( TIME_TACTICALACTION );

		Date warningdate;
		warningdate.SetDate ( &duedate );
		warningdate.AdvanceMinute ( TIME_TACTICALACTION_WARNING * -1 );

		if ( strcmp ( person->name, "PLAYER" ) != 0 ) {

			std::ostrstream reason;
			reason << "hacking into the "
				   << comp->name << ".\n"
				   << '\x0';

			ShotByFedsEvent *event = new ShotByFedsEvent ();
			event->SetName ( person->name );
			event->SetReason ( reason.str () );
			event->SetRunDate ( &duedate );

			game->GetWorld ()->scheduler.ScheduleWarning ( event, &warningdate );
			game->GetWorld ()->scheduler.ScheduleEvent ( event );

			reason.rdbuf()->freeze( 0 );
			//delete [] reason.str ();

		}
		else {

			// This is the player - seize his gateway

			std::ostrstream reason;
			reason << "Hacking into the "
				   << comp->name << ".\n"
				   << '\x0';

			SeizeGatewayEvent *event = new SeizeGatewayEvent ();
			event->SetName ( person->name );
			event->SetGatewayID ( game->GetWorld ()->GetPlayer ()->gateway.id );
			event->SetReason ( reason.str () );
			event->SetRunDate ( &duedate );

			game->GetWorld ()->scheduler.ScheduleWarning ( event, &warningdate );
			game->GetWorld ()->scheduler.ScheduleEvent ( event );

			reason.rdbuf()->freeze( 0 );
			//delete [] reason.str ();

		}

	}

}

void ConsequenceGenerator::ShotByFeds ( Person *person, char *reason )
{

	UplinkAssert (person);

	/*
		Do something here - news stories etc

		*/

}

void ConsequenceGenerator::Arrested ( Person *person, Computer *comp, char *reason )
{

	UplinkAssert (person);

	/*
		For future versions : generate news stories,
		vendetta's etc from this

		*/

	NewsGenerator::Arrested ( person, comp, reason );

}

void ConsequenceGenerator::DidntPayFine ( Person *person, Mission *fine )
{

	UplinkAssert (person);
	UplinkAssert (fine);

	//
	// Schedule an ARREST event for this person
	// Or a SeizeGateway event if its the player

	Date duedate;
	duedate.SetDate ( &(game->GetWorld ()->date) );
	duedate.AdvanceMinute ( TIME_LEGALACTION );

	Date warningdate;
	warningdate.SetDate ( &duedate );
	warningdate.AdvanceMinute ( TIME_LEGALACTION_WARNING * -1 );

	int finesize;
	sscanf ( fine->completionC, "%d", &finesize );

	if ( strcmp ( person->name, "PLAYER" ) != 0 ) {

		int jailtime = finesize / 1000;

		std::ostrstream reason;
		reason << "For refusing to pay a " << finesize << "c fine.\n"
			   << "Send to jail for " << jailtime << " years.\n"
			   << '\x0';

		ArrestEvent *event = new ArrestEvent ();
		event->SetName ( person->name );
		event->SetReason ( reason.str () );
		event->SetRunDate ( &duedate );

		game->GetWorld ()->scheduler.ScheduleWarning ( event, &warningdate );
		game->GetWorld ()->scheduler.ScheduleEvent ( event );

		reason.rdbuf()->freeze( 0 );
		//delete [] reason.str ();

	}
	else {

		// This is the player - seize his gateway

		std::ostrstream reason;
		reason << "For refusing to pay a " << finesize << "c fine.\n"
			   << '\x0';

		SeizeGatewayEvent *event = new SeizeGatewayEvent ();
		event->SetName ( person->name );
		event->SetGatewayID ( game->GetWorld ()->GetPlayer ()->gateway.id );
		event->SetReason ( reason.str () );
		event->SetRunDate ( &duedate );

		game->GetWorld ()->scheduler.ScheduleWarning ( event, &warningdate );
		game->GetWorld ()->scheduler.ScheduleEvent ( event );

		reason.rdbuf()->freeze( 0 );
		//delete [] reason.str ();

	}

}

void ConsequenceGenerator::SeizeGateway  ( Person *person, char *reason )
{

	UplinkAssert (person);

	/*

		Future versions : This person is now disavowed.  They may come back
		as a vengeful new agent.

		Re-incarnation!

		*/

}


void ConsequenceGenerator::ComputerHacked ( Computer *comp, AccessLog *al )
{

	UplinkAssert (comp);
	UplinkAssert (al);

	comp->AddToRecentHacks ( 1 );

	NewsGenerator::ComputerHacked ( comp, al );

	if ( comp->traceaction >= COMPUTER_TRACEACTION_LEGAL ) {

		Person *hacker = game->GetWorld ()->GetPerson ( al->fromname );
		UplinkAssert (hacker);

		if ( hacker == game->GetWorld ()->GetPlayer () ||
			NumberGenerator::RandomNumber (100) < PROB_GENERATETRACEHACKERMISSION ) {

			// If this hasn't already been noticed, generate a new trace hacker mission
			// No point in generating another tracehacker mission if that person and machine
			// are already under investigation

			CompanyUplink *uplink = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
			UplinkAssert (uplink);

			Date testdate;
			testdate.SetDate ( &(game->GetWorld ()->date) );
			testdate.AdvanceHour ( -12 );

			bool alreadynoticed = false;

			for ( int i = 0; i < uplink->missions.Size (); ++i ) {

				Mission *m = uplink->GetMission ( i );
				UplinkAssert (m);

				if ( m->TYPE == MISSION_TRACEUSER &&
					 strcmp ( m->completionA, hacker->name ) == 0 &&
					 strcmp ( m->links.GetData (0), comp->ip ) == 0 ) {

					alreadynoticed = true;
					break;

				}

			}

			if ( !alreadynoticed ) MissionGenerator::Generate_TraceHacker ( comp, hacker );

		}

	}

}

void ConsequenceGenerator::MissionCompleted ( Mission *mission, Person *person )
{

	UplinkAssert ( mission );
	UplinkAssert ( person );

	switch ( mission->TYPE ) {

		case MISSION_STEALFILE:				MissionCompleted_StealFile		( mission, person );		break;
		case MISSION_DESTROYFILE:			MissionCompleted_DestroyFile	( mission, person );		break;
		case MISSION_CHANGEACCOUNT:			MissionCompleted_ChangeAccount	( mission, person );		break;
		case MISSION_TRACEUSER:				MissionCompleted_TraceUser		( mission, person );		break;

		case MISSION_NONE:
		default:																						break;


	}

}

void ConsequenceGenerator::MissionCompleted_StealFile	( Mission *mission, Person *person )
{

	UplinkAssert ( mission );
	UplinkAssert ( person );

	char *ip = mission->completionA;
	char *filename = mission->completionB;
	char *filetype = mission->completionE;

    int numfiles;
    int totalfilesize;
    sscanf ( mission->completionD, "%d %d", &numfiles, &totalfilesize );

	if ( strcmp (filename, "ALL") == 0 ) {

		VLocation *vl = game->GetWorld ()->GetVLocation ( ip );
		UplinkAssert (vl);
		Computer *comp = vl->GetComputer ();
		UplinkAssert (comp);

		NewsGenerator::AllFilesStolen ( comp, filetype, totalfilesize );

		if ( person == game->GetWorld ()->GetPlayer () ||
			 NumberGenerator::RandomNumber (100) < PROB_GENERATETRACEHACKERMISSION ) {

			MissionGenerator::Generate_TraceHacker ( mission, person );

		}

	}

}

void ConsequenceGenerator::MissionCompleted_DestroyFile ( Mission *mission, Person *person )
{

	UplinkAssert (mission);
	UplinkAssert (person);

	char *filename = mission->completionB;

	if ( strcmp (filename, "ALL") == 0 ) {

		if ( person == game->GetWorld ()->GetPlayer () ||
			 NumberGenerator::RandomNumber (100) < PROB_GENERATETRACEHACKERMISSION ) {

			MissionGenerator::Generate_TraceHacker ( mission, person );

		}

	}

}

void ConsequenceGenerator::MissionCompleted_ChangeAccount ( Mission *mission, Person *person )
{

	UplinkAssert (mission);
	UplinkAssert (person);

	if ( person == game->GetWorld ()->GetPlayer () ||
		 NumberGenerator::RandomNumber (100) < PROB_GENERATETRACEHACKERMISSION ) {

		// Extract values from the mission

		char source_ip [SIZE_VLOCATION_IP];
		char target_ip [SIZE_VLOCATION_IP];
		char source_accs [16];
		char target_accs [16];
		int amount;

		UplinkAssert (mission);

		sscanf ( mission->completionA, "%s %s", source_ip, source_accs );
		sscanf ( mission->completionB, "%s %s", target_ip, target_accs );
		sscanf ( mission->completionC, "%d", &amount );

		// Get the source and target computers

		VLocation *vls = game->GetWorld ()->GetVLocation ( source_ip );
		UplinkAssert (vls);
		BankComputer *source = (BankComputer *) vls->GetComputer ();
		UplinkAssert (source);

		VLocation *vlt = game->GetWorld ()->GetVLocation ( target_ip );
		UplinkAssert (vlt);
		BankComputer *target = (BankComputer *) vlt->GetComputer ();
		UplinkAssert (target);

		// Get the source and target bank accounts

		BankAccount *source_acc = BankAccount::GetAccount ( source_ip, source_accs );
		BankAccount *target_acc = BankAccount::GetAccount ( target_ip, target_accs );
		UplinkAssert (source_acc);
		UplinkAssert (target_acc);

		// Get the employer

		Company *employer = game->GetWorld ()->GetCompany ( source->companyname );
		UplinkAssert (employer);

		MissionGenerator::Generate_TraceUser_MoneyTransfer ( employer, source, target, source_acc, target_acc, amount, person );

	}

}

void ConsequenceGenerator::MissionCompleted_TraceUser ( Mission *mission, Person *person )
{

	UplinkAssert ( mission );
	UplinkAssert ( person );

	//
	// Look up the computer that was hacked
	//

	char *hackedip = mission->links.GetData (0);
	VLocation *vl = game->GetWorld ()->GetVLocation ( hackedip );
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);

	//
	// Look up the guilty person
	//

	char *personname = mission->completionA;
	Person *guiltyperson = game->GetWorld ()->GetPerson ( personname );
	UplinkAssert ( guiltyperson );

	//
	// This person is effectivly caught
	//

	if ( guiltyperson->GetStatus () == PERSON_STATUS_NONE ) CaughtHacking ( guiltyperson, comp );

}

