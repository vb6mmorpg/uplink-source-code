
#include <strstream>

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"

#include "game/game.h"
#include "game/data/data.h"

#include "world/world.h"
#include "world/agent.h"
#include "world/message.h"
#include "world/player.h"
#include "world/company/company.h"
#include "world/company/companyuplink.h"
#include "world/company/news.h"
#include "world/company/mission.h"

#include "world/computer/computer.h"
#include "world/computer/bankcomputer.h"

#include "world/computer/computerscreen/genericscreen.h"
#include "world/computer/computerscreen/passwordscreen.h"
#include "world/computer/computerscreen/menuscreen.h"
#include "world/computer/computerscreen/dialogscreen.h"
#include "world/computer/computerscreen/messagescreen.h"
#include "world/computer/computerscreen/highsecurityscreen.h"
#include "world/computer/computerscreen/cypherscreen.h"

#include "world/generator/worldgenerator.h"
#include "world/generator/plotgenerator.h"
#include "world/generator/numbergenerator.h"
#include "world/generator/missiongenerator.h"
#include "world/scheduler/runplotsceneevent.h"
#include "world/scheduler/notificationevent.h"
#include "world/generator/langenerator.h"
#include "world/generator/namegenerator.h"

#include "interface/interface.h"
#include "interface/taskmanager/taskmanager.h"
#include "interface/remoteinterface/remoteinterface.h"

#include "mmgr.h"


char *MISSION_TITLE [] = {
                                "Backfire",
                                "Tracer",
                                "TakeMeToYourLeader",
                                "ARCInfiltration",
                                "CounterAttack",
                                "MaidenFlight",
                                "Darwin",
                                "SaveItForTheJury",
                                "ShinyHammer",
                                "GrandTour",
                                "Mole????",
                                "Wargames????", "12", "13", "14", "15"
                            };


char *MISSION_DESCRIPTION [] = {
                                    "Counter attack!",
								    "Perform a covert installation of a Tracer",
								    "Help bring the CEO of a major company into custody",
								    "Take advantage of an amazing opportunity",
								    "Prevent the outbreak of Revelation",
								    "Test run Revelation Version 1.0",
                                    "Steal vital Research into Digitial Life Forms",
								    "Frame the chief tech of Arunmor Corporation",
								    "Destroy all research completed by Arunmor",
								    "Release the Revelation Virus into the wild",
                                    "Help us track down some people we wish to speak with",
                                    "wargames", "12", "13", "14", "15"
							    };


PlotGenerator::PlotGenerator ()
{

	act = 0;
	scene = 0;

	UplinkStrncpy ( act1scene3agent, " ", sizeof ( act1scene3agent ) );
	UplinkStrncpy ( act1scene4agent, " ", sizeof ( act1scene4agent ) );
	UplinkStrncpy ( act2scene1agent, " ", sizeof ( act2scene1agent ) );

    playervisitsplotsites = false;
    playercancelsmail = false;
    playerloyalty = 0;

    completed_tracer = false;
    completed_darwin = false;
    completed_saveitforthejury = false;
    completed_shinyhammer = false;
    completed_arcinfiltration = false;
    completed_takemetoyourleader = false;

	specialmissionscompleted = 0;

    UplinkStrncpy ( saveitforthejury_guytobeframed, "None", sizeof ( saveitforthejury_guytobeframed ) );
    UplinkStrncpy ( saveitforthejury_targetbankip, "None", sizeof ( saveitforthejury_targetbankip ) );
    UplinkStrncpy ( tracer_lastknownip, "None ", sizeof ( tracer_lastknownip ));

	numuses_revelation = 0;
    revelation_releaseuncontrolled = false;
    revelation_releasefailed = false;
    revelation_arcbusted = false;

	version_revelation = 0.0;
	version_faith = 0.0;

}

PlotGenerator::~PlotGenerator ()
{

}

void PlotGenerator::Initialise ()
{

	Initialise_ARC ();
    Initialise_Andromeda ();
    Initialise_ARUNMOR ();
    Initialise_DARWIN ();

	Run_Act1Scene1 ();

/*
    specialmissionscompleted |= (1 << 0);
    specialmissionscompleted |= (1 << 1);
    specialmissionscompleted |= (1 << 2);
    specialmissionscompleted |= (1 << 3);
    specialmissionscompleted |= (1 << 4);

    specialmissionscompleted |= (1 << 5);
    specialmissionscompleted |= (1 << 6);
    specialmissionscompleted |= (1 << 7);
    specialmissionscompleted |= (1 << 8);
    specialmissionscompleted |= (1 << 9);

    specialmissionscompleted |= (1 << 10);
*/
}

void PlotGenerator::Initialise_ARC ()
{

	char *companyname = "ARC";

	Company *arc = new Company ();
	arc->SetName ( companyname );
	arc->SetSize ( 50 );
	arc->SetTYPE ( COMPANYTYPE_COMMERCIAL );
	arc->SetGrowth ( 10 );
	arc->SetAlignment ( -100 );
	game->GetWorld ()->CreateCompany ( arc );

	Computer *pas = WorldGenerator::GeneratePublicAccessServer		( companyname );
	Computer *ism = WorldGenerator::GenerateInternalServicesMachine ( companyname );
	Computer *cm  = WorldGenerator::GenerateCentralMainframe		( companyname );
    Computer *lan = WorldGenerator::GenerateLAN                     ( companyname );

    HighSecurityScreen *hs = (HighSecurityScreen *) cm->GetComputerScreen (0);
    hs->AddSystem ( "Elliptic-Curve Encryption Cypher", 7 );

	CypherScreen *cs = new CypherScreen ();
	cs->SetMainTitle ( "ARC" );
	cs->SetSubTitle ( "Enter elliptic-curve encryption cypher" );
	cs->SetDifficulty ( HACKDIFFICULTY_ARCCENTRALMAINFRAME );
	cs->SetNextPage ( 0 );
	cm->AddComputerScreen ( cs, 7 );

    Person *ceo = WorldGenerator::GetRandomPerson ();
    UplinkAssert (ceo);
    ceo->SetIsTargetable ( false );
    arc->SetBoss ( ceo->name );

    Person *admin = WorldGenerator::GetRandomPerson ();
    UplinkAssert (admin);
    admin->SetIsTargetable ( false );
    arc->SetAdmin ( admin->name );


	pas->SetIsTargetable ( false );
	ism->SetIsTargetable ( false );
	 cm->SetIsTargetable ( false );
    lan->SetIsTargetable ( false );

	for ( int i = 0; i < 8; ++i ) {
		Computer *comp = WorldGenerator::GenerateComputer ( companyname );
		comp->SetIsTargetable ( false );
	}

	// Generate contact addresses for this company

	char personname [SIZE_PERSON_NAME];
	UplinkSnprintf ( personname, sizeof ( personname ), "internal@%s.net", companyname );
	Person *pinternal = game->GetWorld ()->CreatePerson ( personname, pas->ip );
	pinternal->SetIsTargetable ( false );

	// Set up our share prices for the start

	arc->Grow ( 0 );

}

void PlotGenerator::Initialise_Andromeda ()
{

    // Add a single computer system
    // Owned by ARC


    VLocation *vl = WorldGenerator::GenerateLocation ();

	Computer *comp = new Computer ();
    comp->SetName ( NameGenerator::GenerateAccessTerminalName("Andromeda") );
	comp->SetCompanyName ( "ARC" );
	comp->SetTraceSpeed ( 20 );
	comp->SetTraceAction ( COMPUTER_TRACEACTION_DISCONNECT );
	comp->SetIsTargetable ( false );
	comp->SetIP ( vl->ip );

	game->GetWorld ()->CreateComputer ( comp );


    MessageScreen *ms = new MessageScreen ();
    ms->SetMainTitle ( "Andromeda" );
    ms->SetSubTitle ( "" );
    ms->SetTextMessage ( "\n\n\n\n\n                            THE TIME IS NEAR" );
    ms->SetButtonMessage ( "." );
    ms->SetNextPage ( 1 );
    comp->AddComputerScreen ( ms, 0 );

    MenuScreen *ms1 = new MenuScreen ();
    ms1->SetMainTitle ( "Andromeda" );
    ms1->SetSubTitle ( "The time is near" );
    ms1->AddOption ( "Our beliefs", "Our beliefs", 3 );
    ms1->AddOption ( "Our people", "Our people", 2 );
    ms1->AddOption ( "Our support", "Our support", 4 );
    comp->AddComputerScreen ( ms1, 1 );

    MessageScreen *ms2 = new MessageScreen ();
    ms2->SetMainTitle ( "Andromeda" );
    ms2->SetSubTitle ( "Our people" );
    ms2->SetTextMessage ( "Removed for security purposes\n"
                          "effective from 12-02-2010" );
    ms2->SetButtonMessage ( "." );
    ms2->SetNextPage ( 1 );
    comp->AddComputerScreen ( ms2, 2 );

    MessageScreen *ms3 = new MessageScreen ();
    ms3->SetMainTitle ( "Andromeda" );
    ms3->SetSubTitle ( "Our Beliefs" );
    ms3->SetTextMessage ( "THE ANDROMEDA FOUNDATION IS BUILT ON ITS CORE FIVE BELIEFS\n\n"
                          "1. The Internet, once the saviour of free speech and anonymity, has been completely "
                          "taken over by the evils of capitalism.\n\n"
                          "2. The common man has been reduced to a series of numbers and his life has been made an "
                          "item of public record.\n\n"
                          "3. The Government is using the Internet to catalogue and control the population.\n\n"
                          "4. The destruction of the Anonymity of the Internet has removed with it the last remnants "
                          "of Human creativity.\n\n"
                          "5. Fundamentally, the wide-spread usage of the Internet has led to a new class divide - "
                          "Those that know, and those that do not." );
    ms3->SetButtonMessage ( "." );
    ms3->SetNextPage ( 1 );
    comp->AddComputerScreen ( ms3, 3 );

    MessageScreen *ms4 = new MessageScreen ();
    ms4->SetMainTitle ( "Andromeda" );
    ms4->SetSubTitle ( "Our Support" );
    ms4->SetTextMessage ( "It is heartening to know that we are not alone in our beliefs.\n\n"
                          "The Andromeda group has the support of a number of high profile "
                          "people and organisations.\n\n"
                          "This puts us in a situation where we can make a real difference.\n\n"
                          "The time is near." );
    ms4->SetButtonMessage ( "." );
    ms4->SetNextPage ( 1 );
    comp->AddComputerScreen ( ms4, 4 );


}

void PlotGenerator::Initialise_ARUNMOR ()
{

	char *companyname = "Arunmor";

	Company *company = new Company ();
	company->SetName ( companyname );
	company->SetBoss ( "Unlisted" );
	company->SetSize ( 50 );
	company->SetTYPE ( COMPANYTYPE_COMMERCIAL );
	company->SetGrowth ( 10 );
	company->SetAlignment ( 0 );
	game->GetWorld ()->CreateCompany ( company );

	Computer *pas = WorldGenerator::GeneratePublicAccessServer		( companyname );
	Computer *ism = WorldGenerator::GenerateInternalServicesMachine ( companyname );
	Computer *cm  = WorldGenerator::GenerateCentralMainframe		( companyname );
    Computer *lan = WorldGenerator::GenerateLAN                     ( companyname );


    HighSecurityScreen *hs = (HighSecurityScreen *) cm->GetComputerScreen (0);
    hs->AddSystem ( "Elliptic-Curve Encryption Cypher", 7 );

	CypherScreen *cs = new CypherScreen ();
	cs->SetMainTitle ( "Arunmor" );
	cs->SetSubTitle ( "Enter elliptic-curve encryption cypher" );
	cs->SetDifficulty ( HACKDIFFICULTY_ARUNMORCENTRALMAINFRAME );
	cs->SetNextPage ( 0 );
	cm->AddComputerScreen ( cs, 7 );

    Person *ceo = WorldGenerator::GetRandomPerson ();
    UplinkAssert (ceo);
    ceo->SetIsTargetable ( false );
    company->SetBoss ( ceo->name );

    Person *admin = WorldGenerator::GetRandomPerson ();
    UplinkAssert (admin);
    admin->SetIsTargetable ( false );
    company->SetAdmin ( admin->name );


    pas->SetIsTargetable ( false );
	ism->SetIsTargetable ( false );
	 cm->SetIsTargetable ( false );
    lan->SetIsTargetable ( false );

	for ( int i = 0; i < 8; ++i ) {
		Computer *comp = WorldGenerator::GenerateComputer ( companyname );
		comp->SetIsTargetable ( false );
	}

	// Generate contact addresses for this company

	char personname [SIZE_PERSON_NAME];
	UplinkSnprintf ( personname, sizeof ( personname ), "internal@%s.net", companyname );
	Person *pinternal = game->GetWorld ()->CreatePerson ( personname, pas->ip );
	pinternal->SetIsTargetable ( false );

	// Set up our share prices for the start

	company->Grow ( 0 );

}

void PlotGenerator::Initialise_DARWIN ()
{

    char *companyname = "Darwin Research Associates";

    Company *company = new Company ();
    company->SetName ( companyname );
    company->SetSize ( 20 );
    company->SetTYPE ( COMPANYTYPE_ACADEMIC );
    company->SetGrowth ( 10 );
    company->SetAlignment ( 0 );
    game->GetWorld ()->CreateCompany ( company );


    Computer *pas = WorldGenerator::GeneratePublicAccessServer		( companyname );
	Computer *ism = WorldGenerator::GenerateInternalServicesMachine ( companyname );
	Computer *cm  = WorldGenerator::GenerateCentralMainframe		( companyname );
    Computer *lan = WorldGenerator::GenerateLAN                     ( companyname );

	pas->SetIsTargetable ( false );
	ism->SetIsTargetable ( false );
	 cm->SetIsTargetable ( false );
    lan->SetIsTargetable ( false );

	for ( int i = 0; i < 3; ++i ) {
		Computer *comp = WorldGenerator::GenerateComputer ( companyname );
		comp->SetIsTargetable ( false );
	}


    Person *ceo = WorldGenerator::GetRandomPerson ();
    Person *admin = WorldGenerator::GetRandomPerson ();
    UplinkAssert (ceo);
    UplinkAssert (admin);
	company->SetBoss ( ceo->name );
	company->SetAdmin ( admin->name );

 	// Generate contact addresses for this company

	char personname [SIZE_PERSON_NAME];
	UplinkSnprintf ( personname, sizeof ( personname ), "internal@%s.net", companyname );
	Person *pinternal = game->GetWorld ()->CreatePerson ( personname, pas->ip );
	pinternal->SetIsTargetable ( false );

    company->Grow ( 0 );

}

void PlotGenerator::Run_Scene ( int newact, int newscene )
{

	// Yeah I know.
	// Bite me.

	act = newact;
	scene = newscene;

	switch ( act ) {

		case 1:

			if		( scene == 1 )				Run_Act1Scene1 ();
			else if ( scene == 2 )				Run_Act1Scene2 ();
			else if ( scene == 3 )				Run_Act1Scene3 ();
			else if ( scene == 4 )				Run_Act1Scene4 ();
			else if ( scene == 5 )				Run_Act1Scene5 ();
			else if ( scene == 7 )				Run_Act1Scene7 ();
			else if ( scene == 8 )				Run_Act1Scene8 ();

			else printf ( "Invalid Scene number: Act1 Scene%d\n", newscene );

			break;


#ifndef DEMOGAME

		case 2:

			if		( scene == 1 )				Run_Act2Scene1 ();
			else if ( scene == 2 )				Run_Act2Scene2 ();
			else if ( scene == 3 )				Run_Act2Scene3 ();

			else printf ( "Invalid Scene number: Act2 Scene%d\n", newscene );
			break;

		case 3:

            if      ( scene == 1 )              Run_Act3Scene1 ();
			else if ( scene == 2 )				Run_Act3Scene2 ();
			else if ( scene == 3 )				Run_Act3Scene3 ();
            else if ( scene == 4 )              Run_Act3Scene4 ();

			else printf ( "Invalid Scene number: Act3 Scene%d\n", newscene );
			break;

        case 4:

            if      ( scene == 1 )              Run_Act4Scene1 ();
            else if ( scene == 2 )              Run_Act4Scene2 ();
            else if ( scene == 3 )              Run_Act4Scene3 ();
            else if ( scene == 4 )              Run_Act4Scene4 ();
            else if ( scene == 5 )              Run_Act4Scene5 ();
            else if ( scene == 6 )              Run_Act4Scene6 ();
            else if ( scene == 7 )              Run_Act4Scene7 ();
            else if ( scene == 8 )              Run_Act4Scene8 ();
            else if ( scene == 9 )              Run_Act4Scene9 ();
            else if ( scene == 10 )             Run_Act4Scene10 ();
            else if ( scene == 11 )             Run_Act4Scene11 ();
            else if ( scene == 12 )             Run_Act4Scene12 ();
            else if ( scene == 13 )             Run_Act4Scene13 ();
            else if ( scene == 14 )             Run_Act4Scene14 ();
            else if ( scene == 15 )             Run_Act4Scene15 ();

            else printf ( "Invalid scene number: Act4 scene%d\n", newscene );
            break;

        case 5:

            if      ( scene == 1 )              Run_Act5Scene1 ();
            else if ( scene == 2 )              Run_Act5Scene2 ();
            else if ( scene == 3 )              Run_Act5Scene3 ();
            else if ( scene == 4 )              Run_Act5Scene4 ();
            else if ( scene == 5 )              Run_Act5Scene5 ();
            else if ( scene == 6 )              Run_Act5Scene6 ();
            else if ( scene == 7 )              Run_Act5Scene7 ();

			else printf ( "Invalid Scene number: Act5 Scene%d\n", newscene );
			break;

        case 6:

            if      ( scene == 1 )              Run_Act6Scene1 ();
            else if ( scene == 2 )              Run_Act6Scene2 ();
            else if ( scene == 3 )              Run_Act6Scene3 ();

            else printf ( "Invalid Scene number: Act6 Scene%d\n", newscene );
			break;

#endif

		default:
				printf ( "Invalid Act number: Act%d Scene%d\n", newact, newscene );

	}

}

void PlotGenerator::PlayerCancelsMail ()
{

	playercancelsmail = true;

}

void PlotGenerator::PlayerCompletesSpecialMission ( int missionID )
{

	// Pay him / change ratings
	// Remove the mission from his list

	const char *title = SpecialMissionDescription (missionID);

	for ( int i = 0; i < game->GetWorld ()->GetPlayer ()->missions.Size (); ++i ) {

		Mission *mis = game->GetWorld ()->GetPlayer ()->missions.GetData (i);
		UplinkAssert (mis);

		if ( strcmp ( mis->description, title ) == 0 ) {


			game->GetWorld ()->GetPlayer ()->ChangeBalance ( mis->payment, "Anonymous donation" );
			game->GetWorld ()->GetPlayer ()->rating.ChangeUplinkScore ( mis->difficulty );
			game->GetWorld ()->GetPlayer ()->missions.RemoveData (i);

			specialmissionscompleted |= (1 << missionID);

            if ( strcmp ( mis->contact, "internal@ARC.net" ) == 0 )
       			game->GetWorld ()->GetPlayer ()->rating.ChangeNeuromancerScore ( mis->difficulty * -1 );

            else
                game->GetWorld ()->GetPlayer ()->rating.ChangeNeuromancerScore ( mis->difficulty * 1 );

            delete mis;
            break;

		}

	}

}

void PlotGenerator::PlayerFailsSpecialMission ( int missionID )
{

    // Remove the mission from his list

	const char *title = SpecialMissionDescription (missionID);

	for ( int i = 0; i < game->GetWorld ()->GetPlayer ()->missions.Size (); ++i ) {

		Mission *mis = game->GetWorld ()->GetPlayer ()->missions.GetData (i);
		UplinkAssert (mis);

		if ( strcmp ( mis->description, title ) == 0 ) {

			game->GetWorld ()->GetPlayer ()->missions.RemoveData (i);
            delete mis;
            break;

		}

	}

}

bool PlotGenerator::RemoveSpecialMission ( int missionID )
{

	const char *title = SpecialMissionDescription (missionID);

    CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
    UplinkAssert ( cu );

    for ( int i = 0; i < cu->missions.Size (); ++i ) {

        Mission *m = cu->missions.GetData (i);
        UplinkAssert (m);

        if ( strcmp ( m->description, title ) == 0 ) {

            cu->missions.RemoveData ( i );
            delete m;
            return true;

        }

    }

    return false;

}

bool PlotGenerator::PlayerContactsARC ( Message *msg )
{

#ifndef DEMOGAME

	if ( act == 2 && scene == 1 ) {

		// The player is responding to the offer to begin
		// working for ARC - so send him the "Maiden Flight"
		// mission and a cover note in return, with Revelation attached

		Data *revelation = new Data ();
		revelation->SetTitle ( "Revelation" );
		revelation->SetDetails ( DATATYPE_PROGRAM, 3, 0, 0, 1.0, SOFTWARETYPE_OTHER );

		Message *msg = new Message ();
		msg->SetFrom ( "internal@ARC.net" );
		msg->SetTo ( "PLAYER" );
		msg->SetSubject ( "Welcome to ARC" );
		msg->SetBody ( "Thank you for replying to our offer, agent.  We knew that you "
					   "would make the right decision.\n\n"
					   "As it happens, we have some work for you to get started on immediately.\n"
					   "We are getting ready to test out Revelation and we want you to help.  "
					   "Please read the attached mission description and get started as soon as possible.\n\n"
					   "No doubt you will incur some expenses for this mission, so please accept a small "
					   "donation of 10000c to help you with any equipment you might need.\n\n"
					   "The attached file is to be used as part of your mission.  DO NOT ATTEMPT TO RUN "
					   "THE ATTACHED PROGRAM LOCALLY.\n\n"
					   "[END]" );
		msg->AttachData ( revelation );
		msg->Send ();

		game->GetWorld ()->GetPlayer ()->ChangeBalance ( 10000, "Anonymous donation" );

		Mission *m = GenerateMission_MaidenFlight ();
		if ( UplinkIncompatibleSaveGameAssert ( m, __FILE__, __LINE__ ) )
			return false;

		game->GetWorld ()->GetPlayer ()->GiveMission ( m );

		act = 2;
		scene = 2;

        // Schedule act 2 scene 3 very soon
        // (Arunmor get in contact and offer him another job)

	    Date rundate;
	    rundate.SetDate ( &(game->GetWorld ()->date) );
	    rundate.AdvanceMinute ( 2 );

	    RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
	    rns->SetActAndScene ( 2, 3 );
	    rns->SetRunDate ( &rundate );
	    game->GetWorld ()->scheduler.ScheduleEvent ( rns );

		return true;

	}
	else if ( act == 2 && (scene == 2 || scene == 3) ) {

        IsMissionComplete_MaidenFlight ();
		return true;

	}
    else if ( act == 4 ) {

        if ( playerloyalty == -1 ) {

            if ( strstr ( msg->GetBody (), SpecialMissionDescription (SPECIALMISSION_DARWIN) ) ) {

                if ( !completed_darwin )
                    IsMissionComplete_Darwin ();

            }
            else if ( strstr ( msg->GetBody (), SpecialMissionDescription (SPECIALMISSION_SAVEITFORTHEJURY) ) ) {

                if ( !completed_saveitforthejury )
                    IsMissionComplete_SaveItForTheJury ();

            }
            else if ( strstr ( msg->GetBody (), SpecialMissionDescription (SPECIALMISSION_SHINYHAMMER) ) ) {

                if ( !completed_shinyhammer )
                    IsMissionComplete_ShinyHammer ();

            }

        }

        return true;

    }

#endif			//DEMOGAME

	return false;

}

bool PlotGenerator::PlayerContactsARUNMOR ( Message *msg )
{

#ifndef DEMOGAME

    UplinkAssert (msg);

    if ( act == 2 && scene == 3 ) {

        // Player may have sent us a copy of Revelation
        // In response to our offer

        UplinkAssert (msg);

		Data *attached = msg->GetData ();

		if ( attached && attached->TYPE	== DATATYPE_PROGRAM &&
			 strcmp ( attached->title, "Revelation" ) == 0 ) {

			version_faith = attached->version;

			game->GetWorld ()->GetPlayer ()->ChangeBalance ( PAYMENT_SPECIALMISSION_BACKFIRE, "Arunmor [Faith project]" );
			game->GetWorld ()->GetPlayer ()->rating.ChangeUplinkScore ( 10 );
			game->GetWorld ()->GetPlayer ()->rating.ChangeNeuromancerScore ( 10 );

			Message *msg = new Message ();
			msg->SetTo ( "PLAYER" );
			msg->SetFrom ( "internal@Arunmor.net" );
			msg->SetSubject ( "Your recent submission" );
			msg->SetBody ( "Let us be the first to congratulate you Agent - we have been "
						   "totally unable to find any copies of Revelation "
						   "and we had been hoping that you would decide to work for us.\n\n"
						   "Thanks to your submission, we will now be able to catch up with the "
                           "research of our counter virus to Revelation.  We have already deposited "
						   "the full reward into your account.\n\n"
						   "We will have some more work for you soon regarding this project.  "
                           "We will let you know." );
			msg->Send ();

			specialmissionscompleted |= (1 << SPECIALMISSION_BACKFIRE);

            // Player now works for Arunmor

            playerloyalty = 1;
            act = 2;
            scene = 4;

            // Schedule start of next act for a day from now

			Date rundate;
			rundate.SetDate ( &(game->GetWorld ()->date) );
			rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 12 ) );

			RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
			rns->SetActAndScene ( 3, 1 );
			rns->SetRunDate ( &rundate );
			game->GetWorld ()->scheduler.ScheduleEvent ( rns );

        }

    }
	else if ( act == 3 ) {

		// The player might have sent a copy of Revelation - big news!

		Data *attached = msg->GetData ();

		if ( attached && attached->TYPE	== DATATYPE_PROGRAM &&
			 strcmp ( attached->title, "Revelation" ) == 0 ) {

            if ( playerloyalty != 1 && attached->version > version_faith ) {

				version_faith = attached->version;

				game->GetWorld ()->GetPlayer ()->ChangeBalance ( 25000, "Arunmor [Faith project]" );
				game->GetWorld ()->GetPlayer ()->rating.ChangeUplinkScore ( 10 );
				game->GetWorld ()->GetPlayer ()->rating.ChangeNeuromancerScore ( 10 );

				Message *msg = new Message ();
				msg->SetTo ( "PLAYER" );
				msg->SetFrom ( "Arunmor" );
				msg->SetSubject ( "Your recent submission" );
				msg->SetBody ( "Let us be the first to congratulate you Agent - we have been "
							   "totally unable to recover Revelation from our damaged machines "
							   "and we had been hoping that somebody out there would have a copy of it.\n\n"
							   "Of course, we won't ask where you got it, nor will we let it become known who sent it too us.\n\n"
							   "Thanks to your submission, we will now be able to pull several weeks ahead of Revelation.  We have already deposited "
							   "the full reward of 25000c into your account.\n\n"
							   "We may have some more work for you in future.\n\n"
							   "Once again thank you for your contribution to project Faith." );
				msg->Send ();

			}
			else {

				// He sent us an older version which is no use

				Message *msg = new Message ();
				msg->SetTo ( "PLAYER" );
				msg->SetFrom ( "Arunmor" );
				msg->SetSubject ( "Your recent submission" );
				msg->SetBody ( "Thank you for your recent submission of the Revelation virus, and congratulations on finding "
							   "such a rare item.  Unfortunately, we already have a copy of this version of the virus, so what "
							   "you have sent is will not prove useful.\n\n"
							   "As such, we are unable to pay you any reward.  If you find any newer versions, please send them "
							   "to us." );
				msg->Send ();

			}

		}

		return true;

	}
    else if ( act == 4 ) {

        // Player might have completed one of our missions

        if ( strstr ( msg->GetBody (), SpecialMissionDescription (SPECIALMISSION_TRACER) ) ) {

            if ( !completed_tracer )
                IsMissionComplete_Tracer ();

        }
        else if ( strstr ( msg->GetBody (), SpecialMissionDescription (SPECIALMISSION_TAKEMETOYOURLEADER) ) ) {

            if ( !completed_takemetoyourleader )
                IsMissionComplete_TakeMeToYourLeader ();

        }
        else if ( strstr ( msg->GetBody (), SpecialMissionDescription (SPECIALMISSION_ARCINFILTRATION) ) ) {

            if ( !completed_arcinfiltration )
                IsMissionComplete_ARCInfiltration ();

        }

        return true;

    }


#endif

	return false;

}

void PlotGenerator::RunRevelation ( char *ip, float version, bool playerresponsible )
{

#ifndef DEMOGAME

	// Look up the system attacked

	VLocation *vl = game->GetWorld ()->GetVLocation ( ip );
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);

    if ( comp->isinfected_revelation > 0.0 ) return;

	// Work out how successful he was
	// Based on correct target and how much security was enabled on the target
	// and if he had been spotted when he released it

	int success = -1;				// 0 = total failure, 1 = local system only, 2 = medium success, 3 = total success

	int numsystems = comp->security.NumSystems ();
	int numrunningsystems = comp->security.NumRunningSystems ();

	if		( numrunningsystems == numsystems ) 		success = 1;
	else if ( numrunningsystems == 0 )					success = 3;
	else												success = 2;

	if ( game->GetWorld ()->GetPlayer ()->connection.Traced () ) {

		success = 0;

	}
	else if ( game->GetWorld ()->GetPlayer ()->connection.TraceInProgress () ) {

		success--;

	}

	++numuses_revelation;

	RunRevelation ( ip, version, playerresponsible, success );

	if ( playerresponsible && game->GetWorld ()->GetPlayer ()->GetRemoteHost () == vl ) {

		// The player is still connected - disconnect him!

		game->GetWorld ()->GetPlayer ()->connection.Disconnect ();
		game->GetWorld ()->GetPlayer ()->connection.Reset ();

		game->GetInterface ()->GetRemoteInterface ()->RunNewLocation ();
		game->GetInterface ()->GetRemoteInterface ()->RunScreen ( 2 );

	}

#endif			//DEMOGAME

}

void PlotGenerator::RunFaith ( char *ip, float version, bool playerresponsible )
{

#ifndef DEMOGAME

	//
	// Look up the system attacked
	//

	VLocation *vl = game->GetWorld ()->GetVLocation ( ip );
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);

    if ( comp->isinfected_revelation &&
        version >= comp->isinfected_revelation ) {

        // Works immediately

        comp->DisinfectRevelation ();
        //Disinfected ( ip );

    }
    else {

        // Lower version of Faith - may work

        int percentchance = (int) ( 100 * (1.0 - (float) (comp->isinfected_revelation - version) / 2.0) );
        if ( percentchance <= 33 ) percentchance = 33;
        int random = NumberGenerator::RandomNumber ( 100 );

        if ( random <= percentchance ) {
            comp->DisinfectRevelation ();
          //  Disinfected ( ip );
        }

    }

#endif

}

void PlotGenerator::RunRevelation ( char *ip, float version, bool playerresponsible, int success )
{

#ifndef DEMOGAME

	//
	// Look up the system attacked
	//

	VLocation *vl = game->GetWorld ()->GetVLocation ( ip );
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);

    if ( comp->isinfected_revelation > 0.0 ) return;

    if ( success > 0 ) {

        comp->InfectWithRevelation ( version );
        Infected ( ip );

    }

    if ( version <= 1.0 )
       	NewsRevelationUsed ( ip, success );

/*
	//
	// Now deploy the virus, based on that success value
	//

	if ( success == 0 ) {												// They manage to stop the virus from running

	}
	else if ( success == 1 ) {											// Infect this computer only before it is stopped

		comp->InfectWithRevelation ();

	}
	else if ( success == 2 ) {											// Infect a lot of related computers

		comp->InfectWithRevelation ();

		DArray <Computer *> *comps = game->GetWorld ()->computers.ConvertToDArray ();
		UplinkAssert (comps);

		for ( int i = 0; i < comps->Size (); ++i ) {
			if ( comps->ValidIndex (i) ) {

				Computer *remotecomp = comps->GetData (i);
				UplinkAssert (remotecomp);

				if ( strcmp ( remotecomp->companyname, comp->companyname ) == 0 )
					if ( NumberGenerator::RandomNumber ( 2 ) == 0 )
						remotecomp->InfectWithRevelation ();

			}
		}

		delete comps;

	}
	else if ( success == 3 ) {											// Infect all computers owned by this company

		comp->InfectWithRevelation ();

		DArray <Computer *> *comps = game->GetWorld ()->computers.ConvertToDArray ();
		UplinkAssert (comps);

		for ( int i = 0; i < comps->Size (); ++i ) {
			if ( comps->ValidIndex (i) ) {

				Computer *remotecomp = comps->GetData (i);
				UplinkAssert (remotecomp);

				if ( strcmp ( remotecomp->companyname, comp->companyname ) == 0 )
					remotecomp->InfectWithRevelation ();

			}
		}

		delete comps;

	}
*/

#endif			//DEMOGAME

}

void PlotGenerator::RunRevelationTracer ( char *ip )
{

    UplinkStrncpy ( tracer_lastknownip, ip, sizeof ( tracer_lastknownip ) );

}

float PlotGenerator::GetVersion_Faith ()
{

	return version_faith;

}

float PlotGenerator::GetVersion_Revelation ()
{

	return version_revelation;

}

void PlotGenerator::Run_Act1Scene1 ()
{

	// Scene 2 is the first noticable scene
	// It should happen a few days after a player
	// starts a game

	version_revelation = 0.2f;

	Date rundate;
	rundate.SetDate ( GAME_START_DATE );
	rundate.AdvanceDay ( 1 );
	rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 12 ) );

	RunPlotSceneEvent *rns = new RunPlotSceneEvent ();

#ifdef TESTGAME

    rns->SetActAndScene ( 1, 2 );

//    playerloyalty = 0;
//    version_revelation = 3.0;
//    version_faith = 3.6;
//    WorldGenerator::GenerateEmptyFileServer ( "Arunmor" );


        Mission *darwin = GenerateMission_Darwin ();
        UplinkAssert (darwin);
        game->GetWorld ()->GetPlayer ()->GiveMission ( darwin );

    Computer *test = game->GetWorld ()->GetComputer ("Uplink Test Machine" );
    UplinkAssert ( test );

    Data *data = new Data ();
    data->SetTitle ( "Revelation" );
    data->SetDetails ( DATATYPE_PROGRAM, 3, 0, 0, 1.0, SOFTWARETYPE_OTHER );
    test->databank.PutData ( data );

    Data *data2 = new Data ();
    data2->SetTitle ( "Faith" );
    data2->SetDetails ( DATATYPE_PROGRAM, 3, 0, 0, 2.0, SOFTWARETYPE_OTHER );
    test->databank.PutData ( data2 );

    Data *data3 = new Data ();
    data3->SetTitle ( "Revelation_Tracker" );
    data3->SetDetails ( DATATYPE_PROGRAM, 3, 0, 0, 1.0, SOFTWARETYPE_OTHER );
    test->databank.PutData ( data3 );


#else

    rns->SetActAndScene ( 1, 2 );

#endif

	rns->SetRunDate ( &rundate );
	game->GetWorld ()->scheduler.ScheduleEvent ( rns );

}

void PlotGenerator::Run_Act1Scene2 ()
{

	News *news = new News ();
	news->SetHeadline ( "Unusual hiring practices noticed by analysts" );
	news->SetDetails ( "Andromeda Research Corporation, otherwise known as ARC, have been extremely busy lately.\n\n"
					   "The company has been hiring all of the top Uplink Agents for the past couple of weeks, "
					   "sometimes posting several missions in one day.\n\n"
					   "Details are thin but it appears that the work done is very vague in nature - ARC are "
					   "keeping quiet about the ultimate goal of this work and all of the Agents involved are "
					   "sworn to secrecy.  Analysts predict that ARC has spent over fifty-thousand credits in "
					   "the last week alone on agent fees.\n\n"
					   "At the time of writing, ARC was unwilling to comment." );

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (cu);
	cu->CreateNews ( news );


	//
	// Select the two agents who will be our characters
	// for scenes 3 and 4
	//

	DArray <Person *> *allpeople = game->GetWorld ()->people.ConvertToDArray ();
	LList  <Person *> sorted;

	for ( int ip = 0; ip < allpeople->Size (); ++ip ) {
		if ( allpeople->ValidIndex (ip) ) {

			Person *p = allpeople->GetData (ip);
			UplinkAssert (p);

			if ( p->GetOBJECTID () == OID_AGENT &&
				 p->GetStatus () == PERSON_STATUS_NONE ) {

				bool inserted = false;

				for ( int is = 0; is < sorted.Size (); ++is ) {

					Person *s = sorted.GetData (is);
					UplinkAssert (s);

					if ( p->rating.uplinkscore >= s->rating.uplinkscore ) {

						sorted.PutDataAtIndex ( p, is );
						inserted = true;
						break;

					}

				}

				if ( !inserted ) sorted.PutDataAtEnd ( p );

			}

		}
	}

	delete allpeople;

	UplinkStrncpy ( act1scene3agent, sorted.GetData (1)->name, sizeof ( act1scene3agent ) );
	UplinkStrncpy ( act1scene4agent, sorted.GetData (0)->name, sizeof ( act1scene4agent ) );

	//
	// Set up a log in on scene4 guys computer
	//

    char *computername = NameGenerator::GeneratePersonalComputerName( act1scene4agent );
	Computer *comp = game->GetWorld ()->GetComputer ( computername );
	if ( UplinkIncompatibleSaveGameAssert (comp, __FILE__, __LINE__) )
		return;

#ifdef TESTGAME
	game->GetWorld ()->GetPlayer ()->GiveLink ( comp->ip );
#endif

    ComputerScreen *existing0 = comp->GetComputerScreen (0);
    if ( existing0 ) delete existing0;

	PasswordScreen *ps0 = new PasswordScreen ();
	ps0->SetMainTitle ( comp->name );
	ps0->SetSubTitle ( "Speak friend, and enter" );
	ps0->SetPassword ( "MySocratesNote" );
	ps0->SetDifficulty ( 50 );
	ps0->SetNextPage ( 1 );
	comp->AddComputerScreen ( ps0, 0 );

	MenuScreen *ms1 = new MenuScreen ();
	ms1->SetMainTitle ( comp->name );
	ms1->SetSubTitle ( "Menu" );
	ms1->AddOption ( "Filez", "Mess with my files", 2 );
	ms1->AddOption ( "eMailz", "Edit my mail settings", 3 );
	comp->AddComputerScreen ( ms1, 1 );

	GenericScreen *gs2 = new GenericScreen ();
	gs2->SetMainTitle ( comp->name );
	gs2->SetSubTitle ( "Filez" );
	gs2->SetScreenType ( SCREEN_FILESERVERSCREEN );
	gs2->SetNextPage ( 1 );
	comp->AddComputerScreen ( gs2, 2 );

	DialogScreen *ds3 = new DialogScreen ();
	ds3->SetMainTitle ( comp->name );
	ds3->SetSubTitle ( "eMailz" );
	ds3->AddWidget ( "close", WIDGET_NEXTPAGE, 270, 380, 100, 20, "Close", "Click to close this screen", 1, -1, NULL, NULL );
	comp->AddComputerScreen ( ds3, 3 );

	//
	// Act1Scene3 guy raises suspicion a few days later
	//

	Date rundate;
	rundate.SetDate ( &(game->GetWorld ()->date) );
	rundate.AdvanceDay ( 4 );
	rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 12 ) );

	RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
	rns->SetActAndScene ( 1, 3 );
	rns->SetRunDate ( &rundate );
	game->GetWorld ()->scheduler.ScheduleEvent ( rns );

}

void PlotGenerator::Run_Act1Scene3 ()
{

	if ( UplinkIncompatibleSaveGameAssert ( strcmp ( act1scene3agent, " " ) != 0, __FILE__, __LINE__ ) )
		return;
	Agent *agent = (Agent *) game->GetWorld ()->GetPerson ( act1scene3agent );
	if ( UplinkIncompatibleSaveGameAssert (agent, __FILE__, __LINE__) )
		return;

	std::ostrstream details;
	details << "One of the top ranking Uplink Agents has broken the silence over the recent "
			   "activity of ARC (Andromeda Research Corporation) and raised doubts over the "
			   "morality of the work being done.\n\n"
			   "The announcement comes less than a month after ARC began recruiting all the top "
			   "agents for work on some unspecified project.\n\n"
			   "The man in question, known as Agent " << agent->handle << ", is on one of the top "
			   "ranked Agents in the world and is well respected throughout the hacker community.\n\n"
			   "He states that 'We don't know anything about this company ARC - some of the work "
			   "that i've seen done for them is highly suspicious and could be used in a very "
			   "destructive manner if properly exploited.  ARC are relying on the unofficial code of "
			   "honour of Uplink Agents - that they won't discuss what they were working on.  Someone "
			   "needs to come forward and explain what this company is up to.'\n\n"
			   "Once again, ARC refused to comment, but did say that a press release was imminent."
			   << '\x0';

	News *news = new News ();
	news->SetHeadline ( "High ranking Agent raises doubts over ARC" );
	news->SetDetails ( details.str () );

	details.rdbuf()->freeze( 0 );
	//delete [] details.str ();

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (cu);
	cu->CreateNews ( news );


	//
	// Act1Scene4 guy posts story a few days later
	//

	Date rundate;
	rundate.SetDate ( &(game->GetWorld ()->date) );
	rundate.AdvanceDay ( 2 );
	rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 12 ) );

	RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
	rns->SetActAndScene ( 1, 4 );
	rns->SetRunDate ( &rundate );
	game->GetWorld ()->scheduler.ScheduleEvent ( rns );

}

void PlotGenerator::Run_Act1Scene4 ()
{

	if ( UplinkIncompatibleSaveGameAssert ( strcmp ( act1scene4agent, " " ) != 0, __FILE__, __LINE__ ) )
		return;
	Agent *agent = (Agent *) game->GetWorld ()->GetPerson ( act1scene4agent );
	if ( UplinkIncompatibleSaveGameAssert (agent, __FILE__, __LINE__) )
		return;

	if ( UplinkIncompatibleSaveGameAssert ( strcmp ( act1scene3agent, " " ) != 0, __FILE__, __LINE__ ) )
		return;
	Agent *scene3guy = (Agent *) game->GetWorld ()->GetPerson ( act1scene3agent );
	if ( UplinkIncompatibleSaveGameAssert (scene3guy, __FILE__, __LINE__) )
		return;

	version_revelation = 0.7f;

	//
	// Create the news story
	//

	std::ostrstream details;
	details << "In a suprising development in the mystery of Andromeda Research Corporation (ARC), "
			   "the number one rated Uplink Agent has come forward and tried to re-assure the community.\n\n"
			   "This comes two days after Agent " << scene3guy->handle << " publicly expressed concern "
			   "that ARC were hiring agents to work on some kind of weapon.\n\n"
			   "The Agent, known amoungst the hacker community as " << agent->handle << ", says in his statement "
			   "that 'I have been working for ARC on a confidential project for the past two weeks.  The project "
			   "is immensly ambitious in nature but is nothing to worry about.  ARC have been keeping the public "
			   "in the dark to protect their trade secrets.  Uplink Agents should not be afraid of accepting jobs "
			   "from this company.'\n"
			   "'Some people have said they think ARC are building some kind of military weapon - I can say that I "
			   "have had full access to the ARC Central Mainframe for the past week and there is nothing of the sort "
			   "on the system.  What they are doing is being kept under wraps but it is definately not a weapon.'\n\n"
			   "Agent " << agent->handle << " is a well respected hacker with years of experience and his comments "
			   "will certainly re-assure the community."
			   << '\x0';


	News *news = new News ();
	news->SetHeadline ( "Top Uplink Agent re-assures community" );
	news->SetDetails ( details.str () );

	details.rdbuf()->freeze( 0 );
	//delete [] details.str ();

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (cu);
	cu->CreateNews ( news );

	//
	// Put some interesting logs on the ARC central mainframe
	//

    Computer *comp = game->GetWorld ()->GetComputer ( NameGenerator::GenerateCentralMainframeName("ARC") );
	if ( UplinkIncompatibleSaveGameAssert (comp, __FILE__, __LINE__) )
		return;

	AccessLog *al1 = new AccessLog ();
	al1->SetProperties ( &(game->GetWorld ()->date), WorldGenerator::GetRandomLocation ()->ip, " " );
	al1->SetData1 ( "Deleted file RevelationCoreV0.7" );
	comp->logbank.AddLog ( al1 );

	AccessLog *al2 = new AccessLog ();
	al2->SetProperties ( &(game->GetWorld ()->date), WorldGenerator::GetRandomLocation ()->ip, " " );
	al2->SetData1 ( "Deleted file RevelationDataV0.7" );
	comp->logbank.AddLog ( al2 );

	AccessLog *al3 = new AccessLog ();
	al3->SetProperties ( &(game->GetWorld ()->date), WorldGenerator::GetRandomLocation ()->ip, " " );
	al3->SetData1 ( "Deleted file RevelationSpecV0.7" );
	comp->logbank.AddLog ( al3 );

	//
	// Act1Scene4 guy posts story a few days later
	//

	Date rundate;
	rundate.SetDate ( &(game->GetWorld ()->date) );
	rundate.AdvanceDay ( 7 );
	rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 12 ) );

	RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
	rns->SetActAndScene ( 1, 5 );
	rns->SetRunDate ( &rundate );
	game->GetWorld ()->scheduler.ScheduleEvent ( rns );

}

void PlotGenerator::Run_Act1Scene5 ()
{

	if ( UplinkIncompatibleSaveGameAssert ( strcmp ( act1scene4agent, " " ) != 0, __FILE__, __LINE__ ) )
		return;
	Agent *agent = (Agent *) game->GetWorld ()->GetPerson ( act1scene4agent );
	if ( UplinkIncompatibleSaveGameAssert (agent, __FILE__, __LINE__) )
		return;

    Computer *comp = game->GetWorld ()->GetComputer ( NameGenerator::GenerateCentralMainframeName("ARC") );
	if ( UplinkIncompatibleSaveGameAssert (comp, __FILE__, __LINE__) )
		return;

	//
	// This guy is now dead
	// His last action was to connect to the central mainframe and copy the virus
	// He also sets up the delayed email
	//

	agent->EstablishConnection ( comp->ip );

	AccessLog *al = new AccessLog ();
	al->SetProperties ( &(game->GetWorld ()->date), "unknown", "unknown" );
	al->SetData1 ( "Copied file RevelationCoreV0.7" );
	comp->logbank.AddLog ( al );

    char *hiscomputername = NameGenerator::GeneratePersonalComputerName( act1scene4agent );
	Computer *hiscomp = game->GetWorld ()->GetComputer ( hiscomputername );
	if ( UplinkIncompatibleSaveGameAssert (hiscomp, __FILE__, __LINE__) )
		return;

	Data *data = new Data ();
	data->SetTitle ( "RevelationCoreV0.7" );
	data->SetDetails ( DATATYPE_DATA, 3, 5, 0, 0.7f );
	hiscomp->databank.SetSize ( hiscomp->databank.GetSize () + 5 );
	hiscomp->databank.PutData ( data );

	DialogScreen *ds = (DialogScreen *) hiscomp->GetComputerScreen (3);
	UplinkAssert ( ds->GetOBJECTID () == OID_DIALOGSCREEN );
	ds->AddWidget ( "scheduletitle", WIDGET_CAPTION, 50, 150, 200, 20, "Scheduled emails", "" );
	ds->AddWidget ( "email1", WIDGET_TEXTBOX, 80, 180, 200, 30, "TO: subscriptions@hackers.com\nSUBJECT: continuation of order", "" );
	ds->AddWidget ( "email2", WIDGET_TEXTBOX, 80, 220, 200, 30, "TO: all-agents@uplink.com\nSUBJECT: Warning about ARC", "" );
	ds->AddWidget ( "view1", WIDGET_SCRIPTBUTTON, 280, 180, 50, 14, "View", "Click to view this email", 62, -1, NULL, NULL );
	ds->AddWidget ( "view2", WIDGET_SCRIPTBUTTON, 280, 220, 50, 14, "View", "Click to view this email", 63, -1, NULL, NULL );
	ds->AddWidget ( "cancel1", WIDGET_SCRIPTBUTTON, 280, 195, 50, 14, "Cancel", "Click to stop this mail from being sent", 60, -1, NULL, NULL );
	ds->AddWidget ( "cancel2", WIDGET_SCRIPTBUTTON, 280, 235, 50, 14, "Cancel", "Click to stop this mail from being sent", 61, -1, NULL, NULL );
	ds->AddWidget ( "mailtext", WIDGET_TEXTBOX, 30, 270, 400, 100, "", "" );

	agent->SetStatus ( PERSON_STATUS_DEAD );

	//
	// Generate the news story
	//

	std::ostrstream details;
	details << "Agent " << agent->handle << " has been found dead in his house after an apparent suicide.\n"
			   "Preliminary forensic analysis suggests he overdosed on sleeping pills.\n\n"
			   "He was at the top of the Agent leaderboard and made the news recently after commenting on the "
			   "actions of Andromeda Research Corporation (ARC).  He had been working for them on their latest "
			   "product which is currently shrowded in mystery.  This latest development will no doubt add to "
			   "the mystery surrounding the project.\n\n"
			   "ARC released a statement earlier today, stating 'He was the best programmer we had ever seen and "
			   "was a lead player in the project.  For something like this happen is a tradgedy of the greatest order.  "
			   "Our thoughts go out to this mans family and friends.'"
			   << '\x0';

	News *news = new News ();
	news->SetHeadline ( "Top agent working for ARC found dead" );
	news->SetDetails ( details.str () );

	details.rdbuf()->freeze( 0 );
	//delete [] details.str ();

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (cu);
	cu->CreateNews ( news );

	//
	// Act1Scene4 guys email will go out 5 days later -
	// The player has until then to figure out what to do
	// (If he's even listening to this)
	//

	Date rundate;
	rundate.SetDate ( &(game->GetWorld ()->date) );
	rundate.AdvanceDay ( 5 );
	rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 12 ) );

	RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
	rns->SetActAndScene ( 1, 7 );
	rns->SetRunDate ( &rundate );
	game->GetWorld ()->scheduler.ScheduleEvent ( rns );

}

void PlotGenerator::Run_Act1Scene7 ()
{

	if ( !playercancelsmail ) {

		if ( UplinkIncompatibleSaveGameAssert ( strcmp ( act1scene4agent, " " ) != 0, __FILE__, __LINE__ ) )
			return;
		Agent *agent = (Agent *) game->GetWorld ()->GetPerson ( act1scene4agent );
		if ( UplinkIncompatibleSaveGameAssert (agent, __FILE__, __LINE__) )
			return;

		// Look up the admin password ..........................

        Computer *comp = game->GetWorld ()->GetComputer ( NameGenerator::GenerateCentralMainframeName("ARC") );
		if ( UplinkIncompatibleSaveGameAssert (comp, __FILE__, __LINE__) )
			return;

		std::ostrstream body;
		body << "[Autosend - 7 day delay expired]\n"
				"[Sent to ALL-AGENTS]\n\n"
				"Agents,\n"
				"If you are reading this email then it means I am dead.  I have been working for "
				"Andromeda for the past month on a top secret project - and until yesterday I did not "
				"even know what exactly I was coding.\n\n"
				"This story hit the news and I decided that I should try to find out what was happening "
				"with project Revelation - and yesterday I managed to gain access to their stand-alone "
				"server room and copy the files that we were working on.\n\n"
				"Unfortunately they saw what I was doing and I had to leave quick, and I haven't had time "
				"to analyse the project files yet.  If you read this then they have already caught me.\n\n"
				"ARC cannot be trusted.  We must come together as a group and find out what they are up to "
				"before something terrible happens.  I don't know if it's already too late."
				<< '\x0';

		char from [32];
		UplinkSnprintf ( from, sizeof ( from ), "Agent %s", agent->handle );

		// Include an access code, but it shouldn't work (they've disabled it by now, of course)

		char code [128];
        Computer::GenerateAccessCode( agent->handle, "MySocratesNote", code, sizeof ( code ) );

		Message *msg = new Message ();
		msg->SetTo ( "PLAYER" );
		msg->SetFrom ( from );
		msg->SetSubject ( "Warning about ARC" );
		msg->SetBody ( body.str () );
		msg->GiveLink ( comp->ip );
		msg->GiveCode ( comp->ip, code );
		msg->Send ();

		body.rdbuf()->freeze( 0 );
		//delete [] body.str ();

        // Make sure that mail doesn't appear on his computer any more

        char *hiscomputername = NameGenerator::GeneratePersonalComputerName( act1scene4agent );
	    Computer *hiscomp = game->GetWorld ()->GetComputer ( hiscomputername );
	    if ( UplinkIncompatibleSaveGameAssert (hiscomp, __FILE__, __LINE__) )
			return;

    	DialogScreen *ds = (DialogScreen *) hiscomp->GetComputerScreen (3);
	    UplinkAssert ( ds->GetOBJECTID () == OID_DIALOGSCREEN );

        ds->RemoveWidget ( "email2" );
        ds->RemoveWidget ( "view2" );
        ds->RemoveWidget ( "cancel2" );

	}

	//
	// Uplink issue a formal warning to all Agents
	//

	Date rundate;
	rundate.SetDate ( &(game->GetWorld ()->date) );
	rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 12 ) + 2 );

	RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
	rns->SetActAndScene ( 1, 8 );
	rns->SetRunDate ( &rundate );
	game->GetWorld ()->scheduler.ScheduleEvent ( rns );

}

void PlotGenerator::Run_Act1Scene8 ()
{

	if ( !playercancelsmail ) {

		std::ostrstream details;
		details << "Uplink Corporation has issued a formal warning to all of its agents, warning them "
				   "against accepting work for Andromeda Research Corporation (ARC).\n\n"
				   "In a statement released today, they say 'Uplink Corporation is currently investigating "
				   "the claim that ARC have been developing some kind of high-tech weapon, using the "
				   "expertise of freelance Uplink agents to assist them.  Federal agents have been called in "
				   "to assist and are also persuing the matter of a recent suicide, which they believe may "
				   "have been a related murder.\n\n"
				   "'Until we can determine if these charges have any truth in them, we must insist that all "
				   "Uplink Agents refuse any work being offered to them from ARC or anyone working for that company.  "
				   "Anyone caught working for this company will lose their Uplink status immediately.\n\n";
#ifdef DEMOGAME
        details << "'In addition, all news and data related to ARC will be classified and restricted to those agents "
				   "with fully registered accounts.  Those agents using the Demo Uplink client will not be able to "
				   "view any more of this information, in the interests of our own internal security.\n\n"
				   "'Any agents using the Demo Uplink client who wish to purchase the full client can do so by "
				   "visiting www.introversion.co.uk'";
#endif
        details << '\x0';

		News *news = new News ();
		news->SetHeadline ( "Formal warning issued by Uplink Corporation" );
		news->SetDetails ( details.str () );

		CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
		UplinkAssert (cu);
		cu->CreateNews ( news );

	}

#ifndef DEMOGAME

	//
	// "Maiden Flight" is given out a few days later
	//

	Date rundate;
	rundate.SetDate ( &(game->GetWorld ()->date) );
	rundate.AdvanceDay ( 3 );
	rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 12 ) );

	RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
	rns->SetActAndScene ( 2, 1 );
	rns->SetRunDate ( &rundate );
	game->GetWorld ()->scheduler.ScheduleEvent ( rns );

#endif

}

#ifndef DEMOGAME

void PlotGenerator::Run_Act2Scene1 ()
{

	// ARC remove act1scene4agents computer
	// And replace it with a message screen

    char *hiscomputername;
	{
		char * temp = NameGenerator::GeneratePersonalComputerName( act1scene4agent );
		hiscomputername = new char[ strlen( temp ) + 1 ];
		UplinkSafeStrcpy( hiscomputername, temp );
	}
	Computer *hiscomp = game->GetWorld ()->GetComputer ( hiscomputername );
    if ( UplinkIncompatibleSaveGameAssert (hiscomp, __FILE__, __LINE__) )
		return;
	char *ip = hiscomp->ip;
	game->GetWorld ()->computers.RemoveData ( hiscomputername );

	Computer *comp = new Computer ();
	comp->SetTYPE ( COMPUTER_TYPE_PERSONALCOMPUTER );
	comp->SetName ( hiscomputername );
	comp->SetCompanyName ( act1scene4agent );
	comp->SetTraceSpeed ( 0 );
	comp->SetTraceAction ( COMPUTER_TRACEACTION_DISCONNECT );
	comp->SetIsTargetable ( false );
	comp->SetIP ( ip );

    delete [] hiscomputername;

	MessageScreen *ms0 = new MessageScreen ();
	ms0->SetMainTitle ( "404 error" );
	ms0->SetSubTitle ( "Computer not found" );
	ms0->SetTextMessage ( "This IP address is currently unused.\n"
						  "If you would like to purchase it, please contact sales@internic.net." );
	comp->AddComputerScreen ( ms0 );

	game->GetWorld ()->CreateComputer ( comp );


	version_revelation = 1.0;


	// If the player found the Agents computer
    // Or if the player has revelation
    // Or if the player visited any of the important systems involved
	// then ARC will offer this mission to the player
	// Otherwise it will go to another high ranking agent.

    if ( game->GetWorld ()->GetPlayer ()->gateway.databank.GetData ( "RevelationCoreV0.7" ) ) {

		std::ostrstream body;
		body << "Agent " << game->GetWorld ()->GetPlayer ()->handle << ",\n"
			 << "We couldn't help but notice that you have been poking into our "
                "business recently, and if our backup logs are correct, you are now "
                "in possesion of something that is very important to us.\n\n"
                "Of course, you could make use of this data.  You could probably get "
                "quite a lot of money for it.  Or you could come and work for us.\n"
                "You obviously have the talent.  We were amazed when we realised what you had done.\n\n"
                "We believe you will find that we can offer huge rewards for successfull applicants.\n\n"
				"Reply to this email (or send one to internal@ARC.net) if you are interested in discussing this further."
			 << '\x0';

		Message *msg = new Message ();
		msg->SetFrom ( "internal@ARC.net" );
		msg->SetSubject ( "An offer you can't refuse" );
		msg->SetTo ( "PLAYER" );
		msg->SetBody ( body.str () );
		msg->Send ();

		body.rdbuf()->freeze( 0 );
		//delete [] body.str ();

    }
	else if ( playercancelsmail ) {

	    Agent *act1scene4 = (Agent *) game->GetWorld ()->GetPerson ( act1scene4agent );
    	if ( UplinkIncompatibleSaveGameAssert (act1scene4, __FILE__, __LINE__) )
			return;

		std::ostrstream body;
		body << "Agent " << game->GetWorld ()->GetPlayer ()->handle << ",\n"
			 << "We have been tidying up some of our loose ends recently and we "
			    "couldn't help but notice that you seem to have done us a very "
				"large favour.  It appears that Agent " << act1scene4->handle <<
				" was quite resourceful and would have sent a warning mail to all "
				"agents about us, almost from beyond the grave.\n\n"
				"We didn't find out about this until it would have been too late, "
				"but we were very suprised to see that you stopped this from happening "
				"yourself.\n\n"
				"We're not sure on your motives but would like to offer you some more "
				"work if you are interested.  We will reward you if you decide to accept "
				"of course.\n\n"
				"Why are we offering this to you?  You managed to trace back to his "
				"computer system several days before any of our guys could get there.  "
				"You obviously have talent.  And your motives appear to be good as well.\n\n"
				"Reply to this email (or send one to internal@ARC.net) if you are interested in discussing this further."
			 << '\x0';

		Message *msg = new Message ();
		msg->SetFrom ( "internal@ARC.net" );
		msg->SetSubject ( "An offer you can't refuse" );
		msg->SetTo ( "PLAYER" );
		msg->SetBody ( body.str () );
		msg->Send ();

		body.rdbuf()->freeze( 0 );
		//delete [] body.str ();

	}
    else if ( playervisitsplotsites ) {

		std::ostrstream body;
		body << "Agent " << game->GetWorld ()->GetPlayer ()->handle << ",\n"
			 << "We couldn't help but notice that you have recently been poking around in "
                "some of our computer systems, presumably attempting to determine what is "
                "happening with our company at the moment. We feel we should warn you at this "
                "time that any attempt to gain unauthorised access to our systems will result in "
                "SEVERE consequences.\n\n"
                "Pleasantries aside, You may have heard that we have been hiring a number of Uplink "
                "Agents recently for a yet-to-be-announced project. Your recent activities caused us to "
                "run a background check on you, and you seem to be the kind of agent we are looking for.\n\n"
                "If you would be interested in doing some work for us, we'd love to hear from you. Reply "
                "to this email or send one to internal@ARC.net, and we will see what we can do.  We think "
                "you will find that ARC offer the best rates for freelance agents in the business.\n\n"
                "We look forward to hearing from you."
			 << '\x0';

		Message *msg = new Message ();
		msg->SetFrom ( "internal@ARC.net" );
		msg->SetSubject ( "An offer you can't refuse" );
		msg->SetTo ( "PLAYER" );
		msg->SetBody ( body.str () );
		msg->Send ();

		body.rdbuf()->freeze( 0 );
		//delete [] body.str ();

    }

	// Schedule act 2 scene 2 for 1 weeks time
	// At this time, another agent will do the mission
	// if the player hasn't already

	Date rundate;
	rundate.SetDate ( &(game->GetWorld ()->date) );
	rundate.AdvanceDay ( 7 );
	rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 12 ) );

	RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
	rns->SetActAndScene ( 2, 2 );
	rns->SetRunDate ( &rundate );
	game->GetWorld ()->scheduler.ScheduleEvent ( rns );

}

void PlotGenerator::Run_Act2Scene2 ()
{

	// If the player hasn't done the Arunmor test mission,
	// Someone else will now do it.
    // If his loyalty is -1, he did the mission.
    // If it is 1, he betrayed ARC.
    // If it is 0, he has done neither so someone else should.

	if ( playerloyalty == 0 ) {

		// Do the mission

        Computer *comp = game->GetWorld ()->GetComputer ( NameGenerator::GenerateInternalServicesServerName("Arunmor") );

		++numuses_revelation;
		RunRevelation ( comp->ip, 1.0, false, 2 );

        version_faith = 0.2f;

		// Schedule the main news story for a few days from now

		Date rundate;
		rundate.SetDate ( &(game->GetWorld ()->date) );
		rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 12 ) );

		RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
		rns->SetActAndScene ( 3, 1 );
		rns->SetRunDate ( &rundate );
		game->GetWorld ()->scheduler.ScheduleEvent ( rns );

	}

}

void PlotGenerator::Run_Act2Scene3 ()
{

    // Arunmor get in contact with the player
    // And try to convince him to defect to them
    // By sending them the Revelation Virus

    std::ostrstream body;
    body << "Agent " << game->GetWorld ()->GetPlayer ()->handle << "\n\n"
         << "Firstly let us congratulate you on scoring such a lucrative deal with "
            "the Andromeda Research Corporation.  Secondly, let us warn you that you "
            "are making a big mistake.\n\n"
            "You must have read the headlines, and by now you have probably guessed what "
            "ARC are up to.  Our sources tell us you are about to test run ARCs secret "
            "project on OUR computer systems.  We can only pray this message reaches you "
            "before you go ahead.\n\n"
            "Whatever ARC have offered you, we can do better.  We have a vested interest "
            "in stopping ARC, so we would like to offer you a counter-proposal.  Send us "
            "a copy of whatever ARC sent you, and we'll reward you with " << PAYMENT_SPECIALMISSION_BACKFIRE << "c.  You can "
            "work for us full time.\n\n"
            "Simply reply to this email with the ARC data attached.\n\n"
            "Before you answer, think about the moral implications of what you are about "
            "to do.  Can you really trust ARC, who recently made the news by murdering "
            "two of their best Uplink Agents?"
         << '\x0';

    Message *msg = new Message ();
    msg->SetTo ( "PLAYER" );
    msg->SetFrom ( "internal@Arunmor.net" );
    msg->SetSubject ( "Counter offer from a friend" );
    msg->SetBody ( body.str () );
    msg->Send ();

	body.rdbuf()->freeze( 0 );
    //delete [] body.str ();

}

void PlotGenerator::Run_Act3Scene1 ()
{

    // Remove the ARC mission from the player

    PlayerFailsSpecialMission ( SPECIALMISSION_MAIDENFLIGHT );


    if ( playerloyalty == 1 ) {

        // The player betrayed ARC and went over to Arunmor

        Message *msg = new Message ();
        msg->SetTo ( "PLAYER" );
        msg->SetFrom ( "internal@ARC.net" );
        msg->SetSubject ( "Warning" );
        msg->SetBody ( "So it seems that you were working for the enemy behind our backs all this time.  You cannot imagine "
                       "how suprised we were when we heard that Arunmor had received a complete copy of Revelation, and that "
                       "it was an Uplink Agent who was the source.  Unfortunately for you, you are the only Uplink Agent "
                       "who could have sent them this copy.  This makes you our new enemy.\n\n"
                       "Take this message however you want - but Andromeda Research Corporation will not let this betrayal go "
                       "unpunished.  We'll be watching you, and when we finally come after you, you're going to wish you had "
                       "never heard of Arunmor Corporation or Uplink." );
        msg->Send ();

        News *news = new News ();
	    news->SetHeadline ( "Arunmor Security scare - a special report" );
	    news->SetDetails ( "Earlier today Arunmor Corporation shut down all of its primary computer systems and disconnected "
                           "their Internal Network from the web. The disconnection lasted just under one hour, and caused a great "
                           "deal of disruption to Internet traffic flowing through their systems.\n\n"
                           "Arunmor says they had received word that they were about to be attacked by a group of hackers "
                           "working for the controversial Andromeda Research Corporation. So far Arunmor has not said why they suspect "
                           "this company. It is believed they had some kind of inside information.\n\n"
                           "This latest accusation casts Andromeda Research Corporation in an increasingly bad light." );

    	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	    UplinkAssert (cu);
	    cu->CreateNews ( news );

    }
    else {

        // The virus was launched

        News *news = new News ();
	    news->SetHeadline ( "The Arunmor virus outbreak - a special report" );
	    news->SetDetails ( "Arunmor Corporation recently suffered an attack from a new computer virus, dubbed by "
					       "industry experts as 'Revelation'.\n\n"
					       "This virus has never been seen before and contains a payload capable of destroying "
					       "almost any computer it is used against.  Currently the virus does not appear to spread "
					       "particularly fast and it is limited to systems owned by the target company, but it is believed "
					       "that whoever is responsible is probably still working on improving it.\n\n"
					       "Recently a high ranking Uplink Agent contacted us and suggested that Andromeda Research Corporation "
					       "might be connected to this viral outbreak.  Agents have been assigned the task of investigating." );

    	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	    UplinkAssert (cu);
	    cu->CreateNews ( news );

    }


	Date rundate;
	rundate.SetDate ( &(game->GetWorld ()->date) );
	rundate.AdvanceDay ( 1 );
	rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 12 ) );

	RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
	rns->SetActAndScene ( 3, 2 );
	rns->SetRunDate ( &rundate );
	game->GetWorld ()->scheduler.ScheduleEvent ( rns );

}

void PlotGenerator::Run_Act3Scene2 ()
{

	News *news = new News ();
	news->SetHeadline ( "Arunmor : we'll retaliate" );
	news->SetDetails ( "Following the recent attempted attack on the Arunmor computer network, "
                       "Arunmor have announced that they have begun work on a new project - dubbed 'Faith'. "
					   "The aim of this project is to stop the release of destructive computer viruses onto the Net.\n\n"
					   "A company spokesman said 'We cannot allow any virus, including the so called Revelation virus, "
					   "to roam freely around our corporate networks.  Project Faith will actively seak out "
					   "Revelation and other destructive viruses, and will totally eradicate them.  At this moment "
					   "we are seeking Government assistance as this project could help everyone.  The recent "
					   "attempted attack on our network highlighted how weak Internet security is.  We intend to put a stop to this.'\n\n"
					   "Arunmor has taken the unprecented step of offering a reward of 25000 credits to anyone who "
					   "can help them with their research into Revelation.  Anyone with any data related to this "
					   "virus should send it to internal@Arunmor.net to claim their reward." );

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (cu);
	cu->CreateNews ( news );

	// Version info now becomes available at Uplink

	News *news2 = new News ();
	news2->SetHeadline ( "New Uplink service available" );
	news2->SetDetails ( "In light of the recent developments related to Revelation and Faith, Uplink Corporation "
					   "has installed a new system which will allow agents to track the progress of the two rival "
					   "viruses.\n\nA new option has appeared in the main menu, titled 'Faith Progress'.  Clicking this "
					   "option will show you the current latest versions of both virusus.  This data is based on reports "
					   "of these programs on the Web, so they may not be entirely accurate.\n\n" );

	cu->CreateNews ( news2 );

	Computer *comp = game->GetWorld ()->GetComputer ( NAME_UPLINKINTERNALSERVICES );
	UplinkAssert (comp);
	UplinkAssert ( comp->screens.ValidIndex ( 1 ) );
	MenuScreen *menu2 = (MenuScreen *) comp->screens.GetData (1);
	UplinkAssert (menu2);
	UplinkAssert (menu2->GetOBJECTID () == OID_MENUSCREEN );
	menu2->AddOption ( "Faith Progress", "Shows the latest known versions of Faith and Revelation", 3, 10, 5 );

	GenericScreen *gs3 = new GenericScreen ();
	gs3->SetScreenType ( SCREEN_FAITHSCREEN );
	gs3->SetMainTitle ( comp->name );
	gs3->SetSubTitle ( "Faith V Revelation" );
	gs3->SetNextPage ( 1 );
	comp->AddComputerScreen ( gs3, 3 );


	Date rundate;
	rundate.SetDate ( &(game->GetWorld ()->date) );
	rundate.AdvanceDay ( 2 );
	rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 12 ) );

	RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
	rns->SetActAndScene ( 3, 3 );
	rns->SetRunDate ( &rundate );
	game->GetWorld ()->scheduler.ScheduleEvent ( rns );

}

void PlotGenerator::Run_Act3Scene3 ()
{

    std::ostrstream details;
    details <<  "Arunmor Corporation have been working hard on researching the recently discovered "
                "Revelation project, and have released details of their findings.\n\n"
                "'Revelation is the most destructive computer virus we have ever seen' said the CEO of "
                "Arunmor Corporation earlier today.  'At first we thought it was targetted specifically at us. "
                "It has now become clear that the aim of Revelation is to totally destroy the Internet, from the inside.\n\n"
                "'Once released, it will spead like wildfire to all the main systems, then eventually to the entire Internet, "
                "destroying everything in its path.  Its payload is extremely well designed and will take down any system "
                "it comes up against.'\n\n";

    if ( playerloyalty == 1 )
        details << "'We have hired a small number of capable Uplink Agents to help work against this.'\n\n";

    else
        details << "We will be posting missions on the Uplink BBS mission board.  Any agents that can help us will be "
                   "well rewarded, and will be fighting for a good cause.\n\n"
                   "'The offer of 25000c for any data on related to Revelation is still open, and will remain open "
                   "for the next 48 hours.'\n\n";

    details << "'Our Faith project is now aiming to provide a counter-virus that will stop Revelation in its tracks.'\n";
    details << "Federal Agents today began an investigation into the activities of Andromeda Research Corporation.";
    details << '\x0';


    News *news = new News ();
    news->SetHeadline ( "Arunmor release details on Revelation" );
    news->SetDetails ( details.str () );
	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (cu);
	cu->CreateNews ( news );

	details.rdbuf()->freeze( 0 );
    //delete [] details.str ();


	Date rundate;
	rundate.SetDate ( &(game->GetWorld ()->date) );
	rundate.AdvanceDay ( 2 );
	rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 12 ) );

	RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
	rns->SetActAndScene ( 3, 4 );
	rns->SetRunDate ( &rundate );
	game->GetWorld ()->scheduler.ScheduleEvent ( rns );

}

void PlotGenerator::Run_Act3Scene4 ()
{

    std::ostrstream details;
    details << "Federal Agents today made their first public announcement on their investigation into Andromeda Research Corporation (ARC).\n\n"
               "'Our investigation so far has uncovered a number of disturbing facts, however no concrete evidence has yet been discovered' "
               "said a Federal Spokesman. 'It would appear that ARC are developing a software tool in an attempt to damage the operation of the "
               "Internet. So far no evidence has been found and no witnesses have come forward to back this up.'\n\n"
               "'It is our belief that ARC were responsible for the murder of " << act1scene4agent << " and possibly one other, but until "
               "we aquire evidence we will be unable to act in any legal way.'\n\n"
               "Most importantly, we have determined that Andromeda Research Corporation is privately owned and controlled by the secretive group 'Andromeda', "
               "and that the CEO and the Managing Director of ARC are both core members of this group.  Our profile of Andromeda lists them as "
               "well funded Anti-Capitalist Techno-Anarchists, which would seem to fit the charges against them.  So far, we have not had any success in finding this group.'\n\n"
               "'Their only real-world presence that we are aware of is their Public Access Terminal, but the registered contact details of that site are all fake.'\n\n"
               "'We intend to continue our investigation and appeal for anyone with any information to come forward.'"
            << '\x0';


    News *news = new News ();
    news->SetHeadline ( "Federal Agents release findings on ARC" );
    news->SetDetails ( details.str () );
	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (cu);
	cu->CreateNews ( news );

	details.rdbuf()->freeze( 0 );
    //delete [] details.str ();


	Date rundate;
	rundate.SetDate ( &(game->GetWorld ()->date) );
	rundate.AdvanceDay ( 2 );
	rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 12 ) );

	RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
	rns->SetActAndScene ( 4, 1 );
	rns->SetRunDate ( &rundate );
	game->GetWorld ()->scheduler.ScheduleEvent ( rns );

}

void PlotGenerator::Run_Act4Scene1 ()
{

    // Arunmor put out "Tracer"

    Mission *tracer = GenerateMission_Tracer ();
    if ( UplinkIncompatibleSaveGameAssert (tracer, __FILE__, __LINE__) )
		return;

    if ( playerloyalty == 1 ) {

        game->GetWorld ()->GetPlayer ()->GiveMission ( tracer );

    }
    else {

	    CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	    UplinkAssert ( cu );
	    cu->CreateMission ( tracer );

        // Make sure somebody does it eventually

	    Date rundate;
	    rundate.SetDate ( &(game->GetWorld ()->date) );
        rundate.AdvanceDay ( 7 );
	    rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 2 ) );

	    RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
	    rns->SetActAndScene ( 4, 13 );
	    rns->SetRunDate ( &rundate );
	    game->GetWorld ()->scheduler.ScheduleEvent ( rns );

    }

	Date rundate;
	rundate.SetDate ( &(game->GetWorld ()->date) );
    rundate.AdvanceDay ( 4 );
	rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 5 ) );

	RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
	rns->SetActAndScene ( 4, 7 );
	rns->SetRunDate ( &rundate );
	game->GetWorld ()->scheduler.ScheduleEvent ( rns );

}

void PlotGenerator::Run_Act4Scene2 ()
{

    // Arunmor put out "Take me to your leader"

    Mission *takeme = GenerateMission_TakeMeToYourLeader ();
    if ( UplinkIncompatibleSaveGameAssert (takeme, __FILE__, __LINE__) )
		return;

    if ( playerloyalty == 1 ) {

        game->GetWorld ()->GetPlayer ()->GiveMission ( takeme );

    }
    else {

        CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	    UplinkAssert ( cu );
	    cu->CreateMission ( takeme );

        // Make sure somebody does it eventually

	    Date rundate;
	    rundate.SetDate ( &(game->GetWorld ()->date) );
        rundate.AdvanceDay ( 7 );
	    rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 2 ) );

	    RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
	    rns->SetActAndScene ( 4, 14 );
	    rns->SetRunDate ( &rundate );
	    game->GetWorld ()->scheduler.ScheduleEvent ( rns );

    }

	Date rundate;
	rundate.SetDate ( &(game->GetWorld ()->date) );
    rundate.AdvanceDay ( 4 );
	rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 5 ) );

	RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
	rns->SetActAndScene ( 4, 8 );
	rns->SetRunDate ( &rundate );
	game->GetWorld ()->scheduler.ScheduleEvent ( rns );

}

void PlotGenerator::Run_Act4Scene3 ()
{

    // Arunmor put out "ARC Infiltration"

    Mission *mission = GenerateMission_ARCInfiltration ();
    if ( UplinkIncompatibleSaveGameAssert (mission, __FILE__, __LINE__) )
		return;

    if ( playerloyalty == 1 ) {

        game->GetWorld ()->GetPlayer ()->GiveMission ( mission );

    }
    else {

	    CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	    UplinkAssert ( cu );
	    cu->CreateMission ( mission );

        // Make sure somebody does it eventually

	    Date rundate;
	    rundate.SetDate ( &(game->GetWorld ()->date) );
        rundate.AdvanceDay ( 7 );
	    rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 2 ) );

	    RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
	    rns->SetActAndScene ( 4, 15 );
	    rns->SetRunDate ( &rundate );
	    game->GetWorld ()->scheduler.ScheduleEvent ( rns );

    }

	Date rundate;
	rundate.SetDate ( &(game->GetWorld ()->date) );
    rundate.AdvanceDay ( 4 );
	rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 5 ) );

	RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
	rns->SetActAndScene ( 4, 9 );
	rns->SetRunDate ( &rundate );
	game->GetWorld ()->scheduler.ScheduleEvent ( rns );

}

void PlotGenerator::Run_Act4Scene4 ()
{

	News *news = new News ();
	news->SetHeadline ( "ARC Internal Services System attacked" );
	news->SetDetails ( "The Internal Services System run by Andromeda Research Corporation (ARC) has been "
                       "penetrated by an unknown hacker.  This did not come to light until a few hours ago, when "
                       "system administrators noticed a discrepancy in the access logs on the system.\n\n"
                       "'This is the latest example of the kind of abuse our company is having to deal with' said "
                       "the public relations officer of ARC. 'Ever since these ridiculous allegations about us "
                       "developing some doomsday weapon, we have had serious problems attracting new business.  Now this.'\n\n"
                       "It is not known what the hacker did - he was very careful and covered his tracks. ARC are "
                       "launching a full investigation into this incident." );

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (cu);
	cu->CreateNews ( news );

}

void PlotGenerator::Run_Act4Scene5 ()
{

	News *news = new News ();
	news->SetHeadline ( "CEO of Andromeda Research Corporation arrested" );
	news->SetDetails ( "Following an outbreak of the Revelation computer virus on Arunmor Corporations Internal Services "
                       "System, the CEO of ARC has been arrested by Federal Agents and charged with the crime.  Federal "
                       "officials say they have been watching his company for some time, and have been waiting for "
                       "an opportunity to bring him in.\n\n"
                       "Federal Agents have refused to comment on the results of their questioning at this time." );

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (cu);
	cu->CreateNews ( news );

}

void PlotGenerator::Run_Act4Scene6 ()
{

	News *news = new News ();
	news->SetHeadline ( "ARC Central Mainframe compromised" );
	news->SetDetails ( "Industry experts have expressed suprise at the discovery that the ARC central Mainframe was "
                       "yesteday hacked wide open, and over 50 Gigaquads of data was stolen.  The system has come under "
                       "attack many times recently because of ARC's alleged involvement in the construction of some kind "
                       "of digital weapon, but it was previously believed to be inpenetrable.\n\n"
                       "The ARC central mainframe stores most of its critical data on a stand-alone file server, which is "
                       "physically disconnected from the rest of the Internet.  This ingenious system has stopped all "
                       "previous attempts at access.\n\n"
                       "'We now know that somebody inside our company betrayed us' stated ARCs public relations officer in "
                       "a press release this morning. 'We have found equipment which suggests somebody temporarily connected "
                       "the file server to the outside world with a remote link, which left the file server open to access "
                       "for a few minutes.  They must have been working with somebody on the outside to pull this off.'" );


	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (cu);
	cu->CreateNews ( news );

}

void PlotGenerator::Run_Act4Scene7 ()
{

    //
    // Arc put out the Darwin special mission
    //

    if ( playerloyalty == -1 ) {

        Mission *darwin = GenerateMission_Darwin ();
        if ( UplinkIncompatibleSaveGameAssert (darwin, __FILE__, __LINE__) )
			return;
        game->GetWorld ()->GetPlayer ()->GiveMission ( darwin );

    }
    else {

        // Another agent does the mission now

        completed_darwin = true;
        version_revelation += 2.0;

        // Schedule news story

        Date rundate;
		rundate.SetDate ( &(game->GetWorld ()->date) );
		rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 6 ) );

		RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
		rns->SetActAndScene ( 4, 10 );
		rns->SetRunDate ( &rundate );
		game->GetWorld ()->scheduler.ScheduleEvent ( rns );


    }


    // Next special mission shortly

  	Date rundate;
	rundate.SetDate ( &(game->GetWorld ()->date) );
	rundate.AdvanceDay ( 4 );
	rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 12 ) );

	RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
	rns->SetActAndScene ( 4, 2 );
	rns->SetRunDate ( &rundate );
	game->GetWorld ()->scheduler.ScheduleEvent ( rns );

}

void PlotGenerator::Run_Act4Scene8 ()
{

    //
    // ARC put out the SaveItForTheJury special mission

    if ( playerloyalty == -1 ) {

        Mission *saveit = GenerateMission_SaveItForTheJury ();
        if ( UplinkIncompatibleSaveGameAssert (saveit, __FILE__, __LINE__) )
			return;
        game->GetWorld ()->GetPlayer ()->GiveMission ( saveit );

    }
    else {

        // Another agent does the mission now

        completed_saveitforthejury = true;
        version_faith -= 1.0f;
        if ( version_faith < 0.2f ) version_faith = 0.2f;

        Date rundate;
		rundate.SetDate ( &(game->GetWorld ()->date) );
		rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 3 ) );

		RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
		rns->SetActAndScene ( 4, 11 );
		rns->SetRunDate ( &rundate );
		game->GetWorld ()->scheduler.ScheduleEvent ( rns );

    }


 	Date rundate;
	rundate.SetDate ( &(game->GetWorld ()->date) );
	rundate.AdvanceDay ( 4 );
	rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 12 ) );

	RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
	rns->SetActAndScene ( 4, 3 );
	rns->SetRunDate ( &rundate );
	game->GetWorld ()->scheduler.ScheduleEvent ( rns );

}

void PlotGenerator::Run_Act4Scene9 ()
{

    //
    // ARC put out the ShinyHammer special mission

    if ( playerloyalty == -1 ) {

        Mission *shiny = GenerateMission_ShinyHammer ();
        if ( UplinkIncompatibleSaveGameAssert (shiny, __FILE__, __LINE__) )
			return;
        game->GetWorld ()->GetPlayer ()->GiveMission ( shiny );

    }
    else {

        // Another agent gets the mission

        completed_shinyhammer = true;
        version_faith -= 1.0f;
        if ( version_faith < 0.2f ) version_faith = 0.2f;

        Date rundate;
		rundate.SetDate ( &(game->GetWorld ()->date) );
		rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 3 ) );

		RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
		rns->SetActAndScene ( 4, 12 );
		rns->SetRunDate ( &rundate );
		game->GetWorld ()->scheduler.ScheduleEvent ( rns );

    }

    // Schedule the end game soon

 	Date rundate;
	rundate.SetDate ( &(game->GetWorld ()->date) );
	rundate.AdvanceDay ( 10 );
	rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 12 ) );

	RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
	rns->SetActAndScene ( 5, 1 );
	rns->SetRunDate ( &rundate );
	game->GetWorld ()->scheduler.ScheduleEvent ( rns );

}

void PlotGenerator::Run_Act4Scene10 ()
{

	News *news = new News ();
	news->SetHeadline ( "Digital Life research stolen" );
	news->SetDetails ( "Earlier today a hacker penetrated the Local Area Network of Darwin Research Associates "
                       "and stole Gigaquads of data on their latest project - Darwin Digital Life.  It is not "
                       "known exactly what the hackers wanted the data for, since its use is described as purely theoretical.\n\n"
                       "'The idea behind Darwin Digital Life was to create a life form that could live and reproduce on the Internet' "
                       "said the CEO of Darwin Research Associates recently.  'Obviously we are gutted that somebody has stolen what "
                       "amounts to years of research into this area.'\n\n"
                       "A suspect has been named to Federal Agents, but so far no arrests have been made.  Darwin Research Associates "
                       "earlier today stated that an Uplink Agent was responsible, and that the Agent in question was working for "
                       "Andromeda Research Corporation (ARC) at the time.\n\n"
                       "Once again, ARC was unwilling to comment on the issue, but this latest allegation has fueled speculation as to "
                       "the ultimate motives of the company." );

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (cu);
	cu->CreateNews ( news );

}

void PlotGenerator::Run_Act4Scene11 ()
{

	News *news = new News ();
	news->SetHeadline ( "Arunmor Corporation in Financial Fraud" );
	news->SetDetails ( "The lead Technical director of Arunmor Corporation has been arrested by Federal Agents, "
                       "following allegations that he has been caught attempting bank fraud.  This latest development "
                       "in the Arunmor Versus ARC story has called into question the true motives behind Arunmor Corporation.\n\n"
                       "Federal Agents arrested the technical director in a dawn raid, after an anonymous tip led them to discover "
                       "evidence which immediately incriminated him.\n\n"
                       "So far he has protested his innocence, claiming he has been framed.\n\n"
                       "This arrest will come as a severe blow to Arunmor Corporation, who relied on this man for most of their "
                       "technical leadership.  Their so called 'Faith Project' will have been set back.\n\n"
                       );

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (cu);
	cu->CreateNews ( news );

}

void PlotGenerator::Run_Act4Scene12 ()
{

	News *news = new News ();
	news->SetHeadline ( "Arunmor LAN system suffers crippling blow" );
	news->SetDetails ( "Arunmor Corporation today released a press statememt that said their primary computer system "
                       "had been severely damaged by the acts of a single hacker.\n\n"
                       "It appears that most of their research on that system has been destroyed, as well as the computer "
                       "system itself.  Arunmor are said to be devastated by this critical error of judgement.\n\n"
                       "Arunmor today claimed Andromeda Research Corporation was again responsible for this attack.\n\n"
                       "It is not known what effect this will have on project Faith, but it is known that the Arunmor Local "
                       "Area Network was used to store a great deal of research on that topic.\n\n" );

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (cu);
	cu->CreateNews ( news );

}

void PlotGenerator::Run_Act4Scene13 ()
{

    // Is the Tracer mission still available?

    bool removed = RemoveSpecialMission ( SPECIALMISSION_TRACER );

    if ( removed ) {

	    Date rundate;
	    rundate.SetDate ( &(game->GetWorld ()->date) );
	    rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 6 ) );

	    RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
	    rns->SetActAndScene ( 4, 4 );
	    rns->SetRunDate ( &rundate );
	    game->GetWorld ()->scheduler.ScheduleEvent ( rns );

        version_faith += 1.0;
        completed_tracer = true;

    }

}

void PlotGenerator::Run_Act4Scene14 ()
{

    // Is the TakeMeToYourLeader mission still available?

    bool removed = RemoveSpecialMission ( SPECIALMISSION_TAKEMETOYOURLEADER );

    if ( removed ) {

	    Date rundate;
	    rundate.SetDate ( &(game->GetWorld ()->date) );
	    rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 6 ) );

	    RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
	    rns->SetActAndScene ( 4, 5 );
	    rns->SetRunDate ( &rundate );
	    game->GetWorld ()->scheduler.ScheduleEvent ( rns );

        version_faith += 1.0;
        completed_takemetoyourleader = true;

    }

}

void PlotGenerator::Run_Act4Scene15 ()
{

    // Is the ARC Infiltration mission still available?

    bool removed = RemoveSpecialMission ( SPECIALMISSION_ARCINFILTRATION );

    if ( removed ) {

	    Date rundate;
	    rundate.SetDate ( &(game->GetWorld ()->date) );
	    rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 6 ) );

	    RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
	    rns->SetActAndScene ( 4, 6 );
	    rns->SetRunDate ( &rundate );
	    game->GetWorld ()->scheduler.ScheduleEvent ( rns );

        version_faith += 2.0;
        completed_arcinfiltration = true;

    }

}

void PlotGenerator::Run_Act5Scene1 ()
{

	News *news = new News ();
	news->SetHeadline ( "Revelation Launch due soon?" );
	news->SetDetails ( "Industry experts are predicting that a launch of the destructive Revelation virus "
                       "may be due soon. If true, this could mean damage to a great deal of computer systems.\n\n"
                       "Federal agents have been waiting for an opportunity to enter Andromeda Research Corporation "
                       "but have so far not had enough evidence against the company.\n\n"
                       "Recent events such as the arrest of a high ranked member of the company and the theft of "
                       "program code from Darwin Research Associates has led industry experts to speculate that now "
                       "would be the perfect time for a launch of the virus.\n\n"
                       "Arunmor corporation has been working hard and have a prototype of the counter virus Faith, "
                       "but it is not clear how effective this would be." );

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (cu);
	cu->CreateNews ( news );

	Date rundate;
	rundate.SetDate ( &(game->GetWorld ()->date) );
	rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 6 ) );

	RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
	rns->SetActAndScene ( 5, 2 );
	rns->SetRunDate ( &rundate );
	game->GetWorld ()->scheduler.ScheduleEvent ( rns );

}

void PlotGenerator::Run_Act5Scene2 ()
{

    //
    // Player has had long enough to do the special missions
    // Remove any still pending

    PlayerFailsSpecialMission ( SPECIALMISSION_TRACER );
    PlayerFailsSpecialMission ( SPECIALMISSION_TAKEMETOYOURLEADER );
    PlayerFailsSpecialMission ( SPECIALMISSION_ARCINFILTRATION );
    PlayerFailsSpecialMission ( SPECIALMISSION_DARWIN );
    PlayerFailsSpecialMission ( SPECIALMISSION_SAVEITFORTHEJURY );
    PlayerFailsSpecialMission ( SPECIALMISSION_SHINYHAMMER );


    if ( playerloyalty == -1 ) {

        //
        // Send the player a Tracker now

        Data *data = new Data ();
        data->SetTitle ( "Revelation_Tracker" );
        data->SetDetails ( DATATYPE_PROGRAM, 3, 0, 0, 1.0, SOFTWARETYPE_OTHER );

        Message *msg = new Message ();
        msg->SetTo ( "PLAYER" );
        msg->SetFrom ( "internal@ARC.net" );
        msg->SetSubject ( "Revelation Tracker" );
        msg->SetBody ( "This message includes our Revelation Tracker Software. This will tell you "
                       "how many systems have been infected with the Virus.\n"
                       "You will need this for your next mission." );
        msg->AttachData ( data );
        msg->Send ();

        //
        // Send the player Revelation

        Data *data2 = new Data ();
        data2->SetTitle ( "Revelation" );
        data2->SetDetails ( DATATYPE_PROGRAM, 3, 0, 0, version_revelation, SOFTWARETYPE_OTHER );

        Message *msg2 = new Message ();
        msg2->SetTo ( "PLAYER" );
        msg2->SetFrom ( "internal@ARC.net" );
        msg2->SetSubject ( "Revelation" );
        msg2->SetBody ( "This message includes a copy of the Revelation virus.\n"
                        "You will need this file for your next mission.\n\n"
                        "DO NOT ATTEMPT TO RUN IT LOCALLY" );
        msg2->AttachData ( data2 );
        msg2->Send ();

        //
        // The player is asked to do "Grand Tour"

        Mission *mission = GenerateMission_GrandTour ();
        if ( UplinkIncompatibleSaveGameAssert (mission, __FILE__, __LINE__) )
			return;

        game->GetWorld ()->GetPlayer ()->GiveMission ( mission );

		game->SetGameSpeed ( GAMESPEED_NORMAL );
		EclDirtyButton ( "hud_speed 0" );
        EclDirtyButton ( "hud_speed 1" );
		EclDirtyButton ( "hud_speed 2" );
		EclDirtyButton ( "hud_speed 3" );

        //
        // Start the AI going

        int timebetweenlaunches = (int) ( 3 - (version_faith - version_revelation) );

        Date rundate;
        rundate.SetDate ( &(game->GetWorld ()->date) );
        rundate.AdvanceMinute ( timebetweenlaunches );

        RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
        rns->SetActAndScene ( 5, 5 );
        rns->SetRunDate ( &rundate );
        game->GetWorld ()->scheduler.ScheduleEvent ( rns );

        //
        // Schedule the downfall of ARC for a few minutes

        Date rundate2;
        rundate2.SetDate ( &game->GetWorld ()->date );
        rundate2.AdvanceMinute ( TIME_ARCBUSTED_WITHPLAYER );

        RunPlotSceneEvent *rns2 = new RunPlotSceneEvent ();
        rns2->SetActAndScene ( 5, 6 );
        rns2->SetRunDate ( &rundate2 );
        game->GetWorld ()->scheduler.ScheduleEvent ( rns2 );


    }
    else if ( playerloyalty == 1 ) {

        //
        // The player is asked to do "Counter attack"

        Mission *mission = GenerateMission_CounterAttack ();
        if ( UplinkIncompatibleSaveGameAssert (mission, __FILE__, __LINE__) )
			return;

        game->GetWorld ()->GetPlayer ()->GiveMission ( mission );


		game->SetGameSpeed ( GAMESPEED_NORMAL );
		EclDirtyButton ( "hud_speed 0" );
        EclDirtyButton ( "hud_speed 1" );
		EclDirtyButton ( "hud_speed 2" );
		EclDirtyButton ( "hud_speed 3" );

        //
        // Start the AI going

        for ( int i = 0; i < 3; ++i ) {

            Computer *comp = WorldGenerator::GetRandomComputer ( COMPUTER_TYPE_INTERNALSERVICESMACHINE |
                                                                 COMPUTER_TYPE_CENTRALMAINFRAME        );
            UplinkAssert (comp);
            RunRevelation ( comp->ip, version_revelation, false, 2 );

        }

        int timebetweenlaunches = (int) ( 3 - (version_revelation - version_faith) );

        Date rundate;
        rundate.SetDate ( &(game->GetWorld ()->date) );
        rundate.AdvanceMinute ( timebetweenlaunches );

        RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
        rns->SetActAndScene ( 5, 5 );
        rns->SetRunDate ( &rundate );
        game->GetWorld ()->scheduler.ScheduleEvent ( rns );

        //
        // Schedule the downfall of ARC for a few minutes

        Date rundate2;
        rundate2.SetDate ( &game->GetWorld ()->date );
        rundate2.AdvanceMinute ( TIME_ARCBUSTED_WITHOUTPLAYER );

        RunPlotSceneEvent *rns2 = new RunPlotSceneEvent ();
        rns2->SetActAndScene ( 5, 6 );
        rns2->SetRunDate ( &rundate2 );
        game->GetWorld ()->scheduler.ScheduleEvent ( rns2 );


    }
    else {

        // Grand Tour and Counter attack are done by other people

        Date rundate;
        rundate.SetDate ( &(game->GetWorld ()->date) );
        rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 2 ) );

        RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
        rns->SetActAndScene ( 5, 4 );
        rns->SetRunDate ( &rundate );
        game->GetWorld ()->scheduler.ScheduleEvent ( rns );

    }

}

void PlotGenerator::Run_Act5Scene3 ()
{

	News *news = new News ();
	news->SetHeadline ( "Revelation Virus Release stopped in its path" );
	news->SetDetails ( "Within the last couple of hours the Revelation virus was released onto the Internet "
                       "by operatives at the controversial Andromeda Research Corporation. Experts within the "
                       "industry had predicted this was about to happen, but were unprepared for the scale of the attack.\n\n"
                       "However, Arunmor Corporation have been developing a counter virus known as Faith for the past few "
                       "weeks with the aid of Government assistance, and have managed to stop the Revelation Virus from "
                       "spreading to too many systems.\n\n"
                       "Federal Agents busted into Andromeda Research Corporation shortly after the virus outbreak and "
                       "arrested staff members suspected of involvement.  A number of computer systems were also siezed. "
                       "This co-incided with a world wide effort to take down this rogue group within Andromeda Research "
                       "Corporation. A number of Uplink agents were also arrested in connection with the outbreak.\n\n"
                       "It is not known what will become of Andromeda, but it is clear that many of the company executives "
                       "have come to the end of their career at that company.\n\n"
                       "Federal Agents refused to comment at this time, but did say a statement would be released soon." );

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (cu);
	cu->CreateNews ( news );


    Date rundate;
    rundate.SetDate ( &(game->GetWorld ()->date) );
    rundate.AdvanceDay ( 2 );
    rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 12 ) );

    RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
    rns->SetActAndScene ( 6, 1 );
    rns->SetRunDate ( &rundate );
    game->GetWorld ()->scheduler.ScheduleEvent ( rns );

}

void PlotGenerator::Run_Act5Scene4 ()
{

	News *news = new News ();
	news->SetHeadline ( "Revelation virus outbreak leaves systems crippled" );
	news->SetDetails ( "The Revelation computer virus has been released into the wild and has spread rapidly through "
                       "major computer systems around the world.  A massive amount of damage has been done, and reports "
                       "are still coming in from systems that have been literally wiped clean by the virus.\n\n"
                       "However, Arunmor Corporation have been developing a counter virus known as Faith for the past few "
                       "weeks with the aid of Government assistance, and have managed to stop the Revelation Virus from "
                       "spreading any further.\n\n"
                       "Federal Agents busted into Andromeda Research Corporation shortly after the virus outbreak and "
                       "arrested staff members suspected of involvement.  A number of computer systems were also siezed. "
                       "This co-incided with a world wide effort to take down this rogue group within Andromeda Research "
                       "Corporation. A number of Uplink agents were also arrested in connection with the outbreak.\n\n"
                       "It is not known what will become of Andromeda, but it is clear that many of the company executives "
                       "have come to the end of their career at that company.\n\n"
                       "Federal Agents refused to comment at this time, but did say a statement would be released soon." );

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (cu);
	cu->CreateNews ( news );


    Date rundate;
    rundate.SetDate ( &(game->GetWorld ()->date) );
    rundate.AdvanceDay ( 2 );
    rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 12 ) );

    RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
    rns->SetActAndScene ( 6, 1 );
    rns->SetRunDate ( &rundate );
    game->GetWorld ()->scheduler.ScheduleEvent ( rns );

}

void PlotGenerator::Run_Act5Scene5 ()
{

    if ( revelation_releasefailed == false ) {

        if ( playerloyalty == 1 ) {

            // Release Revelation at a random spot

            Computer *comp = WorldGenerator::GetRandomComputer ( COMPUTER_TYPE_INTERNALSERVICESMACHINE |
                                                                 COMPUTER_TYPE_CENTRALMAINFRAME        );
            UplinkAssert (comp);

            RunRevelation ( comp->ip, version_revelation, false, 2 );

            int timebetweenlaunches = (int) ( 3 - (version_revelation - version_faith) );
			if ( timebetweenlaunches < 1 )
				timebetweenlaunches = 1;

            Date rundate;
            rundate.SetDate ( &(game->GetWorld ()->date) );
            rundate.AdvanceMinute ( timebetweenlaunches );

            RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
            rns->SetActAndScene ( 5, 5 );
            rns->SetRunDate ( &rundate );
            game->GetWorld ()->scheduler.ScheduleEvent ( rns );

        }
        else if ( playerloyalty == -1 ) {

            // Run Faith on an infected system

            if ( infected.Size () > 0 ) {

                int pos = NumberGenerator::RandomNumber ( infected.Size () );
                char *ip = infected.GetData (pos);
                UplinkAssert (ip);

                RunFaith ( ip, version_faith, false );

            }

            int timebetweenlaunches = (int) ( 3 - (version_faith - version_revelation) );
			if ( timebetweenlaunches < 1 )
				timebetweenlaunches = 1;

            Date rundate;
            rundate.SetDate ( &(game->GetWorld ()->date) );
            rundate.AdvanceMinute ( timebetweenlaunches );

            RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
            rns->SetActAndScene ( 5, 5 );
            rns->SetRunDate ( &rundate );
            game->GetWorld ()->scheduler.ScheduleEvent ( rns );

        }

    }
    else {

        revelation_releasefailed = false;

    }

}

void PlotGenerator::Run_Act5Scene6 ()
{

    //
    // If it gets this far, Revelation has failed
    // The player has either won or lost

    revelation_releasefailed = true;
    revelation_arcbusted = true;

    if ( playerloyalty == -1 ) {

        game->GameOver ( "For working for Andromeda Research Corporation\n"
                         "during the Revelation crisis" );

    }
    else if ( playerloyalty == 1 ) {

        // Send him an email about ARC getting busted

         Message *msg = new Message ();
         msg->SetTo ( "PLAYER" );
         msg->SetFrom ( "internal@Arunmor.net" );
         msg->SetSubject ( "Arunmor has been busted" );
         msg->SetBody ( "We know you're busy, so we'll make this brief.\n\n"
                        "We've just been informed that Federal Agents have broken into "
                        "ARCs headquarters and have arrested the staff and seized the "
                        "equipment there.\n\n"
                        "This means that no further outbreaks of Revelation will occur.\n\n"
                        "If you can stop the current outbreak from spreading, we will have "
                        "beaten them. This is our best chance to put an end to the "
                        "Revelation threat once and for all.\n\n"
                        "Do your best and hurry.\n\n"
                        "END" );
         msg->Send ();

    }

}

void PlotGenerator::Run_Act5Scene7 ()
{

    revelation_releaseuncontrolled = true;

    PlayerCompletesSpecialMission ( SPECIALMISSION_GRANDTOUR );

	game->WinCode ( "Completed the storyline with ARC", "ARC" );

    game->GameOver ( "An outbreak of the Revelation virus forced\n"
                     "Uplink Corporation to cease operations" );

}

void PlotGenerator::Run_Act6Scene1 ()
{

	News *news = new News ();
	news->SetHeadline ( "First press release from Andromeda Leader" );
	news->SetDetails ( "The Leader of the Andromeda group today issued a statement through his lawyer, regarding his recent arrest "
                       "for criminal damage to the Web. In it he outlines why he tried to destroy the Internet that we all rely on.\n\n"
                       "'Firstly, I make no apologies for what I have done. My only regret is that I did not succeed' says the man, who "
                       "has not yet been identified by Federal Agents.\n"
                       "'The Internet used to be a place of amazing opportunity and promise - the last remnant of free speech and "
                       "true anonymity.  Now it is little more than an extension of the evil of Western Capitalism.'\n\n"
                       "'Think of this.  Do you have any idea how much money Arunmor will make from recent events?  This is why I tried "
                       "to destroy the Internet.  It only serves to line the pockets of those who would try to control us.'\n\n"
                       "'While the Web exists, you will never be safe. Your entire life is on file, waiting for somebody to tamper with it.  Your lives "
                       "are being destroyed by the Computers you desperately try to defend.'\n\n"
                       "It is believed this man will spend several years in jail for this crime." );

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (cu);
	cu->CreateNews ( news );


    Date rundate;
    rundate.SetDate ( &(game->GetWorld ()->date) );
    rundate.AdvanceDay ( 7 );
    rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 12 ) );

    RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
    rns->SetActAndScene ( 6, 2 );
    rns->SetRunDate ( &rundate );
    game->GetWorld ()->scheduler.ScheduleEvent ( rns );

}

void PlotGenerator::Run_Act6Scene2 ()
{

	News *news = new News ();
	news->SetHeadline ( "Leader of Andromeda jailed" );
	news->SetDetails ( "The Federal courts today found the CEO of ARC and the leader of Andromeda Guilty of criminal destruction of data. \n"
                       "The judge called the Leader of Andromeda 'an evil and vindictive man, who truly believes the world would be a better place in a state of Anarchy.'\n\n"
                       "He went on to say that he had little sympathy for both the defendants.\n\n"
                       "As punishment for their crimes, both members will serve eight years imprisonment and will be banned from the use of computers for life.\n\n" );

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (cu);
	cu->CreateNews ( news );


    Date rundate;
    rundate.SetDate ( &(game->GetWorld ()->date) );
    rundate.AdvanceDay ( 4 );
    rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 12 ) );

    RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
    rns->SetActAndScene ( 6, 3 );
    rns->SetRunDate ( &rundate );
    game->GetWorld ()->scheduler.ScheduleEvent ( rns );

}

void PlotGenerator::Run_Act6Scene3 ()
{

	News *news = new News ();
	news->SetHeadline ( "Revelation : A retrospective" );
	news->SetDetails ( "It would appear that the recent panic over Revelation was (as usual) totally unnecessary.  The Internet has suffered a number of "
                       "high profile scares since its foundation, and it has always survived.  This incident was no different, and only serves to highlight the "
                       "ease with which the public can be scared into thinking their lives are in danger.\n\n"
                       "A special help line was recently set up by Federal Agents for worried members of the public, and they have so far received an average of "
                       "ten-thousand calls a day from people who are terrified their life records are now wide open due to this recent security scare.  'They just "
                       "keep calling' said a Federal Spokesman today.  'These people honestly believe that somebody could just open up their records and change "
                       "them as if they were in the public library.  Its total hysteria.'\n\n"
                       "Since the collapse of the Andromeda Group, ARC has been exposed as little more than a front for their activities, and has shut down all operations. "
                       "It is a wonder that nobody noticed ARC never released any products.\n\n"
                       "Arunmor Corporation yesterday launched their new Active-Virus-Guard software, titled 'Faith'.  This is an extension of the tool used to defeat "
                       "Revelation, and the makers claim it actively seeks out viruses and eradicates them.  Sales have apparently been 'very promising'.\n\n"
                       "At the time of writing, shares in Arunmor Corporation had risen to a new high." );

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (cu);
	cu->CreateNews ( news );


    // This is the end of the plot
    // A tear is forming in my eye ;|

}

Mission *PlotGenerator::GenerateMission_Tracer ()
{

    //
    // Generate our own fileserver

    Computer *ourcomp = WorldGenerator::GenerateEmptyFileServer ( "Arunmor" );
	if ( UplinkIncompatibleSaveGameAssert (ourcomp, __FILE__, __LINE__) )
		return NULL;

    // Put the tracer software on our own file server
    //

    Data *data = new Data ();
    data->SetTitle ( "RevelationTracer" );
    data->SetDetails ( DATATYPE_PROGRAM, 3 );

    ourcomp->databank.PutData ( data );

	//
	// Add in a new account for the player to use
	//

	char *password = "tracer";
	char username [12];
	UplinkSnprintf ( username, sizeof ( username ), "temp%c%c%c%c", 'a' + NumberGenerator::RandomNumber ( 26 ),
																	'a' + NumberGenerator::RandomNumber ( 26 ),
																	'a' + NumberGenerator::RandomNumber ( 26 ),
																	'a' + NumberGenerator::RandomNumber ( 26 ) );

	Record *record = new Record ();
	record->AddField ( RECORDBANK_NAME, username );
	record->AddField ( RECORDBANK_PASSWORD, password );
	record->AddField ( RECORDBANK_SECURITY, "3" );
	ourcomp->recordbank.AddRecord ( record );

	char code [128];
    Computer::GenerateAccessCode( username, password, code, sizeof ( code ) );


    // Find the target computer

    Computer *comp = game->GetWorld ()->GetComputer ( NameGenerator::GenerateInternalServicesServerName("ARC") );
    if ( UplinkIncompatibleSaveGameAssert (comp, __FILE__, __LINE__) )
		return NULL;

    std::ostrstream fulldetails;
    fulldetails << "One of our sources within Andromeda Research Corporation has discovered that ARC "
                   "are planning on launching Revelation from three primary systems. When the time is right, "
                   "their Agents will enter these systems and begin running Revelation.\n\n"
                   "We already have two of the systems covered, but the third is wide open.  We need you to "
                   "get inside that system, and install a Tracer program covertly which will send us routine "
                   "updates, and will give us early warning when Revelation is launched.\n\n"
                   "First, you'll need to download the program from our File Server.  Addresses and log-in details "
                   "are shown below.\n\n"
                   "Second, break into the target system (ARC Internal Services) and compromise all their security "
                   "systems.  This includes Proxies, Monitors, Firewalls etc.  You have to stop them from seeing what "
                   "you are doing.\n\n"
                   "Copy the Tracer into place, then drop into a console.  You can run the tracer from there.  Once it is "
                   "running it will immediately start hiding itself. Check you can't see it from the File Server screen - this "
                   "will ensure nobody else can see it.\n\n"
                   "Cover your tracks and get out.  Reply to this email when you are done.\n\n"
				   "Codename: Tracer\n\n"
                   "Payment will be " << PAYMENT_SPECIALMISSION_TRACER << "\n\n"
                << "Our FileServer: " << ourcomp->ip << "\n"
                << "Target system: " << comp->name << "\n"
                << "Target IP: " << comp->ip
                << '\x0';

    std::ostrstream details;
	details << "Payment for this job is " << PAYMENT_SPECIALMISSION_TRACER << " credits.\n"
			<< "This job has been assigned an Uplink difficulty of " << 10 << ".\n"
			<< "This mission cannot be auto-accepted."
			<< '\x0';


	Mission *m = new Mission ();
	m->SetTYPE ( MISSION_SPECIAL );
	m->SetEmployer ( "Arunmor" );
	m->SetContact ( "internal@Arunmor.net" );
	m->SetPayment ( PAYMENT_SPECIALMISSION_TRACER, (int) ( PAYMENT_SPECIALMISSION_TRACER * 1.1 ) );
	m->SetDifficulty ( 10 );
    m->SetMinRating ( 10 );
    m->SetAcceptRating ( 99 );
	m->SetDescription ( SpecialMissionDescription (SPECIALMISSION_TRACER) );
    m->SetDetails ( details.str () );
	m->SetFullDetails ( fulldetails.str () );
    m->SetWhySoMuchMoney ( "The mission is extremely important." );
    m->SetHowSecure ( "The target will be very secure." );
    m->SetWhoIsTheTarget ( "Andromeda Research Corporation." );
    m->GiveLink ( comp->ip );
    m->GiveLink ( ourcomp->ip );
    m->GiveCode ( ourcomp->ip, code );

	details.rdbuf()->freeze( 0 );
	fulldetails.rdbuf()->freeze( 0 );
	//delete [] details.str ();
    //delete [] fulldetails.str ();

	return m;

}

Mission *PlotGenerator::GenerateMission_TakeMeToYourLeader ()
{

    //
    // Look up the target systen

    Computer *target = game->GetWorld ()->GetComputer ( NameGenerator::GenerateInternalServicesServerName("Arunmor") );
    if ( UplinkIncompatibleSaveGameAssert (target, __FILE__, __LINE__) )
		return NULL;


    //
    // Put Revelation on to our file server

    Computer *ourcomp = game->GetWorld ()->GetComputer ( NameGenerator::GenerateFileServerName("Arunmor") );
	if ( UplinkIncompatibleSaveGameAssert (ourcomp, __FILE__, __LINE__) )
		return NULL;

    Data *data = new Data ();
    data->SetTitle ( "Revelation" );
    data->SetDetails ( DATATYPE_PROGRAM, 3, 0, 0, 1.0 );

    ourcomp->databank.PutData ( data );

	//
	// Add in a new account for the player to use
	//

	char *password = "takemetoyourleader";
	char username [12];
	UplinkSnprintf ( username, sizeof ( username ), "temp%c%c%c%c", 'a' + NumberGenerator::RandomNumber ( 26 ),
										'a' + NumberGenerator::RandomNumber ( 26 ),
										'a' + NumberGenerator::RandomNumber ( 26 ),
										'a' + NumberGenerator::RandomNumber ( 26 ) );

	Record *record = new Record ();
	record->AddField ( RECORDBANK_NAME, username );
	record->AddField ( RECORDBANK_PASSWORD, password );
	record->AddField ( RECORDBANK_SECURITY, "3" );
	ourcomp->recordbank.AddRecord ( record );

	char code [128];
    Computer::GenerateAccessCode( username, password, code, sizeof ( code ) );


    //
    // Generate the mission
    //

    std::ostrstream fulldetails;
    fulldetails << "We have recently been co-operating with Federal Agents in an attempt to bring the CEO "
                   "of Andromeda Research Corporation into custody for questioning.  Unfortunately, for various "
                   "legal and political reasons, the Feds are refusing to arrest the man due to a lack of evidence "
                   "against him.  We are going to provide that evidence.\n\n"
                   "Unfortunately for ARC, everyone now knows they are developing the Revelation virus.  Equally "
                   "unfortunately, we have a copy of an early version of Revelation, thanks to an earlier mission.\n\n"
                   "Your first task is to connect to our File Server and retrieve Revelation.\n\n"
                   "You must then break into OUR Internal Services System (shown below), disable the security and run "
                   "Revelation on that system.  Finally, cover your tracks to make sure the source can't be traced.\n\n"
                   "This attack will give the Federal Agents a good enough reason to arrest the CEO of ARC, and we can "
                   "start questioning him.\n\n"
                   "It is important to note that nobody else knows of this attack - even the system administrators at our "
                   "Internal Services System.  They will still try to trace you, and if you are caught we will not be "
                   "coming to rescue you.\n\n"
				   "Codename: TakeMeToYourLeader\n\n"
                << "Our FileServer: " << ourcomp->ip << "\n"
                << "Username: " << username << "\n"
                << "Password: " << password << "\n"
                << "Filename: Revelation\n"
                << "Target system: " << target->name << "\n"
                << "Target IP: " << target->ip
                << '\x0';

    std::ostrstream details;
	details << "Payment for this job is " << PAYMENT_SPECIALMISSION_TAKEMETOYOURLEADER << " credits.\n"
			<< "This job has been assigned an Uplink difficulty of " << 10 << ".\n"
			<< "This mission cannot be auto-accepted."
			<< '\x0';

	Mission *m = new Mission ();
	m->SetTYPE ( MISSION_SPECIAL );
	m->SetEmployer ( "Arunmor" );
	m->SetContact ( "internal@Arunmor.net" );
	m->SetPayment ( PAYMENT_SPECIALMISSION_TAKEMETOYOURLEADER, (int) ( PAYMENT_SPECIALMISSION_TAKEMETOYOURLEADER * 1.1 ) );
	m->SetDifficulty ( 10 );
    m->SetMinRating ( 10 );
    m->SetAcceptRating ( 99 );
	m->SetDescription ( SpecialMissionDescription (SPECIALMISSION_TAKEMETOYOURLEADER) );
    m->SetDetails ( details.str () );
	m->SetFullDetails ( fulldetails.str () );
    m->SetWhySoMuchMoney ( "It involves very important people." );
    m->SetHowSecure ( "It won't be a problem." );
    m->SetWhoIsTheTarget ( "The CEO of Andromeda Research Corporation." );
    m->GiveLink ( target->ip );
    m->GiveLink ( ourcomp->ip );
    m->GiveCode ( ourcomp->ip, code );

	details.rdbuf()->freeze( 0 );
	fulldetails.rdbuf()->freeze( 0 );
	//delete [] details.str ();
    //delete [] fulldetails.str ();

	return m;


}

Mission *PlotGenerator::GenerateMission_ARCInfiltration ()
{

    // Find the target computer

    Computer *target = game->GetWorld ()->GetComputer ( NameGenerator::GenerateLANName("ARC") );
    if ( UplinkIncompatibleSaveGameAssert (target, __FILE__, __LINE__) )
		return NULL;

	//
	// Get a new computer to dump the files to
	//

    Computer *ourcomp = game->GetWorld ()->GetComputer ( NameGenerator::GenerateFileServerName("Arunmor") );
	if ( UplinkIncompatibleSaveGameAssert (ourcomp, __FILE__, __LINE__) )
		return NULL;

	//
	// Add in a new account for the player to use
	//

	char *password = "infiltrate";
	char username [12];
	UplinkSnprintf ( username, sizeof ( username ), "temp%c%c%c%c", 'a' + NumberGenerator::RandomNumber ( 26 ),
																	'a' + NumberGenerator::RandomNumber ( 26 ),
																	'a' + NumberGenerator::RandomNumber ( 26 ),
																	'a' + NumberGenerator::RandomNumber ( 26 ) );

	Record *record = new Record ();
	record->AddField ( RECORDBANK_NAME, username );
	record->AddField ( RECORDBANK_PASSWORD, password );
	record->AddField ( RECORDBANK_SECURITY, "3" );
	ourcomp->recordbank.AddRecord ( record );

	char code [128];
    Computer::GenerateAccessCode( username, password, code, sizeof ( code ) );

	//
	// Generate around 10 files to be stolen,
	// All fairly large
	//

	int numfiles = 9;
	int encrypted = 5;
    int size = 10;

	for ( int i = 0; i < numfiles; ++i ) {

		char datatitle [64];
		UplinkSnprintf ( datatitle, sizeof ( datatitle ), "ARC-REVELATION %d.dat", i );

		Data *file = new Data ();
		file->SetTitle ( datatitle );
		file->SetDetails ( DATATYPE_DATA, size, encrypted, 0 );

		while ( target->databank.FindValidPlacement (file) == -1 )
			target->databank.SetSize ( target->databank.GetSize () + file->size );

		target->databank.PutData ( file );

	}

    //
	// Generate the fields of the mission
    //

	char description [SIZE_MISSION_DESCRIPTION];
	std::ostrstream fulldetails;

	UplinkStrncpy ( description, SpecialMissionDescription (SPECIALMISSION_ARCINFILTRATION), sizeof ( description ) );

	fulldetails << "As you may be aware, ARC are storing most of their Research on their Local Area Network system. "
                   "However, what is not commonly known is that the critical data (ie the research files themselves) "
                   "are actually stored on a Stand-Alone file server, which is physically disconnected from the rest "
                   "of the Internet.  When the file server is connected for use, the ARC LAN itself is "
                   "disconnected, meaning their data is always totally isolated from public access.  This has made it "
                   "impossible for us to steal their research until now.\n\n"
                   "A member of ARCs team who wishes to remain anonymous has come forward and offered to help us. You "
                   "will need to break in to the system first.  Once inside, you should be able to find the restricted "
                   "file area.  Our man on the inside will then install a remote radio modem on the file server, which "
                   "will give you access.  You can then copy the files.\n\n"
                   "Once done, decrypt the files and dump them onto our File Server - details below.\n\n"
                   "You may also have noticed that the CEO of ARC was recently arrested and questioned by Federal Agents "
                   "in connection with Revelation. We believe ARC are planning to launch Revelation early due to this.  "
                   "This makes the success of this mission Super critical.\n\n"
                   "You will be paid " << PAYMENT_SPECIALMISSION_ARCINFILTRATION << "c for this mission.\n\n"
				   "Codename: ARCInfiltration\n\n";

	fulldetails << "Target computer : " << target->name << "\n"
				<< "IP : " << target->ip << "\n\n"
				<< "Once complete, dump the files you have to this computer:\n"
				<< ourcomp->name << "\n"
				<< "IP : " << ourcomp->ip << "\n\n"
				<< "USERNAME : " << username << "\n"
				<< "PASSWORD : " << password << "\n\n"
                << "Reply to this mail when you have finished.\n\n"
				<< "END"
				<< '\x0';

    std::ostrstream details;
	details << "Payment for this job is " << PAYMENT_SPECIALMISSION_ARCINFILTRATION << " credits.\n"
			<< "This job has been assigned an Uplink difficulty of " << 10 << ".\n"
			<< "This mission cannot be auto-accepted."
			<< '\x0';

	// Insert the mission
	Mission *mission = new Mission ();
	mission->SetTYPE		 ( MISSION_SPECIAL );
	mission->SetCompletion   ( NULL, NULL, NULL, NULL, NULL );
	mission->SetEmployer     ( "Arunmor" );
	mission->SetContact      ( "internal@Arunmor.net" );
	mission->SetDifficulty   ( 10 );
    mission->SetMinRating    ( 10 );
    mission->SetAcceptRating ( 99 );
    mission->SetPayment      ( PAYMENT_SPECIALMISSION_ARCINFILTRATION, (int) ( PAYMENT_SPECIALMISSION_ARCINFILTRATION * 1.1 ) );
	mission->SetDescription  ( description );
    mission->SetDetails      ( details.str () );
	mission->SetFullDetails  ( fulldetails.str () );
    mission->SetWhySoMuchMoney ( "This mission will be extremely difficult" );
    mission->SetWhoIsTheTarget ( "Andromeda Research Corporation" );
    mission->SetHowSecure ( "The system will be extremely secure." );
	mission->GiveLink ( target->ip );
	mission->GiveLink ( ourcomp->ip );
	mission->GiveCode ( ourcomp->ip, code );

	details.rdbuf()->freeze( 0 );
	fulldetails.rdbuf()->freeze( 0 );
    //delete [] details.str ();
    //delete [] fulldetails.str ();

    return mission;


}

Mission *PlotGenerator::GenerateMission_CounterAttack ()
{

    Computer *ourcomp = game->GetWorld ()->GetComputer ( NameGenerator::GenerateFileServerName("Arunmor") );
	if ( UplinkIncompatibleSaveGameAssert (ourcomp, __FILE__, __LINE__) )
		return NULL;

    //
    // Clear file server

    ourcomp->databank.Format ();


    //
    // Put Tracker on file server

    Data *data = new Data ();
    data->SetTitle ( "Revelation_Tracker" );
    data->SetDetails ( DATATYPE_PROGRAM, 3, 0, 0, 1.0, SOFTWARETYPE_OTHER );

    ourcomp->databank.PutData ( data );

    //
    // Put Faith on file server

    Data *data2 = new Data ();
    data2->SetTitle ( "Faith" );
    data2->SetDetails ( DATATYPE_PROGRAM, 3, 0, 0, version_faith, SOFTWARETYPE_OTHER );

    ourcomp->databank.PutData ( data2 );

	//
	// Add in a new account for the player to use
	//

	char *password = "faithinchaos";
	char username [12];
	UplinkSnprintf ( username, sizeof ( username ), "temp%c%c%c%c", 'a' + NumberGenerator::RandomNumber ( 26 ),
																	'a' + NumberGenerator::RandomNumber ( 26 ),
																	'a' + NumberGenerator::RandomNumber ( 26 ),
																	'a' + NumberGenerator::RandomNumber ( 26 ) );

	Record *record = new Record ();
	record->AddField ( RECORDBANK_NAME, username );
	record->AddField ( RECORDBANK_PASSWORD, password );
	record->AddField ( RECORDBANK_SECURITY, "3" );
	ourcomp->recordbank.AddRecord ( record );

	char code [128];
    Computer::GenerateAccessCode( username, password, code, sizeof ( code ) );


    std::ostrstream details;
    details << "Our tracer programs have just informed us that Revelation has been released "
               "into the wild by Andromeda Research Corporation.  Only a small number of systems "
               "are currently infected but more will certainly follow.  This is what we have been planning for.\n\n"
               "Revelation will infect a system, wait for a few minutes, then spread to two other systems. The origional "
               "infected system will then be shut down. If two many systems get infected, we won't be able to stop it.\n\n"
               "Connect to our File Server and download the Revelation Tracker program. This will keep you updated when "
               "systems become infected with Revelation, so you can connect and disinfect them.\n\n"
               "You will also need to download Faith - our countervirus.  This is run by copying it into the databanks of "
               "the infected systems, dropping into a command prompt, and running it.\n\n"
               "There isn't much time.  You have only minutes until Revelation spreads beyond control. Get moving.\n\n"
			   "Codename: CounterAttack\n\n"
            << "Our FileServer: " << ourcomp->ip << "\n"
            << "Username: " << username << "\n"
            << "Password: " << password << "\n"
            << '\x0';

    //
    // Generate the mission

    Mission *m = new Mission ();
    m->SetTYPE ( MISSION_SPECIAL );
    m->SetEmployer ( "Arunmor" );
    m->SetContact ( "internal@Arunmor.net" );
    m->SetPayment ( PAYMENT_SPECIALMISSION_COUNTERATTACK );
    m->SetDifficulty ( 20 );
    m->SetDescription ( SpecialMissionDescription (SPECIALMISSION_COUNTERATTACK) );
    m->SetFullDetails ( details.str () );
    m->GiveLink ( ourcomp->ip );
    m->GiveCode ( ourcomp->ip, code );

    return m;

}

Mission *PlotGenerator::GenerateMission_MaidenFlight ()
{

	// Find the target computer

    Computer *comp = game->GetWorld ()->GetComputer ( NameGenerator::GenerateInternalServicesServerName("Arunmor") );
	if ( UplinkIncompatibleSaveGameAssert (comp, __FILE__, __LINE__) )
		return NULL;

	std::ostrstream fulldetails;
	fulldetails << "Our corporation has been developing a software product known as Revelation for some time now, "
				   "and Version 1.0 is finally ready.  In order to test and refine this program, we need to test it "
				   "in the real world.  Your mission will be to install and run Revelation for us.\n\n"
				   "The target system is the Arunmor Internal Services System.  You will need to first locate this computer. "
                   "No doubt Arunmor will "
				   "not be too pleased if they catch you on their system.\n\n"
				   "You will find that we have sent you Version 1.0 of Revelation in an email.  Once you are inside "
				   "The Arunmor system, copy the program into their memory banks and disrupt all of their security "
				   "systems.  The more security you disable, the better this product will operate.  In order to activate "
				   "the program you will need to enter a command prompt and run it from there.\n\n"
				   "DO NOT ATTEMPT TO RUN REVELATION ON YOUR COMPUTER.\n\n"
				   "Once complete, contact us at internal@ARC.net and we'll arrange your debrief and payment.\n"
				   "You will be paid a further "
                << PAYMENT_SPECIALMISSION_MAIDENFLIGHT
                << "c for successful completion of this mission.\n\n"
				   "Codename: MaidenFlight\n\n"
				   "[END]"
				<< '\x0';


	Mission *m = new Mission ();
	m->SetTYPE ( MISSION_SPECIAL );
	m->SetEmployer ( "ARC" );
	m->SetContact ( "internal@ARC.net" );
	m->SetPayment ( PAYMENT_SPECIALMISSION_MAIDENFLIGHT, PAYMENT_SPECIALMISSION_MAIDENFLIGHT );
	m->SetDifficulty ( 6 );

	m->SetDescription ( SpecialMissionDescription (SPECIALMISSION_MAIDENFLIGHT)  );
	m->SetFullDetails ( fulldetails.str () );

	fulldetails.rdbuf()->freeze( 0 );
	//delete [] fulldetails.str ();

	return m;

}

Mission *PlotGenerator::GenerateMission_Darwin ()
{

//    Computer *target = game->GetWorld ()->GetComputer ( "Darwin Research Associates Central Mainframe" );
    Computer *target = game->GetWorld ()->GetComputer ( NameGenerator::GenerateLANName("Darwin Research Associates") );
    if ( UplinkIncompatibleSaveGameAssert (target, __FILE__, __LINE__) )
		return NULL;

	//
	// Generate a new computer to dump the files to
	//

	Computer *ourcomp = WorldGenerator::GenerateEmptyFileServer ( "ARC" );
	if ( UplinkIncompatibleSaveGameAssert (ourcomp, __FILE__, __LINE__) )
		return NULL;

	//
	// Add in a new account for the player to use
	//

	char *password = "darwin";
	char username [12];
	UplinkSnprintf ( username, sizeof ( username ), "temp%c%c%c%c", 'a' + NumberGenerator::RandomNumber ( 26 ),
																	'a' + NumberGenerator::RandomNumber ( 26 ),
																	'a' + NumberGenerator::RandomNumber ( 26 ),
																	'a' + NumberGenerator::RandomNumber ( 26 ) );

	Record *record = new Record ();
	record->AddField ( RECORDBANK_NAME, username );
	record->AddField ( RECORDBANK_PASSWORD, password );
	record->AddField ( RECORDBANK_SECURITY, "3" );
	ourcomp->recordbank.AddRecord ( record );

	char code [128];
    Computer::GenerateAccessCode( username, password, code, sizeof ( code ) );

	//
	// Generate around 10 files to be stolen,
	// All fairly large
	//

	int numfiles = 9;
	int encrypted = 5;
    int size = 10;

	for ( int i = 0; i < numfiles; ++i ) {

		char datatitle [64];
		UplinkSnprintf ( datatitle, sizeof ( datatitle ), "%c%c%c%c-DARWIN %d.dat", target->companyname [0], target->companyname [1],
												       target->companyname [2], target->companyname [3], i );

		Data *file = new Data ();
		file->SetTitle ( datatitle );
		file->SetDetails ( DATATYPE_DATA, size, encrypted, 0 );

		while ( target->databank.FindValidPlacement (file) == -1 )
			target->databank.SetSize ( target->databank.GetSize () + file->size );

		target->databank.PutData ( file );

	}

    //
	// Generate the fields of the mission
    //

	char description [SIZE_MISSION_DESCRIPTION];
	std::ostrstream fulldetails;

	UplinkStrncpy ( description, SpecialMissionDescription (SPECIALMISSION_DARWIN), sizeof ( description ) );

	fulldetails << "The results of our recent test have been extremely encouraging.  Revelation is easily "
                   "destructive enough to take out most of the Internet when it is finally released.  Unfortunately, "
                   "it is not particularly virulant.  In other words, it doesn't spread very fast.  We need to fix this.\n\n"
                   "We've looked around and we have found a company called Darwin Research Associates.  They are currently "
                   "working on research into digital life forms that exist and breed on the Internet.  Since they have refused "
                   "to sell any of their technology to us, we have decided instead to steal it.\n\n"
                   "Break into their Local Area Network system shown below, and steal any files relating to the Darwin project.  "
                   "Try to copy as much data as possible.  You will need to decrypt the data before we can make any use of it.  "
                   "We have determined that over 80Gqs of their research is stored on this system.\n\n"
                   "Once you have stolen the research, copy it onto the File Server that we have set up for you, detailed below.\n\n"
                   "We are confident we will be able to use their artificial life code to make Revelation spread like wildfire."
                   "You will be paid " << PAYMENT_SPECIALMISSION_DARWIN << "c for this mission.\n\n"
				   "Codename: Darwin\n\n";

	fulldetails << "Target computer : " << target->name << "\n"
				<< "IP : " << target->ip << "\n\n"
				<< "Once complete, dump the files you have to this computer:\n"
				<< ourcomp->name << "\n"
				<< "IP : " << ourcomp->ip << "\n\n"
				<< "USERNAME : " << username << "\n"
				<< "PASSWORD : " << password << "\n\n"
                << "Reply to this mail when you have finished.\n\n"
				<< "END"
				<< '\x0';


	// Insert the mission
	Mission *mission = new Mission ();
	mission->SetTYPE		 ( MISSION_SPECIAL );
	mission->SetCompletion   ( NULL, NULL, NULL, NULL, NULL );
	mission->SetEmployer     ( "ARC" );
	mission->SetContact      ( "internal@ARC.net" );
	mission->SetDifficulty   ( 10 );
    mission->SetPayment      ( PAYMENT_SPECIALMISSION_DARWIN );
	mission->SetDescription  ( description );
	mission->SetFullDetails  ( fulldetails.str () );
	mission->GiveLink ( target->ip );
	mission->GiveLink ( ourcomp->ip );
	mission->GiveCode ( ourcomp->ip, code );

	fulldetails.rdbuf()->freeze( 0 );
    //delete [] fulldetails.str ();

    return mission;

}

Mission *PlotGenerator::GenerateMission_SaveItForTheJury ()
{

    // Find the person who is to be jailed

    Person *poorsod = WorldGenerator::GetRandomPerson ();
    UplinkAssert (poorsod);

    UplinkStrncpy ( saveitforthejury_guytobeframed, poorsod->name, sizeof ( saveitforthejury_guytobeframed ) );

    //char *personalcomputername = strdup( NameGenerator::GeneratePersonalComputerName(poorsod->name) );
	char personalcomputername[MAX_COMPUTERNAME];
	strncpy( personalcomputername, NameGenerator::GeneratePersonalComputerName(poorsod->name),
		     MAX_COMPUTERNAME );
	if ( personalcomputername[MAX_COMPUTERNAME - 1] != '\0' ) {
		personalcomputername[MAX_COMPUTERNAME - 1] = '\0';
	}

    Computer *personalComputer = game->GetWorld ()->GetComputer ( personalcomputername );
    UplinkAssert (personalComputer);

    // Choose a bank that the guy is due to hack

    BankComputer *bank = (BankComputer *) WorldGenerator::GetRandomComputer ( COMPUTER_TYPE_PUBLICBANKSERVER );
	UplinkAssert (bank);

    UplinkStrncpy ( saveitforthejury_targetbankip, bank->ip, sizeof ( saveitforthejury_targetbankip ) );

	char description [SIZE_MISSION_DESCRIPTION];
	std::ostrstream fulldetails;

    UplinkStrncpy ( description, SpecialMissionDescription (SPECIALMISSION_SAVEITFORTHEJURY), sizeof ( description ) );

    fulldetails << "We have decided that it is time to get personal.  The chief technical director of "
                   "Arunmor Corporation, Mr " << poorsod->name << ", has recently excelled in his line of work.\n"
                   "He has managed to make a number of breakthrough's in their research into Revelation, and no doubt "
                   "his presence at Arunmor has increased the power of their rival virus Faith.  We have decided that "
                   "his life should now come to an effective end.\n\n"
                   "In addition, we'd like to strike a blow to Arunmor's public image as well.  As such, we'd like you "
                   "to frame their chief tech for something particularly nasty - Bank Fraud.\n\n"
                   "Break in to the bank below, and start messing around.  Make sure they notice you.  Then break in "
                   "to one of your relay systems and modify the logs so that " << poorsod->name << " is incriminated.\n\n"
                   "To do this, you'll need to make the log point back to his home computer - address below.\n"
				   "Codename: SaveItForTheJury\n\n";
    fulldetails << "\n\nTARGET BANK:\n"
				<< bank->name << "\n"
				   "IP: " << bank->ip << "\n";
	fulldetails << "\n"
				<< "Target individual :\n"
				<< poorsod->name << "\n"
                << "His personal computer: " << personalComputer->ip << "\n"
                << '\x0';

	//
	// Build the mission
	//

	Mission *m = new Mission ();
	m->SetTYPE		   ( MISSION_SPECIAL );
	m->SetEmployer     ( "ARC" );
	m->SetContact      ( "internal@ARC.net" );
	m->SetPayment      ( PAYMENT_SPECIALMISSION_SAVEITFORTHEJURY );
	m->SetDifficulty   ( 10 );
	m->SetDescription  ( description );
	m->SetFullDetails  ( fulldetails.str () );

	m->GiveLink ( bank->ip );
    m->GiveLink ( personalComputer->ip );

    return m;

}

Mission *PlotGenerator::GenerateMission_ShinyHammer ()
{

    //
    // Insert some interesting data onto Arunmor's system

    Computer *target = game->GetWorld ()->GetComputer ( NameGenerator::GenerateLANName("Arunmor") );
    if ( UplinkIncompatibleSaveGameAssert (target, __FILE__, __LINE__) )
		return NULL;

	for ( int i = 0; i < 10; ++i ) {

		char datatitle [64];
		UplinkSnprintf ( datatitle, sizeof ( datatitle ), "%c%c%c%c-FAITH %d.dat", target->companyname [0], target->companyname [1],
												       target->companyname [2], target->companyname [3], i );

        int size = NumberGenerator::RandomNumber ( 10 ) + 1;
        int encrypted = NumberGenerator::RandomNumber ( 5 );

		Data *file = new Data ();
		file->SetTitle ( datatitle );
		file->SetDetails ( DATATYPE_DATA, size, encrypted, 0 );

		while ( target->databank.FindValidPlacement (file) == -1 )
			target->databank.SetSize ( target->databank.GetSize () + file->size );

		target->databank.PutData ( file );

    }

    //
    // Ahhh... a nice simple one for a change

    std::ostrstream details;
    details << "We are getting tired of racing against Arunmor Corporation, and recently "
               "their Faith project has been performing well against Revelation.  We need "
               "to put a stop to this.  Since the CEO of our company was arrested, Arunmor have "
               "been catching up with our research every day.\n\n"
               "This mission is designed to be a crushing blow "
               "to the research efforts of Arunmor Corporation.\n\n"
               "We know from our inside sources that Arunmor keep all of their research on "
               "their Local Area Network, and that it is heavily guarded. The fact that they leave "
               "this system open to the Internet will be their un-doing.\n\n"
               "Your task is simple.  Find their Local Area Network, break in to it, and delete "
               "all the files you find.  Then take down the entire system.\n\n"
			   "Codename: ShinyHammer\n\n"
               "You will encounter severe resistance.  We are prepared to pay you "
            << PAYMENT_SPECIALMISSION_SHINYHAMMER
            << " credits for completion of this mission.  If you succeed, we believe we will be able to "
               "take advantage of their momentary confusion and launch Revelation successfully.\n\n"
               "Be patient...the time is near."
            << '\x0';

    Mission *m = new Mission ();
    m->SetTYPE ( MISSION_SPECIAL );
    m->SetEmployer ( "ARC" );
    m->SetContact ( "internal@ARC.net" );
    m->SetPayment ( PAYMENT_SPECIALMISSION_SHINYHAMMER );
    m->SetDifficulty ( 15 );
    m->SetDescription ( SpecialMissionDescription (SPECIALMISSION_SHINYHAMMER) );
    m->SetFullDetails ( details.str () );

	details.rdbuf()->freeze( 0 );
    //delete [] details.str ();

    return m;

}

Mission *PlotGenerator::GenerateMission_GrandTour ()
{

    std::ostrstream details;
    details << "In your email inbox you will see two emails from us - one containing the final version "
               "of the Revelation virus, and the other containing a tracker which will list the systems "
               "currently infected.\n\n"
               "This is your green light agent " << game->GetWorld ()->GetPlayer ()->handle << " - we have "
               "selected you as our most successful freelance agent to carry out our final mission. Your aim is "
               "simple - release Revelation onto as many systems as you possibly can before our entire company "
               "is shut down by Federal Agents.\n\n"
               "Arunmor have Faith ready - they will be right behind you.  If you are quick enough, you will be able "
               "to spread the virus faster than they can stop it. Revelation will infect a system, scan for nearby systems "
               "on the same subnet, then migrate to the nearest two of those systems. At the same time it will destroy "
               "the host it originated from.  Once a certain number of systems have been disabled, the growth of the virus "
               "will be unstoppable and the entire Internet will suffer a system-wide cascade failure.\n\n"
               "This is the end - if you are successfull there will be no more "
               "Uplink Corporation, no more ARC and no more Arunmor.  The entire Internet will come down with us.  "
               "Think about it - we can destroy all records of ownership, reset everyone back to square one, crash "
               "the whole damn system.  Think of it!\n\n"
               "This will be our last message to you - if all goes to plan, we will never speak again.\n\n"
               "The codename for this mission is GrandTour.  Best of luck.  ARC out."
            << '\x0';


    Mission *m = new Mission ();
    m->SetTYPE ( MISSION_SPECIAL );
    m->SetEmployer ( "ARC" );
    m->SetContact ( "internal@ARC.net" );
    m->SetPayment ( PAYMENT_SPECIALMISSION_GRANDTOUR );
    m->SetDifficulty ( 20 );
    m->SetDescription ( SpecialMissionDescription (SPECIALMISSION_GRANDTOUR) );
    m->SetFullDetails ( details.str() );

	details.rdbuf()->freeze( 0 );
    //delete [] details.str();

    return m;

}

bool PlotGenerator::IsMissionComplete_Tracer ()
{

    // Find the target computer

    Computer *comp = game->GetWorld ()->GetComputer ( NameGenerator::GenerateInternalServicesServerName("ARC") );
    if ( UplinkIncompatibleSaveGameAssert (comp, __FILE__, __LINE__) )
		return false;

    if ( strcmp ( tracer_lastknownip, comp->ip ) == 0 ) {

        // Success

        Message *msg = new Message ();
        msg->SetFrom ( "internal@Arunmor.net" );
        msg->SetTo ( "PLAYER" );
        msg->SetSubject ( "Congratulations" );
        msg->SetBody ( "We just started receiving ping signals from our Tracer Software.\n"
                       "This means it is set up correctly and working. Time will tell if they managed "
                       "to spot what you are doing.\n\n"
                       "We have made the full mission payment into your account.\n\n"
                       "When the time comes, well send you some Tracer software that you can run on "
                       "your local system.  This will list all of the computers that have been infected "
                       "with the Revelation virus." );
        msg->Send ();

        version_faith += 1.0;
        completed_tracer = true;

        PlayerCompletesSpecialMission ( SPECIALMISSION_TRACER );


        // Schedule news story

        Date rundate;
		rundate.SetDate ( &(game->GetWorld ()->date) );
		rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 6 ) + 2 );

		RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
		rns->SetActAndScene ( 4, 4 );
		rns->SetRunDate ( &rundate );
		game->GetWorld ()->scheduler.ScheduleEvent ( rns );

        return true;

    }
    else {

        // Failure

        Message *msg = new Message ();
        msg->SetFrom ( "internal@Arunmor.net" );
        msg->SetTo ( "PLAYER" );
        msg->SetSubject ( "Failure" );
        msg->SetBody ( "We haven't received any ping signals from our Tracer yet.  You can't have installed it right." );
        msg->Send ();

        return false;

    }

}

bool PlotGenerator::IsMissionComplete_TakeMeToYourLeader ()
{

    // Nice and fucking simple

    Computer *comp = game->GetWorld ()->GetComputer ( NameGenerator::GenerateInternalServicesServerName("Arunmor") );
	if ( UplinkIncompatibleSaveGameAssert (comp, __FILE__, __LINE__) )
		return false;

	if ( comp->isinfected_revelation >= 0.0 ) {

        PlayerCompletesSpecialMission ( SPECIALMISSION_TAKEMETOYOURLEADER );

		// Send him a nice message

		Message *m = new Message ();
		m->SetFrom ( "internal@Arunmor.net" ); // Ammended by Ashley Pinner
		m->SetTo ( "PLAYER" );
		m->SetSubject ( "Congratulations" );
		m->SetBody ( "That looks like a total success to us. We'll have to keep quiet for a while, but it looks like "
                     "the Feds are getting ready to arrest the CEO of Andromeda Research Corporation tommorow.\n\n"
                     "It also looks like we are going to be able to question him about Revelation.  This is going to "
                     "help our cause.\n\n"
                     "We believe that the CEO of Andromeda Corporation is not actually the man in charge "
                     "of the Revelation project - since ARC is actually owned by the Andromeda group, and their leader "
                     "is currently unknown.  However, he undoubtably has a strong impact on their progress.\n\n"
                     "If we are very lucky, this man will help us find the leader of Andromeda.\n\n"
                     "It is also likely that our funding will be increased due to this outbreak.  The Government is "
                     "getting quite desperate at the moment - they think their entire infrastructure may be destroyed, "
                     "and we are the only people who can offer them a solution." );
		m->Send ();

        version_faith += 1.0;
        completed_takemetoyourleader = true;

		// Schedule the main news story for a few days from now

        Date rundate;
		rundate.SetDate ( &(game->GetWorld ()->date) );
		rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 6 ) + 2 );

		RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
		rns->SetActAndScene ( 4, 5 );
		rns->SetRunDate ( &rundate );
		game->GetWorld ()->scheduler.ScheduleEvent ( rns );

        return true;

    }

    return false;

}

bool PlotGenerator::IsMissionComplete_ARCInfiltration ()
{


    // Player thinks he has completed the ARC Infiltration mission

    Computer *ourcomp = game->GetWorld ()->GetComputer ( NameGenerator::GenerateFileServerName("Arunmor") );
    if ( UplinkIncompatibleSaveGameAssert (ourcomp, __FILE__, __LINE__) )
		return false;

	int foundsize = 0;

    int numfiles = 10;
	bool *filefound = new bool [numfiles];
	for ( int fi = 0; fi < numfiles; ++fi )
		filefound [fi] = false;

	for ( int i = 0; i < ourcomp->databank.GetDataSize (); ++i ) {
		if ( ourcomp->databank.GetDataFile (i) ) {

			Data *thisfile = ourcomp->databank.GetDataFile (i);
			UplinkAssert (thisfile);

            if ( !thisfile->encrypted ) {

			    if ( strstr ( thisfile->title, "REVELATION" ) != NULL &&
				     thisfile->encrypted == 0 ) {

				    char unused [64];
				    int thisfileindex;

				    sscanf ( thisfile->title, "%s %d.dat", unused, &thisfileindex );

				    if ( thisfileindex >= 0 && thisfileindex < numfiles &&
					     filefound [thisfileindex] == false ) {

					    foundsize += thisfile->size;
					    filefound [thisfileindex] = true;

				    }

			    }

            }

		}

	}

    // How many files out of the 10 did he find?

    if ( foundsize == 0 ) {

        Message *msg = new Message ();
        msg->SetTo ( "PLAYER" );
        msg->SetFrom ( "internal@Arunmor.net" );
        msg->SetSubject ( "No files found" );
        msg->SetBody ( "We don't see any useful files on the File Server we specified.\n"
                       "Have you copied the files onto the file server?  Have you remembered "
                       "to decrypt the files so we can read them?" );
        msg->Send ();

        return false;

    }
    else {

        std::ostrstream body;
        body << "\n\nWe can see "
             << foundsize << " Gigaquads of useful data which will help us "
             "perfect Faith.\n"
             << "Congratulations on your success Agent "
             << game->GetWorld ()->GetPlayer ()->handle
             << ". We are still not entirely confident that we can stop an outbreak of Revelation, "
                "but this data will certainly improve our chances.\n\n"
                "We have managed to determine that the reproductive systems in the Revelation virus "
                "are based on data and code stolen from Darwin Research Associates a few weeks ago.  The "
                "virus mutates, multiplies and evolves, much like a real life form.  We believe the way "
                "to defeat it is to attack it with another virus that it cannot evolve against."
             << '\x0';

        Message *msg = new Message ();
        msg->SetTo ( "PLAYER" );
        msg->SetFrom ( "internal@Arunmor.net" );
        msg->SetSubject ( "Congratulations" );
        msg->SetBody ( body.str () );
        msg->Send ();

		body.rdbuf()->freeze( 0 );
        //delete [] body.str ();

        completed_arcinfiltration = true;
        int change = (int) ( 3 * ((float) foundsize / (float) 90.0) );
        version_faith += change;


        PlayerCompletesSpecialMission ( SPECIALMISSION_ARCINFILTRATION );

        // Schedule news story

        Date rundate;
		rundate.SetDate ( &(game->GetWorld ()->date) );
		rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 6 ) + 2 );

		RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
		rns->SetActAndScene ( 4, 6 );
		rns->SetRunDate ( &rundate );
		game->GetWorld ()->scheduler.ScheduleEvent ( rns );

        return true;

    }

}

bool PlotGenerator::IsMissionComplete_MaidenFlight ()
{

    Computer *comp = game->GetWorld ()->GetComputer ( NameGenerator::GenerateInternalServicesServerName("Arunmor") );
	if ( UplinkIncompatibleSaveGameAssert (comp, __FILE__, __LINE__) )
		return false;

	if ( comp->isinfected_revelation > 0.0 ) {

        PlayerCompletesSpecialMission ( SPECIALMISSION_MAIDENFLIGHT );

		// Send him a nice message

		Message *m = new Message ();
		m->SetFrom ( "internal@ARC.net" );
		m->SetTo ( "PLAYER" );
		m->SetSubject ( "Congratulations" );
		m->SetBody ( "Congratulations on your recent success agent - we are very pleased "
					 "with the way things turned out.  We have made the appropriate payment into "
					 "your account.\n\n"
					 "No doubt this event will manage a lot of media coverage so it "
					 "is probably best if you keep a low profile for a short while.  We will contact "
					 "you again soon with some more work." );
		m->Send ();

        // Player now works for ARC

        playerloyalty = -1;
        act = 2;
        scene = 4;

        version_revelation = 1.0f;
        version_faith = 0.1f;

		// Schedule the main news story for a few days from now

        Date rundate;
		rundate.SetDate ( &(game->GetWorld ()->date) );
		rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 6 ) + 2 );

		RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
		rns->SetActAndScene ( 3, 1 );
		rns->SetRunDate ( &rundate );
		game->GetWorld ()->scheduler.ScheduleEvent ( rns );

        return true;

    }

    return false;

}

bool PlotGenerator::IsMissionComplete_Darwin ()
{

    // Player thinks he has completed the Darwin mission

    Computer *ourcomp = game->GetWorld ()->GetComputer ( NameGenerator::GenerateFileServerName("ARC") );
    if ( UplinkIncompatibleSaveGameAssert (ourcomp, __FILE__, __LINE__) )
		return false;

	int foundsize = 0;

    int numfiles = 10;
	bool *filefound = new bool [numfiles];
	for ( int fi = 0; fi < numfiles; ++fi )
		filefound [fi] = false;

	for ( int i = 0; i < ourcomp->databank.GetDataSize (); ++i ) {
		if ( ourcomp->databank.GetDataFile (i) ) {

			Data *thisfile = ourcomp->databank.GetDataFile (i);
			UplinkAssert (thisfile);

            if ( !thisfile->encrypted ) {

			    if ( strstr ( thisfile->title, "DARWIN" ) != NULL &&
				     thisfile->encrypted == 0 ) {

				    char unused [64];
				    int thisfileindex;

				    sscanf ( thisfile->title, "%s %d.dat", unused, &thisfileindex );

				    if ( thisfileindex >= 0 && thisfileindex < numfiles &&
					     filefound [thisfileindex] == false ) {

					    foundsize += thisfile->size;
					    filefound [thisfileindex] = true;

				    }

			    }

            }

		}

	}

    // How many files out of the 10 did he find?

    if ( foundsize == 0 ) {

        Message *msg = new Message ();
        msg->SetTo ( "PLAYER" );
        msg->SetFrom ( "internal@ARC.net" );
        msg->SetSubject ( "No files found" );
        msg->SetBody ( "We don't see any useful files on the File Server we specified.\n"
                       "Have you copied the files onto the file server?  Have you remembered "
                       "to decrypt the files so we can read them?" );
        msg->Send ();

        return false;

    }
    else {

        std::ostrstream body;
        body << "Congratulations on your success Agent "
             << game->GetWorld ()->GetPlayer ()->handle
             << "\n\nWe can see "
             << foundsize << " Gigaquads of useful data which will help us "
             "perfect Revelation.  At the moment the virus does not spread particularly well - "
             "if the security systems of the local machine are still running, the virus has "
             "trouble getting out of the system at all.  We are planning on using the research "
             "that Darwin has conducted to make the virus mutate and evolve, in order to "
             "combat any defense systems it comes up against."
             << '\x0';

        Message *msg = new Message ();
        msg->SetTo ( "PLAYER" );
        msg->SetFrom ( "internal@ARC.net" );
        msg->SetSubject ( "Congratulations" );
        msg->SetBody ( body.str () );
        msg->Send ();

		body.rdbuf()->freeze( 0 );
        //delete [] body.str ();

        completed_darwin = true;
        int change = (int) ( 2 * ((float) foundsize / (float) 90.0) );
        version_revelation += change;

        PlayerCompletesSpecialMission ( SPECIALMISSION_DARWIN );

        // Schedule news story

        Date rundate;
		rundate.SetDate ( &(game->GetWorld ()->date) );
		rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 6 ) + 2 );

		RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
		rns->SetActAndScene ( 4, 10 );
		rns->SetRunDate ( &rundate );
		game->GetWorld ()->scheduler.ScheduleEvent ( rns );

        return true;

    }

}

bool PlotGenerator::IsMissionComplete_SaveItForTheJury ()
{

    // Look up the persons computer

    Person *poorsod = game->GetWorld ()->GetPerson ( saveitforthejury_guytobeframed );
    if ( UplinkIncompatibleSaveGameAssert (poorsod, __FILE__, __LINE__) )
		return false;

    //char *personalcomputername = strdup( NameGenerator::GeneratePersonalComputerName( poorsod->name ) );
	char personalcomputername[MAX_COMPUTERNAME];
	strncpy( personalcomputername, NameGenerator::GeneratePersonalComputerName( poorsod->name ),
		     MAX_COMPUTERNAME );
	if ( personalcomputername[MAX_COMPUTERNAME - 1] != '\0' ) {
		personalcomputername[MAX_COMPUTERNAME - 1] = '\0';
	}

    Computer *personalComputer = game->GetWorld ()->GetComputer ( personalcomputername );
    UplinkAssert (personalComputer);

    // Lookup the bank he was supposed to be framed for

    VLocation *vl = game->GetWorld ()->GetVLocation ( saveitforthejury_targetbankip );
    UplinkAssert (vl);
    BankComputer *comp = (BankComputer *) vl->GetComputer ();
    UplinkAssert (comp);


    // Look through logs on that system
    // See if any trace back to him

    int success = 0;

	for ( int il = 0; il < comp->logbank.logs.Size (); ++il ) {
		if ( comp->logbank.logs.ValidIndex (il) ) {

			AccessLog *al = comp->logbank.logs.GetData (il);
			UplinkAssert (al);

			if ( al->SUSPICIOUS != LOG_NOTSUSPICIOUS &&
				 al->TYPE == LOG_TYPE_CONNECTIONOPENED ) {

				VLocation *vlf = game->GetWorld ()->GetVLocation ( al->fromip );
				Computer *fromcomp = NULL;
				if ( vlf )
					fromcomp = vlf->GetComputer ();

				if ( fromcomp ) {

					char *traced_ip = fromcomp->logbank.TraceLog ( comp->ip, fromcomp->ip, &(al->date), 10 );

					if ( traced_ip ) {

						if ( strcmp ( traced_ip, personalComputer->ip ) == 0 ) {

							// Now investigated - so set it back to LOG_NOTSUSPICIOUS

							al->SetSuspicious ( LOG_NOTSUSPICIOUS );
							if ( comp->logbank.internallogs.ValidIndex (il) )
								comp->logbank.internallogs.GetData (il)->SetSuspicious ( LOG_NOTSUSPICIOUS );

							// Success!

							success = 2;
							break;

						}
						else {

							success = 1;

						}

					}

				}

            }

        }
    }


	if ( success != 2 ) {

		CompanyUplink *uplink = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
		UplinkAssert (uplink);

		for ( int i = 0; i < uplink->news.Size (); ++i ) {

			News *news = uplink->news.GetData (i);
			UplinkAssert (news);

			if ( news->NEWSTYPE == NEWS_TYPE_ARREST && news->data1 && strcmp ( news->data1, poorsod->name ) == 0 ) {

				if ( news->data2 && strcmp ( news->data2, comp->ip ) == 0 ) {

					success = 2;
					break;

				}

			}

		}

	}


    if ( success == 2 ) {

        Message *msg = new Message ();
        msg->SetTo ( "PLAYER" );
        msg->SetFrom ( "internal@ARC.net" );
        msg->SetSubject ( "Congratulations" );
        msg->SetBody ( "Well done Agent - we have checked the systems ourselves and have determined that "
                       "enough evidence has been planted to convict the man.  Police will soon be receiving "
                       "an anonymous tip, which should end his work at Arunmor.\n\n"
                       "We are planning on using this instance to cast doubt over the actions of Arunmor Corporation. "
                       "Currently they are receiving a great deal of funding from the Government and are making "
                       "very fast progress on their Faith countervirus.  This has become something of an arms race. "
                       "Hopefully the Government will be less willing to fund such an operation when they hear about "
                       "this financial scandal." );
        msg->Send ();

        version_faith -= 1.0f;
        if ( version_faith < 0.2f ) version_faith = 0.2f;

        completed_saveitforthejury = true;

        PlayerCompletesSpecialMission ( SPECIALMISSION_SAVEITFORTHEJURY );

        // Schedule news story

        Date rundate;
		rundate.SetDate ( &(game->GetWorld ()->date) );
		rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 6 ) + 2 );

		RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
		rns->SetActAndScene ( 4, 11 );
		rns->SetRunDate ( &rundate );
		game->GetWorld ()->scheduler.ScheduleEvent ( rns );

        return true;

    }
    else if ( success == 1 ) {

        Message *msg = new Message ();
        msg->SetTo ( "PLAYER" );
        msg->SetFrom ( "internal@ARC.net" );
        msg->SetSubject ( "The wrong person has been framed" );
        msg->SetBody ( "We've looked at the systems involved, and we can see that you have performed the hack.\n"
                       "However, it doesn't appear that the logs lead back to his home computer.  You must fix this \n"
                       "before somebody traces the wrong person." );
        msg->Send ();

        return false;

    }
    else {

        Message *msg = new Message ();
        msg->SetTo ( "PLAYER" );
        msg->SetFrom ( "internal@ARC.net" );
        msg->SetSubject ( "Failure to complete mission" );
        msg->SetBody ( "You don't appear to have done the hack or framed the man in question." );
        msg->Send ();

        return false;

    }

}

bool PlotGenerator::IsMissionComplete_ShinyHammer ()
{

    // Look up the system

    Computer *computer = game->GetWorld ()->GetComputer ( NameGenerator::GenerateLANName("Arunmor") );
    if ( UplinkIncompatibleSaveGameAssert (computer, __FILE__, __LINE__) )
		return false;

    Message *msg = new Message ();
    msg->SetTo ( "PLAYER" );
    msg->SetFrom ( "internal@ARC.net" );
    msg->SetSubject ( "CONGRATULATIONS" );

    if ( !computer->isrunning && computer->databank.formatted ) {

        // Success!

        msg->SetBody ( "We've just heard the news about Arunmor's research - congratulations!\n\n"
                       "It looks like you performed the job with your usual style and skill.  Well done.\n\n"
                       "Faith is certain to take a major blow from this.  We are planning on releasing Revelation "
                       "very shortly...there are just a few more things to take care of first.\n\n"
                       "It is likely that Arunmor had some backups of the countervirus somewhere, so we will have to "
                       "move quickly to take advantage of their loss.\n"
                       "We will contact you soon." );

        version_faith -= 2.0;

    }
    else if ( !computer->isrunning && !computer->databank.formatted ) {

        // Shut down the system, didn't delete the files

        msg->SetBody ( "Well done for attempting the Arunmor crash mission.  It appears you shut down the system, "
                       "which will cause them problems.  But it appears they have recovered the data from back-ups.\n\n"
                       "You should have deleted all the files before you shut the system down.\n\n"
                       "Still, this will hurt Arunmor.  We are planning on releasing Revelation "
                       "very shortly...there are just a few more things to take care of first.\n\n"
                       "We will contact you soon." );

        version_faith -= 0.5;

    }
    else if ( computer->isrunning && computer->databank.formatted ) {

        // Didn't shut down system, but did delete files

        msg->SetBody ( "Congratulations on deleting the data on Arunmor's Local Area Network.  It appears you didn't "
                       "quite manage to shut down the system, but that's ok.  We've dealt them a nasty blow.  "
                       "We are planning on releasing Revelation "
                       "very shortly...there are just a few more things to take care of first.\n\n"
                       "We will contact you soon." );

        version_faith -= 1.0;

    }
    else {

        // Total failure

        msg->SetBody ( "You don't appear to have damaged the Arunmor Local Area Network very much.  Try again." );
        msg->Send ();

        return false;

    }

    if ( version_faith < 0.2f ) version_faith = 0.2f;

    msg->Send ();

    completed_shinyhammer = true;

    PlayerCompletesSpecialMission ( SPECIALMISSION_SHINYHAMMER );

    // Schedule news story

    Date rundate;
	rundate.SetDate ( &(game->GetWorld ()->date) );
	rundate.AdvanceHour ( NumberGenerator::RandomNumber ( 3 ) + 2 );

	RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
	rns->SetActAndScene ( 4, 12 );
	rns->SetRunDate ( &rundate );
	game->GetWorld ()->scheduler.ScheduleEvent ( rns );

    return true;

}

void PlotGenerator::NewsRevelationUsed ( char *ip, int success )
{

	// Look up the system attacked

	VLocation *vl = game->GetWorld ()->GetVLocation ( ip );
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);

	/*
		Headline

		Part1:		Company hit by virus
		Part2:		Extent of damage
		Part3:		History of virus use

		*/

	std::ostrstream headline;
	std::ostrstream part1, part2, part3;

	part1 << comp->companyname << " has been attacked by a computer virus.  "
								  "The virus first infected the " << comp->name << ".";

	switch ( success ) {

		case 0:		part2 << "System administrators were able to contain the virus before it managed any serious damage.  It appears the virus was totally contained.";			break;
		case 1:		part2 << "The virus destroyed most of the data and logs on the computer but the company was able to stop the virus from spreading to other systems.";		break;
		case 2:		part2 << "The " << comp->name << " was totally destroyed and the virus spread to many neighbouring systems, causing a great deal of damage.";				break;
		case 3:		part2 << "It appears that the entire corporate computer network owned by " << comp->companyname << " was severely damaged by the virus.";					break;

	};

	switch ( numuses_revelation ) {

		case 0:
		case 1:		part3 << "This is the first time that this virus has been seen in the wild, but experts who analysed it say it has incredible power.  "
							 "After analysing the virus in detail, it has been determined that the name of the virus is Revelation.";
					headline << "Unknown virus outbreak at " << comp->companyname;
					break;

		case 2:		part3 << "Upon closer inspection, industry experts have been able to determine that the Revelation computer virus is responsible.  "
							 "This is the second time that Revelation has been deployed on the internet.";
					headline << "Revelation seen for second time at " << comp->companyname;
					break;

		default:	part3 << "Once again Revelation is responsible for this destruction.";
					headline << "Revelation outbreak yet again";
					break;

	};

	//
	// Concatenate each part together and post it
	//

	headline << '\x0';
	part1 << '\x0';
	part2 << '\x0';
	part3 << '\x0';

	std::ostrstream details;
	details << part1.str () << "\n\n" << part2.str () << "\n\n" << part3.str () << "\n\n" << '\x0';

	News *news = new News ();
	news->SetHeadline ( headline.str () );
	news->SetDetails ( details.str () );

	news->SetData ( NEWS_TYPE_COMPUTERDESTROYED, comp->ip );

	headline.rdbuf()->freeze( 0 );
	part1.rdbuf()->freeze( 0 );
	part2.rdbuf()->freeze( 0 );
	part3.rdbuf()->freeze( 0 );
	details.rdbuf()->freeze( 0 );
	//delete [] headline.str ();
	//delete [] part1.str ();
	//delete [] part2.str ();
	//delete [] part3.str ();
	//delete [] details.str ();

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert (cu);
	cu->CreateNews ( news );

}

void PlotGenerator::Revelation_ReleaseUncontrolled ()
{

    // Its all over

    Date rundate;
    rundate.SetDate ( &(game->GetWorld ()->date) );
    rundate.AdvanceSecond ( 10 );

    RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
    rns->SetActAndScene ( 5, 7 );
    rns->SetRunDate ( &rundate );
    game->GetWorld ()->scheduler.ScheduleEvent ( rns );

    revelation_releaseuncontrolled = true;

    // Run revelation locally

    game->GetInterface ()->GetTaskManager ()->RunSoftware ( "Revelation", version_revelation );

}

#endif              // FULLGAME


void PlotGenerator::Infected ( char *ip )
{

#ifndef DEMOGAME

    UplinkAssert (ip);

    char *ipcopy = new char [strlen(ip)+1];
    UplinkSafeStrcpy ( ipcopy, ip );

    infected.PutDataAtEnd ( ipcopy );

    if ( infected.Size () >= REVELATION_RELEASEUNCONTROLLED ) {

        // Its game over!
        Revelation_ReleaseUncontrolled ();

    }

#endif

}

void PlotGenerator::Disinfected ( char *ip )
{

#ifndef DEMOGAME

     for ( int i = 0; i < infected.Size (); ++i )
        if ( infected.GetData (i) )
            if ( strcmp ( infected.GetData (i), ip ) == 0 )
                infected.RemoveData (i);

     if ( infected.Size () == 0 &&
          revelation_arcbusted == true &&
          playerloyalty == 1 &&
          !PlayerCompletedSpecialMission (SPECIALMISSION_COUNTERATTACK) ) {

         // Player was working for ARC
         // And has now won the game

         Message *msg = new Message ();
         msg->SetTo ( "PLAYER" );
         msg->SetFrom ( "internal@Arunmor.net" );
         msg->SetSubject ( "Revelation has been destroyed" );
         msg->SetBody ( "Congratulations would seem to be in order - not only have you "
                        "stopped Revelation in its tracks, you've also wiped out any trace "
                        "of the virus on the Internet.\n\n"
                        "As you are probably aware, Andromeda Research Corporation has been "
                        "raided and won't be doing any more damage at any point in the future. "
                        "The key members of the company are facing some serious charges.\n\n"
                        "Once again, a sincere thank you from Arunmor Corporation." );
         msg->Send ();

         PlayerCompletesSpecialMission ( SPECIALMISSION_COUNTERATTACK );

         game->WinCode ( "Completed the storyline with Arunmor", "ARU" );

#ifdef GAME_WINNING_CODE

         if ( game->IsCodeWon () ) {

              char *winningCode = game->GetWinningCode ();
              char *winningDesc = game->GetWinningCodeDesc ();

              Message *msg = new Message ();
              msg->SetFrom ( "internal@UplinkCorporation.net" );
              msg->SetTo ( "PLAYER" );
              msg->SetSubject ( "Code won" );

              std::ostrstream body;
              body << "Congratulations,\n"
                      "You won the following code:\n\n"
                   << winningDesc << "\n"
                   << winningCode << "\n"
                   << '\x0';

              msg->SetBody ( body.str () );

              msg->Send ();

              body.rdbuf()->freeze( 0 );
              //delete [] body.str ();

              delete [] winningDesc;
              delete [] winningCode;

         }

#endif

         // News story

         Date rundate;
         rundate.SetDate ( &(game->GetWorld ()->date) );
         rundate.AdvanceHour ( 3 );

         RunPlotSceneEvent *rns = new RunPlotSceneEvent ();
         rns->SetActAndScene ( 5, 3 );
         rns->SetRunDate ( &rundate );
         game->GetWorld ()->scheduler.ScheduleEvent ( rns );

     }

#endif

}

void PlotGenerator::PlayerVisitsPlotSites ()
{

    playervisitsplotsites = true;

}

bool PlotGenerator::PlayerCompletedSpecialMission ( int missionID )
{

	return ( specialmissionscompleted & (1 << missionID) );

}

int PlotGenerator::GetSpecialMissionsCompleted ()
{

	return specialmissionscompleted;

}

char *PlotGenerator::SpecialMissionTitle ( int missionID )
{

    UplinkAssert (missionID >= 0);
    UplinkAssert (missionID < 16);

    return MISSION_TITLE [missionID];

}

char *PlotGenerator::SpecialMissionDescription ( int missionID )
{

    UplinkAssert (missionID >= 0);
    UplinkAssert (missionID < 16);

    return MISSION_DESCRIPTION [missionID];

}

/********** Patched by François Gagné **********/
bool PlotGenerator::IsPlotCompany ( const char *companyname )
{

	if ( strcmp ( companyname, "ARC" ) != 0 &&
	     strcmp ( companyname, "Arunmor" ) != 0 &&
	     strcmp ( companyname, "Darwin Research Associates" ) != 0 )
		return false;

	return true;

}

void PlotGenerator::MessageIncompatibleSaveGame ( const char *fileassert, int lineassert )
{

	Message *msg = new Message ();
	msg->SetFrom ( "Problems@UplinkCorporation.net" );
	msg->SetTo ( "PLAYER" );
	msg->SetSubject ( "Problems" );

	std::ostrstream body;
	body << "Agent,\n"
			"If you are reading this email then it means your gateway got corrupted beyond "
			"any chances of recovery.\n\n"
			"You'll be forced to start under a new agent if you want to continue to work for us.\n"
			"If you do decide to continue under this agent, it will be impossible for you "
			"to complete the great challenges that were awaiting you.\n\n\n\n\n\n\n"
			"*** Technical info : This is normally appenning when playing with an old agent in a "
			"new version of Uplink.\n"
			"The storyline won't work with this agent.\n\n"
			"Location  :\n"
			<< fileassert << "\n"
			<< "Line      : "
			<< lineassert
			<< "\n"
			<< '\x0';

	msg->SetBody ( body.str () );

	msg->Send ();

	body.rdbuf()->freeze( 0 );
	//delete [] body.str ();

}

bool PlotGenerator::UplinkIncompatibleSaveGameAssert ( const bool &condition, const char *fileassert, int lineassert )
{

    if (!(condition)) {
	    printf ( "\n"
			     "An Uplink Assertion Failure has occured in the plot\n"
			     "===================================================\n"
			     " Location  : %s, line %d\n\n",
			     fileassert, lineassert );

		MessageIncompatibleSaveGame ( fileassert, lineassert );
		return true;

    }

	return false;

}

bool PlotGenerator::Load  ( FILE *file )
{

	LoadID ( file );

	if ( !FileReadData ( &act, sizeof(act), 1, file ) ) return false;
	if ( !FileReadData ( &scene, sizeof(scene), 1, file ) ) return false;

	if ( strcmp( game->GetLoadedSavefileVer(), "SAV59" ) >= 0 ) {
		if ( !LoadDynamicStringStatic ( act1scene3agent, sizeof(act1scene3agent), file ) ) return false;
		if ( !LoadDynamicStringStatic ( act1scene4agent, sizeof(act1scene4agent), file ) ) return false;
		if ( !LoadDynamicStringStatic ( act2scene1agent, sizeof(act2scene1agent), file ) ) return false;
	}
	else {
		if ( !FileReadData ( act1scene3agent, sizeof(act1scene3agent), 1, file ) ) {
			act1scene3agent [ sizeof(act1scene3agent) - 1 ] = '\0';
			return false;
		}
		act1scene3agent [ sizeof(act1scene3agent) - 1 ] = '\0';
		if ( !FileReadData ( act1scene4agent, sizeof(act1scene4agent), 1, file ) ) {
			act1scene4agent [ sizeof(act1scene4agent) - 1 ] = '\0';
			return false;
		}
		act1scene4agent [ sizeof(act1scene4agent) - 1 ] = '\0';
		if ( !FileReadData ( act2scene1agent, sizeof(act2scene1agent), 1, file ) ) {
			act2scene1agent [ sizeof(act2scene1agent) - 1 ] = '\0';
			return false;
		}
		act2scene1agent [ sizeof(act2scene1agent) - 1 ] = '\0';
	}

    if ( !FileReadData ( &playervisitsplotsites, sizeof(playervisitsplotsites), 1, file ) ) return false;
	if ( !FileReadData ( &playercancelsmail, sizeof(playercancelsmail), 1, file ) ) return false;
    if ( !FileReadData ( &playerloyalty, sizeof(playerloyalty), 1, file ) ) return false;

    if ( !FileReadData ( &completed_tracer, sizeof(completed_tracer), 1, file ) ) return false;
    if ( !FileReadData ( &completed_takemetoyourleader, sizeof(completed_takemetoyourleader), 1, file ) ) return false;
    if ( !FileReadData ( &completed_arcinfiltration, sizeof(completed_arcinfiltration), 1, file ) ) return false;
    if ( !FileReadData ( &completed_darwin, sizeof(completed_darwin), 1, file ) ) return false;
    if ( !FileReadData ( &completed_saveitforthejury, sizeof(completed_saveitforthejury), 1, file ) ) return false;
    if ( !FileReadData ( &completed_shinyhammer, sizeof(completed_shinyhammer), 1, file ) ) return false;

    if ( !FileReadData ( &specialmissionscompleted, sizeof(specialmissionscompleted), 1, file ) ) return false;

	if ( strcmp( game->GetLoadedSavefileVer(), "SAV59" ) >= 0 ) {
		if ( !LoadDynamicStringStatic ( saveitforthejury_guytobeframed, sizeof(saveitforthejury_guytobeframed), file ) ) return false;
		if ( !LoadDynamicStringStatic ( saveitforthejury_targetbankip, sizeof(saveitforthejury_targetbankip), file ) ) return false;
		if ( !LoadDynamicStringStatic ( tracer_lastknownip, sizeof(tracer_lastknownip), file ) ) return false;
	}
	else {
		if ( !FileReadData ( saveitforthejury_guytobeframed, sizeof(saveitforthejury_guytobeframed), 1, file ) ) {
			saveitforthejury_guytobeframed [ sizeof(saveitforthejury_guytobeframed) - 1 ] = '\0';
			return false;
		}
		saveitforthejury_guytobeframed [ sizeof(saveitforthejury_guytobeframed) - 1 ] = '\0';
		if ( !FileReadData ( saveitforthejury_targetbankip, sizeof(saveitforthejury_targetbankip), 1, file ) ) {
			saveitforthejury_targetbankip [ sizeof(saveitforthejury_targetbankip) - 1 ] = '\0';
			return false;
		}
		saveitforthejury_targetbankip [ sizeof(saveitforthejury_targetbankip) - 1 ] = '\0';
		if ( !FileReadData ( tracer_lastknownip, sizeof(tracer_lastknownip), 1, file ) ) {
			tracer_lastknownip [ sizeof(tracer_lastknownip) - 1 ] = '\0';
			return false;
		}
		tracer_lastknownip [ sizeof(tracer_lastknownip) - 1 ] = '\0';
	}

	if ( !FileReadData ( &numuses_revelation, sizeof(numuses_revelation), 1, file ) ) return false;
    if ( !FileReadData ( &revelation_releaseuncontrolled, sizeof(revelation_releaseuncontrolled), 1, file ) ) return false;
    if ( !FileReadData ( &revelation_releasefailed, sizeof(revelation_releasefailed), 1, file ) ) return false;
    if ( !FileReadData ( &revelation_arcbusted, sizeof(revelation_arcbusted), 1, file ) ) return false;

	if ( !FileReadData ( &version_revelation, sizeof(version_revelation), 1, file ) ) return false;
	if ( !FileReadData ( &version_faith, sizeof(version_faith), 1, file ) ) return false;

    if ( !LoadLList ( &infected, file ) ) return false;

	LoadID_END ( file );

	return true;

}

void PlotGenerator::Save  ( FILE *file )
{

	SaveID ( file );

	fwrite ( &act, sizeof(act), 1, file );
	fwrite ( &scene, sizeof(scene), 1, file );

	SaveDynamicString ( act1scene3agent, sizeof(act1scene3agent), file );
	SaveDynamicString ( act1scene4agent, sizeof(act1scene4agent), file );
	SaveDynamicString ( act2scene1agent, sizeof(act2scene1agent), file );

    fwrite ( &playervisitsplotsites, sizeof(playervisitsplotsites), 1, file );
    fwrite ( &playercancelsmail, sizeof(playercancelsmail), 1, file );
    fwrite ( &playerloyalty, sizeof(playerloyalty), 1, file );

    fwrite ( &completed_tracer, sizeof(completed_tracer), 1, file );
    fwrite ( &completed_takemetoyourleader, sizeof(completed_takemetoyourleader), 1, file );
    fwrite ( &completed_arcinfiltration, sizeof(completed_arcinfiltration), 1, file );
    fwrite ( &completed_darwin, sizeof(completed_darwin), 1, file );
    fwrite ( &completed_saveitforthejury, sizeof(completed_saveitforthejury), 1, file );
    fwrite ( &completed_shinyhammer, sizeof(completed_shinyhammer), 1, file );

    fwrite ( &specialmissionscompleted, sizeof(specialmissionscompleted), 1, file );

	SaveDynamicString ( saveitforthejury_guytobeframed, sizeof(saveitforthejury_guytobeframed), file );
	SaveDynamicString ( saveitforthejury_targetbankip, sizeof(saveitforthejury_targetbankip), file );
	SaveDynamicString ( tracer_lastknownip, sizeof(tracer_lastknownip), file );

	fwrite ( &numuses_revelation, sizeof(numuses_revelation), 1, file );
    fwrite ( &revelation_releaseuncontrolled, sizeof(revelation_releaseuncontrolled), 1, file );
    fwrite ( &revelation_releasefailed, sizeof(revelation_releasefailed), 1, file );
    fwrite ( &revelation_arcbusted, sizeof(revelation_arcbusted), 1, file );

	fwrite ( &version_revelation, sizeof(version_revelation), 1, file );
	fwrite ( &version_faith, sizeof(version_faith), 1, file );

    SaveLList ( &infected, file );

	SaveID_END ( file );

}

void PlotGenerator::Print ()
{

	printf ( "Plot Generator\n" );
	printf ( "Act=%d, Scene=%d\n", act, scene );
	printf ( "Act1Scene3Agent = %s\n", act1scene3agent );
	printf ( "Act1Scene4Agent = %s\n", act1scene4agent );
	printf ( "Act2Scene1Agent = %s\n", act2scene1agent );

	printf ( "PlayerVisitsPlotSites = %d\n", playervisitsplotsites );
    printf ( "PlayerCancelsMail = %d\n", playercancelsmail );
    printf ( "PlayerLoyalty = %d\n", playerloyalty );
	printf ( "NumUses_Revelation = %d\n", numuses_revelation );
	printf ( "Version Revelation = %f\n", version_revelation );
	printf ( "Version Faith = %f\n", version_faith );

    printf ( "Completed Tracer : %d\n", completed_tracer );
    printf ( "Completed TakeMeToYourLeader : %d\n", completed_takemetoyourleader );
    printf ( "Completed ARC Infiltration : %d\n", completed_arcinfiltration );
    printf ( "Completed Darwin : %d\n", completed_darwin );
    printf ( "Completed SaveItForTheJury : %d\n", completed_saveitforthejury );
    printf ( "Completed ShinyHammer : %d\n", completed_shinyhammer );

	printf ( "specialmissionscompleted : %d\n", specialmissionscompleted );

    printf ( "saveitforthejury_targetbankip : %s\n", saveitforthejury_targetbankip );
    printf ( "saveitforthejury_guytobeframed : %s\n", saveitforthejury_guytobeframed );
    printf ( "tracer_lastknownip : %s\n", tracer_lastknownip );

    printf ( "revelation_releaseuncontrolled : %d\n", revelation_releaseuncontrolled );
    printf ( "revelation_releasefailed : %d\n", revelation_releasefailed );
    printf ( "revelation_arcbusted : %d\n", revelation_arcbusted );

    printf ( "Infected IP addresses:\n" );
    PrintLList ( &infected );

}

void PlotGenerator::Update ()
{
}


char *PlotGenerator::GetID ()
{

	return "PLOTGEN";

}
