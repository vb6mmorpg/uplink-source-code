
/*
	Interface for van bakel library
	By Christopher Delay

  */

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h> /* GLU extention library */

#include <stdio.h>
#include <assert.h>

#include "eclipse.h"
#include "interface.h"
#include "vanbakel.h"
#include "gucci.h"

#include "mmgr.h"


void Svb_initialise_interface ()
{

}

void Svb_button_draw ( Button *button, bool highlighted, bool clicked )
{

	assert ( button );

	int xpos = button->x + 2;
	int ypos = (button->y + button->height / 2) + 1;
		
	glBegin ( GL_QUADS );

		if ( clicked ) glColor4f ( 0.5f, 0.5f, 1.0f, 0.6f );
		else if ( highlighted ) glColor4f ( 0.2f, 0.2f, 0.9f, 0.6f );
		else glColor4f ( 0.0f, 0.0f, 0.4f, 0.6f );
		glVertex3i ( button->x, button->y, 0 );

		if ( clicked ) glColor4f ( 0.7f, 0.7f, 1.0f, 0.6f );
		else if ( highlighted ) glColor4f ( 0.5f, 0.5f, 1.0f, 0.6f );
		else glColor4f ( 0.0f, 0.0f, 0.7f, 0.6f );
		glVertex3i ( button->x + button->width, button->y, 0 );

		if ( clicked ) glColor4f ( 0.5f, 0.5f, 1.0f, 0.6f );
		else if ( highlighted ) glColor4f ( 0.2f, 0.2f, 0.9f, 0.6f );
		else glColor4f ( 0.0f, 0.0f, 0.4f, 0.6f );
		glVertex3i ( button->x + button->width, button->y + button->height, 0 );

		if ( clicked ) glColor4f ( 0.7f, 0.7f, 1.0f, 0.6f );
		else if ( highlighted ) glColor4f ( 0.5f, 0.5f, 1.0f, 0.6f );
		else glColor4f ( 0.0f, 0.0f, 0.7f, 0.6f );
		glVertex3i ( button->x, button->y + button->height, 0 );

	glEnd ();

	glColor4f ( 1.0f, 1.0f, 1.0f, 0.8f );
    GciDrawText ( xpos, ypos, button->caption, HELVETICA_10 );

}

void Svb_textbutton_draw ( Button *button, bool highlighted, bool clicked )
{

	assert ( button );

	// Clear the background
	
	glColor4f ( 0.0f, 0.0f, 0.0f, 1.0f );
		glVertex2i ( button->x,	button->y);
		glVertex2i ( button->x + button->width, button->y );
		glVertex2i ( button->x + button->width, button->y + button->height );
		glVertex2i ( button->x, button->y + button->height );
	glEnd ();

	// Draw the text

	int xpos = button->x + 2;
	int ypos = (button->y + button->height / 2) + 1;

	glColor4f ( 1.0f, 1.0f, 1.0f, 0.8f );    
    GciDrawText ( xpos, ypos, button->caption, HELVETICA_10 );

}

void Svb_column_draw ( Button *button, bool highlighted, bool clicked )
{

	assert ( button );

	int xpos = button->x + 5;
	int ypos = (button->y + button->height / 2) + 2;
		
	float scale = button->width / 50.0f;
	if ( highlighted ) scale *= 2;

	glBegin ( GL_QUADS );

		glColor4f ( 0.0f, 2.0f - scale, scale, 0.6f );
		glVertex3i ( button->x, button->y, 0 );

		glColor4f ( 0.0f, 2.0f - scale, scale, 0.6f );
		glVertex3i ( button->x + button->width, button->y, 0 );

		glColor4f ( 0.0f, 2.0f - scale, scale, 0.6f );
		glVertex3i ( button->x + button->width, button->y + button->height, 0 );

		glColor4f ( 0.0f, 2.0f - scale, scale, 0.6f );
		glVertex3i ( button->x, button->y + button->height, 0 );

	glEnd ();

	glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );
    GciDrawText ( xpos, ypos, button->caption, HELVETICA_10 );

}

local void Svb_button_highlight ( Button *button )
{ 

	assert ( button );

	EclHighlightButton ( button->name );
	//GciPostRedisplay ();

}

local void Svb_button_click ( Button *button )
{

	assert ( button );

	EclClickButton ( button->name );
	//GciPostRedisplay ();

}

local void Svb_decrease_click ( Button *button )
{

	assert ( button );

	int index;
	char unused [32];
	sscanf ( button->name, "%s %d", unused, &index );

	SvbChangePriority ( index, -0.2 );
	SvbCompensatePriorities ();

	SvbUpdateInterface ();
	//GciPostRedisplay ();

}

local void Svb_column_click ( Button *button )
{

	int index;
	char unused [32];
	sscanf ( button->name, "%s %d", unused, &index );

	TaskWrapper *tw = SvbGetTaskWrapperAtIndex (index);
	assert ( tw );

	SvbShowTask ( tw->pid );

}

local void Svb_increase_click ( Button *button )
{

	assert ( button );

	int index;
	char unused [32];
	sscanf ( button->name, "%s %d", unused, &index );

	SvbChangePriority ( index, 0.2 );
	SvbCompensatePriorities ();

	SvbUpdateInterface ();
	//GciPostRedisplay ();

}

void SvbCreateColumn ( int baseX, int baseY, TaskWrapper *tw, int taskindex )
{

	if ( tw ) {

		// Create a mask button to completely cover the bar

		char name [32];
		sprintf ( name, "svb_priority_mask %d", taskindex );
		EclRegisterButton ( baseX, baseY + 8 + taskindex*15, 100, 10, "", name );
		EclRegisterButtonCallbacks ( name, NULL, NULL, NULL, NULL );
		
		// Create the bar

		sprintf ( name, "svb_priority %d", taskindex );
		char caption [32];
		sprintf ( caption, "%s", tw->name );
		
		EclRegisterButton ( baseX, baseY + 8 + taskindex*15, 0, 10, caption, name );
		EclRegisterButtonCallbacks ( name, Svb_column_draw, Svb_column_click, Svb_button_click, Svb_button_highlight );

		// Create the priority change buttons

		char name_decrease [32];
		char name_increase [32];
		sprintf ( name_decrease, "svb_decrease %d", taskindex );
		sprintf ( name_increase, "svb_increase %d", taskindex );

		EclRegisterButton ( baseX - 11, baseY + 8 + taskindex*15, 10, 10, "<", name_decrease );
		EclRegisterButton ( baseX + 101, baseY + 8 + taskindex*15, 10, 10, ">", name_increase );

		EclRegisterButtonCallbacks ( name_decrease, Svb_button_draw, Svb_decrease_click, Svb_button_click, Svb_button_highlight );
		EclRegisterButtonCallbacks ( name_increase, Svb_button_draw, Svb_increase_click, Svb_button_click, Svb_button_highlight );

	}

}

void SvbRemoveColumn ( int taskindex )
{
	
	char name [32];
	sprintf ( name, "svb_priority %d", taskindex );
	
	EclRemoveButton ( name );

	char name_decrease [32];
	char name_increase [32];
	char name_mask     [32];
	sprintf ( name_decrease, "svb_decrease %d", taskindex );
	sprintf ( name_increase, "svb_increase %d", taskindex );
	sprintf ( name_mask,     "svb_priority_mask %d", taskindex );

	EclRemoveButton ( name_decrease );
	EclRemoveButton ( name_increase );
	EclRemoveButton ( name_mask );

}

