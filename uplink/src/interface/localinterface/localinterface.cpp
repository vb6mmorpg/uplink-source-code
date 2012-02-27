// LocalInterface.cpp: implementation of the LocalInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "gucci.h"
#include "vanbakel.h"
#include "soundgarden.h"
#include "redshirt.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"

#include "game/game.h"

#include "interface/localinterface/localinterface.h"

#include "world/world.h"
#include "world/player.h"

#include "interface/interface.h"
#include "interface/localinterface/hud_interface.h"
#include "interface/localinterface/worldmap/worldmap_interface.h"

// === Include all Local Interface objects here ===============================


#include "interface/localinterface/hud_interface.h"
#include "interface/localinterface/hw_interface.h"
#include "interface/localinterface/phonedialler.h"
#include "interface/localinterface/memory_interface.h"
#include "interface/localinterface/status_interface.h"
#include "interface/localinterface/finance_interface.h"
#include "interface/localinterface/email_interface.h"
#include "interface/localinterface/mission_interface.h"
#include "interface/localinterface/sendmail_interface.h"
#include "interface/localinterface/cheat_interface.h"
#include "interface/localinterface/evtqueue_interface.h"
#include "interface/localinterface/analyser_interface.h"
#include "interface/localinterface/irc_interface.h"
#include "interface/localinterface/lan_interface.h"

#include "mmgr.h"


// ============================================================================


LocalInterface::LocalInterface()
{

	currentscreencode = 0;
	screenindex = 0;
	screen = NULL;

	hud = new HUDInterface ();

}

LocalInterface::~LocalInterface()
{

	if ( screen ) delete screen;
	if ( hud ) delete hud;

}

void LocalInterface::Reset ()
{

	currentscreencode = SCREEN_NONE;
	screenindex = 0;

}

void LocalInterface::Create ()
{

	GetHUD ()->Create ();
	RunScreen ( currentscreencode, screenindex );

}

void LocalInterface::Remove ()
{

	if ( IsVisible () ) 
		if ( screen ) {
			screen->Remove ();
			delete screen;
			screen = NULL;
		}

}

bool LocalInterface::IsVisible ()
{

	return ( InScreen () != SCREEN_NONE && screen->IsVisible () );

}

void LocalInterface::RunScreen ( int SCREENCODE, int index )
{

	// Get rid of the current local interface
	if ( screen ) {
		screen->Remove ();
		delete screen;
		screen = NULL;
	}

	// Fixing problems with overlapping world map

	if ( SCREENCODE == SCREEN_IRC || SCREENCODE == SCREEN_LAN ) {
	    WorldMapInterface *wmi = &(game->GetInterface ()->GetLocalInterface ()->GetHUD ()->wmi);
		if ( wmi && wmi->IsVisibleWorldMapInterface () == WORLDMAP_LARGE ) {
			wmi->CloseWorldMapInterface_Large ();
		}
	}

	currentscreencode = SCREENCODE;
	screenindex = index;

	UplinkAssert ( hud );
	hud->MoveSelecter ( currentscreencode, screenindex );	
	
	switch ( currentscreencode ) {

		case SCREEN_NONE:
	        SgPlaySound ( RsArchiveFileOpen ("sounds/close.wav"), "sounds/close.wav", false );
			break;

		case SCREEN_HW:
			screen = new HWInterface ();
			break;

		case SCREEN_MEMORY:
			screen = new MemoryInterface ();
			break;

		case SCREEN_STATUS:
			screen = new StatusInterface ();
			break;

		case SCREEN_FINANCE:
			screen = new FinanceInterface ();
			break;

		case SCREEN_ANALYSER:
			screen = new AnalyserInterface ();
			break;

        case SCREEN_IRC:
            screen = new IRCInterface ();
            break;

        case SCREEN_LAN:
            screen = new LanInterface ();
            break;

		case SCREEN_EMAIL:
            if ( game->GetWorld ()->GetPlayer ()->messages.ValidIndex (screenindex) ) {
                screen = new EmailInterface ();
			    ((EmailInterface *) screen)->SetMessage ( screenindex );
            }
            else currentscreencode = SCREEN_NONE;
			break;

		case SCREEN_MISSION:
            if ( game->GetWorld ()->GetPlayer ()->missions.ValidIndex (screenindex) ) {
                screen = new MissionInterface ();			
			    ((MissionInterface *) screen)->SetMission ( screenindex );			
            }
            else currentscreencode = SCREEN_NONE;
			break;

		case SCREEN_SENDMAIL:
			screen = new SendMailInterface ();
			break;

		case SCREEN_CHEATS:
			screen = new CheatInterface ();
			break;

		case SCREEN_EVTQUEUE:
			screen = new EventQueueInterface ();
			break;

		default:
			UplinkAbortArgs ("Tried to create a local screen with unknown SCREENCODE %d", SCREENCODE );

	}

	if ( currentscreencode != SCREEN_NONE ) screen->Create ();

	// Bring all running tasks to the front
	SvbShowAllTasks ();

}

bool LocalInterface::VerifyScreen ( int SCREENCODE, int index )
{

	switch ( SCREENCODE ) {

		case SCREEN_NONE:
		case SCREEN_HW:
		case SCREEN_MEMORY:
		case SCREEN_STATUS:
		case SCREEN_FINANCE:
		case SCREEN_ANALYSER:
        case SCREEN_IRC:
        case SCREEN_LAN:
		case SCREEN_EMAIL:
		case SCREEN_MISSION:
		case SCREEN_SENDMAIL:
		case SCREEN_CHEATS:
		case SCREEN_EVTQUEUE:
			break;
		default:
			UplinkPrintAbortArgs ( "Tried to create a local screen with unknown SCREENCODE %d", SCREENCODE );
			return false;

	}

	return true;

}

int LocalInterface::InScreen ()
{

	if ( screen )
		return screen->ScreenID ();

	else
		return SCREEN_NONE;

}

LocalInterfaceScreen *LocalInterface::GetInterfaceScreen ()
{

	UplinkAssert ( screen );
	return screen;

}

HUDInterface *LocalInterface::GetHUD ()
{

	UplinkAssert ( hud );
	return hud;

}

bool LocalInterface::Load ( FILE *file )
{

	LoadID ( file );

	if ( !FileReadData ( &currentscreencode, sizeof(currentscreencode), 1, file ) ) return false;
	if ( !FileReadData ( &screenindex, sizeof(screenindex), 1, file ) ) return false;

	if ( !VerifyScreen ( currentscreencode, screenindex ) ) return false;

	if ( !GetHUD ()->Load ( file ) ) return false;
	GetHUD ()->Create ();

	RunScreen ( currentscreencode, screenindex );

	LoadID_END ( file );

	return true;

}

void LocalInterface::Save ( FILE *file )
{

	SaveID ( file );

    // If we're looking at the LAN or IRC, don't save that fact
    // otherwise the window will be recreated on re-load, covering the screen

    if ( currentscreencode == SCREEN_LAN || currentscreencode == SCREEN_IRC ) {                            
        int screencode = SCREEN_NONE;
        fwrite ( &screencode, sizeof(screencode), 1, file );
    }
    else
	    fwrite ( &currentscreencode, sizeof(currentscreencode), 1, file );

	fwrite ( &screenindex, sizeof(screenindex), 1, file );

	GetHUD ()->Save ( file );
	
	SaveID_END ( file );

}

void LocalInterface::Print  ()
{

	printf ( "Local Interface : Current Screen Code = %d, Screen index = %d\n", currentscreencode, screenindex );

}

void LocalInterface::Update ()
{
	
	GetHUD ()->Update ();
	if ( screen ) screen->Update ();

}

char *LocalInterface::GetID ()
{

	return "LINT";

}
