
/*

  News Generator class

	Receives information on important events that have occured
	(eg a company going bust) and generates a new news story.

	Also handles hard-coded news events

  */


#ifndef _included_newsgenerator_h
#define _included_newsgenerator_h

// ============================================================================

class Computer;
class AccessLog;
class Person;

// ============================================================================


class NewsGenerator
{

public:

	static void Initialise ();

	// Events related to Company computers

	static void ComputerHacked		( Computer *comp, AccessLog *al );					// Hacked by unknown person
	static void ComputerDestroyed	( Computer *comp, bool filesdeleted );
	static void AllFilesStolen		( Computer *comp, char *filetype, int totalfilesize );
	static void AllFilesDeleted		( Computer *comp, char *filetype );

	// Events related to people

	static void Arrested ( Person *person, Computer *comp, char *reason );

};



#endif

