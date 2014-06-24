
#ifdef WIN32
#include <windows.h>
#endif

#include <strstream>

#include <GL/gl.h>

#include <GL/glu.h> /*_glu_extention_library_*/


#include "vanbakel.h"
#include "eclipse.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"
#include "app/miscutils.h"

#include "game/game.h"

#include "interface/taskmanager/ipprobe.h"

#include "world/world.h"
#include "world/vlocation.h"
#include "world/player.h"
#include "world/message.h"
#include "world/computer/computer.h"
#include "world/computer/securitysystem.h"

#include "mmgr.h"

void IPProbe::DisplayDraw ( Button *button, bool highlighted, bool clicked )
{

	// Draw a grey background

	glBegin ( GL_QUADS );
		glColor4f ( 0.6f, 0.6f, 0.6f, ALPHA );		glVertex2i ( button->x, button->y );
		glColor4f ( 0.4f, 0.4f, 0.4f, ALPHA );		glVertex2i ( button->x + button->width, button->y );
		glColor4f ( 0.6f, 0.6f, 0.6f, ALPHA );		glVertex2i ( button->x + button->width, button->y + button->height );
		glColor4f ( 0.4f, 0.4f, 0.4f, ALPHA );		glVertex2i ( button->x, button->y + button->height );
	glEnd ();

	// Draw ordinary text

	text_draw ( button, highlighted, clicked );

	// Draw a border

	if ( highlighted || clicked ) border_draw ( button );

}

void IPProbe::CloseClick ( Button *button )
{

	int pid;
	sscanf ( button->name, "probe_close %d", &pid );

	SvbRemoveTask ( pid );

}

void IPProbe::GoClick ( Button *button )
{

	int pid;
	sscanf ( button->name, "probe_go %d", &pid );
	char name_display [64];
	UplinkSnprintf ( name_display, sizeof ( name_display ), "probe_display %d", pid );

	IPProbe *task = (IPProbe *) SvbGetTask ( pid );

	if ( task->status == PROBE_IDLE ) {

		task->status = PROBE_INPROGRESS;
		task->timeout = (int) ( time(NULL) + 1 );						// (3 seconds to complete)
		//EclRegisterCaptionChange ( name_display, "Probing IP..." );

	}
	else
		task->status = PROBE_IDLE;

}

IPProbe::IPProbe () : UplinkTask ()
{

	status = PROBE_IDLE;

}

IPProbe::~IPProbe ()
{
}

void IPProbe::Initialise ()
{
}

void IPProbe::Tick ( int n )
{

	if ( IsInterfaceVisible () ) {

		if ( status == PROBE_IDLE ) {

			// Do nothing

		}
		else if ( status == PROBE_INPROGRESS ) {

			if ( time(NULL) >= timeout ) {

				// Try to lookup the IP in the box

				int pid = SvbLookupPID ( this );
				char name_display [64];
				UplinkSnprintf ( name_display, sizeof ( name_display ), "probe_display %d", pid );

				Button *button = EclGetButton ( name_display );
				UplinkAssert ( button );
				char *ip = StripCarriageReturns ( button->caption );

				VLocation *vl = game->GetWorld ()->GetVLocation ( ip );

				if ( vl ) {

					Computer *comp = vl->GetComputer ();
					UplinkAssert (comp);

					std::ostrstream body;
					body << "Results of IP Probe\n\n"
						 << "IP: " << comp->ip << "\n"
						 << "(" << comp->name << ")\n"
						 << "\n";

					for ( int i = 0; i < comp->security.NumSystems (); ++i ) {

						SecuritySystem *ss = comp->security.GetSystem (i);
						if ( ss ) {

							if		( version == 1.0 ) body << ss->GetName () << "\n";
							else if ( version == 2.0 ) body << ss->GetName () << " level " << ss->level << "\n";
							else if ( version == 3.0 ) {
								body << ss->GetName () << " level " << ss->level << "\n";
								if ( ss->bypassed ) body << "bypassed\n";
								if ( ss->enabled  ) body << "enabled\n";
								if ( !ss->enabled ) body << "disabled\n";
							}

						}

					}

					body << '\x0';

					// Found - send a message to the player with the details
					Message *msg = new Message ();
					msg->SetTo ( "PLAYER" );
					msg->SetFrom ( "IP Probe" );
					msg->SetSubject ( "Results of probe" );
					msg->SetBody ( body.str () );
					msg->Send ();

					body.rdbuf()->freeze( 0 );
					//delete [] body.str ();

				}
				else {

					// Not found
					EclRegisterCaptionChange ( name_display, "IP Not found" );

				}

                delete [] ip;
				timeout = (int) ( time(NULL) + 5 );
				status = PROBE_FINISHED;

			}

		}
		else if ( status == PROBE_FINISHED ) {

			// Revert to idle after 5 secs

			if ( time (NULL) > timeout ) {

				int pid = SvbLookupPID ( this );
				char name_display [64];
				UplinkSnprintf ( name_display, sizeof ( name_display ), "probe_display %d", pid );

				EclRegisterCaptionChange ( name_display, "Enter IP" );
				status = PROBE_IDLE;

			}

		}
		else {

			UplinkAbort ( "Unkown Status" );

		}

	}

}

void IPProbe::CreateInterface ()
{

	if ( !IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char name_go [64];
		char name_display [64];
		char name_close [64];

		UplinkSnprintf ( name_go, sizeof ( name_go ), "probe_go %d", pid );
		UplinkSnprintf ( name_display, sizeof ( name_display ), "probe_display %d", pid );
		UplinkSnprintf ( name_close, sizeof ( name_close ), "probe_close %d", pid );

		EclRegisterButton ( 305, 444, 20, 15, "", "Perform the lookup", name_go );
		EclRegisterButton ( 325, 444, 185, 14, "Enter IP", "Type your IP here", name_display );
		EclRegisterButton ( 510, 445, 13, 13, "", "Close the IP Lookup program", name_close );

		button_assignbitmap ( name_go, "software/go.tif" );
		EclRegisterButtonCallback ( name_go, GoClick );
		EclRegisterButtonCallbacks ( name_display, DisplayDraw, NULL, button_click, button_highlight );
		button_assignbitmaps ( name_close, "close.tif", "close_h.tif", "close_c.tif" );
		EclRegisterButtonCallback ( name_close, CloseClick );

		EclMakeButtonEditable ( name_display );

	}

}

void IPProbe::RemoveInterface ()
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char name_go [64];
		char name_display [64];
		char name_close [64];

		UplinkSnprintf ( name_go, sizeof ( name_go ), "probe_go %d", pid );
		UplinkSnprintf ( name_display, sizeof ( name_display ), "probe_display %d", pid );
		UplinkSnprintf ( name_close, sizeof ( name_close ), "probe_close %d", pid );

		EclRemoveButton ( name_go );
		EclRemoveButton ( name_display );
		EclRemoveButton ( name_close );

	}

}

void IPProbe::ShowInterface ()
{

	if ( !IsInterfaceVisible () ) CreateInterface ();

	int pid = SvbLookupPID ( this );

	char name_go [64];
	char name_display [64];
	char name_close [64];

	UplinkSnprintf ( name_go, sizeof ( name_go ), "probe_go %d", pid );
	UplinkSnprintf ( name_display, sizeof ( name_display ), "probe_display %d", pid );
	UplinkSnprintf ( name_close, sizeof ( name_close ), "probe_close %d", pid );

	EclButtonBringToFront ( name_go );
	EclButtonBringToFront ( name_display );
	EclButtonBringToFront ( name_close );

}

bool IPProbe::IsInterfaceVisible ()
{

	int pid = SvbLookupPID ( this );
	char name_display [64];
	UplinkSnprintf ( name_display, sizeof ( name_display ), "probe_display %d", pid );

	return ( EclGetButton ( name_display ) != 0 );

}
