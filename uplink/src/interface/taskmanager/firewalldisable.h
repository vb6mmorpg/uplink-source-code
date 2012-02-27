
/*

  Firewall Disable software

	Part of the Task Manager sub system

  */


#ifndef _included_firewalldisable_h
#define _included_firewalldisable_h

#include "eclipse.h"

#include "world/vlocation.h"

#include "interface/taskmanager/uplinktask.h"



class FirewallDisable : public UplinkTask
{

protected:

	static void ProgressDraw ( Button *button, bool highlighted, bool clicked );
	static void BorderDraw   ( Button *button, bool highlighted, bool clicked );

	static void GoClick		 ( Button *button );
	static void CloseClick   ( Button *button );

protected:

	char ip [SIZE_VLOCATION_IP];				// Target IP

	int progress;								// 0 = static, 1 = analysing, 2 = disabling, 3 = unable to proceed, 4 = done
	
	int ticksdone;								//
	int ticksrequired;							// Used to syncronise with task manager

	time_t timersync;								// Used to syncronise events to real time

public:

	FirewallDisable ();
	~FirewallDisable ();

	void SetTarget ( UplinkObject *uo, char *uos, int uoi );
	void MoveTo ( int x, int y, int time_ms );			

	void Initialise ();     
	void Tick ( int n );    
	
	void CreateInterface ();       
	void RemoveInterface ();
	void ShowInterface ();

	bool IsInterfaceVisible ();     

};

#endif

