// -*- tab-width:4 c-file-style:"cc-mode" -*-
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h>

#include "soundgarden.h"
#include "redshirt.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/miscutils.h"
#include "app/opengl.h"
#include "app/opengl_interface.h"
#include "app/serialise.h"

#include "options/options.h"

#include "mainmenu/mainmenu.h"
#include "mainmenu/theme_interface.h"

#include "mmgr.h"

DArray <char *> *ThemeInterface::themes = NULL;
int ThemeInterface::currentSelect = -1;


void ThemeInterface::ReturnToMainMenuClick ( Button *button )
{

	app->GetMainMenu ()->RunScreen ( MAINMENU_OPTIONS );

}

void ThemeInterface::ApplyClick ( Button *button )
{

    DArray <char *> *themes = ListAvailableThemes ();
    if ( themes->ValidIndex (currentSelect) ) {
        char *newThemeName = themes->GetData( currentSelect );
        app->GetOptions ()->SetThemeName ( newThemeName );   
    }

    EclReset ( app->GetOptions ()->GetOptionValue( "graphics_screenwidth" ),
               app->GetOptions ()->GetOptionValue( "graphics_screenheight" ) );

    app->GetMainMenu ()->RunScreen ( MAINMENU_THEME );

}

void ThemeInterface::DefaultClick ( Button *button )
{

    app->GetOptions ()->SetThemeName ( "graphics" );

    EclReset ( app->GetOptions ()->GetOptionValue( "graphics_screenwidth" ),
               app->GetOptions ()->GetOptionValue( "graphics_screenheight" ) );
    
    app->GetMainMenu ()->RunScreen ( MAINMENU_THEME );
    currentSelect = -1;

}

ThemeInterface::ThemeInterface ()
{
}

ThemeInterface::~ThemeInterface ()
{
}

DArray <char *> *ThemeInterface::ListAvailableThemes ()
{

    if ( !themes ) {

        themes = ListSubdirs ( app->path );

		for ( int i = themes->Size() - 1; i >= 0; --i ) {
			if ( themes->ValidIndex (i) ) {

				// Is there a theme in here?

				char themeFilename[256];
				UplinkSnprintf ( themeFilename, sizeof ( themeFilename ), "%s/theme.txt", themes->GetData(i) );
				
                if ( !RsArchiveFileLoaded (themeFilename) ) {
					delete [] themes->GetData (i);
					themes->RemoveData (i);
					continue;
				}

                char *lowercaseThemename = LowerCaseString( themes->GetData(i) );
				if ( strcmp ( lowercaseThemename, "graphics" ) == 0 ) {
					delete [] themes->GetData (i);
                    themes->RemoveData (i);
				}
                delete [] lowercaseThemename;

			}
		}

		return themes;

    }
    else
        return themes;

}

void ThemeInterface::ThemeNameDraw ( Button *button, bool highlighted, bool clicked )
{

    UplinkAssert (button);

    char unused [64];
    int index;
    sscanf ( button->name, "%s %d", unused, &index );
    
    if ( highlighted || clicked || index == currentSelect ) {

	    glBegin ( GL_QUADS );		
		    SetColour ( "PanelHighlightA" );	glVertex2i ( button->x, button->y + button->height );
		    SetColour ( "PanelHighlightB" );	glVertex2i ( button->x, button->y );
		    SetColour ( "PanelHighlightA" );	glVertex2i ( button->x + button->width, button->y );
		    SetColour ( "PanelHighlightB" );	glVertex2i ( button->x + button->width, button->y + button->height );
	    glEnd ();

    }
    else {

	    glBegin ( GL_QUADS );		
		    SetColour ( "PanelBackgroundA" );   glVertex2i ( button->x, button->y + button->height );
		    SetColour ( "PanelBackgroundB" );   glVertex2i ( button->x, button->y );
		    SetColour ( "PanelBackgroundA" );   glVertex2i ( button->x + button->width, button->y );
		    SetColour ( "PanelBackgroundB" );   glVertex2i ( button->x + button->width, button->y + button->height );
	    glEnd ();

    }

    if ( clicked || index == currentSelect ) {

        SetColour ( "PanelHighlightBorder" );
        border_draw ( button );

    }
    else {

	    SetColour ( "PanelBorder" );
	    border_draw ( button );

    }

	int xpos = (button->x + button->width  / 2) - ( GciTextWidth ( button->caption ) / 2 );
	int ypos = (button->y + button->height / 2) + 2;

    SetColour ( "DefaultText" );
    GciDrawText ( xpos, ypos, button->caption );

}

void ThemeInterface::ThemeNameClick ( Button *button )
{

    UplinkAssert (button);

    char unused [64];
    int index;
    sscanf ( button->name, "%s %d", unused, &index );
  
    currentSelect = index;

    DArray <char *> *themes = ListAvailableThemes ();
    for ( int i = 0; i < themes->Size(); ++i ) {
        if ( themes->ValidIndex (i) ) {

            char bname [64];
            UplinkSnprintf ( bname, sizeof ( bname ), "theme %d", i );
            EclDirtyButton ( bname );

        }
    }

}

void ThemeInterface::Create ()
{
	
	if ( !IsVisible () ) {

		MainMenuScreen::Create ();

		int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
		int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");

        DArray <char *> *themes = ListAvailableThemes ();
        int numThemes = themes->NumUsed();

        // Title

	    EclRegisterButton ( screenw - 210, screenh - 40, 200, 15, "THEME options", "Close this options screen", "theme_title" );
	    EclRegisterButtonCallback ( "theme_title", ReturnToMainMenuClick );
	    EclRegisterMovement ( "theme_title", screenw - 210, screenh - 170 - numThemes * 20, 500 );

        // Available themes

        EclRegisterButton ( screenw - 210, screenh - 40, 200, 15, "Available Themes", " ", "theme_text" );
        EclRegisterButtonCallbacks ( "theme_text", textbutton_draw, NULL, NULL, NULL );
        EclRegisterMovement ( "theme_text", screenw - 210, screenh - 150 - numThemes * 20, 500 );
        
        // One button for each theme

        int ypos = screenh - 130;

        for ( int i = 0; i < themes->Size(); ++i ) {
            if ( themes->ValidIndex (i) ) {

                char *themeName = themes->GetData(i);
                char bname [64];
                UplinkSnprintf ( bname, sizeof ( bname ), "theme %d", i );
                EclRegisterButton ( screenw - 210, screenh - 40, 200, 15, themeName, "Select this theme", bname );
                EclRegisterButtonCallbacks ( bname, ThemeNameDraw, ThemeNameClick, button_click, button_highlight );
                EclRegisterMovement ( bname, screenw - 210, ypos, 500 );
                ypos -= 20;

            }
        }

        // Apply button 

        EclRegisterButton ( screenw - 210, screenh - 40, 200, 15, "Apply", "Set the current theme", "theme_apply" );
        EclRegisterButtonCallback ( "theme_apply", ApplyClick );
        EclRegisterMovement ( "theme_apply", screenw - 210, screenh - 90, 500 );

        // Default button 

        EclRegisterButton ( screenw - 210, screenh - 40, 200, 15, "Restore Default", "Set the theme back to standard", "theme_default" );
        EclRegisterButtonCallback ( "theme_default", DefaultClick );
        EclRegisterMovement ( "theme_default", screenw - 210, screenh - 70, 500 );
        
		// Close button

		EclRegisterButton ( screenw - 60, screenh - 40, 50, 15, "Close", "Return to Options menu", "theme_return" );
		EclRegisterButtonCallback ( "theme_return", ReturnToMainMenuClick );
		
        // Put some icons left of the screen for a sample

        EclRegisterButton ( -150, 300, 220, 120, "userid", " ", "theme_userid" );
        button_assignbitmap ( "theme_userid", "userid.tif" );
        EclRegisterMovement ( "theme_userid", 50, 300, 500 );

        EclRegisterButton ( 290, screenh + 10, 24, 24, "email", " ", "theme_email" );
        button_assignbitmaps ( "theme_email", "hud/email.tif", "hud/email_h.tif", "hud/email_c.tif" );
        EclRegisterMovement ( "theme_email", 290, 300, 500 );

        EclRegisterButton ( 290, screenh + 50, 24, 24, "finance", " ", "theme_finance" );
        button_assignbitmaps ( "theme_finance", "hud/finance.tif", "hud/finance_h.tif", "hud/finance_c.tif" );
        EclRegisterMovement ( "theme_finance", 290, 332, 500 );

        EclRegisterButton ( 290, screenh + 90, 24, 24, "hardware", " ", "theme_hardware" );
        button_assignbitmaps ( "theme_hardware", "hud/hardware.tif", "hud/hardware_h.tif", "hud/hardware_c.tif" );
        EclRegisterMovement ( "theme_hardware", 290, 364, 500 );

        EclRegisterButton ( 290, screenh + 130, 24, 24, "status", " ", "theme_status" );
        button_assignbitmaps ( "theme_status", "hud/status.tif", "hud/status_h.tif", "hud/status_c.tif" );
        EclRegisterMovement ( "theme_status", 290, 396, 500 );

	}

}

void ThemeInterface::Remove ()
{

	if ( IsVisible () ) {

		EclRemoveButton ( "theme_title" );
        EclRemoveButton ( "theme_text" );
        EclRemoveButton ( "theme_name" );
        EclRemoveButton ( "theme_default" );
        EclRemoveButton ( "theme_apply" );
        EclRemoveButton ( "theme_return" );        

        if ( themes ) {

            for ( int i = 0; i < themes->Size(); ++i ) {
                if ( themes->ValidIndex(i) ) {
            
                    char bname [64];
                    UplinkSnprintf ( bname, sizeof ( bname ), "theme %d", i );
                    EclRemoveButton ( bname );

                }
            }
            
			DeleteDArrayData ( themes );
            delete themes;
            themes = NULL;
        }

        EclRemoveButton ( "theme_userid" );
        EclRemoveButton ( "theme_email" );
        EclRemoveButton ( "theme_finance" );
        EclRemoveButton ( "theme_hardware" );
        EclRemoveButton ( "theme_status" );

	}

}

void ThemeInterface::Update ()
{
}

bool ThemeInterface::IsVisible ()
{

	return ( EclGetButton ( "theme_return" ) != NULL );

}


int  ThemeInterface::ScreenID ()
{

	return MAINMENU_THEME;

}

