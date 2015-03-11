
#ifndef _included_animation_h
#define _included_animation_h


#include "button.h"

class Animation
{

public:

        Animation();
        ~Animation();
    
	char *buttonname;
	Button *button;
	int sourceX, sourceY;      // Where the button started
	int targetX, targetY;      // Where the button should end up
	float dX, dY;              // Amount to move for every time interval

	int sourceW, sourceH;      // Origional size of button
	int targetW, targetH;      // Target size of button
	float dW, dH;              // Amount to resize for every time interval

	char *targetC;			   // Target caption
	float dC;				   // How much of caption to add on every TI

	int time;                  // How long it should take
	int starttime;             // Time animation was started
	int finishtime;            // Time when animation should be finished
	int MOVETYPE;              // Numeric constant - eg MOVE_XFIRST

	void (*callback) ();       // Called when animation has finished

	void DebugPrint ();		   // Print out all state information

        

};



#endif

