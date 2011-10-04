
#define FUDGE 0

// Source file for button class
// Part of the Eclipse interface library
// By Christopher Delay

#include <stdio.h>
#include <string.h>

#include "eclipse.h"
#include "button.h"

#include "mmgr.h"


Button::Button ()
{

	x = y = width = height = 0;
	caption = name = tooltip = NULL;
	draw = NULL;
	image_standard = image_highlighted = image_clicked = NULL;
	mouseup = mousedown = mousemove = NULL;
	SetTooltip ( " " );
	userinfo = 0;

}

Button::Button ( int newx, int newy, int newwidth, int newheight,
				 char *newcaption, char *newname )
{

	caption = name = tooltip = NULL;
	SetProperties ( newx, newy, newwidth, newheight + FUDGE, newcaption, newname );
	draw = NULL;
	image_standard = image_highlighted = image_clicked = NULL;
	mouseup = mousedown = mousemove = NULL;
	SetTooltip ( " " );
	userinfo = 0;

}

Button::~Button ()
{

	if ( caption ) delete [] caption;	
	if ( tooltip ) delete [] tooltip;
	if ( name ) delete [] name;

	// There is a possibility that this button may use the 
	// same image for more than one of these pointers
	// So don't neccisarily delete all three

	bool delete_standard    = ( image_standard    != NULL );
	bool delete_highlighted = ( image_highlighted != NULL && image_highlighted != image_standard );
	bool delete_clicked     = ( image_clicked     != NULL && image_clicked     != image_standard 
														  && image_clicked	   != image_highlighted );

	if ( delete_standard )    delete image_standard;
	if ( delete_highlighted ) delete image_highlighted;
	if ( delete_clicked )     delete image_clicked;

}

void Button::SetProperties ( int newx, int newy, int newwidth, int newheight,
			   				 char *newcaption, char *newname )
{

	x = newx;
	y = newy;
	width = newwidth;
	height = newheight;
	if ( name )
		delete [] name;
	name = new char [strlen (newname) + 1];
	strcpy ( name, newname );
	SetCaption ( newcaption );
	
}

void Button::SetCaption ( char *newcaption )
{

	if ( !caption || strcmp ( newcaption, caption ) != 0 ) 
		Dirty ();

	if ( caption ) delete [] caption;
	caption = new char [strlen (newcaption) + 1];
	strcpy ( caption, newcaption );

}

void Button::SetTooltip ( char *newtooltip )
{

	if ( tooltip ) delete [] tooltip;

	tooltip = new char [strlen (newtooltip) + 1];
	strcpy ( tooltip, newtooltip );

}

void Button::SetStandardImage ( Image *newimage )
{

	if ( image_standard ) delete image_standard;
	image_standard = newimage;

}

void Button::SetImages ( Image *newstandard, Image *newhighlighted, Image *newclicked )
{

	if ( image_standard )    delete image_standard;
	if ( image_highlighted ) delete image_highlighted;
	if ( image_clicked )     delete image_clicked;

	image_standard = newstandard;
	image_highlighted = newhighlighted;
	image_clicked = newclicked;

}

void Button::RegisterDrawFunction ( void (*newdraw) ( Button *, bool, bool ) )
{

	draw = newdraw;

}

void Button::RegisterMouseUpFunction ( void (*newmouseup) ( Button * ) )
{

	mouseup = newmouseup;

}

void Button::RegisterMouseDownFunction ( void (*newmousedown) ( Button * ) )
{

	mousedown = newmousedown;

}

void Button::RegisterMouseMoveFunction  ( void (*newmousemove) ( Button * ) )
{

	mousemove = newmousemove;

}

void Button::Dirty ()
{

	dirty = true;

}

void Button::Draw ( bool highlighted, bool clicked )
{

	if ( draw ) draw ( this, highlighted, clicked );

}

void Button::MouseUp ()
{

	if ( mouseup ) mouseup ( this );

}

void Button::MouseDown ()
{

	if ( mousedown ) mousedown ( this );

}

void Button::MouseMove ()
{

	if ( mousemove ) mousemove ( this );

}

void Button::DebugPrint ()
{

	printf ( "BUTTON : name:'%s', x:%d, y:%d, width:%d, height:%d\n", name, x, y, width, height );
	printf ( "         caption:'%s', tooltip:'%s'\n", caption, tooltip );

}

