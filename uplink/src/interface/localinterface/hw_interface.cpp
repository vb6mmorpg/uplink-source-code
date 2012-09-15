
#ifdef WIN32
#include <windows.h>
#endif

#include <strstream>

#include <GL/gl.h>

#include <GL/glu.h>

#include "eclipse.h"
#include "soundgarden.h"
#include "redshirt.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/miscutils.h"
#include "app/opengl_interface.h"

#include "game/game.h"

#include "options/options.h"

#include "interface/interface.h"
#include "interface/localinterface/localinterface.h"
#include "interface/localinterface/hw_interface.h"
#include "interface/localinterface/hud_interface.h"
#include "interface/remoteinterface/remoteinterface.h"

#include "world/world.h"
#include "world/player.h"

#include "mmgr.h"


void HWInterface::TitleClick ( Button *button )
{

	game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_NONE );

}

void HWInterface::ShowGatewayClick ( Button *button )
{

	game->GetInterface ()->GetLocalInterface ()->GetHUD ()->gw.Create ();

}

void HWInterface::MiniTitleDraw ( Button *button, bool highlighted, bool clicked )
{

	SetColour ( "TitleText" );
	GciDrawText ( button->x + 10, button->y + 10, button->caption, HELVETICA_18 );

}

void HWInterface::CreateHWInterface ()
{

	if ( !IsVisibleHWInterface () ) {

		int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
		int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");
		//int paneltop = SX(100) + 30;
		int paneltop = (int) ( 100.0 * ( (screenw * PANELSIZE) / 188.0 ) + 30 );
		int panelwidth = (int) ( screenw * PANELSIZE );

		EclRegisterButton ( screenw - panelwidth, paneltop + 3, panelwidth - 7, 15, "GATEWAY", "Remove the gateway screen", "hw_title" );
		EclRegisterButtonCallback ( "hw_title", TitleClick );
		
		Gateway *gateway = &(game->GetWorld ()->GetPlayer ()->gateway);

		// Hardware installed

		EclRegisterButton ( screenw - panelwidth + 40, paneltop + 40, 80, 20, "Hardware", "", "hw_hardwaretitle" );
		EclRegisterButtonCallbacks ( "hw_hardwaretitle", MiniTitleDraw, NULL, NULL, NULL );

		EclRegisterButton ( screenw - panelwidth, paneltop + 70,  panelwidth - 10, 15, "", "", "hw_cpu" );
		EclRegisterButton ( screenw - panelwidth, paneltop + 85,  panelwidth - 10, 15, "", "", "hw_modem" );
		EclRegisterButton ( screenw - panelwidth, paneltop + 100, panelwidth - 10, 15, "", "", "hw_memory" );

		EclRegisterButtonCallbacks ( "hw_cpu",    text_draw, NULL, NULL, NULL );
		EclRegisterButtonCallbacks ( "hw_modem",  text_draw, NULL, NULL, NULL );
		EclRegisterButtonCallbacks ( "hw_memory", text_draw, NULL, NULL, NULL );

		// Total CPU speed

		char cpustats [64];
		UplinkSnprintf ( cpustats, sizeof ( cpustats ), "Total CPU speed: %d Ghz", gateway->GetCPUSpeed () );
		EclRegisterCaptionChange ( "hw_cpu",   cpustats );
	
		// Bandwidth

		char bandwidthstats [64];
		UplinkSnprintf ( bandwidthstats, sizeof ( bandwidthstats ), "Bandwidth: %d Gq/s", gateway->GetBandwidth () );
		EclRegisterCaptionChange ( "hw_modem", bandwidthstats );

		// Memory

		char memory [128];
		UplinkSnprintf ( memory, sizeof ( memory ), "Memory Capacity: %d Gq", gateway->memorysize );
		EclRegisterCaptionChange ( "hw_memory", memory );

		// Hardware devices

		std::ostrstream hardware;	
		hardware << "DEVICES\n";
        LList <char *> *security = gateway->GetSecurity ();
        if ( security->Size () == 0 ) 
            hardware << "None";

        else
            for ( int i = 0; i < security->Size (); ++i ) 
                if ( security->ValidIndex (i) ) 
		            hardware << security->GetData (i) << "\n";
        hardware << '\x0';

		EclRegisterButton ( screenw - panelwidth, paneltop + 130, panelwidth - 10, SY(150), "", "", "hw_hardware" );
		EclRegisterButtonCallbacks ( "hw_hardware", text_draw, NULL, NULL, NULL );
		EclRegisterCaptionChange ( "hw_hardware", hardware.str (), 1000 );
		
		hardware.rdbuf()->freeze( 0 );
		//delete [] hardware.str ();
        delete security;

		// HUD Upgrades installed

		EclRegisterButton ( screenw - panelwidth + 20, paneltop + SY(150) + 50, 80, 20, "HUD Upgrades", "", "hw_hudupgradestitle" );
		EclRegisterButtonCallbacks ( "hw_hudupgradestitle", MiniTitleDraw, NULL, NULL, NULL );

		EclRegisterButton ( screenw - panelwidth, paneltop + SY(150) + 70, panelwidth - 10, 60, "", "", "hw_hudupgrades" );
		EclRegisterButtonCallbacks ( "hw_hudupgrades", text_draw, NULL, NULL, NULL );

		std::ostrstream hudupgrades;

		if ( !(gateway->HasAnyHUDUpgrade ()) )								hudupgrades << "None\n";
		if ( gateway->HasHUDUpgrade ( HUDUPGRADE_MAPSHOWSTRACE ) )			hudupgrades << "Show trace on map\n";
		if ( gateway->HasHUDUpgrade ( HUDUPGRADE_CONNECTIONANALYSIS ) )		hudupgrades << "Connection analysis\n";
        if ( gateway->HasHUDUpgrade ( HUDUPGRADE_IRCCLIENT ) )              hudupgrades << "IRC Client\n";
        if ( gateway->HasHUDUpgrade ( HUDUPGRADE_LANVIEW ) )                hudupgrades << "LAN View\n";
        if ( gateway->HasHUDUpgrade ( HUDUPGRADE_KEYMAPPER ) )              hudupgrades << "Keyboard Mapper\n";

		hudupgrades << '\x0';

		EclRegisterCaptionChange ( "hw_hudupgrades", hudupgrades.str () );

		hudupgrades.rdbuf()->freeze( 0 );
		//delete [] hudupgrades.str ();

		// View gateway

		//EclRegisterButton ( screenw - panelwidth, paneltop + SY(282), panelwidth - 7, 15, "View Gateway", "Show the layout of your gateway", "hw_showgateway" );
		EclRegisterButton ( screenw - panelwidth, paneltop + ( SY(300) - 15 - 3 ), panelwidth - 7, 15, "View Gateway", "Show the layout of your gateway", "hw_showgateway" );
		EclRegisterButtonCallback ( "hw_showgateway", ShowGatewayClick );

	}
	
}

void HWInterface::RemoveHWInterface ()
{

	if ( IsVisibleHWInterface () ) {

		EclRemoveButton ( "hw_title" );
		
		EclRemoveButton ( "hw_hardwaretitle" );
		
		EclRemoveButton ( "hw_cpu" );
		EclRemoveButton ( "hw_modem" );
		EclRemoveButton ( "hw_memory" );
		EclRemoveButton ( "hw_hardware" );

		EclRemoveButton ( "hw_hudupgradestitle" );
		EclRemoveButton ( "hw_hudupgrades" );

		EclRemoveButton ( "hw_showgateway" );

	}

}

bool HWInterface::IsVisibleHWInterface ()
{

	return ( EclGetButton ( "hw_title" ) != 0 );

}

void HWInterface::Create ()
{
	
	LocalInterfaceScreen::Create ();
	CreateHWInterface ();
	
}

void HWInterface::Update ()
{
}

void HWInterface::Remove ()
{

	LocalInterfaceScreen::Remove ();
	RemoveHWInterface ();	

}

bool HWInterface::IsVisible ()
{

	return IsVisibleHWInterface ();

}

int HWInterface::ScreenID ()
{

	return SCREEN_HW;

}

