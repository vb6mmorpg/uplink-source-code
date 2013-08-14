
#ifndef _included_interface_h
#define _included_interface_h

#include "taskwrapper.h"

//#ifdef _DEBUG
//#include "slasher.h"
//#endif


// Call before anything else
void Svb_initialise_interface ();

// Draws a button fairly normally, text is left justified
void Svb_button_draw ( Button *button, bool highlighted, bool clicked );

// Draws a button with text only - no background
void Svb_textbutton_draw ( Button *button, bool highlighted, bool clicked );

// Draws a coloured column, colour is determined by length
void Svb_column_draw ( Button *button, bool highlighted, bool clicked );

// Create remove a coloured column and control buttons
void SvbCreateColumn ( int baseX, int BaseY, TaskWrapper *tw, int taskindex );
void SvbRemoveColumn ( int taskindex );


#endif
