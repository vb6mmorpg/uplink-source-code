
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h>

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"

#include "game/game.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/securityscreen_interface.h"

#include "world/world.h"
#include "world/player.h"
#include "world/computer/computer.h"
#include "world/computer/securitysystem.h"
#include "world/computer/computerscreen/genericscreen.h"

#include "mmgr.h"

time_t SecurityScreenInterface::lastupdate = 0;



void SecurityScreenInterface::SystemTitleDraw ( Button *button, bool highlighted, bool clicked )
{

	clear_draw ( button->x + 1, button->y, button->width, button->height );

	// Get the security system in question

	int index;
	char unused [64];
	sscanf ( button->name, "%s %d", unused, &index );

	Computer *comp = game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ()->GetComputer ();
	UplinkAssert (comp);
	SecuritySystem *ss = comp->security.GetSystem (index);
	UplinkAssert (ss);

	// Draw 2 background lines

	if ( ss->enabled ) glColor3f ( 0.2f, 0.2f, 0.7f );
	else			   glColor3f ( 0.0f, 0.0f, 0.3f );

	glLineWidth (2);

	glBegin ( GL_LINES );
		glVertex2i ( button->x, button->y + 10 );
		glVertex2i ( button->x + button->width, button->y + 10 );

		glVertex2i ( button->x, button->y + 15 );
		glVertex2i ( button->x + button->width, button->y + 15 );
	glEnd ();

	glLineWidth (1);

	// Write the text

	if ( ss->enabled )	glColor3f ( 1.0f, 1.0f, 1.0f );
	else				glColor3f ( 0.5f, 0.5f, 0.5f );
	
	GciDrawText ( button->x + 10, button->y + 18, button->caption, HELVETICA_18 );

}

void SecurityScreenInterface::StatusClick ( Button *button )
{

	int index;
	char unused [64];
	sscanf ( button->name, "%s %d", unused, &index );

	Computer *comp = game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ()->GetComputer ();
	UplinkAssert (comp);
	SecuritySystem *ss = comp->security.GetSystem (index);
	UplinkAssert (ss);

	if ( comp->security.IsRunning_Proxy () ) {
		create_msgbox ( "Error", "Denied access by Proxy" );
		return;
	}

	if ( ss->enabled )  ss->Disable ();
	else				ss->Enable ();

	if ( comp->security.IsRunning_Monitor () ) 
		game->GetWorld ()->GetPlayer ()->connection.BeginTrace ();

	lastupdate = 0;							// Force update now

}

void SecurityScreenInterface::CloseClick ( Button *button )
{

    GenericScreen *gs = (GenericScreen *) game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
    UplinkAssert (gs);

	if ( gs->nextpage != -1 ) 
        game->GetInterface ()->GetRemoteInterface ()->RunScreen ( gs->nextpage, gs->GetComputer () );

}

bool SecurityScreenInterface::EscapeKeyPressed ()
{

    CloseClick (NULL);
    return true;

}

void SecurityScreenInterface::Create ( ComputerScreen *newcs )
{

	UplinkAssert (newcs);
	cs = newcs;

	if ( !IsVisible () ) {

		EclRegisterButton ( 80, 60, 350, 25, GetComputerScreen ()->maintitle, "", "securityscreen_maintitle" );
		EclRegisterButtonCallbacks ( "securityscreen_maintitle", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButton ( 80, 80, 350, 20, GetComputerScreen ()->subtitle, "", "securityscreen_subtitle" );
		EclRegisterButtonCallbacks ( "securityscreen_subtitle", DrawSubTitle, NULL, NULL, NULL );

		Computer *comp = game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ()->GetComputer ();
		UplinkAssert (comp);

		if ( comp->security.NumSystems () == 0 ) {

			EclRegisterButton ( 200, 100, 200, 20, "No security systems installed", "", "securityscreen_none" );
			EclRegisterButtonCallbacks ( "securityscreen_none", textbutton_draw, NULL, NULL, NULL );

		}
		else {

			for ( int i = 0; i < comp->security.NumSystems (); ++i ) {
				
				SecuritySystem *ss = comp->security.GetSystem (i);

				if ( ss ) {
		
					char title [64];
					char level [64];
					char status [64];
					
					UplinkSnprintf ( title, sizeof ( title ), "securityscreen_systemtitle %d", i );
					UplinkSnprintf ( level, sizeof ( level ), "securityscreen_systemlevel %d", i );
					UplinkSnprintf ( status, sizeof ( status ), "securityscreen_systemstatus %d", i );

					char leveltext [8];
					UplinkSnprintf ( leveltext, sizeof ( leveltext ), "level %d", ss->level );

					EclRegisterButton ( 50, 140 + i * 40, 150, 25, ss->GetName (), "Click here to toggle its status", title );
					EclRegisterButton ( 200, 140 + i * 40, 100, 25, leveltext, "Click here to toggle its status", level );
					EclRegisterButton ( 300, 140 + i * 40, 100, 25, "", "Click here to toggle its status", status );
					
					EclRegisterButtonCallbacks ( title, SystemTitleDraw, StatusClick, button_click, button_highlight );
					EclRegisterButtonCallbacks ( level, SystemTitleDraw, StatusClick, button_click, button_highlight );
					EclRegisterButtonCallbacks ( status, SystemTitleDraw, StatusClick, button_click, button_highlight );
					
				}

			}

		}

		// Create the close button

		EclRegisterButton ( 320, 370, 80, 20, "Close", "Close the security screen", "securityscreen_close" );
		EclRegisterButtonCallback ( "securityscreen_close", CloseClick );

	}

}

void SecurityScreenInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "securityscreen_maintitle" );
		EclRemoveButton ( "securityscreen_subtitle" );
		EclRemoveButton ( "securityscreen_none" );

		Computer *comp = game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ()->GetComputer ();
		UplinkAssert (comp);

		for ( int i = 0; i < comp->security.NumSystems (); ++i ) {
			
			char title [64];
			char level [64];
			char status [64];
			
			UplinkSnprintf ( title, sizeof ( title ), "securityscreen_systemtitle %d", i );
			UplinkSnprintf ( level, sizeof ( level ), "securityscreen_systemlevel %d", i );
			UplinkSnprintf ( status, sizeof ( status ), "securityscreen_systemstatus %d", i );

			EclRemoveButton ( title );
			EclRemoveButton ( level );
			EclRemoveButton ( status );

		}

        EclRemoveButton ( "securityscreen_close" );

	}
		
}

void SecurityScreenInterface::Update ()
{

	if ( time(NULL) > lastupdate + 5 ) {

		if ( IsVisible () ) {

			Computer *comp = game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ()->GetComputer ();
			UplinkAssert (comp);

			for ( int i = 0; i < comp->security.NumSystems (); ++i ) {
				
				SecuritySystem *ss = comp->security.GetSystem (i);

				if ( ss ) {

					char status [64];
					UplinkSnprintf ( status, sizeof ( status ), "securityscreen_systemstatus %d", i );

					if ( EclGetButton ( status ) ) {
				
						EclGetButton ( status )->SetCaption ( ss->enabled ? (char *) "Enabled" : (char *) "Disabled" );

					}
					
				}

				char title [64];
				char level [64];
				char status [64];
				
				UplinkSnprintf ( title, sizeof ( title ), "securityscreen_systemtitle %d", i );
				UplinkSnprintf ( level, sizeof ( level ), "securityscreen_systemlevel %d", i );
				UplinkSnprintf ( status, sizeof ( status ), "securityscreen_systemstatus %d", i );

				EclDirtyButton ( title );
				EclDirtyButton ( level );
				EclDirtyButton ( status );

			}

		}

		lastupdate = time(NULL);

	}

}

bool SecurityScreenInterface::IsVisible ()
{

	return ( EclGetButton ( "securityscreen_maintitle" ) != NULL );

}


int SecurityScreenInterface::ScreenID ()
{

	return SCREEN_SECURITYSCREEN;

}

GenericScreen *SecurityScreenInterface::GetComputerScreen ()
{

	UplinkAssert (cs);

	return (GenericScreen *) cs;

}

