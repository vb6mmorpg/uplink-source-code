
/*
  LAN Force software tool

	Forces a LAN lock open
    Wakes up the system admin

  */

#ifndef _included_lanforce_h
#define _included_lanforce_h

#include "eclipse.h"
#include "interface/taskmanager/uplinktask.h"


#define LANFORCE_UNUSED				0
#define LANFORCE_FORCING    		1
#define LANFORCE_FINISHED			3

class LanComputer;

// ============================================================================


class LanForce : public UplinkTask
{

protected:

	int status;
	int progress;
	int numticksrequired;

	LanComputer *comp;
	int systemIndex;

protected:

	static void BorderDraw    ( Button *button, bool highlighted, bool clicked );
	static void ProgressDraw  ( Button *button, bool highlighted, bool clicked );

	static void CloseClick    ( Button *button );
	static void BorderClick   ( Button *button );

public:

	LanForce();
	~LanForce();

	void SetTarget ( UplinkObject *uo, char *uos, int uoi );
	void MoveTo ( int x, int y, int time_ms );							// Origin top left		

	void Initialise ();     
	void Tick ( int n );    
	
	void CreateInterface ();       
	void RemoveInterface ();
	void ShowInterface ();

	bool IsInterfaceVisible ();     

};

#endif
