
#ifndef _included_decompiler_h
#define _included_decompiler_h


// ============================================================================

#include "interface/taskmanager/uplinktask.h"

class DataBank;

#define DECOMPILER_OFF		     0
#define DECOMPILER_INPROGRESS    1
#define DECOMPILER_FINISHED      2

// ============================================================================


class Decompiler : public UplinkTask
{

protected:

	DataBank *source;	
	int sourceindex;

	int status;						
    bool remotefile;

	int numticksrequired;
	int progress;					// 0.0 - numticksrequired

protected:

	static void BorderDraw    ( Button *button, bool highlighted, bool clicked );
	static void ProgressDraw  ( Button *button, bool highlighted, bool clicked );

	static void CloseClick    ( Button *button );
	static void BorderClick   ( Button *button );

public:

	Decompiler ();
	~Decompiler ();
	
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
