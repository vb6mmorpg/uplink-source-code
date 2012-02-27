// InterfaceScreen.cpp: implementation of the InterfaceScreen class.
//
//////////////////////////////////////////////////////////////////////

#include "app/globals.h"

#include "interface/interfacescreen.h"
#include "app/serialise.h"
#include "eclipse.h"

#include "mmgr.h"

InterfaceScreen::InterfaceScreen()
{
	this->interface_buttons = new DArray<char *>;
}

InterfaceScreen::~InterfaceScreen()
{
	DeleteDArrayData( this->interface_buttons );
	delete this->interface_buttons;
	this->interface_buttons = NULL;
}

void InterfaceScreen::Create ()
{
}

void InterfaceScreen::Remove ()
{
	DArray<char *> *btns = this->interface_buttons;
	for ( int i = 0; i < btns->Size(); ++i ) {
		if ( btns->ValidIndex( i ) ) {
			char *btn_name = btns->GetData( i );
			if ( (NULL != btn_name) && ('\0' != btn_name[0]) )
				EclRemoveButton( btn_name );
		}
	}
}

void InterfaceScreen::Update ()
{
}

bool InterfaceScreen::IsVisible ()
{
	return false;
}

int InterfaceScreen::ScreenID ()
{

	return 0;

}

void InterfaceScreen::RegisterButton ( int x, int y, int width, int height,
									   char *caption, char *tooltip, char *newname ) {
	EclRegisterButton ( x, y, width, height, caption, tooltip, newname );
	char *name = new char [strlen (newname) + 1];
	UplinkSafeStrcpy( name, newname );
	this->interface_buttons->PutData( name );
}

void InterfaceScreen::RegisterButton ( int x, int y, int width, int height,
									   char *caption, char *newname ) {
	EclRegisterButton ( x, y, width, height, caption, newname );
	char *name = new char [strlen (newname) + 1];
	UplinkSafeStrcpy( name, newname );
	this->interface_buttons->PutData( name );
}
