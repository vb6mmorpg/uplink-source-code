
#include "stdafx.h"

#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"

#include "game/game.h"
#include "game/data/data.h"

#include "world/world.h"
#include "world/rating.h"
#include "world/message.h"
#include "world/player.h"

#include "world/scheduler/eventscheduler.h"
#include "world/scheduler/notificationevent.h"

#include "mmgr.h"


Rating::Rating ()
{

	uplinkrating = 0;
	neuromancerrating = 0;

	uplinkscore = 0;
	neuromancerscore = 0;

	creditrating = 0;

	UplinkStrncpy ( owner, "", sizeof ( owner ) );

}

Rating::~Rating ()
{
}

char *Rating::GetUplinkRatingString ( int urating )
{

	UplinkAssert ( urating >= 0 && urating < NUM_UPLINKRATINGS );
	return UPLINKRATING [urating].name;

}

char *Rating::GetNeuromancerRatingString ( int nrating )
{

	UplinkAssert ( nrating >= 0 && nrating < NUM_NEUROMANCERRATINGS );
	return NEUROMANCERRATING [nrating].name;

}

void Rating::SetOwner ( char *newowner )
{

	UplinkAssert (strlen(newowner) < SIZE_PERSON_NAME );
	UplinkStrncpy ( owner, newowner, sizeof ( owner ) );

}

void Rating::SetUplinkRating ( int rating )
{

	UplinkAssert ( rating >= 0 && rating < NUM_UPLINKRATINGS );
	uplinkrating = rating;
	uplinkscore = UPLINKRATING [rating].score;

	if ( uplinkrating > creditrating )
		creditrating = uplinkrating;

}

char *Rating::GetUplinkRating ()
{

	return GetUplinkRatingString ( uplinkrating );

}

void Rating::SetNeuromancerRating ( int rating )
{

	UplinkAssert ( rating >= 0 && rating < NUM_NEUROMANCERRATINGS );
	neuromancerrating = rating;
	neuromancerscore = NEUROMANCERRATING [rating].score;

}

char *Rating::GetNeuromancerRating ()
{

	return GetNeuromancerRatingString ( neuromancerrating );

}

void Rating::ChangeUplinkScore ( int amount )
{

    if ( amount < 0 && uplinkrating <= 2 ) return;
    if ( amount == 0 ) return;

	uplinkscore += amount;
	if ( amount < 0 && uplinkscore < UPLINKRATING[2].score ) uplinkscore = UPLINKRATING[2].score;

	// Find the new uplinkrating (it might / might not have changed)
	int oldrating = uplinkrating;

	for ( int i = 0; i < NUM_UPLINKRATINGS - 1; ++i ) {
		if ( uplinkscore >= UPLINKRATING [i].score &&
			uplinkscore < UPLINKRATING [i+1].score ) {
			uplinkrating = i;
			break;
		}
	}

	// Special case - highest uplink rating
	if ( uplinkscore >= UPLINKRATING [NUM_UPLINKRATINGS-1].score )
		uplinkrating = NUM_UPLINKRATINGS-1;

	// Special case - less points than minimum rating
	if ( uplinkscore < UPLINKRATING[2].score ) {
		uplinkscore = UPLINKRATING[2].score;
		uplinkrating = 2;
	}

    // Special case - player needs to be told about Connection Analyser
    if ( strcmp ( owner, "PLAYER" ) == 0 ) {

        if ( uplinkrating == 6 && oldrating < 6 ) {
            if ( !game->GetWorld ()->GetPlayer ()->gateway.HasHUDUpgrade (HUDUPGRADE_CONNECTIONANALYSIS) ) {

                Message *m = new Message ();
                m->SetTo ( "PLAYER" );
                m->SetFrom ( "Uplink Corporation" );
                m->SetSubject ( "Information for Skilled Agents" );
                m->SetBody ( "Firstly allow us to congratulate you on your recent successes.  We have been following your "
                             "career with interest.  As an up-and-coming Agent, you will find a number "
                             "of new opportunities are now available to you.  At this point we feel we should give "
                             "you an important piece of information.\n\n"
                             "Now that the stakes are higher, you will certainly begin to encounter computer systems "
                             "that are more secure than before.  These systems can be difficult to attack.\n\n"
                             "Most of our Agents now use the HUD-Connection-Analyser to get around these systems.  "
                             "This item of software was designed by Uplink Corporation specifically for bypassing security "
                             "systems, and can be purchased from Uplink Internal Services.  It is designed to be used "
                             "in conjunction with Security-Bypassers, also available from that site.\n\n"
                             "Uplink Corporation wishes you luck." );
                m->Send ();

            }
        }

    }

#ifdef DEMOGAME

	// Special case - upper limit reached in Demo game

    if ( strcmp ( owner, "PLAYER" ) == 0 ) {

        if ( uplinkrating != oldrating )
            game->GetWorld ()->demoplotgenerator.PlayerRatingChange ();

        if ( uplinkscore >= UPLINKRATING [DEMO_MAXUPLINKRATING].score ) {

		    uplinkrating = DEMO_MAXUPLINKRATING;
		    uplinkscore = UPLINKRATING [DEMO_MAXUPLINKRATING].score;

		    return;

	    }

    }

#endif

#ifdef WAREZRELEASE

    // Special case - illegal copy

    if ( strcmp ( owner, "PLAYER" ) == 0 ) {

        if ( uplinkscore >= UPLINKRATING [WAREZ_MAXUPLINKRATING].score ) {

		    uplinkrating = WAREZ_MAXUPLINKRATING;
		    uplinkscore = UPLINKRATING [WAREZ_MAXUPLINKRATING].score;

            Date rundate;
            rundate.SetDate ( &game->GetWorld ()->date );
            rundate.AdvanceMinute ( TIME_TOWAREZGAMEOVER );

            NotificationEvent *ne = new NotificationEvent ();
            ne->SetTYPE ( NOTIFICATIONEVENT_TYPE_WAREZGAMEOVER );
            ne->SetRunDate ( &rundate );

            game->GetWorld ()->scheduler.ScheduleEvent ( ne );

		    return;

	    }

    }

#endif

	// Has our rating changed ?

	if ( uplinkrating > oldrating ) {

		// Score has gone up
		// Send a congrats message to the owner

		if ( game->GetWorld ()->GetPerson (owner) ) {

			char *agenthandle = strcmp ( owner, "PLAYER" ) != 0 ? owner : game->GetWorld ()->GetPlayer ()->handle;

			std::ostrstream body;
			body << "Congratulations Agent " << agenthandle
				 << " - you have reached the next Uplink Rating.\n\n"
					"Your rating is now " << GetUplinkRating () << "."
				 << '\x0';

			Message *msg = new Message ();
			msg->SetFrom ( "internal@Uplink.net" );
			msg->SetTo ( owner );
			msg->SetSubject ( "Congratulations" );
			msg->SetBody ( body.str () );
			msg->Send ();

			body.rdbuf()->freeze( 0 );
			//delete [] body.str ();

			// Increase our credit rating

			if ( uplinkrating > creditrating )
				creditrating = uplinkrating;

		}

	}
	else if ( uplinkrating < oldrating ) {

		// Score has gone down
		// Send a warning message to the owner

		if ( game->GetWorld ()->GetPerson (owner) ) {

			char *agenthandle = strcmp ( owner, "PLAYER" ) != 0 ? owner : game->GetWorld ()->GetPlayer ()->handle;
			std::ostrstream body;
			body << "This is a warning Agent " << agenthandle << " - your performance recently has been unsatisfactory.\n"
					"As such we have been forced to review your case, and have decided to decrease your Uplink rating.\n\n"
					"Your rating is now " << GetUplinkRating () << "."
					"We hope this is sufficient punishment to make our point clear.\n"
					"Uplink Corporation will not tolerate failure."
				 << '\x0';

			Message *msg = new Message ();
			msg->SetFrom ( "internal@Uplink.net" );
			msg->SetTo ( owner );
			msg->SetSubject ( "Uplink WARNING" );
			msg->SetBody ( body.str () );
			msg->Send ();

			body.rdbuf()->freeze( 0 );
			//delete [] body.str ();

		}

	}
	else {

		// Score has not changed

	}

}

void Rating::ChangeNeuromancerScore ( int amount )
{

	neuromancerscore += amount;

	// Find the new neuromancer rating (it might / might not have changed)
	int oldrating = neuromancerrating;

	for ( int i = 0; i < NUM_NEUROMANCERRATINGS - 1; ++i ) {
		if ( neuromancerscore >= NEUROMANCERRATING [i].score &&
			neuromancerscore < NEUROMANCERRATING [i+1].score ) {
			neuromancerrating = i;
			break;
		}
	}

	// Special case - highest rating
	if ( neuromancerscore >= NEUROMANCERRATING [NUM_NEUROMANCERRATINGS-1].score )
		neuromancerrating = NUM_NEUROMANCERRATINGS-1;

	// Special case - less points than minimum rating (ie negative)
	if ( neuromancerscore < NEUROMANCERRATING [0].score ) {
		neuromancerrating = 0;
	}

	// Has our rating changed ?

	if ( neuromancerrating > oldrating ) {

		// Score has gone up
		// Send a congrats message to the owner

		if ( game->GetWorld ()->GetPerson (owner) ) {

			char body [128];
			UplinkSnprintf ( body, sizeof ( body ), "Agent %s - your Neuromancer rating appears to have changed.\n\n"
													"Your rating is now %s.",
													strcmp ( owner, "PLAYER" ) != 0 ? owner : game->GetWorld ()->GetPlayer ()->handle,
													GetNeuromancerRating () );

			Message *msg = new Message ();
			msg->SetFrom ( "internal@Uplink.net" );
			msg->SetTo ( owner );
			msg->SetSubject ( "Neuromancer rating change" );
			msg->SetBody ( body );
			msg->Send ();

		}

	}
	else if ( neuromancerrating < oldrating ) {

		// Score has gone down

		if ( game->GetWorld ()->GetPerson (owner) ) {

			char body [256];
			UplinkSnprintf ( body, sizeof ( body ), "It appears your neuromancer rating has changed, in view of your recent actions.\n\n"
													"Your new rating is %s.",
													GetNeuromancerRating () );

			Message *msg = new Message ();
			msg->SetFrom ( "internal@Uplink.net" );
			msg->SetTo ( owner );
			msg->SetSubject ( "Neuromancer rating change" );
			msg->SetBody ( body );
			msg->Send ();

		}

	}
	else {

		// Score has not changed

	}

}

void Rating::SetCreditRating ( int rating )
{

	creditrating = rating;

}

void Rating::ChangeCreditRating ( int amount )
{

    creditrating += amount;

}

bool Rating::Load  ( FILE *file )
{

	LoadID ( file );

	if ( !LoadDynamicStringStatic ( owner, SIZE_PERSON_NAME, file ) ) return false;

	if ( !FileReadData ( &uplinkscore, sizeof(uplinkscore), 1, file ) ) return false;
	if ( !FileReadData ( &neuromancerscore, sizeof(neuromancerscore), 1, file ) ) return false;
	if ( !FileReadData ( &uplinkrating, sizeof(uplinkrating), 1, file ) ) return false;
	if ( !FileReadData ( &neuromancerrating, sizeof(neuromancerrating), 1, file ) ) return false;
	if ( !FileReadData ( &creditrating, sizeof(creditrating), 1, file ) ) return false;

	LoadID_END ( file );

	return true;

}

void Rating::Save  ( FILE *file )
{

	SaveID ( file );

	SaveDynamicString ( owner, SIZE_PERSON_NAME, file );

	fwrite ( &uplinkscore, sizeof(uplinkscore), 1, file );
	fwrite ( &neuromancerscore, sizeof(neuromancerscore), 1, file );
	fwrite ( &uplinkrating, sizeof(uplinkrating), 1, file );
	fwrite ( &neuromancerrating, sizeof(neuromancerrating), 1, file );
	fwrite ( &creditrating, sizeof(creditrating), 1, file );

	SaveID_END ( file );

}

void Rating::Print ()
{

	printf ( "Rating : Owner:%s\n", owner );
	printf ( "\tUplink Score      = %d (%s)\n", uplinkscore, GetUplinkRating () );
	printf ( "\tNeuromancer Score = %d (%s)\n", neuromancerscore, GetNeuromancerRating () );
	printf ( "\tCredit rating     = %d\n", creditrating );

}

void Rating::Update ()
{
}


char *Rating::GetID ()
{

	return "RATING";

}
