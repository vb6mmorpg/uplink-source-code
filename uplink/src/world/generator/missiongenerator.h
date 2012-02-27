// MissionGenerator.h: interface for the MissionGenerator class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _included_missiongenerator_h
#define _included_missiongenerator_h

#include "game/data/data.h"

class Person;
class Mission;
class Message;
class Company;
class BankComputer;
class BankAccount;
class Computer;
class Probability;

//=============================================================================
// Mission types

#define		MISSION_NONE				-1
#define     MISSION_SPECIAL				0

#define		MISSION_STEALFILE			1
#define		MISSION_DESTROYFILE			2
#define		MISSION_FINDDATA			3
#define		MISSION_CHANGEDATA			4
#define		MISSION_FRAMEUSER			5
#define		MISSION_TRACEUSER			6
#define		MISSION_CHANGEACCOUNT		7
#define		MISSION_REMOVECOMPUTER		8
#define		MISSION_REMOVECOMPANY		9
#define		MISSION_REMOVEUSER			10

#define		MISSION_PAYFINE				20

//=============================================================================


class MissionGenerator
{

protected:

	static Probability *prob_missiontype [NUM_UPLINKRATINGS];

public:

	static void Initialise ();
    static void Shutdown ();

	/*
		All these functions below generate the mission specified,
		add it into the World::Missions database
		and return it for convenience

	*/

	static Mission *GenerateMission ();					// Generates a random mission
	static Mission *GenerateMission ( int type );
	static Mission *GenerateMission ( int type, Company *employer );


	static Mission *Generate_StealFile			( Company *employer );
	static Mission *Generate_StealSingleFile	( Company *employer, Computer *target );
	static Mission *Generate_StealAllFiles		( Company *employer, Computer *target );
	static Mission *Generate_DestroyFile		( Company *employer );
	static Mission *Generate_DestroySingleFile  ( Company *employer, Computer *target );
	static Mission *Generate_DestroyAllFiles    ( Company *employer, Computer *target );
	
	static Mission *Generate_FindData       ( Company *employer );
	static Mission *Generate_ChangeData     ( Company *employer );
	static Mission *Generate_FrameUser      ( Company *employer );
	static Mission *Generate_TraceUser      ( Company *employer );
	static Mission *Generate_ChangeAccount  ( Company *employer, Computer *source, Computer *target );
	static Mission *Generate_RemoveComputer ( Company *employer, Computer *target );
	static Mission *Generate_RemoveCompany  ( Company *employer );
	static Mission *Generate_RemoveUser     ( Company *employer );

	static Mission *Generate_ChangeData_AcademicRecord ( Company *employer );
	static Mission *Generate_ChangeData_SocialSecurity ( Company *employer );
	static Mission *Generate_ChangeData_CriminalRecord ( Company *employer );

	static Mission *Generate_FindData_FinancialRecord  ( Company *employer, Computer *target );

	/*
		Special case missions that can't be randomly generated - 
		they must be specifically requested
	*/

	static Mission *Generate_PayFine				 ( Person *person, Company *company, int amount, Date *duedate, char *reason );	
	static Mission *Generate_FrameUser				 ( Company *employer, Person *person );
	static Mission *Generate_TraceHacker			 ( Mission *completedmission, Person *hacker );
	static Mission *Generate_TraceHacker			 ( Computer *hacked, Person *hacker );
	static Mission *Generate_TraceUser_MoneyTransfer ( Company *employer, BankComputer *source, BankComputer *target, 
													   BankAccount *sacc, BankAccount *tacc, int amount, Person *hacker );	


	/*
		These functions below test if the player has completed 
		a mission that he was given.  They also act upon his success or failure.
		
	*/

	static bool IsMissionComplete ( Mission *mission, Person *person, Message *message );

	static bool IsMissionComplete_StealFile		 ( Mission *mission, Person *person, Message *message );
	static bool IsMissionComplete_StealAllFiles	 ( Mission *mission, Person *person, Message *message );
	static bool IsMissionComplete_DestroyFile	 ( Mission *mission, Person *person, Message *message );
	static bool IsMissionComplete_FindData		 ( Mission *mission, Person *person, Message *message );
    static bool IsMissionComplete_FindFinancial  ( Mission *mission, Person *person, Message *message );
	static bool IsMissionComplete_ChangeData	 ( Mission *mission, Person *person, Message *message );
	static bool IsMissionComplete_FrameUser		 ( Mission *mission, Person *person, Message *message );
	static bool IsMissionComplete_TraceUser		 ( Mission *mission, Person *person, Message *message );
	static bool IsMissionComplete_ChangeAccount	 ( Mission *mission, Person *person, Message *message );
	static bool IsMissionComplete_RemoveComputer ( Mission *mission, Person *person, Message *message );
	static bool IsMissionComplete_RemoveCompany	 ( Mission *mission, Person *person, Message *message );
	static bool IsMissionComplete_RemoveUser	 ( Mission *mission, Person *person, Message *message );
	static bool IsMissionComplete_PayFine		 ( Mission *mission, Person *person, Message *message );
    static bool IsMissionComplete_Special        ( Mission *mission, Person *person, Message *message );

	static void MissionCompleted ( Mission *mission, Person *person, Message *message );
	static void MissionNotCompleted ( Mission *mission, Person *person, Message *message, char *reason );

	/*
		Call this if a person fails a mission - 
		ie runs out of time or totally fucks it up

	*/

	static void MissionFailed ( Mission *mission, Person *person, char *reason );

};

#endif 
