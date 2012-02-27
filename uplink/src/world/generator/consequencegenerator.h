
/*

  Consequence Generator

	Responsible for analysing an action that has happened, and 
	generating subsequence "consequence" actions.

    This class should operate in DATA only - it shouldn't be handling
	screen flashes, interface changes etc.  Ideally it should stick to 
	the World module.

	Eg action   : A user is caught performing a hack
	consequence : His Uplink rating is decreased and he receives a fine

	*/


#ifndef _included_consequencegenerator_h
#define _included_consequencegenerator_h

// ============================================================================

#include "eclipse.h"

class Person;
class Computer;
class Mission;
class AccessLog;

// ============================================================================




class ConsequenceGenerator
{

public:

	static void Initialise ();

	// Event processing for all people

	static void CaughtHacking ( Person *person, Computer *comp );
	static void Arrested	  ( Person *person, Computer *comp, char *reason );
	static void DidntPayFine  ( Person *person, Mission *fine  );
	static void ShotByFeds	  ( Person *person, char *reason );
	static void SeizeGateway  ( Person *person, char *reason );

	// Event processing for company computers

	static void ComputerHacked ( Computer *comp, AccessLog *al );

	// Event processing for completed missions

	static void MissionCompleted				( Mission *mission, Person *person );
	static void MissionCompleted_StealFile		( Mission *mission, Person *person );
	static void MissionCompleted_DestroyFile	( Mission *mission, Person *person );
	static void MissionCompleted_ChangeAccount  ( Mission *mission, Person *person );
	static void MissionCompleted_TraceUser		( Mission *mission, Person *person );

};



#endif

