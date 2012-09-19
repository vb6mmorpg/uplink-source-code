
#ifndef _included_compressor_h
#define _included_compressor_h


// ============================================================================

#include "interface/taskmanager/uplinktask.h"

class DataBank;

#define COMPRESSOR_OFF		     0
#define COMPRESSOR_INPROGRESS    1
#define COMPRESSOR_FINISHED      2

// ============================================================================


class Compressor : public UplinkTask
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

	Compressor ();
	~Compressor ();
	
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
