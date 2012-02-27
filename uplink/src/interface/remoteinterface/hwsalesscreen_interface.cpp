
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

#include <stdio.h>

#include "eclipse.h"
#include "soundgarden.h"
#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/miscutils.h"
#include "app/opengl_interface.h"

#include "game/game.h"
#include "game/data/data.h"

#include "world/computer/computerscreen/genericscreen.h"

#include "world/world.h"
#include "world/player.h"
#include "world/date.h"
#include "world/company/sale.h"
#include "world/company/companyuplink.h"
#include "world/computer/gatewaydef.h"
#include "world/generator/numbergenerator.h"
#include "world/scheduler/installhardwareevent.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/hwsalesscreen_interface.h"

#include "mmgr.h"


// === Initialise static variables ============================================


int HWSalesScreenInterface::baseoffset = 0;
int HWSalesScreenInterface::currentselect = -1;


// ============================================================================


HWSalesScreenInterface::HWSalesScreenInterface ()
{

	HWType = -1;

}

HWSalesScreenInterface::~HWSalesScreenInterface ()
{
}

bool HWSalesScreenInterface::EscapeKeyPressed ()
{

    ExitClick (NULL);
    return true;

}

void HWSalesScreenInterface::SetHWType ( int newType )
{

	HWType = newType;

}

void HWSalesScreenInterface::ShowSalesMenuClick ( Button *button )
{

	int newHWType;
	sscanf ( button->name, "hwsales_showmenu %d", &newHWType );

	HWSalesScreenInterface *thisint = (HWSalesScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (thisint);

	thisint->RemoveMenu ();
	thisint->SetHWType ( newHWType );
	
	if ( newHWType == -1 ) 
		thisint->CreateMenu ( thisint->cs );

	else
		thisint->CreateSalesMenu ( thisint->cs );

}

void HWSalesScreenInterface::ClickHWButton ( Button *button )
{

	int index;
	sscanf ( button->name, "HWsale %d", &index );

	index += baseoffset;

	// Dirty the old highlighted button

	char oldname [128];
	UplinkSnprintf ( oldname, sizeof ( oldname ), "HWsale %d", currentselect - baseoffset );
	EclDirtyButton ( oldname );

	HWSalesScreenInterface *thisint = (HWSalesScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (thisint);

	Sale *sale = thisint->items.GetData (index);

	if ( sale && sale->GetVersion ( 1 ) ) {

		currentselect = index;
		EclRegisterCaptionChange ( "hwsales_details", sale->GetVersion ( 1 )->GetDetails (), 2000 );

	}
	
}

void HWSalesScreenInterface::DrawHWButton ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert ( button );

	clear_draw ( button->x, button->y, button->width, button->height );

	int index;
	sscanf ( button->name, "HWsale %d", &index );
	index += baseoffset;

	// Get the text from sale number (index + baseoffset)

	HWSalesScreenInterface *thisint = (HWSalesScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (thisint);
	Sale *sale = thisint->items.GetData ( index );

	if ( sale ) {

		SaleVersion *sv = sale->GetVersion ( 1 );
		
		if ( index == currentselect ) {

#ifndef HAVE_GLES
			glBegin ( GL_QUADS );
				SetColour ( "PanelHighlightA" );        glVertex2i ( button->x, button->y );
				SetColour ( "PanelHighlightB" );        glVertex2i ( button->x + button->width, button->y );
				SetColour ( "PanelHighlightA" );        glVertex2i ( button->x + button->width, button->y + button->height );
				SetColour ( "PanelHighlightB" );        glVertex2i ( button->x, button->y + button->height );
			glEnd ();
#else
			ColourOption *col1, *col2;
			col1 = GetColour("PanelHighlightA");
			col2 = GetColour("PanelHighlightB");

			GLfloat verts[] = {
				button->x, button->y,
				button->x + button->width, button->y,
				button->x + button->width, button->y + button->height,
				button->x, button->y + button->height
			};

			GLfloat colors[] = {
				col1->r, col1->g, col1->b, 1.0f,
				col2->r, col2->g, col2->b, 1.0f,
				col1->r, col1->g, col1->b, 1.0f,
				col2->r, col2->g, col2->b, 1.0f
			};

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_COLOR_ARRAY);

			glVertexPointer(2, GL_FLOAT, 0, verts);
			glColorPointer(4, GL_FLOAT, 0, colors);

			glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_COLOR_ARRAY);
#endif

		}

		if ( highlighted || index == currentselect ) {

			SetColour ( "PanelBorder" );
			border_draw ( button );

		}

		bool hasmoney = ( game->GetWorld ()->GetPlayer ()->GetBalance () >= sv->cost );

		if ( hasmoney ) 	SetColour ( "DefaultText" );
		else                SetColour ( "DimmedText" );

		char name [SIZE_SALE_TITLE];		
		char cost [32];
		UplinkStrncpy ( name, sale->title, sizeof ( name ) );
		UplinkSnprintf ( cost, sizeof ( cost ), "%dc", sv->cost );
		GciDrawText ( button->x + 5, button->y + 10, name );		
		GciDrawText ( button->x + 330, button->y + 10, cost );

	}
	
}

void HWSalesScreenInterface::DrawDetails ( Button *button, bool highlighted, bool clicked )
{

#ifndef HAVE_GLES
	glBegin ( GL_QUADS );
		SetColour ( "PanelBackgroundA" );       glVertex2i ( button->x, button->y + button->height );
		SetColour ( "PanelBackgroundB" );       glVertex2i ( button->x, button->y );
		SetColour ( "PanelBackgroundA" );       glVertex2i ( button->x + button->width, button->y );
		SetColour ( "PanelBackgroundB" );       glVertex2i ( button->x + button->width, button->y + button->height );
	glEnd ();
#else
        ColourOption *col1, *col2;
        col1 = GetColour("PanelBackgroundA");
        col2 = GetColour("PanelBackgroundB");
        GLfloat verts[] = {
                button->x, button->y + button->height,
                button->x, button->y,
                button->x + button->width, button->y,
                button->x + button->width, button->y + button->height
        };

        GLfloat colors[] = {
                col1->r, col1->g, col1->b, 1.0f,
                col2->r, col2->g, col2->b, 1.0f,
                col1->r, col1->g, col1->b, 1.0f,
                col2->r, col2->g, col2->b, 1.0f
        };

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        glVertexPointer(2, GL_FLOAT, 0, verts);
        glColorPointer(4, GL_FLOAT, 0, colors);

        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
#endif

	SetColour ( "PanelBorder" );
	border_draw ( button );

	text_draw ( button, highlighted, clicked );

}

void HWSalesScreenInterface::MousedownHWButton ( Button *button )
{

	int index;
	sscanf ( button->name, "HWsale %d", &index );

	HWSalesScreenInterface *thisint = (HWSalesScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (thisint);
	Sale *sale = thisint->items.GetData (index + baseoffset);

	if ( sale ) 
		button_click ( button );
	

}

void HWSalesScreenInterface::HighlightHWButton ( Button *button )
{

	int index;
	sscanf ( button->name, "HWsale %d", &index );

	HWSalesScreenInterface *thisint = (HWSalesScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (thisint);
	Sale *sale = thisint->items.GetData (index + baseoffset);

	if ( sale )
		button_highlight ( button );

}

void HWSalesScreenInterface::ExitClick ( Button *button )
{

	HWSalesScreenInterface *thisint = (HWSalesScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (thisint);

	if ( thisint->HWType == -1 ) {

		// We are viewing the menu
		// So exit to the next page

		GenericScreen *sss = (GenericScreen *) game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
		UplinkAssert (sss);
		game->GetInterface ()->GetRemoteInterface ()->RunScreen ( sss->nextpage, sss->GetComputer () );

	}
	else {

		// We are looking at a sales screen
		// So exit to the menu

		thisint->RemoveSalesMenu ();
		thisint->SetHWType ( -1 );
		thisint->CreateMenu ( thisint->cs );

	}

}

void HWSalesScreenInterface::AcceptClick ( Button *button )
{

	HWSalesScreenInterface *thisint = (HWSalesScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (thisint);
	Sale *sale = thisint->items.GetData ( currentselect );

	if ( sale ) {

		SaleVersion *sv = sale->GetVersion ( 1 );
		
		//
		// Check the player has the money
		//

		if ( game->GetWorld ()->GetPlayer ()->GetBalance () < sv->cost ) {

			EclRegisterCaptionChange ( "hwsales_details", "You do not have enough credits to purchase this device." );
			return;

		}

		//
		// If this is memory, check he has the space
		//

		if ( sale->swhwTYPE == GATEWAYUPGRADETYPE_MEMORY ) {

			Gateway *gateway = &(game->GetWorld ()->GetPlayer ()->gateway);
			UplinkAssert (gateway);
			GatewayDef *gd = gateway->curgatewaydef;
			UplinkAssert (gd);

			if ( gateway->memorysize + sv->data > gd->maxmemory * 8 ) {
				EclRegisterCaptionChange ( "hwsales_details", "You cannot fit this amount of memory in your gateway." );
				return;
			}

		}

        //
        // If this is a security device, check he has the space

        if ( sale->swhwTYPE == GATEWAYUPGRADETYPE_SECURITY ) {

			Gateway *gateway = &(game->GetWorld ()->GetPlayer ()->gateway);
			UplinkAssert (gateway);
			GatewayDef *gd = gateway->curgatewaydef;
			UplinkAssert (gd);

			if ( gateway->GetNumSecurity () + 1 > gd->maxsecurity ) {
				EclRegisterCaptionChange ( "hwsales_details", "You cannot fit this security device in your gateway." );
				return;
			}

			if ( ( strcmp ( sale->title, "Gateway Self Destruct" ) == 0 || strcmp ( sale->title, "Gateway Motion Sensor" ) == 0 ) ) {
				DataBank *db = &game->GetWorld ()->GetPlayer ()->gateway.databank;
				int alldatasize = 0;
				for ( int i = 0; i < db->GetDataSize (); i++ ) {
					Data *curData = db->GetDataFile ( i );
					if ( curData )
						alldatasize += curData->size;
				}

				if ( alldatasize >= db->GetSize () ) {
					EclRegisterCaptionChange ( "hwsales_details", "You cannot fit this security device in your gateway, you need at least 1Gq of free memory for the driver." );
					return;
				}
			}

        }

        //
        // If this is a modem, check his Gateway can handle it
        //

        if ( sale->swhwTYPE == GATEWAYUPGRADETYPE_MODEM ) {

			Gateway *gateway = &(game->GetWorld ()->GetPlayer ()->gateway);
			UplinkAssert (gateway);
			GatewayDef *gd = gateway->curgatewaydef;
			UplinkAssert (gd);
        
            if ( gd->bandwidth < sv->data ) {
                EclRegisterCaptionChange ( "hwsales_details", "Your gateway does not have sufficient bandwidth to run a modem of that speed." );
                return;
            }
            
        }

		// Item purchased

		game->GetWorld ()->GetPlayer ()->ChangeBalance ( sv->cost * -1, "Uplink hardware sales" );

		// Schedule an Install Hardware Event

		Date duedate;
		duedate.SetDate ( &(game->GetWorld ()->date) );			
		duedate.AdvanceMinute ( TIME_TOINSTALLHARDWARE );
		
		Date warningdate;
		warningdate.SetDate ( &duedate );
		warningdate.AdvanceMinute ( TIME_TOINSTALLHARDWARE_WARNING * -1 );

		InstallHardwareEvent *event = new InstallHardwareEvent ();
		event->SetRunDate ( &duedate );
		event->SetHWSale ( sale, 1 );
		
		game->GetWorld ()->scheduler.ScheduleWarning ( event, &warningdate );
		game->GetWorld ()->scheduler.ScheduleEvent ( event );			
		
		// Change the caption

		EclRegisterCaptionChange ( "hwsales_details", 
								   "Item purchased\n"
								   "The Hardware will be installed in the next 24 hours.\n"
								   "You will be notified when the job is completed." );

		// Force refresh of each button

		for ( int i = 0; i < 12; ++ i ) {

			char name [128];
			UplinkSnprintf ( name, sizeof ( name ), "HWsale %d", i );

			EclDirtyButton ( name );

		}

	}

}

void HWSalesScreenInterface::ScrollUpClick ( Button *button )
{
	
	--baseoffset;	
	if ( baseoffset < 0 ) baseoffset = 0;

	for ( int i = 0; i < 12; ++ i ) {

		char name [128];
		UplinkSnprintf ( name, sizeof ( name ), "HWsale %d", i );

		EclDirtyButton ( name );

	}

}

void HWSalesScreenInterface::ScrollDownClick ( Button *button )
{

	++baseoffset;

	HWSalesScreenInterface *thisint = (HWSalesScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (thisint);
	int numitems = thisint->items.Size ();

	if ( baseoffset > numitems - 12 )
		baseoffset = numitems - 12;
	
	if ( baseoffset < 0 ) baseoffset = 0;

	for ( int i = 0; i < 12; ++ i ) {

		char name [128];
		UplinkSnprintf ( name, sizeof ( name ), "HWsale %d", i );

		EclDirtyButton ( name );

	}

}

void HWSalesScreenInterface::Create ( ComputerScreen *newcs )
{

	SetHWType ( -1 );
	CreateMenu ( newcs );

}

void HWSalesScreenInterface::CreateMenu ( ComputerScreen *newcs ) 
{

	if ( !IsVisibleMenu () ) {

		UplinkAssert (newcs);
		cs = newcs;

		// Processors

		EclRegisterButton ( 80, 180, 32, 32, " ", "Show new Processors", "hwsales_showmenu 1" );
		button_assignbitmaps ( "hwsales_showmenu 1", "upgrades/cpu.tif", "upgrades/cpu_h.tif", "upgrades/cpu_c.tif" );
		EclRegisterButton ( 115, 180, 100, 15, "Processors", " ", "hwsales_caption 1" );

		// Modems

		EclRegisterButton ( 80, 230, 32, 32, " ", "Show new Modems", "hwsales_showmenu 2" );
		button_assignbitmaps ( "hwsales_showmenu 2", "upgrades/modem.tif", "upgrades/modem_h.tif", "upgrades/modem_c.tif" );
		EclRegisterButton ( 115, 230, 100, 15, "Modems", " ", "hwsales_caption 2" );

		// Cooling
/*
		EclRegisterButton ( 80, 280, 32, 32, " ", "Show Cooling equipment", "hwsales_showmenu 3" );
		button_assignbitmaps ( "hwsales_showmenu 3", "upgrades/cooling.tif", "upgrades/cooling_h.tif", "upgrades/cooling_c.tif" );
		EclRegisterButton ( 115, 280, 100, 15, "Cooling", " ", "hwsales_caption 3" );
*/
		// Memory

		EclRegisterButton ( 280, 180, 32, 32, " ", "Show Memory upgrades", "hwsales_showmenu 4" );
		button_assignbitmaps ( "hwsales_showmenu 4", "upgrades/memory.tif", "upgrades/memory_h.tif", "upgrades/memory_c.tif" );
		EclRegisterButton ( 315, 180, 100, 15, "Memory", " ", "hwsales_caption 4" );

		// Security

		EclRegisterButton ( 280, 230, 32, 32, " ", "Show Security upgrades", "hwsales_showmenu 5" );
		button_assignbitmaps ( "hwsales_showmenu 5", "upgrades/security.tif", "upgrades/security_h.tif", "upgrades/security_c.tif" );
		EclRegisterButton ( 315, 230, 100, 15, "Security", " ", "hwsales_caption 5" );

		// Power
/*
		EclRegisterButton ( 280, 280, 32, 32, " ", "Show power supplies", "hwsales_showmenu 6" );
		button_assignbitmaps ( "hwsales_showmenu 6", "upgrades/power.tif", "upgrades/power_h.tif", "upgrades/power_c.tif" );
		EclRegisterButton ( 315, 280, 100, 15, "Power", " ", "hwsales_caption 6" );
*/

		EclRegisterButton ( 280, 320, 32, 32, " ", "Return to the main menu", "hwsales_exit" );
		button_assignbitmaps ( "hwsales_exit", "upgrades/exit.tif", "upgrades/exit_h.tif", "upgrades/exit_c.tif" );
		EclRegisterButton ( 315, 320, 100, 15, "Exit", " ", "hwsales_exitcaption" );
		EclRegisterButtonCallback ( "hwsales_exit", ExitClick );
		EclRegisterButtonCallbacks ( "hwsales_exitcaption", DrawMainTitle, NULL, NULL, NULL );


		EclRegisterButtonCallback ( "hwsales_showmenu 1", ShowSalesMenuClick );
		EclRegisterButtonCallback ( "hwsales_showmenu 2", ShowSalesMenuClick );
		EclRegisterButtonCallback ( "hwsales_showmenu 3", ShowSalesMenuClick );
		EclRegisterButtonCallback ( "hwsales_showmenu 4", ShowSalesMenuClick );
		EclRegisterButtonCallback ( "hwsales_showmenu 5", ShowSalesMenuClick );
//		EclRegisterButtonCallback ( "hwsales_showmenu 6", ShowSalesMenuClick );
				
		EclRegisterButtonCallbacks ( "hwsales_caption 1", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButtonCallbacks ( "hwsales_caption 2", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButtonCallbacks ( "hwsales_caption 3", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButtonCallbacks ( "hwsales_caption 4", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButtonCallbacks ( "hwsales_caption 5", DrawMainTitle, NULL, NULL, NULL );
//		EclRegisterButtonCallbacks ( "hwsales_caption 6", DrawMainTitle, NULL, NULL, NULL );

	}

}

void HWSalesScreenInterface::CreateSalesMenu ( ComputerScreen *newcs ) 
{

	if ( !IsVisibleSalesMenu () ) {

		UplinkAssert ( newcs );
		cs = newcs;

		//
		// Populate the "items" array with all items of this type
		//

		items.Empty ();
		CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
		UplinkAssert ( cu );
		for ( int ic = 0; ic < cu->hw_sales.Size (); ++ic ) {
			Sale *thissale = cu->GetHWSale (ic);
			UplinkAssert (thissale);
			if (thissale->swhwTYPE == HWType )
				items.PutData (thissale);
		}
	
		//
		// 
		
	
		EclRegisterButton ( 20, 30, 328, 15, "Name", "This column shows the name of the item for sale", "hwsales_name" );
		EclRegisterButton ( 350, 30, 60, 15, "Cost", "This column shows the cost of the item in credits", "hwsales_cost" );
		EclRegisterButton ( 412, 30, 15, 15, "", "", "hwsales_padding" );

		for ( int i = 0; i < 12; ++i ) {

			char name [128];
			UplinkSnprintf ( name, sizeof ( name ), "HWsale %d", i );
			EclRegisterButton ( 20, i * 20 + 50, 390, 17, "", "View details on this item", name );
			EclRegisterButtonCallbacks ( name, DrawHWButton, ClickHWButton, MousedownHWButton, HighlightHWButton ); 
						
		}
		
		EclRegisterButton ( 20, 290, 330, 110, "", "", "hwsales_details" );
		EclRegisterButtonCallbacks ( "hwsales_details", DrawDetails, NULL, NULL, NULL );

		EclRegisterButton ( 355, 290, 72, 15, "Purchase", "Purchase this item", "hwsales_accept" );
		EclRegisterButtonCallback ( "hwsales_accept", AcceptClick );
		EclRegisterButton ( 355, 387, 72, 15, "Exit Market", "Close the screen and return to the hardware menu", "hwsales_exit" );
		EclRegisterButtonCallback ( "hwsales_exit", ExitClick );

		EclRegisterButton ( 412, 50, 15, 15, "^", "Scroll upwards", "hwsales_scrollup" );
		button_assignbitmaps ( "hwsales_scrollup", "up.tif", "up_h.tif", "up_c.tif" );
		EclRegisterButtonCallback ( "hwsales_scrollup", ScrollUpClick );

		EclRegisterButton ( 412, 270, 15, 15, "v", "Scroll downwards", "hwsales_scrolldown" );
		button_assignbitmaps ( "hwsales_scrolldown", "down.tif", "down_h.tif", "down_c.tif" );
		EclRegisterButtonCallback ( "hwsales_scrolldown", ScrollDownClick );

		EclRegisterButton ( 412, 67, 15, 201, "", "", "hwsales_scrollbar" );		

		baseoffset = 0;
		currentselect = -1;

	}

}

void HWSalesScreenInterface::Remove ()
{

	if ( IsVisibleMenu () ) RemoveMenu ();

	if ( IsVisibleSalesMenu () ) RemoveSalesMenu ();

}

void HWSalesScreenInterface::RemoveMenu ()
{

	EclRemoveButton ( "hwsales_showmenu 1" );
	EclRemoveButton ( "hwsales_showmenu 2" );
	EclRemoveButton ( "hwsales_showmenu 3" );
	EclRemoveButton ( "hwsales_showmenu 4" );
	EclRemoveButton ( "hwsales_showmenu 5" );
//	EclRemoveButton ( "hwsales_showmenu 6" );

	EclRemoveButton ( "hwsales_caption 1" );
	EclRemoveButton ( "hwsales_caption 2" );
	EclRemoveButton ( "hwsales_caption 3" );
	EclRemoveButton ( "hwsales_caption 4" );
	EclRemoveButton ( "hwsales_caption 5" );
//	EclRemoveButton ( "hwsales_caption 6" );

    EclRemoveButton ( "hwsales_exit" );
    EclRemoveButton ( "hwsales_exitcaption" );

}

void HWSalesScreenInterface::RemoveSalesMenu ()
{

	for ( int i = 0; i < 12; ++ i ) {

		char name [128];
		UplinkSnprintf ( name, sizeof ( name ), "HWsale %d", i );

		EclRemoveButton ( name );

	}

	EclRemoveButton ( "hwsales_details" );

	EclRemoveButton ( "hwsales_name" );
	EclRemoveButton ( "hwsales_cost" );
	EclRemoveButton ( "hwsales_padding" );
	
	EclRemoveButton ( "hwsales_scrollup" );
	EclRemoveButton ( "hwsales_scrolldown" );
	EclRemoveButton ( "hwsales_scrollbar" );

	EclRemoveButton ( "hwsales_accept" );
	EclRemoveButton ( "hwsales_exit" );

}

bool HWSalesScreenInterface::IsVisible ()
{

	return ( IsVisibleMenu () || IsVisibleSalesMenu () );

}

bool HWSalesScreenInterface::IsVisibleMenu ()
{

	return ( EclGetButton ( "hwsales_showmenu 1" ) != 0 );

}

bool HWSalesScreenInterface::IsVisibleSalesMenu ()
{

	return ( EclGetButton ( "hwsales_exit" ) != 0 );

}

int HWSalesScreenInterface::ScreenID ()
{

	return SCREEN_HWSALESSCREEN;

}

GenericScreen *HWSalesScreenInterface::GetComputerScreen ()
{

	UplinkAssert ( cs );
	return (GenericScreen *) cs;

}
