
#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h> /*_glu_extention_library_*/

#include <string.h>
#include <stdio.h>

#include "soundgarden.h"
#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"
#include "app/serialise.h"

#include "options/options.h"

#include "game/game.h"
#include "game/scriptlibrary.h"
#include "game/data/data.h"

#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/remoteinterface/remoteinterfacescreen.h"
#include "interface/remoteinterface/passwordscreen_interface.h"
#include "interface/taskmanager/taskmanager.h"
#include "interface/taskmanager/darwinmonitor.h"

#include "world/world.h"
#include "world/player.h"
#include "world/computer/computer.h"
#include "world/computer/recordbank.h"
#include "world/computer/computerscreen/passwordscreen.h"
#include "world/computer/computerscreen/useridscreen.h"
#include "world/generator/numbergenerator.h"
#include "world/generator/plotgenerator.h"

#include "darwinia.h"

#define NUMBER_OF_LINES		(DARWINIA_LIFE_MAX + 1)

int DarwinMonitor::selectedIndex = 0;
int DarwinMonitor::pageStart = 0;


DarwinMonitor::DarwinMonitor () : UplinkTask ()
{

}

DarwinMonitor::~DarwinMonitor()
{

}

void DarwinMonitor::Initialise ()
{
}

void DarwinMonitor::Tick ( int n )
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );
		char bname [32];
		UplinkSnprintf( bname, sizeof(bname), "darwinmonitor %d", pid );
	
		EclDirtyButton(bname);
	}

}


void DarwinMonitor::LeftClick ( Button *button )
{
	int pid;
	char bname [64];
	sscanf ( button->name, "%s %d", bname, &pid );

	if ( selectedIndex > 0 )
	{
		selectedIndex--;
		
		char selectedname [64];
		UplinkSnprintf ( selectedname, sizeof(selectedname), "darwinmonitor_selected %d", pid );

		EclRegisterCaptionChange( selectedname, game->GetWorld ()->GetDarwinia ()->GetIsland (selectedIndex)->name );
		pageStart = (selectedIndex/(NUMBER_OF_LINES-1)) *(NUMBER_OF_LINES-1);
	}
}
void DarwinMonitor::RightClick ( Button *button )
{
	int pid;
	char bname [64];
	sscanf ( button->name, "%s %d", bname, &pid );

	if ( selectedIndex < (game->GetWorld ()->GetDarwinia ()->CountIslands()-1) )
	{
		selectedIndex++;
		
		char selectedname [64];
		UplinkSnprintf ( selectedname, sizeof(selectedname), "darwinmonitor_selected %d", pid );

		EclRegisterCaptionChange( selectedname, game->GetWorld ()->GetDarwinia ()->GetIsland (selectedIndex)->name );
		pageStart = (selectedIndex/(NUMBER_OF_LINES-1)) *(NUMBER_OF_LINES-1);
	}
}

void DarwinMonitor::DarwinMonitorDraw ( Button *button, bool highlighted, bool clicked )
{
	Darwinia *darwinia = game->GetWorld ()->GetDarwinia ();

	int pid;
	char bname [64];
	sscanf ( button->name, "%s %d", bname, &pid );
    DarwinMonitor *ual = (DarwinMonitor *) SvbGetTask ( pid );
    UplinkAssert (ual);

    //
    // Draw the background

	glBegin ( GL_QUADS );		
		glColor3ub ( 8, 20, 0 );		glVertex2i ( button->x, button->y + button->height );
		glColor3ub ( 8, 20, 124 );		glVertex2i ( button->x, button->y );
		glColor3ub ( 8, 20, 0 );		glVertex2i ( button->x + button->width, button->y );
		glColor3ub ( 8, 20, 124 );		glVertex2i ( button->x + button->width, button->y + button->height );
	glEnd ();

	glColor3ub ( 81, 138, 215 );
	border_draw ( button );

	glBegin ( GL_LINE_LOOP );

		glVertex2i ( button->x + 2,			button->y + 2 );
		glVertex2i ( button->x + 185,		button->y + 2);
		glVertex2i ( button->x + 185,		button->y + button->height - 40 );
		glVertex2i ( button->x + 2,			button->y + button->height - 40 );

	glEnd ();


	UplinkAssert (button);

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
	glEnable ( GL_SCISSOR_TEST );


    //
    // Draw the names
    
	//Computer *comp = game->GetWorld ()->GetComputer ( "Darwin Research Associates Project Server" );
	//UplinkAssert ( comp );


	glColor3f ( 0.0, 1.0, 0.0 );
	//GciDrawText ( button->x + 10, button->y + 15, "Garden" );

	// Draw Titles

	GciDrawText ( button->x +  10, button->y + 15 , "Island" );
	GciDrawText ( button->x + 100, button->y + 15 , "Status" );
	GciDrawText ( button->x + 190, button->y + 15 , "Digital Lifeforms" );
	//GciDrawText ( button->x + 235, button->y + 15 , "Red" );
	//GciDrawText ( button->x + 280, button->y + 15 , "Other Features" );
	GciDrawText ( button->x + 340, button->y + 15 , "Other Features" );
	//GciDrawText ( button->x + 390, button->y + 15 , "Incubators" );
	glColor3f ( 1.0, 1.0, 1.0 );

	int souls[3];
	souls[0] = souls[1] = souls[2] = 0;

	for ( int i = 0; i < darwinia->CountIslands(); i++ )
	{
		int offset = i - pageStart;

		DarwiniaIsland *island = darwinia->GetIsland(i);

		if ( i == selectedIndex ) { glColor3f ( 1.0, 1.0, 0.0 ); }
		else { 	glColor3f ( 1.0, 1.0, 1.0 ); }

		if ( offset >= 0 && offset <= (NUMBER_OF_LINES-2) )
		{
			//char debugstring[64];
			//int push = island->GetPushFactor();
			//UplinkSnprintf(debugstring, sizeof(debugstring), "Push: %d", push);
			//GciDrawText ( button->x + 10, button->y + 15 + (offset+1) * 15, debugstring );
			GciDrawText ( button->x + 10, button->y + 15 + (offset+1) * 15, island->name );

			int stat = island->status;

			if ( stat & DARWINIA_STATUS_DELETED ) {
				glColor3f ( 0.5, 0.5, 0.5 );
				GciDrawText ( button->x + 100, button->y + 15 + (offset+1) * 15, "Destroyed" ); }
			else if ( stat & DARWINIA_STATUS_OFFLINE ) {
				glColor3f ( 0.5, 0.5, 0.5 );
				GciDrawText ( button->x + 100, button->y + 15 + (offset+1) * 15, "Server Offline" ); }
			else if ( stat & DARWINIA_STATUS_INFECTED ) {
				glColor3f ( 1.0, 0.0, 0.0 );
				GciDrawText ( button->x + 100, button->y + 15 + (offset+1) * 15, "Infected" ); }
			else if ( stat & DARWINIA_STATUS_CORRUPTED ) {
				glColor3f ( 0.5, 0.5, 0.0 );
				GciDrawText ( button->x + 100, button->y + 15 + (offset+1) * 15, "Corrupted" ); }
			else {
				glColor3f ( 1.0, 1.0, 1.0 );
				GciDrawText ( button->x + 100, button->y + 15 + (offset+1) * 15, "System Stable" ); }

			glColor3f ( 1.0, 1.0, 1.0 );
			/*if ( !(stat & DARWINIA_STATUS_DELETED) )
			{
				char pop[64];

				int green = island->GetLife(DARWINIA_LIFE_DARWINIAN);
				UplinkSnprintf(pop,sizeof(pop),"%d", green);
				GciDrawText ( button->x + 190, button->y + 15 + (offset+1) * 15, pop );

				int red   = island->GetLife(DARWINIA_LIFE_REDDARWINIAN);
				UplinkSnprintf(pop,sizeof(pop),"%d", red);
				GciDrawText ( button->x + 235, button->y + 15 + (offset+1) * 15, pop );

				int squads = island->GetLife(DARWINIA_LIFE_SQUADDIE);
				UplinkSnprintf(pop,sizeof(pop),"%d", squads);
				GciDrawText ( button->x + 280, button->y + 15 + (offset+1) * 15, pop );

				int eng = island->GetLife(DARWINIA_LIFE_ENGINEER);
				UplinkSnprintf(pop,sizeof(pop),"%d", eng);
				GciDrawText ( button->x + 325, button->y + 15 + (offset+1) * 15, pop );

				int inc = island->GetIncubators();
				UplinkSnprintf(pop,sizeof(pop),"%d", inc);
				GciDrawText ( button->x + 390, button->y + 15 + (offset+1) * 15, pop );

			}*/
					}
		for ( int s = 0; s < 3; s++ )
			souls[s] = souls[s] + island->GetSouls(s);
	}

	glColor3f ( 1.0, 1.0, 1.0 );

	char soultext[128];

	UplinkSnprintf(soultext, sizeof(soultext),"Free Souls: %d", souls[0]);
	GciDrawText ( button->x + 10, button->y + (NUMBER_OF_LINES * 15) + 22, soultext );

	UplinkSnprintf(soultext, sizeof(soultext),"Processed Souls: %d", souls[1]);
	GciDrawText ( button->x + 156, button->y + (NUMBER_OF_LINES * 15) + 22, soultext );

	UplinkSnprintf(soultext, sizeof(soultext),"Lost Souls: %d", souls[2]);
	GciDrawText ( button->x + 303, button->y + (NUMBER_OF_LINES * 15) + 22, soultext );

	int infection = darwinia->GetPatternInfection();
	int corruption = darwinia->GetPatternCorruption();

	if ( infection == -1 || corruption == -1 ) {
		UplinkSnprintf(soultext, sizeof(soultext), "No Pattern Buffer Found"); }
	else if ( infection && corruption ) {
		UplinkSnprintf(soultext, sizeof(soultext), "Pattern Buffer %d Infected, %d Corrupted", infection, corruption); }
	else if ( infection ) {
		UplinkSnprintf(soultext, sizeof(soultext), "Pattern Buffer %d Infected", infection); }
	else if ( corruption ) {
		UplinkSnprintf(soultext, sizeof(soultext), "Pattern Buffer %d Corrupted", corruption); }
	else {
		UplinkSnprintf(soultext, sizeof(soultext), "Pattern Buffer Stable"); }
	GciDrawText ( button->x + 10, button->y + (NUMBER_OF_LINES * 15) + 37, soultext );

	//float infection = game->GetWorld ()->plotgenerator.darwinia->GetPatternInfection();
	//if ( infection > 0.0 ) { sprintf(souls,"Pattern Infection: %d%%", (int) (infection * 100.0) ); }
	//else { sprintf(souls,"Pattern Stable"); }
	//GciDrawText ( button->x + 10, button->y + 275, souls );

	DarwiniaIsland *selected = darwinia->GetIsland(selectedIndex);
	UplinkAssert(selected);

	if ( selected->status & DARWINIA_STATUS_DELETED )
	{
		glColor3f ( 1.0, 0.0, 0.0 );
		GciDrawText ( button->x + 190, button->y + 30, "Island Destroyed" );
		glColor3f ( 1.0, 1.0, 1.0 );
	}
	else if ( selected->status & DARWINIA_STATUS_OFFLINE )
	{
		glColor3f ( 0.5, 0.5, 0.5 );
		GciDrawText ( button->x + 190, button->y + 30, "Server Offline" );
		glColor3f ( 1.0, 1.0, 1.0 );
	}
	else
	{
		int count = 2;
		for ( int p = 0; p < DARWINIA_LIFE_MAX; p++ )
		{
			int num1 = selected->GetLife(p);
			int num2 = selected->GetLifeChange(p);

			if ( num1 > 0 || num2 != 0 )
			{
				UplinkSnprintf(soultext, sizeof(soultext), "%s", darwinia->GetLifeformName(p));
				GciDrawText ( button->x + 190, button->y + (15*count), soultext );
				UplinkSnprintf(soultext, sizeof(soultext), "%d (%d)", num1, num2);
				GciDrawText ( button->x + 280, button->y + (15*count), soultext );
				count++;
			}
		}

		count = 2;
		if ( selected->GetSpawnPoints() > 0 )
		{
			UplinkSnprintf(soultext, sizeof(soultext), "Spawn Points: %d", selected->GetSpawnPoints());
			GciDrawText ( button->x + 340, button->y + (15*count), soultext );
			count++;
		}
		if ( selected->GetIncubators() > 0 )
		{
			UplinkSnprintf(soultext, sizeof(soultext), "Incubators: %d", selected->GetIncubators());
			GciDrawText ( button->x + 340, button->y + (15*count), soultext );
			count++;
		}
		if ( selected->GetAntHills() > 0 )
		{
			UplinkSnprintf(soultext, sizeof(soultext), "Ant Hills: %d", selected->GetAntHills());
			GciDrawText ( button->x + 340, button->y + (15*count), soultext );
			count++;
		}

		if ( count > 2 )
			count++;

		if ( selected->features & DARWINIA_FEATURE_FENCES ) {
			GciDrawText ( button->x + 340, button->y + (15*count), "Laser Fence Network" );
			count++; }
		if ( selected->features & DARWINIA_FEATURE_REFINERY ) {
			GciDrawText ( button->x + 340, button->y + (15*count), "Pixel Refinery" );
			count++; }
		if ( selected->features & DARWINIA_FEATURE_GENERATOR ) {
			GciDrawText ( button->x + 340, button->y + (15*count), "Power Generator" );
			count++; }
		if ( selected->features & DARWINIA_FEATURE_YARD ) {
			GciDrawText ( button->x + 340, button->y + (15*count), "Construction Yard" );
			count++; }
		if ( selected->features & DARWINIA_FEATURE_RECEIVER ) {
			GciDrawText ( button->x + 340, button->y + (15*count), "Soul Receiver Network" );
			count++; }
		if ( selected->features & DARWINIA_FEATURE_PATTERN ) {
			GciDrawText ( button->x + 340, button->y + (15*count), "Darwinian Pattern Buffer" );
			count++; }
		if ( selected->features & DARWINIA_FEATURE_UPLINK ) {
			GciDrawText ( button->x + 340, button->y + (15*count), "External Connection" );
			count++; }

		count++;

		GciDrawText ( button->x + 340, button->y + (15*count), "Trunk Ports" );
		count ++; ;

		int ports = darwinia->GetLinks(selected->name);
		for ( int p = 0; p < ports; p++ )
		{
			UplinkSnprintf(soultext, sizeof(soultext), "> %s", darwinia->GetLink(selected->name, p));
			GciDrawText ( button->x + 345, button->y + (15*count), soultext );
			count++;
		}
		
	}
    glDisable ( GL_SCISSOR_TEST );
}

void DarwinMonitor::SetTarget ( UplinkObject *uo, char *uos, int uoi )
{


}

void DarwinMonitor::CloseClick ( Button *button )
{

	int pid;
	char bname [64];
	sscanf ( button->name, "%s %d", bname, &pid );

	SvbRemoveTask ( pid );

}

void DarwinMonitor::TitleClick ( Button *button )
{

	int pid;
	char bname [64];
	sscanf ( button->name, "%s %d", bname, &pid );
    DarwinMonitor *ual = (DarwinMonitor *) SvbGetTask ( pid );
    UplinkAssert (ual);

	ual->followmouse = true;
	game->GetInterface ()->GetTaskManager ()->SetTargetProgram ( pid );

}

void DarwinMonitor::SquadClick ( Button *button )
{
	Darwinia *darwinia = game->GetWorld ()->GetDarwinia ();
	DarwiniaIsland *island = darwinia->GetIsland ( selectedIndex );
	UplinkAssert(island);
	if ( island->status & DARWINIA_STATUS_DELETED || island->status & DARWINIA_STATUS_OFFLINE )
	{
	}
	else
	{
		island->RunTask(DARWINIA_LIFE_SQUADDIE);
	}
	//game->GetWorld ()->plotgenerator.darwinia->InsertSquad( selectedIndex );
}

void DarwinMonitor::EngineerClick ( Button *button )
{
	Darwinia *darwinia = game->GetWorld ()->GetDarwinia ();
	DarwiniaIsland *island = darwinia->GetIsland ( selectedIndex );
	UplinkAssert(island);
	if ( island->status & DARWINIA_STATUS_DELETED || island->status & DARWINIA_STATUS_OFFLINE )
	{
	}
	else
	{
		island->RunTask(DARWINIA_LIFE_ENGINEER);
	}
	//game->GetWorld ()->plotgenerator.darwinia->InsertEngineer( selectedIndex );
}

void DarwinMonitor::CreateInterface ()
{

	if ( !IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

        int x = 100;
        int y = 100;
        int width = 470;
        int height = (NUMBER_OF_LINES * 15) + 45;

        //
        // Create the list

		char bname [32];
		UplinkSnprintf ( bname,sizeof(bname), "darwinmonitor %d", pid );
	
		EclRegisterButton ( x, y, width, height, "", "", bname );
		EclRegisterButtonCallbacks ( bname, DarwinMonitorDraw, NULL, button_click, button_highlight );
		EclRegisterCaptionChange ( bname, "Darwinia Monitor" );

        //
        // Title bar

        char titlename [64];
        UplinkSnprintf ( titlename, sizeof(titlename), "darwinmonitor_title %d", pid );
        EclRegisterButton ( x, y - 14, width - 13, 14, "Darwin Research Associates Project Monitor", "Click to move this tool", titlename );
        EclRegisterButtonCallback ( titlename, TitleClick );

        
        //
        // Create the close button

		char closename [64];
		UplinkSnprintf ( closename, sizeof(closename), "darwinmonitor_close %d", pid );

		EclRegisterButton ( width + x - 13, y - 14, 13, 13, "X", "Close the Monitor", closename );
		button_assignbitmaps ( closename, "close.tif", "close_h.tif", "close_c.tif" );
		EclRegisterButtonCallback ( closename, CloseClick );
		  
		if ( version >= 2.0 )
		{
			char leftname [64];
			UplinkSnprintf ( leftname, sizeof(leftname), "darwinmonitor_left %d", pid );
			EclRegisterButton ( x, y + height+1, 15,15, "<<", "Select Previous Island", leftname );
			EclRegisterButtonCallback( leftname, LeftClick );

			char selectedname [64];
			UplinkSnprintf ( selectedname, sizeof(selectedname), "darwinmonitor_selected %d", pid );
			EclRegisterButton ( x + 15, y + height+1, 100,15, game->GetWorld ()->GetDarwinia ()->GetIsland (0)->name, "Selected Island", selectedname );
			EclRegisterButtonCallbacks( selectedname, textbutton_draw, NULL, NULL, NULL );

			char rightname [64];
			UplinkSnprintf ( rightname, sizeof(rightname), "darwinmonitor_right %d", pid );
			EclRegisterButton ( x + 115, y + height+1, 15,15, ">>", "Select Next Island", rightname );
			EclRegisterButtonCallback( rightname, RightClick );

			char squadname [64];
			UplinkSnprintf ( squadname, sizeof(squadname), "darwinmonitor_squad %d", pid );
			EclRegisterButton ( x + 145, y + height+1, 50,15, "Squad", "Deploy Squad", squadname );
			EclRegisterButtonCallback( squadname, SquadClick );

			char engname [64];
			UplinkSnprintf ( engname, sizeof(engname), "darwinmonitor_engineer %d", pid );
			EclRegisterButton ( x + 200, y + height+1, 50,15, "Engineer", "Deploy Engineer", engname );
			EclRegisterButtonCallback( engname, EngineerClick );

		}
	}

}

void DarwinMonitor::MoveTo ( int x, int y, int time_ms )
{

	int pid = SvbLookupPID ( this );

    int width = 470;
    int height = (NUMBER_OF_LINES * 15) + 45;

	char bname [128];
	UplinkSnprintf ( bname, sizeof(bname), "darwinmonitor %d", pid );
	EclRegisterMovement ( bname, x, y, time_ms );

	char closename [64];
	UplinkSnprintf ( closename, sizeof(closename), "darwinmonitor_close %d", pid );
    EclRegisterMovement ( closename, x + width - 13, y - 14, time_ms );
	
    char titlename [64];
    UplinkSnprintf ( titlename, sizeof(titlename), "darwinmonitor_title %d", pid );
    EclRegisterMovement ( titlename, x, y - 14, time_ms );

	if ( version >= 2.0 )
	{
		char leftname [64];
		UplinkSnprintf ( leftname, sizeof(leftname), "darwinmonitor_left %d", pid );
		EclRegisterMovement ( leftname, x, y + height + 1, time_ms );

		char selectedname [64];
		UplinkSnprintf ( selectedname, sizeof(selectedname), "darwinmonitor_selected %d", pid );
		EclRegisterMovement ( selectedname, x+15, y + height + 1, time_ms );

		char rightname [64];
		UplinkSnprintf ( rightname, sizeof(rightname), "darwinmonitor_right %d", pid );
		EclRegisterMovement ( rightname, x+115, y + height + 1, time_ms );

		char squadname [64];
		UplinkSnprintf ( squadname, sizeof(squadname), "darwinmonitor_squad %d", pid );
		EclRegisterMovement ( squadname, x+145, y + height + 1, time_ms );

		char engname [64];
		UplinkSnprintf ( engname, sizeof(engname), "darwinmonitor_engineer %d", pid );
		EclRegisterMovement ( engname, x+200, y + height + 1, time_ms );
	}

}

void DarwinMonitor::RemoveInterface ()
{

	if ( IsInterfaceVisible () ) {

		int pid = SvbLookupPID ( this );

		char bname [128];
		UplinkSnprintf ( bname, sizeof(bname), "darwinmonitor %d", pid );
		EclRemoveButton ( bname );

		char closename [64];
		UplinkSnprintf ( closename, sizeof(closename), "darwinmonitor_close %d", pid );
		EclRemoveButton ( closename );

        char titlename [64];
        UplinkSnprintf ( titlename, sizeof(titlename), "darwinmonitor_title %d", pid );
        EclRemoveButton ( titlename );

		if ( version >= 2.0 )
		{
			char leftname [64];
			UplinkSnprintf ( leftname, sizeof(leftname), "darwinmonitor_left %d", pid );
			EclRemoveButton ( leftname );

			char selectedname [64];
			UplinkSnprintf ( selectedname, sizeof(selectedname), "darwinmonitor_selected %d", pid );
			EclRemoveButton ( selectedname );

			char rightname [64];
			UplinkSnprintf ( rightname, sizeof(rightname), "darwinmonitor_right %d", pid );
			EclRemoveButton ( rightname );

			char squadname [64];
			UplinkSnprintf ( squadname, sizeof(squadname), "darwinmonitor_squad %d", pid );
			EclRemoveButton ( squadname );

			char engname [64];
			UplinkSnprintf ( engname,sizeof(engname),  "darwinmonitor_engineer %d", pid );
			EclRemoveButton ( engname );
		}
	}

}

void DarwinMonitor::ShowInterface ()
{

	if ( !IsInterfaceVisible () ) CreateInterface ();

	int pid = SvbLookupPID ( this );

	char bname [128];
	sprintf ( bname, "darwinmonitor %d", pid );
	EclButtonBringToFront ( bname );

	char closename [64];
	sprintf ( closename, "darwinmonitor_close %d", pid );
	EclButtonBringToFront ( closename );

    char titlename [64];
    sprintf ( titlename, "darwinmonitor_title %d", pid );
    EclButtonBringToFront ( titlename );

	if ( version >= 2.0 )
	{
		char leftname [64];
		UplinkSnprintf ( leftname, sizeof(leftname), "darwinmonitor_left %d", pid );
		EclButtonBringToFront ( leftname );

		char selectedname [64];
		UplinkSnprintf ( selectedname, sizeof(selectedname), "darwinmonitor_selected %d", pid );
		EclButtonBringToFront ( selectedname );

		char rightname [64];
		UplinkSnprintf ( rightname, sizeof(rightname), "darwinmonitor_right %d", pid );
		EclButtonBringToFront ( rightname );

		char squadname [64];
		UplinkSnprintf ( squadname, sizeof(squadname), "darwinmonitor_squad %d", pid );
		EclButtonBringToFront ( squadname );

		char engname [64];
		UplinkSnprintf ( engname,sizeof(engname),  "darwinmonitor_engineer %d", pid );
		EclButtonBringToFront ( engname );
	}
}

bool DarwinMonitor::IsInterfaceVisible ()
{

	char bname [128];
	UplinkSnprintf ( bname, sizeof(bname), "darwinmonitor %d", SvbLookupPID (this) );

	return ( EclGetButton ( bname ) != NULL );

}

