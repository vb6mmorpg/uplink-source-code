
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h>


#include "gucci.h"
#include "soundgarden.h"
#include "redshirt.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/miscutils.h"
#include "app/opengl_interface.h"

#include "options/options.h"

#include "game/game.h"
#include "game/data/data.h"

#include "interface/interface.h"
#include "interface/localinterface/localinterface.h"
#include "interface/localinterface/analyser_interface.h"
#include "interface/taskmanager/taskmanager.h"

#include "world/world.h"
#include "world/player.h"
#include "world/computer/computer.h"
#include "world/computer/securitysystem.h"

#include "mmgr.h"


char AnalyserInterface::remotehost [SIZE_VLOCATION_IP];

static int stippleindex = 0;
static int STIPPLE []  = { 0x1111,		//		0001000100010001
						   0x1111,		//		0001000100010001
						   0x2222,		//		0010001000100010
						   0x2222,		//		0010001000100010
						   0x4444,		//		0100010001000100
						   0x4444,		//		0100010001000100
						   0x8888,		//		1000100010001000
						   0x8888 };	//		1000100010001000

int AnalyserInterface::moving_stipplepattern = STIPPLE [0];
int AnalyserInterface::static_stipplepattern = STIPPLE [0];


void AnalyserInterface::CycleStipplePattern ()
{

	--stippleindex;
	if ( stippleindex < 0 ) stippleindex = 7;

	moving_stipplepattern = STIPPLE [stippleindex];

}


void AnalyserInterface::TitleClick ( Button *button )
{

	game->GetInterface ()->GetLocalInterface ()->RunScreen ( SCREEN_NONE );

}

void AnalyserInterface::SystemClick ( Button *button )
{

	int systemindex;
	sscanf ( button->name, "analyser_system %d", &systemindex );

	VLocation *vl = game->GetWorld ()->GetVLocation ( remotehost );
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);

	game->GetInterface ()->GetTaskManager ()->SetProgramTarget ( (UplinkObject *) &(comp->security), button->name, systemindex );

}

void AnalyserInterface::ConnectionDraw ( Button *button, bool highlighted, bool clicked )
{

	int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
	int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");
	int paneltop = (int)(100.0 * ( (screenw * PANELSIZE) / 188.0 ) + 30);
	int panelwidth = (int)(screenw * PANELSIZE);

	//
	// Look up the remote host
	//

	VLocation *vl = game->GetWorld ()->GetVLocation ( remotehost );
	UplinkAssert (vl);
	Computer *comp = vl->GetComputer ();
	UplinkAssert (comp);

	//
	// Draw the standard shaded background
	//

	glBegin ( GL_QUADS );
		SetColour("PanelBackgroundA");		glVertex2i ( button->x, button->y + button->height );
		SetColour("PanelBackgroundB");  	glVertex2i ( button->x, button->y );
		SetColour("PanelBackgroundA");		glVertex2i ( button->x + button->width, button->y );
		SetColour("PanelBackgroundB");		glVertex2i ( button->x + button->width, button->y + button->height );
	glEnd ();

	SetColour("PanelBorder");
	border_draw ( button );

	//
	// Draw connecting lines
	//

	if ( strcmp ( remotehost, IP_LOCALHOST ) != 0 ) {

		glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );
		glLineWidth ( 2 );
		glLineStipple ( 2, moving_stipplepattern );
		glEnable ( GL_LINE_STIPPLE );

/*
		glBegin ( GL_LINE_STRIP );
			glVertex2i ( screenw - panelwidth + 40, paneltop + 75 );
			glVertex2i ( screenw - panelwidth + 40, paneltop + 160 );
			glVertex2i ( screenw - 40, paneltop + 160 );
			glVertex2i ( screenw - 40, paneltop + 240 );				
		glEnd ();
*/
		glBegin ( GL_LINE_STRIP );
			glVertex2i ( screenw - panelwidth + 40, paneltop +  75 );
			glVertex2i ( screenw - 40,				paneltop +  75 );
			glVertex2i ( screenw - 40,				paneltop + 160 );
			glVertex2i ( screenw - panelwidth + 40, paneltop + 160 );
			glVertex2i ( screenw - panelwidth + 40, paneltop + 240 );
			glVertex2i ( screenw - 40,				paneltop + 240 );
		glEnd ();

		glLineWidth ( 1 );
		glDisable ( GL_LINE_STIPPLE );

	}

	//
	// Draw boxes around any systems that are being monitored
	//

	int systemindex = 0;
	char name [32];
	UplinkStrncpy ( name, "analyser_system 0", sizeof ( name ) );

	while ( EclGetButton ( name ) ) {

		if ( SecurityMonitor::IsMonitored ( systemindex ) ) {

			Button *b = EclGetButton (name);

			glColor4f ( 1.0f, 0.1f, 0.1f, 1.0f );
			glLineWidth ( 2 );
			glLineStipple ( 2, moving_stipplepattern );
			glEnable ( GL_LINE_STIPPLE );

			glBegin ( GL_LINE_LOOP );
				glVertex2i ( b->x - 10, b->y - 10 );
				glVertex2i ( b->x + b->width + 10, b->y - 10 );
				glVertex2i ( b->x + b->width + 10, b->y + b->height + 10 );
				glVertex2i ( b->x - 10, b->y + b->height + 10 );
			glEnd ();

			glLineWidth ( 1 );
			glDisable ( GL_LINE_STIPPLE );

		}
		
		++systemindex;
		UplinkSnprintf ( name, sizeof ( name ), "analyser_system %d", systemindex );

	}

}

AnalyserInterface::AnalyserInterface ()
{

	lastupdate = 0;
	stippleupdate = 0;

}

AnalyserInterface::~AnalyserInterface ()
{
}

void AnalyserInterface::Create ()
{

	if ( !IsVisible () ) {

		LocalInterfaceScreen::Create ();
		EclRegisterButtonCallbacks ( "localint_background", ConnectionDraw, NULL, NULL, NULL );

		UplinkStrncpy ( remotehost, " ", sizeof ( remotehost ) );

		int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
		int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");
		int paneltop = (int)(100.0 * ( (screenw * PANELSIZE) / 188.0 )) + 30;
		int panelwidth = (int)(screenw * PANELSIZE);

		EclRegisterButton ( screenw - panelwidth, paneltop + 3, panelwidth - 7, 15, "CONNECTION ANALYSER", "Remove this screen", "analyser_title" );
		EclRegisterButtonCallback ( "analyser_title", TitleClick );
	
		// Local host button

		EclRegisterButton ( screenw - panelwidth + 20, paneltop + 35, 40, 40, "", "Shows your home computer", "analyser_localhost" );
		button_assignbitmaps ( "analyser_localhost", "analyser/localhost.tif", "analyser/localhost_h.tif", "analyser/localhost_c.tif" );

		EclRegisterButton ( screenw - panelwidth + 6, paneltop + 20, 70, 15, "Localhost", "", "analyser_localhost_t" );
		EclRegisterButtonCallbacks ( "analyser_localhost_t", text_draw, NULL, NULL, NULL );

	}

}

void AnalyserInterface::Remove ()
{

	if ( IsVisible () ) {
	
		LocalInterfaceScreen::Remove ();

		EclRemoveButton ( "analyser_title" );

		EclRemoveButton ( "analyser_localhost" );
		EclRemoveButton ( "analyser_localhost_t" );

		EclRemoveButton ( "analyser_remotehost" );
		EclRemoveButton ( "analyser_remotehost_t" );
		EclRemoveButton ( "analyser_remotehost_title" );

		// Remove any security system buttons

		int systemindex = 0;
		char name [32];
		UplinkStrncpy ( name, "analyser_system 0", sizeof ( name ) );

		while ( EclGetButton ( name ) ) {

			EclRemoveButton ( name );

			UplinkSnprintf ( name, sizeof ( name ), "analyser_system_t %d", systemindex );
			EclRemoveButton ( name );

			++systemindex;
			UplinkSnprintf ( name, sizeof ( name ), "analyser_system %d", systemindex );

		}

	}

}

void AnalyserInterface::Update ()
{

	//
	// Has our remotehost changed since we last updated?
	//

	if ( strcmp ( remotehost, game->GetWorld ()->GetPlayer ()->remotehost ) != 0 ) {

		// Yes it has - redo everything
	
		UplinkStrncpy ( remotehost, game->GetWorld ()->GetPlayer ()->remotehost, sizeof ( remotehost ) );

		// Remove the old remote host button

		if ( EclGetButton ( "analyser_remotehost" ) ) {				
			EclRemoveButton ( "analyser_remotehost" );
			EclRemoveButton ( "analyser_remotehost_t" );
			EclRemoveButton ( "analyser_remotehost_title" );
		}

		// Remove any old security system buttons

		int systemindex = 0;
		char name [32];
		UplinkStrncpy ( name, "analyser_system 0", sizeof ( name ) );

		while ( EclGetButton ( name ) ) {

			EclRemoveButton ( name );

			UplinkSnprintf ( name, sizeof ( name ), "analyser_system_t %d", systemindex );
			EclRemoveButton ( name );

			++systemindex;
			UplinkSnprintf ( name, sizeof ( name ), "analyser_system %d", systemindex );

		}

		// Get screen dimensions

		int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
		int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");
		int paneltop = (int)(100.0 * ( (screenw * PANELSIZE) / 188.0 )) + 30;
		int panelwidth = (int)(screenw * PANELSIZE);

		// Look up the new remote host

		VLocation *vl = game->GetWorld ()->GetVLocation ( remotehost );
		UplinkAssert (vl);
		Computer *comp = vl->GetComputer ();
		UplinkAssert (comp);

		// Create the new remote host buttons

		if ( strcmp ( comp->ip, IP_LOCALHOST ) != 0 ) {

			if ( !EclGetButton ( "analyser_remotehost" ) ) {

				EclRegisterButton ( screenw - 60, paneltop + 240, 40, 40, "", "Shows the remote computer target", "analyser_remotehost" );
				button_assignbitmaps ( "analyser_remotehost", "analyser/remotehost.tif", "analyser/remotehost_h.tif", "analyser/remotehost_c.tif" );

				EclRegisterButton ( screenw - 78, paneltop + 281, 70, 15, "Remotehost", "", "analyser_remotehost_t" );
				EclRegisterButtonCallbacks ( "analyser_remotehost_t", text_draw, NULL, NULL, NULL );

				EclRegisterButton ( screenw - panelwidth + 5, paneltop + 240, 110, 40, "", "", "analyser_remotehost_title" );
				EclRegisterButtonCallbacks ( "analyser_remotehost_title", text_draw, NULL, NULL, NULL );

			}

		}
		
		EclRegisterCaptionChange ( "analyser_remotehost_title", comp->name );

		// Create the new security system buttons

		if ( comp->security.NumSystems () == 1 ) {

			EclRegisterButton ( screenw - 110, paneltop + 145, 32, 32, "", "Shows a security system", "analyser_system 0" );			
			button_assignbitmaps_blend ( "analyser_system 0", "analyser/unknown.tif", "analyser/unknown.tif", "analyser/unknown.tif" );
			EclRegisterButtonCallback ( "analyser_system 0", SystemClick );

			EclRegisterButton ( screenw - 120, paneltop + 180, 70, 15, "Analysing", "", "analyser_system_t 0" );
			EclRegisterButtonCallbacks ( "analyser_system_t 0", text_draw, NULL, NULL, NULL );			

		}
		else if ( comp->security.NumSystems () == 2 ) {

			EclRegisterButton ( screenw - panelwidth + 25, paneltop + 145, 32, 32, "", "Shows a security system", "analyser_system 0" );
			button_assignbitmaps_blend ( "analyser_system 0", "analyser/unknown.tif", "analyser/unknown.tif", "analyser/unknown.tif" );
			EclRegisterButtonCallback ( "analyser_system 0", SystemClick );

			EclRegisterButton ( screenw - panelwidth + 15, paneltop + 180, 70, 15, "Analysing", "", "analyser_system_t 0" );
			EclRegisterButtonCallbacks ( "analyser_system_t 0", text_draw, NULL, NULL, NULL );

			EclRegisterButton ( screenw - 55, paneltop + 145, 32, 32, "", "Shows a security system", "analyser_system 1" );
			button_assignbitmaps_blend ( "analyser_system 1", "analyser/unknown.tif", "analyser/unknown.tif", "analyser/unknown.tif" );
			EclRegisterButtonCallback ( "analyser_system 1", SystemClick );

			EclRegisterButton ( screenw - 65, paneltop + 130, 70, 15, "Analysing", "", "analyser_system_t 1" );
			EclRegisterButtonCallbacks ( "analyser_system_t 1", text_draw, NULL, NULL, NULL );

		}
		else if ( comp->security.NumSystems () == 3 ) {

			EclRegisterButton ( screenw - panelwidth + 25, paneltop + 145, 32, 32, "", "Shows a security system", "analyser_system 0" );
			button_assignbitmaps_blend ( "analyser_system 0", "analyser/unknown.tif", "analyser/unknown.tif", "analyser/unknown.tif" );
			EclRegisterButtonCallback ( "analyser_system 0", SystemClick );

			EclRegisterButton ( screenw - panelwidth + 15, paneltop + 180, 70, 15, "Analysing", "", "analyser_system_t 0" );
			EclRegisterButtonCallbacks ( "analyser_system_t 0", text_draw, NULL, NULL, NULL );

			EclRegisterButton ( screenw - panelwidth / 2 - 16, paneltop + 145, 32, 32, "", "Shows a security system", "analyser_system 1" );			
			button_assignbitmaps_blend ( "analyser_system 1", "analyser/unknown.tif", "analyser/unknown.tif", "analyser/unknown.tif" );
			EclRegisterButtonCallback ( "analyser_system 1", SystemClick );

			EclRegisterButton ( screenw - panelwidth / 2 - 26, paneltop + 180, 70, 15, "Analysing", "", "analyser_system_t 1" );
			EclRegisterButtonCallbacks ( "analyser_system_t 1", text_draw, NULL, NULL, NULL );

			EclRegisterButton ( screenw - 55, paneltop + 145, 32, 32, "", "Shows a security system", "analyser_system 2" );
			button_assignbitmaps_blend ( "analyser_system 2", "analyser/unknown.tif", "analyser/unknown.tif", "analyser/unknown.tif" );
			EclRegisterButtonCallback ( "analyser_system 2", SystemClick );
	
			EclRegisterButton ( screenw - 65, paneltop + 130, 70, 15, "Analysing", "", "analyser_system_t 2" );
			EclRegisterButtonCallbacks ( "analyser_system_t 2", text_draw, NULL, NULL, NULL );

		}
		else if ( comp->security.NumSystems () == 4 ) {

			EclRegisterButton ( screenw - panelwidth + 25, paneltop + 145, 32, 32, "", "Shows a security system", "analyser_system 0" );
			button_assignbitmaps_blend ( "analyser_system 0", "analyser/unknown.tif", "analyser/unknown.tif", "analyser/unknown.tif" );
			EclRegisterButtonCallback ( "analyser_system 0", SystemClick );

			EclRegisterButton ( screenw - panelwidth + 15, paneltop + 180, 70, 15, "Analysing", "", "analyser_system_t 0" );
			EclRegisterButtonCallbacks ( "analyser_system_t 0", text_draw, NULL, NULL, NULL );

			EclRegisterButton ( screenw - panelwidth / 2 - 16, paneltop + 145, 32, 32, "", "Shows a security system", "analyser_system 1" );			
			button_assignbitmaps_blend ( "analyser_system 1", "analyser/unknown.tif", "analyser/unknown.tif", "analyser/unknown.tif" );
			EclRegisterButtonCallback ( "analyser_system 1", SystemClick );

			EclRegisterButton ( screenw - panelwidth / 2 - 26, paneltop + 180, 70, 15, "Analysing", "", "analyser_system_t 1" );
			EclRegisterButtonCallbacks ( "analyser_system_t 1", text_draw, NULL, NULL, NULL );

			EclRegisterButton ( screenw - 55, paneltop + 145, 32, 32, "", "Shows a security system", "analyser_system 2" );
			button_assignbitmaps_blend ( "analyser_system 2", "analyser/unknown.tif", "analyser/unknown.tif", "analyser/unknown.tif" );
			EclRegisterButtonCallback ( "analyser_system 2", SystemClick );
	
			EclRegisterButton ( screenw - 65, paneltop + 130, 70, 15, "Analysing", "", "analyser_system_t 2" );
			EclRegisterButtonCallbacks ( "analyser_system_t 2", text_draw, NULL, NULL, NULL );

			EclRegisterButton ( screenw - 55, paneltop + 60, 32, 32, "", "Shows a security system", "analyser_system 3" );
			button_assignbitmaps_blend ( "analyser_system 3", "analyser/unknown.tif", "analyser/unknown.tif", "analyser/unknown.tif" );
			EclRegisterButtonCallback ( "analyser_system 3", SystemClick );
	
			EclRegisterButton ( screenw - 65, paneltop + 117, 70, 15, "Analysing", "", "analyser_system_t 3" );
			EclRegisterButtonCallbacks ( "analyser_system_t 3", text_draw, NULL, NULL, NULL );
		}
		// Bring all bypassers to the front

		SvbShowAllTasks ();


		// Set up last update for a 3 second delay

		lastupdate = time(NULL);

	}

	//
	// Update security system buttons with status
	// Every few seconds
	//

	if ( time(NULL) > lastupdate + 3 ) {

		//
		// Look up the host computer
		//

		VLocation *vl = game->GetWorld ()->GetVLocation ( remotehost );
		UplinkAssert (vl);
		Computer *comp = vl->GetComputer ();
		UplinkAssert (comp);

		int systemindex = 0;
		char name [32];
		UplinkStrncpy ( name, "analyser_system 0", sizeof ( name ) );

		while ( EclGetButton ( name ) ) {

			SecuritySystem *ss = comp->security.GetSystem (systemindex);

			if ( ss && ss->enabled && !ss->bypassed ) 
				button_assignbitmaps_blend ( name, "analyser/secure.tif", "analyser/secure_h.tif", "analyser/secure_c.tif" );

			else
				button_assignbitmaps_blend ( name, "analyser/nonsecure.tif", "analyser/nonsecure_h.tif", "analyser/nonsecure_c.tif" );

			EclRegisterButtonCallback ( name, SystemClick );

			UplinkSnprintf ( name, sizeof ( name ), "analyser_system_t %d", systemindex );
			EclGetButton ( name )->SetCaption ( ss->GetName () );

			++systemindex;
			UplinkSnprintf ( name, sizeof ( name ), "analyser_system %d", systemindex );

		}
	
		lastupdate = time(NULL);

	}


	// Update the stipple patterns
	// AT A FIXED RATE ;)

	if ( EclGetAccurateTime () > stippleupdate ) {

		CycleStipplePattern ();
		EclDirtyButton ( "localint_background" );
		stippleupdate = (int)EclGetAccurateTime () + 20;			

	}
	
}

bool AnalyserInterface::IsVisible ()
{

	return ( EclGetButton ( "analyser_title" ) != NULL );

}

int AnalyserInterface::ScreenID ()
{

	return SCREEN_ANALYSER;

}

