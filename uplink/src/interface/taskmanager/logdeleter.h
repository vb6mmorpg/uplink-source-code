
#ifndef _included_logdeleter_h
#define _included_logdeleter_h


// ============================================================================

#include "interface/taskmanager/uplinktask.h"

class LogBank;

#define LOGDELETER_OFF		    0
#define LOGDELETER_INPROGRESS   1
#define LOGDELETER_REPLACING    2
#define LOGDELETER_FINISHED     3

// ============================================================================


class LogDeleter : public UplinkTask
{

protected:

	LogBank *source;	
	int sourceindex;

	int currentreplaceindex;			// Used by later versions to keep track of searches
	int currentreplaceprogress;			// "" ""

	int status;						// 0 = off, 1 = in progress, 2 = fnished
	int numticksrequired;
	int progress;					// 0.0 - numticksrequired
	
protected:

	static void BorderDraw    ( Button *button, bool highlighted, bool clicked );
	static void ProgressDraw  ( Button *button, bool highlighted, bool clicked );

	static void CloseClick    ( Button *button );
	static void BorderClick   ( Button *button );

public:

	LogDeleter ();
	~LogDeleter ();

	void SetTarget ( UplinkObject *uo, char *uos, int uoi );
	void MoveTo ( int x, int y, int time_ms );				// Centred on top-right of title image

	void Initialise ();     
	void Tick ( int n );    
	
	void CreateInterface ();       
	void RemoveInterface ();
	void ShowInterface ();

	bool IsInterfaceVisible ();     

};



#endif
