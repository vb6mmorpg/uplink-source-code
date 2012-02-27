
/*
	Interface class object

		Responsible for handling all user input 
		including mouse, keyboard, buttons, sounds, animations etc.

		Includes the main HUD, the interface for the current players location
		and the task manager, as well as a list of all possible "tasks" - 
		eg break password, crack code etc.

  */


#ifndef _included_classinterface_h				
#define _included_classinterface_h

#include <stdio.h>

// ============================================================================

#include "app/uplinkobject.h"


class LocalInterface;
class RemoteInterface;
class TaskManager;

// ============================================================================


class Interface : public UplinkObject
{

protected:

	LocalInterface  *li;
	RemoteInterface *ri;
	TaskManager     *tm;

public:

	Interface();
	virtual ~Interface();

	void Create ();


	LocalInterface  *GetLocalInterface  ();				// Asserts li
	RemoteInterface *GetRemoteInterface ();				// Asserts ri
	TaskManager     *GetTaskManager     ();				// Asserts tm
	

	// Common functions

	bool Load   ( FILE *file );
	void Save   ( FILE *file );
	void Print  ();
	void Update ();

	char *GetID ();

};


#endif 
