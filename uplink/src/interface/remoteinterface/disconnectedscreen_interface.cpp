
#ifdef WIN32
#include <windows.h>
#endif

#include <strstream>

#include "eclipse.h"
#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"

#include "game/game.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/disconnectedscreen_interface.h"

#include "world/message.h"
#include "world/world.h"
#include "world/computer/computer.h"
#include "world/computer/computerscreen/disconnectedscreen.h"

#include "mmgr.h"


void DisconnectedScreenInterface::Click ( Button *button )
{

	int nextpage;
	char ip [ SIZE_VLOCATION_IP ] = {0};
	sscanf ( button->name, "disconnectedscreen_click %d %s", &nextpage, ip );

	Computer *comp = NULL;
	if ( ip ) {
		VLocation *loc = game->GetWorld ()->GetVLocation ( ip );
		if ( loc )
			comp = loc->GetComputer ();
	}
	if ( nextpage != -1 ) game->GetInterface ()->GetRemoteInterface ()->RunScreen ( nextpage, comp );

}

bool DisconnectedScreenInterface::ReturnKeyPressed ()
{

	if ( GetComputerScreen ()->nextpage != -1 )
		game->GetInterface ()->GetRemoteInterface ()->RunScreen ( GetComputerScreen ()->nextpage, GetComputerScreen ()->GetComputer () );

	return true;

}

void DisconnectedScreenInterface::Create ()
{

	if ( cs ) Create ( cs );
	else printf ( "DisconnectedScreenInterface::Create, tried to create when MessageScreen==NULL\n" );

}

void DisconnectedScreenInterface::Create ( ComputerScreen *newcs )
{

	UplinkAssert ( newcs );
	cs = newcs;

	if ( !IsVisible () ) {

		EclRegisterButton ( 80, 60, 350, 25, GetComputerScreen ()->maintitle, "", "disconnectedscreen_maintitle" );
		EclRegisterButtonCallbacks ( "disconnectedscreen_maintitle", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButton ( 80, 80, 350, 20, GetComputerScreen ()->subtitle, "", "disconnectedscreen_subtitle" );
		EclRegisterButtonCallbacks ( "disconnectedscreen_subtitle", DrawSubTitle, NULL, NULL, NULL );

		EclRegisterButton ( 50, 120, 400, 30, "", "", "disconnectedscreen_message" );
		EclRegisterButtonCallbacks ( "disconnectedscreen_message", textbutton_draw, NULL, NULL, NULL );
		EclRegisterCaptionChange   ( "disconnectedscreen_message", GetComputerScreen ()->textmessage, 2000 );

        if ( DisconnectedScreen::loginslost.Size () > 0 ) {

            EclRegisterButton ( 50, 180, 400, 200, "", "", "disconnectedscreen_loginslost" );
            EclRegisterButtonCallbacks ( "disconnectedscreen_loginslost", textbutton_draw, NULL, NULL, NULL );

            std::ostrstream loginslost;
            loginslost << "Your username and password was revoked on these systems:\n\n";

            for ( int i = 0; i < DisconnectedScreen::loginslost.Size (); ++i ) {

				if ( DisconnectedScreen::loginslost.ValidIndex ( i ) ) {

					char *ip = DisconnectedScreen::loginslost.GetData (i);
					UplinkAssert (ip);
					VLocation *vl = game->GetWorld ()->GetVLocation (ip);

					if ( vl ) {

						Computer *comp = vl->GetComputer ();
						UplinkAssert (comp);

						loginslost << "    - " << comp->name << "\n";

					}

				}

            }

            loginslost << '\x0';
            EclRegisterCaptionChange ( "disconnectedscreen_loginslost", loginslost.str (),  2000 );
            //delete [] loginslost.str ();

			loginslost.rdbuf()->freeze( 0 );

            DisconnectedScreen::ClearLoginsLost ();

        }

		int width = 100;
		char name [128 + SIZE_VLOCATION_IP + 1];
		UplinkSnprintf ( name, sizeof ( name ), "disconnectedscreen_click %d %s", GetComputerScreen ()->nextpage, GetComputerScreen ()->GetComputer ()->ip );
		EclRegisterButton ( 320 - width/2, 380, width, 20, "OK", "Click to close this screen", name );
		EclRegisterButtonCallback ( name, Click );

	}

}

void DisconnectedScreenInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "disconnectedscreen_message" );
        EclRemoveButton ( "disconnectedscreen_loginslost" );

		EclRemoveButton ( "disconnectedscreen_maintitle" );
		EclRemoveButton ( "disconnectedscreen_subtitle" );

		char name [128 + SIZE_VLOCATION_IP + 1];
		UplinkSnprintf ( name, sizeof ( name ), "disconnectedscreen_click %d %s", GetComputerScreen ()->nextpage, GetComputerScreen ()->GetComputer ()->ip );
		EclRemoveButton ( name );

	}

}

bool DisconnectedScreenInterface::IsVisible ()
{

	return ( EclGetButton ( "disconnectedscreen_message" ) != NULL );

}

DisconnectedScreen *DisconnectedScreenInterface::GetComputerScreen ()
{

	UplinkAssert (cs);
	return (DisconnectedScreen *) cs;

}

int DisconnectedScreenInterface::ScreenID ()
{

	return SCREEN_DISCONNECTEDSCREEN;

}
