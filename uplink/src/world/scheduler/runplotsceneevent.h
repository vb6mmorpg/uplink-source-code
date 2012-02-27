
/*

  Run Plot Scene Event

	Runs the next scene in the main plot line

  */


#ifndef _included_runplotsceneevent_h
#define _included_runplotsceneevent_h


#include "world/scheduler/uplinkevent.h"



class RunPlotSceneEvent : public UplinkEvent
{

public:

	int act;                                                    // If act = -1, this is a DemoPlotSceneEvent
	int scene;

public:

	RunPlotSceneEvent ();
	~RunPlotSceneEvent ();

	void SetActAndScene ( int newact, int newscene );

	void Run ();

	char *GetShortString ();
	char *GetLongString ();

	
	// Common functions

	bool Load  ( FILE *file );
	void Save  ( FILE *file );
	void Print ();
	
	char *GetID ();		
	int   GetOBJECTID ();

};


#endif

