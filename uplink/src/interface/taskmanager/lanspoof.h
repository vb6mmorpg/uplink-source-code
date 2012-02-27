
/*

  Lan Spoof

	Runs on a LAN viewer
	Targets a system on the LAN
	Spoofs the LAN into believing you are the system you are spoofing

  */

// ============================================================================

#ifndef _included_lanspoof_h
#define _included_lanspoof_h

#include "eclipse.h"
#include "interface/taskmanager/uplinktask.h"

#define LANSPOOF_UNUSED			0
#define LANSPOOF_SPOOFING		1
#define LANSPOOF_FAILED			2
#define LANSPOOF_SPOOFED		3

class LanComputer;

// ============================================================================



class LanSpoof : public UplinkTask
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

	LanSpoof();
	~LanSpoof();

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

