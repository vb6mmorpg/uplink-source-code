
#ifndef _included_probe_h
#define _included_probe_h

// ============================================================================

#include "interface/taskmanager/uplinktask.h"

#define PROBE_IDLE			0						// Waiting for input
#define PROBE_INPROGRESS	1						// Looking up IP
#define PROBE_FINISHED		2						// Sending result

// ============================================================================



class IPProbe : public UplinkTask
{

protected:

	int status;
	int timeout;					// Used to reset to idle after 5 secs

protected:
		
	static void DisplayDraw ( Button *button, bool highlighted, bool clicked );
	static void CloseClick ( Button *button );
	static void GoClick ( Button *button );

public:

	IPProbe ();
	~IPProbe ();
	
	void Initialise ();     
	void Tick ( int n );	

	void CreateInterface ();       
	void RemoveInterface ();
	void ShowInterface ();

	bool IsInterfaceVisible ();    

};



#endif

