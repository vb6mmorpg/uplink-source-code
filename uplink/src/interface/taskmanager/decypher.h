
#ifndef _included_decypher_h
#define _included_decypher_h


// ============================================================================

#include "interface/taskmanager/uplinktask.h"

#define DECYPHER_OFF		    0
#define DECYPHER_INPROGRESS     1
#define DECYPHER_FINISHED       2

// ============================================================================


class Decypher : public UplinkTask
{

protected:

	int status;						

	int numticksrequired;
	int progress;					// 0.0 - numticksrequired

protected:

	static void BorderDraw    ( Button *button, bool highlighted, bool clicked );
	static void ProgressDraw  ( Button *button, bool highlighted, bool clicked );

	static void CloseClick    ( Button *button );
	static void BorderClick   ( Button *button );

public:

	Decypher ();
	~Decypher ();
	
	void SetTarget ( UplinkObject *uo, char *uos, int uoi );
	void MoveTo ( int x, int y, int time_ms );				// Centred on top-right of title image

	void Initialise ();     // Called at creation time, neccisary
	void Tick ( int n );           // n ticks occur
	
	void CreateInterface ();       
	void RemoveInterface ();
	void ShowInterface ();	

	bool IsInterfaceVisible ();     

};



#endif
