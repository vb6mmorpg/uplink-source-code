

/*
   ============================================================================

		  V A N B A K E L

	   Task Manager for Uplink
		By Christopher Delay

		Started 9th June '99

   ============================================================================
*/


#ifndef _included_vanbakel_h
#define _included_vanbakel_h

#include "tosser.h"
#include "eclipse.h"

#include "task.h"
#include "taskwrapper.h"

//#ifdef _DEBUG
//#include "slasher.h"
//#endif


// Initialisation functions ===================================================


void SvbReset ();

int  SvbRegisterTask ( char *name, Task *task );			// Returns PID
void SvbRemoveTask ( int pid );
void SvbRemoveTask ( Task *task );

// Lookup functions ===========================================================

int  SvbNumTasks ();

int  SvbLookupIndex			   ( int pid );
Task *SvbGetTask               ( int pid );
Task *SvbGetTask               ( char *name );
TaskWrapper *SvbGetTaskWrapper ( int pid );

TaskWrapper *SvbGetTaskWrapperAtIndex ( int index );					// Careful - indexes are not constant

int  SvbLookupPID			   ( Task *task );
int SvbGenerateUniquePID ();

// Priority functions =========================================================


void SvbCalculatePriorities ();         // Loses all pre-sets, levels out tasks
void SvbCalculatePriorities_Add ();     // Takes into account adding 1 process
void SvbCalculatePriorities_Remove ( double removedpriority);  
										// Takes into account removing 1 process 

void SvbCompensatePriorities ();        // Compensates for accumulated errors

// Locked - no other priorities will be changed
void SvbChangePriority ( int index, double amount, bool locked = false );

// Visualisation of VanBakel tasks (uses Eclipse library) =====================


void SvbCreateInterface ( int x, int y );
void SvbUpdateInterface ();

void SvbShowTask ( int pid );			// Calls Task::CreateInterface
void SvbHideTask ( int pid );			// Calls Task::RemoveInterface

void SvbShowAllTasks ();				// Calls Task::ShowInterface on every task

bool SvbIsInterfaceVisible ();
void SvbRemoveInterface ();

// Main loop function - call once for a complete cycle ========================


void SvbCycle ();
void SvbCycle ( int n );


// Debugging ==================================================================


void SvbDebugPrint ();


#endif
