// MainMenuScreen.cpp: implementation of the MainMenuScreen class.
//
//////////////////////////////////////////////////////////////////////

#include "eclipse.h"

#include "app/app.h"
#include "app/opengl_interface.h"
#include "app/serialise.h"
#include "eclipse.h"

#include "options/options.h"

#include "mainmenu/mainmenuscreen.h"

#include "mmgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MainMenuScreen::MainMenuScreen()
{
	this->interface_buttons = new DArray<char *>;
}

MainMenuScreen::~MainMenuScreen()
{
	DeleteDArrayData( this->interface_buttons );
	delete this->interface_buttons;
	this->interface_buttons = NULL;
}

void MainMenuScreen::Create ()
{	
	
	// Background picture

	int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
	int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");

	RegisterButton ( SX(320) - 170, 75, 425, 60, "", "mainmenu_background" );
	button_assignbitmap ( "mainmenu_background", "mainmenu/uplinklogo.tif" );		
	EclRegisterButtonCallbacks ( "mainmenu_background", imagebutton_draw, NULL, NULL, NULL );

	//RegisterButton ( SX(320) - 165, 75 + 60, 330, 15, "", "mainmenu_version" );
	//button_assignbitmap ( "mainmenu_version", "mainmenu/versionnew.tif" );
	//EclRegisterButtonCallbacks ( "mainmenu_version", imagebutton_draw, NULL, NULL, NULL );

}

void MainMenuScreen::Remove ()
{
	DArray<char *> *btns = this->interface_buttons;
	for ( int i = 0; i < btns->Size(); ++i ) {
		if ( btns->ValidIndex( i ) ) {
			char *btn_name = btns->GetData( i );
			if ( (NULL != btn_name) && ('\0' != btn_name[0]) )
				EclRemoveButton( btn_name );
		}
	}
	//EclRemoveButton ( "mainmenu_background" );
	//EclRemoveButton ( "mainmenu_version" );

}

bool MainMenuScreen::ReturnKeyPressed ()
{

	return false;

}

void MainMenuScreen::Update ()
{
}

bool MainMenuScreen::IsVisible ()
{
	return false;
}

int MainMenuScreen::ScreenID ()
{

	return 0;

}

void MainMenuScreen::RegisterButton ( int x, int y, int width, int height,
									  char *caption, char *tooltip, char *newname ) {
	EclRegisterButton ( x, y, width, height, caption, tooltip, newname );
	char *name = new char [strlen (newname) + 1];
	UplinkSafeStrcpy( name, newname );
	this->interface_buttons->PutData( name );
}

void MainMenuScreen::RegisterButton ( int x, int y, int width, int height,
									  char *caption, char *newname ) {
	EclRegisterButton ( x, y, width, height, caption, newname );
	char *name = new char [strlen (newname) + 1];
	UplinkSafeStrcpy( name, newname );
	this->interface_buttons->PutData( name );
}
