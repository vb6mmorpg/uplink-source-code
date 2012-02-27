
/*
  LAN Scan software tool

	Run on a LAN viewer
	Does a quick scan for all available systems
	Will show up Terminals, Mail servers etc

  */

#ifndef _included_lanscan_h
#define _included_lanscan_h

#include "world/vlocation.h"
#include "interface/taskmanager/uplinktask.h"

class LanComputer;

#define LANSCAN_UNUSED			0
#define LANSCAN_SCANNING		1
#define LANSCAN_FINISHED		2


// ============================================================================


class LanScan : public UplinkTask
{

protected:

	int status;
	int progress;
	int numticksrequired;

	char ip [SIZE_VLOCATION_IP];

protected:

	static void ProgressDraw ( Button *button, bool highlighted, bool clicked );
	static void BorderDraw   ( Button *button, bool highlighted, bool clicked );

	static void GoClick		 ( Button *button );
	static void CloseClick   ( Button *button );

public:

	LanScan();
	~LanScan();

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
