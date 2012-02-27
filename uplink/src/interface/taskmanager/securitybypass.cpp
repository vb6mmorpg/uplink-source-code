
#include "vanbakel.h"
#include "eclipse.h"


#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"
#include "app/uplinkobject.h"

#include "game/game.h"

#include "world/world.h"
#include "world/player.h"
#include "world/computer/security.h"
#include "world/computer/securitysystem.h"


#include "interface/interface.h"
#include "interface/taskmanager/taskmanager.h"
#include "interface/taskmanager/securitybypass.h"
#include "interface/localinterface/localinterface.h"

#include "mmgr.h"


void SecurityBypass::PauseClick ( Button *button )
{

	UplinkAssert (button);

	char name [32];
	int pid;
	sscanf ( button->name, "%s %d", name, &pid );

	SecurityBypass *pb = (SecurityBypass *) SvbGetTask ( pid );
	UplinkAssert (pb);

	if		( pb->status == 1 ) pb->PauseCurrentBypass ();
	else if ( pb->status == 2 ) pb->ResumeCurrentBypass ();

}

void SecurityBypass::CloseClick ( Button *button )
{

	UplinkAssert (button);

	char name [32];
	int pid;
	sscanf ( button->name, "%s %d", name, &pid );

	SecurityBypass *pb = (SecurityBypass *) SvbGetTask ( pid );
	UplinkAssert (pb);
	pb->EndCurrentBypass ();

	SvbRemoveTask ( pid );

}

void SecurityBypass::BypassClick ( Button *button )
{

	UplinkAssert (button);

	char name [32];
	int pid;
	sscanf ( button->name, "%s %d", name, &pid );

	game->GetInterface ()->GetTaskManager ()->SetTargetProgram ( pid );

}

void SecurityBypass::StatusLightDraw ( Button *button, bool highlighted, bool clicked )
{

	char name [32];
	int pid;
	sscanf ( button->name, "%s %d", name, &pid );

	if ( ShouldDraw ( pid ) ) {

		SecurityBypass *pb = (SecurityBypass *) SvbGetTask ( pid );
		UplinkAssert (pb);

		if ( pb->status > 0 ) {

			UplinkAssert ( pb->targetobject );
			SecuritySystem *ss = ((Security *) pb->targetobject)->GetSystem (pb->targetint);
			UplinkAssert (ss);

			if ( ss->bypassed || !ss->enabled )													//
				button->image_standard->Draw ( button->x, button->y );							//
																								// C L E V E R ! ! ! 
			else																				//
				button->image_highlighted->Draw ( button->x, button->y );						//

		}
		else {

			button->image_highlighted->Draw ( button->x, button->y );						

		}

	}

}

void SecurityBypass::ImageButtonDraw ( Button *button, bool highlighted, bool clicked )
{

	char name [32];
	int pid;
	sscanf ( button->name, "%s %d", name, &pid );

	if ( ShouldDraw ( pid ) ) {

		imagebutton_draw ( button, highlighted, clicked );

	}

}

bool SecurityBypass::ShouldDraw ( int pid )
{

	SecurityBypass *pb = (SecurityBypass *) SvbGetTask ( pid );
	if ( !pb || pb->status == 0 ) // 0 = static, 1 = working
		return true;

	LocalInterface *li = game->GetInterface()->GetLocalInterface();
	UplinkTask *ut;
	if ( ( li && li->InScreen() == SCREEN_ANALYSER && li->IsVisible() ) ||
	     ( game->IsRunning() && ( ut = game->GetInterface()->GetTaskManager()->GetTargetProgram() ) &&
	       ut->GetPID() == pid ) ) {

		return true;

	}

	return false;

}

SecurityBypass::SecurityBypass () : UplinkTask ()
{

	status = 0;
		
}

SecurityBypass::SecurityBypass ( int newTYPE )
{

	status = 0;
	SetTYPE ( newTYPE );

}

SecurityBypass::~SecurityBypass ()
{
}

void SecurityBypass::SetTYPE ( int newTYPE )
{

	TYPE = newTYPE;

}

void SecurityBypass::PauseCurrentBypass ()
{

	if ( status == 1 ) {

		UplinkAssert ( targetobject );
		UplinkAssert ( ((Security *) targetobject)->GetSystem (targetint) );
		((Security *) targetobject)->GetSystem (targetint)->EndBypass ();
		
		status = 2;

	}
	else {

		printf ( "WARNING : SecurityBypass::PauseCurrentBypass, no bypass in existence!\n" );

	}

}

void SecurityBypass::ResumeCurrentBypass ()
{

	if ( status == 2 ) {

		UplinkAssert ( targetobject );
		UplinkAssert ( ((Security *) targetobject)->GetSystem (targetint) );
		((Security *) targetobject)->GetSystem (targetint)->Bypass ();
		
		status = 1;

	}
	else {

		printf ( "WARNING : SecurityBypass::ResumeCurrentBypass, not paused to begin with!\n" );

	}

}

void SecurityBypass::EndCurrentBypass ()
{

	if ( status > 0 ) {

		UplinkAssert ( targetobject );
		UplinkAssert ( ((Security *) targetobject)->GetSystem (targetint) );
		((Security *) targetobject)->GetSystem (targetint)->EndBypass ();
		
		targetobject = NULL;
		targetint = 0;
		status = 0;

	}

}

void SecurityBypass::SetTarget ( UplinkObject *uo, char *uos, int uoi )
{

	/*
		uo  : Security object that belongs to target computer
		uos : Button name representing system in connection analyser
		uoi : index of security system 

		*/

	UplinkAssert (uo);

	if ( strcmp ( uo->GetID (), "SECUR" ) == 0 ) {

		UplinkAssert ( ((Security *) uo)->GetSystem (uoi) );

		if ( ((Security *) uo)->GetSystem (uoi)->TYPE == TYPE ) {

			//
			// If we were already running, stop
			//

			EndCurrentBypass ();

			//
			// Move the button into place
			//

			Button *b = EclGetButton ( uos );
			UplinkAssert (b);
			MoveTo ( b->x, b->y + b->height, 1000 );

			//
			// Store our target
			//

			targetobject = uo;
			targetint = uoi;

			UplinkStrncpy ( ip, game->GetWorld ()->GetPlayer ()->remotehost, sizeof ( ip ) );


			//
			// Start it running
			//

			if ( version >= ((Security *) targetobject)->GetSystem (targetint)->level ) {

				((Security *) targetobject)->GetSystem (targetint)->Bypass ();
				status = 1;

			}
			else {

				status = 0;

			}

		}

	}

}

void SecurityBypass::MoveTo ( int x, int y, int time_ms )
{

	// X and Y of bottom left corner

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char main [32];
		char pause [32];
		char close [32];
		char title [32];
		char status [32];

		UplinkSnprintf ( main, sizeof ( main ), "sbypass_main %d", pid );
		UplinkSnprintf ( pause, sizeof ( pause ), "sbypass_pause %d", pid );
		UplinkSnprintf ( close, sizeof ( close ), "sbypass_close %d",  pid );
		UplinkSnprintf ( title, sizeof ( title ), "sbypass_title %d", pid );
		UplinkSnprintf ( status, sizeof ( status ), "sbypass_status %d", pid );

		EclButtonBringToFront ( main );
		EclButtonBringToFront ( pause );
		EclButtonBringToFront ( close );
		EclButtonBringToFront ( title );
		EclButtonBringToFront ( status );
		
//		EclRegisterMovement ( main,   x,	  y - 32, 1000, MOVE_STRAIGHTLINE );
///		EclRegisterMovement ( pause,  x + 1,  y - 31, 1000, MOVE_STRAIGHTLINE );
//		EclRegisterMovement ( close,  x + 19, y - 31, 1000, MOVE_STRAIGHTLINE );
//		EclRegisterMovement ( title,  x + 1,  y - 13, 1000, MOVE_STRAIGHTLINE );
//		EclRegisterMovement ( status, x + 19, y - 13, 1000, MOVE_STRAIGHTLINE );

		EclRegisterMovement ( main,   x,	  y - 32, time_ms );
		EclRegisterMovement ( pause,  x + 1,  y - 31, time_ms );
		EclRegisterMovement ( close,  x + 19, y - 31, time_ms );
		EclRegisterMovement ( title,  x + 1,  y - 13, time_ms );
		EclRegisterMovement ( status, x + 19, y - 13, time_ms );

	}

}

void SecurityBypass::Initialise ()
{
}

void SecurityBypass::Tick ( int n )
{

	//
	// Switch off if we have disconnected
	//

	if ( status > 0 ) {

		if ( strcmp ( game->GetWorld ()->GetPlayer ()->remotehost, ip ) != 0 ) {

			EndCurrentBypass ();

			SvbRemoveTask ( SvbLookupPID ( this ) );

		}

	}

}

void SecurityBypass::CreateInterface ()
{

	if ( !IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char main [32];
		char pause [32];
		char close [32];
		char title [32];
		char status [32];

		UplinkSnprintf ( main, sizeof ( main ), "sbypass_main %d", pid );
		UplinkSnprintf ( pause, sizeof ( pause ), "sbypass_pause %d", pid );
		UplinkSnprintf ( close, sizeof ( close ), "sbypass_close %d",  pid );
		UplinkSnprintf ( title, sizeof ( title ), "sbypass_title %d", pid );
		UplinkSnprintf ( status, sizeof ( status ), "sbypass_status %d", pid );


		/********** Patched by François Gagné **********/
		// To be complete, each type would need a picture just like the proxy bypass (software/proxy.tif)

		char *bypassCaption, *pauseCaption, *closeCaption;
		switch ( TYPE ) {
		  case SECURITY_TYPE_PROXY:
			bypassCaption = "Click to use this proxy bypass";
			pauseCaption = "Pause the proxy bypass";
			closeCaption = "Close the proxy bypass";
			break;
		  case SECURITY_TYPE_FIREWALL:
			bypassCaption = "Click to use this firewall bypass";
			pauseCaption = "Pause the firewall bypass";
			closeCaption = "Close the firewall bypass";
			break;
		  case SECURITY_TYPE_ENCRYPTION:
			bypassCaption = "Click to use this encryption bypass";
			pauseCaption = "Pause the encryption bypass";
			closeCaption = "Close the encryption bypass";
			break;
		  case SECURITY_TYPE_MONITOR:
			bypassCaption = "Click to use this monitor bypass";
			pauseCaption = "Pause the monitor bypass";
			closeCaption = "Close the monitor bypass";
			break;
		  default: // SECURITY_TYPE_NONE
			bypassCaption = "Click to use this bypass";
			pauseCaption = "Pause the bypass";
			closeCaption = "Close the bypass";
		}

		EclRegisterButton ( 100, 100, 32, 32, "Bypass", bypassCaption, main ); 
		button_assignbitmaps ( main, "software/proxybypass.tif", "software/proxybypass.tif", "software/proxybypass.tif" );
		//EclRegisterButtonCallback ( main, BypassClick );
		EclRegisterButtonCallbacks ( main, ImageButtonDraw, BypassClick, NULL, NULL );
		
		EclRegisterButton ( 101, 101, 12, 12, "P", pauseCaption, pause );
		button_assignbitmaps ( pause, "software/pause.tif", "software/pause_h.tif", "software/pause_c.tif" );
		//EclRegisterButtonCallback ( pause, PauseClick );
		EclRegisterButtonCallbacks ( pause, ImageButtonDraw, PauseClick, NULL, NULL );

		EclRegisterButton ( 119, 101, 12, 12, "", closeCaption, close );
		button_assignbitmaps ( close, "close.tif", "close_h.tif", "close_c.tif" );
		EclGetButton ( close )->image_standard->Scale	 ( 12, 12 );
		EclGetButton ( close )->image_highlighted->Scale ( 12, 12 );
		EclGetButton ( close )->image_clicked->Scale	 ( 12, 12 );
		//EclRegisterButtonCallback ( close, CloseClick );
		EclRegisterButtonCallbacks ( close, ImageButtonDraw, CloseClick, NULL, NULL );

		EclRegisterButton ( 101, 119, 12, 12, "", "", title );
		button_assignbitmap ( title, "software/proxy.tif" );
		//EclRegisterButtonCallbacks ( title, imagebutton_draw, NULL, NULL, NULL );
		EclRegisterButtonCallbacks ( title, ImageButtonDraw, NULL, NULL, NULL );

		EclRegisterButton ( 119, 119, 12, 12, "", "", status );
		button_assignbitmaps ( status, "software/greenlight.tif", "software/redlight.tif", "software/redlight.tif" );
		EclRegisterButtonCallbacks ( status, StatusLightDraw, NULL, NULL, NULL );

	}

}

void SecurityBypass::RemoveInterface ()
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char main [32];
		char pause [32];
		char close [32];
		char title [32];
		char status [32];

		UplinkSnprintf ( main, sizeof ( main ), "sbypass_main %d", pid );
		UplinkSnprintf ( pause, sizeof ( pause ), "sbypass_pause %d", pid );
		UplinkSnprintf ( close, sizeof ( close ), "sbypass_close %d",  pid );
		UplinkSnprintf ( title, sizeof ( title ), "sbypass_title %d", pid );
		UplinkSnprintf ( status, sizeof ( status ), "sbypass_status %d", pid );
		
		EclRemoveButton ( main );
		EclRemoveButton ( pause );
		EclRemoveButton ( close );
		EclRemoveButton ( title );
		EclRemoveButton ( status );
		
	}

}

void SecurityBypass::ShowInterface ()
{

	if ( !IsInterfaceVisible () ) CreateInterface ();

	int pid = SvbLookupPID ( this );

	char main [32];
	char pause [32];
	char close [32];
	char title [32];
	char status [32];

	UplinkSnprintf ( main, sizeof ( main ), "sbypass_main %d", pid );
	UplinkSnprintf ( pause, sizeof ( pause ), "sbypass_pause %d", pid );
	UplinkSnprintf ( close, sizeof ( close ), "sbypass_close %d",  pid );
	UplinkSnprintf ( title, sizeof ( title ), "sbypass_title %d", pid );
	UplinkSnprintf ( status, sizeof ( status ), "sbypass_status %d", pid );
	
	/********** Patched by François Gagné **********/
	if ( ShouldDraw ( pid ) ) {

		EclButtonBringToFront ( main );
		EclButtonBringToFront ( pause );
		EclButtonBringToFront ( close );
		EclButtonBringToFront ( title );
		EclButtonBringToFront ( status );

	} else {

		EclButtonSendToBack ( status );
		EclButtonSendToBack ( title );
		EclButtonSendToBack ( close );
		EclButtonSendToBack ( pause );
		EclButtonSendToBack ( main );

	}

}

bool SecurityBypass::IsInterfaceVisible ()
{

	int pid = SvbLookupPID ( this );
	char main [32];
	UplinkSnprintf ( main, sizeof ( main ), "sbypass_main %d", pid );

	return ( EclGetButton ( main ) != 0 );

}
