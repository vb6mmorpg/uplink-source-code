
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h>


#include "app/globals.h"
#include "app/app.h"
#include "app/opengl_interface.h"

#include "options/options.h"

#include "game/game.h"
#include "game/data/data.h"

#include "world/world.h"
#include "world/player.h"
#include "world/vlocation.h"
#include "world/computer/computer.h"
#include "world/computer/computerscreen/genericscreen.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/auxgatewayscreen_interface.h"
#include "interface/localinterface/localinterface.h"

#include "mmgr.h"


AuxGatewayScreenInterface::AuxGatewayScreenInterface ()
{
}

AuxGatewayScreenInterface::~AuxGatewayScreenInterface ()
{
}

void AuxGatewayScreenInterface::DrawBlack ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert (button);
	clear_draw ( button->x, button->y, button->width, button->height );

}

void AuxGatewayScreenInterface::DrawLocation ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert (button);

    glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );

    glBegin ( GL_QUADS );
        glVertex2i ( button->x, button->y );
        glVertex2i ( button->x + 7, button->y );
        glVertex2i ( button->x + 7, button->y + 7 );
        glVertex2i ( button->x, button->y +7 );
    glEnd ();

	// Write some text

	Computer *comp = game->GetWorld ()->GetComputer ( button->caption );
	UplinkAssert(comp);
	
	VLocation *vl = game->GetWorld ()->GetVLocation ( comp->ip );
	UplinkAssert(vl);

	GciDrawText ( button->x - 10, button->y + 20, button->caption );
	GciDrawText ( button->x - 10, button->y + 30, vl->ip );

}

void AuxGatewayScreenInterface::DrawMainMap ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert (button);

	imagebutton_drawtextured ( button, highlighted, clicked );

	glColor3ub ( 81, 138, 215 );
	border_draw ( button );

}

void AuxGatewayScreenInterface::ClickLocation ( Button *button )
{

	// Set the players physical location 

	Computer *comp = game->GetWorld ()->GetComputer ( button->caption );
	UplinkAssert(comp);
	
	VLocation *vl = game->GetWorld ()->GetVLocation ( comp->ip );
	UplinkAssert(vl);

	VLocation *localvl = game->GetWorld ()->GetPlayer ()->GetLocalHost ();
	UplinkAssert(localvl);

	Computer *localhost = localvl->GetComputer();
	UplinkAssert(localhost);

	game->GetWorld ()->GetPlayer ()->connection.Disconnect();
	game->GetWorld ()->GetPlayer ()->SetLocalHost (vl->ip);

	localhost->SetIP(vl->ip);

	VLocation *newvl = new VLocation ();
	newvl->SetIP(vl->ip);
	newvl->SetComputer(localvl->computer);
	newvl->SetPLocation ( vl->x, vl->y );
	newvl->SetListed (false);
	newvl->SetDisplayed(true);

	game->GetWorld ()->GetPlayer ()->gateway.GiveStartingHardware(comp->name);
	game->GetWorld ()->GetPlayer ()->gateway.GiveStartingSoftware(comp->name);

	game->GetWorld ()->GetPlayer ()->connection.Disconnect(); // Does some magic that makes sure connection is valid
	game->GetWorld ()->GetPlayer ()->connection.Reset();

	// Is this safe?
	game->GetWorld ()->computers.RemoveData(comp->name);
	game->GetWorld ()->locations.RemoveData(vl->ip);
	game->GetWorld ()->locations.RemoveData(localvl->ip);
	game->GetWorld ()->GetPlayer ()->RemoveLink(vl->ip); // The links get a little messed up otherwise
	game->GetWorld ()->CreateVLocation(newvl);

	// Go on to the next screen

	game->GetInterface ()->GetRemoteInterface ()->RunNewLocation ();
	game->GetInterface ()->GetRemoteInterface ()->RunScreen ( 3 );
    game->GetInterface ()->GetLocalInterface ()->Create ();


}

bool AuxGatewayScreenInterface::ReturnKeyPressed ()
{
	
	return false;

}

void AuxGatewayScreenInterface::Create ( ComputerScreen *newcs )
{

	cs = newcs;
 
	if ( !IsVisible () ) {

		EclRegisterButton ( 40, 10, 350, 25, GetComputerScreen ()->subtitle, "", "nearestgateway_maintitle" );
		EclRegisterButtonCallbacks ( "nearestgateway_maintitle", DrawMainTitle, NULL, NULL, NULL );

		// Black out the computer picture

		EclRegisterButton ( 470, 280, 140, 140, " ", " ", "nearestgateway_black" );
		EclRegisterButtonCallbacks ( "nearestgateway_black", DrawBlack, NULL, NULL, NULL );

		// Work out the size/ratios of the map

		int screenw = app->GetOptions ()->GetOptionValue ( "graphics_screenwidth" );
	    int screenh = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );

		int x1 = 23;
		int y1 = 50;
		int fullsizeX = screenw - x1 * 2;
		int fullsizeY = (int) ( 316.0 * ( fullsizeX / 595.0 ) );

		// Create the large map graphic

		int numPhysicalGatewayLocations;
		EclRegisterButton ( x1, y1, fullsizeX, fullsizeY, "", "", "nearestgateway_largemap" );                                               
		if ( game->GetWorldMapType () == Game::defconworldmap ) {
			numPhysicalGatewayLocations = NUM_PHYSICALGATEWAYLOCATIONS_DEFCON;
			button_assignbitmap ( "nearestgateway_largemap", "worldmaplarge_defcon.tif" );
		}
		else {
			numPhysicalGatewayLocations = NUM_PHYSICALGATEWAYLOCATIONS;
			button_assignbitmap ( "nearestgateway_largemap", "worldmaplarge.tif" );
		}
		EclRegisterButtonCallbacks ( "nearestgateway_largemap", DrawMainMap, NULL, NULL, NULL );
    
		// Create a button for each gateway

	DArray <Computer *> *computers = game->GetWorld ()->computers.ConvertToDArray ();
	UplinkAssert (computers);

	int currentIndex = 0;
	for ( int i = 0; i < computers->Size (); ++i ) {

		if ( computers->ValidIndex ( i ) ) {

			Computer *computer = computers->GetData (i);
			UplinkAssert (computer);

			if ( strcmp( computer->companyname, "Player") == 0 &&
					strcmp( computer->ip, game->GetWorld ()->GetPlayer ()->localhost ) != 0 )
			{
				VLocation *vl = game->GetWorld ()->GetVLocation (computer->ip);
				UplinkAssert(vl);

				char bname [64];
				UplinkSnprintf ( bname, sizeof ( bname ), "nearestgateway_location %d", currentIndex );

				char tooltip [128];
				UplinkSnprintf ( tooltip, sizeof ( tooltip ), "Select %s as your local Gateway", computer->name );

				//EclRegisterButton ( x1 + SX(pgl->x), y1 + SY(pgl->y), 7, 7, pgl->city, tooltip, bname );
				EclRegisterButton ( x1 + (int) ( ( (float) vl->x / (float) VIRTUAL_WIDTH ) * fullsizeX ), y1 + (int) ( ( (float) vl->y / (float) VIRTUAL_HEIGHT ) * fullsizeY ), 7, 7, computer->name, tooltip, bname );
				EclRegisterButtonCallbacks ( bname, DrawLocation, ClickLocation, button_click, button_highlight );
				currentIndex++;
			}

		}

	}

    delete computers;
	
	//for ( int i = 0; i < numPhysicalGatewayLocations; ++i ) {

	//		const PhysicalGatewayLocation *pgl;
	//		if ( game->GetWorldMapType () == Game::defconworldmap )
	//			pgl = &(PHYSICALGATEWAYLOCATIONS_DEFCON[i]);
	//		else
	//			pgl = &(PHYSICALGATEWAYLOCATIONS[i]);
	//		UplinkAssert (pgl);

	//		char bname [64];
	//		UplinkSnprintf ( bname, sizeof ( bname ), "nearestgateway_location %d", i );

	//		char tooltip [128];
	//		UplinkSnprintf ( tooltip, sizeof ( tooltip ), "Select %s as your local Gateway", pgl->city );

	//		//EclRegisterButton ( x1 + SX(pgl->x), y1 + SY(pgl->y), 7, 7, pgl->city, tooltip, bname );
	//		EclRegisterButton ( x1 + (int) ( ( (float) pgl->x / (float) VIRTUAL_WIDTH ) * fullsizeX ), y1 + (int) ( ( (float) pgl->y / (float) VIRTUAL_HEIGHT ) * fullsizeY ), 7, 7, pgl->city, tooltip, bname );
	//		EclRegisterButtonCallbacks ( bname, DrawLocation, ClickLocation, button_click, button_highlight );

	//	}

		// Create some helpfull text

		EclRegisterButton ( 50, y1 + fullsizeY + 20, 400, 70, " ", " ", "nearestgateway_helptext" );
		EclRegisterButtonCallbacks ( "nearestgateway_helptext", textbutton_draw, NULL, NULL, NULL );
		EclRegisterCaptionChange ( "nearestgateway_helptext", "Uplink records indicate that you have one or more additional\n"
															  "gateways located around the world. Please select the gateway\n"
															  "you wish to connect to.", 2000 );

	}

}

void AuxGatewayScreenInterface::Remove ()
{

	if ( IsVisible () ) {
		
		EclRemoveButton ( "nearestgateway_maintitle" );
		EclRemoveButton ( "nearestgateway_black" );
		EclRemoveButton ( "nearestgateway_largemap" );

		int i = 0;
		char bname [64];
		UplinkSnprintf ( bname, sizeof ( bname ), "nearestgateway_location %d", i );

		while ( EclGetButton(bname) )
		{
			EclRemoveButton ( bname );
			i++;
			UplinkSnprintf ( bname, sizeof ( bname ), "nearestgateway_location %d", i );
		}
		EclRemoveButton ( "nearestgateway_helptext" );

	}

}

bool AuxGatewayScreenInterface::IsVisible ()
{
	
	return ( EclGetButton ( "nearestgateway_largemap" ) != NULL );

}

int AuxGatewayScreenInterface::ScreenID ()
{

	return SCREEN_AUXGATEWAY;

}

GenericScreen *AuxGatewayScreenInterface::GetComputerScreen ()
{
	
	UplinkAssert (cs);
	return (GenericScreen *) cs;

}
