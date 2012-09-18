
#include "eclipse.h"
#include "vanbakel.h"

#include "app/app.h"
#include "app/miscutils.h"
#include "app/serialise.h"
#include "app/opengl_interface.h"

#include "options/options.h"

#include "game/game.h"
#include "game/data/data.h"

#include "interface/interface.h"
#include "interface/localinterface/localinterface.h"
#include "interface/localinterface/keymapper_interface.h"
#include "interface/remoteinterface/remoteinterface.h"

#include "world/world.h"
#include "world/player.h"

#include "world/computer/computer.h"
#include "world/computer/recordbank.h"
#include "world/computer/computerscreen/computerscreen.h"

#include "mmgr.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void KeyMapperInterface::TitleClick ( Button *button )
{
	game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_NONE );
}

void KeyMapperInterface::CommitClick ( Button *button )
{
	Record *keys = &(game->GetWorld ()->GetPlayer ()->gateway.functionKeys);
	UplinkAssert(keys);
	for ( int i = 0; i < 13; i++ )
	{
		char bname[64];
		UplinkSnprintf(bname, sizeof(bname), "keymapper_key %d", i);

		char btitle[64];
		UplinkSnprintf(btitle, sizeof(btitle), "keymapper_key_title %d", i);

		Button *button = EclGetButton(btitle);
		Button *button2 = EclGetButton(bname);
		if ( button && button2 && EclIsButtonEditable(bname) )
			keys->ChangeField(button->caption, button2->caption);

	}
	create_msgbox ( "Success", "Keyboard mappings updated" );
}

void KeyMapperInterface::FunctionClick ( Button *button )
{
	Create_FunctionInterface();
}
void KeyMapperInterface::Page1Click ( Button *button )
{
	Create_Page1Interface();
}
void KeyMapperInterface::Page2Click ( Button *button )
{
	Create_Page2Interface();
}

void KeyMapperInterface::Create ()
{

	if ( !IsVisible () ) {

		LocalInterfaceScreen::Create ();

		int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
		int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");
		int paneltop = (int)(100.0 * ( (screenw * PANELSIZE) / 188.0 )) + 30;
		int panelwidth = (int)(screenw * PANELSIZE);

		// Title

		EclRegisterButton ( screenw - panelwidth, paneltop + 3, panelwidth - 7, 15, "Keyboard Mapper", "Remove the key mapper screen", "keymapper_title" );
		EclRegisterButtonCallback ( "keymapper_title", TitleClick );

		Create_FunctionInterface();

		EclRegisterButton ( screenw - panelwidth, paneltop + 30 + (14 * 20), (panelwidth/3) - 3, 15, "F1-F12", "Show function key mappings", "keymapper_functions" );
		EclRegisterButtonCallback ( "keymapper_functions", FunctionClick );
		EclRegisterButton ( screenw - panelwidth + (panelwidth/3), paneltop + 30 + (14 * 20), (panelwidth/3) - 3, 15, "A-M", "Show Ctrl+A - Ctrl+M mappings", "keymapper_page1" );
		EclRegisterButtonCallback ( "keymapper_page1", Page1Click );
		EclRegisterButton ( screenw - panelwidth + 2 * (panelwidth/3), paneltop + 30 + (14 * 20), (panelwidth/3) - 3, 15, "N-Z", "Show Ctrl+N - Ctrl+Z mappings", "keymapper_page2" );
		EclRegisterButtonCallback ( "keymapper_page2", Page2Click );

		EclRegisterButton ( screenw - panelwidth, paneltop + 30 + (15 * 20), panelwidth - 7, 15, "Commit", "Save the keyboard mappings", "keymapper_commit" );
		EclRegisterButtonCallback ( "keymapper_commit", CommitClick );
	}



}

void KeyMapperInterface::Create_FunctionInterface ()
{
	int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
	int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");
	int paneltop = (int)(100.0 * ( (screenw * PANELSIZE) / 188.0 )) + 30;
	int panelwidth = (int)(screenw * PANELSIZE);

	Record *keys = &(game->GetWorld ()->GetPlayer ()->gateway.functionKeys);
	UplinkAssert(keys);
	for ( int i = 0; i < 13; i++ )
	{

		char btitle[64];
		UplinkSnprintf(btitle,sizeof(btitle),  "keymapper_key_title %d", i);
		char bname[64];
		UplinkSnprintf(bname, sizeof(bname), "keymapper_key %d", i);
		char bcaption[5];
		UplinkSnprintf(bcaption, sizeof(bcaption), "F%d", i);
		Button *button = EclGetButton(btitle);
		if ( button )
		{
			button->SetCaption(bcaption);
			EclGetButton(bname)->SetCaption(keys->GetField(bcaption));
			EclMakeButtonEditable ( bname );
		}
		else
		{
			EclRegisterButton ( screenw - panelwidth, paneltop + 30 + (i*20), 45, 15, bcaption, btitle );
			EclRegisterButton ( screenw - panelwidth + 50, paneltop + 30 + (i*20), panelwidth - 57, 15, keys->GetField(bcaption), bname );
			EclRegisterButtonCallbacks ( bname, textbutton_draw, NULL, NULL, button_highlight );
			EclMakeButtonEditable ( bname );
		}
		EclMakeButtonUnEditable("keymapper_key 0"); // F0
	}
}

void KeyMapperInterface::Create_Page1Interface ()
{
	int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
	int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");
	int paneltop = (int)(100.0 * ( (screenw * PANELSIZE) / 188.0 )) + 30;
	int panelwidth = (int)(screenw * PANELSIZE);

	Record *keys = &(game->GetWorld ()->GetPlayer ()->gateway.functionKeys);
	UplinkAssert(keys);
	for ( int i = 0; i < 13; i++ )
	{

		char btitle[64];
		UplinkSnprintf(btitle,sizeof(btitle),  "keymapper_key_title %d", i);
		char bname[64];
		UplinkSnprintf(bname, sizeof(bname), "keymapper_key %d", i);
		char bcaption[10];
		UplinkSnprintf(bcaption, sizeof(bcaption), "Ctrl+%c", (char) (i + 65));

		Button *button = EclGetButton(btitle);
		if ( !button )
		{
			EclRegisterButton ( screenw - panelwidth, paneltop + 30 + (i*20), 45, 15, bcaption, btitle );
			EclRegisterButton ( screenw - panelwidth + 50, paneltop + 30 + (i*20), panelwidth - 57, 15, keys->GetField(bcaption), bname );
			EclRegisterButtonCallbacks ( bname, textbutton_draw, NULL, NULL, button_highlight );
			EclMakeButtonEditable ( bname );
		}
		else
		{
			button->SetCaption(bcaption);
			EclGetButton(bname)->SetCaption(keys->GetField(bcaption));
			EclMakeButtonEditable ( bname );
		}
		EclMakeButtonUnEditable("keymapper_key 7"); // Ctrl+H == Backspace
		EclMakeButtonUnEditable("keymapper_key 8"); // Ctrl+I == TAB
		EclMakeButtonUnEditable("keymapper_key 12"); // Ctrl+M == CRLF
	}
}

void KeyMapperInterface::Create_Page2Interface ()
{
	int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
	int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");
	int paneltop = (int)(100.0 * ( (screenw * PANELSIZE) / 188.0 )) + 30;
	int panelwidth = (int)(screenw * PANELSIZE);

	Record *keys = &(game->GetWorld ()->GetPlayer ()->gateway.functionKeys);
	UplinkAssert(keys);
	for ( int i = 0; i < 13; i++ )
	{

		char btitle[64];
		UplinkSnprintf(btitle,sizeof(btitle),  "keymapper_key_title %d", i);
		char bname[64];
		UplinkSnprintf(bname, sizeof(bname), "keymapper_key %d", i);
		char bcaption[10];
		UplinkSnprintf(bcaption, sizeof(bcaption), "Ctrl+%c", (char) (i + 78));

		Button *button = EclGetButton(btitle);
		if ( !button )
		{
			EclRegisterButton ( screenw - panelwidth, paneltop + 30 + (i*20), 45, 15, bcaption, btitle );
			EclRegisterButton ( screenw - panelwidth + 50, paneltop + 30 + (i*20), panelwidth - 57, 15, keys->GetField(bcaption), bname );
			EclRegisterButtonCallbacks ( bname, textbutton_draw, NULL, NULL, button_highlight );
			EclMakeButtonEditable ( bname );
		}
		else
		{
			button->SetCaption(bcaption);
			EclGetButton(bname)->SetCaption(keys->GetField(bcaption));
			EclMakeButtonEditable ( bname );
		}
	}
}

void KeyMapperInterface::Remove ()
{

	if ( IsVisible () ) {

		LocalInterfaceScreen::Remove ();

		EclRemoveButton ( "keymapper_title" );
		EclRemoveButton ( "keymapper_commit" );
		EclRemoveButton ( "keymapper_functions" );
		EclRemoveButton ( "keymapper_page1" );
		EclRemoveButton ( "keymapper_page2" );

		for ( int i = 0; i < 13; i++ )
		{
			char btitle[64];
			UplinkSnprintf(btitle,sizeof(btitle),  "keymapper_key_title %d", i);
			char bname[64];
			UplinkSnprintf(bname, sizeof(bname), "keymapper_key %d", i);

			EclRemoveButton(btitle);
			EclRemoveButton(bname);
		}

	}

}

void KeyMapperInterface::Update ()
{
}

bool KeyMapperInterface::IsVisible ()
{

	return ( EclGetButton ( "keymapper_title" ) != NULL );

}

int KeyMapperInterface::ScreenID ()
{
	
	return SCREEN_KEYMAPPER;

}
