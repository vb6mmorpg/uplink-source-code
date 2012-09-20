
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h> /*_glu_extention_library_*/


#include "vanbakel.h"
#include "eclipse.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"
#include "app/miscutils.h"

#include "game/game.h"
#include "options/options.h"

#include "world/computer/computer.h"
#include "world/computer/securitysystem.h"

#include "world/world.h"
#include "world/vlocation.h"
#include "world/player.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/taskmanager/taskmanager.h"
#include "interface/taskmanager/denial.h"

void Denial::DisplayDraw ( Button *button, bool highlighted, bool clicked )
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

void Denial::MainBoxDraw ( Button *button, bool highlighted, bool clicked )
{

	glBegin ( GL_QUADS );		
		glColor3ub ( 8, 20, 0 );		glVertex2i ( button->x, button->y + button->height );
		glColor3ub ( 8, 20, 124 );		glVertex2i ( button->x, button->y );
		glColor3ub ( 8, 20, 0 );		glVertex2i ( button->x + button->width, button->y );
		glColor3ub ( 8, 20, 124 );		glVertex2i ( button->x + button->width, button->y + button->height );
	glEnd ();

	glColor3ub ( 81, 138, 215 );
	border_draw ( button );
	
    text_draw ( button, highlighted, clicked );
}

void Denial::TitleClick ( Button *button )
{

    UplinkAssert (button);

    char unused [128];
    int pid;
    sscanf ( button->name, "%s %d", unused, &pid );

    game->GetInterface ()->GetTaskManager ()->SetTargetProgram ( pid );

}
void Denial::CloseClick ( Button *button )
{

	int pid;
	sscanf ( button->name, "denial_close %d", &pid );

	SvbRemoveTask ( pid );

}

void Denial::GoClick ( Button *button )
{

	int pid;
	sscanf ( button->name, "denial_go %d", &pid );

	Denial *task = (Denial *) SvbGetTask ( pid );

	if ( task->status == DENIAL_IDLE )
		task->status = DENIAL_INPROGRESS;

	else
		task->status = DENIAL_IDLE;

}

Denial::Denial () : UplinkTask ()
{

	status = DENIAL_IDLE;
	strcpy(myCap,"x");

}

Denial::~Denial ()
{
}

void Denial::Initialise ()
{
}

void Denial::Tick ( int n )
{

	if ( IsInterfaceVisible () ) {

		if ( status == DENIAL_IDLE ) {
			int pid = SvbLookupPID ( this );
			// While idle, keep track of the number of clients

			char lowername [64];
			char lowercap [64];
			sprintf ( lowername, "denial_lower %d", pid );
			Button *lower = EclGetButton(lowername);
			UplinkAssert(lower);

			int iCount = 0;
			DArray <Computer *> *comps = game->GetWorld ()->computers.ConvertToDArray ();
			UplinkAssert (comps);

			for ( int i = 0; i < comps->Size (); ++i ) {
				if ( comps->ValidIndex (i) ) {
					Computer *remotecomp = comps->GetData (i);
					UplinkAssert (remotecomp);
					if ( remotecomp->iTSR & TSR_DENIAL  ) { iCount++; }
				}
			}
			delete comps;

			sprintf(lowercap, "%d clients online", iCount);
			if ( strcmp(myCap, lowercap) != 0 ) {
				EclRegisterCaptionChange(lowername, lowercap);
				sprintf(myCap, "%d clients online", iCount);
			}
		}
		else if ( status == DENIAL_INPROGRESS ) {

			// Try to lookup the IP in the box

			int pid = SvbLookupPID ( this );
			char name_display [64];
			sprintf ( name_display, "denial_display %d", pid );
			
			Button *button = EclGetButton ( name_display );
			UplinkAssert ( button );
			char *ip = StripCarriageReturns (button->caption);

			VLocation *vl = game->GetWorld ()->GetVLocation ( ip );

			if ( vl ) {
				
				int iCount = -5;
				DArray <Computer *> *comps = game->GetWorld ()->computers.ConvertToDArray ();
				UplinkAssert (comps);

				for ( int i = 0; i < comps->Size (); ++i ) {
					if ( comps->ValidIndex (i) ) {

						Computer *remotecomp = comps->GetData (i);
						UplinkAssert (remotecomp);

						if ( remotecomp->iTSR & TSR_DENIAL  )
						{
							remotecomp->iTSR = 0;
							remotecomp->ChangeSecurityCodes ();
							iCount++;
						}

					}
				}
				
				delete comps;
				Computer *comp = vl->GetComputer();

				if ( !comp->isexternallyopen ) { iCount /= 2; }
				for ( int j = 0; j < comp->security.NumSystems(); j++ ) {
					iCount -= comp->security.GetSystem(j)->level;
				}
				if ( iCount >= 0 ) {
					comp->SetIsRunning(false);
					comp->RunTSR(TSR_DENIED); // For news reports
					comp->LoadTSR();
					EclRegisterCaptionChange(button->name,"System Denied!");
				} else {
					EclRegisterCaptionChange(button->name,"Denial Failed");
				}
				if ( strcmp(comp->name,"Gateway") == 0 ) {
					comp->SetIsRunning(true); // It is probably bad if your gateway isnt running
					game->GetWorld ()->GetPlayer ()->gateway.GenerateNewID();

					game->GetWorld ()->GetPlayer ()->GetConnection ()->Disconnect ();
					game->GetWorld ()->GetPlayer ()->GetConnection ()->Reset ();

					game->GetInterface ()->GetRemoteInterface ()->RunNewLocation ();
					game->GetInterface ()->GetRemoteInterface ()->RunScreen ( 0 );		

					EclReset ( app->GetOptions ()->GetOptionValue ("graphics_screenwidth"),
							   app->GetOptions ()->GetOptionValue ("graphics_screenheight") );
						
				}
			}
			else {

				// Not found
				EclRegisterCaptionChange ( name_display, "IP Not found" );

			}

            delete [] ip;
			timeout = (int) time(NULL);
			status = DENIAL_FINISHED;


		}
		else if ( status == DENIAL_FINISHED ) {

			// Revert to idle after 5 secs

			if ( time (NULL) > timeout + 5 ) {

				int pid = SvbLookupPID ( this );
				char name_display [64];
				sprintf ( name_display, "denial_display %d", pid );

				EclRegisterCaptionChange ( name_display, "Enter Target IP" );
				status = DENIAL_IDLE;
				
			}

		}
		else {

			UplinkAbort ( "Unkown Status" );

		}

	}

}

void Denial::CreateInterface ()
{
	
	if ( !IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char name_go [64];
		char name_display [64];
		char name_close [64];
		char titlename [64];
		char lowername [64];
		char boxname [64];

		sprintf ( boxname, "denial_box %d", pid );
		sprintf ( lowername, "denial_lower %d", pid );
		sprintf ( titlename, "denial_title %d", pid );
		sprintf ( name_go, "denial_go %d", pid );
		sprintf ( name_display, "denial_display %d", pid );
		sprintf ( name_close, "denial_close %d", pid );

		EclRegisterButton ( 305, 430, 220, 60, " ", " ", boxname);
        EclRegisterButton ( 305, 430, 207, 15, "Denial", " ", titlename );
		EclRegisterButton ( 305, 476, 20, 15, "", "Deny Service", name_go );
		EclRegisterButton ( 325, 452, 185, 14, "Enter Target IP", "Type target IP here", name_display );
		EclRegisterButton ( 512, 430, 13, 13, "", "Close the Denial server", name_close );
        EclRegisterButton ( 325, 475, 200, 15, "0 Clients Online", " ", lowername );

		button_assignbitmap ( name_go, "software/go.tif" );
		EclRegisterButtonCallback ( name_go, GoClick );
		EclRegisterButtonCallbacks ( name_display, DisplayDraw, NULL, button_click, button_highlight );
		button_assignbitmaps ( name_close, "close.tif", "close_h.tif", "close_c.tif" );
		EclRegisterButtonCallback ( name_close, CloseClick );
		EclRegisterButtonCallback ( titlename, TitleClick );
        EclRegisterButtonCallbacks ( boxname, MainBoxDraw, NULL, NULL, NULL );

		EclMakeButtonEditable ( name_display );

	}

}

void Denial::RemoveInterface ()
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char name_go [64];
		char name_display [64];
		char name_close [64];
		char titlename [64];
		char lowername [64];
		char boxname [64];

		sprintf ( boxname, "denial_box %d", pid );
		sprintf ( lowername, "denial_lower %d", pid );
		sprintf ( titlename, "denial_title %d", pid );
		sprintf ( name_go, "denial_go %d", pid );
		sprintf ( name_display, "denial_display %d", pid );
		sprintf ( name_close, "denial_close %d", pid );

		EclRemoveButton ( boxname );
		EclRemoveButton ( titlename );
		EclRemoveButton ( name_go );
		EclRemoveButton ( name_display );
		EclRemoveButton ( name_close );
		EclRemoveButton ( lowername );
	}

}

void Denial::ShowInterface ()
{

	if ( !IsInterfaceVisible () ) CreateInterface ();

	int pid = SvbLookupPID ( this );

	char name_go [64];
	char name_display [64];
	char name_close [64];
	char titlename [64];
	char lowername [64];
	char boxname [64];

	sprintf ( boxname, "denial_box %d", pid );
	sprintf ( lowername, "denial_lower %d", pid );
	sprintf ( titlename, "denial_title %d", pid );
	sprintf ( name_go, "denial_go %d", pid );
	sprintf ( name_display, "denial_display %d", pid );
	sprintf ( name_close, "denial_close %d", pid );

	EclButtonBringToFront ( boxname );
	EclButtonBringToFront ( titlename );
	EclButtonBringToFront ( name_go );
	EclButtonBringToFront ( name_display );
	EclButtonBringToFront ( name_close );
	EclButtonBringToFront ( lowername );

}

bool Denial::IsInterfaceVisible ()
{

	int pid = SvbLookupPID ( this );
	char name_display [64];
	sprintf ( name_display, "denial_display %d", pid );

	return ( EclGetButton ( name_display ) != 0 );

}

void Denial::MoveTo ( int x, int y, int time_ms )
{

    if ( IsInterfaceVisible () ) {

        int pid = SvbLookupPID ( this );

		char name_go [64];
		char name_display [64];
		char name_close [64];
		char titlename [64];
		char lowername [64];
		char boxname [64];

		sprintf ( boxname, "denial_box %d", pid );
		sprintf ( lowername, "denial_lower %d", pid );
		sprintf ( titlename, "denial_title %d", pid );
		sprintf ( name_go, "denial_go %d", pid );
		sprintf ( name_display, "denial_display %d", pid );
		sprintf ( name_close, "denial_close %d", pid );

		EclRegisterMovement ( boxname,      x,       y,      time_ms );
        EclRegisterMovement ( titlename,    x,       y,      time_ms );
		EclRegisterMovement ( name_go,      x,       y + 45, time_ms );
		EclRegisterMovement ( name_display, x + 20,  y + 22, time_ms );
		EclRegisterMovement ( name_close,   x + 207, y,      time_ms );
		EclRegisterMovement ( lowername,    x + 20,  y + 45, time_ms );
    }

}