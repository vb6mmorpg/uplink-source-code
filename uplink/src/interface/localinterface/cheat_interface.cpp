
#include "eclipse.h"
#include "vanbakel.h"

#include "app/app.h"
#include "app/miscutils.h"
#include "app/serialise.h"

#include "options/options.h"

#include "game/game.h"
#include "game/data/data.h"

#include "interface/interface.h"
#include "interface/localinterface/localinterface.h"
#include "interface/localinterface/cheat_interface.h"
#include "interface/remoteinterface/remoteinterface.h"

#include "world/world.h"
#include "world/player.h"

#include "world/computer/computer.h"
#include "world/computer/lancomputer.h"
#include "world/computer/computerscreen/computerscreen.h"
#include "world/generator/consequencegenerator.h"
#include "world/generator/worldgenerator.h"
#include "world/scheduler/shotbyfedsevent.h"

#include "mmgr.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CheatInterface::TitleClick ( Button *button )
{
#ifdef TESTGAME
	game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_NONE );
#endif
}

void CheatInterface::AllLinksClick ( Button *button )
{
#ifdef TESTGAME
	game->GetWorld ()->GetPlayer ()->GiveAllLinks ();
#endif
}

void CheatInterface::AllSoftwareClick ( Button *button )
{
#ifdef TESTGAME
	game->GetWorld ()->GetPlayer ()->gateway.SetMemorySize ( 256 );
	game->GetWorld ()->GetPlayer ()->gateway.GiveAllSoftware ();
#endif
}

void CheatInterface::AllHardwareClick ( Button *button )
{
#ifdef TESTGAME
    game->GetWorld ()->GetPlayer ()->gateway.GiveAllHardware ();
#endif
}

void CheatInterface::LotsOfMoneyClick ( Button *button )
{
#ifdef TESTGAME
	game->GetWorld ()->GetPlayer ()->ChangeBalance ( 10000, "You cheating motherfucker!" );
#endif
}

void CheatInterface::MaxRatingsClick ( Button *button )
{
#ifdef TESTGAME
	game->GetWorld ()->GetPlayer ()->rating.SetUplinkRating ( NUM_UPLINKRATINGS-1 );
//	game->GetWorld ()->GetPlayer ()->rating.SetNeuromancerRating ( NUM_NEUROMANCERRATINGS-1 );
#endif
}

void CheatInterface::NextRatingClick ( Button *button )
{
#ifdef TESTGAME
	game->GetWorld ()->GetPlayer ()->rating.SetUplinkRating ( game->GetWorld ()->GetPlayer ()->rating.uplinkrating + 1 );
    game->GetWorld ()->GetPlayer ()->rating.ChangeUplinkScore ( 1 );
#endif
}

void CheatInterface::EventQueueClick ( Button *button )
{
#ifdef TESTGAME
	game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_EVTQUEUE );
#endif
}

void CheatInterface::RevelationClick ( Button *button )
{
#ifdef TESTGAME
    for ( int i = 0; i < 5; ++i ) {

        Computer *comp = WorldGenerator::GetRandomComputer ();
        game->GetWorld ()->plotgenerator.RunRevelation ( comp->ip, 3.0, true );

    }
#endif
}

void CheatInterface::EndGameClick ( Button *button )
{
#ifdef TESTGAME
	// Simulate a "shot by feds" event in 5 seconds

	Date duedate;
	duedate.SetDate ( &(game->GetWorld ()->date) );
	duedate.AdvanceSecond ( 5 );
	
	ShotByFedsEvent *event = new ShotByFedsEvent ();
	event->SetName ( "PLAYER" );
	event->SetReason ( "clicking on the END GAME button you fucking wanker" );
	event->SetRunDate ( &duedate );

	game->GetWorld ()->scheduler.ScheduleEvent ( event );
#endif
}

void CheatInterface::ShowLANClick ( Button *button )
{
#ifdef TESTGAME	
	VLocation *vl = game->GetWorld()->GetPlayer()->GetRemoteHost();
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);

	if ( comp->GetOBJECTID () != OID_LANCOMPUTER ) 
		return;
	
	LanComputer *lan = (LanComputer *) comp;

	for ( int i = 0; i < lan->systems.Size(); ++i ) {
		if ( lan->systems.ValidIndex(i) ) {

			LanComputerSystem *system = lan->systems.GetData(i);
			UplinkAssert (system);
			system->visible = LANSYSTEMVISIBLE_FULL;

		}
	}

	for ( int j = 0; j < lan->links.Size(); ++j ) {
		if ( lan->links.ValidIndex(j) ) {

			LanComputerLink *link = lan->links.GetData(j);
			UplinkAssert (link);
			link->visible = LANLINKVISIBLE_AWARE;

		}
	}
#endif
}

void CheatInterface::CancelTraceClick ( Button *button )
{
#ifdef TESTGAME
	game->GetWorld ()->GetPlayer ()->GetConnection ()->EndTrace ();
#endif
}

void CheatInterface::DebugPrintClick ( Button *button )
{
#ifdef TESTGAME
	app->Print ();
	EclDebugPrint ();
	SvbDebugPrint ();
#endif
}

void CheatInterface::Create ()
{

	if ( !IsVisible () ) {

		LocalInterfaceScreen::Create ();

		int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
		int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");
		int paneltop = (int)(100.0 * ( (screenw * PANELSIZE) / 188.0 )) + 30;
		int panelwidth = (int)(screenw * PANELSIZE);

		// Title

		EclRegisterButton ( screenw - panelwidth, paneltop + 3, panelwidth - 7, 15, "CHEATS", "Remove the cheats screen", "cheat_title" );
		EclRegisterButtonCallback ( "cheat_title", TitleClick );

		// Give all links

		EclRegisterButton ( screenw - panelwidth, paneltop + 50, panelwidth - 7, 15, "Give all links", "cheat_alllinks" );
		EclRegisterButtonCallback ( "cheat_alllinks", AllLinksClick );
		
		// All software

		EclRegisterButton ( screenw - panelwidth, paneltop + 70, panelwidth - 7, 15, "Give all software", "cheat_allsoftware" );
		EclRegisterButtonCallback ( "cheat_allsoftware", AllSoftwareClick );

		// All hardware

		EclRegisterButton ( screenw - panelwidth, paneltop + 90, panelwidth - 7, 15, "Give all hardware", "cheat_allhardware" );
		EclRegisterButtonCallback ( "cheat_allhardware", AllHardwareClick );

		// Lots of money

		EclRegisterButton ( screenw - panelwidth, paneltop + 110, panelwidth - 7, 15, "Give lots of money", "cheat_money" );
		EclRegisterButtonCallback ( "cheat_money", LotsOfMoneyClick );

		// Next rating

		EclRegisterButton ( screenw - panelwidth, paneltop + 130, panelwidth - 7, 15, "Give next rating", "cheat_nextrating" );
		EclRegisterButtonCallback ( "cheat_nextrating", NextRatingClick );

		// Maximum ratings

		EclRegisterButton ( screenw - panelwidth, paneltop + 150, panelwidth - 7, 15, "Give max ratings", "cheat_maxratings" );
		EclRegisterButtonCallback ( "cheat_maxratings", MaxRatingsClick );

		// Cancel trace

		EclRegisterButton ( screenw - panelwidth, paneltop + 170, panelwidth - 7, 15, "Cancel current trace", "cheat_canceltrace" );
		EclRegisterButtonCallback ( "cheat_canceltrace", CancelTraceClick );

		// End game

		EclRegisterButton ( screenw - panelwidth, paneltop + 190, panelwidth - 7, 15, "End Game", "cheat_endgame" );
		EclRegisterButtonCallback ( "cheat_endgame", EndGameClick );
		
		// Debug Print

		EclRegisterButton ( screenw - panelwidth, paneltop + 210, panelwidth - 7, 15, "Debug Print", "cheat_debugprint" );
		EclRegisterButtonCallback ( "cheat_debugprint", DebugPrintClick );

		// Event Queue 

		EclRegisterButton ( screenw - panelwidth, paneltop + 230, panelwidth - 7, 15, "Event Queue", "cheat_eventqueue" );
		EclRegisterButtonCallback ( "cheat_eventqueue", EventQueueClick );
		
        // Run Revelation

        EclRegisterButton ( screenw - panelwidth, paneltop + 250, panelwidth - 7, 15, "Run Revelation", "cheat_revelation" );
        EclRegisterButtonCallback ( "cheat_revelation", RevelationClick );

		// Show LAN

		EclRegisterButton ( screenw - panelwidth, paneltop + 270, panelwidth - 7, 15, "Show Local Area Network", "cheat_showlan" );
		EclRegisterButtonCallback ( "cheat_showlan", ShowLANClick );

	}

}

void CheatInterface::Remove ()
{

	if ( IsVisible () ) {

		LocalInterfaceScreen::Remove ();

		EclRemoveButton ( "cheat_title" );

		EclRemoveButton ( "cheat_alllinks" );
		EclRemoveButton ( "cheat_allsoftware" );
		EclRemoveButton ( "cheat_allhardware" );
		EclRemoveButton ( "cheat_money" );
		EclRemoveButton ( "cheat_nextrating" );
		EclRemoveButton ( "cheat_maxratings" );
		EclRemoveButton ( "cheat_endgame" );
		EclRemoveButton ( "cheat_canceltrace" );
		EclRemoveButton ( "cheat_debugprint" );
		EclRemoveButton ( "cheat_eventqueue" );
        EclRemoveButton ( "cheat_revelation" );
		EclRemoveButton ( "cheat_showlan" );

	}

}

void CheatInterface::Update ()
{
}

bool CheatInterface::IsVisible ()
{

	return ( EclGetButton ( "cheat_title" ) != NULL );

}

int CheatInterface::ScreenID ()
{
	
	return SCREEN_CHEATS;

}
