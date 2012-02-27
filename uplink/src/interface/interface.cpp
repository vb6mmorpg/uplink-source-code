// Interface.cpp: implementation of the Interface class.
//
//////////////////////////////////////////////////////////////////////

#include "vanbakel.h"
#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"

#include "options/options.h"

#include "interface/interface.h"
#include "interface/localinterface/localinterface.h"
#include "interface/remoteinterface/remoteinterface.h"
#include "interface/taskmanager/taskmanager.h"

#include "mmgr.h"

Interface::Interface()
{

	li = new LocalInterface ();
	ri = new RemoteInterface ();
	tm = new TaskManager ();
	
}

Interface::~Interface()
{

	if ( li ) delete li;
	if ( ri ) delete ri;
	if ( tm ) delete tm;

}	

void Interface::Create ()
{

	GetRemoteInterface ()->Create ();
	
	if ( app->GetOptions ()->IsOptionEqualTo ( "game_firsttime", 1 ) ) {
	
		// Stop the local interface from being created

	}
	else {

		GetLocalInterface ()->Reset ();
		GetLocalInterface ()->Create ();

	}

}

LocalInterface *Interface::GetLocalInterface ()
{

	UplinkAssert ( li );
	return li;

}

RemoteInterface *Interface::GetRemoteInterface ()
{

	UplinkAssert ( ri );
	return ri;

}

TaskManager *Interface::GetTaskManager ()
{

	UplinkAssert ( tm );
	return tm;

}

bool Interface::Load ( FILE *file )
{

	LoadID ( file );
	
	if ( !GetRemoteInterface ()->Load ( file ) ) return false;
	if ( !GetLocalInterface  ()->Load ( file ) ) return false;
	if ( !GetTaskManager     ()->Load ( file ) ) return false;
	
	LoadID_END ( file );

	return true;

}

void Interface::Save ( FILE *file )
{

	SaveID ( file );

	GetRemoteInterface ()->Save ( file );
	GetLocalInterface  ()->Save ( file );	
	GetTaskManager     ()->Save ( file );
	
	SaveID_END ( file );

}

void Interface::Print ()
{

	printf ( "============== I N T E R F A C E ===========================\n" );
	
	GetRemoteInterface ()->Print ();
	GetLocalInterface  ()->Print ();
	GetTaskManager     ()->Print ();

	printf ( "============== E N D  O F  I N T E R F A C E ===============\n" );

}

char *Interface::GetID ()
{

	return "INTERFA";

}

void Interface::Update ()
{

	GetLocalInterface  ()->Update ();
	GetRemoteInterface ()->Update ();
	GetTaskManager     ()->Update ();

}
