
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
#include "soundgarden.h"
#include "redshirt.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"

#include "game/game.h"
#include "game/data/data.h"

#include "world/world.h"
#include "world/player.h"
#include "world/computer/gateway.h"
#include "world/computer/gatewaydef.h"

#include "interface/interface.h"
#include "interface/localinterface/localinterface.h"
#include "interface/localinterface/hud_interface.h"
#include "interface/localinterface/gateway_interface.h"

#include "mmgr.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void GatewayInterface::CloseClick ( Button *button )
{

	game->GetInterface ()->GetLocalInterface ()->GetHUD ()->gw.Remove ();

}

void GatewayInterface::DrawPopulatedItem ( Button *button, bool highlighted, bool clicked )
{

	imagebutton_draw ( button, highlighted, clicked );

	if ( highlighted || clicked ) {

		glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );
		border_draw ( button );

	}

}

void GatewayInterface::DrawUnPopulatedItem ( Button *button, bool highlighted, bool clicked )
{

}


void GatewayInterface::DrawGatewayBackground ( Button *button, bool highlighted, bool clicked )
{

//	glBegin ( GL_QUADS );		
//		glColor3ub ( 8, 20, 0 );		glVertex2i ( button->x, button->y + button->height );
//		glColor3ub ( 8, 20, 124 );		glVertex2i ( button->x, button->y );
//		glColor3ub ( 8, 20, 0 );		glVertex2i ( button->x + button->width, button->y );
//		glColor3ub ( 8, 20, 124 );		glVertex2i ( button->x + button->width, button->y + button->height );
//	glEnd ();

	clear_draw ( button->x, button->y, button->width, button->height );	
	glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );
	border_draw ( button );

}

void GatewayInterface::DrawGatewayPicture ( Button *button, bool highlighted, bool clicked )
{

	imagebutton_draw ( button, highlighted, clicked );
//	glColor3ub ( 81, 138, 215 );	
//	border_draw ( button );

}

void GatewayInterface::DrawMainTitle ( Button *button, bool highlighted, bool clicked )
{

	glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );
	int ypos = (button->y + button->height / 2) + 5;

	GciDrawText ( button->x, ypos, button->caption, HELVETICA_18 );

}

void GatewayInterface::Create ()
{

	if ( !IsVisible () ) {

		// Remove the HW screen and the small worldmap
	
		game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_NONE );
		game->GetInterface ()->GetLocalInterface ()->GetHUD ()->wmi.Remove ();

		Gateway *gateway = &(game->GetWorld ()->GetPlayer ()->gateway);
		GatewayDef *gatewaydef = gateway->curgatewaydef;
		UplinkAssert (gatewaydef);

		EclRegisterButton ( 20, 50, SX(600), SY(350), "", "", "gateway_background" );
		EclRegisterButtonCallbacks ( "gateway_background", DrawGatewayBackground, NULL, NULL, NULL );		

		EclRegisterButton ( ( 20 + SX(600) ) - 15, 52, 13, 13, "", "", "gateway_close" );
		button_assignbitmaps ( "gateway_close", "close.tif", "close_h.tif", "close_c.tif" );
		EclRegisterButtonCallback ( "gateway_close", CloseClick );

		EclRegisterButton ( 25, 55, 100, 30, "GATEWAY", " ", "gateway_title" );
		EclRegisterButtonCallbacks ( "gateway_title", DrawMainTitle, NULL, NULL, NULL );
	
		// Create the actual gateway graphics

		int centreX = 20 + SX(600) / 2;
		int centreY = 50 + SY(350) / 2;

		int gatewayX = centreX - gatewaydef->width/2;
		int gatewayY = centreY - gatewaydef->height/2;
		int gatewayW = gatewaydef->width;
		int gatewayH = gatewaydef->height;

		//char gatewayname [64];
		//UplinkSnprintf ( gatewayname, sizeof ( gatewayname ), "gateway/gateway%d.tif", gateway->type );
		EclRegisterButton ( gatewayX, gatewayY, gatewayW, gatewayH, "", "", "gateway_picture" );
		button_assignbitmap ( "gateway_picture", gatewaydef->filename );
//		EclGetButton ("gateway_picture")->image_standard->Scale ( 256, 256 );
		EclRegisterButtonCallbacks ( "gateway_picture", DrawGatewayPicture, NULL, NULL, NULL );

		// Create the CPUs

		LList <char *> *cpus = gateway->GetCPUs ();

		for ( int ic = 0; ic < gatewaydef->maxcpus; ++ic ) {

			if ( gatewaydef->cpus.ValidIndex ( ic ) ) {

				GatewayDefLocation *gdl = gatewaydef->cpus.GetData (ic);
				UplinkAssert (gdl);
				char bname [64];
				UplinkSnprintf ( bname, sizeof ( bname ), "gateway_cpu %d", ic );
				EclRegisterButton ( gatewayX + gdl->x, gatewayY + gdl->y, 
									SIZE_GATEWAY_CPU_W, SIZE_GATEWAY_CPU_H, " ", " ", bname );

				if ( ic < cpus->Size () ) {

					EclGetButton ( bname )->SetTooltip ( cpus->GetData (ic) );
					button_assignbitmap ( bname, "gateway/cpu.tif" );
					EclRegisterButtonCallbacks ( bname, DrawPopulatedItem, NULL, button_click, button_highlight );

				}
				else {

					EclGetButton ( bname )->SetTooltip ( "Empty CPU slot" );
					EclRegisterButtonCallbacks ( bname, DrawUnPopulatedItem, NULL, button_click, button_highlight );

				}

			}

		}

		delete cpus;

		// Create the memory chips

		for ( int im = 0; im < gatewaydef->maxmemory; ++im ) {

			if ( gatewaydef->memory.ValidIndex ( im ) ) {

				GatewayDefLocation *gdl = gatewaydef->memory.GetData (im);
				UplinkAssert (gdl);

				char bname [64];
				UplinkSnprintf ( bname, sizeof ( bname ), "gateway_memory %d", im );

				EclRegisterButton ( gatewayX + gdl->x, gatewayY + gdl->y, 
									SIZE_GATEWAY_MEM_W, SIZE_GATEWAY_MEM_H, " ", bname );

				if ( im < gateway->memorysize / 8 ) {
					EclGetButton ( bname )->SetTooltip ( "Memory (8 Gqs)" );
					button_assignbitmap ( bname, "gateway/memory.tif" );
					EclRegisterButtonCallbacks ( bname, DrawPopulatedItem, NULL, button_click, button_highlight );
				}
				else {
					EclGetButton ( bname )->SetTooltip ( "Empty memory slot" );			
					EclRegisterButtonCallbacks ( bname, DrawUnPopulatedItem, NULL, button_click, button_highlight );
				}

			}

		}

		// Create the security devices

		LList <char *> *security = gateway->GetSecurity ();

		for ( int is = 0; is < gatewaydef->maxsecurity; ++is ) {

			if ( gatewaydef->security.ValidIndex ( is ) ) {

				GatewayDefLocation *gdl = gatewaydef->security.GetData (is);
				UplinkAssert (gdl);

				char bname [64];
				UplinkSnprintf ( bname, sizeof ( bname ), "gateway_security %d", is );

				EclRegisterButton ( gatewayX + gdl->x, gatewayY + gdl->y, 
									SIZE_GATEWAY_SEC_W, SIZE_GATEWAY_SEC_H, " ", bname );

				if ( is < gateway->GetNumSecurity () ) {
					EclGetButton ( bname )->SetTooltip ( security->GetData (is) );
					button_assignbitmap ( bname, "gateway/security.tif" );
					EclRegisterButtonCallbacks ( bname, DrawPopulatedItem, NULL, button_click, button_highlight );
				}
				else {
					EclGetButton ( bname )->SetTooltip ( "Empty security slot" );			
					EclRegisterButtonCallbacks ( bname, DrawUnPopulatedItem, NULL, button_click, button_highlight );
				}

			}

		}

		delete security;

		// Create the modem

		char modemtooltip [32];
		UplinkSnprintf ( modemtooltip, sizeof ( modemtooltip ), "Modem (%dGqs)", gateway->GetBandwidth () );
		EclRegisterButton ( gatewayX + gatewaydef->modemX, gatewayY + gatewaydef->modemY, 
							SIZE_GATEWAY_MODEM_W, SIZE_GATEWAY_MODEM_H, " ", modemtooltip, "gateway_modem" );
		button_assignbitmap ( "gateway_modem", "gateway/modem.tif" );
		EclRegisterButtonCallbacks ( "gateway_modem", DrawPopulatedItem, NULL, button_click, button_highlight );

		

		// Create the power supply

		EclRegisterButton ( gatewayX + gatewaydef->powerX, gatewayY + gatewaydef->powerY, 
							SIZE_GATEWAY_POWER_W, SIZE_GATEWAY_POWER_H, " ", "Power Supply Unit", "gateway_power" );
		button_assignbitmap ( "gateway_power", "gateway/power.tif" );
		EclRegisterButtonCallbacks ( "gateway_power", DrawPopulatedItem, NULL, button_click, button_highlight );

	}

}

void GatewayInterface::Remove ()
{

	if ( IsVisible () ) {

        SgPlaySound ( RsArchiveFileOpen ("sounds/close.wav"), "sounds/close.wav", false );

		EclRemoveButton ( "gateway_background" );
		EclRemoveButton ( "gateway_title" );
		EclRemoveButton ( "gateway_close" );
		EclRemoveButton ( "gateway_addhw" );
		EclRemoveButton ( "gateway_upgradegw" );
		
		EclRemoveButton ( "gateway_picture" );

		// Remove the chips

		Gateway *gateway = &(game->GetWorld ()->GetPlayer ()->gateway);
		GatewayDef *gatewaydef = gateway->curgatewaydef;
		UplinkAssert (gatewaydef);

		for ( int ic = 0; ic < gatewaydef->maxcpus; ++ic ) {
			char bname [64];
			UplinkSnprintf ( bname, sizeof ( bname ), "gateway_cpu %d", ic );
			EclRemoveButton ( bname );
			UplinkSnprintf ( bname, sizeof ( bname ), "gateway_cpu_caption %d", ic );
			EclRemoveButton ( bname );
		}

		for ( int im = 0; im < gatewaydef->maxmemory; ++im ) {
			char bname [64];
			UplinkSnprintf ( bname, sizeof ( bname ), "gateway_memory %d", im );
			EclRemoveButton ( bname );
		}

		for ( int is = 0; is < gatewaydef->maxsecurity; ++is ) {
			char bname [64];
			UplinkSnprintf ( bname, sizeof ( bname ), "gateway_security %d", is );
			EclRemoveButton ( bname );
		}

		EclRemoveButton ( "gateway_modem" );
		EclRemoveButton ( "gateway_power" );

		// Replace the world map

		game->GetInterface ()->GetLocalInterface ()->GetHUD ()->wmi.CreateWorldMapInterface_Small ();

	}

}

void GatewayInterface::Update ()
{
}

bool GatewayInterface::IsVisible ()
{

	return ( EclGetButton ( "gateway_background" ) != NULL );

}

int GatewayInterface::ScreenID ()
{
	
	return SCREEN_GATEWAY;

}
