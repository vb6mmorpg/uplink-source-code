
#include "soundgarden.h"
#include "redshirt.h"

#include "app/app.h"
#include "app/opengl_interface.h"

#include "options/options.h"

#include "mainmenu/mainmenu.h"
#include "mainmenu/firsttimeloading_interface.h"

#include "mmgr.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FirstTimeLoadingInterface::FirstTimeLoadingInterface()
{

}

FirstTimeLoadingInterface::~FirstTimeLoadingInterface()
{

}

void FirstTimeLoadingInterface::Create ()
{	

	if ( !IsVisible () ) {

		// Play some music

		if ( app->GetOptions ()->IsOptionEqualTo ( "sound_musicenabled", 1 ) ) {

            //SgPlaylist_Play ("ambient");
            SgPlaylist_Play ("main");

		}

		int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
		int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");

		EclRegisterButton ( screenw - 370, screenh - 30, 370, 20, "", "", "firsttimeloading_text" );
		EclRegisterButtonCallbacks ( "firsttimeloading_text", textbutton_draw, NULL, NULL, NULL );

	}

}

void FirstTimeLoadingInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "firsttimeloading_text" );		

	}

}

void FirstTimeLoadingInterface::Update ()
{
}

bool FirstTimeLoadingInterface::IsVisible ()
{

	return ( EclGetButton ( "firsttimeloading_text" ) != 0 );

}

int FirstTimeLoadingInterface::ScreenID ()
{

	return MAINMENU_FIRSTLOAD;

}
