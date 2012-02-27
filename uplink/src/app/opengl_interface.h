
#ifndef _included_openglinterface_h
#define _included_openglinterface_h

#include "eclipse.h"
#include "gucci.h"

/*

	Generic OpenGL functions for rendering the interface
	(defined by Eclipse)	

  */


void clear_draw       ( int x, int y, int w, int h );

void button_draw      ( Button *button, bool highlighted, bool clicked );
void button_highlight ( Button *button );
void button_click     ( Button *button );

void imagebutton_draw ( Button *button, bool highlighted, bool clicked );
void imagebutton_draw ( Button *button, bool highlighted, bool clicked, Image *standard_i_ref, Image *highlighted_i_ref, Image *clicked_i_ref );
void imagebutton_drawtextured ( Button *button, bool highlighted, bool clicked );
void imagebutton_draw_blend ( Button *button, bool highlighted, bool clicked );

void border_draw ( Button *button );
void textbutton_draw  ( Button *button, bool highlighted, bool clicked );
void textbutton_keypress ( Button *button, char key );

LList <char *> *wordwraptext ( const char *string, int linesize );

void text_draw		  ( int x, int y, const char *string, int linesize );
void text_draw		  ( Button *button, bool highlighted, bool clicked );
void textbox_draw     ( Button *button, bool highlighted, bool clicked );

void buttonborder_draw ( Button *button, bool highlighted, bool clicked );

void superhighlight_draw ( Button *button, bool highlighted, bool clicked );

void button_assignbitmap  ( char *name,	char *standard_f );
void button_assignbitmap  ( char *name,	Image *standard_i_ref );
void button_assignbitmaps ( char *name, char *standard_f, char *highlighted_f, char *clicked_f );
void button_assignbitmaps ( char *name, Image *standard_i_ref, Image *highlighted_i_ref, Image *clicked_i_ref );

void button_assignbitmap_blend ( char *name, char *standard_f );
void button_assignbitmaps_blend ( char *name, char *standard_f, char *highlighted_f, char *clicked_f );

Image *get_assignbitmap ( char *filename );


void tooltip_update ( char *newtooltip );

Button *getcurrentbutton ();					// Returns button under mouse or NULL

//
// This set of functions can create and manage a scrollable text box
//

void create_stextbox ( int x, int y, int width, int height, 
                        char *caption, char *name );
void remove_stextbox ( char *name );

void draw_stextbox	 ( Button *button, bool highlighted, bool clicked );
void draw_scrollbox  ( Button *button, bool highlighted, bool clicked );
void stextbox_scroll ( char *name, int newValue );

//
// Functions for modal message boxes
//

void create_msgbox      ( char *title, char *message, void (*closeclick) (Button *) = NULL );
void create_yesnomsgbox ( char *title, char *message, void (*yesclick) (Button *) = NULL, void (*noclick) (Button *) = NULL );
void remove_msgbox      ();
bool isvisible_msgbox   ();


//
// Functions to handle screen resize / button repositioning

void SetWindowScaleFactor ( float scaleX, float scaleY );
int GetScaledXPosition ( int xPos );
int GetScaledYPosition ( int yPos );

#define SX(x)       GetScaledXPosition(x)
#define SY(y)       GetScaledYPosition(y)

#endif
