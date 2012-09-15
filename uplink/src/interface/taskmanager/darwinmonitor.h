
#ifndef _included_darwinmonitor_h
#define _included_darwinmonitor_h


#include "vanbakel.h"

#include "interface/taskmanager/uplinktask.h"



class DarwinMonitor : public UplinkTask  
{


protected:

	static void DarwinMonitorDraw ( Button *button, bool highlighted, bool clicked );	
	
    static void TitleClick ( Button *button );
    static void CloseClick ( Button *button );

	static void LeftClick ( Button *button );
    static void RightClick ( Button *button );

	static void SquadClick ( Button *button );
    static void EngineerClick ( Button *button );

	static int selectedIndex;
	static int pageStart;

public:

	DarwinMonitor();
	virtual ~DarwinMonitor();

	void SetTarget ( UplinkObject *uo, char *uos, int uoi );				

	void Initialise ();     
	void Tick ( int n );    

	void MoveTo ( int x, int y, int time_ms );			
    
	void CreateInterface ();       
	void RemoveInterface ();
	void ShowInterface ();

	bool IsInterfaceVisible ();     

};

#endif