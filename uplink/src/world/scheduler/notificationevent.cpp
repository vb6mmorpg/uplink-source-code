
#include <strstream>

#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"

#include "game/game.h"
#include "game/scriptlibrary.h"
#include "game/data/data.h"

#include "world/world.h"
#include "world/player.h"
#include "world/message.h"
#include "world/company/company.h"
#include "world/company/companyuplink.h"
#include "world/company/mission.h"
#include "world/company/news.h"
#include "world/computer/computer.h"
#include "world/computer/bankaccount.h"
#include "world/scheduler/notificationevent.h"
#include "world/scheduler/attemptmissionevent.h"

#include "world/generator/missiongenerator.h"
#include "world/generator/numbergenerator.h"
#include "world/generator/worldgenerator.h"

#include "mmgr.h"


NotificationEvent::NotificationEvent ()
{

	TYPE = NOTIFICATIONEVENT_TYPE_NONE;

}

NotificationEvent::~NotificationEvent ()
{
}

void NotificationEvent::SetTYPE ( int newTYPE )
{

	TYPE = newTYPE;

}

void NotificationEvent::Run ()
{

	switch ( TYPE ) {

		case NOTIFICATIONEVENT_TYPE_GROWCOMPANIES:				ApplyMonthlyGrowth ();					break;
		case NOTIFICATIONEVENT_TYPE_GENNEWMISSION:				GenerateNewMission ();					break;
		case NOTIFICATIONEVENT_TYPE_CHECKFORSECURITYBREACHES:	CheckForSecurityBreaches ();			break;
		case NOTIFICATIONEVENT_TYPE_CHECKMISSIONDUEDATES:		CheckMissionDueDates ();				break;
		case NOTIFICATIONEVENT_TYPE_CHECKRECENTHACKCOUNT:		CheckRecentHackCount ();				break;
		case NOTIFICATIONEVENT_TYPE_GIVEMISSIONTONPC:			GiveMissionToNPC ();					break;
		case NOTIFICATIONEVENT_TYPE_UPLINKMONTHLYFEE:			PayUplinkMonthlyFee ();					break;
		case NOTIFICATIONEVENT_TYPE_EXPIREOLDSTUFF:				ExpireOldStuff ();						break;
		case NOTIFICATIONEVENT_TYPE_ADDINTERESTONLOANS:			AddInterestOnLoans ();					break;
        case NOTIFICATIONEVENT_TYPE_DEMOGAMEOVER:               DemoGameOver ();                        break;
        case NOTIFICATIONEVENT_TYPE_DEMOGENNEWMISSION:          DemoGenerateNewMission ();              break;
        case NOTIFICATIONEVENT_TYPE_BUYAGENTLIST:               BuyAgentList ();                        break;
        case NOTIFICATIONEVENT_TYPE_AGENTSONLISTDIE:            AgentsOnListDie ();                     break;
        case NOTIFICATIONEVENT_TYPE_WAREZGAMEOVER:              WarezGameOver ();                       break;

		case NOTIFICATIONEVENT_TYPE_NONE:
			UplinkWarning ( "Notification event type not specified" );
			break;

		default:
			UplinkAbortArgs ( "Unrecognised notification type: %d", TYPE );

	}

}

char *NotificationEvent::GetShortString ()
{

	std::ostrstream shortstring;

	switch ( TYPE ) {

		case NOTIFICATIONEVENT_TYPE_GROWCOMPANIES:				shortstring << "Grow companies";				break;
		case NOTIFICATIONEVENT_TYPE_GENNEWMISSION:				shortstring << "Generate new mission";			break;
		case NOTIFICATIONEVENT_TYPE_CHECKFORSECURITYBREACHES:	shortstring << "Check for security breaches";	break;
		case NOTIFICATIONEVENT_TYPE_CHECKMISSIONDUEDATES:		shortstring << "Check mission due dates";		break;
		case NOTIFICATIONEVENT_TYPE_CHECKRECENTHACKCOUNT:		shortstring << "Check recent hack count";		break;
		case NOTIFICATIONEVENT_TYPE_GIVEMISSIONTONPC:			shortstring << "Give mission to NPC";			break;
		case NOTIFICATIONEVENT_TYPE_UPLINKMONTHLYFEE:			shortstring << "Pay Uplink monthly fee";		break;
		case NOTIFICATIONEVENT_TYPE_EXPIREOLDSTUFF:				shortstring << "Expire old stuff";				break;
		case NOTIFICATIONEVENT_TYPE_ADDINTERESTONLOANS:			shortstring << "Add Interest to loans";			break;
        case NOTIFICATIONEVENT_TYPE_DEMOGAMEOVER:               shortstring << "Demo Game Over";                break;
        case NOTIFICATIONEVENT_TYPE_DEMOGENNEWMISSION:          shortstring << "Demo Generate Mission";         break;
        case NOTIFICATIONEVENT_TYPE_BUYAGENTLIST:               shortstring << "Buy Agent List";                break;
        case NOTIFICATIONEVENT_TYPE_AGENTSONLISTDIE:            shortstring << "Agents on list die";            break;
        case NOTIFICATIONEVENT_TYPE_WAREZGAMEOVER:              shortstring << "Warez Game Over";               break;


		case NOTIFICATIONEVENT_TYPE_NONE:
			UplinkWarning ( "Notification event type not specified" );
			shortstring << "Unknown notification";
			break;

		default:
			UplinkAbortArgs ( "Unrecognised notification type: %d", TYPE );

	}

	shortstring << '\x0';
	return shortstring.str ();

}

char *NotificationEvent::GetLongString ()
{

	return GetShortString ();

}

void NotificationEvent::ApplyMonthlyGrowth ()
{

	DArray <Company *> *d_companies = game->GetWorld ()->companies.ConvertToDArray ();

	for ( int i = 0; i < d_companies->Size (); ++i ) {
		if ( d_companies->ValidIndex (i) ) {
			d_companies->GetData (i)->Grow ( 30 );
			d_companies->GetData (i)->VaryGrowth ();
		}
	}

	delete d_companies;

	// Schedule another at the start of the next month

	Date duedate;
	duedate.SetDate ( &rundate );
	duedate.AdvanceMonth ( 1 );
	duedate.SetDate ( duedate.GetSecond (), duedate.GetMinute (), duedate.GetHour (),
					  1, duedate.GetMonth (), duedate.GetYear () );

	NotificationEvent *event = new NotificationEvent ();
	event->SetTYPE ( NOTIFICATIONEVENT_TYPE_GROWCOMPANIES );
	event->SetRunDate ( &duedate );
	game->GetWorld ()->scheduler.ScheduleEvent ( event );

}

void NotificationEvent::GenerateNewMission ()
{

	MissionGenerator::GenerateMission ();

	// Schedule another

	Date duedate;
	duedate.SetDate ( &rundate );
	duedate.AdvanceMinute ( FREQUENCY_GENERATENEWMISSION );

	NotificationEvent *event = new NotificationEvent ();
	event->SetTYPE ( NOTIFICATIONEVENT_TYPE_GENNEWMISSION );
	event->SetRunDate ( &duedate );
	game->GetWorld ()->scheduler.ScheduleEvent ( event );

}

void NotificationEvent::DemoGenerateNewMission ()
{

    game->GetWorld ()->demoplotgenerator.GenerateNewMission ();

	// Schedule another

	Date duedate;
	duedate.SetDate ( &rundate );
	duedate.AdvanceMinute ( FREQUENCY_DEMOGENERATENEWMISSION );

	NotificationEvent *event = new NotificationEvent ();
	event->SetTYPE ( NOTIFICATIONEVENT_TYPE_DEMOGENNEWMISSION );
	event->SetRunDate ( &duedate );
	game->GetWorld ()->scheduler.ScheduleEvent ( event );

}

void NotificationEvent::CheckForSecurityBreaches ()
{

	DArray <Computer *> *d_computers = game->GetWorld ()->computers.ConvertToDArray ();

	for ( int i = 0; i < d_computers->Size (); ++i )
		if ( d_computers->ValidIndex (i) )
			d_computers->GetData (i)->CheckForSecurityBreaches ();

	delete d_computers;


	// Schedule another

	Date duedate;
	duedate.SetDate ( &rundate );
	duedate.AdvanceMinute ( FREQUENCY_CHECKFORSECURITYBREACHES );

	NotificationEvent *event = new NotificationEvent ();
	event->SetTYPE ( NOTIFICATIONEVENT_TYPE_CHECKFORSECURITYBREACHES );
	event->SetRunDate ( &duedate );
	game->GetWorld ()->scheduler.ScheduleEvent ( event );

}

void NotificationEvent::BuyAgentList ()
{

    ScriptLibrary::RunScript ( 71 );

}

void NotificationEvent::AgentsOnListDie ()
{

    ScriptLibrary::RunScript ( 72 );

}

void NotificationEvent::ExpireOldStuff ()
{

	//
	// Expire old access logs
	//


	DArray <Computer *> *d_computers = game->GetWorld ()->computers.ConvertToDArray ();

	for ( int i = 0; i < d_computers->Size (); ++i )
		if ( d_computers->ValidIndex (i) )
			d_computers->GetData (i)->ManageOldLogs ();

	delete d_computers;


	//
	// Expire old news stories
	//

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (cu);

	for ( int in = cu->news.Size () - 1; in >= 1; --in ) {
		News *news = cu->news.GetData (in);
		if ( news ) {

			Date testdate;
			testdate.SetDate ( &news->date );
			testdate.AdvanceMinute ( TIME_TOEXPIRENEWS );

			if ( testdate.Before ( &(game->GetWorld ()->date) ) ) {
				delete cu->news.GetData (in);
				cu->news.RemoveData (in);
			}

		}
	}

	//
	// Expire old missions
	//

	for ( int im = cu->missions.Size () - 1; im >= 1; --im ) {
		Mission *mission = cu->missions.GetData (im);
		if ( mission ) {

			Date testdate;
			testdate.SetDate ( &mission->createdate );
			testdate.AdvanceMinute ( TIME_TOEXPIREMISSIONS );

			if ( testdate.Before ( &(game->GetWorld ()->date) ) ) {
				delete mission;
				cu->missions.RemoveData (im);
			}

		}
	}

	//
	// Schedule another
	//

	Date duedate;
	duedate.SetDate ( &rundate );
	duedate.AdvanceMinute ( FREQUENCY_EXPIREOLDSTUFF );

	NotificationEvent *event = new NotificationEvent ();
	event->SetTYPE ( NOTIFICATIONEVENT_TYPE_EXPIREOLDSTUFF );
	event->SetRunDate ( &duedate );
	game->GetWorld ()->scheduler.ScheduleEvent ( event );

}

void NotificationEvent::AddInterestOnLoans ()
{

	//
	// This event only does interest on the players loans
	//

	for ( int i = 0; i < game->GetWorld ()->GetPlayer ()->accounts.Size (); ++i ) {

		char *fullacc = game->GetWorld ()->GetPlayer ()->accounts.GetData (i);
		UplinkAssert (fullacc);

		char ip [SIZE_VLOCATION_IP];
		char accno [16];
		sscanf ( fullacc, "%s %s", ip, accno );

		BankAccount *account = BankAccount::GetAccount ( ip, accno );
        if (account) {

		    if ( account->loan > 0 ) {

			    int amountpaid = 0;

			    if ( account->loan <= SMALLLOAN_MAX )
				    amountpaid = (int) ( ( account->loan * SMALLLOAN_INTEREST ) / 12 );

			    else if ( account->loan <= MEDIUMLOAN_MAX )
				    amountpaid += (int) ( ( account->loan * MEDIUMLOAN_INTEREST ) / 12 );

			    else if ( account->loan <= LARGELOAN_MAX )
				    amountpaid += (int) ( ( account->loan * LARGELOAN_INTEREST ) / 12 );

			    else
				    amountpaid += (int) ( ( account->loan * MAXLOAN_INTEREST ) / 12 );

			    account->loan += amountpaid;


			    VLocation *vl = game->GetWorld ()->GetVLocation ( ip );
			    UplinkAssert (vl);
			    Computer *comp = vl->GetComputer ();
			    UplinkAssert (comp);

			    std::ostrstream body;
			    body << "You have been charged interest on the following account:\n"
				     << "IP : " << ip << "\n"
				     << "BANK : " << comp->name << "\n"
				     << "ACCNO : " << account->accountnumber << "\n\n"
				     << "You have been charged " << amountpaid << "c extra on your loan."
				     << "Your new loan is " << account->loan << "c."
				     << '\x0';

			    Message *msg = new Message ();
			    msg->SetFrom ( comp->name );
			    msg->SetTo ( "PLAYER" );
			    msg->SetSubject ( "Interest charged" );
			    msg->SetBody ( body.str () );
			    msg->Send ();

				body.rdbuf()->freeze( 0 );
			    //delete [] body.str ();

		    }

        }

	}

	//
	// Schedule another
	//

	Date duedate;
	duedate.SetDate ( &rundate );
	duedate.AdvanceMinute ( FREQUENCY_ADDINTERESTONLOANS );

	NotificationEvent *event = new NotificationEvent ();
	event->SetTYPE ( NOTIFICATIONEVENT_TYPE_ADDINTERESTONLOANS );
	event->SetRunDate ( &duedate );
	game->GetWorld ()->scheduler.ScheduleEvent ( event );

}

void NotificationEvent::CheckMissionDueDates ()
{

	game->GetWorld ()->GetPlayer ()->CheckMissionDueDates ();

	// Schedule another

	Date duedate;
	duedate.SetDate ( &rundate );
	duedate.AdvanceMinute ( FREQUENCY_CHECKMISSIONDUEDATES );

	NotificationEvent *event = new NotificationEvent ();
	event->SetTYPE ( NOTIFICATIONEVENT_TYPE_CHECKMISSIONDUEDATES );
	event->SetRunDate ( &duedate );
	game->GetWorld ()->scheduler.ScheduleEvent ( event );

}

void NotificationEvent::CheckRecentHackCount ()
{

	DArray <Computer *> *d_computers = game->GetWorld ()->computers.ConvertToDArray ();

	for ( int i = 0; i < d_computers->Size (); ++i )
		if ( d_computers->ValidIndex (i) )
			d_computers->GetData (i)->UpdateRecentHacks ();

	delete d_computers;

	// Schedule another

	NotificationEvent *ne = new NotificationEvent ();
	ne->SetTYPE ( NOTIFICATIONEVENT_TYPE_CHECKRECENTHACKCOUNT );

	Date duedate;
	duedate.SetDate ( &rundate );

	// ***** There must be exaclty 4 of these in a month ******

	if ( duedate.GetDay () < 22 )
		duedate.AdvanceDay ( 7 );
	else {
		duedate.AdvanceMonth ( 1 );
		duedate.SetDate ( 1, 1, 1, 1, duedate.GetMonth (), duedate.GetYear () );
	}

	ne->SetRunDate ( &duedate );

	game->GetWorld ()->scheduler.ScheduleEvent ( ne );

}

void NotificationEvent::GiveMissionToNPC ()
{

	//
	// Choose a random Uplink Agent (not the player)
	//

	Agent *agent = WorldGenerator::GetRandomAgent ();
	UplinkAssert (agent);


	//
	// Build a list of candidate missions
	// Sorted into difficulty order
	// Keep an eye out for "priority" missions
	//

	CompanyUplink *uplink = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (uplink);
	LList <Mission *> *fullist = &(uplink->missions);
	LList <Mission *> missions;
	LList <int>		  missions_index;											// Indexes of each mission

	Mission *prioritymission = NULL;
	int prioritymissionindex = -1;

	for ( int i = 0; i < fullist->Size (); ++i ) {

		Mission *mission = fullist->GetData (i);
		UplinkAssert (mission);

		if ( agent->rating.uplinkrating >= mission->minuplinkrating && mission->TYPE != MISSION_SPECIAL &&
			 !( mission->TYPE == MISSION_TRACEUSER && strcmp ( mission->completionA, agent->name ) == 0 ) ) {

			if ( mission->npcpriority ) {
				// This is a priority mission - accept it!
				prioritymission = mission;
				prioritymissionindex = i;
				break;
			}

			Date testdate;
			testdate.SetDate ( &(mission->createdate) );
			testdate.AdvanceDay ( 7 );

			if ( testdate.Before ( &(game->GetWorld ()->date) ) ) {

				// This is not a priority mission, so let it hang around for a week
				// before giving it to NPC agents

				bool inserted = false;

				for ( int j = 0; j < missions.Size (); ++j ) {
					if ( mission->difficulty < missions.GetData (j)->difficulty ) {
						missions.PutDataAtIndex ( mission, j );
						missions_index.PutDataAtIndex ( i, j );
						inserted = true;
						break;
					}
				}

				if ( !inserted ) {
					missions.PutDataAtEnd ( mission );
					missions_index.PutDataAtEnd ( i );
				}

			}

		}

	}

	//
	// If we have found a priority mission, accept it immediately
	// Otherwise, choose a mission from the available list
	//

	if ( prioritymission ) {

		agent->GiveMission ( prioritymission );
		fullist->RemoveData ( prioritymissionindex );

		// Schedule an event to make the agent attempt the mission
		// in a few hours

		Date duedate;
		duedate.SetDate ( &rundate );
		duedate.AdvanceHour ( NumberGenerator::RandomNumber ( 2 ) + 1 );

		AttemptMissionEvent *ami = new AttemptMissionEvent ();
		ami->SetAgentName ( agent->name );
		ami->SetRunDate ( &duedate );
		game->GetWorld ()->scheduler.ScheduleEvent ( ami );

	}
	else {

		//
		// Choose a mission based on a Normal  distribution curve, peeking at 1 or 2 difficulty
		// levels higher than your uplink rating.
		//

		int nummissions = missions.Size ();

		if ( nummissions > 0 ) {

			int missionindex = (int) NumberGenerator::RandomNormalNumber ( (float) ( nummissions / 4), (float) ( nummissions / 2 ) );
			if ( missionindex < 0 ) missionindex = 0;

			Mission *targetmission = missions.GetData (missionindex);
			UplinkAssert (targetmission);

			//
			// Give this mission to the agent
			// And remove it from the list of available missions
			//

			agent->GiveMission ( targetmission );
			fullist->RemoveData ( missions_index.GetData (missionindex) );

			// Schedule an event to make the agent attempt the mission
			// in a few hours

			Date duedate;
			duedate.SetDate ( &rundate );
			duedate.AdvanceHour ( NumberGenerator::RandomNumber ( 4 ) + 1 );

			AttemptMissionEvent *ami = new AttemptMissionEvent ();
			ami->SetAgentName ( agent->name );
			ami->SetRunDate ( &duedate );
			game->GetWorld ()->scheduler.ScheduleEvent ( ami );

		}

	}

	//
	// Schedule this GiveMissionToNPC event again in a few hours
	//

	Date duedate;
	duedate.SetDate ( &rundate );
	duedate.AdvanceMinute ( FREQUENCY_GIVEMISSIONTONPC );

	NotificationEvent *ne = new NotificationEvent ();
	ne->SetRunDate ( &duedate );
	ne->SetTYPE ( NOTIFICATIONEVENT_TYPE_GIVEMISSIONTONPC );
	game->GetWorld ()->scheduler.ScheduleEvent ( ne );

}

void NotificationEvent::PayUplinkMonthlyFee ()
{

	//
	// Deduct the fee and send a message to
	// everyone with an Uplink Rating
	//

	DArray <Person *> *people = (DArray <Person *> *) game->GetWorld ()->people.ConvertToDArray ();

	for ( int i = 0; i < people->Size (); ++i ) {
		if ( people->ValidIndex (i) ) {

			Person *person = people->GetData (i);
			UplinkAssert (person);

			if ( person->rating.uplinkrating > 0 ) {

				person->ChangeBalance ( COST_UPLINK_PERMONTH * -1, "Uplink Corporation : Monthly fee" );

				std::ostrstream body;
				body << "Uplink Corporation has deducted the standard monthly "
						"membership fee of " << COST_UPLINK_PERMONTH << " credits from "
						"you current account.\n\n"
						"We are confident you will agree that this represents "
						"incredible value for money and wish you another successful "
						"and prosperous month with our organisation.\n"
						"Thank you." << '\x0';

				Message *m = new Message ();
				m->SetTo ( person->name );
				m->SetFrom ( "Uplink Corporation" );
				m->SetSubject ( "Uplink monthly fee deducted" );
				m->SetBody ( body.str () );
				m->Send ();

				body.rdbuf()->freeze( 0 );
				//delete [] body.str ();


			}

		}
	}

    delete people;

	//
	// Schedule another in a month
	//

	Date duedate;
	duedate.SetDate ( &rundate );
	duedate.AdvanceMonth ( 1 );

	NotificationEvent *ne = new NotificationEvent ();
	ne->SetRunDate ( &duedate );
	ne->SetTYPE ( NOTIFICATIONEVENT_TYPE_UPLINKMONTHLYFEE );
	game->GetWorld ()->scheduler.ScheduleEvent ( ne );

}

void NotificationEvent::DemoGameOver ()
{

    // This is a demo game
    // and the game has come to an end

    game->DemoGameOver ();

}

void NotificationEvent::WarezGameOver ()
{

    // This is a warez copy
    game->WarezGameOver ();

}

void NotificationEvent::ScheduleStartingEvents ()
{

	NotificationEvent *monthlygrowth = new NotificationEvent ();
	monthlygrowth->SetTYPE ( NOTIFICATIONEVENT_TYPE_GROWCOMPANIES );
	monthlygrowth->SetRunDate ( &(game->GetWorld ()->date) );
	game->GetWorld ()->scheduler.ScheduleEvent ( monthlygrowth );

	NotificationEvent *gennewmission = new NotificationEvent ();
	gennewmission->SetTYPE ( NOTIFICATIONEVENT_TYPE_GENNEWMISSION );
	gennewmission->SetRunDate ( &(game->GetWorld ()->date) );
	game->GetWorld ()->scheduler.ScheduleEvent ( gennewmission );

	NotificationEvent *checksec = new NotificationEvent ();
	checksec->SetTYPE ( NOTIFICATIONEVENT_TYPE_CHECKFORSECURITYBREACHES );
	checksec->SetRunDate ( &(game->GetWorld ()->date) );
	game->GetWorld ()->scheduler.ScheduleEvent ( checksec );

	NotificationEvent *duedates = new NotificationEvent ();
	duedates->SetTYPE ( NOTIFICATIONEVENT_TYPE_CHECKMISSIONDUEDATES );
	duedates->SetRunDate ( &(game->GetWorld ()->date) );
	game->GetWorld ()->scheduler.ScheduleEvent ( duedates );

	NotificationEvent *hackcount = new NotificationEvent ();
	hackcount->SetTYPE ( NOTIFICATIONEVENT_TYPE_CHECKRECENTHACKCOUNT );
	Date duedate;
	duedate.SetDate ( &(game->GetWorld ()->date) );
	duedate.AdvanceMonth ( 1 );
	duedate.SetDate ( 1, 1, 1, 1, duedate.GetMonth (), duedate.GetYear () );
	hackcount->SetRunDate ( &duedate );
	game->GetWorld ()->scheduler.ScheduleEvent ( hackcount );

	NotificationEvent *givemission = new NotificationEvent ();
	givemission->SetTYPE ( NOTIFICATIONEVENT_TYPE_GIVEMISSIONTONPC );
	givemission->SetRunDate ( &(game->GetWorld ()->date) );
	game->GetWorld ()->scheduler.ScheduleEvent ( givemission );

	NotificationEvent *payfee = new NotificationEvent ();
	payfee->SetTYPE ( NOTIFICATIONEVENT_TYPE_UPLINKMONTHLYFEE );
	duedate.SetDate ( &(game->GetWorld ()->date) );
	duedate.AdvanceMonth ( 1 );
	duedate.SetDate ( 1, 1, 1, 1, duedate.GetMonth (), duedate.GetYear () );
	payfee->SetRunDate ( &duedate );
	game->GetWorld ()->scheduler.ScheduleEvent ( payfee );

	NotificationEvent *expirelogs = new NotificationEvent ();
	expirelogs->SetTYPE ( NOTIFICATIONEVENT_TYPE_EXPIREOLDSTUFF );
	expirelogs->SetRunDate ( &(game->GetWorld ()->date) );
	game->GetWorld ()->scheduler.ScheduleEvent ( expirelogs );

	NotificationEvent *interestonloans = new NotificationEvent ();
	interestonloans->SetTYPE ( NOTIFICATIONEVENT_TYPE_ADDINTERESTONLOANS );
	interestonloans->SetRunDate ( &(game->GetWorld ()->date) );
	game->GetWorld ()->scheduler.ScheduleEvent ( interestonloans );

#ifdef DEMOGAME

	NotificationEvent *demogennewmission = new NotificationEvent ();
	demogennewmission->SetTYPE ( NOTIFICATIONEVENT_TYPE_DEMOGENNEWMISSION );
	demogennewmission->SetRunDate ( &(game->GetWorld ()->date) );
	game->GetWorld ()->scheduler.ScheduleEvent ( demogennewmission );

#endif

}

bool NotificationEvent::Load  ( FILE *file )
{

	LoadID ( file );

	if ( !UplinkEvent::Load ( file ) ) return false;

	if ( !FileReadData ( &TYPE, sizeof(TYPE), 1, file ) ) return false;

	LoadID_END ( file );

	return true;

}

void NotificationEvent::Save  ( FILE *file )
{

	SaveID ( file );

	UplinkEvent::Save ( file );

	fwrite ( &TYPE, sizeof(TYPE), 1, file );

	SaveID_END ( file );

}

void NotificationEvent::Print ()
{

	printf ( "NotificationEvent : TYPE = %d\n", TYPE );
	UplinkEvent::Print ();

}

char *NotificationEvent::GetID ()
{

	return "EVT_NOT";

}

int   NotificationEvent::GetOBJECTID ()
{

	return OID_NOTIFICATIONEVENT;

}
