
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>


#include <stdio.h>

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"

#include "game/game.h"

#include "world/world.h"
#include "world/player.h"
#include "world/computer/computer.h"
#include "world/computer/computerscreen/menuscreen.h"
#include "world/computer/computerscreen/highsecurityscreen.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/highsecurityscreen_interface.h"

#include "mmgr.h"



HighSecurityScreenInterface::HighSecurityScreenInterface ()
{
}

HighSecurityScreenInterface::~HighSecurityScreenInterface ()
{
}

void HighSecurityScreenInterface::SystemTitleDraw ( Button *button, bool highlighted, bool clicked )
{

	int index;
	char unused [64];
	sscanf ( button->name, "%s %d", unused, &index );

	HighSecurityScreen *hs = (HighSecurityScreen *) game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
	UplinkAssert (hs);
	MenuScreenOption *mso = hs->systems.GetData (index);
	UplinkAssert (mso);


	clear_draw ( button->x + 1, button->y, button->width, button->height );

	// Draw 2 background lines

	if ( mso->security == 10 )	glColor3f ( 0.2f, 0.2f, 0.7f );
	else						glColor3f ( 0.0f, 0.0f, 0.3f );

	glLineWidth (2);

	glBegin ( GL_LINES );
		glVertex2i ( button->x, button->y + 10 );
		glVertex2i ( button->x + button->width, button->y + 10 );

		glVertex2i ( button->x, button->y + 15 );
		glVertex2i ( button->x + button->width, button->y + 15 );
	glEnd ();

	glLineWidth (1);

	// Write the text

	if ( mso->security == 10 )	glColor3f ( 1.0f, 1.0f, 1.0f );
	else						glColor3f ( 0.5f, 0.5f, 0.5f );
		
	GciDrawText ( button->x + 10, button->y + 18, button->caption, HELVETICA_18 );

}

void HighSecurityScreenInterface::SystemClick ( Button *button )
{

	int index;
	char unused [64];
	sscanf ( button->name, "%s %d", unused, &index );

	HighSecurityScreen *hs = (HighSecurityScreen *) game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
	UplinkAssert (hs);
	MenuScreenOption *mso = hs->systems.GetData (index);
	UplinkAssert (mso);

	int nextpage = mso->nextpage;
	if ( nextpage != -1 ) game->GetInterface ()->GetRemoteInterface ()->RunScreen ( nextpage, hs->GetComputer () );

}

void HighSecurityScreenInterface::BypassClick ( Button *button )
{

	// Give admin access
		
	game->GetInterface ()->GetRemoteInterface ()->SetSecurity ( "Admin", 1 );


	// Run the next screen

	HighSecurityScreen *hs = (HighSecurityScreen *) game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
	UplinkAssert (hs);

	if ( hs->nextpage != -1 )
		game->GetInterface ()->GetRemoteInterface ()->RunScreen ( hs->nextpage, hs->GetComputer () );

}

void HighSecurityScreenInterface::ProceedClick ( Button *button )
{

	UplinkAssert ( button );

	HighSecurityScreen *hs = (HighSecurityScreen *) game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
	UplinkAssert (hs);

	int worstlevel = 1;

	for ( int i = 0; i < hs->systems.Size (); ++i ) {
		if ( hs->systems.GetData(i)->security > worstlevel )
			worstlevel = hs->systems.GetData(i)->security;
	}

	if ( worstlevel < 10 ) {
		if ( hs->nextpage != -1 ) {
			
			game->GetInterface ()->GetRemoteInterface ()->SetSecurity ( "Valid", worstlevel );
			game->GetInterface ()->GetRemoteInterface ()->RunScreen ( hs->nextpage, hs->GetComputer () );
			hs->ResetSecurity ();

			game->GetWorld ()->GetPlayer ()->score_highsecurityhacks ++;

		}
	}

}

void HighSecurityScreenInterface::Create ( ComputerScreen *newcs )
{

	cs = newcs;

	if ( !IsVisible () ) {

		EclRegisterButton ( 80, 60, 350, 25, GetComputerScreen ()->maintitle, "", "highsecurity_maintitle" );
		EclRegisterButtonCallbacks ( "highsecurity_maintitle", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButton ( 80, 80, 350, 20, GetComputerScreen ()->subtitle, "", "highsecurity_subtitle" );
		EclRegisterButtonCallbacks ( "highsecurity_subtitle", DrawSubTitle, NULL, NULL, NULL );

		HighSecurityScreen *hs = GetComputerScreen ();
		UplinkAssert (hs);

		for ( int i = 0; i < hs->systems.Size (); ++i ) {
			
			MenuScreenOption *mso = hs->systems.GetData (i);
			UplinkAssert (mso);

			// Have we just finished bypassing this screen?
			// If so, store the result of the bypass
			// ie the security level gained

			if ( game->GetInterface ()->GetRemoteInterface ()->previousscreenindex == mso->nextpage &&
				 game->GetInterface ()->GetRemoteInterface ()->security_level < 10 ) {
				
				mso->security = game->GetInterface ()->GetRemoteInterface ()->security_level;
                
			}

            // 
            // Start a trace going if one of the systems is disabled
            //

            if ( mso->security != 10 ) {

                Computer *comp = hs->GetComputer ();
                UplinkAssert (comp);

	            if ( comp->security.IsRunning_Monitor () )
		            game->GetWorld ()->GetPlayer ()->GetConnection ()->BeginTrace ();

            }

			char title [64];
			char button [64];
			char status [64];
			
			UplinkSnprintf ( title, sizeof ( title ), "highsecurity_systemtitle %d", i );
			UplinkSnprintf ( button, sizeof ( button ), "highsecurity_systembutton %d", i );
			UplinkSnprintf ( status, sizeof ( status ), "highsecurity_systemstatus %d", i );

			EclRegisterButton ( 30, 140 + i * 40, 300, 25, mso->caption, "Click here to access this system", title );
			EclRegisterButtonCallbacks ( title, SystemTitleDraw, SystemClick, button_click, button_highlight );			

			EclRegisterButton ( 335, 140 + i * 40, 100, 20, "", "Click here to access this system", status );
			
			if ( mso->security == 10 )
				button_assignbitmap ( status, "accessdenied.tif" );

			else
				button_assignbitmap ( status, "accessgranted.tif" );

			EclRegisterButtonCallback ( status, SystemClick );

			EclRegisterButton ( 335, 350, 80, 15, " ", "Click here when done", "highsecurity_proceed" );
			button_assignbitmaps ( "highsecurity_proceed", "proceed.tif", "proceed_h.tif", "proceed_c.tif" );
			EclRegisterButtonCallback ( "highsecurity_proceed", ProceedClick );

#ifdef CHEATMODES_ENABLED
		// Create a symbol for quick entry into this screen
		EclRegisterButton ( 3, 20, 30, 15, "#", "Click here to bypass this screen (DEBUG MODE ONLY)", "highsecurity_bypass" );
		EclRegisterButtonCallbacks ( "highsecurity_bypass", textbutton_draw, BypassClick, button_click, button_highlight );
#endif		

		}

	}

}

void HighSecurityScreenInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "highsecurity_maintitle" );
		EclRemoveButton ( "highsecurity_subtitle" );

		HighSecurityScreen *hs = GetComputerScreen ();
		UplinkAssert (hs);

		for ( int i = 0; i < hs->systems.Size (); ++i ) {
			
			char title [64];
			char button [64];
			char status [64];
			
			UplinkSnprintf ( title, sizeof ( title ), "highsecurity_systemtitle %d", i );
			UplinkSnprintf ( button, sizeof ( button ), "highsecurity_systembutton %d", i );
			UplinkSnprintf ( status, sizeof ( status ), "highsecurity_systemstatus %d", i );

			EclRemoveButton ( title );
			EclRemoveButton ( button );
			EclRemoveButton ( status );

		}

		EclRemoveButton ( "highsecurity_proceed" );
		EclRemoveButton ( "highsecurity_bypass" );

	}

}

void HighSecurityScreenInterface::Update ()
{
}

bool HighSecurityScreenInterface::IsVisible ()
{

	return ( EclGetButton ( "highsecurity_maintitle" ) != NULL );

}

int HighSecurityScreenInterface::ScreenID ()
{

	return SCREEN_HIGHSECURITYSCREEN;

}

bool HighSecurityScreenInterface::ReturnKeyPressed ()
{
		
	return false;

}

HighSecurityScreen *HighSecurityScreenInterface::GetComputerScreen ()
{

	UplinkAssert (cs);
	return (HighSecurityScreen *) cs;

}
