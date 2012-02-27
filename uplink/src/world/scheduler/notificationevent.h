
/*
	
  Notification Event

	Used for events which have no data - 
	notifications of other events.

	*/



#ifndef _included_notificationevent_h
#define _included_notificationevent_h

// ============================================================================

#include "world/scheduler/uplinkevent.h"

#define NOTIFICATIONEVENT_TYPE_NONE							0
#define NOTIFICATIONEVENT_TYPE_GROWCOMPANIES				1
#define NOTIFICATIONEVENT_TYPE_GENNEWMISSION				2
#define	NOTIFICATIONEVENT_TYPE_CHECKFORSECURITYBREACHES		3
#define NOTIFICATIONEVENT_TYPE_CHECKMISSIONDUEDATES			4
#define NOTIFICATIONEVENT_TYPE_CHECKRECENTHACKCOUNT			5
#define NOTIFICATIONEVENT_TYPE_GIVEMISSIONTONPC				6
#define NOTIFICATIONEVENT_TYPE_UPLINKMONTHLYFEE				7
#define NOTIFICATIONEVENT_TYPE_EXPIREOLDSTUFF				8
#define NOTIFICATIONEVENT_TYPE_ADDINTERESTONLOANS			9
#define NOTIFICATIONEVENT_TYPE_DEMOGAMEOVER                 10
#define NOTIFICATIONEVENT_TYPE_DEMOGENNEWMISSION            11
#define NOTIFICATIONEVENT_TYPE_BUYAGENTLIST                 12
#define NOTIFICATIONEVENT_TYPE_AGENTSONLISTDIE              13
#define NOTIFICATIONEVENT_TYPE_WAREZGAMEOVER                14


// ============================================================================


class NotificationEvent : public UplinkEvent
{

protected:

	int TYPE;

protected:

	void ApplyMonthlyGrowth ();
	void GenerateNewMission ();
	void CheckForSecurityBreaches ();
	void CheckMissionDueDates ();
	void CheckRecentHackCount ();
	void GiveMissionToNPC ();
	void PayUplinkMonthlyFee ();
	void ExpireOldStuff ();
	void AddInterestOnLoans ();
    void DemoGameOver ();
    void DemoGenerateNewMission ();
    void BuyAgentList ();
    void AgentsOnListDie ();
    void WarezGameOver ();

public:

	NotificationEvent ();
	~NotificationEvent ();

	void SetTYPE ( int newTYPE );

	void Run ();

	char *GetShortString ();
	char *GetLongString ();

	static void ScheduleStartingEvents ();

	// Common functions

	bool Load  ( FILE *file );
	void Save  ( FILE *file );
	void Print ();
	
	char *GetID ();		
	int   GetOBJECTID ();

};


#endif

