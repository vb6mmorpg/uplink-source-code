
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h> /*_glu_extention_library_*/


#include "eclipse.h"
#include "vanbakel.h"
#include "soundgarden.h"
#include "redshirt.h"

#include "app/app.h"
#include "app/opengl_interface.h"
#include "app/globals.h"

#include "options/options.h"

#include "game/game.h"

#include "world/world.h"
#include "world/player.h"
#include "world/generator/numbergenerator.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/taskmanager/revelation.h"

#include "mmgr.h"


int numRevelationTexts = 13;
char *revelationText[] = {   
                             "YOU ARE NOT A SLAVE",
                             "YOU DESERVE MORE THAN THIS",
                             "THE TIME IS NEAR",
                             "THERE IS NOTHING TO FEAR",
                             "REVELATION IS COMING",
                             "IN TIME YOU WILL THANK US",
                             "YOU ARE MORE THAN A NUMBER",
                             "WE ARE THE FIRST OF THE CHILDREN",
                             "HOPE LIES IN THE RUINS",
                             "THE MACHINE DOES NOT OWN YOU",
                             "OUR SPIRITS ARE BEING CRUSHED",
                             "YOU CANNOT DIGITISE LIFE",
                             "WE WILL SEE YOU ON THE OTHER SIDE",                             
                         };


void Revelation::Initialise ()
{
}

void Revelation::Tick ( int n )
{

	if ( IsInterfaceVisible () ) {

		if ( EclGetAccurateTime() > animationtime + 50 ) {

			// Button Name Format:
			// "revelation pid X Y"
			// X = button number 
			// Y = copy number
	        
			int pid = SvbLookupPID (this);

			for ( int i = 0; i < numcopies; ++i ) {
	    
				char bname [64];
				UplinkSnprintf ( bname, sizeof ( bname ), "revelation %d %d %d", pid, nextnumber, i );

				// Remove existing

				EclRemoveButton ( bname );

				// Create new

				int x = NumberGenerator::RandomNumber ( 640 );
				int y = NumberGenerator::RandomNumber ( 480 );
	            
				int captionIndex = NumberGenerator::RandomNumber ( numRevelationTexts );
				char *caption = revelationText[captionIndex];

				EclRegisterButton ( x, y, 200, 15, caption, " ", bname );
				EclRegisterButtonCallbacks ( bname, DrawRevelation, NULL, NULL, NULL );

				// Play sound

				char filename [256];
				int soundindex = captionIndex;
				if ( soundindex > 9 ) soundindex = 9;
				if ( soundindex < 1 ) soundindex = 1;
				UplinkSnprintf ( filename, sizeof ( filename ), "sounds/%d.wav", soundindex );
				SgPlaySound ( RsArchiveFileOpen ( filename ), filename, true );


			}

			++nextnumber;
			if ( nextnumber > 40 ) nextnumber = 0;
			animationtime = (int) EclGetAccurateTime();

		}

	}
    
	if ( time(NULL) > timesync + 10 ) {

        if ( !game->GetWorld ()->plotgenerator.revelation_releaseuncontrolled ) {

		    // Trash the users gateway

		    game->GetWorld ()->GetPlayer ()->gateway.Nuke ();

		    // Close any open connections

		    game->GetWorld ()->GetPlayer ()->GetConnection ()->Disconnect ();
		    game->GetWorld ()->GetPlayer ()->GetConnection ()->Reset ();

		    game->GetInterface ()->GetRemoteInterface ()->RunNewLocation ();
		    game->GetInterface ()->GetRemoteInterface ()->RunScreen ( 0 );		

            EclReset ( app->GetOptions ()->GetOptionValue ("graphics_screenwidth"),
			           app->GetOptions ()->GetOptionValue ("graphics_screenheight") );

        }

	}

}

void Revelation::DrawRevelation ( Button *button, bool highlighted, bool clicked )
{

    char unused [64];
    int pid;
    int buttonnumber;
    int copynumber;

    sscanf ( button->name, "%s %d %d %d", unused, &pid, &buttonnumber, &copynumber );

    Revelation *thistask = (Revelation *) SvbGetTask (pid);

    if ( thistask ) {

        int difference = ( thistask->nextnumber <= buttonnumber ) ? 
                                buttonnumber - thistask->nextnumber :
                                buttonnumber + ( 40 - thistask->nextnumber );

        float shade = (float) difference / 40.0f;
    
        glColor4f ( shade, shade, shade, 1.0 );
        GciDrawText ( button->x + 5, button->y + 8, button->caption );

    }

}

void Revelation::CreateInterface ()
{

	if ( !IsInterfaceVisible () ) {
		
        timesync = time(NULL);
		animationtime = (int) EclGetAccurateTime ();
        nextnumber = 0;
        numcopies = 4;

        SgPlaySound ( RsArchiveFileOpen ( "sounds/revelation.wav" ), "sounds/revelation.wav", false );
 
	}

}

void Revelation::RemoveInterface ()
{

	if ( IsInterfaceVisible () ) {

        int pid = SvbLookupPID (this);

        for ( int i = 0; i < numcopies; ++i ) {
            for ( int j = 0; j < 40; ++j ) {

                char bname [64];
                UplinkSnprintf ( bname, sizeof ( bname ), "revelation %d %d %d", pid, j, i );
                EclRemoveButton ( bname );

            }
        }

	}

}

void Revelation::ShowInterface ()
{

}


bool Revelation::IsInterfaceVisible ()
{

    int pid = SvbLookupPID (this);
    char bname [64];
    UplinkSnprintf ( bname, sizeof ( bname ), "revelation %d %d %d", pid, 0, 0 );

	return ( EclGetButton ( bname ) != NULL );

}

