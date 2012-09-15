
#ifdef WIN32
#include <windows.h>
#endif

#include <strstream>

#include <GL/gl.h>

#include <GL/glu.h>


#include "app/app.h"
#include "app/globals.h"
#include "app/miscutils.h"
#include "app/opengl_interface.h"

#include "game/game.h"
#include "game/data/data.h"

#include "world/world.h"
#include "world/player.h"
#include "world/message.h"
#include "world/computer/computer.h"
#include "world/computer/recordbank.h"
#include "world/computer/gatewaydef.h"
#include "world/computer/computerscreen/genericscreen.h"
#include "world/company/company.h"
#include "world/company/companyuplink.h"
#include "world/company/sale.h"
#include "world/scheduler/changegatewayevent.h"
#include "world/generator/numbergenerator.h"
#include "world/generator/worldgenerator.h"

#include "interface/interface.h"
#include "interface/scrollbox.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/buygatewayscreen_interface.h"

#include "mmgr.h"


int BuyGatewayScreenInterface::baseOffset = 0;

BuyGatewayScreenInterface::BuyGatewayScreenInterface ()
{

	currentselect = 0;

}

BuyGatewayScreenInterface::~BuyGatewayScreenInterface ()
{
}

void BuyGatewayScreenInterface::GatewayButtonDraw ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert (button);

	char unused [64];
	int index;
	sscanf ( button->name, "%s %d", unused, &index );
    index += baseOffset;

	GatewayDef *gd = NULL;
	if ( game->GetWorld ()->gatewaydefs.ValidIndex ( index ) )
		gd = game->GetWorld ()->gatewaydefs.GetData ( index );
	if ( !gd ) {
		clear_draw ( button->x, button->y, button->width, button->height );
		return;
	}

	BuyGatewayScreenInterface *thisint = (BuyGatewayScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (thisint);

	// Fill in the background

//	if ( index == thisint->currentselect ) {
//
//		glBegin ( GL_QUADS );
//			glColor4f ( 0.6f, 0.6f, 0.6f, ALPHA );		glVertex2i ( button->x, button->y );
//			glColor4f ( 0.4f, 0.4f, 0.4f, ALPHA );		glVertex2i ( button->x + button->width, button->y );
//			glColor4f ( 0.6f, 0.6f, 0.6f, ALPHA );		glVertex2i ( button->x + button->width, button->y + button->height );
//			glColor4f ( 0.4f, 0.4f, 0.4f, ALPHA );		glVertex2i ( button->x, button->y + button->height );
//		glEnd ();
//
//  	glColor3ub ( 81, 138, 215 );
//	    border_draw ( button );
//
//	}
//	else {

		if ( index % 2 == 0 ) {

			glBegin ( GL_QUADS );
				SetColour ( "DarkPanelA" );     glVertex2i ( button->x, button->y + button->height );
				SetColour ( "DarkPanelB" );     glVertex2i ( button->x, button->y );
				SetColour ( "DarkPanelA" );     glVertex2i ( button->x + button->width, button->y );
				SetColour ( "DarkPanelB" );     glVertex2i ( button->x + button->width, button->y + button->height );
			glEnd ();

		}
		else {

			glBegin ( GL_QUADS );
				SetColour ( "DarkPanelB" );     glVertex2i ( button->x, button->y + button->height );
				SetColour ( "DarkPanelA" );     glVertex2i ( button->x, button->y );
				SetColour ( "DarkPanelB" );     glVertex2i ( button->x + button->width, button->y );
				SetColour ( "DarkPanelA" );     glVertex2i ( button->x + button->width, button->y + button->height );
			glEnd ();

		}

//	}

	if ( highlighted ) {

		SetColour ( "PanelHighlightBorder" );
		border_draw ( button );

	}
    else if ( index == thisint->currentselect ) {

        SetColour ( "PanelBorder" );
        border_draw ( button );

    }

	// Draw the title and cost

	int price = thisint->GetGatewayPrice(index);
	bool hasmoney = ( game->GetWorld ()->GetPlayer ()->GetBalance () >= price );

	if ( hasmoney ) 	SetColour ( "DefaultText" );
	else				SetColour ( "DimmedText" );

	char cost [16];
	UplinkSnprintf ( cost, sizeof ( cost ), "%dc", price );

	GciDrawText ( button->x + 5, button->y + 10, gd->name );
	GciDrawText ( (button->x + button->width - 10 ) - GciTextWidth ( cost ), button->y + 10, cost );

}

void BuyGatewayScreenInterface::InfoTextDraw ( Button *button, bool highlighted, bool clicked )
{

	textbutton_draw ( button, true, true );

}

void BuyGatewayScreenInterface::GatewayButtonClick ( Button *button )
{

	UplinkAssert (button);

	char unused [64];
	int index;
	sscanf ( button->name, "%s %d", unused, &index );
    index += baseOffset;

	BuyGatewayScreenInterface *thisint = (BuyGatewayScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (thisint);

	thisint->ShowGateway ( index );

}

void BuyGatewayScreenInterface::CloseClick ( Button *button )
{

	GenericScreen *gs= (GenericScreen *) game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
	UplinkAssert (gs);

	if ( gs->nextpage != -1 )
		game->GetInterface ()->GetRemoteInterface ()->RunScreen ( gs->nextpage, gs->GetComputer () );

}

void BuyGatewayScreenInterface::BuyClick ( Button *button )
{

	BuyGatewayScreenInterface *thisint = (BuyGatewayScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (thisint);

	if ( !game->GetWorld ()->gatewaydefs.ValidIndex ( thisint->currentselect ) || !game->GetWorld ()->gatewaydefs.GetData ( thisint->currentselect ) )
		return;

	int price = thisint->GetGatewayPrice(thisint->currentselect);
	bool hasmoney = ( game->GetWorld ()->GetPlayer ()->GetBalance () >= price );

	if ( hasmoney ) {

#ifdef DEMOGAME

        if ( thisint->currentselect > DEMO_MAXGATEWAY ) {

            create_msgbox ( "Sorry", "Not available in demo!\n"
                                     "Visit www.introversion.co.uk to buy the full game." );

            return;

        }

#endif
		BuyConfirmClick ( button );

	}

}

void BuyGatewayScreenInterface::BuyConfirmClick ( Button *button )
{

	BuyGatewayScreenInterface *thisint = (BuyGatewayScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (thisint);

	if ( !game->GetWorld ()->gatewaydefs.ValidIndex ( thisint->currentselect ) || !game->GetWorld ()->gatewaydefs.GetData ( thisint->currentselect ) )
		return;

	// Take the money and run
	// Take the money and run
	// Here i'm alive
	// Everything all of the time

	int price = thisint->GetGatewayPrice(thisint->currentselect);
	game->GetWorld ()->GetPlayer ()->ChangeBalance ( price * -1, "Uplink Corporation gateway purchase" );

	GatewayDef *gatewaydef = game->GetWorld ()->gatewaydefs.GetData ( thisint->currentselect );
	UplinkAssert(gatewaydef); // Should never fail due to checks further up

	Computer *computer = WorldGenerator::GenerateAuxGateway( gatewaydef->name );
	Record *record = computer->recordbank.GetRecordFromName(game->GetWorld ()->GetPlayer ()->handle);
	UplinkAssert(record); // If this fails, GenerateAuxGateway is dead

	// Send a confirmation email to the player

	char code[128];
	computer->GenerateAccessCode(record->GetField(RECORDBANK_NAME), record->GetField(RECORDBANK_PASSWORD), code, sizeof(code));

	Message *msg = new Message ();
	msg->SetTo ( "PLAYER" );
	msg->SetFrom ( "Uplink Corporation" );
	msg->SetSubject ( "Gateway Purchase" );
	msg->SetBody ( "Uplink Corporation has now prepared your new gateway computer.\n"
				   "Your IP and access codes are included in this message. "
				   "The new gateway will only accept connections from your current gateway "
				   "however should your gateway be compromised, we can reroute your dial-up "
				   "connection to your new gateway.\n"
				   "[END]" );
	msg->GiveLink(computer->ip);
	msg->GiveCode(computer->ip, code);
	msg->Send ();


	// Bye Bye

	GenericScreen *gs= (GenericScreen *) game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ();
	UplinkAssert (gs);

	if ( gs->nextpage != -1 )
		game->GetInterface ()->GetRemoteInterface ()->RunScreen ( gs->nextpage, gs->GetComputer () );

}

void BuyGatewayScreenInterface::BuyCancelClick ( Button *button )
{

	BuyGatewayScreenInterface *thisint = (BuyGatewayScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (thisint);

	thisint->Remove ();
	thisint->Create ();

}


void BuyGatewayScreenInterface::GatewayPictureDraw ( Button *button, bool highlighted, bool clicked )
{

    imagebutton_draw ( button, highlighted, clicked );

	BuyGatewayScreenInterface *thisint = (BuyGatewayScreenInterface *) game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ();
	UplinkAssert (thisint);

    glColor3f ( 1.0f, 1.0f, 1.0f );
    border_draw ( button );

#ifdef DEMOGAME
    if ( thisint->currentselect > DEMO_MAXGATEWAY ) {
        glColor3f ( 1.0f, 1.0f, 1.0f );
        GciDrawText ( button->x + 50, button->y + button->height/2, "NOT AVAILABLE IN DEMO" );
    }
#endif

}

void BuyGatewayScreenInterface::ScrollChange ( char *scrollname, int newIndex )
{

    baseOffset = newIndex;

    int numItems = 10;

	for ( int i = 0; i < numItems; ++i ) {

		char bname [64];
		UplinkSnprintf ( bname, sizeof ( bname ), "changegateway_title %d", i );
		EclDirtyButton ( bname );

	}

}

void BuyGatewayScreenInterface::ShowGateway ( int index )
{

	currentselect = index;

	// Dirty the select buttons

    int numItems = 10;

	for ( int i = 0; i < numItems; ++i ) {

		char bname [64];
		UplinkSnprintf ( bname, sizeof ( bname ), "changegateway_title %d", i );
		EclDirtyButton ( bname );

	}


	// Get the new gateway

	GatewayDef *gd = NULL;
	if ( game->GetWorld ()->gatewaydefs.ValidIndex ( currentselect ) )
		gd = game->GetWorld ()->gatewaydefs.GetData ( currentselect );

    if ( gd ) {

	    char newstats1[128];
	    char newstats2[128];
	    UplinkSnprintf ( newstats1, sizeof ( newstats1 ), "Max CPUs : %d\nMax Memory : %dGq", gd->maxcpus, gd->maxmemory * 8 );
	    UplinkSnprintf ( newstats2, sizeof ( newstats2 ), "Max Security : %d\nBandwidth : %dGqs", gd->maxsecurity, gd->bandwidth );
	    EclRegisterCaptionChange ( "changegateway_stats1", newstats1 );
	    EclRegisterCaptionChange ( "changegateway_stats2", newstats2 );

	    EclRegisterCaptionChange ( "changegateway_text", gd->description, 1500 );


	    // Show the new picture

	    delete (EclGetButton ("changegateway_picture")->image_standard);
	    EclGetButton ("changegateway_picture")->image_standard = NULL;
	    //char imagename [64];
	    //UplinkSnprintf ( imagename, sizeof ( imagename ), "gateway/gateway_t%d.tif", index );
	    button_assignbitmap ( "changegateway_picture", gd->thumbnail );
    //	EclGetButton ("changegateway_picture")->image_standard->Scale ( 240, 140 );
	    EclRegisterButtonCallbacks ( "changegateway_picture", GatewayPictureDraw, NULL, NULL, NULL );

    }

}

int BuyGatewayScreenInterface::GetGatewayPrice ( int index )
{

	GatewayDef *oldgateway = game->GetWorld ()->GetPlayer ()->gateway.curgatewaydef;
	UplinkAssert (oldgateway);

	GatewayDef *newgateway = NULL;
	if ( game->GetWorld ()->gatewaydefs.ValidIndex ( index ) )
		newgateway = game->GetWorld ()->gatewaydefs.GetData ( index );
	else
		return 0;

	CompanyUplink *cu = (CompanyUplink *) game->GetWorld ()->GetCompany ( "Uplink" );
	UplinkAssert ( cu );

	int cost = newgateway->cost;

	int cpuCost = 0;
	int memCost = 0;
	int modemCost = 0;

	char modemType[64];
	UplinkSnprintf(modemType, sizeof(modemType), "Modem ( %d Gq / s )", newgateway->bandwidth);
	for ( int i = 0; i < cu->hw_sales.Size(); i++ )
	{
		if ( strcmp(cu->GetHWSale(i)->title, PLAYER_START_CPUTYPE) == 0 )
			cpuCost = cu->GetHWSale(i)->GetVersion(1)->cost * newgateway->maxcpus;

		if ( strcmp(cu->GetHWSale(i)->title, "Memory ( 8 Gq )") == 0 )
			memCost = cu->GetHWSale(i)->GetVersion(1)->cost * newgateway->maxmemory;

		if ( strcmp(cu->GetHWSale(i)->title, modemType) == 0 )
			modemCost = cu->GetHWSale(i)->GetVersion(1)->cost;
	}
	cost = cost + cpuCost + memCost + modemCost;
	return cost;

}

void BuyGatewayScreenInterface::Create ()
{

	UplinkAssert (cs);
	Create ( cs );

}

void BuyGatewayScreenInterface::Create ( ComputerScreen *newcs )
{

	cs = newcs;

	if ( !IsVisible () ) {

		EclRegisterButton ( 80, 60, 350, 25, GetComputerScreen ()->maintitle, "", "changegateway_maintitle" );
		EclRegisterButtonCallbacks ( "changegateway_maintitle", DrawMainTitle, NULL, NULL, NULL );
		EclRegisterButton ( 80, 80, 350, 20, GetComputerScreen ()->subtitle, "", "changegateway_subtitle" );
		EclRegisterButtonCallbacks ( "changegateway_subtitle", DrawSubTitle, NULL, NULL, NULL );

		EclRegisterButton ( 20, 120, 220, 15, "Gateway Name and Cost", " ", "changegateway_title" );

		// One button for each gateway

        int numRows = 10;

		for ( int i = 0; i < numRows; ++i ) {

			GatewayDef *gd = NULL;
			if ( game->GetWorld ()->gatewaydefs.ValidIndex ( i ) )
				gd = game->GetWorld ()->gatewaydefs.GetData ( i );

			char bname [64];
			UplinkSnprintf ( bname, sizeof ( bname ), "changegateway_title %d", i );

			if ( gd )
				EclRegisterButton ( 20, 140 + i * 20, 220, 15, gd->name, "View this gateway", bname );
			else
				EclRegisterButton ( 20, 140 + i * 20, 220, 15, "", "View this gateway", bname );
			
			EclRegisterButtonCallbacks ( bname, GatewayButtonDraw, GatewayButtonClick, button_click, button_highlight );

		}

        // Scroll bar baby yeah

        int numItems = game->GetWorld ()->gatewaydefs.Size ();
        if ( numItems > numRows )
            ScrollBox::CreateScrollBox( "changegatewayscroll", 5, 140, 15, numRows * 20, numItems, 10, baseOffset, ScrollChange );

		// Picture of the gateway

		EclRegisterButton ( 245, 120, 200, 140, " ", " ", "changegateway_picture" );
		button_assignbitmap ( "changegateway_picture", "gateway/gateway_t0.tif" );
//		EclGetButton ("changegateway_picture")->image_standard->Scale ( 200, 140 );
		EclRegisterButtonCallbacks ( "changegateway_picture", GatewayPictureDraw, NULL, NULL, NULL );

		// Stats

		EclRegisterButton ( 245, 270, 110, 30, "accessing...", " ", "changegateway_stats1" );
		EclRegisterButton ( 355, 270, 110, 30, "accessing...", " ", "changegateway_stats2" );

		EclRegisterButtonCallbacks ( "changegateway_stats1", textbutton_draw, NULL, NULL, NULL );
		EclRegisterButtonCallbacks ( "changegateway_stats2", textbutton_draw, NULL, NULL, NULL );

		EclRegisterButton ( 245, 315, 200, 70, "accessing...", " ", "changegateway_text" );
		EclRegisterButtonCallbacks ( "changegateway_text", InfoTextDraw, NULL, NULL, NULL );

		// Control buttons

		int lowY = 140 + numRows * 20;

		EclRegisterButton ( 20, lowY + 10, 220, 15, "Buy", "Click to buy this additional gatewat", "changegateway_buy" );
		EclRegisterButtonCallback ( "changegateway_buy", BuyClick );

		EclRegisterButton ( 20, lowY + 30, 220, 15, "Close", "Return to the main menu", "changegateway_close" );
		EclRegisterButtonCallback ( "changegateway_close", CloseClick );


		// Select your gateway

        GatewayDef *gd = game->GetWorld ()->GetPlayer ()->gateway.curgatewaydef;
		UplinkAssert( gd );

		int sizegatewaydefs = game->GetWorld ()->gatewaydefs.Size ();
		for ( int ii = 0; ii < sizegatewaydefs; ii++ )
			if ( game->GetWorld ()->gatewaydefs.ValidIndex ( ii ) )
				if ( strcmp ( game->GetWorld ()->gatewaydefs.GetData ( ii )->name, gd->name ) == 0 ) {
					ShowGateway ( ii );
					break;
				}

	}

}

void BuyGatewayScreenInterface::CreateAreYouSure ()
{
}

void BuyGatewayScreenInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "changegateway_maintitle" );
		EclRemoveButton ( "changegateway_subtitle" );

		EclRemoveButton ( "changegateway_title" );

        int numRows = 10;

		for ( int i = 0; i < numRows; ++i ) {

			char bname [64];
			UplinkSnprintf ( bname, sizeof ( bname ), "changegateway_title %d", i );
			EclRemoveButton ( bname );

		}

        ScrollBox::RemoveScrollBox ( "changegatewayscroll" );

		EclRemoveButton ( "changegateway_picture" );
		EclRemoveButton ( "changegateway_stats1" );
		EclRemoveButton ( "changegateway_stats2" );
		EclRemoveButton ( "changegateway_text" );

		EclRemoveButton ( "changegateway_buy" );
		EclRemoveButton ( "changegateway_close" );

	}

}

void BuyGatewayScreenInterface::Update ()
{
}

bool BuyGatewayScreenInterface::IsVisible ()
{

	return ( EclGetButton ( "changegateway_maintitle" ) != NULL );

}

bool BuyGatewayScreenInterface::ReturnKeyPressed ()
{

	return false;

}

bool BuyGatewayScreenInterface::EscapeKeyPressed ()
{

    CloseClick ( NULL );
    return true;

}

int BuyGatewayScreenInterface::ScreenID ()
{

	return SCREEN_CHANGEGATEWAY;

}

GenericScreen *BuyGatewayScreenInterface::GetComputerScreen ()
{

	UplinkAssert (cs);
	return (GenericScreen *) cs;

}
