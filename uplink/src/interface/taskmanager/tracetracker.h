// TraceTracker.h: interface for the TraceTracker class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _included_tracetracker_h
#define _included_tracetracker_h

#include "interface/taskmanager/uplinktask.h"


class TraceTracker : public UplinkTask   
{

protected:

    int x, y;

    int traceprogress;                              // Trace progress at time of last estimate
    int traceestimate;                              // Time of estimated trace
    int nextbeep;                                   // Time of next beep
    int beepdelay;                                  // The time between beeps
	int lastupdate;                                 // Time since last update

    bool audioon;

protected:

    static void TraceDraw ( Button *button, bool highlighted, bool clicked );
    static void CloseClick ( Button *button );
	
    static void AudioDraw ( Button *button, bool highlighted, bool clicked );
    static void AudioClick ( Button *button );

public:

	TraceTracker();

	virtual ~TraceTracker();

	void Initialise ();     // Called at creation time, neccisary
	void Tick ( int n );           // n ticks occur

	void MoveTo ( int x, int y, int time_ms );			

	void CreateInterface ();       
	void RemoveInterface ();
	void ShowInterface ();

	bool IsInterfaceVisible ();     

};

#endif 
