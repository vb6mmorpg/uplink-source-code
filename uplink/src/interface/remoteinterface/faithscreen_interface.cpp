
#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"

#include "game/game.h"

#include "world/world.h"
#include "world/computer/computerscreen/genericscreen.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/faithscreen_interface.h"

#include "mmgr.h"


void FaithScreenInterface::CloseClick ( Button *button )
{

    GenericScreen *gs = (GenericScreen *) game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
    UplinkAssert (gs);

	if ( gs->nextpage != -1 ) 
        game->GetInterface ()->GetRemoteInterface ()->RunScreen ( gs->nextpage, gs->GetComputer () );

}

FaithScreenInterface::FaithScreenInterface ()
{
}

FaithScreenInterface::~FaithScreenInterface ()
{
}

bool FaithScreenInterface::EscapeKeyPressed ()
{

    CloseClick ( NULL );
    return true;

}

void FaithScreenInterface::Create ()
{

	if ( cs ) Create ( cs );
	else printf ( "FaithScreenInterface::Create, tried to create when GenericScreen==NULL\n" );

}

void FaithScreenInterface::Create ( ComputerScreen *newcs )
{

	UplinkAssert ( newcs );
	cs = newcs;

	if ( !IsVisible () ) {

		// Create the titles

		EclRegisterButton ( 80, 60, 350, 25, GetComputerScreen ()->maintitle, "", "faithscreen_maintitle" );
		EclRegisterButtonCallbacks ( "faithscreen_maintitle", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButton ( 80, 80, 350, 20, GetComputerScreen ()->subtitle, "", "faithscreen_subtitle" );
		EclRegisterButtonCallbacks ( "faithscreen_subtitle", DrawSubTitle, NULL, NULL, NULL );


		// Create the information boxes

		char faith [128];
		char revelation [128];
		
		UplinkSnprintf ( faith, sizeof ( faith ), "Version %1.1f", game->GetWorld ()->plotgenerator.GetVersion_Faith () );
		UplinkSnprintf ( revelation, sizeof ( revelation ), "Version %1.1f", game->GetWorld ()->plotgenerator.GetVersion_Revelation () );

		EclRegisterButton ( 100, 150, 100, 30, "Faith", "", "faithscreen_faithtitle" );
		EclRegisterButton ( 100, 200, 100, 30, "Revelation", "", "faithscreen_revelationtitle" );

		EclRegisterButton ( 250, 150, 200, 30, faith, "", "faithscreen_faith" );
		EclRegisterButton ( 250, 200, 200, 30, revelation, "", "faithscreen_revelation" );

		EclRegisterButtonCallbacks ( "faithscreen_faithtitle",		DrawSubTitle, NULL, NULL, NULL );
		EclRegisterButtonCallbacks ( "faithscreen_faith",			DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButtonCallbacks ( "faithscreen_revelationtitle", DrawSubTitle, NULL, NULL, NULL );
		EclRegisterButtonCallbacks ( "faithscreen_revelation",		DrawMainTitle, NULL, NULL, NULL );
		
		EclRegisterButton ( 90, 260, 300, 110, "", "", "faithscreen_text" );
		EclRegisterButtonCallbacks ( "faithscreen_text", textbutton_draw, NULL, NULL, NULL );
		EclRegisterCaptionChange ( "faithscreen_text",	"This information is based on unofficial reports from the Web.  "
														"No guarantee is given as to the accuracy of this information.\n\n"
														"If you have any information which may help to keep this up to date, "
														"please send it to internal@Uplink.net.", 2000 );

		// Create the close button

		EclRegisterButton ( 421, 121, 13, 13, "", "Close the Faith Progress Screen", "faithscreen_close" );
		button_assignbitmaps ( "faithscreen_close", "close.tif", "close_h.tif", "close_c.tif" );
		EclRegisterButtonCallback ( "faithscreen_close", CloseClick );
		
	}

}

void FaithScreenInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "faithscreen_maintitle" );
		EclRemoveButton ( "faithscreen_subtitle" );

		EclRemoveButton ( "faithscreen_faithtitle" );
		EclRemoveButton ( "faithscreen_faith" );
		EclRemoveButton ( "faithscreen_revelationtitle" );
		EclRemoveButton ( "faithscreen_revelation" );

		EclRemoveButton ( "faithscreen_text" );
		EclRemoveButton ( "faithscreen_close" );

	}

}

void FaithScreenInterface::Update ()
{
}

bool FaithScreenInterface::IsVisible ()
{

	return ( EclGetButton ( "faithscreen_maintitle" ) != NULL );

}

int FaithScreenInterface::ScreenID ()
{

	return SCREEN_FAITHSCREEN;

}

GenericScreen *FaithScreenInterface::GetComputerScreen ()
{

	UplinkAssert (cs);
	return (GenericScreen *) cs;

}

