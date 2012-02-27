// MainMenu.cpp: implementation of the MainMenu class.
//
//////////////////////////////////////////////////////////////////////

#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"

#include "options/options.h"

#include "mainmenu/mainmenu.h"

// === Include all Local Interface objects here ===============================

#include "mainmenu/loading_interface.h"
#include "mainmenu/firsttimeloading_interface.h"
#include "mainmenu/options_interface.h"
#include "mainmenu/login_interface.h"
#include "mainmenu/obituary_interface.h"
#include "mainmenu/connectionlost_interface.h"
#include "mainmenu/disavowed_interface.h"
#include "mainmenu/genericoptions_interface.h"
#include "mainmenu/networkoptions_interface.h"
#include "mainmenu/theteam_interface.h"
#include "mainmenu/closing_interface.h"
#include "mainmenu/demogameover_interface.h"
#include "mainmenu/revelationwon_interface.h"
#include "mainmenu/revelationlost_interface.h"
#include "mainmenu/warezgameover_interface.h"
#include "mainmenu/theme_interface.h"
#include "mainmenu/graphicoptions_interface.h"

// ============================================================================

#include "mmgr.h"


MainMenu::MainMenu()
{

	currentscreencode = 0;
	screen = NULL;

}

MainMenu::~MainMenu()
{

	if ( screen ) delete screen;

}

void MainMenu::Create ()
{

    EclReset ( app->GetOptions ()->GetOptionValue ("graphics_screenwidth"),
			   app->GetOptions ()->GetOptionValue ("graphics_screenheight") );
	RunScreen ( currentscreencode );

}

void MainMenu::Remove ()
{

	if ( IsVisible () ) 
		if ( screen ) {
			screen->Remove ();
			delete screen;
			screen = NULL;
		}

}

bool MainMenu::IsVisible ()
{

	return ( InScreen () != MAINMENU_UNKNOWN );

}

void MainMenu::RunScreen ( int SCREENCODE )
{

	// Get rid of the current local interface
	if ( screen ) {
		screen->Remove ();
		delete screen;
		screen = NULL;
	}

	currentscreencode = SCREENCODE;

	switch ( currentscreencode ) {

		case MAINMENU_LOGIN:			screen = new LoginInterface ();					break;
		case MAINMENU_LOADING:			screen = new LoadingInterface ();				break;
		case MAINMENU_FIRSTLOAD:		screen = new FirstTimeLoadingInterface ();		break;
		case MAINMENU_OPTIONS:			screen = new OptionsInterface ();				break;		
		case MAINMENU_OBITUARY:			screen = new ObituaryInterface ();				break;
		case MAINMENU_CONNECTIONLOST:	screen = new ConnectionLostInterface ();		break;
		case MAINMENU_DISAVOWED:		screen = new DisavowedInterface ();				break;
		case MAINMENU_THETEAM:			screen = new TheTeamInterface ();				break;
        case MAINMENU_CLOSING:          screen = new ClosingInterface ();               break;
        case MAINMENU_DEMOGAMEOVER:     screen = new DemoGameOverInterface ();          break;
        case MAINMENU_REVELATIONWON:    screen = new RevelationWonInterface ();         break;
        case MAINMENU_REVELATIONLOST:   screen = new RevelationLostInterface ();        break;
        case MAINMENU_WAREZGAMEOVER:    screen = new WarezGameOverInterface ();         break;

		case MAINMENU_GENERICOPTIONS:	screen = new GenericOptionsInterface ();		break;
		case MAINMENU_NETWORKOPTIONS:	screen = new NetworkOptionsInterface ();		break;
        case MAINMENU_THEME:            screen = new ThemeInterface ();                 break;
		case MAINMENU_GRAPHICOPTIONS:	screen = new GraphicOptionsInterface ();		break;

		case MAINMENU_UNKNOWN:
			UplinkAbort ( "Tried to create a local screen with MAINMENU_UNKNOWN SCREENCODE" );

		default:
			UplinkAbort( "Tried to create a local screen with unknown SCREENCODE" );

	}

	screen->Create ();

}

int MainMenu::InScreen ()
{

	if ( screen )
		return screen->ScreenID ();

	else
		return MAINMENU_UNKNOWN;

}

MainMenuScreen *MainMenu::GetMenuScreen ()
{

	UplinkAssert ( screen );
	return screen;

}

bool MainMenu::Load ( FILE *file )
{
	return true;
}

void MainMenu::Save ( FILE *file )
{
}

void MainMenu::Print  ()
{
}

void MainMenu::Update ()
{
	
	if ( screen ) screen->Update ();

}

char *MainMenu::GetID ()
{

	return "MMI";

}

