#ifndef _included_lanspike_h
#define _included_lanspike_h

#include "eclipse.h"
#include "interface/taskmanager/uplinktask.h"


#define lanspike_UNUSED				0
#define lanspike_SPIKEING    		1
#define lanspike_FINISHED			3

class LanComputer;

// ============================================================================


class lanspike : public UplinkTask
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

	lanspike();
	~lanspike();

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
