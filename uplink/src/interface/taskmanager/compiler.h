
#ifndef _included_compiler_h
#define _included_compiler_h

// ============================================================================

#include "interface/taskmanager/uplinktask.h"

#define COMPILER_IDLE		     0						// Waiting for input
#define COMPILER_COMPILING_SPEC  1						// Looking up IP
#define COMPILER_COMPILING_CORE  2						// Looking up IP
#define COMPILER_COMPILING_DATA  3						// Looking up IP
#define COMPILER_LINKING         4						// Showing result
#define COMPILER_FINISHED        5						// Showing result
#define COMPILER_FAILED          6						// Showing result

// ============================================================================



class Compiler : public UplinkTask
{

protected:

	int status;
	time_t timeout;					// Used to reset to idle after 5 secs
	int dataversion;
	int datasize;
	int datatype;
	int progress;
	int numticksrequired;
	char filename[64];

protected:
		
	static void DisplayDraw ( Button *button, bool highlighted, bool clicked );
	static void CloseClick ( Button *button );
	static void GoClick ( Button *button );

public:

	Compiler ();
	~Compiler ();
	
	void Initialise ();     
	void Tick ( int n );	

	void CreateInterface ();       
	void RemoveInterface ();
	void ShowInterface ();

	void SetFilename ( char *newfilename );

	bool IsInterfaceVisible ();    

};



#endif

