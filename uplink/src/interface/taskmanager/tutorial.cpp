

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

#include "vanbakel.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"
#include "app/serialise.h"

#include "game/game.h"
#include "game/data/data.h"

#include "world/world.h"
#include "world/player.h"
#include "world/vlocation.h"
#include "world/message.h"
#include "world/company/mission.h"
#include "world/computer/computerscreen/computerscreen.h"

#include "interface/interface.h"
#include "interface/localinterface/localinterface.h"
#include "interface/localinterface/hud_interface.h"
#include "interface/localinterface/worldmap/worldmap_interface.h"
#include "interface/localinterface/sendmail_interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/taskmanager/taskmanager.h"
#include "interface/taskmanager/tutorial.h"
#include "interface/taskmanager/filecopier.h"

#include "mmgr.h"


Tutorial::Tutorial ()
{

	current_tutorial = 0;
	current_part = 0;
	nextclicked = false;
	menu = false;

}

Tutorial::~Tutorial ()
{
}

void Tutorial::CloseClick ( Button *button )
{

	UplinkAssert (button);

	char name [64];
	int pid;
	sscanf ( button->name, "%s %d", name, &pid );

	SvbRemoveTask ( pid );

}

void Tutorial::TitleClick ( Button *button )
{

	UplinkAssert (button);

	char name [64];
	int pid;
	sscanf ( button->name, "%s %d", name, &pid );

	Tutorial *thistask = (Tutorial *) SvbGetTask ( pid );

	thistask->followmouse = true;
	game->GetInterface ()->GetTaskManager ()->SetTargetProgram ( pid );

}

void Tutorial::SkipClick ( Button *button )
{

	UplinkAssert (button);

	char name [64];
	int pid;
	sscanf ( button->name, "%s %d", name, &pid );

	Tutorial *thistask = (Tutorial *) SvbGetTask ( pid );

	thistask->RunMenu ();

}

void Tutorial::NextDraw ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert (button);

	char name [64];
	int pid;
	sscanf ( button->name, "%s %d", name, &pid );

	Tutorial *thistask = (Tutorial *) SvbGetTask ( pid );

	if ( thistask->nextvisible ) {

		button_draw ( button, highlighted, clicked );

	}

}

void Tutorial::NextMouseMove ( Button *button )
{

	UplinkAssert (button);

	char name [64];
	int pid;
	sscanf ( button->name, "%s %d", name, &pid );

	Tutorial *thistask = (Tutorial *) SvbGetTask ( pid );

	if ( thistask->nextvisible ) {

		button_highlight ( button );

	}

}

void Tutorial::NextMouseDown ( Button *button )
{

	UplinkAssert (button);

	char name [64];
	int pid;
	sscanf ( button->name, "%s %d", name, &pid );

	Tutorial *thistask = (Tutorial *) SvbGetTask ( pid );

	if ( thistask->nextvisible ) {

		button_click ( button );

	}

}

void Tutorial::NextClick ( Button *button )
{

	UplinkAssert (button);

	char name [64];
	int pid;
	sscanf ( button->name, "%s %d", name, &pid );

	Tutorial *thistask = (Tutorial *) SvbGetTask ( pid );

	if ( thistask->nextvisible ) {
		
		thistask->nextclicked = true;

	}

}

void Tutorial::MenuButtonClick ( Button *button )
{

	char name [64];
	int buttonindex;
	int pid;

	sscanf ( button->name, "%s %d %d", name, &buttonindex, &pid );


	// Run that part of the tutorial

	Tutorial *thistask = (Tutorial *) SvbGetTask (pid);
	UplinkAssert (thistask);
	thistask->RemoveMenuInterface ();
	thistask->menu = false;
	thistask->RunTutorial ( buttonindex, 1 );

}

void Tutorial::MainTextDraw ( Button *button, bool highlighted, bool clicked )
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
                8, 20, 124, 255
        };

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        glVertexPointer(2, GL_FLOAT, 0, verts);
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, colors);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
#endif


	glColor4ub ( 81, 138, 215, 255 );
	border_draw ( button );

	text_draw ( button, highlighted, clicked );

}


void Tutorial::Initialise ()
{
}

void Tutorial::Tick ( int n )
{

	if ( !menu && HasCompletedCurrentSection () ) {

		RunTutorial ( current_tutorial, current_part + 1 );

	}

}

void Tutorial::MoveTo ( int x, int y, int time_ms )
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char title [64];
		char skip [64];
		char close [64];
		char text [64];
		char next [64];

		UplinkSnprintf ( title, sizeof ( title ), "tutorial_title %d", pid );
		UplinkSnprintf ( skip, sizeof ( skip ), "tutorial_skip %d", pid );
		UplinkSnprintf ( close, sizeof ( close ), "tutorial_close %d", pid );
		UplinkSnprintf ( text, sizeof ( text ), "tutorial_text %d", pid );
		UplinkSnprintf ( next, sizeof ( next ), "tutorial_next %d", pid );

		EclRegisterMovement ( title, x + 1, y + 1, time_ms );
		EclRegisterMovement ( skip, x + 236, y + 1, time_ms );
		EclRegisterMovement ( close, x + 276, y + 1, time_ms );
		EclRegisterMovement ( text, x + 1, y + 13, time_ms );
		EclRegisterMovement ( next, x + 247, y + 47, time_ms );

		EclButtonBringToFront ( title );
		EclButtonBringToFront ( close );
		EclButtonBringToFront ( text );
		EclButtonBringToFront ( next );

		if ( IsMenuInterfaceVisible () ) {

			MoveMenuTo ( x, y, time_ms );

		}

	}

}

void Tutorial::CreateInterface ()
{

	if ( !IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char title [64];
		char skip [64];
		char close [64];
		char text [64];
		char next [64];

		UplinkSnprintf ( title, sizeof ( title ), "tutorial_title %d", pid );
		UplinkSnprintf ( skip, sizeof ( skip ), "tutorial_skip %d", pid );
		UplinkSnprintf ( close, sizeof ( close ), "tutorial_close %d", pid );
		UplinkSnprintf ( text, sizeof ( text ), "tutorial_text %d", pid );
		UplinkSnprintf ( next, sizeof ( next ), "tutorial_next %d", pid );

		EclRegisterButton ( 200, 400, 235, 13, "Tutorial Daemon", "The tutorial software", title );
		EclRegisterButtonCallback ( title, TitleClick );
		
		EclRegisterButton ( 435, 400, 40, 13, "Skip", "Skip this section of the tutorial", skip );
		EclRegisterButtonCallback ( skip, SkipClick );

		EclRegisterButton ( 475, 400, 13, 13, "Close", "Close the tutorial software", close );
		button_assignbitmaps ( close, "close.tif", "close_h.tif", "close_c.tif" );
		EclRegisterButtonCallback ( close, CloseClick );
		
		EclRegisterButton ( 200, 413, 288, 50, "Starting...", "", text );
		EclRegisterButtonCallbacks ( text, MainTextDraw, NULL, NULL, NULL );

		EclRegisterButton ( 447, 447, 40, 15, "Next", "Click to go on to the next stage", next );
		EclRegisterButtonCallbacks ( next, NextDraw, NextClick, NextMouseDown, NextMouseMove );
				
		// Run tutorial 1, part 1
		RunTutorial ( 1, 1 );

		//RunMenu ();

	}

}

void Tutorial::RemoveInterface ()
{
	
	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char title [64];
		char skip [64];
		char close [64];
		char text [64];
		char next [64];

		UplinkSnprintf ( title, sizeof ( title ), "tutorial_title %d", pid );
		UplinkSnprintf ( skip, sizeof ( skip ), "tutorial_skip %d", pid );
		UplinkSnprintf ( close, sizeof ( close ), "tutorial_close %d", pid );
		UplinkSnprintf ( text, sizeof ( text ), "tutorial_text %d", pid );
		UplinkSnprintf ( next, sizeof ( next ), "tutorial_next %d", pid );

		EclRemoveButton ( title );
		EclRemoveButton ( skip );
		EclRemoveButton ( close );
		EclRemoveButton ( text );
		EclRemoveButton ( next );

		if ( IsMenuInterfaceVisible () ) {

			RemoveMenuInterface ();

		}

	}

}

void Tutorial::ShowInterface ()
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char title [64];
		char skip [64];
		char close [64];
		char text [64];
		char next [64];

		UplinkSnprintf ( title, sizeof ( title ), "tutorial_title %d", pid );
		UplinkSnprintf ( skip, sizeof ( skip ), "tutorial_skip %d", pid );
		UplinkSnprintf ( close, sizeof ( close ), "tutorial_close %d", pid );
		UplinkSnprintf ( text, sizeof ( text ), "tutorial_text %d", pid );
		UplinkSnprintf ( next, sizeof ( next ), "tutorial_next %d", pid );

		EclButtonBringToFront ( title );
		EclButtonBringToFront ( skip );
		EclButtonBringToFront ( close );
		EclButtonBringToFront ( text );
		EclButtonBringToFront ( next );

		if ( IsMenuInterfaceVisible () ) {

			ShowMenuInterface ();

		}

	}

}

bool Tutorial::IsInterfaceVisible ()
{

	int pid = SvbLookupPID ( this );
	char title [64];
	UplinkSnprintf ( title, sizeof ( title ), "tutorial_title %d", pid );

	return ( EclGetButton ( title ) != NULL );

}

void Tutorial::MoveMenuTo ( int x, int y, int time_ms )
{

	if ( IsMenuInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char part1 [64];
		char part2 [64];
		char part3 [64];
		char part1button [64];
		char part2button [64];
		char part3button [64];

		UplinkSnprintf ( part1, sizeof ( part1 ), "tutorial_part1 1 %d", pid );
		UplinkSnprintf ( part2, sizeof ( part2 ), "tutorial_part2 2 %d", pid );
		UplinkSnprintf ( part3, sizeof ( part3 ), "tutorial_part3 3 %d", pid );
		UplinkSnprintf ( part1button, sizeof ( part1button ), "tutorial_part1button 1 %d", pid );
		UplinkSnprintf ( part2button, sizeof ( part2button ), "tutorial_part2button 2 %d", pid );
		UplinkSnprintf ( part3button, sizeof ( part3button ), "tutorial_part3button 3 %d", pid );

		EclRegisterMovement ( part1button, x + 10, y + 15 + 0, time_ms );
		EclRegisterMovement ( part2button, x + 10, y + 15 + 16, time_ms );
		EclRegisterMovement ( part3button, x + 10, y + 15 + 32, time_ms );

		EclRegisterMovement ( part1, x + 90, y + 15 + 0, time_ms );
		EclRegisterMovement ( part2, x + 90, y + 15 + 16, time_ms );
		EclRegisterMovement ( part3, x + 90, y + 15 + 32, time_ms );

		EclButtonBringToFront ( part1 );
		EclButtonBringToFront ( part2 );
		EclButtonBringToFront ( part3 );
		EclButtonBringToFront ( part1button );
		EclButtonBringToFront ( part2button );
		EclButtonBringToFront ( part3button );

	}

}

void Tutorial::CreateMenuInterface ()
{

	if ( !IsMenuInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char title [64];
		UplinkSnprintf ( title, sizeof ( title ), "tutorial_title %d", pid );
		Button *titlebutton = EclGetButton ( title );
		UplinkAssert (titlebutton);

		//
		// Add the new buttons in
		//

		char part1 [64];
		char part2 [64];
		char part3 [64];
		char part1button [64];
		char part2button [64];
		char part3button [64];

		UplinkSnprintf ( part1, sizeof ( part1 ), "tutorial_part1 1 %d", pid );
		UplinkSnprintf ( part2, sizeof ( part2 ), "tutorial_part2 2 %d", pid );
		UplinkSnprintf ( part3, sizeof ( part3 ), "tutorial_part3 3 %d", pid );
		UplinkSnprintf ( part1button, sizeof ( part1button ), "tutorial_part1button 1 %d", pid );
		UplinkSnprintf ( part2button, sizeof ( part2button ), "tutorial_part2button 2 %d", pid );
		UplinkSnprintf ( part3button, sizeof ( part3button ), "tutorial_part3button 3 %d", pid );

		EclRegisterButton ( titlebutton->x + 10, titlebutton->y + 15 + 0, 80, 14, "Tutorial One", "Run the first tutorial", part1button );
		EclRegisterButton ( titlebutton->x + 10, titlebutton->y + 15 + 16, 80, 14, "Tutorial Two", "Run the second tutorial", part2button );
		EclRegisterButton ( titlebutton->x + 10, titlebutton->y + 15 + 32, 80, 14, "Tutorial Three", "Run the third tutorial", part3button );

		EclRegisterButtonCallback ( part1button, MenuButtonClick );
		EclRegisterButtonCallback ( part2button, MenuButtonClick );
		EclRegisterButtonCallback ( part3button, MenuButtonClick );

		EclRegisterButton ( titlebutton->x + 90, titlebutton->y + 15 + 0, 190, 14, "The basics of your screen", "Run the first tutorial", part1 );
		EclRegisterButton ( titlebutton->x + 90, titlebutton->y + 15 + 16, 190, 14, "Completing the first mission", "Run the second tutorial", part2 );
		EclRegisterButton ( titlebutton->x + 90, titlebutton->y + 15 + 32, 150, 14, "Working as an Agent", "Run the third tutorial", part3 );

		EclRegisterButtonCallbacks ( part1, text_draw, MenuButtonClick, button_click, button_highlight );
		EclRegisterButtonCallbacks ( part2, text_draw, MenuButtonClick, button_click, button_highlight );
		EclRegisterButtonCallbacks ( part3, text_draw, MenuButtonClick, button_click, button_highlight );

	}

}

void Tutorial::RemoveMenuInterface ()
{

	if ( IsMenuInterfaceVisible () ) {
		
		int pid = SvbLookupPID ( this );

		char part1 [64];
		char part2 [64];
		char part3 [64];
		char part1button [64];
		char part2button [64];
		char part3button [64];

		UplinkSnprintf ( part1, sizeof ( part1 ), "tutorial_part1 1 %d", pid );
		UplinkSnprintf ( part2, sizeof ( part2 ), "tutorial_part2 2 %d", pid );
		UplinkSnprintf ( part3, sizeof ( part3 ), "tutorial_part3 3 %d", pid );
		UplinkSnprintf ( part1button, sizeof ( part1button ), "tutorial_part1button 1 %d", pid );
		UplinkSnprintf ( part2button, sizeof ( part2button ), "tutorial_part2button 2 %d", pid );
		UplinkSnprintf ( part3button, sizeof ( part3button ), "tutorial_part3button 3 %d", pid );

		EclRemoveButton ( part1 );
		EclRemoveButton ( part2 );
		EclRemoveButton ( part3 );
		EclRemoveButton ( part1button );
		EclRemoveButton ( part2button );
		EclRemoveButton ( part3button );

	}

}

void Tutorial::ShowMenuInterface ()
{

	if ( IsMenuInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char part1 [64];
		char part2 [64];
		char part3 [64];
		char part1button [64];
		char part2button [64];
		char part3button [64];

		UplinkSnprintf ( part1, sizeof ( part1 ), "tutorial_part1 1 %d", pid );
		UplinkSnprintf ( part2, sizeof ( part2 ), "tutorial_part2 2 %d", pid );
		UplinkSnprintf ( part3, sizeof ( part3 ), "tutorial_part3 3 %d", pid );
		UplinkSnprintf ( part1button, sizeof ( part1button ), "tutorial_part1button 1 %d", pid );
		UplinkSnprintf ( part2button, sizeof ( part2button ), "tutorial_part2button 2 %d", pid );
		UplinkSnprintf ( part3button, sizeof ( part3button ), "tutorial_part3button 3 %d", pid );

		EclButtonBringToFront ( part1 );
		EclButtonBringToFront ( part2 );
		EclButtonBringToFront ( part3 );
		EclButtonBringToFront ( part1button );
		EclButtonBringToFront ( part2button );
		EclButtonBringToFront ( part3button );
		
	}

}

bool Tutorial::IsMenuInterfaceVisible ()
{

	int pid = SvbLookupPID ( this );
	char part1 [64];
	UplinkSnprintf ( part1, sizeof ( part1 ), "tutorial_part1 1 %d", pid );
	return ( EclGetButton ( part1 ) != NULL );

}

void Tutorial::RunMenu () 
{

	//
	// Change the state of the existing system
	//

	menu = true;
	nextvisible = false;
	current_tutorial = 0;
	current_part = 0;

	int pid = SvbLookupPID ( this );
	char text [64];
	UplinkSnprintf ( text, sizeof ( text ), "tutorial_text %d", pid );

	EclRegisterCaptionChange ( text, " " );

	EclSuperUnHighlightAll ();

	//
	// Create the menu
	//

	CreateMenuInterface ();

}

void Tutorial::RunTutorial ( int tutorial, int part )
{

	current_tutorial = tutorial;
	current_part = part;

	if      ( tutorial == 1 ) RunTutorial1 ( part );
    else if ( tutorial == 2 ) RunTutorial2 ( part );
	else if ( tutorial == 3 ) RunTutorial3 ( part );
    else {

        int pid = SvbLookupPID ( this );
        char text [64];
        UplinkSnprintf ( text, sizeof ( text ), "tutorial_text %d", pid );

        EclRegisterCaptionChange ( text, "[To be completed]" );
		printf ( "Tutorial::RunTutorial, unrecognised tutorial number %d\n", tutorial );

	}

}

void Tutorial::RunTutorial1 ( int part )
{

	int pid = SvbLookupPID ( this );
	char text [64];
	UplinkSnprintf ( text, sizeof ( text ), "tutorial_text %d", pid );
	int time_ms = 3000;


	if ( part == 1 ) {						// Tut 1 Part 1

		EclRegisterCaptionChange ( text, "Welcome to the Uplink Tutorial.\n"
										 "This program should help you to get started.\n"
										 "Click next to continue.", time_ms );

		EclSuperHighlight ( text );
		nextvisible = true;
		nextclicked = false;

	}
	else if ( part == 2 ) {					// Tut 1 Part 2
		
		EclRegisterCaptionChange ( text, "First things first - you can move this tutorial\n"
										 "on the screen by left-clicking on the title bar.\n"
										 "Right click to put it down again.", time_ms );	

		EclSuperUnHighlight ( text );						
		nextvisible = true;
		nextclicked = false;

	}
	else if ( part == 3 ) {					// Tut 1 Part 3

		EclRegisterCaptionChange ( text, "We're going to start by looking at some of the\n"
										 "facilities available on your screen.\n"
										 "Click on the flashing HARDWARE button.\n", time_ms );

		EclSuperHighlight ( "hud_hardware" );
		nextvisible = false;
		
	}
	else if ( part == 4 ) {					// Tut 1 Part 4

		EclRegisterCaptionChange ( text, "This screen shows the status of your GATEWAY.\n"
										 "Your gateway is the link between your home PC\n"
										 "and the rest of the Net.", time_ms );
    
		EclSuperUnHighlight ( "hud_hardware" );
		nextvisible = true;
		nextclicked = false;

	}
	else if ( part == 5 ) {					// Tut 1 Part 5

		EclRegisterCaptionChange ( text, "In reality, the gateway is a computer sitting in\n"
										 "one of our offices.  You'll dial into this Gateway\n"
										 "automatically every time you start Uplink.", time_ms );

		nextclicked = false;

	}
	else if ( part == 6 ) {					// Tut 1 Part 6

		EclRegisterCaptionChange ( text, "At the moment your Gateway is very basic.\n"
										 "It has a slow processor and very little memory.\n"
										 "You'll be able to upgrade later.", time_ms );

		nextclicked = false;

	}
	else if ( part == 7 ) {					// Tut 1 Part 7

		EclRegisterCaptionChange ( text, "Click on the flashing MEMORY BANKS button.", time_ms/3 );
		
		EclSuperUnHighlight ( text );
		EclSuperHighlight ( "hud_memory" );
		nextvisible = false;
		
	}
	else if ( part == 8 ) {					// Tut 1 Part 8

		EclRegisterCaptionChange ( text, "This screen shows all of the files you have stored\n"
										 "in your memory banks.  At the moment you only have\n"
										 "this tutorial and a couple of file utilites.", time_ms );

		EclSuperUnHighlight ( "hud_memory" );
		nextvisible = true;
		nextclicked = false;

	}		
	else if ( part == 9 ) {					// Tut 1 Part 9

		EclRegisterCaptionChange ( text, "Click on the flashing PERSONAL STATUS button.", time_ms/3 );

		EclSuperHighlight ( "hud_status" );
		nextvisible = false;

	}
	else if ( part == 10 ) {				// Tut 1 Part 10

		EclRegisterCaptionChange ( text, "This screen shows your official ratings.\n"
										 "These reflect your skill and success, and will\n"
										 "no doubt increase with time.", time_ms );

		EclSuperUnHighlight ( "hud_status" );
		nextvisible = true;
		nextclicked = false;

	}
	else if ( part == 11 ) {				// Tut 1 Part 11

		EclRegisterCaptionChange ( text, "As your rating increases, you'll find more\n"
										 "interesting and profitable avenues of work\n"
										 "become available to you.", time_ms );

		nextclicked = false;

	}
	else if ( part == 12 ) {				// Tut 1 Part 12

		EclRegisterCaptionChange ( text, "Click on the flashing FINANCE button.", time_ms/3 );

		EclSuperHighlight ( "hud_finance" );
		nextvisible = false;

	}
	else if ( part == 13 ) {				// Tut 1 Part 13

		EclRegisterCaptionChange ( text, "Here you can see how much money you have\n"
										 "in your bank accounts.\n", time_ms*2/3 );

		EclSuperUnHighlight ( "hud_finance" );
	
		nextvisible = true;
		nextclicked = false;

	}
	else if ( part == 14 ) {				// Tut 1 Part 14

		EclRegisterCaptionChange ( text, "The final button allows you to compose a\n"
										 "new email and send it.  We don't need to\n"
										 "do this at the moment.", time_ms );

		EclSuperHighlight ( "hud_sendmail" );

		nextclicked = false;

	}			
	else if ( part == 15 ) {				// Tut 1 Part 15

		EclRegisterCaptionChange ( text, "The big button to the left allows you to\n"
										 "run the software tools on your Gateway.\n"
										 "You can start any program from here.", time_ms );

		EclSuperUnHighlight ( "hud_sendmail" );
		EclSuperHighlight ( "hud_software" );

		nextclicked = false;

	}
	else if ( part == 16 ) {				// Tut 1 Part 16

		EclRegisterCaptionChange ( text, "These two boxes show the date and time,\n"
										 "and your current location on the Net.", time_ms*2/3 );

		EclSuperUnHighlight ( "hud_software" );
		EclSuperHighlight ( "hud_date" );
		EclSuperHighlight ( "hud_location" );

		nextclicked = false;

	}
	else if ( part == 17 ) {				// Tut 1 Part 17

		EclRegisterCaptionChange ( text, "These buttons can be used to accelerate time.\n"
										 "This can be useful when you're waiting\n"
										 "for something to happen.", time_ms );

		EclSuperUnHighlight ( "hud_date" );
		EclSuperUnHighlight ( "hud_location" );
	
        EclSuperHighlight ( "hud_speed 0" );
		EclSuperHighlight ( "hud_speed 1" );
		EclSuperHighlight ( "hud_speed 2" );
		EclSuperHighlight ( "hud_speed 3" );

		nextclicked = false;

	}
	else if ( part == 18 ) {				// Tut 1 Part 18

		EclRegisterCaptionChange ( text, "This column shows all currently running programs,\n"
										 "and how much CPU time is used by each.\n"
										 "You can see this tutorial program in the list.", time_ms );

		EclSuperUnHighlight ( "hud_speed 0" );
        EclSuperUnHighlight ( "hud_speed 1" );
		EclSuperUnHighlight ( "hud_speed 2" );
		EclSuperUnHighlight ( "hud_speed 3" );

		EclSuperHighlight ( "svb_priority_mask 0" );

		nextclicked = false;

	}
	else if ( part == 19 ) {				// Tut 1 Part 19

		EclRegisterCaptionChange ( text, "Bring up the Global Communications screen\n"
										 "by clicking on the world map.", time_ms*2/3 );

		EclSuperUnHighlight ( "svb_priority_mask 0" );
		EclSuperHighlight ( "worldmap_smallmap" );

		nextvisible = false;

	}
	else if ( part == 20 ) {				// Tut 1 Part 20

		EclRegisterCaptionChange ( text, "This screen allows you to set up long distance\n"
										 "connections that bounce around the world.\n"
										 "You'll make extensive use of this screen.", time_ms );

		EclSuperUnHighlight ( "worldmap_smallmap" );
		
		nextvisible = true;
		nextclicked = false;

	}
	else if ( part == 21 ) {				// Tut 1 Part 21

		EclRegisterCaptionChange ( text, "We'll come back here later.\n"
										 "Click the close button to return\n"
										 "to the main screen.", time_ms );

		EclSuperHighlight ( "worldmap_close" );

		nextvisible = false;

	}
	else if ( part == 22 ) {				// Tut 1 Part 22

		EclSuperUnHighlight ( "worldmap_close" );

		if ( game->GetWorld ()->GetPlayer ()->messages.Size () > 0 && strcmp ( game->GetWorld ()->GetPlayer ()->messages.GetData ( 0 )->from, "Uplink public access system" ) == 0 ) {

			EclRegisterCaptionChange ( text, "New emails will queue up in the bottom right\n"
											 "of the screen.  Click on the email to read it.\n", time_ms*2/3 );

			EclSuperHighlight ( "hud_message 0" );

			nextclicked = false;

		}
		else {

			EclRegisterCaptionChange ( text, "New emails will queue up in the bottom right\n"
											 "of the screen.\n", time_ms*2/3 );

			nextvisible = true;
			nextclicked = false;

		}

	}
	else if ( part == 23 ) {				// Tut 1 Part 23

		EclSuperUnHighlight ( "hud_message 0" );

		if ( game->GetWorld ()->GetPlayer ()->messages.Size () == 0 ) {

			current_part = 24;
			RunTutorial1 ( 24 );
			return;

		}

		EclRegisterCaptionChange ( text, "This is an email from Uplink Corporation.\n"
										 "Its basically a welcome message.  You should take\n"
										 "the time to read it at some point.", time_ms );

		nextvisible = true;
		nextclicked = false;

	}
	else if ( part == 24 ) {				// Tut 1 Part 24

		EclRegisterCaptionChange ( text, "As well as mail, you'll also get missions\n"
										 "queuing up at the bottom right of the screen.\n"
										 "They'll remain here until you complete them.", time_ms );

		nextvisible = true;
		nextclicked = false;

	}
	else if ( part == 25 ) {				// Tut 1 Part 25

		if ( game->GetWorld ()->GetPlayer ()->missions.Size () == 0 || strcmp ( game->GetWorld ()->GetPlayer ()->missions.GetData ( 0 )->employer, "Uplink" ) != 0 ) {

			current_part = 28;
			RunTutorial1 ( 28 );
			return;

		}

		EclRegisterCaptionChange ( text, "The only mission you have at the moment is a\n"
										 "test mission from Uplink.  Click on it to read\n"
										 "the details.", time_ms );

		EclSuperHighlight ( "hud_mission 0" );
		nextvisible = false;

	}
	else if ( part == 26 ) {				// Tut 1 Part 26

		EclRegisterCaptionChange ( text, "This screen shows you all the information you need\n"
										 "on how to complete the mission.  You should always\n"
										 "study this in depth before beginning work.", time_ms );

		EclSuperUnHighlight ( "hud_mission 0" );
		nextvisible = true;
		nextclicked = false;

	}
	else if ( part == 27 ) {				// Tut 1 Part 27

		EclRegisterCaptionChange ( text, "You'll learn how to complete missions like this in\n"
										 "part 2 of the tutorial.  For now, click on the\n"
										 "close button.", time_ms );

		nextvisible = false;
		EclSuperHighlight ( "mission_close" );

	}
	else if ( part == 28 ) {				// Tut 1 Part 28

		EclRegisterCaptionChange ( text, "That is the end of Tutorial One.  You should have\n"
										 "an overview by now of the basic functions\n"
										 "on screen. Click next to continue.", time_ms );

		EclSuperUnHighlight ( "mission_close" );
		EclSuperHighlight ( text );
		nextvisible = true;
		nextclicked = false;

	}
	else if ( part == 29 ) {

		EclSuperUnHighlight ( text );
		RunMenu ();

	}

}

void Tutorial::RunTutorial2 ( int part )
{

	int pid = SvbLookupPID ( this );
	char text [64];
	UplinkSnprintf ( text, sizeof ( text ), "tutorial_text %d", pid );
	int time_ms = 3000;

    
    if ( part == 1 ) {                      

        EclRegisterCaptionChange ( text, "Welcome to Part 2 of the tutorial.\n"
                                         "We will now show you how to complete your\n"
                                         "first hack for Uplink.", time_ms );

        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 2 ) {

		// TODO : Check the player has this mission from uplink

        EclRegisterCaptionChange ( text, "Your first hack will be the Uplink test mission.\n"
                                         "Click on the mission description to read the\n"
                                         "details.", time_ms );

        nextvisible = false;
        nextclicked = false;
		EclSuperHighlight ( "hud_mission 0" );

    }
    else if ( part == 3 ) {

        EclRegisterCaptionChange ( text, "Your should read the description carefully, but\n"
                                         "the basic aim is to break into the Uplink Test\n"
                                         "Machine and steal a file.", time_ms );

        EclSuperUnHighlight ( "hud_mission 0" );
        nextvisible = true;
        nextclicked = false;

        // - Caution -
        // Can jump back to this part from part 6

    }
    else if ( part == 4 ) {

        // Make sure the player isn't connected anywhere

        if ( game->GetWorld ()->GetPlayer ()->IsConnected () ) {

            EclRegisterCaptionChange ( text, "Disconnect from your current site.", time_ms / 3 );

            EclSuperHighlight ( "worldmap_connect" );
            nextvisible = false;
            nextclicked = false;

        }
        else {

            // Completion condition is "disconnected"
            // So next part will play immediately

        }

    }
    else if ( part == 5 ) {

        EclRegisterCaptionChange ( text, "We're going to start by connecting to the target.\n"
                                         "Select 'Uplink Test Machine' from the list\n"
                                         "of bookmarked links on screen.", time_ms );

        EclSuperUnHighlight ( "worldmap_connect" );
        nextvisible = false;
        nextclicked = false;

    }
    else if ( part == 6 ) {

        // We are connected somewhere, but are we at the right spot?

        if ( strcmp ( game->GetWorld ()->GetPlayer ()->GetRemoteHost ()->ip, IP_UPLINKTESTMACHINE ) == 0 ) {

            EclRegisterCaptionChange ( text, "We're now dialled in to the Test Machine.\n"
                                             "You should read the opening text message\n"
                                             "before clicking next.", time_ms );

            nextvisible = true;
            nextclicked = false;

        }
        else {

            EclRegisterCaptionChange ( text, "You've connected to the wrong site.\n"
                                             "The one we want is the 'Uplink Test Machine'.\n"
                                             "Click next to try again.", time_ms );

            nextvisible = true;
            nextclicked = false;
            current_part = 3;                   // Jump back to just before part 4

        }

    }
    else if ( part == 7 ) {

        EclRegisterCaptionChange ( text, "Click on the 'Start the test' button\n"
                                         "to bypass this screen.\n", time_ms*2/3 );

        nextvisible = false;
        nextclicked = false;

        EclSuperHighlight ( "messagescreen_click 1" );

    }
    else if ( part == 8 ) {

        // UserID screen

        EclRegisterCaptionChange ( text, "This is a User ID screen.  You will need\n"
                                         "a username and password to get past,\n"
                                         "which we don't have.", time_ms );

        EclSuperUnHighlight ( "messagescreen_click 1" );
        
        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 9 ) {

        EclRegisterCaptionChange ( text, "In reality, you will almost never have a\n"
                                         "genuine username and password.\n"
                                         "You'll have to hack your way in.", time_ms );

        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 10 ) {

        EclRegisterCaptionChange ( text, "In order to hack our way in, we'll need to\n"
                                         "buy some software.\n"
                                         "Disconnect from this machine.", time_ms );

        nextvisible = false;
        nextclicked = false;

        EclSuperHighlight ( "worldmap_connect" );

    }
    else if ( part == 11 ) {

        EclRegisterCaptionChange ( text, "The best place to buy software for hacking\n"
                                         "Is the 'Uplink Internal Services System'.\n"
                                         "Connect to that site.", time_ms );

        EclSuperUnHighlight ( "worldmap_connect" );

    }
    else if ( part == 12 ) {

        // Uplink Internal Services Machine

        EclRegisterCaptionChange ( text, "Unlike most sites, you do have a valid\n"
                                         "Username and password because you are a\n"
                                         "registered Uplink Agent.", time_ms );

        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 13 ) {

        EclRegisterCaptionChange ( text, "You can either type your username/password\n"
                                         "or just click on them to enter automatically.\n"
                                         "Do this now then click on proceed.", time_ms );

        nextvisible = false;
        nextclicked = false;

    }
    else if ( part == 14 ) {

        EclRegisterCaptionChange ( text, "We're in.  Your authorisation has been accepted\n"
                                         "and your actions are being monitored by Uplink's\n"
                                         "central computer system.", time_ms );

        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 15 ) {

        EclRegisterCaptionChange ( text, "This computer system provides a number of\n"
                                         "valuable services to the hacker community,\n"
                                         "including software tools which we need.", time_ms );

        nextclicked = false;

    }
    else if ( part == 16 ) {

        EclRegisterCaptionChange ( text, "You can access Uplink-related news stories,\n"
                                         "accept work from contractors, and upgrade your\n"
                                         "computer system.", time_ms );

        nextclicked = false;

    }
    else if ( part == 17 ) {

        EclRegisterCaptionChange ( text, "We want to upgrade our software so we can\n"
                                         "hack UserID screens.  Click on 'Software\n"
                                         "Upgrades' from the menu.", time_ms );

        nextvisible = false;
        nextclicked = false;

    }
    else if ( part == 18 ) {

        EclRegisterCaptionChange ( text, "This screen lists all of the software tools\n"
                                         "that Uplink currently has for sale.  Items\n"
                                         "in grey are out of your price range.", time_ms );

        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 19 ) {

        EclRegisterCaptionChange ( text, "The first tool you will need is one of the\n"
                                         "basic Uplink Agent tools - \n"
                                         "the Password Breaker.", time_ms );

        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 20 ) {

        EclRegisterCaptionChange ( text, "Scroll down the list and select the Password\n"
                                         "Breaker.  The current price for this tool is\n"
                                         "1500 credits.  Click 'purchase'.", time_ms );

        nextvisible = false;
        nextclicked = false;

    }
    else if ( part == 21 ) {

        EclRegisterCaptionChange ( text, "That software tool has now been uploaded into\n"
                                         "your memory banks, and 1500c has been deducted.\n"
                                         "We can now try breaking that password.", time_ms );

        nextvisible = true;
        nextclicked = false;
		
    }
    else if ( part == 22 ) {

        EclRegisterCaptionChange ( text, "Disconnect from here and connect to the Uplink\n"
                                         "Test machine.  Bypass the opening text screen.", time_ms );

        nextvisible = false;
        nextclicked = false;

    }
    else if ( part == 23 ) {

        EclRegisterCaptionChange ( text, "We want to run our Password Breaker software\n"
                                         "on this screen.  Our new software will appear\n"
                                         "in our Software menu.", time_ms );

        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 24 ) {

        EclRegisterCaptionChange ( text, "Click on the software Run button.", time_ms/3 );

   		EclSuperHighlight ( "hud_software" );

        nextvisible = false;
        nextclicked = false;

    }
    else if ( part == 25 ) {

        EclRegisterCaptionChange ( text, "Start your new Password Breaker by selecting it\n"
                                         "from the menu.  It will be under the 'Crackers'\n"
                                         "sub-menu.", time_ms );
        
	    EclSuperUnHighlight ( "hud_software" );

    }
    else if ( part == 26 ) {

        EclRegisterCaptionChange ( text, "The Password Breaker is now running.\n"
                                         "We now need to give it a target.", time_ms );

        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 27 ) {

        EclRegisterCaptionChange ( text, "Click on the one of the text boxes\n"
                                         "labelled NAME or CODE.  This will start\n"
                                         "the hack attempt.", time_ms );

        nextvisible = false;

    }
    else if ( part == 28 ) {
        
        EclRegisterCaptionChange ( text, "Oh dear. You've been disconnected by the\n"
                                         "computer you were trying to hack in to.\n", time_ms );
        
        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 29 ) {

        EclRegisterCaptionChange ( text, "They noticed the fact that you were trying\n"
                                         "to hack their password screen, and kicked you\n"
                                         "off their system.", time_ms );

        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 30 ) {

        EclRegisterCaptionChange ( text, "Because this is a TEST machine, you won't be\n"
                                         "punished in any way.  In the real world, you\n"
                                         "could now face a fine or even jail time.", time_ms );

        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 31 ) {

        EclRegisterCaptionChange ( text, "This tutorial will now show you how to hack\n"
                                         "into a computer without being caught in\n"
                                         "any way.", time_ms );

        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 32 ) {

        EclRegisterCaptionChange ( text, "The first thing we need is a warning system,\n"
                                         "so we can tell if they are about to catch us.\n"
                                         "We can buy software that will do this.", time_ms );

        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 33 ) {

        EclRegisterCaptionChange ( text, "Connect to the Uplink Internal Services System\n"
                                         "and go to the Software Sales menu, as before.", time_ms );

        nextvisible = false;
        
    }
    else if ( part == 34 ) {

        EclRegisterCaptionChange ( text, "Scroll down the list and click on\n"
                                         "'Trace Tracker'.  This item of software can tell\n"
                                         "you how close you are to being traced.", time_ms );

        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 35 ) {

        EclRegisterCaptionChange ( text, "There are several Versions of this item.\n"
                                         "Higher versions have more features but\n"
                                         "are more expensive.", time_ms );

        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 36 ) {

        EclRegisterCaptionChange ( text, "You can afford a Trace Tracker V2.0.  Use the\n"
                                         "'Next Version' button to select Version 2.0, then\n"
                                         "click on 'purchase'.", time_ms );

        nextvisible = false;
        
    }
    else if ( part == 37 ) {

        EclRegisterCaptionChange ( text, "You've now bought that item of software.\n"
                                         "You should run it straight away from the\n"
                                         "software menu, under 'Security'.", time_ms );

  		EclSuperHighlight ( "hud_software" );

        nextvisible = false;
        nextclicked = false;

    }
    else if ( part == 38 ) {

        EclRegisterCaptionChange ( text, "The Trace Tracker is now running.  It is\n"
                                         "currently telling you that nobody is trying\n"
                                         "to trace your location at this time.", time_ms );

        nextvisible = true;
        nextclicked = false;

        EclSuperUnHighlight ( "hud_software" );

    }
    else if ( part == 39 ) {

        EclRegisterCaptionChange ( text, "Having a warning system is a step in the right\n"
                                         "direction. We also need to slow down any traces\n"
                                         "that are being performed on us.", time_ms );

        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 40 ) {

        EclRegisterCaptionChange ( text, "We do this by 'Call Bouncing'. This involves\n"
                                         "extending our connection aroung the world, to\n"
                                         "confuse the opponent.\n", time_ms );

        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 41 ) {

        EclRegisterCaptionChange ( text, "First, disconnect from this system.", time_ms / 3 );

        nextvisible = false;
        
        EclSuperHighlight ( "worldmap_connect" );
        
    }
    else if ( part == 42 ) {

        EclRegisterCaptionChange ( text, "Bring up the Global Communications map.", time_ms / 3 );

        nextvisible = false;

        EclSuperUnHighlight ( "worldmap_connect" );
        EclSuperHighlight ( "worldmap_smallmap" );
                                         
    }
    else if ( part == 43 ) {

        EclRegisterCaptionChange ( text, "We need to establish a long distance connection\n"
                                         "to our target (the Uplink Test Machine), bounced\n"
                                         "through several intermediate systems.", time_ms );

        EclSuperUnHighlight ( "worldmap_smallmap" );

        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 44 ) {

        EclRegisterCaptionChange ( text, "We build this connection by clicking on each\n"
                                         "of the intermediate nodes in sequence.\n"
                                         "Click on the location 'InterNIC'.", time_ms );

        nextvisible = false;
        
    }
    else if ( part == 45 ) {

        EclRegisterCaptionChange ( text, "Click on the location\n"
                                         "'Uplink Public Access Server'.\n"
                                         "Click 'Cancel' if you make a mistake.", time_ms );

        nextvisible = false;

    }
    else if ( part == 46 ) {

        if ( game->GetWorld ()->GetPlayer ()->GetConnection ()->GetSize () == 1 ) {
            current_part = 43;
            RunTutorial2 ( 43 );
            return;
        }
            
        EclRegisterCaptionChange ( text, "Click on the location\n"
                                         "'Uplink Internal Services System'.\n"
                                         "Click 'Cancel' if you make a mistake.", time_ms );

        nextvisible = false;

    }
    else if ( part == 47 ) {

        if ( game->GetWorld ()->GetPlayer ()->GetConnection ()->GetSize () == 1 ) {
            current_part = 43;
            RunTutorial2 ( 43 );
            return;
        }

        EclRegisterCaptionChange ( text, "Finally, click on the target\n"
                                         "'Uplink Test Machine'.\n"
                                         "Click 'Cancel' if you make a mistake.", time_ms );

        nextvisible = false;

    }
    else if ( part == 48 ) {

        if ( game->GetWorld ()->GetPlayer ()->GetConnection ()->GetSize () == 1 ) {
            current_part = 43;
            RunTutorial2 ( 43 );
            return;
        }

        EclRegisterCaptionChange ( text, "We've now set up a long distance connection\n"
                                         "to the target, bounced through three\n"
                                         "intermediate nodes.", time_ms );

        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 49 ) {

        EclRegisterCaptionChange ( text, "It will take the opponent a lot longer to\n"
                                         "trace and disconnect us this time.  Click on\n"
                                         "'Connect' to establish the link.", time_ms );

        nextvisible = false;
        
    }
    else if ( part == 50 ) {

        EclRegisterCaptionChange ( text, "Bypass the opening screen.", time_ms / 3 );
          
    }
    else if ( part == 51 ) {

        EclRegisterCaptionChange ( text, "Start the password breaker now.", time_ms / 3 );
        
        nextvisible = false;

    }
    else if ( part == 52 ) {

        EclRegisterCaptionChange ( text, "Target the password box. As soon as you do this,\n"
                                         "they will begin tracing you. Click on Proceed\n"
                                         "when it has finished.", time_ms );

    }
    else if ( part == 53 ) {

        EclRegisterCaptionChange ( text, "The password has been broken and we're in.\n"
                                         "Note the trace is now in progress. This is only\n"
                                         "a test machine so it will be a slow trace.", time_ms );

        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 54 ) {

        EclRegisterCaptionChange ( text, "If you recall the mission description, we have to\n"
                                         "steal a file from this system. Click on\n"
                                         "'Access File Server'.", time_ms );

        nextvisible = false;
        
    }
    else if ( part == 55 ) {

        EclRegisterCaptionChange ( text, "The file we have to steal is named\n"
                                         "'Uplink Test Data'. We'll need to use a\n"
                                         "File Copier to download the file.", time_ms );

        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 56 ) {

        EclRegisterCaptionChange ( text, "Start the File Copier utility from your\n"
                                         "software menu, under 'File Utilities'", time_ms / 2 );

        nextvisible = false;

    }
    else if ( part == 57 ) {

        EclRegisterCaptionChange ( text, "Target the file in question by clicking on it.\n"
                                         "(Uplink Test Data)", time_ms / 2 );

    }
    else if ( part == 58 ) {

        EclRegisterCaptionChange ( text, "The file is now being copied.\n"
                                         "You can speed this up by clicking on this button\n"
                                         "at the top to divert more CPU time to this task.", time_ms );

        EclSuperHighlight ( "svb_increase 2" );

    }
    else if ( part == 59 ) {

        EclRegisterCaptionChange ( text, "We've finished copying the file. We now want\n"
                                         "to dump it into our memory banks. Bring up the\n"
                                         "Memory banks screen.", time_ms );

        EclSuperUnHighlight ( "svb_increase 2" );

        EclSuperHighlight ( "hud_memory" );

    }
    else if ( part == 60 ) {
    
        EclRegisterCaptionChange ( text, "Pick up the File Copier by clicking on it", time_ms / 3 );
                
        EclSuperUnHighlight ( "hud_memory" );

    }
    else if ( part == 61 ) {

        EclRegisterCaptionChange ( text, "Now click on a clear area of memory\n"
                                         "to dump the file in to.", time_ms / 2 );

/*
        EclSuperHighlight ( "memory_block 6" );
        EclSuperHighlight ( "memory_block 7" );
        EclSuperHighlight ( "memory_block 8" );
        EclSuperHighlight ( "memory_block 9" );
        EclSuperHighlight ( "memory_block 10" );
        EclSuperHighlight ( "memory_block 11" );
    */

    }
    else if ( part == 62 ) {

/*
        EclSuperUnHighlight ( "memory_block 11" );
        EclSuperUnHighlight ( "memory_block 10" );
        EclSuperUnHighlight ( "memory_block 9" );
        EclSuperUnHighlight ( "memory_block 8" );
        EclSuperUnHighlight ( "memory_block 7" );
        EclSuperUnHighlight ( "memory_block 6" );
    */

        // TODO : Above crashes 


        EclRegisterCaptionChange ( text, "We've now got the file. Disconnect from\n"
                                         "this computer system.", time_ms / 2 );

    }
    else if ( part == 63 ) {

        EclRegisterCaptionChange ( text, "In a real-world hack, we'd now have to\n"
                                         "make some attempt to delete the access logs\n"
                                         "that we've just left behind.", time_ms );

        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 64 ) {

        EclRegisterCaptionChange ( text, "Every time you log on to a system, copy\n"
                                         "a file or make any changes, you leave\n"
                                         "behind access logs.", time_ms );

        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 65 ) {

        EclRegisterCaptionChange ( text, "These logs can be used by other Agents\n"
                                         "to trace you and put you in jail. You may\n"
                                         "even end up tracing another Agent one day.", time_ms );

        nextclicked = false;

    }
    else if ( part == 66 ) {

        EclRegisterCaptionChange ( text, "Back to the mission. Our final task is to\n"
                                         "send the copied file back to the customer.\n"
                                         "Bring up the mission description.", time_ms );

        nextvisible = false;

        EclSuperHighlight ( "hud_mission 0" );

    }
    else if ( part == 67 ) {

        EclRegisterCaptionChange ( text, "Click on 'reply'.", time_ms / 3 );

        EclSuperUnHighlight ( "hud_mission 0" );
        EclSuperHighlight ( "mission_reply" );

    }
    else if ( part == 68 ) {

        EclRegisterCaptionChange ( text, "We need to attach the stolen file to this\n"
                                         "email before we send it. Click on the File\n"
                                         "button and select 'Uplink Test Data'.", time_ms );

        EclSuperUnHighlight ( "mission_reply" );
        EclSuperHighlight ( "sendmail_file" );

    }
    else if ( part == 69 ) {

        EclRegisterCaptionChange ( text, "Now click on send.", time_ms / 3 );
        
        EclSuperUnHighlight ( "sendmail_file" );
        EclSuperHighlight ( "sendmail_send" );

    }
    else if ( part == 70 ) {

        EclRegisterCaptionChange ( text, "The mission has now been completed.\n"
                                         "You'll receive two emails, both from Uplink.", time_ms / 2 );

        nextvisible = true;
        nextclicked = false;                                       

    }
    else if ( part == 71 ) {

        EclRegisterCaptionChange ( text, "The first is congratulating you on completing\n"
                                         "the mission, and informing you that you've been\n"
                                         "paid. (In this case, nothing).", time_ms );

        nextclicked = false;

    }
    else if ( part == 72 ) {

        EclRegisterCaptionChange ( text, "The second is informing you that your rating\n"
                                         "has gone up, and you'll now be able to do some\n"
                                         "more dangerous/profitable work.", time_ms );

        nextclicked = false;

    }
    else if ( part == 73 ) {

        EclRegisterCaptionChange ( text, "You should now go to the Uplink Internal\n"
                                         "Services System and look through the work\n"
                                         "available to you.", time_ms );

        nextclicked = false;
    
    }
    else if ( part == 74 ) {

        EclRegisterCaptionChange ( text, "This is the end of Part2 of the tutorial.\n"
                                         "In part3 you'll learn how to start making\n"
                                         "money as an Uplink agent.", time_ms );

        nextclicked = false;

    }
    else if ( part == 75 ) {

        RunMenu ();

    }

}

void Tutorial::RunTutorial3 ( int part )
{

	int pid = SvbLookupPID ( this );
	char text [64];
	UplinkSnprintf ( text, sizeof ( text ), "tutorial_text %d", pid );
	int time_ms = 3000;

    
    if ( part == 1 ) {                      

        EclRegisterCaptionChange ( text, "Welcome to Part 3 of the Tutorial.\n"
                                         "Hopefully you've finished the Test mission.\n"
                                         "This part will show you what to do next.", time_ms );

        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 2 ) {                      

        EclRegisterCaptionChange ( text, "The Uplink Internal Services system is the place\n"
                                         "where you will find all of your future work.\n"
                                         "Connect there now.", time_ms );

        nextvisible = false;

    }
    else if ( part == 3 ) {

        EclRegisterCaptionChange ( text, "Log in with your username and password.\n", time_ms / 3 );
                                         
    }
    else if ( part == 4 ) {

        EclRegisterCaptionChange ( text, "Uplink maintains a list of available missions.\n"
                                         "You can access this list by selecting\n"
                                         "'Mission List' from the menu.", time_ms );

    }
    else if ( part == 5 ) {

        EclRegisterCaptionChange ( text, "You should see a few missions that are available\n"
                                         "Now. If you don't see any available missions,\n"
                                         "you can accelerate time until some appear.", time_ms );

        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 6 ) {

        EclRegisterCaptionChange ( text, "You can use these buttons at the top of the screen\n"
                                         "to accelerate time until a mission becomes\n"
                                         "available.", time_ms );

		EclSuperHighlight ( "hud_speed 0" );
        EclSuperHighlight ( "hud_speed 1" );
		EclSuperHighlight ( "hud_speed 2" );
		EclSuperHighlight ( "hud_speed 3" );

        nextclicked = false;

    }
    else if ( part == 7 ) {

        EclRegisterCaptionChange ( text, "Each time you successfully complete a mission,\n"
                                         "you'll be paid and your Uplink rating will go up.", time_ms );

		EclSuperUnHighlight ( "hud_speed 0" );
        EclSuperUnHighlight ( "hud_speed 1" );
		EclSuperUnHighlight ( "hud_speed 2" );
		EclSuperUnHighlight ( "hud_speed 3" );

        nextclicked = false;

    }
    else if ( part == 8 ) {

        EclRegisterCaptionChange ( text, "As your rating goes up, more missions will appear.\n"
                                         "Employers are more likely to offer more dangerous\n"
                                         "missions and more pay as your rating gets higher.", time_ms );

        nextclicked = false;

    }
    else if ( part == 9 ) {

        EclRegisterCaptionChange ( text, "In addition, your Neuromancer rating is affected\n"
                                         "by the types of missions you do. This rating\n"
                                         "is effectivly a record of your moral values.", time_ms );

        nextclicked = false;

    }
    else if ( part == 10 ) {

        EclRegisterCaptionChange ( text, "You may find employees are unwilling to give you\n"
                                         "certain missions if your Neuromancer rating\n"
                                         "is too low.", time_ms );

        nextclicked = false;

    }
    else if ( part == 11 ) {

        EclRegisterCaptionChange ( text, "Exit out from the Mission screen.", time_ms / 3 );

        nextvisible = false;

    }
    else if ( part == 12 ) {

        EclRegisterCaptionChange ( text, "Uplink Corporation maintains a large database\n"
                                         "of useful information on the subject of hacking\n"
                                         "and computer security.", time_ms );
        
        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 13 ) {

        EclRegisterCaptionChange ( text, "Select 'Help' from the menu.", time_ms / 3 );
        
        nextvisible = false;

    }
    else if ( part == 14 ) {

        EclRegisterCaptionChange ( text, "As you attempt more complex missions,\n"
                                         "you will undoubtably run into more complex\n"
                                         "security systems.", time_ms );

        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 15 ) {

        EclRegisterCaptionChange ( text, "All of the information we have on these systems\n"
                                         "is kept in these help pages. If you get stuck,\n"
                                         "this is the place to look for data.", time_ms );

        nextclicked = false;

    }
    else if ( part == 16 ) {

        EclRegisterCaptionChange ( text, "You may learn that you need some new software\n"
                                         "or a hardware upgrade.", time_ms );

        nextclicked = false;

    }
    else if ( part == 17 ) {

        EclRegisterCaptionChange ( text, "Exit out from the Help screen.", time_ms / 3 );

        nextvisible = false;

    }
    else if ( part == 18 ) {

        EclRegisterCaptionChange ( text, "Once you are working on the harder missions,\n"
                                         "you will find your current Gateway is too\n"
                                         "limiting.", time_ms );
        
        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 19 ) {

        EclRegisterCaptionChange ( text, "You may need faster CPU capabilities,\n"
                                         "more memory capacity or bandwidth,\n"
                                         "or more personal security.", time_ms );

        nextclicked = false;

    }
    else if ( part == 20 ) {

        EclRegisterCaptionChange ( text, "Select 'Gateway Upgrades' from the menu.", time_ms / 3 );
        
        nextvisible = false;

    }
    else if ( part == 21 ) {

        EclRegisterCaptionChange ( text, "This screen shows all of the Gateways\n"
                                         "that you can buy from Uplink Corporation.\n"
                                         "Currently you have the first one off the list.", time_ms );

        nextvisible = true;
        nextclicked = false;

    }
    else if ( part == 22 ) {

        EclRegisterCaptionChange ( text, "After a few missions you'll be able to afford\n"
                                         "to upgrade. Your old Gateway is part-exchanged,\n"
                                         "so you can work up the list slowly.", time_ms );

        nextclicked = false;

    }
    else if ( part == 23 ) {

        EclRegisterCaptionChange ( text, "Uplink Corporation does not store any records\n"
                                         "on who is using which Gateway. This means that\n"                                        
                                         "your Gateway can be traced, but you are safe.", time_ms );

        nextclicked = false;

    }
    else if ( part == 24 ) {

        EclRegisterCaptionChange ( text, "If you are traced during a hack, we will be\n"
                                         "forced to sieze your accounts and Gateway.", time_ms );

        nextclicked = false;
                                         
    }
    else if ( part == 25 ) {

        EclRegisterCaptionChange ( text, "This means you'll have to start from the bottom\n"
                                         "of the Gateway list again when you rejoin.\n", time_ms );
                                         
        nextclicked = false;

    }
    else if ( part == 26 ) {

        EclRegisterCaptionChange ( text, "Later gateways have room for your own Security.\n"
                                         "You can install cameras and motion sensors which\n"
                                         "will give you early warning of trouble.", time_ms );

        nextclicked = false;

    }
    else if ( part == 27 ) {

        EclRegisterCaptionChange ( text, "If you destroy your gateway before it is siezed,\n"
                                         "you can normally save your accounts and name.\n"
                                         "However you will of course lose your Gateway.", time_ms );

        nextclicked = false;

    }
    else if ( part == 28 ) {

        EclRegisterCaptionChange ( text, "This is the end of Tutorial 3.\n"
                                         "You should have a good idea of what to do next.\n"
                                         "Uplink Corp. wishes you luck for the future.", time_ms );

        nextclicked = false;

    }
    else if ( part == 29 ) {

        RunMenu ();

    }

}

bool Tutorial::HasCompletedCurrentSection ()
{

	// Default is nextclicked

	if ( current_tutorial == 1 ) {

		if      ( current_part == 3 )   				return ( game->GetInterface ()->GetLocalInterface ()->InScreen () == SCREEN_HW );
		else if ( current_part == 7 )					return ( game->GetInterface ()->GetLocalInterface ()->InScreen () == SCREEN_MEMORY );
		else if ( current_part == 9 )					return ( game->GetInterface ()->GetLocalInterface ()->InScreen () == SCREEN_STATUS );
		else if ( current_part == 12 )					return ( game->GetInterface ()->GetLocalInterface ()->InScreen () == SCREEN_FINANCE );
		else if ( current_part == 19 )					return ( game->GetInterface ()->GetLocalInterface ()->GetHUD ()->wmi.IsVisibleWorldMapInterface () == WORLDMAP_LARGE );
		else if ( current_part == 21 )					return ( game->GetInterface ()->GetLocalInterface ()->GetHUD ()->wmi.IsVisibleWorldMapInterface () == WORLDMAP_SMALL );
		else if ( current_part == 22 )					return ( game->GetInterface ()->GetLocalInterface ()->InScreen () == SCREEN_EMAIL || nextclicked );
		else if ( current_part == 25 )					return ( game->GetInterface ()->GetLocalInterface ()->InScreen () == SCREEN_MISSION );
		else if ( current_part == 27 )					return ( game->GetInterface ()->GetLocalInterface ()->InScreen () == SCREEN_NONE );
		else											return nextclicked;
		
	}
    else if ( current_tutorial == 2 ) {

        if      ( current_part == 2 )                   return ( game->GetInterface ()->GetLocalInterface ()->InScreen () == SCREEN_MISSION );
        else if ( current_part == 4 )                   return ( !game->GetWorld ()->GetPlayer ()->IsConnected () );
        else if ( current_part == 5 )                   return ( game->GetWorld ()->GetPlayer ()->IsConnected () );
        else if ( current_part == 7 )                   return ( strcmp ( game->GetWorld ()->GetPlayer ()->GetRemoteHost ()->ip, IP_UPLINKTESTMACHINE ) == 0 &&
                                                                 game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ()->GetOBJECTID () == OID_USERIDSCREEN );
        else if ( current_part == 10 )                  return ( !game->GetWorld ()->GetPlayer ()->IsConnected () );
        else if ( current_part == 11 )                  return ( strcmp ( game->GetWorld ()->GetPlayer ()->GetRemoteHost ()->ip, IP_UPLINKINTERNALSERVICES ) == 0 );
        else if ( current_part == 13 )                  return ( strcmp ( game->GetWorld ()->GetPlayer ()->GetRemoteHost ()->ip, IP_UPLINKINTERNALSERVICES ) == 0 &&
                                                                 game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ()->GetOBJECTID () == OID_MENUSCREEN );
        else if ( current_part == 17 )                  return ( strcmp ( game->GetWorld ()->GetPlayer ()->GetRemoteHost ()->ip, IP_UPLINKINTERNALSERVICES ) == 0 &&
                                                                 game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ()->ScreenID () == SCREEN_SWSALESSCREEN );
        else if ( current_part == 20 )                  return ( game->GetWorld ()->GetPlayer ()->gateway.databank.GetData ( "Password_Breaker" ) != NULL );
        else if ( current_part == 22 )                  return ( strcmp ( game->GetWorld ()->GetPlayer ()->GetRemoteHost ()->ip, IP_UPLINKTESTMACHINE ) == 0 &&
                                                                 game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ()->GetOBJECTID () == OID_USERIDSCREEN );
        else if ( current_part == 24 )                  return ( game->GetInterface ()->GetLocalInterface ()->GetHUD ()->si.IsVisibleSoftwareMenu () );
        else if ( current_part == 25 )                  return ( SvbGetTask ("Password_Breaker") != NULL );
        else if ( current_part == 27 )                  return ( !game->GetWorld ()->GetPlayer ()->IsConnected () );
        else if ( current_part == 33 )                  return ( strcmp ( game->GetWorld ()->GetPlayer ()->GetRemoteHost ()->ip, IP_UPLINKINTERNALSERVICES ) == 0 &&
                                                                 game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ()->ScreenID () == SCREEN_SWSALESSCREEN );
        else if ( current_part == 36 )                  return ( game->GetWorld ()->GetPlayer ()->gateway.databank.ContainsData ( "Trace_Tracker", 2.0f ) );
        else if ( current_part == 37 )                  return ( SvbGetTask ("Trace_Tracker") != NULL );
        else if ( current_part == 41 )                  return ( !game->GetWorld ()->GetPlayer ()->IsConnected () );
        else if ( current_part == 42 )					return ( game->GetInterface ()->GetLocalInterface ()->GetHUD ()->wmi.IsVisibleWorldMapInterface () == WORLDMAP_LARGE );        
        else if ( current_part == 44 )                  return ( game->GetWorld ()->GetPlayer ()->GetConnection ()->LocationIncluded (IP_INTERNIC) );
        else if ( current_part == 45 )                  return ( game->GetWorld ()->GetPlayer ()->GetConnection ()->LocationIncluded (IP_UPLINKPUBLICACCESSSERVER) || 
                                                                 game->GetWorld ()->GetPlayer ()->GetConnection ()->GetSize () == 1 );        
        else if ( current_part == 46 )                  return ( game->GetWorld ()->GetPlayer ()->GetConnection ()->LocationIncluded (IP_UPLINKINTERNALSERVICES) || 
                                                                 game->GetWorld ()->GetPlayer ()->GetConnection ()->GetSize () == 1 );                
        else if ( current_part == 47 )                  return ( game->GetWorld ()->GetPlayer ()->GetConnection ()->LocationIncluded (IP_UPLINKTESTMACHINE) ||
                                                                 game->GetWorld ()->GetPlayer ()->GetConnection ()->GetSize () == 1 );        
        else if ( current_part == 49 )                  return ( strcmp ( game->GetWorld ()->GetPlayer ()->GetRemoteHost ()->ip, IP_UPLINKTESTMACHINE ) == 0 );                                                                 
        else if ( current_part == 50 )                  return ( strcmp ( game->GetWorld ()->GetPlayer ()->GetRemoteHost ()->ip, IP_UPLINKTESTMACHINE ) == 0 &&
                                                                 game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ()->GetOBJECTID () == OID_USERIDSCREEN );
        else if ( current_part == 51 )                  return ( SvbGetTask ("Password_Breaker") != NULL );
        else if ( current_part == 52 )                  return ( strcmp ( game->GetWorld ()->GetPlayer ()->GetRemoteHost ()->ip, IP_UPLINKTESTMACHINE ) == 0 &&
                                                                 game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ()->GetOBJECTID () == OID_MENUSCREEN );
        else if ( current_part == 54 )                  return ( strcmp ( game->GetWorld ()->GetPlayer ()->GetRemoteHost ()->ip, IP_UPLINKTESTMACHINE ) == 0 &&
                                                                 game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ()->ScreenID () == SCREEN_FILESERVERSCREEN );
        else if ( current_part == 56 )                  return ( SvbGetTask ("File_Copier") != NULL );
        else if ( current_part == 57 )                  return ( SvbGetTask ("File_Copier") != NULL &&
                                                                 ((FileCopier *) SvbGetTask ("File_Copier"))->GetState () == FILECOPIER_INPROGRESS );
        else if ( current_part == 58 )                  return ( SvbGetTask ("File_Copier") != NULL &&
                                                                 ((FileCopier *) SvbGetTask ("File_Copier"))->GetState () == FILECOPIER_WAITINGFORTARGET );
		else if ( current_part == 59 )					return ( game->GetInterface ()->GetLocalInterface ()->InScreen () == SCREEN_MEMORY );
        else if ( current_part == 60 )                  return ( game->GetInterface ()->GetTaskManager ()->GetTargetProgram () &&
                                                                 strcmp ( game->GetInterface ()->GetTaskManager ()->GetTargetProgramName (), "File_Copier" ) == 0 );
        else if ( current_part == 61 )                  return ( game->GetWorld ()->GetPlayer ()->gateway.databank.GetData ( "Uplink test data" ) != NULL );
        else if ( current_part == 62 )                  return ( !game->GetWorld ()->GetPlayer ()->IsConnected () );
        else if ( current_part == 66 )                  return ( game->GetInterface ()->GetLocalInterface ()->InScreen () == SCREEN_MISSION );        
        else if ( current_part == 67 )                  return ( game->GetInterface ()->GetLocalInterface ()->InScreen () == SCREEN_SENDMAIL );        
        else if ( current_part == 68 )                  return ( game->GetInterface ()->GetLocalInterface ()->InScreen () == SCREEN_SENDMAIL &&
                                                                 ((SendMailInterface *) game->GetInterface ()->GetLocalInterface ()->GetInterfaceScreen () )->sGetMessage () &&
                                                                 ((SendMailInterface *) game->GetInterface ()->GetLocalInterface ()->GetInterfaceScreen () )->sGetMessage ()->GetData () &&
                                                                 strcmp ( ((SendMailInterface *) game->GetInterface ()->GetLocalInterface ()->GetInterfaceScreen () )->sGetMessage ()->GetData ()->title, "Uplink test data" ) == 0 );
        else if ( current_part == 69 )                  return ( game->GetInterface ()->GetLocalInterface ()->InScreen () == SCREEN_NONE );
        else                                            return nextclicked;


    }
    else if ( current_tutorial == 3 ) {

        if      ( current_part == 2 )                   return ( strcmp ( game->GetWorld ()->GetPlayer ()->GetRemoteHost ()->ip, IP_UPLINKINTERNALSERVICES ) == 0 );
        else if ( current_part == 3 )                   return ( strcmp ( game->GetWorld ()->GetPlayer ()->GetRemoteHost ()->ip, IP_UPLINKINTERNALSERVICES ) == 0 &&
                                                                 game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ()->GetOBJECTID () == OID_MENUSCREEN );
        else if ( current_part == 4 )                   return ( strcmp ( game->GetWorld ()->GetPlayer ()->GetRemoteHost ()->ip, IP_UPLINKINTERNALSERVICES ) == 0 &&
                                                                 game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ()->ScreenID () == SCREEN_BBSSCREEN );

        else if ( current_part == 11 )                  return ( strcmp ( game->GetWorld ()->GetPlayer ()->GetRemoteHost ()->ip, IP_UPLINKINTERNALSERVICES ) == 0 &&
                                                                 game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ()->GetOBJECTID () == OID_MENUSCREEN );
        else if ( current_part == 13 )                  return ( strcmp ( game->GetWorld ()->GetPlayer ()->GetRemoteHost ()->ip, IP_UPLINKINTERNALSERVICES ) == 0 &&
                                                                 strcmp ( game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ()->subtitle, "Help/" ) == 0 );
        else if ( current_part == 17 )                  return ( strcmp ( game->GetWorld ()->GetPlayer ()->GetRemoteHost ()->ip, IP_UPLINKINTERNALSERVICES ) == 0 &&
                                                                 strcmp ( game->GetInterface ()->GetRemoteInterface ()->GetComputerScreen ()->subtitle, "Internal Services Main Menu" ) == 0 );
        else if ( current_part == 20 )                  return ( strcmp ( game->GetWorld ()->GetPlayer ()->GetRemoteHost ()->ip, IP_UPLINKINTERNALSERVICES ) == 0 &&
                                                                 game->GetInterface ()->GetRemoteInterface ()->GetInterfaceScreen ()->ScreenID () == SCREEN_CHANGEGATEWAY );
        else                                            return nextclicked;

    }

	return false;

}

