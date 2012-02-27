
#ifndef _included_iplookup_h
#define _included_iplookup_h

// ============================================================================

#include "interface/taskmanager/uplinktask.h"

#define IPLOOKUP_IDLE		0						// Waiting for input
#define IPLOOKUP_INPROGRESS 1						// Looking up IP
#define IPLOOKUP_FINISHED   2						// Showing result

// ============================================================================



class IPLookup : public UplinkTask
{

protected:

	int status;
	time_t timeout;					// Used to reset to idle after 5 secs

protected:
		
	static void DisplayDraw ( Button *button, bool highlighted, bool clicked );
	static void CloseClick ( Button *button );
	static void GoClick ( Button *button );

public:

	IPLookup ();
	~IPLookup ();
	
	void Initialise ();     
	void Tick ( int n );	

	void CreateInterface ();       
	void RemoveInterface ();
	void ShowInterface ();

	bool IsInterfaceVisible ();    

};



#endif

