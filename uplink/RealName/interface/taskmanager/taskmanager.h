
/* 
	
  Task Manager class object

	Part of the Interface sub-system
	Handles your local computer's task list which may or may not
	 be visible at any point.  Note that this class is mostly a
	 mapping into the Svb library, which does the real work.

  */

// ============================================================================

#ifndef _included_tasks_h
#define _included_tasks_h


#include "vanbakel.h"

#include "app/uplinkobject.h"

class UplinkTask;

// ============================================================================



class TaskManager : public UplinkObject
{

protected:

	int			  targetprogram;						// PID of running program targetted
	int			  targetprogramreason;					// Reason the target program was changed
	int			  lasttargetprogram;					// PID of last running program targetted
	UplinkObject *programtarget;						// Object to be targeted by program

	int	lastcycle;									// Used to regulate cycle speed

public:

	TaskManager ();
	~TaskManager ();

	void RunSoftware ( char *name, float version );
												
	void SetProgramTarget ( UplinkObject *newprogramtarget, char *targetstring, int targetint );
	void SetTargetProgram ( int newtargetprogram );

	bool IsTargetProgramLast ( int newtargetprogram );

	UplinkTask *GetTargetProgram ();					// Can return NULL
    char *GetTargetProgramName ();                      // Can return NULL

	// Common functions

	bool Load   ( FILE *file );
	void Save   ( FILE *file );
	void Print  ();
	void Update ();

	char *GetID ();

};


#endif

