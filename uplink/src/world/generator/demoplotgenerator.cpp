
#ifdef WIN32
#include <windows.h>
#endif

#include <strstream>

#include "gucci.h"


#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"

#include "game/game.h"
#include "game/data/data.h"


#include "world/world.h"
#include "world/player.h"
#include "world/message.h"
#include "world/company/company.h"
#include "world/company/companyuplink.h"
#include "world/company/mission.h"
#include "world/company/news.h"
#include "world/computer/computer.h"

#include "world/generator/demoplotgenerator.h"
#include "world/generator/worldgenerator.h"
#include "world/generator/missiongenerator.h"
#include "world/generator/numbergenerator.h"
#include "world/generator/namegenerator.h"

#include "world/scheduler/eventscheduler.h"
#include "world/scheduler/notificationevent.h"
#include "world/scheduler/runplotsceneevent.h"

#include "mmgr.h"


DemoPlotGenerator::DemoPlotGenerator ()
{

    scene = 0;

}

DemoPlotGenerator::~DemoPlotGenerator ()
{
}

void DemoPlotGenerator::Initialise ()
{

}

void DemoPlotGenerator::MoveNewMissionToRandomLocation ( Mission *mission )
{

    CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
    UplinkAssert (cu);

    // Remove the mission from the list first

    for ( int i = 0; i < cu->missions.Size (); ++i ) {
        if ( cu->missions.GetData (i) == mission ) {

            cu->missions.RemoveData (i);
            break;

        }
    }

    // Re-insert at a random spot

    int position = 3 + NumberGenerator::RandomNumber (9);
    Mission *prevmission = cu->missions.GetData (position-1);
    UplinkAssert (prevmission);
    mission->SetCreateDate ( &(prevmission->createdate) );
    cu->missions.PutDataAtIndex ( mission, position );

}

bool DemoPlotGenerator::NumAvailableMissions ( int missiontype )
{

    CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
    UplinkAssert (cu);

    int found = 0;

    // "available" now means "in the first 10" as well
    int maxmission = cu->missions.Size () < 10 ? cu->missions.Size () : 10;

    for ( int i = 0; i < maxmission; ++i ) {

        Mission *m = cu->missions.GetData (i);
        UplinkAssert (m);

        if ( m->TYPE == missiontype &&
            m->minuplinkrating <= game->GetWorld ()->GetPlayer ()->rating.uplinkrating ) {

            found++;

        }

    }

    return found;

}

Mission *DemoPlotGenerator::Generate_FileCopyMission ()
{

	Company *employer1 = NULL;
	Computer *target1 = NULL;
    Mission *mission = NULL;

    while ( !mission ) {

	    while ( !employer1 || !target1 ||
		    strcmp ( employer1->name, target1->companyname ) == 0 ) {

		    employer1 = WorldGenerator::GetRandomCompany ();
		    UplinkAssert (employer1);
		    target1 = WorldGenerator::GetRandomLowSecurityComputer ( COMPUTER_TYPE_INTERNALSERVICESMACHINE );
		    UplinkAssert (target1);

	    }

	    mission = MissionGenerator::Generate_StealSingleFile ( employer1, target1 );

    }

    mission->SetMinRating ( 2 );
    return mission;

}

Mission *DemoPlotGenerator::Generate_FileDeleteMission ()
{

	Company *employer2 = NULL;
	Computer *target2 = NULL;
    Mission *mission = NULL;

    while ( !mission ) {

	    while ( !employer2 || !target2 ||
		    strcmp ( employer2->name, target2->companyname ) == 0 ) {

		    employer2 = WorldGenerator::GetRandomCompany ();
		    UplinkAssert (employer2);
		    target2 = WorldGenerator::GetRandomLowSecurityComputer ( COMPUTER_TYPE_INTERNALSERVICESMACHINE );
		    UplinkAssert (target2);

	    }

	    mission = MissionGenerator::Generate_DestroySingleFile ( employer2, target2 );

    }

    mission->SetMinRating ( 2 );
    return mission;

}

Mission *DemoPlotGenerator::Generate_ChangeAcademicMission ()
{

    Mission *mission = NULL;

    while ( !mission ) {

        Company *employer = WorldGenerator::GetRandomCompany ();
	    UplinkAssert (employer);
        mission = MissionGenerator::Generate_ChangeData_AcademicRecord ( employer );

    }

    mission->SetMinRating ( 3 );
    return mission;

}

Mission *DemoPlotGenerator::Generate_ChangeSocialRecordARC ()
{

	Computer *target = game->GetWorld ()->GetComputer ( "International Social Security Database" );
	UplinkAssert (target);

    //Company *employer = game->GetWorld ()->GetCompany ( "ARC" );
    //UplinkAssert (employer);

	//
	// Set up the basic variables of the mission
	//

    int difficulty      = 10;
	int payment			= (int) NumberGenerator::RandomNormalNumber ( (float) ( difficulty * PAYMENT_MISSION_CHANGEDATA_SOCIAL ),
	                                                                  (float) ( difficulty * PAYMENT_MISSION_CHANGEDATA_SOCIAL
	                                                                                       * PAYMENT_MISSION_VARIANCE ) );

	int minrating		= 3;
	int acceptrating	= 4;

	payment = int ( payment / 100 ) * 100;				// Rounds payment to the nearest 100

	char personname [SIZE_PERSON_NAME];					// Person to send completion email to
	UplinkStrncpy ( personname, "internal@ARC.net", sizeof ( personname ) );

	char description [SIZE_MISSION_DESCRIPTION];
	std::ostrstream details;
	std::ostrstream fulldetails;

    UplinkStrncpy ( description, "Modify a protected Social Security document", sizeof ( description ) );

	details << "Payment for this job is " << payment << " credits.\n"
			<< "This job has been assigned an Uplink difficulty of " << difficulty << ".\n"
			<< "An UplinkRating of " << Rating::GetUplinkRatingString ( acceptrating ) << " or above will be sufficient for automatic acceptance.\n\n"
			<< '\x0';

    fulldetails << "Thank you for replying Agent.\n"
                << "We have some work for you which you can get started on immediately.\n"
	            << "Your skills with the Academic Database will come in useful here.\n\n"
                << "Break into the International Social Security Database:\n"
				<< "TARGET COMPUTER DATA :\n"
				<< "   LOCATION: " << target->name << "\n"
				<< "   IP      : " << target->ip << "\n"
				<< "\n\n";

	char completionA [SIZE_VLOCATION_IP];			// IP
	char completionB [SIZE_PERSON_NAME];			// Target person
	char completionC [64];							// Field to be changed
	char completionD [64];							// Word that must appear in the field
	char completionE [64];							// Word that must appear in the field

	// Find someone who isn't dead

	Person *person = NULL;

    while ( !person ) {

	    Record *record = target->recordbank.GetRandomRecord ( "Personal Status ! Deceased" );
        if ( record ) {

	        char *personname = record->GetField ( RECORDBANK_NAME );
	        UplinkAssert (personname);

            if ( strcmp ( personname, game->GetWorld ()->GetPlayer ()->handle ) != 0 ) {

		        person = game->GetWorld ()->GetPerson ( personname );
		        UplinkAssert (person);
		        break;

            }
        }

	}

    int newnumber = 10000000 + NumberGenerator::RandomNumber ( 99999999 );

	fulldetails << "Below you will find the personal details of our target. We have selected this man "
                   "from a large candidate list due to his suitability to our aims. We wish to make use of his "
                   "identity for one of our own undercover agents, and in order for this to go ahead we first need "
                   "to make some changes to his Social Security Record.\n\n"
                   "You can help us by making the necessary changes.  All we need you to do is to change his "
                   "Social Security Number to "
                << newnumber
                << ".  We will take care of the rest.\n";

	UplinkStrncpy ( completionC, "Social Security", sizeof ( completionC ) );
	UplinkSnprintf ( completionD, sizeof ( completionD ), "%d", newnumber );
	UplinkSnprintf ( completionE, sizeof ( completionE ), "%d", newnumber );

	UplinkStrncpy ( completionA, target->ip, sizeof ( completionA ) );
	UplinkStrncpy ( completionB, person->name, sizeof ( completionB ) );

    UplinkStrncpy ( arcmissiontarget, person->name, sizeof ( arcmissiontarget ) );

	fulldetails	<< "\nTARGET INDIVIDUAL :\n"
				<< "   NAME    : " << person->name << "\n"
				<< "\n\n"
				<< "Once you have finished, be sure to let us know as soon as possible.\n"
                << "Payment for this job is " << payment << " credits.\n"
                << "Send a notice of completion to\n"
				<< personname << "\n"
				<< "\n"
				<< "END"
				<< '\x0';

	//
	// Generate the mission
	//

	Mission *mission = new Mission ();
	mission->SetTYPE		 ( MISSION_CHANGEDATA );
	mission->SetCompletion   ( completionA, completionB, completionC, completionD, completionE );
	//mission->SetEmployer     ( employer->name );
	mission->SetEmployer     ( "ARC" );
	mission->SetContact      ( personname );
	mission->SetPayment      ( payment, (int)(payment * 1.1) );
	mission->SetDifficulty   ( difficulty );
	mission->SetMinRating    ( minrating );
	mission->SetAcceptRating ( acceptrating );
	mission->SetDescription  ( description );
	mission->SetDetails		 ( details.str () );
	mission->SetFullDetails  ( fulldetails.str () );
	mission->GiveLink ( target->ip );

	details.rdbuf()->freeze( 0 );
	fulldetails.rdbuf()->freeze( 0 );
	//delete [] details.str ();
	//delete [] fulldetails.str ();

    return mission;

}

void DemoPlotGenerator::PlayerRatingChange ()
{

    GenerateNewMission ( true );

/*
    //
    // MAX RATING REACHED
    //

    Player *player = game->GetWorld ()->GetPlayer ();
    UplinkAssert (player);

    if ( player->rating.uplinkscore >= UPLINKRATING [DEMO_MAXUPLINKRATING].score ) {

		Message *m = new Message ();
		m->SetTo ( "PLAYER" );
		m->SetFrom ( "Uplink" );
		m->SetSubject ( "Maximum uplink rating reached" );
		m->SetBody ( "Congratulations agent - you have reached the maximum possible Uplink Rating "
					 "in the demo version of Uplink.  In order to achieve higher ratings and participate "
					 "in later missions, you'll need to register your copy of Uplink.\n\n"
					 "Go to www.introversion.co.uk for details on how to do this.\n\n"
					 "Regards\n"
					 "The Uplink Team" );
		m->Send ();

    }
*/

}

void DemoPlotGenerator::PlayerCompletedMission ( Mission *mission )
{

    UplinkAssert (mission);

    if ( mission->TYPE == MISSION_CHANGEDATA &&
         strcmp ( mission->completionA, IP_ACADEMICDATABASE ) == 0 &&
         scene == 0 ) {

        //
        // Just finished the academic database hack
        // Send a "we're impressed" msg
        // to the player from ARC
        //

        RunPlotSceneEvent *rpse = new RunPlotSceneEvent ();
        rpse->SetActAndScene ( -1, 1 );
        Date rundate;
        rundate.SetDate ( &game->GetWorld ()->date );
        rundate.AdvanceMinute ( 1 );
        rpse->SetRunDate ( &rundate );
        game->GetWorld ()->scheduler.ScheduleEvent ( rpse );

    }
    else if ( mission->TYPE == MISSION_CHANGEDATA &&
              strcmp ( mission->completionA, IP_SOCIALSECURITYDATABASE ) == 0 &&
              strcmp ( mission->contact, "internal@ARC.net" ) == 0 ) {

        //
        // Just finished the special social security mission from ARC
        // Send congrats
        // Kill of the target of that mission

        RunScene ( 3 );

    }

}

void DemoPlotGenerator::GenerateNewMission ( bool randomdate )
{

    int playerrating = game->GetWorld ()->GetPlayer ()->rating.uplinkrating;

    if ( playerrating == 2 ) {

        // Ensure he always has a steal file mission

        int numm1 = NumAvailableMissions ( MISSION_STEALFILE );
        if ( numm1 == 0 ) {
            Mission *m = Generate_FileCopyMission ();
            if ( randomdate ) MoveNewMissionToRandomLocation ( m );
        }

        // Ensure he always has a destroy file mission

        int numm2 = NumAvailableMissions ( MISSION_DESTROYFILE );
        if ( numm2 == 0 ) {
            Mission *m = Generate_FileDeleteMission ();
            if ( randomdate ) MoveNewMissionToRandomLocation ( m );
        }

    }
    else if ( playerrating == 3 ) {

        // Ensure he always has an academic hack

        int numm1 = NumAvailableMissions ( MISSION_CHANGEDATA );
        if ( numm1 == 0 ) {
            Mission *m = Generate_ChangeAcademicMission ();
            if ( randomdate ) MoveNewMissionToRandomLocation ( m );
        }

    }

}

void DemoPlotGenerator::RunScene ( int newscene )
{

    scene = newscene;

    if ( scene == 1 ) {

        // ARC offer a special mission to the Player:
        // Jig this guys social-security record
        //

        Message *m = new Message ();
        m->SetFrom ( "internal@ARC.net" );
        m->SetTo ( "PLAYER" );
        m->SetSubject ( "Offer from Andromeda Research Corporation" );

        std::ostrstream body;
        body <<      "Agent "
             <<      game->GetWorld ()->GetPlayer ()->handle << ",\n"
             <<      "We have been monitoring your progress as a new Uplink Agent, "
                     "and we've been extremely impressed with the rate at which you are "
                     "advancing through the ranks.\n\n"
                     "We have some work which needs completing, and your recent experience "
                     "with the International Academic Database makes you particularly suited to the job.\n"
                     "The work that we are doing is of a similar nature.\n\n"
                     "You will find working for Andromeda Research Corporation is much more profitable "
                     "than working for Uplink Corporation. In any case, we need an answer soon.\n"
                     "If you are interested, simply reply to this email within the next 24 hours.\n\n"
                     "[END]"
             <<      '\x0';

        m->SetBody ( body.str () );
        m->Send ();

		body.rdbuf()->freeze( 0 );
        //delete [] body.str ();

    }
    else if ( scene == 2 ) {

        // He accepts the offer
        // Send him the mission

        Mission *mission = Generate_ChangeSocialRecordARC ();
        UplinkAssert (mission);
        game->GetWorld ()->GetPlayer ()->GiveMission (mission);

    }
    else if ( scene == 3 ) {

        // Player completed the ARC mission

        Message *m = new Message ();
        m->SetFrom ( "internal@ARC.net" );
        m->SetTo ( "PLAYER" );
        m->SetSubject ( "Message from ARC" );
        m->SetBody ( "Congratulations on your recent success with the Social Security Database. "
                     "We knew you would be successfull - we would not have selected you otherwise.\n\n"
                     "If you check your bank accounts, you will see we have deposited the amount promised.\n\n"
                     "We hope you will be willing to work for us in the future. We will contact you soon "
                     "with more work, as it becomes available.\n\n"
                     "Andromeda Research Corporation" );
        m->Send ();

        // The target of that mission dies in one minute

        RunPlotSceneEvent *rpse = new RunPlotSceneEvent ();
        rpse->SetActAndScene ( -1, 4 );
        Date rundate;
        rundate.SetDate ( &game->GetWorld ()->date );
        rundate.AdvanceMinute ( 1 );
        rpse->SetRunDate ( &rundate );
        game->GetWorld ()->scheduler.ScheduleEvent ( rpse );

    }
    else if ( scene == 4 ) {

        // The target of the ARC mission dies
        // News story

        std::ostrstream newsdetails;
        char *agenthandle = NameGenerator::GenerateAgentAlias ();
        UplinkAssert (agenthandle);

        newsdetails << "An Uplink Agent going by the handle of " << agenthandle <<
                       " has been found dead in his house after an apparent suicide.\n"
			           "Preliminary forensic analysis suggests he overdosed on sleeping pills.\n\n"
			           "The agent's real name is " << arcmissiontarget << " and he had been working for the controversial "
                       "Andromeda Research Corporation, otherwise known as ARC.  ARC have been in the news in previous months, "
                       "responding to allegations that they have been developing some kind of weapon using the expertise of new and naive Uplink Agents.\n"
                       "In particular, they have been accused of targeting users of the Demo Uplink Client, "
                       "who often have little understanding or knowledge of the companies involved.\n\n"
			           "ARC released a statement earlier today, stating 'He was the best programmer we had ever seen and "
			           "was a lead player in the project.  For something like this happen is a tradgedy of the greatest order.  "
			           "Our thoughts go out to this mans family and friends.'\n\n"
                       "Due to the seriousness of these claims, Uplink Corporation has asked all agents not to accept work from ARC while this issue is resolved."
                    << '\x0';

	    News *news = new News ();
	    news->SetHeadline ( "Uplink agent dies in mysterious circumstances" );
	    news->SetDetails ( newsdetails.str () );

	    CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	    UplinkAssert (cu);
	    cu->CreateNews ( news );

		newsdetails.rdbuf()->freeze( 0 );
        //delete [] newsdetails.str ();

        // Warning mail to all

        Message *m = new Message ();
        m->SetFrom ( "Uplink Corporation" );
        m->SetTo ( "PLAYER" );
        m->SetSubject ( "Warning to all Agents" );
        m->SetBody ( "[Sent to ALL-AGENTS]\n\n"
                     "We have recently uncovered some alarming evidence regarding the "
                     "Andromeda Research Corporation, otherwise known as ARC.  Several of our agents "
                     "have made accusations and we are currently investigating.\n\n"
                     "In the mean time, we wish to request that Uplink Agents refuse any work offered to them "
                     "by ARC until this matter is resolved.\n\n"
                     "More details can be found on our News Server, on the Uplink Internal Services System.\n\n"
                     "END" );
        m->Send ();

        scene = 5;

    }
    else {

        // No more plot for the demo

    }

}

bool DemoPlotGenerator::PlayerContactsARC ( Message *msg )
{

    UplinkAssert (msg);

    if ( scene == 1 ) {

        // This is a response to the offer from ARC
        RunScene ( 2 );
        return true;

    }

    return false;

}

bool DemoPlotGenerator::Load  ( FILE *file )
{

    LoadID ( file );

    if ( !FileReadData ( &scene, sizeof(scene), 1, file ) ) return false;

    LoadID_END ( file );

	return true;

}

void DemoPlotGenerator::Save  ( FILE *file )
{

    SaveID ( file );

    fwrite ( &scene, sizeof(scene), 1, file );

    SaveID_END ( file );

}

void DemoPlotGenerator::Print ()
{

    printf ( "Demo Plot Generator\n" );
    printf ( "Scene %d\n", scene );

}

void DemoPlotGenerator::Update ()
{

    if ( game->GetWorld ()->GetPlayer ()->connection.TraceInProgress () &&
        game->GetWorld ()->GetPlayer ()->rating.uplinkscore >= UPLINKRATING [DEMO_MAXUPLINKRATING].score ) {

        // The player is performing a hack
        // but has reached the maximum rating.
        // Schedule a DemoGameOver event

        Date rundate;
        rundate.SetDate ( &game->GetWorld ()->date );
        rundate.AdvanceMinute ( TIME_TODEMOGAMEOVER );

        NotificationEvent *ne = new NotificationEvent ();
        ne->SetTYPE ( NOTIFICATIONEVENT_TYPE_DEMOGAMEOVER );
        ne->SetRunDate ( &rundate );

        game->GetWorld ()->scheduler.ScheduleEvent ( ne );

    }

}

char *DemoPlotGenerator::GetID ()
{

    return "DEMOPGEN";

}

