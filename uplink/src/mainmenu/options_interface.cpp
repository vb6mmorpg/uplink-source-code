
#include "eclipse.h"

#include "app/app.h"
#include "app/opengl_interface.h"
#include "app/globals.h"

#include "mainmenu/mainmenu.h"
#include "mainmenu/options_interface.h"
#include "mainmenu/genericoptions_interface.h"

#include "options/options.h"

#include "mmgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
 
OptionsInterface::OptionsInterface()
{

}

OptionsInterface::~OptionsInterface()
{

}

void OptionsInterface::GameOptionsClick ( Button *button )
{

	app->GetMainMenu ()->RunScreen ( MAINMENU_GENERICOPTIONS );
	UplinkAssert ( app->GetMainMenu ()->GetMenuScreen ()->ScreenID () == MAINMENU_GENERICOPTIONS );
	((GenericOptionsInterface *) app->GetMainMenu ()->GetMenuScreen ())->SetOptionTYPE ( "game" );

}

void OptionsInterface::GraphicsOptionsClick	( Button *button )
{

//	app->GetMainMenu ()->RunScreen ( MAINMENU_GENERICOPTIONS );
//	UplinkAssert ( app->GetMainMenu ()->GetMenuScreen ()->ScreenID () == MAINMENU_GENERICOPTIONS );
//	((GenericOptionsInterface *) app->GetMainMenu ()->GetMenuScreen ())->SetOptionTYPE ( "graphics" );

	app->GetMainMenu ()->RunScreen ( MAINMENU_GRAPHICOPTIONS );

}

void OptionsInterface::SoundOptionsClick ( Button *button )
{

	app->GetMainMenu ()->RunScreen ( MAINMENU_GENERICOPTIONS );
	UplinkAssert ( app->GetMainMenu ()->GetMenuScreen ()->ScreenID () == MAINMENU_GENERICOPTIONS );
	((GenericOptionsInterface *) app->GetMainMenu ()->GetMenuScreen ())->SetOptionTYPE ( "sound" );

}

void OptionsInterface::ThemeOptionsClick ( Button *button )
{

    app->GetMainMenu ()->RunScreen ( MAINMENU_THEME );

}

void OptionsInterface::NetworkOptionsClick ( Button *button )
{

	app->GetMainMenu ()->RunScreen ( MAINMENU_NETWORKOPTIONS );

}

void OptionsInterface::ReturnToMainMenuClick ( Button *button )
{

	app->GetMainMenu ()->RunScreen ( MAINMENU_LOGIN );

}

void OptionsInterface::Create ()
{	

	if ( !IsVisible () ) {

		MainMenuScreen::Create ();

		int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
		int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");

		EclRegisterButton ( -832, screenh - 50, 32, 32, "", "Edit GAME options", "gameoptions" );
		EclRegisterButton ( -672, screenh - 50, 32, 32, "", "Edit GRAPHICS options", "graphicsoptions" );
		EclRegisterButton ( -512, screenh - 50, 32, 32, "", "Edit SOUND options", "soundoptions" );

#ifdef MULTIMONITOR_SUPPORT
		EclRegisterButton ( -352, screenh - 50, 32, 32, "", "Edit MULTIMONITOR options", "networkoptions" );
        EclRegisterButton ( -129, screenh - 50, 32, 32, "", "Edit THEME options", "themeoptions" );
#else
        EclRegisterButton ( -352, screenh - 50, 32, 32, "", "Edit THEME options", "themeoptions" );
#endif

		EclRegisterButton ( -32, screenh - 50, 32, 32, "", "Return to Main Menu", "mainmenu" );

		button_assignbitmaps ( "gameoptions",		"mainmenu/gameoptions.tif",		"mainmenu/gameoptions_h.tif",		"mainmenu/gameoptions_c.tif" );
		button_assignbitmaps ( "graphicsoptions",	"mainmenu/graphicsoptions.tif", "mainmenu/graphicsoptions_h.tif",	"mainmenu/graphicsoptions_c.tif" );
		button_assignbitmaps ( "soundoptions",		"mainmenu/soundoptions.tif",	"mainmenu/soundoptions_h.tif",		"mainmenu/soundoptions_c.tif" );
#ifdef MULTIMONITOR_SUPPORT
		button_assignbitmaps ( "networkoptions",	"mainmenu/networkoptions.tif",	"mainmenu/networkoptions_h.tif",	"mainmenu/networkoptions_c.tif" );
#endif
        button_assignbitmaps ( "themeoptions",      "mainmenu/theme.tif",           "mainmenu/theme_h.tif",             "mainmenu/theme_c.tif" );
		button_assignbitmaps ( "mainmenu",			"mainmenu/exitgame.tif",		"mainmenu/exitgame_h.tif",			"mainmenu/exitgame_c.tif" );

		EclRegisterButtonCallback ( "gameoptions",		GameOptionsClick );
		EclRegisterButtonCallback ( "graphicsoptions",	GraphicsOptionsClick );
		EclRegisterButtonCallback ( "soundoptions",		SoundOptionsClick );
		EclRegisterButtonCallback ( "mainmenu",			ReturnToMainMenuClick );

#ifdef DEMOGAME
	#ifdef MULTIMONITOR_SUPPORT
		EclGetButton ( "networkoptions" )->SetTooltip ( "(FULL GAME ONLY) Edit MULTIMONITOR options" );
	#endif
        EclGetButton ( "themeoptions" )->SetTooltip ( "(FULL GAME ONLY) Edit THEME options" );
#else
	#ifdef MULTIMONITOR_SUPPORT
		EclRegisterButtonCallback ( "networkoptions",	NetworkOptionsClick );			      
	#endif
        EclRegisterButtonCallback ( "themeoptions",     ThemeOptionsClick );
#endif

		EclRegisterMovement ( "mainmenu",		screenw - 40, screenh - 50, 975 );
        EclRegisterMovement ( "themeoptions",   screenw - 80, screenh - 50, 1200 );
#ifdef MULTIMONITOR_SUPPORT
		EclRegisterMovement ( "networkoptions", screenw - 120, screenh - 50, 1425 );
		EclRegisterMovement ( "soundoptions",   screenw - 160, screenh - 50, 1650 );
		EclRegisterMovement ( "graphicsoptions",screenw - 200, screenh - 50, 1875 );
		EclRegisterMovement ( "gameoptions",    screenw - 240, screenh - 50, 2100 );
#else
		EclRegisterMovement ( "soundoptions",   screenw - 120, screenh - 50, 1425 );
		EclRegisterMovement ( "graphicsoptions",screenw - 160, screenh - 50, 1650 );
		EclRegisterMovement ( "gameoptions",    screenw - 200, screenh - 50, 1875 );
#endif

	}

}

void OptionsInterface::Remove ()
{

	if ( IsVisible () ) {

		MainMenuScreen::Remove ();

		EclRemoveButton ( "gameoptions" );
		EclRemoveButton ( "graphicsoptions" );
		EclRemoveButton ( "soundoptions" );
#ifdef MULTIMONITOR_SUPPORT
		EclRemoveButton ( "networkoptions" );
#endif
        EclRemoveButton ( "themeoptions" );
		EclRemoveButton ( "mainmenu" );

	}

}

void OptionsInterface::Update ()
{
}

bool OptionsInterface::IsVisible ()
{

	return ( EclGetButton ( "gameoptions" ) != NULL );

}

int OptionsInterface::ScreenID ()
{

	return MAINMENU_OPTIONS;

}

