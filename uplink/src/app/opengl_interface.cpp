
#include "stdafx.h"

#include <time.h>

#include <GL/gl.h>

#include <GL/glu.h> /* glu extention library */

#include <stdio.h>

#include "eclipse.h"
#include "opengl_interface.h"
#include "soundgarden.h"
#include "gucci.h"
#include "redshirt.h"

#include "options/options.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"
#include "app/miscutils.h"

#include "interface/scrollbox.h"

#include "mmgr.h"

local int tooltipanimindex = -1;                // Index of animation running on tooltip button
local char currentbuttonname [512] = {0};       // Button mouse is currently over

local int superhighlight_flash = 0;			    // Used to flash superhighlighted buttons
local time_t curserflash = 0;					// Used to flash curser

local float windowScaleX = 1.0;                 // Used to convert pixel space onto screen size
local float windowScaleY = 1.0;                 


// ================================================
// Backdrop stuff

//#define USE_DESKTOPBACKDROP
//#define USE_BUTTONBACKDROP

local Image *backdrop = NULL;					// Used for semi-transparent effects


void initialise_transparency ()
{

	if ( !backdrop ) {

		backdrop = new Image ();
        char *filename = app->GetOptions()->ThemeFilename("backdrops/loading.tif");
		backdrop->LoadTIF ( RsArchiveFileOpen ( filename ) );
		backdrop->ScaleToOpenGL ();
		backdrop->FlipAroundH ();
        delete [] filename;
				
	}

}

void clear_draw ( int x, int y, int w, int h ) 
{

#ifdef USEDESKTOPBACKDROP

	if ( !backdrop ) initialise_transparency ();

	glPushAttrib ( GL_ALL_ATTRIB_BITS );

	glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );
	glEnable ( GL_TEXTURE_2D );

	glBindTexture ( GL_TEXTURE_2D, 1 );
	glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, backdrop->Width(), backdrop->Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, backdrop->pixels );

	float scaleX = (float) x / 640.0;
	float scaleY = (float) y / 480.0;
	float scaleW = (float) w / 640.0;
	float scaleH = (float) h / 480.0;

	glBegin(GL_QUADS);
		glTexCoord2f(scaleX, scaleY);						glVertex2f(x, y);
		glTexCoord2f(scaleX + scaleW, scaleY);				glVertex2f(x + w, y);
		glTexCoord2f(scaleX + scaleW, scaleY + scaleH);		glVertex2f(x + w, y + h);
		glTexCoord2f(scaleX, scaleY + scaleH );				glVertex2f(x, y + h);
	glEnd ();

	glPopAttrib ();

#else

    SetColour ( "Background" );

	glBegin ( GL_QUADS );

		glVertex2i ( x,		y	  );
		glVertex2i ( x + w, y	  );
		glVertex2i ( x + w, y + h );
		glVertex2i ( x,		y + h );

	glEnd ();

#endif

}

void button_draw ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert (  button );

#ifdef USE_BUTTONBACKDROP

	// ============================================================
	// Transparency effects

	if ( !backdrop ) initialise_transparency ();

	glPushAttrib ( GL_ALL_ATTRIB_BITS );

	glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );
	glEnable ( GL_TEXTURE_2D );

	glBindTexture ( GL_TEXTURE_2D, 1 );
	glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, backdrop->Width(), backdrop->Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, backdrop->pixels );

	float scaleX = (float) button->x / 640.0;
	float scaleY = (float) button->y / 480.0;
	float scaleW = (float) button->width / 640.0;
	float scaleH = (float) button->height / 480.0;

	glBegin(GL_QUADS);
		glTexCoord2f(scaleX, scaleY);						glVertex2f(button->x, button->y);
		glTexCoord2f(scaleX + scaleW, scaleY);				glVertex2f(button->x + button->width, button->y);
		glTexCoord2f(scaleX + scaleW, scaleY + scaleH);		glVertex2f(button->x + button->width, button->y + button->height);
		glTexCoord2f(scaleX, scaleY + scaleH );				glVertex2f(button->x, button->y + button->height);
	glEnd ();

	glPopAttrib ();

	// ============================================================

#else
	
	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
 	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
 	glEnable ( GL_SCISSOR_TEST );

	// Draw the button
	
	glBegin ( GL_QUADS );

		if		( clicked )		SetColour ( "ButtonClickedA" );
		else if ( highlighted ) SetColour ( "ButtonHighlightedA" );
		else					SetColour ( "ButtonNormalA" );
		glVertex2i ( button->x, button->y + button->height );

		if      ( clicked )		SetColour ( "ButtonClickedB" );
		else if ( highlighted ) SetColour ( "ButtonHighlightedB" );
		else					SetColour ( "ButtonNormalB" );
		glVertex2i ( button->x, button->y );

		if		( clicked )		SetColour ( "ButtonClickedA" );
		else if ( highlighted ) SetColour ( "ButtonHighlightedA" );
		else					SetColour ( "ButtonNormalA" );
		glVertex2i ( button->x + button->width, button->y );

		if		( clicked )		SetColour ( "ButtonClickedB" );
		else if ( highlighted ) SetColour ( "ButtonHighlightedB" );
		else					SetColour ( "ButtonNormalB" );
		glVertex2i ( button->x + button->width, button->y + button->height );

	glEnd ();
	
#endif

	// Draw the text
	int xpos = (button->x + button->width  / 2) - ( GciTextWidth ( button->caption ) / 2 );
	int ypos = (button->y + button->height / 2) + 2;

    SetColour ( "DefaultText" );
    GciDrawText ( xpos, ypos, button->caption );

 	glDisable ( GL_SCISSOR_TEST );
}

void imagebutton_draw ( Button *button, bool highlighted, bool clicked )
{

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
	glEnable ( GL_SCISSOR_TEST );

	UplinkAssert (button);
	
	if ( clicked && button->image_clicked ) {
		button->image_clicked->Draw ( button->x, button->y );
	}
	else if ( highlighted && button->image_highlighted ) {
		button->image_highlighted->Draw ( button->x, button->y );
	}
	else {
		UplinkAssert ( button->image_standard );
		button->image_standard->Draw ( button->x, button->y );
	}

	glDisable ( GL_SCISSOR_TEST );

}

void imagebutton_draw ( Button *button, bool highlighted, bool clicked, Image *standard_i_ref, Image *highlighted_i_ref, Image *clicked_i_ref )
{

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
	glEnable ( GL_SCISSOR_TEST );

	UplinkAssert (button);
	
	if ( clicked && clicked_i_ref ) {
		clicked_i_ref->Draw ( button->x, button->y );
	}
	else if ( highlighted && highlighted_i_ref ) {
		highlighted_i_ref->Draw ( button->x, button->y );
	}
	else if ( standard_i_ref ) {
		standard_i_ref->Draw ( button->x, button->y );
	}

	glDisable ( GL_SCISSOR_TEST );

}

void imagebutton_draw_blend ( Button *button, bool highlighted, bool clicked )
{

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
	glEnable ( GL_SCISSOR_TEST );

	UplinkAssert (button);
	
	if ( clicked && button->image_clicked ) {
		UplinkAssert (button->image_clicked);
		button->image_clicked->DrawBlend ( button->x, button->y );
	}
	else if ( highlighted && button->image_highlighted ) {
		UplinkAssert (button->image_highlighted);
		button->image_highlighted->DrawBlend ( button->x, button->y );
	}
	else {
		UplinkAssert ( button->image_standard );
		button->image_standard->DrawBlend ( button->x, button->y );
	}

	glDisable ( GL_SCISSOR_TEST );

}

static float currentX = 0.1f;
static float currentY = 0.1f;

void imagebutton_drawtextured ( Button *button, bool highlighted, bool clicked )
{

#ifdef USE_BUTTONBACKDROP

	// ============================================================
	// Transparency effects

	if ( !backdrop ) initialise_transparency ();

	glPushAttrib ( GL_ALL_ATTRIB_BITS );

	glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );
	glEnable ( GL_TEXTURE_2D );
	glEnable ( GL_BLEND );
	glBlendFunc ( GL_ONE, GL_ZERO );

	glBindTexture ( GL_TEXTURE_2D, 1 );
	glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, backdrop->Width(), backdrop->Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, backdrop->pixels );

	currentY += 0.003;
	float scaleX = currentX;
	float scaleY = currentY;
	float scaleW = 0.8;
	float scaleH = 0.5;

	glBegin(GL_QUADS);
		glTexCoord2f(scaleX, scaleY);						glVertex2f(button->x, button->y);
		glTexCoord2f(scaleX + scaleW, scaleY);				glVertex2f(button->x + button->width, button->y);
		glTexCoord2f(scaleX + scaleW, scaleY + scaleH);		glVertex2f(button->x + button->width, button->y + button->height);
		glTexCoord2f(scaleX, scaleY + scaleH );				glVertex2f(button->x, button->y + button->height);
	glEnd ();

	// ============================================================

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
	glEnable ( GL_SCISSOR_TEST );
	glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	UplinkAssert (button);
	
	Image *image = NULL;
	if ( clicked && button->image_clicked ) {
		UplinkAssert (button->image_clicked);
		image = button->image_clicked;
	}
	else if ( highlighted && button->image_highlighted ) {
		UplinkAssert (button->image_highlighted);
		image = button->image_highlighted;
	}
	else {
		UplinkAssert ( button->image_standard );
		image = button->image_standard;
	}

	UplinkAssert ( image );
	
	glBindTexture ( GL_TEXTURE_2D, 1 );
	glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGBA, image->Width(), image->Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels );

	// Scale the image to fit the button size

	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 1.0);		glVertex2f(button->x, button->y);
		glTexCoord2f(1.0, 1.0);		glVertex2f(button->x + button->width, button->y);
		glTexCoord2f(1.0, 0.0);		glVertex2f(button->x + button->width, button->y + button->height);
		glTexCoord2f(0.0, 0.0);		glVertex2f(button->x, button->y + button->height);
	glEnd ();

	glDisable ( GL_SCISSOR_TEST );

	glPopAttrib ();

#else


	glPushAttrib ( GL_ALL_ATTRIB_BITS );

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
	glEnable ( GL_SCISSOR_TEST );

	glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );

	glEnable ( GL_TEXTURE_2D );
	
	UplinkAssert (button);
	
	Image *image = NULL;
	if ( clicked && button->image_clicked ) {
		UplinkAssert (button->image_clicked);
		image = button->image_clicked;
	}
	else if ( highlighted && button->image_highlighted ) {
		UplinkAssert (button->image_highlighted);
		image = button->image_highlighted;
	}
	else {
		UplinkAssert ( button->image_standard );
		image = button->image_standard;
	}

	UplinkAssert ( image );
	
	glBindTexture ( GL_TEXTURE_2D, 1 );
	glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, image->Width(), image->Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image->pixels );

	// Scale the image to fit the button size

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);		glVertex2i(button->x, button->y);
		glTexCoord2f(1.0f, 1.0f);		glVertex2i(button->x + button->width, button->y);
		glTexCoord2f(1.0f, 0.0f);		glVertex2i(button->x + button->width, button->y + button->height);
		glTexCoord2f(0.0f, 0.0f);		glVertex2i(button->x, button->y + button->height);
	glEnd ();

	glDisable ( GL_SCISSOR_TEST );

	glPopAttrib ();

#endif

}

void border_draw ( Button *button )
{

	glBegin ( GL_LINE_LOOP );

		glVertex2i ( button->x,						button->y );
		glVertex2i ( button->x + button->width - 1, button->y );
		glVertex2i ( button->x + button->width - 1, button->y + button->height - 1 );
		glVertex2i ( button->x,						button->y + button->height - 1 );

	glEnd ();

}

LList <char *> *wordwraptext ( const char *string, int linesize ) 
{

	/*

		Goes through the string and divides it up into several 
		smaller strings, taking into account newline characters,
		and the width of the text area.
		
		*/

	if ( !string ) return NULL;

	LList <char *> *llist = new LList <char *> ();

	if ( strlen ( string ) == 0 ) {

		char *emptystring = new char [ 2 ];
		*emptystring = '\0';
		llist->PutData ( emptystring );
		return llist;

	}

	// Calculate the maximum width in characters for 1 line
	float averagecharsize = ((float) GciTextWidth ( "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ" )) / 52.0f;	
	int linewidth = int (linesize / averagecharsize);
	if ( linewidth == 0 )
		linewidth = 1;
	

	// Create a copy of the string which we will use as our output strings
	// (All connected together but seperated by 0)
	// And add a newline on at the end (to make sure a newline will be found)

	size_t newstringsize = strlen(string) + 2;
	char *newstring = new char [ newstringsize + 1 ];
	UplinkSnprintf ( newstring, newstringsize, "%s\n", string );

	// Build a linked list of pointers into this new string
	// Each pointer representing another line

	char *currentpos = newstring;
	llist->PutData ( currentpos );

	while ( true ) {

		char *nextnewline = strchr ( currentpos, '\n' );
		if ( !nextnewline ) break;

		if ( nextnewline - currentpos > linewidth ) {

			// This line is too long and needs trimming down
			// Ignore the newline char we found - it will be dealt with
			// as part of the next line
			// Place a terminater in the space between the last 2 words
			currentpos += linewidth;
			char oldchar = *(currentpos - 1);
			*(currentpos - 1) = 0;
			char *space = strrchr ( currentpos - linewidth, ' ' );
			
			if ( space ) {
				*(currentpos - 1) = oldchar;
				currentpos = space + 1;
				*space = 0;
				llist->PutData ( currentpos );
			}
			else {
				// We cannot wrap this line - the word is longer than the max line width
				llist->PutData ( currentpos );
			}

		}
		else {

			// Found a newline char - replace with a terminator
			// then add this position in as the next line	
			// then continue from this position
			currentpos =  nextnewline + 1;
			*nextnewline = 0;
			llist->PutData ( currentpos );

		}

	}

	return llist;

}

void text_draw	( int x, int y, const char *string, int linesize )
{

	LList <char *> *wrappedtext = wordwraptext ( string, linesize );

	if ( wrappedtext ) {

		for ( int i = 0; i < wrappedtext->Size (); ++i ) {

			int xpos = x;
			int	ypos = y + i * 15;

			GciDrawText ( xpos, ypos, wrappedtext->GetData (i) );		

		}

		if ( wrappedtext->ValidIndex (0) && wrappedtext->GetData (0) )
			delete [] wrappedtext->GetData (0);				// Only delete first entry - since there is only one string really
		delete wrappedtext;

	}

}

void text_draw ( Button *button, bool highlighted, bool clicked ) 
{

	UplinkAssert (button);

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
	glEnable ( GL_SCISSOR_TEST );

	SetColour ( "DefaultText" );    

	// Print the text

	LList <char *> *wrappedtext = NULL;
	
	if ( !highlighted || !EclIsButtonEditable (button->name) ) {
		
		wrappedtext = wordwraptext ( button->caption, button->width );
		
	}
	else {

		// Highlighted - put a symbol at the end of every other second

		if ( time(NULL) >= (curserflash - 1) ) {

			size_t newcaptionsize = strlen(button->caption) + 2;
			char *newcaption = new char [newcaptionsize];
			UplinkSnprintf ( newcaption, newcaptionsize, "%s_", button->caption );

			wrappedtext = wordwraptext ( newcaption, button->width );

			delete [] newcaption;					// It was copied by wordwraptext

			if ( time(NULL) >= curserflash ) 
				curserflash = time(NULL) + 2;

		}
		else {

			wrappedtext = wordwraptext ( button->caption, button->width );

		}

		// Must dirty this button so the curser is redrawn

		EclDirtyButton ( button->name );

	}

	if ( wrappedtext ) {

		for ( int i = 0; i < wrappedtext->Size (); ++i ) {

			int xpos = button->x + 10;
			int	ypos = button->y + 10 + i * 15;

			GciDrawText ( xpos, ypos, wrappedtext->GetData (i) );		

		}

		if ( wrappedtext->ValidIndex (0) && wrappedtext->GetData (0) )
			delete [] wrappedtext->GetData (0);				// Only delete first entry - since there is only one string really
		delete wrappedtext;

	}

	glDisable ( GL_SCISSOR_TEST );

}

void textbutton_draw  ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert (button);

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
	glEnable ( GL_SCISSOR_TEST );

	// Black out the background
	clear_draw ( button->x, button->y, button->width, button->height );

	// Draw the text
	text_draw ( button, highlighted, clicked );

	// Draw a box around the text if highlighted
	if ( highlighted || clicked ) {

		SetColour ( "TextBorder" );
		border_draw ( button );	

	}

	glDisable ( GL_SCISSOR_TEST );

}

void textbutton_keypress ( Button *button, char key )
{

	UplinkAssert (button);

	if ( key == 8 ) {

		// Backspace key

		if ( strlen(button->caption) > 0 )
			*((button->caption) + strlen(button->caption) - 1) = 0;
	
	}
	else if ( key == 27 ) {

		// Escape key
		button->SetCaption ( "" );

	}
	else if ( key == 13 ) {

		// Enter key
		size_t newcaptionsize = strlen(button->caption) + 2;
		char *newcaption = new char [newcaptionsize];
		UplinkSnprintf ( newcaption, newcaptionsize, "%s\n", button->caption );
		
		button->SetCaption ( newcaption );

		delete [] newcaption;

	}
	else {

		size_t newcaptionsize = strlen(button->caption) + 2;
		char *newcaption = new char [newcaptionsize];
		UplinkSnprintf ( newcaption, newcaptionsize, "%s%c", button->caption, key );
		button->SetCaption ( newcaption );

		delete [] newcaption;

	}

	EclDirtyButton ( button->name );

}

void textbox_draw ( Button *button, bool highlighted, bool clicked )
{

	// A text button with a background

	UplinkAssert (  button );

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
	glEnable ( GL_SCISSOR_TEST );

	// Draw the background

	glBegin ( GL_QUADS );

		if		( clicked )		SetColour ( "ButtonClickedA" );
		else if ( highlighted ) SetColour ( "ButtonHighlightedA" );
		else					SetColour ( "ButtonNormalA" );
		glVertex2i ( button->x, button->y + button->height );

		if      ( clicked )		SetColour ( "ButtonClickedB" );
		else if ( highlighted ) SetColour ( "ButtonHighlightedB" );
		else					SetColour ( "ButtonNormalB" );
		glVertex2i ( button->x, button->y );

		if		( clicked )		SetColour ( "ButtonClickedA" );
		else if ( highlighted ) SetColour ( "ButtonHighlightedA" );
		else					SetColour ( "ButtonNormalA" );
		glVertex2i ( button->x + button->width, button->y );

		if		( clicked )		SetColour ( "ButtonClickedB" );
		else if ( highlighted ) SetColour ( "ButtonHighlightedB" );
		else					SetColour ( "ButtonNormalB" );
		glVertex2i ( button->x + button->width, button->y + button->height );

	glEnd ();

	// Draw the text
			
	text_draw ( button, highlighted, clicked );

	// Draw a box around the text if highlighted
	if ( highlighted || clicked ) 
		border_draw ( button );

	glDisable ( GL_SCISSOR_TEST );

}

void buttonborder_draw ( Button *button, bool highlighted, bool clicked )
{

	glBegin ( GL_QUADS );

		glColor4f ( 0.2f, 0.2f, 0.4f, ALPHA );			glVertex2i ( button->x, button->y );
		glColor4f ( 0.3f, 0.3f, 0.5f, ALPHA );			glVertex2i ( button->x + button->width, button->y );
		glColor4f ( 0.2f, 0.2f, 0.4f, ALPHA );			glVertex2i ( button->x + button->width, button->y + button->height );
		glColor4f ( 0.3f, 0.3f, 0.5f, ALPHA );			glVertex2i ( button->x, button->y + button->height );

	glEnd ();

}
/*
void superhighlight_draw ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert (  button );

	if ( time(NULL) >= (superhighlight_flash-1) ) {

		glBegin ( GL_QUADS );

			// Top
			glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		glVertex2i ( button->x, button->y - 5 );
			glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		glVertex2i ( button->x + button->width, button->y - 5 );
			glColor4f ( 1.0f, 1.0f, 0.5f, ALPHA );		glVertex2i ( button->x + button->width, button->y );
			glColor4f ( 1.0f, 1.0f, 0.5f, ALPHA );		glVertex2i ( button->x, button->y );

			// Right
			glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		glVertex2i ( button->x + button->width + 5, button->y );
			glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		glVertex2i ( button->x + button->width + 5, button->y + button->height );
			glColor4f ( 1.0f, 1.0f, 0.5f, ALPHA );		glVertex2i ( button->x + button->width, button->y + button->height );
			glColor4f ( 1.0f, 1.0f, 0.5f, ALPHA );		glVertex2i ( button->x + button->width, button->y );
	
			// Bottom
			glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		glVertex2i ( button->x, button->y + button->height + 5 );
			glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		glVertex2i ( button->x + button->width, button->y + button->height + 5 );
			glColor4f ( 1.0f, 1.0f, 0.5f, ALPHA );		glVertex2i ( button->x + button->width, button->y + button->height );
			glColor4f ( 1.0f, 1.0f, 0.5f, ALPHA );		glVertex2i ( button->x, button->y + button->height );

			// Left
			glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		glVertex2i ( button->x - 5, button->y );
			glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		glVertex2i ( button->x - 5, button->y + button->height );
			glColor4f ( 1.0f, 1.0f, 0.5f, ALPHA );		glVertex2i ( button->x, button->y + button->height );
			glColor4f ( 1.0f, 1.0f, 0.5f, ALPHA );		glVertex2i ( button->x, button->y );
	
			// Top left			
			glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		glVertex2i ( button->x - 5, button->y - 5 );
			glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		glVertex2i ( button->x - 5, button->y );
			glColor4f ( 1.0f, 1.0f, 0.5f, ALPHA );		glVertex2i ( button->x, button->y );
			glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		glVertex2i ( button->x, button->y - 5 );

			// Top right			
			glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		glVertex2i ( button->x + button->width + 5, button->y - 5 );			
			glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		glVertex2i ( button->x + button->width + 5, button->y );
			glColor4f ( 1.0f, 1.0f, 0.5f, ALPHA );		glVertex2i ( button->x + button->width, button->y );
			glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		glVertex2i ( button->x + button->width, button->y - 5 );

			// Bottom right					
			glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		glVertex2i ( button->x + button->width + 5, button->y + button->height + 5 );
			glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		glVertex2i ( button->x + button->width, button->y + button->height + 5 );
			glColor4f ( 1.0f, 1.0f, 0.5f, ALPHA );		glVertex2i ( button->x + button->width, button->y + button->height );
			glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		glVertex2i ( button->x + button->width + 5, button->y + button->height );

			// Bottom left								
			glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		glVertex2i ( button->x - 5, button->y + button->height + 5 );			
			glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		glVertex2i ( button->x, button->y + button->height + 5 );
			glColor4f ( 1.0f, 1.0f, 0.5f, ALPHA );		glVertex2i ( button->x, button->y + button->height );
			glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		glVertex2i ( button->x - 5, button->y + button->height );

		glEnd ();

		if ( time (NULL) >= superhighlight_flash )
			superhighlight_flash = time(NULL) + 2;

	}

}
*/
void superhighlight_draw ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert (  button );

	glBegin ( GL_QUADS );

		int border = 3;

        float timediff = (float) (superhighlight_flash - EclGetAccurateTime ()) / 1000.0f;
        float fraction;
        if ( timediff <= 1.0f ) 
            fraction = timediff;
        else if ( timediff <= 2.0f )
            fraction = 2.0f - timediff;
        else
            fraction = 0.0f;

		// Top
		glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		                    glVertex2i ( button->x + border, button->y );
		glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		                    glVertex2i ( button->x + button->width - border, button->y );
		glColor4f ( fraction, fraction, fraction/2.0f, ALPHA );		glVertex2i ( button->x + button->width - border, button->y + border );
		glColor4f ( fraction, fraction, fraction/2.0f, ALPHA );		glVertex2i ( button->x + border, button->y + border );

		// Right
		glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		                    glVertex2i ( button->x + button->width, button->y + border );
		glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		                    glVertex2i ( button->x + button->width, button->y + button->height - border );
		glColor4f ( fraction, fraction, fraction/2.0f, ALPHA );		glVertex2i ( button->x + button->width - border, button->y + button->height - border );
		glColor4f ( fraction, fraction, fraction/2.0f, ALPHA );		glVertex2i ( button->x + button->width - border, button->y + border );			

		// Bottom
		glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		                    glVertex2i ( button->x + border, button->y + button->height );
		glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		                    glVertex2i ( button->x + button->width - border, button->y + button->height );
		glColor4f ( fraction, fraction, fraction/2.0f, ALPHA );		glVertex2i ( button->x + button->width - border, button->y + button->height - border );
		glColor4f ( fraction, fraction, fraction/2.0f, ALPHA );		glVertex2i ( button->x + border, button->y + button->height - border );

		// Left
		glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		                    glVertex2i ( button->x, button->y + border );
		glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		                    glVertex2i ( button->x, button->y + button->height - border );
		glColor4f ( fraction, fraction, fraction/2.0f, ALPHA );		glVertex2i ( button->x + border, button->y + button->height - border );
		glColor4f ( fraction, fraction, fraction/2.0f, ALPHA );		glVertex2i ( button->x + border, button->y + border );

		// Top left			
		glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		                    glVertex2i ( button->x + border/2, button->y + border/2 );
		glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		                    glVertex2i ( button->x + border, button->y );
		glColor4f ( fraction, fraction, fraction/2.0f, ALPHA );		glVertex2i ( button->x + border, button->y + border );
		glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		                    glVertex2i ( button->x, button->y + border );

		// Top right			
		glColor4f ( fraction, fraction, fraction/2.0f, ALPHA );		glVertex2i ( button->x + button->width - border, button->y + border );			
		glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		                    glVertex2i ( button->x + button->width - border, button->y );
		glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		                    glVertex2i ( button->x + button->width - border/2, button->y + border/2 );
		glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		                    glVertex2i ( button->x + button->width, button->y + border );

		// Bottom right					
		glColor4f ( fraction, fraction, fraction/2.0f, ALPHA );		glVertex2i ( button->x + button->width - border, button->y + button->height - border );
		glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		                    glVertex2i ( button->x + button->width, button->y + button->height - border );
		glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		                    glVertex2i ( button->x + button->width - border/2, button->y + button->height - border/2 );
		glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		                    glVertex2i ( button->x + button->width - border, button->y + button->height );

		// Bottom left								
		glColor4f ( fraction, fraction, fraction/2.0f, ALPHA );		glVertex2i ( button->x + border, button->y + button->height - border );			
		glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		                    glVertex2i ( button->x + border, button->y + button->height );
		glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		                    glVertex2i ( button->x + border/2, button->y + button->height - border/2 );
		glColor4f ( 0.0f, 0.0f, 0.0f, ALPHA );		                    glVertex2i ( button->x, button->y + button->height - border );

	glEnd ();

	if ( EclGetAccurateTime () >= superhighlight_flash )
		superhighlight_flash = (int) ( EclGetAccurateTime () + 2000 );

}


void button_highlight ( Button *button )
{
	UplinkAssert ( button );
	
	EclHighlightButton ( button->name );
	tooltip_update ( button->tooltip );
	
	if ( strcmp ( button->name, currentbuttonname ) != 0 ) {

//		char filename [256];
//		UplinkSnprintf ( filename, sizeof ( filename ), "%ssounds/mouseover.wav", app->path );
//		SgPlaySound ( RsArchiveFileOpen ( filename ) );

		UplinkStrncpy ( currentbuttonname, button->name, sizeof ( currentbuttonname ) );

	}

	//GciPostRedisplay ();

}

void button_click ( Button *button )
{

	UplinkAssert ( button );

	EclClickButton ( button->name );

	SgPlaySound ( RsArchiveFileOpen ( "sounds/mouseclick.wav" ), "sounds/mouseclick.wav" );
	
	//GciPostRedisplay ();

}

void button_assignbitmap ( char *name, char *standard_f )
{

	Button *button = EclGetButton ( name );
	UplinkAssert ( button );

    char *fullfilename = app->GetOptions()->ThemeFilename( standard_f );
	Image *image = new Image ();
	image->LoadTIF ( RsArchiveFileOpen ( fullfilename ) );
	image->SetAlpha ( ALPHA );	
    delete [] fullfilename;
    
	button->SetStandardImage ( image );
	button->RegisterDrawFunction ( imagebutton_draw );

	EclDirtyButton ( name );

}

void button_assignbitmap  ( char *name,	Image *standard_i_ref )
{

	Button *button = EclGetButton ( name );
	UplinkAssert ( button );

	Image *image = NULL;
	if ( standard_i_ref )
		image = new Image ( *standard_i_ref );
    
	button->SetStandardImage ( image );
	button->RegisterDrawFunction ( imagebutton_draw );

	EclDirtyButton ( name );

}

void button_assignbitmaps ( char *name, char *standard_f, char *highlighted_f, char *clicked_f )
{

	Button *button = EclGetButton ( name );
	UplinkAssert ( button );

    char *fullfilename = app->GetOptions()->ThemeFilename( standard_f );
	Image *standard_i = new Image ();
	standard_i->LoadTIF ( RsArchiveFileOpen ( fullfilename ) );
	standard_i->SetAlpha ( ALPHA );	
    delete [] fullfilename;

    fullfilename = app->GetOptions()->ThemeFilename( highlighted_f );
	Image *highlighted_i = new Image ();
	highlighted_i->LoadTIF ( RsArchiveFileOpen ( fullfilename ) );
	highlighted_i->SetAlpha ( ALPHA );
    delete [] fullfilename;
    
    fullfilename = app->GetOptions()->ThemeFilename( clicked_f );
	Image *clicked_i = new Image ();
	clicked_i->LoadTIF ( RsArchiveFileOpen ( fullfilename ) );
	clicked_i->SetAlpha ( ALPHA );
    delete [] fullfilename;

	button->SetImages ( standard_i, highlighted_i, clicked_i );
	button->RegisterDrawFunction ( imagebutton_draw );
	
	EclDirtyButton ( name );

}

void button_assignbitmaps ( char *name, Image *standard_i_ref, Image *highlighted_i_ref, Image *clicked_i_ref )
{

	Button *button = EclGetButton ( name );
	UplinkAssert ( button );

	Image *standard_i = NULL;
	if ( standard_i_ref )
		standard_i = new Image ( *standard_i_ref );

	Image *highlighted_i = NULL;
	if ( highlighted_i_ref )
		highlighted_i = new Image ( *highlighted_i_ref );
    
	Image *clicked_i = NULL;
	if ( clicked_i_ref )
		clicked_i = new Image ( *clicked_i_ref );

	button->SetImages ( standard_i, highlighted_i, clicked_i );
	button->RegisterDrawFunction ( imagebutton_draw );
	
	EclDirtyButton ( name );

}

void button_assignbitmaps_blend ( char *name, char *standard_f, char *highlighted_f, char *clicked_f )
{

	Button *button = EclGetButton ( name );
	UplinkAssert ( button );

	float br = 0.0f, bg = 0.0f, bb = 0.0f;
	ColourOption *col;
	if ( app && 
	     app->GetOptions () && 
	     ( col = app->GetOptions ()->GetColour( "Background" ) ) ) {

		br = col->r;
		bg = col->g;
		bb = col->b;
	}

    char *fullfilename = app->GetOptions()->ThemeFilename( standard_f );
	Image *standard_i = new Image ();
	standard_i->LoadTIF ( RsArchiveFileOpen ( fullfilename ) );
	standard_i->SetAlpha ( 1.0f );
	standard_i->SetAlphaBorder ( 0.0f, br, bg, bb );
    delete [] fullfilename;

    fullfilename = app->GetOptions()->ThemeFilename( highlighted_f );
	Image *highlighted_i = new Image ();
	highlighted_i->LoadTIF ( RsArchiveFileOpen ( fullfilename ) );
	highlighted_i->SetAlpha ( 1.0f );
	highlighted_i->SetAlphaBorder ( 0.0f, br, bg, bb );
    delete [] fullfilename;
    
    fullfilename = app->GetOptions()->ThemeFilename( clicked_f );
	Image *clicked_i = new Image ();
	clicked_i->LoadTIF ( RsArchiveFileOpen ( fullfilename ) );
	clicked_i->SetAlpha ( 1.0f );
	clicked_i->SetAlphaBorder ( 0.0f, br, bg, bb );
    delete [] fullfilename;

	button->SetImages ( standard_i, highlighted_i, clicked_i );
	button->RegisterDrawFunction ( imagebutton_draw_blend );
	
	EclDirtyButton ( name );

}

void button_assignbitmap_blend ( char *name, char *standard_f )
{

	Button *button = EclGetButton ( name );
	UplinkAssert ( button );

	float br = 0.0f, bg = 0.0f, bb = 0.0f;
	ColourOption *col;
	if ( app && 
	     app->GetOptions () && 
	     ( col = app->GetOptions ()->GetColour( "Background" ) ) ) {

		br = col->r;
		bg = col->g;
		bb = col->b;
	}

    char *fullfilename = app->GetOptions()->ThemeFilename( standard_f );
	Image *image = new Image ();
	image->LoadTIF ( RsArchiveFileOpen ( fullfilename ) );
	image->SetAlpha ( 1.0f );
	image->SetAlphaBorder ( 0.0f, br, bg, bb );
    delete [] fullfilename;
    
	button->SetStandardImage ( image );
	button->RegisterDrawFunction ( imagebutton_draw_blend );

	EclDirtyButton ( name );

}

Image *get_assignbitmap ( char *filename )
{

    char *fullfilename = app->GetOptions()->ThemeFilename( filename );
	Image *standard_i = new Image ();
	standard_i->LoadTIF ( RsArchiveFileOpen ( fullfilename ) );
	standard_i->SetAlpha ( ALPHA );	
    delete [] fullfilename;

	return standard_i;

}

local void tooltip_callback ()
{

	// Tooltip has finished updating
	tooltipanimindex = -1;

}

void tooltip_update ( char *newtooltip )
{

	Button *tooltip = EclGetButton ( "tooltip" );

	if ( !tooltip ) {

		// The tooltip button has been removed / never created
		// So create it now
		int screenwidth = app->GetOptions ()->GetOptionValue ( "graphics_screenwidth" );
		int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
		EclRegisterButton ( 0, screenheight - 15, 500, 15, "", "tooltip" );
		EclRegisterButtonCallbacks ( "tooltip", textbutton_draw, NULL, NULL, NULL );
		EclButtonSendToBack ( "tooltip" );
		tooltip = EclGetButton ( "tooltip" );
		
	}

	if ( strcmp ( tooltip->tooltip, newtooltip ) != 0 ) {

		// Moved mouse over a different button
		tooltip->SetTooltip ( newtooltip );		// Store target here
		
		// Remove any currently running animation
		if ( tooltipanimindex != -1 ) {
			EclRemoveAnimation ( tooltipanimindex );
			tooltipanimindex = -1;
		}

		if ( strcmp ( newtooltip, " " ) == 0 ) {
			// No button under mouse
			tooltip->SetCaption ( " " );
			currentbuttonname[0] = '\0';
		}		
		else {
			tooltipanimindex = EclRegisterCaptionChange ( "tooltip", newtooltip, tooltip_callback );			
		}

	}

}

Button *getcurrentbutton ()
{

	if ( currentbuttonname[0] != '\0' ) 
		return EclGetButton ( currentbuttonname );

	else
		return NULL;

}

void create_stextbox ( int x, int y, int width, int height, char *caption, char *name )
{

	char name_box  [128];
	//char name_up   [128];
	//char name_down [128];
	//char name_bar  [128];

	UplinkSnprintf ( name_box, sizeof ( name_box ),  "%s box",    name );
	EclRegisterButton ( x, y, width - 16, height, caption, "", name_box );
	EclRegisterButtonCallbacks ( name_box, draw_stextbox, NULL, NULL, NULL );

    int numItems = 0;
    if ( strlen(caption) < 5 ) {
        numItems = 15;
    }
    else {
        LList <char *> *wrappedText = wordwraptext( caption, width );
		if ( wrappedText ) {
			numItems = wrappedText->Size() + 2;
			if ( wrappedText->ValidIndex (0) && wrappedText->GetData (0) )
				delete [] wrappedText->GetData(0);
			delete wrappedText;
		}
		if ( numItems < 4 ) numItems = 15;
    }

	int scrollLeft = x + width - 15;
	int windowSize = height / 15;
	ScrollBox::CreateScrollBox( name, scrollLeft, y, 15, height, numItems, windowSize, 0, stextbox_scroll );

}

void remove_stextbox ( char *name )
{

	char name_box  [128];
	UplinkSnprintf ( name_box, sizeof ( name_box ),  "%s box",    name );
	EclRemoveButton ( name_box );

	ScrollBox::RemoveScrollBox( name );

}

void draw_stextbox ( Button *button, bool highlighted, bool clicked )
{

	UplinkAssert (button);

	int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
	glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
	glEnable ( GL_SCISSOR_TEST );

	// Get the offset

	char name_base [128];
	sscanf ( button->name, "%s", name_base );
    ScrollBox *scrollBox = ScrollBox::GetScrollBox( name_base );
    if ( !scrollBox ) return;
	int offset = scrollBox->currentIndex;


	// Draw the button

	glBegin ( GL_QUADS );

		if		( clicked )		SetColour ( "ButtonClickedA" );
		else if ( highlighted ) SetColour ( "ButtonHighlightedA" );
		else					SetColour ( "ButtonNormalA" );
		glVertex2i ( button->x, button->y + button->height );

		if      ( clicked )		SetColour ( "ButtonClickedB" );
		else if ( highlighted ) SetColour ( "ButtonHighlightedB" );
		else					SetColour ( "ButtonNormalB" );
		glVertex2i ( button->x, button->y );

		if		( clicked )		SetColour ( "ButtonClickedA" );
		else if ( highlighted ) SetColour ( "ButtonHighlightedA" );
		else					SetColour ( "ButtonNormalA" );
		glVertex2i ( button->x + button->width, button->y );

		if		( clicked )		SetColour ( "ButtonClickedB" );
		else if ( highlighted ) SetColour ( "ButtonHighlightedB" );
		else					SetColour ( "ButtonNormalB" );
		glVertex2i ( button->x + button->width, button->y + button->height );

	glEnd ();

	// Draw a border if highlighted

	if ( highlighted || clicked )
		border_draw ( button );

	// Draw the text

	int maxnumlines = (button->height - 10 ) / 15;

	SetColour ( "DefaultText" );    

	LList <char *> *wrappedtext = wordwraptext ( button->caption, button->width );

	if ( wrappedtext ) {

		for ( int i = offset; i < wrappedtext->Size (); ++i ) {

			if ( i > maxnumlines + offset )
				break;

			int xpos = button->x + 10;
			int	ypos = button->y + 10 + (i-offset) * 15;

			GciDrawText ( xpos, ypos, wrappedtext->GetData (i), HELVETICA_10 );

		}

		//DeleteLListData ( wrappedtext );							// Only delete first entry - since there is only one string really
		if ( wrappedtext->ValidIndex (0) && wrappedtext->GetData (0) )
			delete [] wrappedtext->GetData (0);
		delete wrappedtext;

	}

	glDisable ( GL_SCISSOR_TEST );

}

void draw_scrollbox  ( Button *button, bool highlighted, bool clicked )
{

	glBegin ( GL_QUADS );

		if		( clicked )		SetColour ( "ButtonClickedA" );
		else if ( highlighted ) SetColour ( "ButtonHighlightedA" );
		else					SetColour ( "ButtonNormalA" );
		glVertex2i ( button->x, button->y + button->height );

		if      ( clicked )		SetColour ( "ButtonClickedB" );
		else if ( highlighted ) SetColour ( "ButtonHighlightedB" );
		else					SetColour ( "ButtonNormalB" );
		glVertex2i ( button->x, button->y );

		if		( clicked )		SetColour ( "ButtonClickedA" );
		else if ( highlighted ) SetColour ( "ButtonHighlightedA" );
		else					SetColour ( "ButtonNormalA" );
		glVertex2i ( button->x + button->width, button->y );

		if		( clicked )		SetColour ( "ButtonClickedB" );
		else if ( highlighted ) SetColour ( "ButtonHighlightedB" );
		else					SetColour ( "ButtonNormalB" );
		glVertex2i ( button->x + button->width, button->y + button->height );

	glEnd ();

}

void stextbox_scroll ( char *name, int newValue )
{

    char name_box [256];
	UplinkSnprintf ( name_box, sizeof ( name_box ),  "%s box", name );
    EclDirtyButton ( name_box );
  
}

void draw_msgboxbackground ( Button *button, bool highlighted, bool clicked )
{
/*
	glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable ( GL_BLEND );
	
	glColor4f ( 0.0f, 0.0f, 0.0f, 0.5f );

	glBegin ( GL_QUADS );
		glVertex2i ( button->x, button->y );
		glVertex2i ( button->x + button->width, button->y );
		glVertex2i ( button->x + button->width, button->y + button->height );
		glVertex2i ( button->x, button->y + button->height );
	glEnd ();
	
	glDisable ( GL_BLEND );
*/

}

void draw_msgboxbox ( Button *button, bool highlighted, bool clicked )
{

	glBegin ( GL_QUADS );
		SetColour ( "PanelBackgroundA" );		glVertex2i ( button->x, button->y + button->height );
		SetColour ( "PanelBackgroundB" );		glVertex2i ( button->x, button->y );
		SetColour ( "PanelBackgroundA" );		glVertex2i ( button->x + button->width, button->y );
		SetColour ( "PanelBackgroundB" );		glVertex2i ( button->x + button->width, button->y + button->height );
	glEnd ();

	SetColour ( "PanelBorder" );
	border_draw ( button );

}

void closeclick_msgbox ( Button *button )
{

	remove_msgbox ();

}

void create_msgbox ( char *title, char *message, void (*closeclick) (Button *) /* = NULL */ )
{

	if ( !isvisible_msgbox () ) {

		// Blank out the background

		int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
		int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");

		EclRegisterButton ( 0, 0, screenw, screenh, "", "", "msgbox_background" );
		EclRegisterButtonCallbacks ( "msgbox_background", draw_msgboxbackground, NULL, NULL, NULL );

		// Create the message box

		EclRegisterButton ( screenw / 2 - 100, screenh / 3, 200, 140, "", "", "msgbox_box" );
		EclRegisterButtonCallbacks ( "msgbox_box", draw_msgboxbox, NULL, NULL, NULL );

		// Title bar

		EclRegisterButton ( screenw / 2 - 100 + 2, screenh / 3 + 2, 195, 15, title, "Close the message box", "msgbox_title" );
		EclRegisterButtonCallback ( "msgbox_title", closeclick_msgbox );

		// Message box

		EclRegisterButton ( screenw / 2 - 100 + 2, screenh / 3 + 20, 195, 80, "", "", "msgbox_text" );
		EclRegisterButtonCallbacks ( "msgbox_text", text_draw, NULL, NULL, NULL );
		EclRegisterCaptionChange ( "msgbox_text", message );

		// OK button

		EclRegisterButton ( screenw / 2 - 50, screenh / 3 + 110, 100, 15, "Close", "Close the message box", "msgbox_close" );
		if ( closeclick )
			EclRegisterButtonCallback ( "msgbox_close", closeclick );
		else
			EclRegisterButtonCallback ( "msgbox_close", closeclick_msgbox );

	}

}

void create_yesnomsgbox ( char *title, char *message, void (*yesclick) (Button *) /* = NULL */, void (*noclick) (Button *) /* = NULL */ )
{

	if ( !isvisible_msgbox () ) {

		// Blank out the background

		int screenw = app->GetOptions ()->GetOptionValue ("graphics_screenwidth");
		int screenh = app->GetOptions ()->GetOptionValue ("graphics_screenheight");

		EclRegisterButton ( 0, 0, screenw, screenh, "", "", "msgbox_background" );
		EclRegisterButtonCallbacks ( "msgbox_background", draw_msgboxbackground, NULL, NULL, NULL );

		// Create the message box

		EclRegisterButton ( screenw / 2 - 150, screenh / 3, 300, 140, "", "", "msgbox_box" );
		EclRegisterButtonCallbacks ( "msgbox_box", draw_msgboxbox, NULL, NULL, NULL );

		// Title bar

		EclRegisterButton ( screenw / 2 - 150 + 2, screenh / 3 + 2, 295, 15, title, "Close the message box", "msgbox_title" );
		EclRegisterButtonCallback ( "msgbox_title", closeclick_msgbox );

		// Message box

		EclRegisterButton ( screenw / 2 - 150 + 2, screenh / 3 + 20, 295, 80, "", "", "msgbox_text" );
		EclRegisterButtonCallbacks ( "msgbox_text", text_draw, NULL, NULL, NULL );
		EclRegisterCaptionChange ( "msgbox_text", message );

		// Yes button

		EclRegisterButton ( screenw / 2 - 116, screenh / 3 + 110, 100, 15, "Yes", "Yes", "msgbox_yes" );
		if ( yesclick )
			EclRegisterButtonCallback ( "msgbox_yes", yesclick );
		else
			EclRegisterButtonCallback ( "msgbox_yes", closeclick_msgbox );

		// No button

		EclRegisterButton ( screenw / 2 + 16, screenh / 3 + 110, 100, 15, "No", "No", "msgbox_no" );
		if ( noclick )
			EclRegisterButtonCallback ( "msgbox_no", noclick );
		else
			EclRegisterButtonCallback ( "msgbox_no", closeclick_msgbox );

	}

}

void remove_msgbox ()
{

	if ( isvisible_msgbox () ) {

		EclRemoveButton ( "msgbox_background" );
		EclRemoveButton ( "msgbox_box" );
		EclRemoveButton ( "msgbox_title" );
		EclRemoveButton ( "msgbox_text" );
		if ( EclGetButton ( "msgbox_close" ) )
			EclRemoveButton ( "msgbox_close" );
		if ( EclGetButton ( "msgbox_yes" ) )
			EclRemoveButton ( "msgbox_yes" );
		if ( EclGetButton ( "msgbox_no" ) )
			EclRemoveButton ( "msgbox_no" );

	}

}

bool isvisible_msgbox ()
{

	return ( EclGetButton ( "msgbox_background" ) != NULL );

}

void SetWindowScaleFactor ( float scaleX, float scaleY )
{
    windowScaleX = scaleX;
    windowScaleY = scaleY;
}

int GetScaledXPosition ( int xPos )
{
    return (int) ( xPos * windowScaleX );
}

int GetScaledYPosition ( int yPos )
{
    return (int) ( yPos * windowScaleY );
}

