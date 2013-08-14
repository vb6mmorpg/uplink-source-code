
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h> /* glu extention library */

#include <math.h>


#include "eclipse.h"
#include "soundgarden.h"
#include "redshirt.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"

#include "mainmenu/mainmenu.h"
#include "mainmenu/closing_interface.h"

#include "world/generator/numbergenerator.h"

#include "mmgr.h"


ClosingInterface::ClosingInterface ()
{

    timesync = 0;
    status = 0;
    count = 0;

}

ClosingInterface::~ClosingInterface ()
{
}

void ClosingInterface::LargeTextDraw ( Button *button, bool highlighted, bool clicked )
{

    UplinkAssert (button);

    clear_draw ( button->x, button->y, button->width, button->height );

	glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );
	int ypos = (button->y + button->height / 2) + 5;
	GciDrawText ( button->x, ypos, button->caption, HELVETICA_18 );

}

void ClosingInterface::Create ()
{

    EclRegisterButton ( 260, 200, 125, 20, " ", " ", "closing_text" );
    EclRegisterButtonCallbacks ( "closing_text", LargeTextDraw, NULL, NULL, NULL );

}

void ClosingInterface::Remove ()
{

    if ( IsVisible () ) {

        EclRemoveButton ( "closing_text" );
        EclRemoveButton ( "closing_www" );
        EclRemoveButton ( "closing_couk" );
        EclRemoveButton ( "closing_time" );

    }

}

void ClosingInterface::Update ()
{

    if ( EclGetAccurateTime () > timesync ) {

        Button *button = EclGetButton ( "closing_text" );
        UplinkAssert (button);

        char *captions [] = { "Judgement", "Perception",
                              "Thinking", "Feeling",
                              "Sensation", "Intuition",
                              "Extraversion" };

        switch ( status ) {

            case 0:         
                timesync = (int)EclGetAccurateTime () + 1000;
                status = 1;
                break;

            case 1:
            {
                int captionNumber = count % 7;                       // (int) fmod(count, 7);
                button->SetCaption ( captions[captionNumber] );
                char filename [256];
                UplinkSnprintf ( filename, sizeof ( filename ), "sounds/%d.wav", NumberGenerator::RandomNumber (9) + 1 );
                SgPlaySound ( RsArchiveFileOpen ( filename ), filename, true );
                timesync = (int)EclGetAccurateTime () + 160;
                count++;
                if ( count > 12 ) status = 2;
                break;
            }

            case 2:
            {
                button->SetCaption ( "INTROVERSION" );
                char filename [256];
                UplinkSnprintf ( filename, sizeof ( filename ), "sounds/%d.wav", NumberGenerator::RandomNumber (9) + 1 );
                SgPlaySound ( RsArchiveFileOpen ( filename ), filename, true );       
                timesync = (int)EclGetAccurateTime () + 1000;
                status++;
                break;
            }

            case 3:
            {

                EclRegisterButton ( 220, 205, 40, 15, "www.", " ", "closing_www" );
                EclRegisterButtonCallbacks ( "closing_www", textbutton_draw, NULL, NULL, NULL );
                
                EclRegisterButton ( 375, 205, 40, 15, ".co.uk", " ", "closing_couk" );
                EclRegisterButtonCallbacks ( "closing_couk", textbutton_draw, NULL, NULL, NULL );

                EclRegisterButton ( 120, 460, 500, 15, " ", " ", "closing_time" );
                EclRegisterButtonCallbacks ( "closing_time", textbutton_draw, NULL, NULL, NULL );
                int playtime = int( ((float)(EclGetAccurateTime () - app->starttime)/1000.0)/60.0 );
                char closingtime[128];
                UplinkSnprintf ( closingtime, sizeof ( closingtime ), "You've been using Uplink for %d minutes. Visit our web site to buy the full product.", playtime );
                EclRegisterCaptionChange ( "closing_time", closingtime, 1500 );

                EclButtonBringToFront ( "closing_text" );

                SgPlaySound ( RsArchiveFileOpen ( "sounds/ringout.wav" ), "sounds/ringout.wav", true );

                timesync = (int)EclGetAccurateTime () + 3000;
                status++;
                break;

            }
            case 4:
                SgPlaySound ( RsArchiveFileOpen ( "sounds/ringout.wav" ), "sounds/ringout.wav", true );
                timesync = (int)EclGetAccurateTime () + 3000;
                status++;
                break;
                
            case 5:
                status++;
				//This is bad, closing the game in the middle of a Update.
                app->Close ();
                break;

        };

    }

}

bool ClosingInterface::IsVisible ()
{

    return ( EclGetButton ( "closing_text" ) != NULL );
        
}

int  ClosingInterface::ScreenID ()
{

    return MAINMENU_CLOSING;
        
}

