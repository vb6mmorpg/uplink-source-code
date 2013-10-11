
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "vanbakel.h"
#include "interface.h"

#include "mmgr.h"

// Local protected variables 


local LList <TaskWrapper *> tasklist;          // List of all running tasks

// Local interface variables

local int numregisteredbuttons = 0;            // Number of buttons currently on screen
local int xpos;
local int ypos;

local int nextpid = 0;

void SvbReset ()
{

	for ( int i = 0; i < tasklist.Size (); i++ )
		if ( tasklist.ValidIndex ( i ) && tasklist.GetData ( i ) )
			SvbRemoveTask ( tasklist.GetData ( i )->task );

	tasklist.Empty ();
	nextpid = 0;

	Svb_initialise_interface ();

}

int SvbRegisterTask ( char *name, Task *task )
{

	// Add the task to the list

	TaskWrapper *newtw = new TaskWrapper;
	newtw->SetPID ( SvbGenerateUniquePID () );
	newtw->SetName ( name );
	newtw->SetTask ( task );
	newtw->SetPriority ( 0.0 );	
	tasklist.PutData ( newtw );

	SvbCalculatePriorities_Add ();	
	SvbUpdateInterface ();

	// Run the task

	task->Initialise ();
	if ( SvbIsInterfaceVisible () ) task->CreateInterface ();

	return newtw->pid;

}

void SvbRemoveTask ( int pid )
{

	int index = SvbLookupIndex ( pid );
	
	double removedpriority;

	if ( tasklist.ValidIndex (index) ) {

		TaskWrapper *taskwrapper = tasklist [index];
		assert ( taskwrapper );

		if ( taskwrapper->task ) {
			taskwrapper->task->RemoveInterface ();
			delete taskwrapper->task;
		}

		removedpriority = taskwrapper->priority;
		tasklist.RemoveData ( index );

		delete taskwrapper;

		SvbCalculatePriorities_Remove ( removedpriority );

	}

	SvbUpdateInterface ();

}

void SvbRemoveTask ( Task *task )
{

	int pid = SvbLookupPID ( task );

	SvbRemoveTask ( pid );

}

int SvbNumTasks ()
{

	return tasklist.Size ();

}

int SvbGenerateUniquePID ()
{

	++nextpid;
	return nextpid;

}

void SvbCalculatePriorities ()
{

	for ( int i = 0; i < tasklist.Size (); ++i ) {

		TaskWrapper *tw = tasklist [i];
		assert ( tw );
		tw->SetPriority ( 1.0 / (double) tasklist.Size () );

	}

	SvbCompensatePriorities ();

}

void SvbCalculatePriorities_Add ()
{

	if ( tasklist.Size () <= 0 )
		return;

	double newpriority = 1.0 / (double) tasklist.Size ();

	for ( int i = 0; i < tasklist.Size () - 1; ++i ) {
		assert ( tasklist [i] );
		SvbChangePriority ( i, tasklist [i]->priority * -newpriority, true );
	}
	
	assert ( tasklist [tasklist.Size () - 1] );
	SvbChangePriority ( tasklist.Size () - 1, newpriority, true );

	SvbCompensatePriorities ();

}

void SvbCalculatePriorities_Remove ( double removedpriority )
{

	if ( tasklist.Size () <= 0 )
		return;

	double dP = double ( tasklist.Size () + 1 ) / double ( tasklist.Size () );	

	for ( int i = 0; i < tasklist.Size (); ++i ) {
		assert ( tasklist [i] );
		SvbChangePriority ( i, tasklist [i]->priority * removedpriority * dP, true );
	}

	SvbCompensatePriorities ();

}

void SvbChangePriority ( int index, double amount, bool locked )
{

	TaskWrapper *tw = tasklist [index];

	if ( tw ) {

		// Change the indexed priority, but not out of bounds
		double actualamount;
		
		assert ( tasklist [index] );

		if ( tasklist [index]->priority + amount > 1.0 ) {
			actualamount = 1.0 - tasklist [index]->priority;
			tasklist [index]->priority = 1.0;
		}
		else if ( tasklist [index]->priority + amount < 0.0 ) {
			actualamount = 0.0 - tasklist [index]->priority;
			tasklist [index]->priority = 0.0;
		}
		else {
			actualamount = amount;
			tasklist [index]->priority += amount;
		}

		if ( !locked ) {

			// Change all other task priorities the other direction

			for ( int i = 0; i < tasklist.Size (); ++i ) {

				assert ( tasklist [i] );
				double dP = double ( tasklist.Size () ) / double ( tasklist.Size () - 1 );	

				if ( i != index ) SvbChangePriority ( i, tasklist [i]->priority * (double) actualamount * -dP, true );

			}

		}

	}

}

void SvbCompensatePriorities ()
{

	if ( tasklist.Size () <= 0 )
		return;

	// Calculate the total priority

	double total = 0.0;

	for ( int i = 0; i < tasklist.Size (); ++i ) {

		assert ( tasklist [i] );
		total += tasklist [i]->priority;

	}

	// Work out the error

	double error = 1.0 - total;

	// Distribute the error over the tasks if neccisary

	if ( error > 0.05 ) {

		for ( int i = 0; i < tasklist.Size (); ++i ) {
			
			assert ( tasklist [i] );
			SvbChangePriority ( i, tasklist [i]->priority * error, true );

		}

	}

}

void SvbCycle ()
{
	
	SvbCycle ( 1 );

}

void SvbCycle ( int n )
{

	for ( int t = 0; t < tasklist.Size (); ++t ) {

		TaskWrapper *tw = tasklist [t];
		assert (tw);

		tw->progress += ( n * tw->priority );

		if ( tw->progress > 1.0 ) {
			
			int numticks = 1 + int (tw->progress - 1.0);
			tw->task->Tick ( numticks );
			tw->progress -= (double) numticks;

		}

	}

}

int SvbLookupIndex ( int pid )
{

	for ( int i = 0; i < tasklist.Size (); ++i ) {

		TaskWrapper *tw = tasklist [i];
		assert ( tw );

		if ( tw->pid == pid )
			return i;

	}

	return -1;	

}

int SvbLookupPID ( Task *task )
{

	for ( int i = 0; i < tasklist.Size (); ++i ) {

		TaskWrapper *tw = tasklist [i];
		assert ( tw );

		if ( tw->task == task )
			return tw->pid;

	}

	return -1;	

}

Task *SvbGetTask ( int pid )
{
	
	for ( int i = 0; i < tasklist.Size (); ++i ) {

		TaskWrapper *tw = tasklist [i];
		assert ( tw );

		if ( tw->pid == pid )
			return tw->task;

	}

	return NULL;

}

Task *SvbGetTask ( char *name )
{

    for ( int i = 0; i < tasklist.Size (); ++i ) {

        TaskWrapper *tw = tasklist [i];
        assert (tw);

        if ( strcmp ( tw->name, name ) == 0 )
            return tw->task;

    }

    return NULL;

}

TaskWrapper *SvbGetTaskWrapper ( int pid )
{

	for ( int i = 0; i < tasklist.Size (); ++i ) {

		TaskWrapper *tw = tasklist [i];
		assert ( tw );

		if ( tw->pid == pid )
			return tw;

	}

	return NULL;
	
}

TaskWrapper *SvbGetTaskWrapperAtIndex ( int index )
{
	
	if ( index < tasklist.Size () ) 
		return tasklist [index];

	else
		return NULL;

}

void SvbCreateInterface ( int x, int y )
{

	xpos = x;
	ypos = y;

	if ( !SvbIsInterfaceVisible () ) {

		// Create a button which covers the scale bar (to ensure it is properly masked when removed)
		EclRegisterButton ( x - 3, y - 4, 106, 11, "", "svb_mask" );
		EclRegisterButtonCallbacks ( "svb_mask", NULL, NULL, NULL, NULL );

		// Create a title bar
		EclRegisterButton ( x, y - 15, 100, 12, "CPU usage", "", "svb_titlebar" );
		EclRegisterButtonCallbacks ( "svb_titlebar", Svb_textbutton_draw, NULL, NULL, NULL );

		// Create the scale bar
		EclRegisterButton ( x, y, 100, 5, "0...........50...........100", "svb_scalebar" );
		EclRegisterButtonCallbacks ( "svb_scalebar", Svb_button_draw, NULL, NULL, NULL );
										  
		// Create the task buttons

		for ( int i = 0; i < SvbNumTasks (); ++i ) {
			
			SvbCreateColumn ( xpos, ypos, tasklist [i], i );
			assert ( tasklist [i]->task );
			tasklist [i]->task->CreateInterface ();

		}
		
		numregisteredbuttons = SvbNumTasks ();

	}

}

void SvbRemoveInterface ()
{

	if ( SvbIsInterfaceVisible () ) {

		// Remove the scale bar
		EclRemoveButton ( "svb_mask" );
		EclRemoveButton ( "svb_titlebar" );
		EclRemoveButton ( "svb_scalebar" );

		// Remove the task buttons
		for ( int i = 0; i < SvbNumTasks (); ++i ) {

			SvbRemoveColumn ( i );
			assert ( tasklist [i]->task );
			tasklist [i]->task->RemoveInterface ();

		}

	}
}

void SvbShowTask ( int pid )
{

	TaskWrapper *tw = SvbGetTaskWrapper ( pid );

	if ( tw ) {

		assert ( tw->task );
		tw->task->CreateInterface ();

	}

}

void SvbHideTask ( int pid )
{

	TaskWrapper *tw = SvbGetTaskWrapper ( pid );

	if ( tw ) {

		assert ( tw->task );
		tw->task->RemoveInterface ();

	}

}

void SvbShowAllTasks ()
{

	for ( int i = 0; i < tasklist.Size (); ++i ) {

		TaskWrapper *tw = tasklist [i];
		assert ( tw );
		assert ( tw->task );

		tw->task->ShowInterface ();
		
	}

}

void SvbUpdateInterface ()
{

	if ( SvbIsInterfaceVisible () ) {

		// Add / remove task buttons as neccisary

		if ( SvbNumTasks () > numregisteredbuttons ) {

			// Add more buttons
			for ( int i = numregisteredbuttons; i < SvbNumTasks (); ++i )
				SvbCreateColumn ( xpos, ypos, tasklist [i], i );

		}
		else if ( SvbNumTasks () < numregisteredbuttons ) {

			// Remove some buttons
			for ( int i = SvbNumTasks (); i < numregisteredbuttons; ++i )
				SvbRemoveColumn ( i );

		}

		numregisteredbuttons = SvbNumTasks ();

		// Resize/update the remaining buttons
		
		for ( int i = 0; i < SvbNumTasks (); ++i ) {

			TaskWrapper *tw = tasklist [i];
			assert ( tw );

			char name [32];
			sprintf ( name, "svb_priority %d", i );

			char caption [32];
			sprintf ( caption, "%s", tw->name );
			//sprintf ( caption, "%f", tw->priority );

			EclRegisterResize ( name, (int)(tw->priority * 100), 10, 500 );
			EclGetButton ( name )->SetCaption ( caption );

		}

	}

}

bool SvbIsInterfaceVisible ()
{

	return ( EclGetButton ( "svb_scalebar" ) != NULL );

}

void SvbDebugPrint ()
{

	printf ( "============== V A N  B A K E L ============================\n" );	

	printf ( "numregisteredbuttons:%d, xpos:%d, ypos:%d, nextpid:%d\n", numregisteredbuttons, xpos, ypos, nextpid );

	for ( int i = 0; i < tasklist.Size (); ++i )
		tasklist.GetData (i)->DebugPrint ();

	printf ( "============== E N D  O F  V A N  B A K E L ================\n" );

}

