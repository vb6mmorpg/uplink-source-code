
/*

  Console Screen Interface

	Allows extremely low level access to 
	a computer system

  */

#ifndef _included_consolescreeninterface_h
#define _included_consolescreeninterface_H

// ============================================================================

#include "tosser.h"
#include "eclipse.h"

#include "interface/remoteinterface/remoteinterfacescreen.h"

class GenericScreen;
class ConsoleCommand;

// ============================================================================


class ConsoleScreenInterface : public RemoteInterfaceScreen 
{

protected:
	
	LList <char *> users;							// 0 is system, 1 is player
	
	LList <ConsoleCommand *> queue;					// Commands NOT YET RUN
	bool waiting;									// True if waiting for commands to "scroll" on

	int timesync;									// Used to syncronise things
	char currentdir [8];

	bool deleted_sys;

protected:

	static bool IsVisibleInterface ();

	static void WaitingCallback ();							// Called when messagehas finished "scrolling" on
	
	static void TypeHereDraw ( Button *button, bool highlighted, bool clicked );
	static void PostClick ( Button *button );

	static void BorderDraw ( Button *button, bool highlighted, bool clicked );
	static void MessageDraw ( Button *button, bool highlighted, bool clicked );

public:

	ConsoleScreenInterface ();
	~ConsoleScreenInterface ();

	void AddUser ( char *name );

	void SetCurrentDir ( char *currentdir );
	
	void PutText ( int userid, char *text );					// Adds text to end of queue
	void PutTextAtStart ( int userid, char *text );
	
	void RunCommand				( char *command );				// Parses command into ConsoleCommand object
	void RunCommand				( ConsoleCommand *cc );

	void RunCommand_TEXT		( char *text );
	void RunCommand_DIR			();
	void RunCommand_HELP		();
	void RunCommand_EXIT		();
	void RunCommand_DELETEALL	( char *dir );
	void RunCommand_RUN			( char *program, bool actuallyrun );				// If true, the command itself is run
	void RunCommand_SHUTDOWN	();
	void RunCommand_DISCONNECT	();

	void Create ( ComputerScreen *cs );
	void Remove ();
	void Update ();
	bool IsVisible ();

	bool ReturnKeyPressed ();

	int ScreenID ();

	GenericScreen *GetComputerScreen ();


};


// ============================================================================

#define CMDTYPE_NONE		-1
#define CMDTYPE_TEXT		0				// Data1 = string
#define CMDTYPE_DIR			1
#define CMDTYPE_HELP		2
#define CMDTYPE_EXIT		3
#define CMDTYPE_DELETEALL	4				// Data1 = directory or NULL to use current
#define CMDTYPE_CD			5
#define CMDTYPE_SHUTDOWN	6
#define CMDTYPE_DISCONNECT  7
#define CMDTYPE_RUN			8				// Data1 = program name
#define CMDTYPE_RUNPROGRAM	9				// Data1 = program name, actually runs it


class ConsoleCommand
{

public:

	int TYPE;
	char *data1;
	int time;

public:

	ConsoleCommand ();
	ConsoleCommand ( int newTYPE, char *newdata1, int newtime );
	~ConsoleCommand ();

};


#endif
