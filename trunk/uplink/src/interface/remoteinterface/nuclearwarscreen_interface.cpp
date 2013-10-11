
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h>

#include "soundgarden.h"
#include "redshirt.h"

#include "app/globals.h"
#include "app/app.h"
#include "app/opengl_interface.h"

#include "options/options.h"

#include "game/game.h"
#include "game/data/data.h"

#include "world/world.h"
#include "world/player.h"
#include "world/vlocation.h"
#include "world/generator/numbergenerator.h"
#include "world/computer/computerscreen/genericscreen.h"
#include "world/company/news.h"
#include "world/company/companyuplink.h"


#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/nuclearwarscreen_interface.h"

#include "mmgr.h"


NuclearWarScreenInterface::NuclearWarScreenInterface ()
{
}

NuclearWarScreenInterface::~NuclearWarScreenInterface ()
{
}

void NuclearWarScreenInterface::DrawBlack ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert (button);
	clear_draw ( button->x, button->y, button->width, button->height );

}

void NuclearWarScreenInterface::DrawLocation ( Button *button, bool highlighted, bool clicked )
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

	char unused [64];
	int index;
	sscanf ( button->name, "%s %d", unused, &index );

	const PhysicalGatewayLocation *pgl;
	if ( game->GetWorldMapType () == Game::defconworldmap ) {
		UplinkAssert (index < NUM_PHYSICALGATEWAYLOCATIONS_DEFCON);
		pgl = &(PHYSICALGATEWAYLOCATIONS_DEFCON[index]);
	}
	else {
		UplinkAssert (index < NUM_PHYSICALGATEWAYLOCATIONS);
		pgl = &(PHYSICALGATEWAYLOCATIONS[index]);
	}
	UplinkAssert (pgl);

	GciDrawText ( button->x - 10, button->y + 20, pgl->city );
//	GciDrawText ( button->x - 10, button->y + 30, pgl->country );

}

void NuclearWarScreenInterface::DrawMainMap ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert (button);

    imagebutton_drawtextured ( button, highlighted, clicked );

	glColor3ub ( 81, 138, 215 );
	border_draw ( button );
    
    NuclearWarScreenInterface *nwsi = (NuclearWarScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen();
    UplinkAssert (nwsi);

    for ( int i = 0; i < nwsi->nukes.Size (); ++i ) {

        NuclearWar_Nuke *nuke = nwsi->nukes.GetData (i);
        int timediff = (int) ( EclGetAccurateTime () - nuke->time );


        if ( timediff > 10000 ) {

            delete nuke;
            nwsi->nukes.RemoveData (i);
            --i;

        }
        else if ( timediff > 3000 ) {

            float d = (float) (timediff - 3000) / 7000.0f;

            int dX = nuke->x + button->x;
            int dY = nuke->y + button->y;            
            
            int sX = (int) ( nuke->sx + ((nuke->x + button->x) - nuke->sx) * d );
            int sY = (int) ( nuke->sy + ((nuke->y + button->y) - nuke->sy) * d );

            float col = 1.0f - d;            
            glColor3f ( col, 0.0, 0.0 );

            glBegin ( GL_LINE_LOOP );
                glVertex2i ( sX, sY );
                glVertex2i ( dX, dY );
            glEnd ();
                    
            //
            // Draw the explosion
            //

            
            int centreX = dX + 3;
            int centreY = dY + 3;
            int width = (int) ( 50 - (50 * d) );
            int height = (int) ( 50 - (50 * d) );

            if ( timediff < 3200 )
                glColor3f ( 1.0f, 0.8f, 0.0f );
            else
                glColor3f ( col, 0.0, 0.0 );

            glBegin ( GL_QUADS );
                glVertex2i ( centreX, centreY - height/2 );
                glVertex2i ( centreX + width/2, centreY );
                glVertex2i ( centreX, centreY + height/2 );
                glVertex2i ( centreX - width/2, centreY );
            glEnd ();

            if ( !nuke->sound ) {
                char explosion [128];
                UplinkSnprintf ( explosion, sizeof ( explosion ), "sounds/explosion%d.wav", NumberGenerator::RandomNumber(3)+1 );
                SgPlaySound ( RsArchiveFileOpen ( explosion ), explosion, false );
                nuke->sound = true;
            }

        }
        else if ( timediff <= 3000 ) {

            float d = (float) timediff / 3000.0f;
            int dX = (int) ( nuke->sx + ((nuke->x + button->x) - nuke->sx) * d );
            int dY = (int) ( nuke->sy + ((nuke->y + button->y) - nuke->sy) * d );

            float col = (float) (timediff) / 3000.0f;
            glColor3f ( col, col, col );
            glBegin ( GL_LINE_LOOP );
                glVertex2i ( nuke->sx, nuke->sy );
                glVertex2i ( dX, dY );
            glEnd ();

        }
                  
    }

}

void NuclearWarScreenInterface::ClickLocation ( Button *button )
{

	// Set the players physical location 

	char unused [64];
	int index;
	sscanf ( button->name, "%s %d", unused, &index );

	const PhysicalGatewayLocation *pgl;
	if ( game->GetWorldMapType () == Game::defconworldmap ) {
		UplinkAssert (index < NUM_PHYSICALGATEWAYLOCATIONS_DEFCON);
		pgl = &(PHYSICALGATEWAYLOCATIONS_DEFCON[index]);
	}
	else {
		UplinkAssert (index < NUM_PHYSICALGATEWAYLOCATIONS);
		pgl = &(PHYSICALGATEWAYLOCATIONS[index]);
	}
	UplinkAssert (pgl);

    NuclearWarScreenInterface *nwsi = (NuclearWarScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen();
    UplinkAssert (nwsi);

    NuclearWar_Nuke *nuke = new NuclearWar_Nuke ();
    nuke->sx = NumberGenerator::RandomNumber ( 640 );
    nuke->sy = 0;
    nuke->x = (int) NumberGenerator::RandomNormalNumber ( (float) SX(pgl->x), 20.0f );
    nuke->y = (int) NumberGenerator::RandomNormalNumber ( (float) SY(pgl->y), 20.0f );
    nuke->time = (int) EclGetAccurateTime ();
    nuke->sound = false;
    nwsi->nukes.PutData ( nuke );

    SgPlaySound ( RsArchiveFileOpen ( "sounds/zap.wav" ), "sounds/zap.wav", false );  
 
}

bool NuclearWarScreenInterface::ReturnKeyPressed ()
{
	
	return false;

}

void NuclearWarScreenInterface::CloseClick ( Button *button )
{

    if ( !game->GetWorld ()->plotgenerator.PlayerCompletedSpecialMission (SPECIALMISSION_WARGAMES) ) {

        game->GetWorld ()->plotgenerator.specialmissionscompleted |= (1 << SPECIALMISSION_WARGAMES);

	    News *news = new News ();
	    news->SetHeadline ( "Government run Nuclear Defense system compromised" );
	    news->SetDetails ( "It emerged today that a high-profile experimental Government computer system "
                           "was very nearly fooled into starting World War III.  The system began receiving "
                           "information that major cities were being attacked by enemy Nuclear Weapons.  The "
                           "system raised the defense status to Defcon2 before somebody pulled the plug.\n\n"
                           "'Its not yet clear how this happened' commented a senior member of the military. "
                           "'However we do know that a hacker was involved, probably playing some kind of prank "
                           "on the world.  Let me tell you this - that prank nearly wiped out the Western Hemisphere.\n\n"
                          );

	    CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	    UplinkAssert (cu);
	    cu->CreateNews ( news );
        
    }
    
    game->GetWorld ()->GetPlayer ()->GetConnection ()->Disconnect ();
    game->GetWorld ()->GetPlayer ()->GetConnection ()->Reset ();

    game->GetInterface ()->GetRemoteInterface ()->RunNewLocation ();
    game->GetInterface ()->GetRemoteInterface ()->RunScreen ( 1 );        

}

void NuclearWarScreenInterface::Create ( ComputerScreen *newcs )
{

	cs = newcs;
 
	if ( !IsVisible () ) {

		EclRegisterButton ( 40, 10, 350, 25, GetComputerScreen ()->subtitle, "", "nuclearwar_maintitle" );
		EclRegisterButtonCallbacks ( "nuclearwar_maintitle", DrawMainTitle, NULL, NULL, NULL );

		// Work out the size/ratios of the map

		int screenw = app->GetOptions ()->GetOptionValue ( "graphics_screenwidth" );
	    int screenh = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );

		int x1 = 23;
		int y1 = 50;
		int fullsizeX = screenw - x1 * 2;
		int fullsizeY = (int) ( 316.0 * ( fullsizeX / 595.0 ) );

		// Black out the background

		EclRegisterButton ( 0, 0, screenw, screenh, " ", " ", "nuclearwar_black" );
		EclRegisterButtonCallbacks ( "nuclearwar_black", DrawBlack, NULL, NULL, NULL );

		// Create the large map graphic

		int numPhysicalGatewayLocations;

		EclRegisterButton ( x1, y1, fullsizeX, fullsizeY, "", "", "nuclearwar_largemap" );                                                
		if ( game->GetWorldMapType () == Game::defconworldmap ) {
			numPhysicalGatewayLocations = NUM_PHYSICALGATEWAYLOCATIONS_DEFCON;
			button_assignbitmap ( "nuclearwar_largemap", "worldmaplarge_defcon.tif" );
		}
		else {
			numPhysicalGatewayLocations = NUM_PHYSICALGATEWAYLOCATIONS;
			button_assignbitmap ( "nuclearwar_largemap", "worldmaplarge.tif" );
		}
		EclRegisterButtonCallbacks ( "nuclearwar_largemap", DrawMainMap, NULL, NULL, NULL );
    
		// Create a button for each gateway

		for ( int i = 0; i < numPhysicalGatewayLocations; ++i ) {

			const PhysicalGatewayLocation *pgl;
			if ( game->GetWorldMapType () == Game::defconworldmap )
				pgl = &(PHYSICALGATEWAYLOCATIONS_DEFCON[i]);
			else
				pgl = &(PHYSICALGATEWAYLOCATIONS[i]);
			UplinkAssert (pgl);

			char bname [64];
			UplinkSnprintf ( bname, sizeof ( bname ), "nuclearwar_location %d", i );

			char tooltip [128];
			UplinkSnprintf ( tooltip, sizeof ( tooltip ), "Simulate attack on this site", pgl->city );

			EclRegisterButton ( x1 + SX(pgl->x), y1 + SY(pgl->y), 7, 7, pgl->city, tooltip, bname );
			EclRegisterButtonCallbacks ( bname, DrawLocation, ClickLocation, button_click, button_highlight );

		}

		// Create some helpfull text

		EclRegisterButton ( 50, y1 + fullsizeY + 20, 400, 70, " ", " ", "nuclearwar_helptext" );
		EclRegisterButtonCallbacks ( "nuclearwar_helptext", textbutton_draw, NULL, NULL, NULL );
		EclRegisterCaptionChange ( "nuclearwar_helptext", "Good Evening, Dr Falken.\n"
                                                          "Would you like to play a game?", 2000 );

        // Exit button

        EclRegisterButton ( 540, 465, 100, 15, "Close", "How about a nice game of chess?", "nuclearwar_close" );
        EclRegisterButtonCallback ( "nuclearwar_close", CloseClick );

        // Bring some buttons forward

        EclButtonBringToFront ( "hud_date" );
        
	}

}

void NuclearWarScreenInterface::Remove ()
{

	if ( IsVisible () ) {
		
		EclRemoveButton ( "nuclearwar_maintitle" );
		EclRemoveButton ( "nuclearwar_black" );
		EclRemoveButton ( "nuclearwar_largemap" );

		int numPhysicalGatewayLocations;
		if ( game->GetWorldMapType () == Game::defconworldmap )
			numPhysicalGatewayLocations = NUM_PHYSICALGATEWAYLOCATIONS_DEFCON;
		else
			numPhysicalGatewayLocations = NUM_PHYSICALGATEWAYLOCATIONS;

		for ( int i = 0; i < numPhysicalGatewayLocations; ++i ) {

			char bname [64];
			UplinkSnprintf ( bname, sizeof ( bname ), "nuclearwar_location %d", i );
			EclRemoveButton ( bname );

		}

		EclRemoveButton ( "nuclearwar_helptext" );
        EclRemoveButton ( "nuclearwar_close" );

	}

}

void NuclearWarScreenInterface::Update ()
{

    EclDirtyButton ( "nuclearwar_largemap" );

}

bool NuclearWarScreenInterface::IsVisible ()
{
	
	return ( EclGetButton ( "nuclearwar_largemap" ) != NULL );

}

int NuclearWarScreenInterface::ScreenID ()
{

	return SCREEN_NUCLEARWAR;

}

GenericScreen *NuclearWarScreenInterface::GetComputerScreen ()
{
	
	UplinkAssert (cs);
	return (GenericScreen *) cs;

}
