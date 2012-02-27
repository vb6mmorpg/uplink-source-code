
#ifdef WIN32
#include <windows.h>
#endif

#ifndef HAVE_GLES
#include <GL/gl.h>
#include <GL/glu.h> /*_glu_extention_library_*/
#else
#include <GLES/gl.h>
#include <GLES/glues.h>
#endif


#include "vanbakel.h"
#include "eclipse.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"
#include "app/miscutils.h"

#include "game/game.h"

#include "interface/taskmanager/iplookup.h"

#include "world/world.h"
#include "world/vlocation.h"
#include "world/player.h"

#include "mmgr.h"

void IPLookup::DisplayDraw ( Button *button, bool highlighted, bool clicked )
{

	// Draw a grey background

#ifndef HAVE_GLES
	glBegin ( GL_QUADS );
		glColor4f ( 0.6f, 0.6f, 0.6f, ALPHA );		glVertex2i ( button->x, button->y );
		glColor4f ( 0.4f, 0.4f, 0.4f, ALPHA );		glVertex2i ( button->x + button->width, button->y );
		glColor4f ( 0.6f, 0.6f, 0.6f, ALPHA );		glVertex2i ( button->x + button->width, button->y + button->height );
		glColor4f ( 0.4f, 0.4f, 0.4f, ALPHA );		glVertex2i ( button->x, button->y + button->height );
	glEnd ();
#else
	GLfloat verts[] = {
		button->x, button->y,
		button->x + button->width, button->y,
		button->x + button->width, button->y + button->height,
		button->x, button->y + button->height
	};

	GLfloat colors[] = {
		0.6f, 0.6f, 0.6f, ALPHA,
		0.4f, 0.4f, 0.4f, ALPHA,
		0.6f, 0.6f, 0.6f, ALPHA,
		0.4f, 0.4f, 0.4f, ALPHA
	};

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glVertexPointer(2, GL_FLOAT, 0, verts);
	glColorPointer(4, GL_FLOAT, 0, colors);

	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
#endif

	// Draw ordinary text

	text_draw ( button, highlighted, clicked );

	// Draw a border

	if ( highlighted || clicked ) border_draw ( button );

}

void IPLookup::CloseClick ( Button *button )
{

	int pid;
	sscanf ( button->name, "iplookup_close %d", &pid );

	SvbRemoveTask ( pid );

}

void IPLookup::GoClick ( Button *button )
{

	int pid;
	sscanf ( button->name, "iplookup_go %d", &pid );

	IPLookup *task = (IPLookup *) SvbGetTask ( pid );

	if ( task->status == IPLOOKUP_IDLE )
		task->status = IPLOOKUP_INPROGRESS;

	else
		task->status = IPLOOKUP_IDLE;

}

IPLookup::IPLookup () : UplinkTask ()
{

	status = IPLOOKUP_IDLE;

}

IPLookup::~IPLookup ()
{
}

void IPLookup::Initialise ()
{
}

void IPLookup::Tick ( int n )
{

	if ( IsInterfaceVisible () ) {

		if ( status == IPLOOKUP_IDLE ) {

			// Do nothing

		}
		else if ( status == IPLOOKUP_INPROGRESS ) {

			// Try to lookup the IP in the box

			int pid = SvbLookupPID ( this );
			char name_display [64];
			UplinkSnprintf ( name_display, sizeof ( name_display ), "iplookup_display %d", pid );
			
			Button *button = EclGetButton ( name_display );
			UplinkAssert ( button );
			char *ip = StripCarriageReturns (button->caption);

			VLocation *vl = game->GetWorld ()->GetVLocation ( ip );

			if ( vl ) {

				// Found - add to links
				EclRegisterCaptionChange ( name_display, vl->computer );
				game->GetWorld ()->GetPlayer ()->GiveLink ( ip );

			}
			else {

				// Not found
				EclRegisterCaptionChange ( name_display, "IP Not found" );

			}

            delete [] ip;
			timeout = time(NULL);
			status = IPLOOKUP_FINISHED;


		}
		else if ( status == IPLOOKUP_FINISHED ) {

			// Revert to idle after 5 secs

			if ( time (NULL) > timeout + 5 ) {

				int pid = SvbLookupPID ( this );
				char name_display [64];
				UplinkSnprintf ( name_display, sizeof ( name_display ), "iplookup_display %d", pid );

				EclRegisterCaptionChange ( name_display, "Enter IP" );
				status = IPLOOKUP_IDLE;
				
			}

		}
		else {

			UplinkAbort ( "Unkown Status" );

		}

	}

}

void IPLookup::CreateInterface ()
{
	
	if ( !IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char name_go [64];
		char name_display [64];
		char name_close [64];

		UplinkSnprintf ( name_go, sizeof ( name_go ), "iplookup_go %d", pid );
		UplinkSnprintf ( name_display, sizeof ( name_display ), "iplookup_display %d", pid );
		UplinkSnprintf ( name_close, sizeof ( name_close ), "iplookup_close %d", pid );

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

void IPLookup::RemoveInterface ()
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char name_go [64];
		char name_display [64];
		char name_close [64];

		UplinkSnprintf ( name_go, sizeof ( name_go ), "iplookup_go %d", pid );
		UplinkSnprintf ( name_display, sizeof ( name_display ), "iplookup_display %d", pid );
		UplinkSnprintf ( name_close, sizeof ( name_close ), "iplookup_close %d", pid );

		EclRemoveButton ( name_go );
		EclRemoveButton ( name_display );
		EclRemoveButton ( name_close );

	}

}

void IPLookup::ShowInterface ()
{

	if ( !IsInterfaceVisible () ) CreateInterface ();

	int pid = SvbLookupPID ( this );

	char name_go [64];
	char name_display [64];
	char name_close [64];

	UplinkSnprintf ( name_go, sizeof ( name_go ), "iplookup_go %d", pid );
	UplinkSnprintf ( name_display, sizeof ( name_display ), "iplookup_display %d", pid );
	UplinkSnprintf ( name_close, sizeof ( name_close ), "iplookup_close %d", pid );

	EclButtonBringToFront ( name_go );
	EclButtonBringToFront ( name_display );
	EclButtonBringToFront ( name_close );


}

bool IPLookup::IsInterfaceVisible ()
{

	int pid = SvbLookupPID ( this );
	char name_display [64];
	UplinkSnprintf ( name_display, sizeof ( name_display ), "iplookup_display %d", pid );

	return ( EclGetButton ( name_display ) != 0 );

}
