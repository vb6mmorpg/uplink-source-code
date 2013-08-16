// Task.cpp: implementation of the Task class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>

#include "vanbakel.h"

#include "task.h"

#include "mmgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Task::Task()
{
	callback = NULL;
}

Task::~Task()
{

}

void Task::SetCallback ( void (*newcallback) () )
{

	callback = newcallback;

}

void Task::Initialise () 
{
}

void Task::Tick ()
{
	Tick ( 1 );
}

void Task::Tick ( int n ) 
{
}

void Task::CreateInterface ()
{
}

void Task::RemoveInterface ()
{
}

void Task::ShowInterface ()
{
}

bool Task::IsInterfaceVisible ()
{
	return false;
}

int Task::GetPID ()
{

	return SvbLookupPID ( this );

}
