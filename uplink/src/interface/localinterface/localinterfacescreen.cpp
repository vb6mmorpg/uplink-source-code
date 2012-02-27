// LocalInterfaceScreen.cpp: implementation of the LocalInterfaceScreen class.
//
//////////////////////////////////////////////////////////////////////

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

#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/miscutils.h"
#include "app/opengl_interface.h"

#include "options/options.h"

#include "game/game.h"

#include "interface/interface.h"
#include "interface/localinterface/localinterface.h"
#include "interface/localinterface/localinterfacescreen.h"

#include "mmgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void LocalInterfaceScreen::BackgroundDraw ( Button *button, bool highlighted, bool clicked )
{
   
#ifndef HAVE_GLES
	glBegin ( GL_QUADS );		
		SetColour ( "PanelBackgroundA" );		glVertex2i ( button->x, button->y + button->height );
		SetColour ( "PanelBackgroundB" );		glVertex2i ( button->x, button->y );
		SetColour ( "PanelBackgroundA" );		glVertex2i ( button->x + button->width, button->y );
		SetColour ( "PanelBackgroundB" );		glVertex2i ( button->x + button->width, button->y + button->height );
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

}

LocalInterfaceScreen::LocalInterfaceScreen()
{

}

LocalInterfaceScreen::~LocalInterfaceScreen()
{

}

void LocalInterfaceScreen::Create ()
{

	CreateHeight ( -1 );

}

void LocalInterfaceScreen::CreateHeight ( int panelheight )
{	

	if ( !IsVisible () ) {

		int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
		int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");
		//int paneltop = SY(100) + 30;
		int paneltop = (int) ( 100.0 * ( (screenw * PANELSIZE) / 188.0 ) + 30 );
		int panelwidth = (int) ( screenw * PANELSIZE );

		if ( panelheight == -1 )
			EclRegisterButton ( screenw - panelwidth - 3, paneltop, panelwidth, SY(300), "", "", "localint_background" );
		else
			EclRegisterButton ( screenw - panelwidth - 3, paneltop, panelwidth, panelheight, "", "", "localint_background" );

		EclRegisterButtonCallbacks ( "localint_background", BackgroundDraw, NULL, NULL, NULL );		

	}

}

void LocalInterfaceScreen::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "localint_background" );

	}

}

void LocalInterfaceScreen::Update ()
{

}

bool LocalInterfaceScreen::IsVisible ()
{
	return false;
}

int LocalInterfaceScreen::ScreenID ()
{

	return 0;

}

LocalInterfaceScreen *LocalInterfaceScreen::GetInterfaceScreen ( int screenID )
{
	
	LocalInterfaceScreen *result = game->GetInterface ()->GetLocalInterface ()->GetInterfaceScreen ();
	UplinkAssert ( result );
	UplinkAssert ( result->ScreenID () == screenID );
	return result;
}

