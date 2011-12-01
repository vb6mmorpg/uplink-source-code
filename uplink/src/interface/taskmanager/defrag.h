
/*

	Defrag software

	Part of the Task Manager sub system

  */


#ifndef _included_defrag_h
#define _included_defrag_h

#include "eclipse.h"

#include "world/vlocation.h"
#include "world/computer/databank.h"

#include "interface/taskmanager/uplinktask.h"



class Defrag : public UplinkTask
{

protected:

	static void ProgressDraw ( Button *button, bool highlighted, bool clicked );
	static void BorderDraw   ( Button *button, bool highlighted, bool clicked );

	static void GoClick		 ( Button *button );
	static void CloseClick   ( Button *button );

	void HandleNewMemorySlot ( int index );

	void StopDefrag  ( );

protected:

	int progress;								// 0 = static, 1 = defragging, 2 = done
	int ticksdone;								//
	int ticksrequired;							// Used to syncronise with task manager
	time_t timersync;								// Used to syncronise events to real time

	int currentMemorySlot;
	int firstFreeMemorySlot;

	LList <Data *> transitData;

	int nbDataDatabank;

public:

	Defrag ();
	~Defrag ();

	void Initialise ();     
	void Tick ( int n );    
	
	void CreateInterface ();       
	void RemoveInterface ();
	void ShowInterface ();

	bool IsInterfaceVisible ();     

};

#endif

