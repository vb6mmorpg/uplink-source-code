// TaskWrapper.cpp: implementation of the TaskWrapper class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>

#include "taskwrapper.h"

#include "mmgr.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TaskWrapper::TaskWrapper()
{

	task = NULL;
	name = NULL;
	priority = 0.0;
	progress = 0.0;

}

TaskWrapper::~TaskWrapper()
{

	if ( name )
		delete [] name;

}

void TaskWrapper::SetPID ( int newpid )
{

	pid = newpid;

}

void TaskWrapper::SetName ( char *newname )
{

	if ( name ) delete name;
	name = new char [strlen(newname)+1];
	strcpy ( name, newname );

}

void TaskWrapper::SetTask ( Task *newtask )
{

	task = newtask;

}

void TaskWrapper::SetPriority ( double newpriority )
{

	priority = newpriority;

}

void TaskWrapper::DebugPrint ()
{

	printf ( "TASK :   %s : PID:'%d', priority:%f, progress:%f\n", name, pid, (float) priority, (float) progress );

}


