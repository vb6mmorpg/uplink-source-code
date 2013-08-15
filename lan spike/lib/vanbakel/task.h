
/*

  Task Object

	Any task that you wish to run on the VanBakel
	Task Manager must derive from this class and implement
	the virtual functions.

  */


#ifndef _included_task_h
#define _included_task_h

//#ifdef _DEBUG
//#include "slasher.h"
//#endif


class Task  
{

protected:

	void (*callback) ();			// Called when the task has finished

public:

	Task();
	virtual ~Task();

	void SetCallback ( void (*newcallback) () );

	virtual void Initialise ();     // Called at creation time, neccisary
	void Tick ();					// Called every clock tick, provided
	virtual void Tick ( int n );	// n ticks occur, neccisary

	virtual void CreateInterface ();        // You must provide this
	virtual void RemoveInterface ();		// You must provice this
	virtual void ShowInterface ();			// ie bring to front (optional)

	virtual bool IsInterfaceVisible ();     // You must provide this

	int GetPID ();							// Looks up PID of this task

};

#endif 
