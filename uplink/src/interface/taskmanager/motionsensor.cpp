
#ifdef WIN32
#include <windows.h>
#endif

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

#include "interface/taskmanager/motionsensor.h"

#include "mmgr.h"


void MotionSensor::SensorDraw ( Button *button, bool highlighted, bool clicked )
{

	//
	// Get the number of nearby people from the caption
	//

	int numpeople;
	sscanf ( button->caption, "%d", &numpeople );

	// 
	// Draw a coloured box depending on the number of people
	//

	if ( numpeople == 0 )
		glColor4f ( 0.2f, 0.2f, 0.2f, 1.0f );

	else if ( numpeople == 1 ) 
		glColor4f ( 0.9f, 0.9f, 0.2f, 1.0f );

	else if ( numpeople >= 2 ) 
		glColor4f ( 1.0f, 0.3f, 0.3f, 1.0f );

#ifndef HAVE_GLES
	glBegin ( GL_QUADS );
		glVertex2d ( button->x, button->y );
		glVertex2d ( button->x + button->width, button->y );
		glVertex2d ( button->x + button->width, button->y + button->height );
		glVertex2d ( button->x, button->y + button->height );
	glEnd ();
#else
	GLfloat verts[] = {
		button->x, button->y,
		button->x, button->width, button->y,
		button->x + button->width, button->y + button->height,
		button->x, button->y + button->height
	};

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, verts);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDisableClientState(GL_VERTEX_ARRAY);
#endif


	// 
	// Border if mouse over
	//

	if ( highlighted || clicked ) {
		glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );
		border_draw ( button );
	}

}

void MotionSensor::SensorClose ( Button *button )
{

	// Close it down

	int pid;
	sscanf ( button->name, "motionsensor_main %d", &pid );

	SvbRemoveTask ( pid );

}

MotionSensor::MotionSensor () : UplinkTask ()
{

	oldnumpeople = 0;

}

MotionSensor::~MotionSensor ()
{
}

void MotionSensor::SetTarget ( UplinkObject *uo, char *uos, int uoi )
{
}

void MotionSensor::MoveTo ( int x, int y, int time_ms )
{
}


void MotionSensor::Initialise ()
{
}

void MotionSensor::Tick ( int n )
{

	if ( IsInterfaceVisible () ) {

		// No point in doing this n times

		int numpeople = game->GetWorld ()->GetPlayer ()->gateway.proximity;
		
		if ( game->IsRunning () && numpeople != oldnumpeople ) {
			game->SetGameSpeed ( GAMESPEED_NORMAL );
			EclDirtyButton ( "hud_speed 0" );
			EclDirtyButton ( "hud_speed 1" );
			EclDirtyButton ( "hud_speed 2" );
			EclDirtyButton ( "hud_speed 3" );
		}
		else return;

		oldnumpeople = numpeople;

		char caption [16];
		UplinkSnprintf ( caption, sizeof ( caption ), "%d", numpeople);

		int pid = SvbLookupPID ( this );

		char bname [32];
		UplinkSnprintf ( bname, sizeof ( bname ), "motionsensor_main %d", pid );

		UplinkAssert ( EclGetButton ( bname ) );
		EclGetButton ( bname )->SetCaption ( caption );

		if		( numpeople == 0 ) EclGetButton ( bname )->SetTooltip ( "There is nobody near your Gateway" );
		else if ( numpeople == 1 ) EclGetButton ( bname )->SetTooltip ( "There is someone near your Gateway" );
		else if ( numpeople == 2 ) EclGetButton ( bname )->SetTooltip ( "There is more than one person near your Gateway" );
		else if ( numpeople >= 3 ) EclGetButton ( bname )->SetTooltip ( "There are a lot of people near your Gateway" );

	}

}
	
void MotionSensor::CreateInterface ()
{

	if ( !IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char bname [32];
		UplinkSnprintf ( bname, sizeof ( bname ), "motionsensor_main %d", pid );

		EclRegisterButton ( 280, 20, 40, 5, "0", "Shows the status of the Motion sensor on your Gateway", bname );
		EclRegisterButtonCallbacks ( bname, SensorDraw, SensorClose, button_click, button_highlight );

	}

}

void MotionSensor::RemoveInterface ()
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char bname [32];
		UplinkSnprintf ( bname, sizeof ( bname ), "motionsensor_main %d", pid );

		EclRemoveButton ( bname );

	}

}

void MotionSensor::ShowInterface ()
{

	if ( !IsInterfaceVisible () ) CreateInterface ();

	int pid = SvbLookupPID ( this );

	char bname [32];
	UplinkSnprintf ( bname, sizeof ( bname ), "motionsensor_main %d", pid );

	EclButtonBringToFront ( bname );

}

bool MotionSensor::IsInterfaceVisible ()
{

	int pid = SvbLookupPID ( this );
	char bname [32];
	UplinkSnprintf ( bname, sizeof ( bname ), "motionsensor_main %d", pid );

	return ( EclGetButton ( bname ) != NULL );

}

