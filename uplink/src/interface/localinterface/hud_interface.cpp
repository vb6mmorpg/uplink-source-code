#ifdef WIN32
#include <windows.h>
#endif

#ifndef HAVE_GLES
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <GLES/gl.h>
#include <GLES/glues.h>
#endif

#include <time.h>

#include "vanbakel.h"
#include "eclipse.h"
#include "gucci.h"
#include "soundgarden.h"
#include "redshirt.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl.h"
#include "app/opengl_interface.h"

#include "game/game.h"

#include "options/options.h"

#include "view/view.h"

#include "interface/interface.h"
#include "interface/localinterface/localinterface.h"
#include "interface/localinterface/hud_interface.h"
#include "interface/localinterface/worldmap/worldmap_interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/taskmanager/taskmanager.h"

#include "mainmenu/mainmenu.h"

#include "world/world.h"
#include "world/player.h"
#include "world/message.h"
#include "world/company/mission.h"
#include "world/vlocation.h"

#include "mmgr.h"


HUDInterfaceUpgrade HUDInterface::hudUpgrades[8] = {
    
    2, "Analyser",     "Show the connection analyser",     "hud_analyser",     "hud/analyser.tif",     "hud/analyser_h.tif",   "hud/analyser_c.tif",   AnalyserClick,
    4, "IRCClient",    "Show the IRC Client",              "hud_ircclient",    "hud/irc.tif",          "hud/irc_h.tif",        "hud/irc_c.tif",        IRCClick,
    8, "LANView",      "Show the LAN Viewer",              "hud_lanview",      "hud/lan.tif",          "hud/lan_h.tif",        "hud/lan_c.tif",        LANClick

    };


HUDInterface::HUDInterface ()
{
	
	previoushighlight = NULL;
	previousimage = NULL;

    for ( int i = 0; i < 8; ++i )
        visibleUpgrades[i] = 0;

}

HUDInterface::~HUDInterface ()
{

	if ( previoushighlight ) delete [] previoushighlight;
	if ( previousimage ) delete previousimage;

}

void HUDInterface::MainMenuClick ( Button *button )
{

	CloseGame ();

}

void HUDInterface::CloseGame ()
{

    SgPlaySound ( RsArchiveFileOpen ("sounds/close.wav"), "sounds/close.wav", false );
    
	// Close any connections

	game->GetWorld ()->GetPlayer ()->GetConnection ()->Disconnect ();
	game->GetWorld ()->GetPlayer ()->GetConnection ()->Reset ();

	game->GetInterface ()->GetRemoteInterface ()->RunNewLocation ();
	game->GetInterface ()->GetRemoteInterface ()->RunScreen ( 0 );		


	// Save game and log off

	app->SaveGame ( game->GetWorld ()->GetPlayer ()->handle );
	game->SetGameSpeed ( GAMESPEED_PAUSED );

    EclReset ( app->GetOptions ()->GetOptionValue ("graphics_screenwidth"),
			   app->GetOptions ()->GetOptionValue ("graphics_screenheight") );
	app->GetMainMenu ()->RunScreen ( MAINMENU_LOGIN );

}

void HUDInterface::SoftwareClick ( Button *button )
{

	GetHUD ()->si.ToggleSoftwareMenu ();

}

void HUDInterface::HardwareClick ( Button *button )
{

	if ( GetHUD ()->previoushighlight && strcmp ( GetHUD ()->previoushighlight, button->name ) == 0 ) 
		game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_NONE );

	else
		game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_HW );

}

void HUDInterface::MemoryClick ( Button *button )
{

	if ( GetHUD ()->previoushighlight && strcmp ( GetHUD ()->previoushighlight, button->name ) == 0 ) 
		game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_NONE );

	else
		game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_MEMORY );

}

void HUDInterface::StatusClick ( Button *button )
{

	if ( GetHUD ()->previoushighlight && strcmp ( GetHUD ()->previoushighlight, button->name ) == 0 ) 
		game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_NONE );

	else
		game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_STATUS );

}

void HUDInterface::FinanceClick	( Button *button )
{

	if ( GetHUD ()->previoushighlight && strcmp ( GetHUD ()->previoushighlight, button->name ) == 0 ) 
		game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_NONE );

	else
		game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_FINANCE );

}

void HUDInterface::SendMailClick ( Button *button )
{

	if ( GetHUD ()->previoushighlight && strcmp ( GetHUD ()->previoushighlight, button->name ) == 0 ) 
		game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_NONE );

	else
		game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_SENDMAIL );

}

void HUDInterface::AnalyserClick ( Button *button )
{

	if ( GetHUD ()->previoushighlight && strcmp ( GetHUD ()->previoushighlight, button->name ) == 0 ) 
		game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_NONE );

	else
		game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_ANALYSER );

}

void HUDInterface::IRCClick ( Button *button )
{

	if ( GetHUD ()->previoushighlight && strcmp ( GetHUD ()->previoushighlight, button->name ) == 0 ) 
		game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_NONE );

	else
		game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_IRC );

}

void HUDInterface::LANClick ( Button *button )
{

	if ( GetHUD ()->previoushighlight && strcmp ( GetHUD ()->previoushighlight, button->name ) == 0 ) 
		game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_NONE );

	else
		game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_LAN );

}

void HUDInterface::ToolbarButtonDraw ( Button *button, bool highlighted, bool clicked )
{

//	UplinkAssert ( button );

	glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );
//	GciDrawText ( button->x, button->y + 34, button->caption, HELVETICA_10 );

	imagebutton_draw ( button, highlighted, clicked );

}

void HUDInterface::EmailClick ( Button *button )
{

	UplinkAssert ( button );

	int index;
	sscanf ( button->name, "hud_message %d", &index );

    if ( !game->GetWorld()->GetPlayer()->messages.ValidIndex(index) ) {
        printf ( "HUDInterface WARNING: Tried to view an email that didn't exist\n" );
        EclRemoveButton ( button->name );
        return;
    }

	if ( GetHUD ()->previoushighlight && strcmp ( GetHUD ()->previoushighlight, button->name ) == 0 ) 
		game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_NONE );

	else
		game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_EMAIL, index );

}

void HUDInterface::EmailHighlight ( Button *button )
{

	UplinkAssert ( button );

	int index;
	sscanf ( button->name, "hud_message %d", &index );

	Message *message = game->GetWorld ()->GetPlayer ()->messages [index];

	if ( message ) {

		char tooltip [512];
		UplinkSnprintf ( tooltip, sizeof ( tooltip ), "'%s', From '%s'", message->GetSubject (), message->from );

		button->SetTooltip ( tooltip );
		button_highlight ( button );

	}

}

void HUDInterface::MissionClick	( Button *button )
{

	UplinkAssert ( button );

	int index;
	sscanf ( button->name, "hud_mission %d", &index );

    if ( !game->GetWorld()->GetPlayer()->missions.ValidIndex(index) ) {
        printf ( "HUDInterface WARNING: Tried to view a mission that didn't exist\n" );
        EclRemoveButton ( button->name );
        return;
    }

	if ( GetHUD ()->previoushighlight && strcmp ( GetHUD ()->previoushighlight, button->name ) == 0 ) 
		game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_NONE );

	else
		game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_MISSION, index );

}

void HUDInterface::MissionHighlight ( Button *button )
{

	UplinkAssert ( button );

	int index;
	sscanf ( button->name, "hud_mission %d", &index );

	Mission *mission = game->GetWorld ()->GetPlayer ()->missions [index];
	
	if ( mission ) {

		char tooltip [512];
		UplinkStrncpy ( tooltip, mission->description, sizeof ( tooltip ) );

		button->SetTooltip ( tooltip );
		button_highlight ( button );
	
	}

}

void HUDInterface::SpeedButtonDraw ( Button *button, bool highlighted, bool clicked )
{

	int index;
	sscanf ( button->name, "hud_speed %d", &index );

	if ( index == game->GameSpeed () ) 
		imagebutton_draw ( button, highlighted, true );

	else
		imagebutton_draw ( button, highlighted, clicked );

}

void HUDInterface::SpeedButtonClick ( Button *button )
{

	int index;
	sscanf ( button->name, "hud_speed %d", &index );

    game->SetGameSpeed ( index );

    EclDirtyButton ( "hud_speed 0" );
	EclDirtyButton ( "hud_speed 1" );
	EclDirtyButton ( "hud_speed 2" );
	EclDirtyButton ( "hud_speed 3" );

#ifdef CHEATMODES_ENABLED
	EclDirtyButton ( "hud_speed 4" );
#endif

}

void HUDInterface::UnPauseClick ( Button *button )
{

    remove_msgbox ();
    game->SetGameSpeed ( GAMESPEED_NORMAL );

    EclDirtyButton ( "hud_speed 0" );
	EclDirtyButton ( "hud_speed 1" );
	EclDirtyButton ( "hud_speed 2" );
	EclDirtyButton ( "hud_speed 3" );

#ifdef CHEATMODES_ENABLED
	EclDirtyButton ( "hud_speed 4" );
#endif

}

void HUDInterface::PauseButtonClick  ( Button *button )
{

    game->SetGameSpeed ( GAMESPEED_PAUSED );

    //
    // Create a message 
    // And tie the close button to Play

    create_msgbox ( "Message", "Uplink is paused\nClick Close to continue" );
    EclRegisterButtonCallback ( "msgbox_close", UnPauseClick );
    EclRegisterButtonCallback ( "msgbox_title", UnPauseClick );

    EclDirtyButton ( "hud_speed 0" );
	EclDirtyButton ( "hud_speed 1" );
	EclDirtyButton ( "hud_speed 2" );
	EclDirtyButton ( "hud_speed 3" );

#ifdef CHEATMODES_ENABLED
	EclDirtyButton ( "hud_speed 4" );
#endif
    
}

void HUDInterface::MoveSelecter ( int screenID, int screenindex )
{

	/*
				
	    screenindex is only used in some cases when a particular screenID
		might not be enough to locate the button - for example the emails
		at the bottom of the screen (all the same screenID, but different
		buttons based on index)

		*/

	UnHighlightToolbarButton ();

	switch ( screenID ) {

		case SCREEN_HW: 			HighlightToolbarButton ( "hud_hardware" );				break;
		case SCREEN_MEMORY:			HighlightToolbarButton ( "hud_memory" );				break;
		case SCREEN_STATUS:			HighlightToolbarButton ( "hud_status" );				break;
		case SCREEN_FINANCE:		HighlightToolbarButton ( "hud_finance" );				break;
		case SCREEN_SENDMAIL:		HighlightToolbarButton ( "hud_sendmail" );				break;
		case SCREEN_ANALYSER:		HighlightToolbarButton ( "hud_analyser" );				break;
        case SCREEN_IRC:            HighlightToolbarButton ( "hud_ircclient" );             break;
        case SCREEN_LAN:            HighlightToolbarButton ( "hud_lanview" );               break;
		case SCREEN_CHEATS:																	break;
		case SCREEN_EVTQUEUE:																break;
		case SCREEN_NONE:																	break;

		case SCREEN_EMAIL:
		{
			char bname [32];
			UplinkSnprintf ( bname, sizeof ( bname ), "hud_message %d", screenindex );
			HighlightToolbarButton ( bname );
			break;
		}

		case SCREEN_MISSION:
		{
			char bname [32];
			UplinkSnprintf ( bname, sizeof ( bname ), "hud_mission %d", screenindex );
			HighlightToolbarButton ( bname );
			break;
		}

		default:
			UplinkAbort ( "Unknown screenID" );

	}

}

void HUDInterface::HighlightToolbarButton   ( char *bname )
{

	// Get the button

	Button *button = EclGetButton ( bname );

	if ( button ) {

		// Swap the highlighted/unhighlighted/clicked graphics around

		GetHUD ()->previousimage = button->image_highlighted;
		Image *new_standard      = button->image_clicked;
		Image *new_highlighted   = button->image_clicked;
		Image *new_clicked       = button->image_standard;

		button->image_standard    = new_standard;
		button->image_highlighted = new_highlighted;
		button->image_clicked     = new_clicked;

		EclDirtyButton ( bname );

		// Remember this button was highlighted

		if ( GetHUD ()->previoushighlight ) delete [] GetHUD ()->previoushighlight;
		GetHUD ()->previoushighlight = new char [strlen(bname)+1];
		UplinkSafeStrcpy ( GetHUD ()->previoushighlight, bname );

	}
	else {

		printf ( "HUDInterface::HighlightToolbarButton, invalid button : %s\n", bname );	

	}

}

void HUDInterface::UnHighlightToolbarButton ()
{

	if ( GetHUD ()->previoushighlight ) {

		// Get the button

		Button *button = EclGetButton ( GetHUD ()->previoushighlight );
		
        if ( button ) {

		    // Swap the highlighted/unhighlighted/clicked graphics around

		    Image *new_standard    = button->image_clicked;
		    Image *new_highlighted = GetHUD ()->previousimage;
		    Image *new_clicked     = button->image_standard;

		    GetHUD ()->previousimage = NULL;

		    button->image_standard    = new_standard;
		    button->image_highlighted = new_highlighted;
		    button->image_clicked     = new_clicked;

		    EclDirtyButton ( GetHUD ()->previoushighlight );

        }

		// No button is now highlighted

		delete [] GetHUD ()->previoushighlight;
		GetHUD ()->previoushighlight = NULL;

	}

}

void HUDInterface::Create ()
{

	if ( !IsVisible () ) {

		int screenh = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );

		// Main world map 

		wmi.Create ( WORLDMAP_SMALL );		

		// Software button
		
		EclRegisterButton ( 3, screenh - 70, 53, 53, "", "Run a software application", "hud_software" );
		button_assignbitmaps ( "hud_software", "hud/software.tif", "hud/software_h.tif", "hud/software_c.tif" );
		EclRegisterButtonCallback ( "hud_software", SoftwareClick );
		
		// Button bar

		EclRegisterButton ( 60, screenh - 42, 24, 24, "Hardware", "Show the hardware installed in your gateway", "hud_hardware" );
		button_assignbitmaps ( "hud_hardware", "hud/hardware.tif", "hud/hardware_h.tif", "hud/hardware_c.tif" );
		EclRegisterButtonCallbacks ( "hud_hardware", ToolbarButtonDraw, HardwareClick, button_click, button_highlight );

		EclRegisterButton ( 60, screenh - 42, 24, 24, "Memory", "Show your memory banks", "hud_memory" );
		button_assignbitmaps ( "hud_memory", "hud/memory.tif", "hud/memory_h.tif", "hud/memory_c.tif" );
		EclRegisterButtonCallbacks ( "hud_memory", ToolbarButtonDraw, MemoryClick, button_click, button_highlight );

		EclRegisterButton ( 60, screenh - 42, 24, 24, "Status", "Show your status in the World", "hud_status" );
		button_assignbitmaps ( "hud_status", "hud/status.tif", "hud/status_h.tif", "hud/status_c.tif" );
		EclRegisterButtonCallbacks ( "hud_status", ToolbarButtonDraw, StatusClick, button_click, button_highlight );

		EclRegisterButton ( 60, screenh - 42, 24, 24, "Finances", "Show your financial situation", "hud_finance" );
		button_assignbitmaps ( "hud_finance", "hud/finance.tif", "hud/finance_h.tif", "hud/finance_c.tif" );
		EclRegisterButtonCallbacks ( "hud_finance", ToolbarButtonDraw, FinanceClick, button_click, button_highlight );

		EclRegisterButton ( 60, screenh - 42, 24, 24, "New Mail", "Send an email", "hud_sendmail" );
		button_assignbitmaps ( "hud_sendmail", "hud/newmail.tif", "hud/newmail_h.tif", "hud/newmail_c.tif" );
		EclRegisterButtonCallbacks ( "hud_sendmail", ToolbarButtonDraw, SendMailClick, button_click, button_highlight );

		EclRegisterMovement ( "hud_hardware",  60,  screenh - 42, 200 );
		EclRegisterMovement ( "hud_memory",    87,  screenh - 42, 300 );
		EclRegisterMovement ( "hud_status",    114, screenh - 42, 400 );
		EclRegisterMovement ( "hud_finance",   141, screenh - 42, 500 );
		EclRegisterMovement ( "hud_sendmail",  168, screenh - 42, 600 );		
		
		// Status bar 
		
		EclRegisterButton ( 3, 4, 13, 13, "", "Close any connections and log off", "hud_mainmenu" );
		button_assignbitmaps ( "hud_mainmenu","close.tif", "close_h.tif", "close_c.tif" );
		EclRegisterButtonCallback ( "hud_mainmenu", MainMenuClick );
		
		EclRegisterButton ( 0, 0, 3, 3, "", "Close any connections and log off", "hud_topleftclick" );
		EclRegisterButtonCallbacks ( "hud_topleftclick", NULL, MainMenuClick, button_click, button_highlight );

		EclRegisterButton ( 20, 3, 147, 15, "", "Shows the current Date and Time", "hud_date" );
		EclRegisterButton ( 170, 3, 90, 15, "", "Shows your current Uplink location", "hud_location" );

		EclRegisterButtonCallbacks ( "hud_date", button_draw, NULL, NULL, NULL );
		EclRegisterButtonCallbacks ( "hud_location", button_draw, NULL, NULL, NULL );

		// Speed buttons

		EclRegisterButton ( 263, 3, 15, 15, "", "Run at PAUSED speed", "hud_speed 0" );
		button_assignbitmaps ( "hud_speed 0", "hud/speed0.tif", "hud/speed0_h.tif", "hud/speed0_c.tif" );
		EclRegisterButtonCallbacks ( "hud_speed 0", SpeedButtonDraw, PauseButtonClick, button_click, button_highlight ); 
        
		EclRegisterButton ( 280, 3, 15, 15, "", "Run at NORMAL speed", "hud_speed 1" );
		button_assignbitmaps ( "hud_speed 1", "hud/speed1.tif", "hud/speed1_h.tif", "hud/speed1_c.tif" );
		EclRegisterButtonCallbacks ( "hud_speed 1", SpeedButtonDraw, SpeedButtonClick, button_click, button_highlight ); 

		EclRegisterButton ( 297, 3, 15, 15, "", "Run at FAST speed", "hud_speed 2" );
		button_assignbitmaps ( "hud_speed 2", "hud/speed2.tif", "hud/speed2_h.tif", "hud/speed2_c.tif" );
		EclRegisterButtonCallbacks ( "hud_speed 2", SpeedButtonDraw, SpeedButtonClick, button_click, button_highlight );
		
		EclRegisterButton ( 314, 3, 15, 15, "", "Run at VERYFAST speed", "hud_speed 3" );
		button_assignbitmaps ( "hud_speed 3", "hud/speed3.tif", "hud/speed3_h.tif", "hud/speed3_c.tif" );
		EclRegisterButtonCallbacks ( "hud_speed 3", SpeedButtonDraw, SpeedButtonClick, button_click, button_highlight );
		
#ifdef CHEATMODES_ENABLED
		EclRegisterButton ( 331, 3, 15, 15, "", "Run at OH-MY-GOD speed (DEBUG ONLY)", "hud_speed 4" );
		button_assignbitmaps ( "hud_speed 4", "hud/speed3.tif", "hud/speed3_h.tif", "hud/speed3_c.tif" );
		EclRegisterButtonCallbacks ( "hud_speed 4", SpeedButtonDraw, SpeedButtonClick, button_click, button_highlight );
#endif

		// Task manager 
		SvbCreateInterface ( 335, 15 );

	}

}


bool HUDInterface::IsUpgradeVisible ( char upgrade )
{

    int visible = 0;

    for ( int i = 0; i < 8; ++i ) 
        if ( visibleUpgrades[i] == upgrade ) {
            visible++;
            break;
        }


    HUDInterfaceUpgrade *theUpgrade = GetUpgrade(upgrade);
    if ( theUpgrade && EclGetButton ( theUpgrade->buttonName ) )
        visible++;

    return ( visible == 2 );

}

void HUDInterface::AddUpgrade ( char upgrade )
{

    if ( !IsUpgradeVisible (upgrade) ) {
    
        int insertedAt = -1;
        for ( int i = 0; i < 8; ++i ) {
            
            if ( visibleUpgrades[i] == 0 ) {                
                visibleUpgrades [i] = upgrade;
                insertedAt = i;
                break;
            }
                   
        }

        if ( insertedAt == -1 ) return;

        HUDInterfaceUpgrade *theUpgrade = GetUpgrade(upgrade);

        if ( theUpgrade ) {

            int xPos = 195 + 27 * insertedAt;
		    int screenh = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );

		    EclRegisterButton ( 60, screenh - 42, 24, 24, theUpgrade->name, theUpgrade->tooltip, theUpgrade->buttonName );
		    button_assignbitmaps ( theUpgrade->buttonName, theUpgrade->buttonFilename, theUpgrade->buttonFilename_h, theUpgrade->buttonFilename_c );
		    EclRegisterButtonCallbacks ( theUpgrade->buttonName, ToolbarButtonDraw, theUpgrade->callback, button_click, button_highlight );
		    EclRegisterMovement ( theUpgrade->buttonName, xPos, screenh - 42, 700 );		

        }

    }

}

void HUDInterface::RemoveUpgrade ( char upgrade )
{

    UplinkAbort ( "You said this was never needed!" );

}

HUDInterfaceUpgrade *HUDInterface::GetUpgrade ( char upgrade )
{

    for ( int i = 0; i < 8; ++i ) {
        
        HUDInterfaceUpgrade *thisUpgrade = &hudUpgrades[i];
        if ( thisUpgrade->number == upgrade )
            return thisUpgrade;

    }

    return NULL;

}

void HUDInterface::Update ()
{

	if ( IsVisible () ) {

		int screenw = app->GetOptions ()->GetOptionValue ( "graphics_screenwidth" );
		int screenh = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );

		// Update the location, date/time 

		char caption [128];
		char *date = game->GetWorld ()->date.GetLongString ();

        UplinkStrncpy ( caption, game->GetWorld ()->GetPlayer ()->GetRemoteHost ()->ip, sizeof ( caption ) );

		EclGetButton ( "hud_location" )->SetCaption ( caption );
		EclGetButton ( "hud_date" )->SetCaption ( date );
		
		// Update the messages on display

		for ( int mi = 0; mi < game->GetWorld ()->GetPlayer ()->messages.Size (); ++mi ) {

			char bname [128];
			UplinkSnprintf ( bname, sizeof ( bname ), "hud_message %d", mi );
			if ( !EclGetButton ( bname ) ) {
				EclRegisterButton ( 222, screenh - 41, 24, 24, "", "Read this message", bname );
				button_assignbitmaps ( bname, "hud/email.tif", "hud/email_h.tif", "hud/email_c.tif" );
				EclRegisterButtonCallbacks ( bname, imagebutton_draw, EmailClick, button_click, EmailHighlight );
				EclRegisterMovement ( bname, screenw - 30 * (mi+1), screenh - 41, 1000 );
                SgPlaySound ( RsArchiveFileOpen ("sounds/newmail.wav"), "sounds/newmail.wav", false );
			}	

		}

		// What the fuck is this for?
		// It removes any message buttons that shouldn't be there 
		// (ie if they have been deleted)

        int removeMessageIndex = game->GetWorld ()->GetPlayer ()->messages.Size ();
		char bname [128];
		UplinkSnprintf ( bname, sizeof ( bname ), "hud_message %d", removeMessageIndex );
        while ( EclGetButton ( bname ) ) {
			EclRemoveButton ( bname );
            ++removeMessageIndex;
            UplinkSnprintf ( bname, sizeof ( bname ), "hud_message %d", removeMessageIndex );
        }


		// Update the missions on display

		int baseX = screenw - 30 * (game->GetWorld ()->GetPlayer ()->messages.Size () + 1);

		for ( int msi = 0; msi < game->GetWorld ()->GetPlayer ()->missions.Size (); ++msi ) {

			char bname [128];
			UplinkSnprintf ( bname, sizeof ( bname ), "hud_mission %d", msi );

			if ( !EclGetButton ( bname ) ) {
				EclRegisterButton ( 222, screenh - 41, 24, 24, "", "View this mission", bname );
				button_assignbitmaps ( bname, "hud/mission.tif", "hud/mission_h.tif", "hud/mission_c.tif" );
				EclRegisterButtonCallbacks ( bname, imagebutton_draw, MissionClick, button_click, MissionHighlight );
				EclRegisterMovement ( bname, baseX - 30 * msi, screenh - 41, 1000 );
                SgPlaySound ( RsArchiveFileOpen ("sounds/newmail.wav"), "sounds/newmail.wav", false );
			}	
			else {
				if ( EclGetButton ( bname ) && EclGetButton ( bname )->x != baseX - 30 * msi )
					if ( EclIsAnimationActive ( bname ) == -1 )
						EclRegisterMovement ( bname, baseX - 30 * msi, screenh - 41, 300 );

			}

		}
		
		// Remove any mission buttons that shouldn't be here
        int removeMissionIndex = game->GetWorld ()->GetPlayer ()->missions.Size ();
        UplinkSnprintf ( bname, sizeof ( bname ), "hud_mission %d", removeMissionIndex );
        while ( EclGetButton ( bname ) ) {
			EclRemoveButton ( bname );
            ++removeMissionIndex;
            UplinkSnprintf ( bname, sizeof ( bname ), "hud_mission %d", removeMissionIndex );
        }

        // Add / remove any upgrade buttons

        if ( game->GetWorld ()->GetPlayer ()->gateway.HasHUDUpgrade ( HUDUPGRADE_CONNECTIONANALYSIS ) ) 
            if ( !IsUpgradeVisible ( HUDUPGRADE_CONNECTIONANALYSIS ) )
                    AddUpgrade ( HUDUPGRADE_CONNECTIONANALYSIS );

        if ( game->GetWorld ()->GetPlayer ()->gateway.HasHUDUpgrade ( HUDUPGRADE_IRCCLIENT ) ) 
            if ( !IsUpgradeVisible ( HUDUPGRADE_IRCCLIENT ) )
                    AddUpgrade ( HUDUPGRADE_IRCCLIENT );

        if ( game->GetWorld ()->GetPlayer ()->gateway.HasHUDUpgrade ( HUDUPGRADE_LANVIEW ) ) 
            if ( !IsUpgradeVisible ( HUDUPGRADE_LANVIEW ) )
                    AddUpgrade ( HUDUPGRADE_LANVIEW );

		// Update the world map and software menu

		wmi.Update ();
		si.Update ();

		// Update the task manager

		SvbUpdateInterface ();

	}

}

bool HUDInterface::IsVisible ()
{

	return ( EclGetButton ( "hud_software" ) != NULL );

}

int HUDInterface::ScreenID ()
{

	return SCREEN_HUD;

}

HUDInterface *HUDInterface::GetHUD ()
{

	return game->GetInterface ()->GetLocalInterface ()->GetHUD ();

}

bool HUDInterface::Load ( FILE *file )
{

	if ( strcmp( game->GetLoadedSavefileVer(), "SAV57" ) >= 0 ) {

		LoadID ( file );

		if ( !wmi.Load ( file ) ) return false;

		LoadID_END ( file );

	}

	return true;

}

void HUDInterface::Save ( FILE *file )
{

	SaveID ( file );
	
	wmi.Save ( file );

	SaveID_END ( file );

}

void HUDInterface::Print ()
{

}
	
char *HUDInterface::GetID ()
{
	
	return "HUD_INT";

}
