
#define FUDGE 10

#include <string.h>
#include <time.h>
#include <stdio.h>

#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h> 
#include <unistd.h> 
#endif

#include "eclipse.h"
#include "tosser.h"


#include <vector>
using namespace std;

#include <assert.h>

#include "mmgr.h"


// Protected members ==========================================================

local LList <Button *> buttons;
local char *currenthighlight = 0;
local char *currentclick = 0;

local DArray <Animation *> anims;
local bool animsenabled = true;
local bool animsfasterenabled = false;
local double animsfasterspeed = ECL_FASTER_ANIMATION_SPEED;

local BTree <int> superhighlightedbuttons;
local int superhighlight_borderwidth;

class dirtyrect
{

public:

    dirtyrect () {  x = y = width = height = 0;    }
    dirtyrect ( int newx, int newy, int newwidth, int newheight ) :
                x(newx), y(newy), width(newwidth), height(newheight) {}
    ~dirtyrect () {    }

	int x;
	int y;
	int width;
	int height;

};

local vector <dirtyrect> dirtyrectangles;
local LList <char *> editablebuttons;						// List of editable buttons

// Default button callbacks

local void (*default_draw)      (Button *, bool, bool) = NULL;
local void (*default_mouseup)   (Button *) = NULL;
local void (*default_mousedown) (Button *) = NULL;
local void (*default_mousemove) (Button *) = NULL;

local void (*clear_draw)        (int, int, int, int) = NULL;

local void (*superhighlight_draw) (Button *, bool, bool) = NULL;

// ============================================================================


void EclReset ( int width, int height )
{


	// (They are in a linked list - so be careful when removing them
	// As the list will shrink during the process )
/*
    // TODO trim this out
	int numbuttons = buttons.Size ();

    for ( int i = numbuttons - 1; i >= 0; --i ) 
		if ( buttons.ValidIndex (i) ) 
			EclRemoveButton ( buttons [i]->name );
  */

    delete [] currenthighlight;
    delete [] currentclick;
        
	currenthighlight = NULL;
	currentclick = NULL;
    
    while ( buttons.ValidIndex ( 0 ) ) {
        Button *b = buttons.GetData (0);
        //buttons.RemoveData (0);
        //delete b;
		EclRemoveButton ( b->name );
    }

    // Also delete the animations
    // TODO : Doing this stops the animation of the caption on the log in screen
    // After you load a game, then exit back to the login screen
/*
    for ( int j = 0; j < anims.Size (); ++j )
        if ( anims.ValidIndex (j) )
            if ( anims.GetData (j) ) 
                anims.GetData (j)->DebugPrint ();
*/

    

    // Also delete the edidable buttons

    while ( editablebuttons.GetData (0) ) {
        char *b = editablebuttons.GetData (0);
        editablebuttons.RemoveData (0);
        delete [] b;
    }

	buttons.Empty ();
//    anims.Empty ();
	editablebuttons.Empty ();
        
	superhighlight_borderwidth = 0;
    
    // TODO sort this out sometime
    EclDirtyClear();
    EclDirtyRectangle ( 0, 0, width, height );
    
}

void EclRegisterButton ( int x, int y, int width, int height, 
  			 			 char *caption, char *name )
{

	if ( EclGetButton ( name ) ) {

#ifdef _DEBUG
		printf ( "ECL WARNING : EclRegisterButton called, Button name not unique : %s\n", name );
#endif

		return;

	}

	Button *button = new Button ( x, y, width, height, caption, name );
	if ( button ) {
		buttons.PutDataAtStart ( button );

		EclRegisterButtonCallbacks ( name, default_draw, default_mouseup, 
									 default_mousedown, default_mousemove );

		EclDirtyButton ( name );
	}

}

void EclRegisterButton ( int x, int y, int width, int height,
						 char *caption, char *tooltip, char *name )
{

	EclRegisterButton ( x, y, width, height, caption, name );
	EclGetButton ( name )->SetTooltip ( tooltip );
	
}

void EclRegisterImageButton ( int x, int y, int width, int height,
							  char *caption, char *tooltip, char *name )
{

#ifdef _DEBUG
	if ( EclGetButton ( name ) )
		printf ( "ECL WARNING : EclRegisterImageButton called, Button name not unique : %s\n", name );
#endif

	Button *button = new Button ( x, y, width, height, caption, name );
	if ( button ) {
		buttons.PutDataAtStart ( button );

		EclRegisterButtonCallbacks ( name, default_draw, default_mouseup, 
									 default_mousedown, default_mousemove );

		EclDirtyButton ( name );
	}
}

void EclRemoveButton ( char *name )
{

	int index = EclLookupIndex ( name );
	
	if ( buttons.ValidIndex (index) ) {

		Button *button = buttons [index];

		if ( EclIsHighlighted ( name ) ) {
                    delete [] currenthighlight;
			currenthighlight = NULL;
                }

		if ( EclIsClicked ( name ) ) {
                    delete [] currentclick;
			currentclick = NULL;
                }

		if ( EclIsSuperHighlighted ( name ) )
			EclSuperUnHighlight ( name );			

		if ( EclIsButtonEditable ( name ) )
			EclMakeButtonUnEditable ( name );

		for( int i = anims.Size() - 1; i >= 0; i-- ){
			if ( anims.ValidIndex (i) ) {
				Animation *anim = anims [i];

				if ( strcmp ( name, anim->buttonname ) == 0 )
					EclRemoveAnimation(i);
			}
		}

		EclDirtyRectangle ( button->x, button->y, button->width, button->height );

		buttons.RemoveData ( index );
		if ( button ) delete button;

	}
	else {

#ifdef _DEBUG
		printf ( "ECL WARNING : EclRemoveButton called, button does not exist : %s\n", name );
#endif

	}

}

void EclButtonBringToFront ( char *name )
{

	int index = EclLookupIndex ( name );

	if ( buttons.ValidIndex ( index ) ) {

		Button *button = buttons [index];
		buttons.RemoveData ( index );
		buttons.PutDataAtStart ( button );
		EclDirtyButton ( name );

	}
	else {

#ifdef _DEBUG
		printf ( "ECL WARNING : EclButtonBringToFront called, button does not exist : %s\n", name );
#endif

	}

}

void EclButtonSendToBack   ( char *name )
{

	int index = EclLookupIndex ( name );

	if ( buttons.ValidIndex ( index ) ) {

		Button *button = buttons [index];
		buttons.RemoveData ( index );
		buttons.PutDataAtEnd ( button );
		EclDirtyButton ( name );

	}
	else {

#ifdef _DEBUG
		printf ( "ECL WARNING : EclButtonSendToBack called, button does not exist : %s\n", name );
#endif

	}


}

void EclRegisterDefaultButtonCallbacks ( void (*draw) (Button *, bool, bool),
										 void (*mouseup) (Button *),
										 void (*mousedown) (Button *),
										 void (*mousemove) (Button *) )
{

	default_draw = draw;
	default_mouseup = mouseup;
	default_mousedown = mousedown;
	default_mousemove = mousemove;

}

void EclRegisterButtonCallbacks ( char *name, void (*draw) (Button *, bool, bool), 
								  void (*mouseup) (Button *), 
								  void (*mousedown) (Button *),
								  void (*mousemove) (Button *) )
{

	int index = EclLookupIndex ( name );

	if ( buttons.ValidIndex ( index ) ) {

		buttons [index]->RegisterDrawFunction ( draw );
		buttons [index]->RegisterMouseUpFunction ( mouseup );
		buttons [index]->RegisterMouseDownFunction ( mousedown );
		buttons [index]->RegisterMouseMoveFunction ( mousemove );

	}
	else {

#ifdef _DEBUG
		printf ( "ECL WARNING : EclRegisterButtonCallbacks called, button does not exist : %s\n", name );
#endif

	}

}

void EclRegisterButtonCallback ( char *name, 
								 void (*mouseup) (Button *) )
{

	int index = EclLookupIndex ( name );

	if ( buttons.ValidIndex ( index ) )
		buttons [index]->RegisterMouseUpFunction ( mouseup );

#ifdef _DEBUG
	else 
		printf ( "ECL WARNING : EclRegisterButtonCallback called, button does not exist : %s\n", name );
#endif

}

void EclMakeButtonEditable ( char *name )
{

	Button *button = EclGetButton ( name );
	
	if ( button ) {
		char *bname = new char [strlen(name)+1];
		strcpy ( bname, name );
		editablebuttons.PutDataAtEnd ( bname );
	}

#ifdef DEBUG

	else
		printf ( "ECL WARNING : EclMakeButtonEditable called, invalid button name\n" );

#endif

}

void EclMakeButtonUnEditable ( char *name )
{

	for ( int i = 0; i < editablebuttons.Size (); ++i ) {
		if ( editablebuttons.ValidIndex (i) ) {
			if ( strcmp ( editablebuttons.GetData (i), name ) == 0 ) {				
				delete [] ( editablebuttons.GetData (i) );
				editablebuttons.RemoveData (i);
				break;
			}
		}
	}
				
}

bool EclIsButtonEditable ( char *name )
{

	if ( !EclGetButton (name) ) 
		return false;

	for ( int i = 0; i < editablebuttons.Size (); ++i )
		if ( editablebuttons.ValidIndex (i) )
			if ( strcmp ( editablebuttons.GetData (i), name ) == 0 )
				return true;

	return false;

}

void EclHighlightNextEditableButton ()
{

	//
	// Look to see if an existing button is highlighted
	//

	if ( currenthighlight ) {

		for ( int i = 0; i < editablebuttons.Size (); ++i ) {
			if ( editablebuttons.ValidIndex (i) ) {
				if ( strcmp ( editablebuttons.GetData (i), currenthighlight ) == 0 ) {

					// This is the currently highlighted button 
					// move onto the next

					char *nextbutton = ( i < editablebuttons.Size () - 1 ) ?
											 editablebuttons.GetData (i+1) :
											 editablebuttons.GetData (0);

					EclHighlightButton ( nextbutton );
					return;
					
				}
			}
		}

	}

	// 
	// Nothing editable is currentlyhighlighted, so move to the first
	//

	if ( editablebuttons.ValidIndex (0) )
		EclHighlightButton ( editablebuttons.GetData (0) );
	
}

int  EclLookupIndex ( char *name )
{

	if ( name )
		for ( int i = 0; i < buttons.Size (); ++i )
			if ( buttons.ValidIndex ( i ) )		
				if ( strcmp ( buttons [i]->name, name ) == 0 )
					return i;

	return -1;

}

local bool EclRectangleOverlap ( int x1, int y1, int w1, int h1,
								 int x2, int y2, int w2, int h2 )
{

	int maxleft = x1 > x2 ? x1 : x2;			
	int maxtop  = y1 > y2 ? y1 : y2;
	
	int minright  = x1 + w1 < x2 + w2 ? x1 + w1 : x2 + w2;
	int minbottom = y1 + h1 < y2 + h2 ? y1 + h1 : y2 + h2;

	if ( maxtop < minbottom && maxleft < minright ) 
		return true;

	else
		return false;

}

bool EclIsOccupied ( int x, int y, int w, int h )
{

	for ( int i = 0; i < buttons.Size (); ++i ) {

		if ( buttons.ValidIndex (i) ) {
			
			Button *b = buttons [i];
			if ( EclRectangleOverlap ( b->x, b->y, b->width, b->height,
									   x, y, w, h ) )
				return true;

		}

	}

	return false;

}

void EclDirtyButton ( char *name )
{

/********************************************************************
 *** Removed now that the screen is totally repainted every frame ***
 ********************************************************************

	Button *button = EclGetButton ( name );

	if ( button ) {

		if ( !button->dirty ) {
				
			button->Dirty ();

			// Dirty any buttons that are above this one (ie have a lower index)
			// This occurs recursivly - as multiple overlapping buttons need refreshing

			int thisindex = EclLookupIndex (name);

			for ( int i = thisindex - 1; i >= 0; --i )
				if ( buttons.ValidIndex (i) ) {
					Button *b = buttons [i];
					if ( EclRectangleOverlap ( b->x, b->y, b->width, b->height,
											   button->x, button->y, button->width, button->height ) )
						EclDirtyButton ( b->name );
				}

		}

	}
#ifdef _DEBUG
	else 
		printf ( "ECL WARNING : EclDirtyButton called, button does not exist : %s\n", name );
#endif
*/

}

void EclDirtyRectangle ( int x, int y, int w, int h )
{
/*
	dirtyrect *d = new dirtyrect ();
	d->x = x;
	d->y = y;
	d->width = w;
	d->height = h;

	dirtyrectangles.PutData ( d );
*/

/********************************************************************
 *** Removed now that the screen is totally repainted every frame ***
 ********************************************************************

    dirtyrectangles.push_back ( dirtyrect(x, y, w, h) );

	// Dirty any buttons in this area

	for ( int i = 0; i < buttons.Size (); ++i ) {
		if ( buttons.ValidIndex (i) ) {
			
			Button *b = buttons.GetData (i);

			if ( EclRectangleOverlap ( x, y, w, h, b->x, b->y, b->width, b->height ) ) 
				EclDirtyButton ( b->name );
			
		}
	}

*/

}

void EclDrawAllButtons ()
{

/*

	// Blank all dirty rectangles

	if ( clear_draw ) 
		for ( unsigned int i = 0; i < dirtyrectangles.size (); ++i ) {
			dirtyrect &d = dirtyrectangles [i];
            
            clear_draw ( d.x, d.y, d.width, d.height );
		}
	

	// Blank all dirty buttons
////// *
	if ( clear_draw )
		for ( int i = buttons.Size () - 1; i >= 0; --i )
			if ( buttons.ValidIndex ( i ) ) 
				if ( buttons [i]->dirty ) 			
					clear_draw ( buttons [i]->x, buttons [i]->y, buttons [i]->width, buttons [i]->height );
* //////
		
	// Draw all buttons

	for ( int i = buttons.Size () - 1; i >= 0; --i )
		if ( buttons.ValidIndex ( i ) ) 
			if ( buttons [i]->dirty ) 			
				if ( buttons [i]->x >= 0 && buttons [i]->y >= 0 )
					EclDrawButton ( i );				

//        EclDirtyClear();

*/

	// Draw all buttons

	for ( int i = buttons.Size () - 1; i >= 0; --i )
		if ( buttons.ValidIndex ( i ) ) {
			Button *b = buttons [i];
			if ( b->x >= 0 && b->y >= 0 ) {
				EclDrawButton ( i );
				b->dirty = false;
			}
		}

    dirtyrectangles.clear ();

}

void EclClearRectangle ( int x, int y, int w, int h )
{

	if ( clear_draw ) 
        clear_draw ( x, y, w, h );

}

void EclDirtyClear  ()
{
/*
    // TODO trim this out
	if ( dirtyrectangles.size () > 0 )
		for ( int i = dirtyrectangles.size () - 1; i >= 0; --i ) 
            if ( dirtyrectangles[i] )
			    delete dirtyrectangles [i];
*/

/*
    while ( dirtyrectangles.GetData (0) ) {
        dirtyrect *d = dirtyrectangles.GetData (0);
        dirtyrectangles.RemoveData (0);
        delete d;
    }
*/

//	dirtyrectangles.Empty ();

    dirtyrectangles.clear ();

	for ( int ib = buttons.Size () - 1; ib >= 0; --ib )
		if ( buttons.ValidIndex ( ib ) ) 
			buttons [ib]->dirty = false;	

}

void EclDrawButton ( char *name )
{

	int index = EclLookupIndex ( name );

	if ( buttons.ValidIndex ( index ) )
		EclDrawButton ( index );

}

void EclDrawButton ( int index )
{

	if ( buttons.ValidIndex ( index ) )
		if ( EclIsClicked ( buttons [index]->name ) )
			buttons [index]->Draw ( false, true );
		else if ( EclIsHighlighted ( buttons [index]->name ) )
			buttons [index]->Draw ( true, false );
		else
			buttons [index]->Draw ( false, false );

}

void EclHighlightButton ( char *name )
{

//	if ( !currenthighlight || strcmp ( currenthighlight, name ) != 0 ) {
	if ( !EclIsHighlighted ( name ) ) {
	
		EclUnHighlightButton ();
                delete [] currenthighlight;
		currenthighlight = new char [strlen (name) + 1];
		strcpy ( currenthighlight, name );
		EclDirtyButton ( name );

	}

}

void EclClickButton ( char *name )
{

//	if ( !currentclick || strcmp ( currentclick, name ) != 0 ) {
	if ( !EclIsClicked ( name ) ) {
	
		EclUnClickButton ();
                delete [] currentclick;
		currentclick = new char [strlen (name) + 1];
		strcpy ( currentclick, name );
		EclDirtyButton ( name );

	}

}

bool EclIsHighlighted ( char *name )
{

	return ( currenthighlight && strcmp ( currenthighlight, name ) == 0 );

}

bool EclIsClicked ( char *name )
{

	return ( currentclick && strcmp ( currentclick, name ) == 0 );

}

void EclUnHighlightButton ()
{

	Button *button = EclGetButton ( currenthighlight );
	if ( button ) EclDirtyButton ( button->name );

        delete [] currenthighlight;
	currenthighlight = NULL;
		
}
			
void EclUnClickButton ()
{

	Button *button = EclGetButton ( currentclick );
	if ( button ) EclDirtyButton ( button->name );

    delete [] currentclick;
	currentclick = NULL;

}

void EclSuperHighlight ( char *name )
{

	Button *button = EclGetButton ( name );

	if ( !superhighlightedbuttons.LookupTree ( name ) && button ) {

		// Needs to be added
		superhighlightedbuttons.PutData ( name, 0 );

		// Create the button itself
		
		int x = button->x - superhighlight_borderwidth;
		int y = button->y - superhighlight_borderwidth;
		int width = button->width + superhighlight_borderwidth * 2;
		int height = button->height + superhighlight_borderwidth * 2;

		char superhighlightname [128];
		sprintf ( superhighlightname, "Ecl_superhighlight %s", name );

		EclRegisterButton ( x, y, width, height, "", "", superhighlightname );
		EclRegisterButtonCallbacks ( superhighlightname, superhighlight_draw, NULL, NULL, NULL );

		EclButtonSendToBack ( superhighlightname );

	}

}

void EclSuperUnHighlight ( char *name )
{

	if ( superhighlightedbuttons.LookupTree ( name ) ) {

		superhighlightedbuttons.RemoveData ( name );

		// Remove the button itself
		char superhighlightname [128];
		sprintf ( superhighlightname, "Ecl_superhighlight %s", name );
		EclRemoveButton ( superhighlightname );

	}
#ifdef _DEBUG
	else 
		printf ( "ECL WARNING : SuperUnHighlight called, button not found : %s\n", name );
#endif

}

void EclSuperUnHighlightAll	()
{

	DArray <char *> *highlights = superhighlightedbuttons.ConvertIndexToDArray ();

	for ( int i = 0; i < highlights->Size (); ++i )
		if ( highlights->ValidIndex (i) )
			EclSuperUnHighlight ( highlights->GetData (i) );

}

bool EclIsSuperHighlighted ( char *name )
{

	return ( superhighlightedbuttons.LookupTree (name) != NULL );

}

void EclRegisterClearDrawFunction ( void (*draw) (int, int, int, int) )
{

	clear_draw = draw;

}

void EclRegisterSuperHighlightFunction ( int borderwidth, void (*draw) (Button *, bool, bool) )
{

	superhighlight_borderwidth = borderwidth;
	superhighlight_draw = draw;

}

void EclUpdateSuperHighlights ( char *name )
{

	Button *sourcebutton = EclGetButton ( name );

	if ( !sourcebutton ) {
		
		EclSuperUnHighlight ( name );
		return;

	}

	if ( EclIsSuperHighlighted ( name ) ) {

		char superhighlight_name [128];
		sprintf ( superhighlight_name, "Ecl_superhighlight %s", name );

		Button *highlightbutton = EclGetButton ( superhighlight_name );
		
		if ( highlightbutton ) {

			// Update the position of this highlight button 
			// to keep up with the actual button

			if ( highlightbutton->x != sourcebutton->x ||
				highlightbutton->y != sourcebutton->y ) {

				EclDirtyRectangle ( highlightbutton->x, highlightbutton->y, 
									highlightbutton->width, highlightbutton->height );

				highlightbutton->x = sourcebutton->x - superhighlight_borderwidth;
				highlightbutton->y = sourcebutton->y - superhighlight_borderwidth;
			
			}
			
			EclDirtyButton ( superhighlight_name );

		}
#ifdef _DEBUG
		else 
			printf ( "ECL WARNING : UpdateSuperHighlights called, button is SuperHighlighted : %s\nBut no SuperHighlight button exists\n", name );
#endif

	}
#ifdef _DEBUG
	else
		printf ( "ECL WARNING : UpdateSuperHighlights called, button is not SuperHighlighted : %s\n", name );
#endif

}

char *EclGetButtonAtCoord ( int x, int y )
{

	for ( int i = 0; i < buttons.Size (); ++i ) {
		if ( buttons.ValidIndex (i) ) {
			Button *b = buttons [i];
			if ( x >= b->x && x <= b->x + b->width &&
				 y >= b->y && y <= b->y + b->height )				 
				return b->name;
		}
	}

	return NULL;

}

char *EclGetHighlightedButton ()
{

	return currenthighlight;

}

Button *EclGetButton ( char *name )
{

	int index = EclLookupIndex ( name );

	if ( buttons.ValidIndex ( index ) )
		return buttons [index];
	else
		return NULL;

}

void EclEnableAnimations ()
{

	animsenabled = true;

}

void EclDisableAnimations ()
{

	animsenabled = false;

}

void EclEnableFasterAnimations ( double speed )
{

	animsfasterenabled = true;
	animsfasterspeed = speed;

}

void EclDisableFasterAnimations ()
{

	animsfasterenabled = false;

}

int  EclRegisterMovement ( char *bname, int targetX, int targetY, int time_ms,
						   void (*callback) ())
{

	return EclRegisterMovement ( bname, targetX, targetY, time_ms, MOVE_STRAIGHTLINE, callback );

}

int  EclRegisterMovement  ( char *bname, int targetX, int targetY, int time_ms, int MOVETYPE,
						    void (*callback) () )
{

	Button *button = EclGetButton ( bname );
	if ( button ) 
		return EclRegisterAnimation ( bname, targetX, targetY, MOVETYPE, button->width, button->height, NULL, time_ms, callback );

	else
		return -1;

}

int  EclRegisterResize ( char *bname, int targetW, int targetH, int time_ms, void (*callback) () )
{

	Button *button = EclGetButton ( bname );
	if ( button ) 
		return EclRegisterAnimation ( bname, button->x, button->y, targetW, targetH, time_ms, callback );

	else
		return -1;
}

int  EclRegisterAnimation ( char *bname, int targetX, int targetY, 
							int targetW, int targetH, int time_ms,
							void (*callback) () )
{

	return EclRegisterAnimation ( bname, targetX, targetY, MOVE_STRAIGHTLINE, targetW, targetH, NULL, time_ms, callback );

}

int  EclRegisterCaptionChange ( char *bname, char *targetC, int time_ms,
							    void (*callback) ())
{

	Button *button = EclGetButton ( bname );
	if ( button ) 
		return EclRegisterAnimation ( bname, button->x, button->y, MOVE_STRAIGHTLINE, button->width, button->height, targetC, time_ms, callback );
	
	else {
#ifdef _DEBUG	
		printf ( "ECL WARNING : Tried to register a caption change, could not find button %s\n", bname );
#endif
		return -1;
	}

}

int  EclRegisterCaptionChange ( char *bname, char *targetC,				
							    void (*callback) () )
{

	int time = (int)(strlen(targetC) * 50);
	return EclRegisterCaptionChange ( bname, targetC, time, callback );

}

int EclAnimationsRunning()
{
	return anims.Size() > 0;
}


int  EclRegisterAnimation ( char *bname, int targetX, int targetY, int MOVETYPE, 
							int targetW, int targetH, char *targetC, int time_ms,
							void (*callback) () )
{

	Button *button = EclGetButton ( bname );

	if ( button ) {

		Animation *anim = new Animation;
		anim->buttonname = new char [strlen(bname)+1];
		strcpy ( anim->buttonname, bname );
		anim->button = button;
		anim->MOVETYPE = MOVETYPE;

		anim->sourceX = button->x;
		anim->sourceY = button->y;
		anim->targetX = targetX;
		anim->targetY = targetY;
		anim->sourceW = button->width;
		anim->sourceH = button->height;
		anim->targetW = targetW;
		anim->targetH = targetH;

		anim->callback = callback;

		anim->time = time_ms;
		if ( animsfasterenabled )
			anim->time = (int) ( anim->time / animsfasterspeed );

		anim->starttime = (int)EclGetAccurateTime ();              
		anim->finishtime = (int)EclGetAccurateTime () + anim->time;
	
		if ( anim->finishtime - anim->starttime == 0 ) anim->finishtime = anim->starttime + 1;

		if ( anim->targetX != anim->sourceX || anim->targetY != anim->sourceY ) {

			// Button will move
			anim->dX = float ( anim->targetX - anim->sourceX ) / float ( anim->finishtime - anim->starttime );
			anim->dY = float ( anim->targetY - anim->sourceY ) / float ( anim->finishtime - anim->starttime );

		}
		else {

			// Button will not move
			anim->dX = 0.0;
			anim->dY = 0.0;

		}

		if ( anim->targetW != anim->sourceW || anim->targetH != anim->sourceH ) {

			// Button will resize
			anim->dW = float ( anim->targetW - anim->sourceW ) / float ( anim->finishtime - anim->starttime );
			anim->dH = float ( anim->targetH - anim->sourceH ) / float ( anim->finishtime - anim->starttime );

		}
		else {

			// Button will not resize
			anim->dW = 0.0;
			anim->dH = 0.0;

		}

		if ( targetC ) {

			// Button caption will change
			// Remove ALL existing caption change animations

			int ca = EclIsCaptionChangeActive ( bname );
			while ( ca != -1 ) {
				EclRemoveAnimation ( ca );
				ca = EclIsCaptionChangeActive ( bname );
			}
			
			if ( anim->targetC )
				delete [] anim->targetC;
			anim->targetC = new char [strlen(targetC)+1];
			strcpy ( anim->targetC, targetC );			
			anim->dC = float ( strlen ( targetC ) ) / float ( anim->finishtime - anim->starttime );
			
		}
		else {

			// Button caption will not change
			anim->targetC = NULL;
			anim->dC = 0.0;

		}

		return anims.PutData ( anim );

	}
	else {

		// Could not find button
		return -1;

	}

}

int EclIsAnimationActive ( char *bname )
{

	for ( int i = 0; i < anims.Size (); ++i ) {

		if ( anims.ValidIndex (i) ) {

			Animation *anim = anims [i];
			assert ( anim );

			if ( strcmp ( bname, anim->buttonname ) == 0 )
				return i;

		}

	}

	return -1;

}

int EclIsCaptionChangeActive ( char *bname )
{

	for ( int i = 0; i < anims.Size (); ++i ) {

		if ( anims.ValidIndex (i) ) {

			Animation *anim = anims [i];
			assert ( anim );

			if ( strcmp ( bname, anim->buttonname ) == 0 &&
				 anim->targetC != NULL )
				return i;

		}

	}

	return -1;

}

int EclIsNoCaptionChangeActive ( char *bname )
{

	for ( int i = 0; i < anims.Size (); ++i ) {

		if ( anims.ValidIndex (i) ) {

			Animation *anim = anims [i];
			assert ( anim );

			if ( strcmp ( bname, anim->buttonname ) == 0 &&
				 anim->targetC == NULL )
				return i;

		}

	}

	return -1;

}

void EclRemoveAnimation ( int idnumber )
{

    if ( anims.ValidIndex ( idnumber ) ) {
        Animation *anim = anims.GetData (idnumber);
        if ( anim ) delete anim;
		anims.RemoveData ( idnumber );
    }

}

void EclUpdateAllAnimations ()
{

	int i;


	//
	// Update all super-highlighted buttons
	//

	DArray <char *> *superhighlighted = superhighlightedbuttons.ConvertIndexToDArray ();

	for ( i = 0; i < superhighlighted->Size (); ++i )
		if ( superhighlighted->ValidIndex (i) )
			EclUpdateSuperHighlights ( superhighlighted->GetData (i) );

	delete superhighlighted;


	//
	// Update all animations
	//

	for ( i = 0; i < anims.Size (); ++i ) {

		if ( anims.ValidIndex (i) ) {

			Animation *anim = anims [i];
			assert ( anim );
			
			// Button may have been deleted - attempt to re-aquire it here
			anim->button = EclGetButton ( anim->buttonname );
			
			if ( !anim->button ) {

				// This button has been destroyed - remove the anim
#ifdef _DEBUG
				printf ( "ECL WARNING : Animation defined on invalid button : %s\n", anim->buttonname );
#endif

                EclRemoveAnimation ( i );

			}
			else {
							
				EclDirtyButton ( anim->buttonname );

				if ( EclGetAccurateTime () >= anim->finishtime || !animsenabled ) {

					// Animation has finished - set to final position / size / caption
					// OR Animations are not enabled - set to final position as well

					if ( anim->dX || anim->dY ) {

						EclDirtyRectangle ( anim->button->x, anim->button->y, anim->button->width, anim->button->height );		

						anim->button->x = anim->targetX;
						anim->button->y = anim->targetY;						
						
					}

					if ( anim->dW || anim->dH ) {

						EclDirtyRectangle ( anim->button->x, anim->button->y, anim->button->width, anim->button->height );		

						anim->button->width = anim->targetW;
						anim->button->height = anim->targetH;						

					}

					if ( anim->dC ) {

						anim->button->SetCaption ( anim->targetC );

					}

					// Update any SuperHighlights that exist on this button

					if ( EclIsSuperHighlighted ( anim->buttonname ) )
						EclUpdateSuperHighlights ( anim->buttonname );
						
					// Call the callback if neccesary
					if ( anim->callback ) anim->callback ();

					// Remove the animation from the list
                    EclRemoveAnimation ( i );

				}
				else {

					if ( anim->MOVETYPE == MOVE_STRAIGHTLINE ) {

						// Button moves at a constant rate in a straight line to target

						if ( anim->dX || anim->dY ) {
							
							int dX = (int)(anim->sourceX + ( EclGetAccurateTime () - anim->starttime ) * anim->dX) - anim->button->x;
							int dY = (int)(anim->sourceY + ( EclGetAccurateTime () - anim->starttime ) * anim->dY) - anim->button->y;

							//EclDirtyRectangle ( anim->button->x, anim->button->y, dX, anim->button->height );		
							EclDirtyRectangle ( anim->button->x, anim->button->y, anim->button->width, anim->button->height );		

							// Buttons location can change
							// Calculate location of button
							anim->button->x = anim->sourceX + (int)(( EclGetAccurateTime () - anim->starttime ) * anim->dX);
							anim->button->y = anim->sourceY + (int)(( EclGetAccurateTime () - anim->starttime ) * anim->dY);							
// 							double now = EclGetAccurateTime();
// 							double f = ( now - anim->starttime ) / (anim->finishtime - anim->starttime);
// 							anim->button->x = anim->sourceX + (anim->targetX - anim->sourceX) * f;
// 							anim->button->y = anim->sourceY + (anim->targetY - anim->sourceY) * f;
						}
					
					}
					else if ( anim->MOVETYPE == MOVE_XFIRST ) {

						// Button moves in X direction, then in Y direction

						if ( anim->dX || anim->dY ) {

							EclDirtyRectangle ( anim->button->x, anim->button->y, anim->button->width, anim->button->height );		
						
							float ratio = (anim->dX / (anim->dX + anim->dY));
							int halftime = anim->starttime + (int)(ratio * (anim->finishtime - anim->starttime));

							if ( EclGetAccurateTime () < halftime ) {
								float firsthalf = (float) ( EclGetAccurateTime () - anim->starttime ) / (float) ( halftime - anim->starttime );
								anim->button->x = anim->sourceX + (int)((anim->targetX - anim->sourceX) * firsthalf);
							}
							else {
								float secondhalf = (float) ( EclGetAccurateTime () - halftime ) / (float) ( anim->finishtime - halftime );
								anim->button->x = anim->targetX;
								anim->button->y = anim->sourceY + (int)((anim->targetY - anim->sourceY) * secondhalf);
							}

						}

					}
					else if ( anim->MOVETYPE == MOVE_YFIRST ) {

						// Button moves in Y direction, then in X direction

						if ( anim->dX || anim->dY ) {
						
							EclDirtyRectangle ( anim->button->x, anim->button->y, anim->button->width, anim->button->height );		

							float ratio = (anim->dY / (anim->dX + anim->dY));
							int halftime = anim->starttime + (int)(ratio * (anim->finishtime - anim->starttime));

							if ( EclGetAccurateTime () < halftime ) {
								float firsthalf = (float) ( EclGetAccurateTime () - anim->starttime ) / (float) ( halftime - anim->starttime );
								anim->button->y = anim->sourceY + (int)((anim->targetY - anim->sourceY) * firsthalf);
							}
							else {
								float secondhalf = (float) ( EclGetAccurateTime () - halftime ) / (float) ( anim->finishtime - halftime );
								anim->button->y = anim->targetY;
								anim->button->x = anim->sourceX + (int)((anim->targetX - anim->sourceX) * secondhalf);
							}

						}					

					}

					if ( anim->dW || anim->dH ) {

						// Buttons size can change
						// Calculate new size
						EclDirtyRectangle ( anim->button->x, anim->button->y, anim->button->width, anim->button->height );		

						anim->button->width = anim->sourceW + (int)(( EclGetAccurateTime () - anim->starttime ) * anim->dW);
						anim->button->height = anim->sourceH + (int)(( EclGetAccurateTime () - anim->starttime ) * anim->dH);

					}

					if ( anim->dC ) {

						// Buttons caption can change
						size_t showlength = (size_t)( ( (float)EclGetAccurateTime () - anim->starttime ) * anim->dC );
						char *caption = new char [strlen(anim->targetC)+1];
						strcpy ( caption, anim->targetC );
						if ( showlength < strlen(anim->targetC)+1 )
                                                    caption [showlength] = 0;
						anim->button->SetCaption ( caption );
						delete [] caption;

					}

					// Update any SuperHighlights that exist on this button

					if ( EclIsSuperHighlighted ( anim->buttonname ) )
						EclUpdateSuperHighlights ( anim->buttonname );

				}

			}

		}

	}

}

double EclGetAccurateTime ()
{

#ifdef WIN32

	static bool supportsHighResTime = false;
	static bool initted = false;

	static LARGE_INTEGER frequency;
	static LARGE_INTEGER firstPerformanceCount;
	static DWORD firstTickCount = 0;

	if ( !initted ) {
		initted = true;

		if ( QueryPerformanceFrequency ( &frequency ) && 
		     QueryPerformanceCounter ( &firstPerformanceCount ) ) {

			supportsHighResTime = true;
		}
		else {
			firstTickCount = GetTickCount();
		}
	}

	if ( supportsHighResTime ) {
		LARGE_INTEGER performanceCount;
		QueryPerformanceCounter ( &performanceCount );

		return 1000.0 * ( performanceCount.QuadPart - firstPerformanceCount.QuadPart ) / frequency.QuadPart;
	}
	else {
		return GetTickCount() - firstTickCount;
	}

	/*
	//return 1000 * ( (float) clock () / (float) CLOCKS_PER_SEC );

	LARGE_INTEGER lpFrequency;
	LARGE_INTEGER lpPerformanceCount;
	if ( QueryPerformanceFrequency ( &lpFrequency ) != 0 && QueryPerformanceCounter ( &lpPerformanceCount ) != 0 ) {

		return (double) ( ( lpPerformanceCount.QuadPart * 1000 ) / lpFrequency.QuadPart );

	}

	return GetTickCount ();
	*/

#else

	// Linux version

	static bool initted = false; 
	static struct timeval startTime; 
	struct timeval tv; 
  
	if (!initted) { 
		initted = true; 
		gettimeofday(&startTime, NULL); 
		startTime.tv_usec = 0;
		return 0; 
	} 

	gettimeofday(&tv, NULL); 
  
	long diff_usec = tv.tv_usec - startTime.tv_usec; 
// 	if (diff_usec < 0) { 
// 		diff_usec += 1000000; 
// 		tv.tv_sec --; 
// 	} 
	long diff_sec = tv.tv_sec - startTime.tv_sec; 

	return 1000.0 * diff_sec + diff_usec / 1000.0; 

#endif

}

void EclDebugPrint ()
{

	printf ( "============== E C L I P S E ===============================\n" );	

	printf ( "BUTTONS : \n" );
	for ( int bi = 0; bi < buttons.Size (); ++bi )
		buttons.GetData (bi)->DebugPrint ();		

	printf ( "Current Highlight = %s\n", currenthighlight );
	printf ( "Current Click     = %s\n", currentclick );

	printf ( "ANIMATIONS : \n" );
	if ( animsenabled )  printf ( "Animations are ENABLED\n" );
	else				 printf ( "Animations are DISABLED\n" );

	if ( animsfasterenabled )  printf ( "Faster Animations are ENABLED\n" );
	else                       printf ( "Faster Animations are DISABLED\n" );

	for ( int ai = 0; ai < anims.Size (); ++ai ) 
		if ( anims.ValidIndex (ai) )
			anims.GetData (ai)->DebugPrint ();

	printf ( "EDITABLE BUTTONS : \n" );
	
	for ( int eb = 0; eb < editablebuttons.Size (); ++eb )
		if ( editablebuttons.ValidIndex (eb) )
			printf ( "%s\n", editablebuttons.GetData (eb) );

	printf ( "============== E N D  O F  E C L I P S E ===================\n" );

}

