
/*
   ============================================================================

			E C L I P S E

		  Interface Library
		By Christopher Delay

		Started 23rd May '99

   ============================================================================
*/

#ifndef _included_eclipse_h
#define _included_eclipse_h

/*
#ifdef DMALLOC
#include "dmalloc.h"
#endif
*/

/*
#ifdef _DEBUG
#include "slasher.h"
#endif
*/

#include "button.h"
#include "animation.h"

#include "tosser.h"

#define local static



// Initialisation functions ===================================================


void EclReset ( int width, int height );

void EclRegisterButton ( int x, int y, int width, int height,
						 char *caption, char *name );

void EclRegisterButton ( int x, int y, int width, int height,
						 char *caption, char *tooltip, char *name );

void EclRegisterImageButton ( int x, int y, int width, int height,
							  char *caption, char *tooltip, char *name );

void EclRemoveButton ( char *name );

void EclButtonBringToFront ( char *name );
void EclButtonSendToBack   ( char *name );

void EclRegisterDefaultButtonCallbacks ( void (*draw) (Button *, bool, bool),
										 void (*mouseup) (Button *),
										 void (*mousedown) (Button *),
										 void (*mousemove) (Button *) );

void EclRegisterButtonCallbacks ( char *name, 
								  void (*draw) (Button *, bool, bool), 
								  void (*mouseup) (Button *),
								  void (*mousedown) (Button *),
								  void (*mousemove) (Button *) );

void EclRegisterButtonCallback ( char *name, 
								 void (*mouseup) (Button *) );


void EclMakeButtonEditable ( char *name );
void EclMakeButtonUnEditable ( char *name );
bool EclIsButtonEditable ( char *name );
void EclHighlightNextEditableButton ();

// Graphical functions ========================================================

void EclDirtyButton		  ( char *name );					// Tells eclipse that this needs re-drawing
void EclDirtyRectangle	  ( int x, int y, int w, int h );	// Tells eclipse to blank this area
void EclDirtyClear        ();								// Clears all dirty buttons / rectangles
void EclClearRectangle    ( int x, int y, int w, int h );   // Tells eclipse to blank this area (now)

bool EclIsOccupied        ( int x, int y, int w, int h );	// True if there is a button here

void EclDrawAllButtons    ();
void EclDrawButton        ( char *name );
void EclDrawButton        ( int index );

void EclHighlightButton   ( char *name );
bool EclIsHighlighted     ( char *name );
void EclUnHighlightButton ();

void EclClickButton       ( char *name );
bool EclIsClicked         ( char *name );
void EclUnClickButton     ();

void EclSuperHighlight        ( char *name );			
bool EclIsSuperHighlighted    ( char *name );
void EclSuperUnHighlight      ( char *name );
void EclSuperUnHighlightAll	  ();
void EclUpdateSuperHighlights ( char *name );

void EclRegisterClearDrawFunction ( void (*draw) (int, int, int, int) );
void EclRegisterSuperHighlightFunction ( int borderwidth, void (*draw) (Button *, bool, bool) );


// Lookup functions ===========================================================


int  EclLookupIndex ( char *name );				// Can change
char *EclGetButtonAtCoord ( int x, int y );
char *EclGetHighlightedButton ();
Button *EclGetButton ( char *name );


// Animation functions ========================================================


#define MOVE_STRAIGHTLINE    1
#define MOVE_XFIRST          2
#define MOVE_YFIRST          3

void EclEnableAnimations ();				// Default
void EclDisableAnimations ();

#define ECL_FASTER_ANIMATION_SPEED 2.0

void EclEnableFasterAnimations ( double speed = ECL_FASTER_ANIMATION_SPEED );
void EclDisableFasterAnimations ();			// Default


int  EclRegisterMovement  ( char *bname, int targetX, int targetY, int time_ms,
							void (*callback) () = NULL );
int  EclRegisterMovement  ( char *bname, int targetX, int targetY, int time_ms, int MOVETYPE,
						    void (*callback) () = NULL );
int  EclRegisterResize    ( char *bname, int targetW, int targetH, int time_ms,
						    void (*callback) () = NULL );
int  EclRegisterAnimation ( char *bname, int targetX, int targetY, 
							int targetW, int targetH, int time_ms,
							void (*callback) () = NULL );

int  EclRegisterCaptionChange ( char *bname, char *targetC, int time_ms,
							    void (*callback) () = NULL );
int  EclRegisterCaptionChange ( char *bname, char *targetC,				// Determines time_ms from 							   
							    void (*callback) () = NULL );			// length of caption

int  EclRegisterAnimation ( char *bname, int targetX, int targetY, int MOVETYPE, 
							int targetW, int targetH, char *targetC, int time_ms,
							void (*callback) () = NULL );

int EclIsAnimationActive ( char *bname );					// Returns ID number of anim, or -1
int EclIsCaptionChangeActive ( char *bname );
int EclIsNoCaptionChangeActive ( char *bname );

void EclRemoveAnimation ( int idnumber );


int EclAnimationsRunning();
void EclUpdateAllAnimations ();

double EclGetAccurateTime ();				// Returns the time in millisecond accuracy

// ============================================================================


void EclDebugPrint ();					// Prints out all state information


#endif

