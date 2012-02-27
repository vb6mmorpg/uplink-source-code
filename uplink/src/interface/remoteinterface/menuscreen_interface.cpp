
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

#include "eclipse.h"
#include "gucci.h"
#include "soundgarden.h"
#include "redshirt.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/miscutils.h"
#include "app/opengl_interface.h"

#include "game/game.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/menuscreen_interface.h"

#include "world/world.h"
#include "world/computer/computer.h"
#include "world/computer/computerscreen/menuscreen.h"

#include "mmgr.h"


void MenuScreenInterface::DrawMenuOption ( Button *button, bool highlighted, bool clicked )
{

    SetColour ( "MenuText" );
    GciDrawText ( button->x, button->y + 20, button->caption, HELVETICA_18 );

}

void MenuScreenInterface::DrawMenuOptionDimmed ( Button *button, bool highlighted, bool clicked )
{

	SetColour ( "DimmedText" );
	GciDrawText ( button->x, button->y + 20, button->caption, HELVETICA_18 );

}

void MenuScreenInterface::ClickMenuScreenOption ( Button *button )
{

	//Removed, wasn't heard when using mikmod
    //SgPlaySound ( RsArchiveFileOpen ("sounds/menuitemclick.wav"), "sounds/menuitemclick.wav", false );

	char text [32];
	int nextpage, unused;
	char ip [ SIZE_VLOCATION_IP ] = {0};
	sscanf ( button->name, "%s %d %d %s", text, &unused, &nextpage, ip );

	Computer *comp = NULL;
	if ( ip ) {
		VLocation *loc = game->GetWorld ()->GetVLocation ( ip );
		if ( loc )
			comp = loc->GetComputer ();
	}
	if ( nextpage != -1 ) game->GetInterface ()->GetRemoteInterface ()->RunScreen ( nextpage, comp );

}

void MenuScreenInterface::Create ( ComputerScreen *newcs )
{

	UplinkAssert ( newcs );
	cs = newcs;

	if ( !IsVisible () ) {
		
		EclRegisterButton ( 0, 0, 0, 0, "", "", "menuscreeninterface" );
		EclRegisterButtonCallbacks ( "menuscreeninterface", NULL, NULL, NULL, NULL );

		EclRegisterButton ( 80, 60, 350, 25, GetComputerScreen ()->maintitle, "", "menuscreen_maintitle" );
		EclRegisterButtonCallbacks ( "menuscreen_maintitle", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButton ( 80, 80, 350, 20, GetComputerScreen ()->subtitle, "", "menuscreen_subtitle" );
		EclRegisterButtonCallbacks ( "menuscreen_subtitle", DrawSubTitle, NULL, NULL, NULL );

		int timeinterval = (int) ( 1000.0 / GetComputerScreen ()->NumOptions () );

		Computer *comp = GetComputerScreen ()->GetComputer ();

		for ( int i = 0; i < GetComputerScreen ()->NumOptions (); ++i ) {

			if ( game->GetInterface ()->GetRemoteInterface ()->security_level <= GetComputerScreen ()->GetSecurity (i) ) {

				// You have clearance to view this option

				char bname1 [32 + SIZE_VLOCATION_IP + 1], bname2 [32 + SIZE_VLOCATION_IP + 1];
				UplinkSnprintf ( bname1, sizeof ( bname1 ), "menuscreenimage %d %d %s", i, GetComputerScreen ()->GetNextPage ( i ), comp->ip );
				UplinkSnprintf ( bname2, sizeof ( bname2 ), "menuscreentext %d %d %s", i, GetComputerScreen ()->GetNextPage ( i ), comp->ip );

				EclRegisterButton ( -350, 120 + i * 30, 16, 16, "", GetComputerScreen ()->GetTooltip ( i ), bname1 );
				button_assignbitmaps ( bname1, "menuscreenoption.tif", "menuscreenoption_h.tif", "menuscreenoption_c.tif" );
				EclRegisterButtonCallback ( bname1, ClickMenuScreenOption );

				EclRegisterButton ( -300, 120 + i * 30 - 5, 300, 32, GetComputerScreen ()->GetCaption ( i ), GetComputerScreen ()->GetTooltip ( i ), bname2 );
				EclRegisterButtonCallbacks ( bname2, DrawMenuOption, ClickMenuScreenOption, button_click, button_highlight );

				EclRegisterMovement ( bname1, 100, 130 + i * 30, (i+1) * timeinterval );
				EclRegisterMovement ( bname2, 140, 130 + i * 30 - 5, (i+1) * timeinterval );

			}
			else {

				// You don't have clearance to view this option

				char bname1 [32 + SIZE_VLOCATION_IP + 1], bname2 [32 + SIZE_VLOCATION_IP + 1];
				UplinkSnprintf ( bname1, sizeof ( bname1 ), "menuscreenimage %d %d %s", i, GetComputerScreen ()->GetNextPage ( i ), comp->ip );
				UplinkSnprintf ( bname2, sizeof ( bname2 ), "menuscreentext %d %d %s", i, GetComputerScreen ()->GetNextPage ( i ), comp->ip );

				EclRegisterButton ( -350, 120 + i * 30, 16, 16, "", GetComputerScreen ()->GetTooltip ( i ), bname1 );
				button_assignbitmap ( bname1, "menuscreenoption.tif" );
				EclGetButton ( bname1 )->image_standard->SetAlpha ( ALPHA_DISABLED );
				EclRegisterButtonCallbacks ( bname1, imagebutton_draw, NULL, NULL, NULL );

				EclRegisterButton ( -300, 120 + i * 30 - 5, 300, 32, GetComputerScreen ()->GetCaption ( i ), GetComputerScreen ()->GetTooltip ( i ), bname2 );
				EclRegisterButtonCallbacks ( bname2, DrawMenuOptionDimmed, NULL, NULL, NULL );

				EclRegisterMovement ( bname1, 100, 130 + i * 30, (i+1) * timeinterval );
				EclRegisterMovement ( bname2, 140, 130 + i * 30 - 5, (i+1) * timeinterval );


			}

		}
		
	}

}

void MenuScreenInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "menuscreeninterface" );

		EclRemoveButton ( "menuscreen_maintitle" );
		EclRemoveButton ( "menuscreen_subtitle" );

		Computer *comp = GetComputerScreen ()->GetComputer ();

		for ( int i = 0; i < GetComputerScreen ()->NumOptions (); ++i ) {

			char bname1 [32 + SIZE_VLOCATION_IP + 1], bname2 [32 + SIZE_VLOCATION_IP + 1];
			UplinkSnprintf ( bname1, sizeof ( bname1 ), "menuscreenimage %d %d %s", i, GetComputerScreen ()->GetNextPage ( i ), comp->ip );
			UplinkSnprintf ( bname2, sizeof ( bname2 ), "menuscreentext %d %d %s", i, GetComputerScreen ()->GetNextPage ( i ), comp->ip );
			
			EclRemoveButton ( bname1 );
			EclRemoveButton ( bname2 );

		}

	}

}

bool MenuScreenInterface::IsVisible ()
{

	return ( EclGetButton ( "menuscreeninterface" ) != NULL );

}

int MenuScreenInterface::ScreenID ()
{

	return SCREEN_MENUSCREEN;

}

MenuScreen *MenuScreenInterface::GetComputerScreen ()
{

	UplinkAssert (cs);
	return (MenuScreen *) cs;

}

