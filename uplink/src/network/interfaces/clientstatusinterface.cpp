
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

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"
#include "app/serialise.h"

#include "options/options.h"

#include "mainmenu/mainmenu.h"

#include "network/network.h"
#include "network/networkclient.h"
#include "network/interfaces/clientstatusinterface.h"

#include "mmgr.h"



ClientStatusInterface::ClientStatusInterface()
{

	UplinkStrncpy ( hardware, " ", sizeof ( hardware ) );
	UplinkStrncpy ( hudupgrades, " ", sizeof ( hudupgrades ) );
	UplinkStrncpy ( connection, " ", sizeof ( connection ) );

	UplinkStrncpy ( ratings, " ", sizeof ( ratings ) );
	UplinkStrncpy ( criminal, " ", sizeof ( criminal ) );
	UplinkStrncpy ( financial, " ", sizeof ( financial ) );

	news.PutDataAtStart ( " " );

}

ClientStatusInterface::~ClientStatusInterface()
{

    DeleteLListData ( &news );

}

void ClientStatusInterface::BackgroundDraw ( Button *button, bool highlighted, bool clicked )
{
	
#ifndef HAVE_GLES
	glBegin ( GL_QUADS );		
		glColor3ub ( 8, 20, 0 );		glVertex2i ( button->x, button->y + button->height );
		glColor3ub ( 8, 20, 124 );		glVertex2i ( button->x, button->y );
		glColor3ub ( 8, 20, 0 );		glVertex2i ( button->x + button->width, button->y );
		glColor3ub ( 8, 20, 124 );		glVertex2i ( button->x + button->width, button->y + button->height );
	glEnd ();
#else
	GLfloat verts[] = {
		button->x, button->y + button->height,
		button->x, button->y,
		button->x + button->width, button->y,
		button->x + button->width, button->y + button->height
	};

	GLfloat colors[] = {
		8, 20, 0, 255,
		8, 20, 124, 255,
		8, 20, 0, 255,
		8, 20, 124, 255
	};

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(2, GL_FLOAT, 0, verts);
	glColorPointer(2, GL_FLOAT, 0, colors);

	glDrawArrays(GL_TRIANGLES, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
#endif

	glColor4ub ( 81, 138, 215, 255 );
	border_draw ( button );

}

void ClientStatusInterface::GatewayPanelDraw ( Button *button, bool highlighted, bool clicked )
{

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
	glEnable ( GL_SCISSOR_TEST );


	ClientStatusInterface *thisint = (ClientStatusInterface *) app->GetNetwork ()->GetClient ()->GetNetworkScreen ();
	UplinkAssert (thisint);

	BackgroundDraw ( button, highlighted, clicked );

	int x = button->x;
	int y = button->y;

	glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );

	GciDrawText ( x + 55, y + 40, "Hardware", HELVETICA_18 );
	text_draw ( x + 20, y + 70, thisint->hardware, button->width );
	
	GciDrawText ( x + 40, y + 200, "HUD Upgrades", HELVETICA_18 );
	text_draw ( x + 20, y + 230, thisint->hudupgrades, button->width );

	GciDrawText ( x + 50, y + 300, "Connection", HELVETICA_18 );
	text_draw ( x + 20, y + 330, thisint->connection, button->width );
	
	glDisable ( GL_SCISSOR_TEST );

}

void ClientStatusInterface::PersonalPanelDraw ( Button *button, bool highlighted, bool clicked )
{

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
	glEnable ( GL_SCISSOR_TEST );


	ClientStatusInterface *thisint = (ClientStatusInterface *) app->GetNetwork ()->GetClient ()->GetNetworkScreen ();
	UplinkAssert (thisint);

	BackgroundDraw ( button, highlighted, clicked );

	int x = button->x;
	int y = button->y;

	glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );

	GciDrawText ( x + 50, y + 40, "Ratings", HELVETICA_18 );
	text_draw ( x + 20, y + 70, thisint->ratings, button->width );

	GciDrawText ( x + 50, y + 160, "Criminal", HELVETICA_18 );
	text_draw ( x + 20, y + 190, thisint->criminal, button->width );

	GciDrawText ( x + 50, y + 280, "Financial", HELVETICA_18 );
	text_draw ( x + 20, y + 310, thisint->financial, button->width );

	glDisable ( GL_SCISSOR_TEST );

}

void ClientStatusInterface::WorldPanelDraw ( Button *button, bool highlighted, bool clicked )
{

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
	glEnable ( GL_SCISSOR_TEST );


	ClientStatusInterface *thisint = (ClientStatusInterface *) app->GetNetwork ()->GetClient ()->GetNetworkScreen ();
	UplinkAssert (thisint);

	BackgroundDraw ( button, highlighted, clicked );

	int x = button->x;
	int y = button->y;

	glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );

	GciDrawText ( x + 50, y + 40, "Headlines", HELVETICA_18 );

	for ( int i = 0; i < thisint->news.Size (); ++i ) {
		if ( thisint->news.ValidIndex (i) ) {

			char *newsitem = thisint->news.GetData (i);
			UplinkAssert (newsitem);

			text_draw ( x + 10, y + 70 + i * 55, newsitem, button->width + 20 );

		}
	}

	glDisable ( GL_SCISSOR_TEST );

}

void ClientStatusInterface::MainMenuClick ( Button *button )
{

	UplinkAssert ( app->GetNetwork ()->STATUS == NETWORK_CLIENT );

	app->GetNetwork ()->GetClient ()->SetClientType ( CLIENT_NONE );
	app->GetMainMenu ()->RunScreen ( MAINMENU_LOGIN );

}

void ClientStatusInterface::SetHardware	( char *newhw )
{

	UplinkAssert ( strlen(newhw) < sizeof(hardware) );
	UplinkStrncpy ( hardware, newhw, sizeof ( hardware ) );

	EclDirtyButton ( "status_gatewaybg" );

}

void ClientStatusInterface::SetHUDUpgrades ( char *newhud )
{
	
	UplinkAssert ( strlen(newhud) < sizeof(hudupgrades) );
	UplinkStrncpy ( hudupgrades, newhud, sizeof ( hudupgrades ) );

	EclDirtyButton ( "status_gatewaybg" );

}

void ClientStatusInterface::SetConnection	( char *newconnection )
{

	UplinkAssert ( strlen(newconnection) < sizeof(connection) );
	UplinkStrncpy ( connection, newconnection, sizeof ( connection ) );

	EclDirtyButton ( "status_gatewaybg" );

}

void ClientStatusInterface::SetRating ( char *newrating )
{
	
	UplinkAssert ( strlen(newrating) < sizeof(ratings) );
	UplinkStrncpy ( ratings, newrating, sizeof ( ratings ) );

	EclDirtyButton ( "status_personalbg" );

}

void ClientStatusInterface::SetFinancial ( char *newfinancial )
{

	UplinkAssert (strlen(newfinancial) < sizeof(financial) );
	UplinkStrncpy ( financial, newfinancial, sizeof ( financial ) );

	EclDirtyButton ( "status_personalbg" );

}

void ClientStatusInterface::SetCriminal  ( char *newcriminal )
{

	UplinkAssert (strlen(newcriminal) < sizeof(criminal) );
	UplinkStrncpy ( criminal, newcriminal, sizeof ( criminal ) );

	EclDirtyButton ( "status_personalbg" );

}

void ClientStatusInterface::AddNewsStory ( char *newsstory )
{

	char *newscopy = new char [strlen(newsstory)+1];
	UplinkSafeStrcpy ( newscopy, newsstory );

	news.PutDataAtStart ( newscopy );

	while ( news.Size () > 6 )	news.RemoveData (6);

	EclDirtyButton ( "status_worldbg" );

}

void ClientStatusInterface::Create ()
{

	if ( !IsVisible () ) {

		int screenw = app->GetOptions ()->GetOptionValue ( "graphics_screenwidth" );
		int screenh = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );

		// Create a close button

		EclRegisterButton ( 3, 4, 13, 13, "", "Go to the Main Menu", "status_mainmenu" );
		button_assignbitmaps ( "status_mainmenu","close.tif", "close_h.tif", "close_c.tif" );
		EclRegisterButtonCallback ( "status_mainmenu", MainMenuClick );

		// Create a "gateway" panel

		EclRegisterButton ( 10, 35, 200, 400, "", "", "status_gatewaybg" );
		EclRegisterButtonCallbacks ( "status_gatewaybg", GatewayPanelDraw, NULL, NULL, NULL );		
		EclRegisterButton ( 13, 38, 193, 15, "Gateway Status", "Shows the status of your Gateway", "status_gatewaytitle" );
			
		// Create a "world" panel

		EclRegisterButton ( 220, 35, 200, 400, "", "", "status_worldbg" );
		EclRegisterButtonCallbacks ( "status_worldbg", WorldPanelDraw, NULL, NULL, NULL );		
		EclRegisterButton ( 223, 38, 193, 15, "World Status", "Shows the status of the world", "status_worldtitle" );

		// Create a "personal" panel

		EclRegisterButton ( 430, 35, 200, 400, "", "", "status_personalbg" );
		EclRegisterButtonCallbacks ( "status_personalbg", PersonalPanelDraw, NULL, NULL, NULL );		
		EclRegisterButton ( 433, 38, 193, 15, "Personal Status", "Shows your personal status", "status_personaltitle" );

	}

}

void ClientStatusInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "status_mainmenu" );

		EclRemoveButton ( "status_gatewaytitle" );
		EclRemoveButton ( "status_gatewaybg" );

		EclRemoveButton ( "status_personaltitle" );
		EclRemoveButton ( "status_personalbg" );

		EclRemoveButton ( "status_worldtitle" );
		EclRemoveButton ( "status_worldbg" );

	}

}

void ClientStatusInterface::Update ()
{
}

bool ClientStatusInterface::IsVisible ()
{

	return ( EclGetButton ( "status_mainmenu" ) != NULL );

}

int  ClientStatusInterface::ScreenID ()
{

	return CLIENT_STATUS;

}
