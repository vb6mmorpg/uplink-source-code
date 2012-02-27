
#ifdef WIN32
#include <windows.h>
#endif

#include <strstream>

#ifndef HAVE_GLES
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <GLES/gl.h>
#include <GLES/glues.h>
#endif


#include "eclipse.h"
#include "vanbakel.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"

#include "game/game.h"

#include "world/world.h"
#include "world/player.h"
#include "world/vlocation.h"
#include "world/computer/computer.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/taskmanager/taskmanager.h"
#include "interface/taskmanager/revelationtracker.h"

#include "mmgr.h"

RevelationTracker::RevelationTracker ()
{

    timesync = 0;
    height = 0;

}

RevelationTracker::~RevelationTracker ()
{
}

void RevelationTracker::Initialise ()
{
}

void RevelationTracker::Tick ( int n )
{

	if ( IsInterfaceVisible () ) {

		if ( time(NULL) > timesync + 3 ) {

			int pid = SvbLookupPID ( this );
			char boxname [128];
			UplinkSnprintf ( boxname, sizeof ( boxname ), "revelationtracker_box %d", pid );
			Button *button = EclGetButton ( boxname );
			UplinkAssert(button);

			//
			// How many computers are infected?

			int numinfected = game->GetWorld ()->plotgenerator.infected.Size ();

			if ( numinfected != height ) {

				// Resize the box

				EclRegisterResize ( boxname, button->width, 15 + numinfected * 15, 1000 );
				height = numinfected;

			}

			//
			// Rebuild the caption

			if ( numinfected == 0 ) {

				button->SetCaption ( "No infected systems" );

			}
			else {

				std::ostrstream newcaption;

				newcaption << "Infected systems" << "\n";

				for ( int i = 0; i < game->GetWorld ()->plotgenerator.infected.Size (); ++i ) {

					char *ip = game->GetWorld ()->plotgenerator.infected.GetData (i);
					UplinkAssert (ip);

					VLocation *vl = game->GetWorld ()->GetVLocation (ip);
					UplinkAssert (vl);

					Computer *comp = vl->GetComputer ();
					UplinkAssert (comp);

					if ( comp->isinfected_revelation > 0.0 ) {

						newcaption << comp->ip;
						newcaption << " at ";
						newcaption << comp->infectiondate.GetHour () << ":" << comp->infectiondate.GetMinute ();
						newcaption << "\n";

					}

				}

				newcaption << '\x0';

				button->SetCaption ( newcaption.str () );

				newcaption.rdbuf()->freeze( 0 );
				//delete [] newcaption.str ();

			}

		}

	}

}


void RevelationTracker::TitleClick ( Button *button )
{

    UplinkAssert (button);

    char unused [128];
    int pid;
    sscanf ( button->name, "%s %d", unused, &pid );

    game->GetInterface ()->GetTaskManager ()->SetTargetProgram ( pid );

}

void RevelationTracker::CloseClick ( Button *button )
{

	int pid;
	char bname [128];
	sscanf ( button->name, "%s %d", bname, &pid );

	SvbRemoveTask ( pid );

}

void RevelationTracker::MainBoxDraw ( Button *button, bool highlighted, bool clicked )
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

        GLubyte colors[] = {
                8, 20, 0, 255,
                8, 20, 124, 255,
                8, 20, 0, 255,
                8, 20, 124
        };

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(2, GL_FLOAT, 0, verts);
	glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
#endif


	glColor4ub ( 81, 138, 215, 255 );
	border_draw ( button );

    text_draw ( button, highlighted, clicked );

}

void RevelationTracker::CreateInterface ()
{

	if ( !IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

        char titlename [128];
        char boxname [128];
        char closename [128];

        UplinkSnprintf ( titlename, sizeof ( titlename ), "revelationtracker_title %d", pid );
        UplinkSnprintf ( boxname, sizeof ( boxname ), "revelationtracker_box %d", pid );
        UplinkSnprintf ( closename, sizeof ( closename ), "revaltiontracker_close %d", pid );

        EclRegisterButton ( 0, 50, 130, 15, "Revelation Tracker", "Shows all currently infected computers", titlename );
        EclRegisterButton ( 130, 51, 13, 13, "X", "Close the Revelation Tracker", closename );
        EclRegisterButton ( 0, 65, 143, 30, "Listening...", " ", boxname );

        button_assignbitmaps ( closename, "close.tif", "close_h.tif", "close_c.tif" );

        EclRegisterButtonCallback ( titlename, TitleClick );
        EclRegisterButtonCallback ( closename, CloseClick );
        EclRegisterButtonCallbacks ( boxname, MainBoxDraw, NULL, NULL, NULL );

		timesync = time(NULL);
        height = 0;

	}

}

void RevelationTracker::RemoveInterface ()
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

        char titlename [128];
        char boxname [128];
        char closename [128];

        UplinkSnprintf ( titlename, sizeof ( titlename ), "revelationtracker_title %d", pid );
        UplinkSnprintf ( boxname, sizeof ( boxname ), "revelationtracker_box %d", pid );
        UplinkSnprintf ( closename, sizeof ( closename ), "revaltiontracker_close %d", pid );

        EclRemoveButton ( titlename );
        EclRemoveButton ( boxname );
        EclRemoveButton ( closename );

	}

}

void RevelationTracker::ShowInterface ()
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

        char titlename [128];
        char boxname [128];
        char closename [128];

        UplinkSnprintf ( titlename, sizeof ( titlename ), "revelationtracker_title %d", pid );
        UplinkSnprintf ( boxname, sizeof ( boxname ), "revelationtracker_box %d", pid );
        UplinkSnprintf ( closename, sizeof ( closename ), "revaltiontracker_close %d", pid );

        EclButtonBringToFront ( titlename );
        EclButtonBringToFront ( boxname );
        EclButtonBringToFront ( closename );

	}

}

void RevelationTracker::MoveTo ( int x, int y, int time_ms )
{

    if ( IsInterfaceVisible () ) {

        int pid = SvbLookupPID ( this );

        char titlename [128];
        char boxname [128];
        char closename [128];

        UplinkSnprintf ( titlename, sizeof ( titlename ), "revelationtracker_title %d", pid );
        UplinkSnprintf ( boxname, sizeof ( boxname ), "revelationtracker_box %d", pid );
        UplinkSnprintf ( closename, sizeof ( closename ), "revaltiontracker_close %d", pid );

        EclRegisterMovement ( titlename, x, y, time_ms );
        EclRegisterMovement ( boxname, x, y + 15, time_ms );
        EclRegisterMovement ( closename, x + 130, y + 1, time_ms );

    }

}

bool RevelationTracker::IsInterfaceVisible ()
{

	int pid = SvbLookupPID ( this );
    char titlename [128];
    UplinkSnprintf ( titlename, sizeof ( titlename ), "revelationtracker_title %d", pid );

	return ( EclGetButton ( titlename ) != NULL );

}
