
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h>

#include <stdio.h>

#include "eclipse.h"
#include "soundgarden.h"
#include "gucci.h"
#include "redshirt.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/miscutils.h"
#include "app/opengl_interface.h"

#include "options/options.h"

#include "game/game.h"

#include "interface/interface.h"
#include "interface/scrollbox.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/swsalesscreen_interface.h"
#include "world/computer/computerscreen/genericscreen.h"

#include "world/world.h"
#include "world/player.h"
#include "world/company/sale.h"
#include "world/company/companyuplink.h"

#include "mmgr.h"

// === Initialise static variables ============================================


int SWSalesScreenInterface::baseoffset = 0;
int SWSalesScreenInterface::currentselect = -1;
int SWSalesScreenInterface::currentversion = -1;


// ============================================================================


void SWSalesScreenInterface::ClickSWButton ( Button *button )
{

	int index;
	sscanf ( button->name, "SWsale %d", &index );

	index += baseoffset;

	// Dirty the old highlighted button

	char oldname [128];
	UplinkSnprintf ( oldname, sizeof ( oldname ), "SWsale %d", currentselect - baseoffset );
	EclDirtyButton ( oldname );

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );

	if ( cu->GetSWSale ( index ) && cu->GetSWSale ( index )->GetVersion (1) ) {

		currentselect = index;
		currentversion = 1;

		EclRegisterCaptionChange ( "swsales_details", cu->GetSWSale ( index )->GetVersion (1)->GetDetails (), 2000 );

	}
	
}

void SWSalesScreenInterface::DrawSWButton ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert ( button );

	clear_draw ( button->x, button->y, button->width, button->height );

	int index;
	sscanf ( button->name, "SWsale %d", &index );
	index += baseoffset;

	// Get the text from sale number (index + baseoffset)

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );

	Sale *sale = cu->GetSWSale ( index );

	if ( sale ) {

		SaleVersion *sv;
		
		if ( index == currentselect )	sv = sale->GetVersion ( currentversion );
		else							sv = sale->GetVersion ( 1 );
		
		if ( index == currentselect ) {

			glBegin ( GL_QUADS );
				SetColour ( "PanelHighlightA" );		glVertex2i ( button->x, button->y );
				SetColour ( "PanelHighlightB" );		glVertex2i ( button->x + button->width, button->y );
				SetColour ( "PanelHighlightA" );		glVertex2i ( button->x + button->width, button->y + button->height );
				SetColour ( "PanelHighlightB" );		glVertex2i ( button->x, button->y + button->height );
			glEnd ();

		}

		if ( highlighted || index == currentselect ) {

			SetColour ( "PanelBorder" );
			border_draw ( button );

		}

		bool hasmoney = ( game->GetWorld ()->GetPlayer ()->GetBalance () >= sv->cost );

		if ( hasmoney ) 	SetColour ( "DefaultText" );
		else				SetColour ( "DimmedText" );

		char name [SIZE_SALE_TITLE];
		char version [8];
		char cost [32];
		char size [32];
		UplinkStrncpy ( name, sale->title, sizeof ( name ) );
		UplinkSnprintf ( version, sizeof ( version ), "v%d.0", sv->data );
		UplinkSnprintf ( cost, sizeof ( cost ), "%dc", sv->cost );
		if ( sale->swhwTYPE != SOFTWARETYPE_HUDUPGRADE ) {
			UplinkSnprintf ( size, sizeof ( size ), "%dGq", sv->size );
		}
		else {
			UplinkStrncpy ( size, "0Gq", sizeof ( size ) );
		}
		GciDrawText ( button->x + 5, button->y + 10, name );
		GciDrawText ( button->x + 240, button->y + 10, version );
		GciDrawText ( button->x + 270, button->y + 10, cost );
		GciDrawText ( button->x + 335, button->y + 10, size );

	}
	
}

void SWSalesScreenInterface::DrawDetails ( Button *button, bool highlighted, bool clicked )
{

	glBegin ( GL_QUADS );
		SetColour ( "PanelBackgroundA" );       glVertex2i ( button->x, button->y + button->height );
		SetColour ( "PanelBackgroundB" );       glVertex2i ( button->x, button->y );
		SetColour ( "PanelBackgroundA" );       glVertex2i ( button->x + button->width, button->y );
		SetColour ( "PanelBackgroundB" );       glVertex2i ( button->x + button->width, button->y + button->height );
	glEnd ();

	SetColour ( "PanelBorder" );
	border_draw ( button );

	text_draw ( button, highlighted, clicked );

}

void SWSalesScreenInterface::MousedownSWButton ( Button *button )
{

	int index;
	sscanf ( button->name, "SWsale %d", &index );

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );

	if ( cu->GetSWSale ( index + baseoffset ) ) {

		button_click ( button );

	}
	

}

void SWSalesScreenInterface::HighlightSWButton ( Button *button )
{

	int index;
	sscanf ( button->name, "SWsale %d", &index );

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );

	if ( cu->GetSWSale ( index + baseoffset ) )
		button_highlight ( button );

}

void SWSalesScreenInterface::ExitClick ( Button *button )
{

	GenericScreen *sss = (GenericScreen *) game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
	UplinkAssert (sss);

	game->GetInterface ()->GetRemoteInterface ()->RunScreen ( sss->nextpage, sss->GetComputer () );

}

bool SWSalesScreenInterface::EscapeKeyPressed ()
{

    ExitClick ( NULL );
    return true;

}

void SWSalesScreenInterface::AcceptClick ( Button *button )
{

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );

	Sale *sale = cu->GetSWSale ( currentselect );

	if ( sale ) {

		SaleVersion *sv = sale->GetVersion ( currentversion );

		Data *newsw = new Data ();
		newsw->SetTitle ( sale->title );		
		newsw->SetDetails ( DATATYPE_PROGRAM, sv->size, 0, 0, (float) sv->data, sale->swhwTYPE );

		// Check the player has space and money
		int memoryindex = game->GetWorld ()->GetPlayer ()->gateway.databank.FindValidPlacement ( newsw );
		bool hasmoney = ( game->GetWorld ()->GetPlayer ()->GetBalance () >= sv->cost );

		if ( ( memoryindex != -1 || sale->swhwTYPE == SOFTWARETYPE_HUDUPGRADE ) && hasmoney ) {

			if ( sale->swhwTYPE == SOFTWARETYPE_HUDUPGRADE ) {

			    //
			    // Special case - HUD upgrades
			    //
	
				if ( strcmp ( sale->title, "HUD_MapShowTrace" ) == 0 )
					game->GetWorld ()->GetPlayer ()->gateway.GiveHUDUpgrade ( HUDUPGRADE_MAPSHOWSTRACE );

				else if ( strcmp ( sale->title, "HUD_ConnectionAnalysis" ) == 0 )
					game->GetWorld ()->GetPlayer ()->gateway.GiveHUDUpgrade ( HUDUPGRADE_CONNECTIONANALYSIS );

                else if ( strcmp ( sale->title, "HUD_IRC-Client" ) == 0 )
                    game->GetWorld ()->GetPlayer ()->gateway.GiveHUDUpgrade ( HUDUPGRADE_IRCCLIENT );

                else if ( strcmp ( sale->title, "HUD_LANView" ) == 0 )
                    game->GetWorld ()->GetPlayer ()->gateway.GiveHUDUpgrade ( HUDUPGRADE_LANVIEW );

                else if ( strcmp ( sale->title, "HUD_KeyMapper" ) == 0 )
                    game->GetWorld ()->GetPlayer ()->gateway.GiveHUDUpgrade ( HUDUPGRADE_KEYMAPPER );

				else
					UplinkAbort ( "Unrecognised HUD upgrade" );

			}
			else if ( sale->swhwTYPE == SOFTWARETYPE_BYPASSER ) {

			    //
			    // Special case - Bypass software (requires HUD_ConnectionAnalysis)
			    //

				if ( !game->GetWorld ()->GetPlayer ()->gateway.HasHUDUpgrade ( HUDUPGRADE_CONNECTIONANALYSIS ) ) {

					delete newsw;

					SgPlaySound ( RsArchiveFileOpen ( "sounds/failure.wav" ), "sounds/failure.wav" );			

					EclRegisterCaptionChange ( "swsales_details", "You require a HUD_ConnectionAnalysis upgrade before purchasing any bypasser." );
					return;					

				}
                else {

                    game->GetWorld ()->GetPlayer ()->gateway.databank.PutData ( newsw, memoryindex );

                }

			}
            else if ( sale->swhwTYPE == SOFTWARETYPE_LANTOOL ) {

                //
                // Special case - Lan Tool (requires HUD_LanView)
                //

                if ( !game->GetWorld ()->GetPlayer ()->gateway.HasHUDUpgrade ( HUDUPGRADE_LANVIEW ) ) {

                    delete newsw;

                    SgPlaySound ( RsArchiveFileOpen ( "sounds/failure.wav" ), "sounds/failure.wav" );

                    EclRegisterCaptionChange ( "swsales_details", "You require a HUD_LanView upgrade before purchasing any LAN tools." );
                    return;

                }
                else {

                    game->GetWorld ()->GetPlayer ()->gateway.databank.PutData ( newsw, memoryindex );

                }

            }
			else {

				game->GetWorld ()->GetPlayer ()->gateway.databank.PutData ( newsw, memoryindex );

			}

			//
			// Go ahead and buy it
			//

			game->GetWorld ()->GetPlayer ()->ChangeBalance ( sv->cost * -1, "Uplink Corporation Software sales" );

			EclRegisterCaptionChange ( "swsales_details", "Item purchased" );

			// Force refresh of each button

			for ( int i = 0; i < 12; ++ i ) {

				char name [128];
				UplinkSnprintf ( name, sizeof ( name ), "SWsale %d", i );

				EclDirtyButton ( name );

			}

		}
		else if ( memoryindex == -1 ) {
	
			// Not enough space
			delete newsw;

			SgPlaySound ( RsArchiveFileOpen ( "sounds/failure.wav" ), "sounds/failure.wav" );			

			EclRegisterCaptionChange ( "swsales_details", "You do not have enough memory to store this program." );

		}
		else {

			// Not enough money
			delete newsw;

			SgPlaySound ( RsArchiveFileOpen ( "sounds/failure.wav" ), "sounds/failure.wav" );			

			EclRegisterCaptionChange ( "swsales_details", "You do not have enough credits to purchase this program." );

		}

	}

}

void SWSalesScreenInterface::NextVersionClick ( Button *button )
{

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );

	Sale *sale = cu->GetSWSale ( currentselect );

	if ( sale ) {
	
		if ( sale->GetVersion ( currentversion+1 ) ) {

			++currentversion;
			SaleVersion *sv = sale->GetVersion ( currentversion );
			
			EclRegisterCaptionChange ( "swsales_details", sv->GetDetails (), 2000 );
			
			char buttonname [32];
			UplinkSnprintf ( buttonname, sizeof ( buttonname ), "SWsale %d", currentselect - baseoffset );
			EclDirtyButton ( buttonname );

		}

	}

}

void SWSalesScreenInterface::PrevVersionClick ( Button *button )
{

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );

	Sale *sale = cu->GetSWSale ( currentselect );

	if ( sale ) {
	
		if ( currentversion > 1 ) {

			--currentversion;
			SaleVersion *sv = sale->GetVersion ( currentversion );
			
			EclRegisterCaptionChange ( "swsales_details", sv->GetDetails (), 2000 );
			
			char buttonname [32];
			UplinkSnprintf ( buttonname, sizeof ( buttonname ), "SWsale %d", currentselect - baseoffset );
			EclDirtyButton ( buttonname );

		}

	}

}

void SWSalesScreenInterface::ScrollChange ( char *scrollname, int newValue )
{

    baseoffset = newValue;

	for ( int i = 0; i < NumItemsOnScreen(); ++i ) {

		char name [128];
		UplinkSnprintf ( name, sizeof ( name ), "SWsale %d", i );
		EclDirtyButton ( name );

	}

}

int SWSalesScreenInterface::NumItemsOnScreen ()
{
    int screenheight = app->GetOptions()->GetOptionValue( "graphics_screenheight" );
    int availablePixels = screenheight - 200 - 45;
    return availablePixels/20;
}

void SWSalesScreenInterface::Create ( ComputerScreen *newcs )
{

	if ( !IsVisible () ) {

		UplinkAssert ( newcs );
		cs = newcs;

		EclRegisterButton ( 20, 30, 237, 15, "Name", "This column shows the name of the item for sale", "swsales_name" );
		EclRegisterButton ( 260, 30, 22, 15, "Ver",	 "This column shows the version number of the software", "swsales_version" );
		EclRegisterButton ( 285, 30, 60, 15, "Cost", "This column shows the cost of the item in credits", "swsales_cost" );
		EclRegisterButton ( 350, 30, 57, 15, "Size", "This column shows the space required for this item", "swsales_requires" );
		EclRegisterButton ( 412, 30, 20 + SY(390) - 395, 15, "", "", "swsales_padding" );

		for ( int i = 0; i < NumItemsOnScreen(); ++i ) {

			char name [128];
			UplinkSnprintf ( name, sizeof ( name ), "SWsale %d", i );
			EclRegisterButton ( 20, i * 20 + 50, SY(390), 17, "", "View details on this item", name );
			EclRegisterButtonCallbacks ( name, DrawSWButton, ClickSWButton, MousedownSWButton, HighlightSWButton ); 
						
		}
		
        int baseX = 23 + SY(388) - 55;
        int baseY = 50 + NumItemsOnScreen() * 20;

		EclRegisterButton ( 20, baseY, SY(388) - 55, 110, "", "", "swsales_details" );
		EclRegisterButtonCallbacks ( "swsales_details", DrawDetails, NULL, NULL, NULL );

		EclRegisterButton ( baseX, baseY, 72, 15, "Purchase", "Purchase this item", "swsales_accept" );
		EclRegisterButtonCallback ( "swsales_accept", AcceptClick );

		EclRegisterButton ( baseX, baseY + 20, 72, 15, "Next Version", "View the next version", "swsales_nextver" );
		EclRegisterButtonCallback ( "swsales_nextver", NextVersionClick );

		EclRegisterButton ( baseX, baseY + 40, 72, 15, "Prev Version", "View the previous version", "swsales_prevver" );
		EclRegisterButtonCallback ( "swsales_prevver", PrevVersionClick );

		EclRegisterButton ( baseX, baseY + 110 - 15, 72, 15, "Exit Market", "Close the screen and return to the main menu", "swsales_exit" );
		EclRegisterButtonCallback ( "swsales_exit", ExitClick );

    	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	    UplinkAssert ( cu );
        ScrollBox::CreateScrollBox ( "swsales_scroll", 22 + SY(390), 47, 15, NumItemsOnScreen() * 20, cu->sw_sales.Size(), NumItemsOnScreen(), 0, ScrollChange );

		baseoffset = 0;
		currentselect = -1;

	}

}

void SWSalesScreenInterface::Remove ()
{

	if ( IsVisible () ) {

		for ( int i = 0; i < NumItemsOnScreen(); ++ i ) {

			char name [128];
			UplinkSnprintf ( name, sizeof ( name ), "SWsale %d", i );

			EclRemoveButton ( name );

		}

		EclRemoveButton ( "swsales_details" );

		EclRemoveButton ( "swsales_name" );
		EclRemoveButton ( "swsales_version" );
		EclRemoveButton ( "swsales_cost" );
		EclRemoveButton ( "swsales_requires" );
		EclRemoveButton ( "swsales_padding" );

		EclRemoveButton ( "swsales_accept" );
		EclRemoveButton ( "swsales_nextver" );
		EclRemoveButton ( "swsales_prevver" );
		EclRemoveButton ( "swsales_exit" );

        ScrollBox::RemoveScrollBox( "swsales_scroll" );

	}

}

bool SWSalesScreenInterface::IsVisible ()
{

	return ( EclGetButton ( "swsales_exit" ) != 0 );

}

int SWSalesScreenInterface::ScreenID ()
{

	return SCREEN_SWSALESSCREEN;

}

GenericScreen *SWSalesScreenInterface::GetComputerScreen ()
{

	UplinkAssert ( cs );
	return (GenericScreen *) cs;

}
