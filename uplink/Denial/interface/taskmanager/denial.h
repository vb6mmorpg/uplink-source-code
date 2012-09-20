
#ifndef _included_denial_h
#define _included_denial_h

// ============================================================================

#include "interface/taskmanager/uplinktask.h"

#define DENIAL_IDLE			0						// Waiting for input
#define DENIAL_INPROGRESS	1						// DDoSing
#define DENIAL_FINISHED		2						// Showing result

// ============================================================================



class Denial : public UplinkTask
{

protected:

	int status;
	int timeout;					// Used to reset to idle after 5 secs

protected:
		
	static void DisplayDraw ( Button *button, bool highlighted, bool clicked );
	static void MainBoxDraw ( Button *button, bool highlighted, bool clicked );
	static void CloseClick ( Button *button );
	static void TitleClick ( Button *button );
	static void GoClick ( Button *button );

public:

	Denial ();
	~Denial ();
	
	char myCap[64];

	void Initialise ();     
	void Tick ( int n );	
    void MoveTo ( int x, int y, int time_ms );

	void CreateInterface ();       
	void RemoveInterface ();
	void ShowInterface ();

	bool IsInterfaceVisible ();    

};



#endif