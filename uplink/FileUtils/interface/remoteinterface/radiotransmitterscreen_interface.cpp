
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h>

#include "app/app.h"
#include "app/globals.h"
#include "app/miscutils.h"
#include "app/opengl_interface.h"

#include "game/game.h"
#include "game/scriptlibrary.h"
#include "game/data/data.h"

#include "world/world.h"
#include "world/vlocation.h"
#include "world/player.h"
#include "world/computer/computer.h"
#include "world/computer/lancomputer.h"
#include "world/computer/lanmonitor.h"

#include "interface/interface.h"
#include "interface/localinterface/localinterface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/radiotransmitterscreen_interface.h"

#include "mmgr.h"

int RadioTransmitterScreenInterface::frequencyGhz = 160;
int RadioTransmitterScreenInterface::frequencyMhz = 0;


void RadioTransmitterScreenInterface::BackgroundDraw ( Button *button, bool highlighted, bool clicked )
{

    glColor3f ( 0.0f, 0.0f, 0.0f );

	glBegin ( GL_QUADS );		
		glVertex2i ( button->x, button->y + button->height );
		glVertex2i ( button->x, button->y );
		glVertex2i ( button->x + button->width, button->y );
		glVertex2i ( button->x + button->width, button->y + button->height );
	glEnd ();

	SetColour ( "PanelBorder" );
	border_draw ( button );

}

void RadioTransmitterScreenInterface::FrequencyDraw ( Button *button, bool highlighted, bool clicked )
{

    textbutton_draw ( button, highlighted, clicked );
    SetColour ( "PanelBorder" );
    border_draw ( button );

    char frequency[64];
    UplinkSnprintf ( frequency, sizeof ( frequency ), "%d.%d Ghz", frequencyGhz, frequencyMhz );

    GciDrawText ( button->x + 10, button->y + 10, frequency );

}

void RadioTransmitterScreenInterface::DownClick ( Button *button )
{

    frequencyMhz -= 1;

    if ( frequencyMhz < 0 ) {
        frequencyMhz = 9;
        frequencyGhz -= 1;
    }

    if ( frequencyGhz < RADIOTRANSMITTER_MINRANGE ) {
        frequencyGhz = RADIOTRANSMITTER_MINRANGE;
        frequencyMhz = 0;
    }

    EclDirtyButton ( "radioscreen_frequency" );

}

void RadioTransmitterScreenInterface::BigDownClick ( Button *button )
{
    
    frequencyGhz -= 1;

    if ( frequencyGhz < RADIOTRANSMITTER_MINRANGE ) {
        frequencyGhz = RADIOTRANSMITTER_MINRANGE;
        frequencyMhz = 0;
    }

    EclDirtyButton ( "radioscreen_frequency" );    

}

void RadioTransmitterScreenInterface::UpClick ( Button *button )
{

    frequencyMhz += 1;

    if ( frequencyMhz > 9 ) {
        frequencyMhz = 0;
        frequencyGhz += 1;
    }

    if ( frequencyGhz > RADIOTRANSMITTER_MAXRANGE ) {
        frequencyGhz = RADIOTRANSMITTER_MAXRANGE;
        frequencyMhz = 9;
    }

    EclDirtyButton ( "radioscreen_frequency" );

}

void RadioTransmitterScreenInterface::BigUpClick ( Button *button )
{
    
    frequencyGhz += 1;

    if ( frequencyGhz > RADIOTRANSMITTER_MAXRANGE ) {
        frequencyGhz = RADIOTRANSMITTER_MAXRANGE;
        frequencyMhz = 9;
    }

    EclDirtyButton ( "radioscreen_frequency" );    

}

void RadioTransmitterScreenInterface::ConnectClick ( Button *button )
{

    ScriptLibrary::RunScript( 85 );

}

bool RadioTransmitterScreenInterface::ReturnKeyPressed ()
{
    ConnectClick ( NULL );
    return true;
}

void RadioTransmitterScreenInterface::Create ()
{

	if ( cs ) Create ( cs );
	else printf ( "RadioTransmitterScreenInterface::Create, tried to create when GenericScreen==NULL\n" );

}

void RadioTransmitterScreenInterface::Create ( ComputerScreen *newcs )
{

    if ( !IsVisible () ) {

        UplinkAssert (newcs);
        cs = newcs;

		EclRegisterButton ( 80, 60, 350, 25, GetComputerScreen ()->maintitle, "", "radioscreen_maintitle" );
		EclRegisterButtonCallbacks ( "radioscreen_maintitle", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButton ( 80, 80, 350, 20, GetComputerScreen ()->subtitle, "", "radioscreen_subtitle" );
		EclRegisterButtonCallbacks ( "radioscreen_subtitle", DrawSubTitle, NULL, NULL, NULL );

        EclRegisterButton ( 100, 120, 300, 100, "Wireless LAN transmitter online\nReady to establish Uplink.", " ", "radioscreen_text" );
        EclRegisterButtonCallbacks ( "radioscreen_text", textbutton_draw, NULL, NULL, NULL );

        //
        // Frequency selection dialog

        EclRegisterButton ( 160, 220, 220, 140, " ", " ", "radioscreen_background" );
        EclRegisterButtonCallbacks ( "radioscreen_background", BackgroundDraw, NULL, NULL, NULL );

        EclRegisterButton ( 170, 223, 220, 30, "Transmitter Frequency", " ", "radioscreen_bigtext" );
        EclRegisterButtonCallbacks ( "radioscreen_bigtext", DrawMainTitle, NULL, NULL, NULL );

        EclRegisterButton ( 220, 260, 100, 20, " ", "Enter desired frequency here", "radioscreen_frequency" );
        EclRegisterButtonCallbacks ( "radioscreen_frequency", FrequencyDraw, NULL, NULL, NULL );
        EclMakeButtonEditable ( "radioscreen_frequency" );

        EclRegisterButton ( 200, 260, 20, 20, "<", "Scan lower frequencies", "radioscreen_down" );
        EclRegisterButton ( 180, 260, 20, 20, "<<", "Scan much lower frequencies", "radioscreen_bigdown" );
        EclRegisterButton ( 320, 260, 20, 20, ">", "Scan higher frequencies", "radioscreen_up" );
        EclRegisterButton ( 340, 260, 20, 20, ">>", "Scan much higher frequencies", "radioscreen_bigup" );
        
        EclRegisterButtonCallback ( "radioscreen_bigdown", BigDownClick );
        EclRegisterButtonCallback ( "radioscreen_down", DownClick );
        EclRegisterButtonCallback ( "radioscreen_bigup", BigUpClick );
        EclRegisterButtonCallback ( "radioscreen_up", UpClick );

        EclRegisterButton ( 180, 300, 180, 15, "Establish uplink", "Click to establish the radio link", "radioscreen_connect" );
        EclRegisterButtonCallback ( "radioscreen_connect", ConnectClick );
        
        EclRegisterButton ( 180, 320, 180, 15, "Not Connected", " ", "radioscreen_statustext" );
        EclRegisterButtonCallbacks ( "radioscreen_statustext", textbutton_draw, NULL, NULL, NULL );
        
    }

}

void RadioTransmitterScreenInterface::Remove ()
{

    if ( IsVisible () ) {

        EclRemoveButton ( "radioscreen_maintitle" );
        EclRemoveButton ( "radioscreen_subtitle" );

        EclRemoveButton ( "radioscreen_background" );
        EclRemoveButton ( "radioscreen_bigtext" );
        EclRemoveButton ( "radioscreen_text" );
        EclRemoveButton ( "radioscreen_down" );
        EclRemoveButton ( "radioscreen_bigdown" );
        EclRemoveButton ( "radioscreen_frequency" );
        EclRemoveButton ( "radioscreen_up" );
        EclRemoveButton ( "radioscreen_bigup" );

        EclRemoveButton ( "radioscreen_connect" );
        EclRemoveButton ( "radioscreen_statustext" );

    }

}

void RadioTransmitterScreenInterface::Update ()
{

}

bool RadioTransmitterScreenInterface::IsVisible ()
{
    return ( (bool) EclGetButton ( "radioscreen_maintitle" ) );
}

int RadioTransmitterScreenInterface::ScreenID ()
{
    return SCREEN_RADIOTRANSMITTER;
}

GenericScreen *RadioTransmitterScreenInterface::GetComputerScreen ()
{
    UplinkAssert (cs);
    return (GenericScreen *) cs;
}

void RadioTransmitterScreenInterface::Connect ()
{

    //
    // Search for a radio receiver at our frequency

    char *remotehost = game->GetWorld()->GetPlayer()->remotehost;
  	VLocation *vl = game->GetWorld ()->GetVLocation ( remotehost );
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);

    if ( comp->TYPE == COMPUTER_TYPE_LAN )
    {

        LanComputer *lc = (LanComputer *) comp;
        
        for ( int i = 0; i < lc->systems.Size(); ++i ) {
            if ( lc->systems.ValidIndex(i) ) {

                LanComputerSystem *lcs = lc->systems.GetData(i);
                UplinkAssert (lcs);

                if ( lcs->TYPE == LANSYSTEM_RADIORECEIVER &&
                     lcs->data1 == frequencyGhz &&
                     lcs->data2 == frequencyMhz ) {

                    // Found our target system
                    
                    LanMonitor::ForceExtendConnection ( i );
        		    LanMonitor::SetCurrentSystem ( i );
			
                    //
                    // Show the screen attached to this system

			        if ( lc->systems.ValidIndex(i) ) {

				        LanComputerSystem *system = lc->systems.GetData(i);
				        if ( system->screenIndex != -1 ) {
					        game->GetInterface()->GetRemoteInterface()->RunScreen( system->screenIndex, lc );
					        game->GetInterface()->GetLocalInterface()->RunScreen( SCREEN_NONE );
                            return;
				        }
					        
			        }

                }

            }
        }

    }

    //
    // If we get here then our connection attempt failed

    EclRegisterCaptionChange ( "radioscreen_statustext", "Not Connected", 0 );

}
