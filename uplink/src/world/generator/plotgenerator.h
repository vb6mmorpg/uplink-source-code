
/*

  Plot Generator 

	Only one instance exists - owned by the World object
	
	Handles the details of running the main
	plot (Revelation) along side the game.

	Replaces the other generators for this task -
	generates missions, computers, people etc etc

	Also keeps track of the progress of the two main companies

  */


// ============================================================================


#ifndef _included_plotgenerator_h
#define _included_plotgenerator_h

#include "app/uplinkobject.h"
#include "world/person.h"

class Mission;


#define SPECIALMISSION_BACKFIRE				0
#define SPECIALMISSION_TRACER				1
#define SPECIALMISSION_TAKEMETOYOURLEADER	2
#define SPECIALMISSION_ARCINFILTRATION		3
#define SPECIALMISSION_COUNTERATTACK		4

#define SPECIALMISSION_MAIDENFLIGHT			5
#define SPECIALMISSION_DARWIN				6
#define SPECIALMISSION_SAVEITFORTHEJURY		7
#define SPECIALMISSION_SHINYHAMMER			8
#define SPECIALMISSION_GRANDTOUR			9 

#define SPECIALMISSION_MOLE                 10
#define SPECIALMISSION_WARGAMES             11
#define SPECIALMISSION_


// ============================================================================


class PlotGenerator : public UplinkObject
{

protected:

	int act;
	int scene;

	char act1scene3agent [SIZE_PERSON_NAME];						// The guy who first raises the issue
	char act1scene4agent [SIZE_PERSON_NAME];						// The guy who is first executed
	char act2scene1agent [SIZE_PERSON_NAME];						// The guy who does the Arunmor test mission

    bool playervisitsplotsites;                                     // Set to true if the player pokes around the plot
    bool playercancelsmail;											// Has the player cancelled the warning mail from a1s7?
    int playerloyalty;                                              // 0 = none, -1 = ARC, 1 = Arunmor

    bool completed_tracer;
    bool completed_takemetoyourleader; 
    bool completed_arcinfiltration;
    bool completed_darwin;                                          
    bool completed_saveitforthejury;
    bool completed_shinyhammer;

    char saveitforthejury_guytobeframed [SIZE_PERSON_NAME];         // Special variables for special missions
    char saveitforthejury_targetbankip [SIZE_VLOCATION_IP];
    char tracer_lastknownip [SIZE_VLOCATION_IP];

	int numuses_revelation;											// Number of times it has been released
 
	float version_revelation;
	float version_faith;

public:
    
    LList <char *> infected;                                        // List of all infected IP addresses
    bool revelation_releaseuncontrolled;                            // Has the Internet been destroyed?
    bool revelation_releasefailed;                                  // Failed to reach critical mass in time
    bool revelation_arcbusted;                                      // Has ARC been busted?

   	int specialmissionscompleted;									// Bitfield of player completed special missions, see above

protected:

	void Initialise_ARC ();
    void Initialise_Andromeda ();
	void Initialise_ARUNMOR ();
    void Initialise_DARWIN ();

	void Run_Act1Scene1 ();											// ARC begin work on Revelation
	void Run_Act1Scene2 ();											// ARC are hiring all top Uplink Agents - this makes the news
	void Run_Act1Scene3 ();											// Act1Scene3Agent raises suspicion over ARCs activity
	void Run_Act1Scene4 ();											// Act1Scene4Agent posts story - "everything is ok with ARC"
	void Run_Act1Scene5 ();											// Act1Scene4Agent is found dead
	void Run_Act1Scene7 ();											// Act1Scebe4Agent sends out a "delayed" warning mail about ARC
	void Run_Act1Scene8 ();											// Uplink issue a warning to all agents

    void Run_Act2Scene1 ();                                         // The mission "Maiden Flight" is given out
    void Run_Act2Scene2 ();                                         // Another agent completes "Maiden Flight"
    void Run_Act2Scene3 ();                                         // Arunmor get in contact and try to get you to betray ARC

    void Run_Act3Scene1 ();                                         // News story about Revelation attack
    void Run_Act3Scene2 ();                                         // Arunmor begin work on Faith
    void Run_Act3Scene3 ();                                         // Arunmor release details on Revelation, announce their plan
    void Run_Act3Scene4 ();                                         // Feds reveal that ARC is run by Andromeda

    void Run_Act4Scene1 ();                                         // "Tracer" is put out
    void Run_Act4Scene2 ();                                         // "TakeMeToYourLeader" is out
    void Run_Act4Scene3 ();                                         // "ARCInfiltration" is put out
    void Run_Act4Scene4 ();                                         // News story about Tracer
    void Run_Act4Scene5 ();                                         // News story about TakeMeToYourLeader
    void Run_Act4Scene6 ();                                         // News story about ARCInfiltration

    void Run_Act4Scene7 ();                                         // "Darwin" is put out
    void Run_Act4Scene8 ();                                         // "SaveItForTheJury" is put out
    void Run_Act4Scene9 ();                                         // "ShinyHammer" is put out
    void Run_Act4Scene10 ();                                        // News story about Darwin
    void Run_Act4Scene11 ();                                        // News story about SaveItForTheJury
    void Run_Act4Scene12 ();                                        // News story about ShinyHammer

    void Run_Act4Scene13 ();                                        // Somebody else attempts "Tracer"
    void Run_Act4Scene14 ();                                        // Somebody else attempts "TakeMeToYourLeader"
    void Run_Act4Scene15 ();                                        // Somebody else attempts "ARC Infiltration"

    void Run_Act5Scene1 ();                                         // Arunmor warn everyone that virus release is imminent
    void Run_Act5Scene2 ();                                         // "TheGrandTour" and "CounterAttack" are put out
    void Run_Act5Scene3 ();                                         // Player works for Arunmor, successful with Faith
    void Run_Act5Scene4 ();                                         // Player works for no-one
    void Run_Act5Scene5 ();                                         // AI launches Revelation/Faith at a random system
    void Run_Act5Scene6 ();                                         // ARC is busted
    void Run_Act5Scene7 ();                                         // Revelation Release Uncontrolled - GAME OVER

    void Run_Act6Scene1 ();                                         // Leader of Andromeda releases press statement
    void Run_Act6Scene2 ();                                         // Leaders of ARC sentenced to jail
    void Run_Act6Scene3 ();                                         // Profits of Arunmor soar high

    //
    // Special Mission Generators
    
    Mission *GenerateMission_Tracer ();                             // Arunmor #2
    Mission *GenerateMission_TakeMeToYourLeader ();                 // Arunmor #3
    Mission *GenerateMission_ARCInfiltration ();                    // Arunmor #4
    Mission *GenerateMission_CounterAttack ();                      // Arunmor #5
    Mission *GenerateMission_MaidenFlight ();                       // ARC #1
    Mission *GenerateMission_Darwin ();                             // ARC #2
    Mission *GenerateMission_SaveItForTheJury ();                   // ARC #3
    Mission *GenerateMission_ShinyHammer ();                        // ARC #4
    Mission *GenerateMission_GrandTour ();                          // ARC #5
	
    bool IsMissionComplete_Tracer ();                       
    bool IsMissionComplete_TakeMeToYourLeader ();   
    bool IsMissionComplete_ARCInfiltration ();
    bool IsMissionComplete_MaidenFlight ();
    bool IsMissionComplete_Darwin ();
    bool IsMissionComplete_SaveItForTheJury ();
    bool IsMissionComplete_ShinyHammer ();  
     
    void PlayerCompletesSpecialMission	( int missionID );	
    void PlayerFailsSpecialMission		( int missionID );
    bool RemoveSpecialMission			( int missionID );          // Remove from public BBS board
	
    void Revelation_ReleaseUncontrolled ();                         // Its game over! (success for ARC)

	void NewsRevelationUsed ( char *ip, int success );				// Generates a news story

public:

	PlotGenerator ();
	~PlotGenerator ();

	void Initialise ();												// Set up companies/computers/people

	void Run_Scene ( int act, int scene );							// Called by the scheduler

    void PlayerVisitsPlotSites ();   
	void PlayerCancelsMail ();

	bool PlayerContactsARC		( Message *msg );					// These handle all emails from the player to the main co's.  
	bool PlayerContactsARUNMOR	( Message *msg );					// They return true if this function handled the email

	void RunRevelation ( char *ip, float version, bool playerresponsible );
	void RunRevelation ( char *ip, float version, bool playerresponsible, int success );

    void RunRevelationTracer ( char *ip );

    void RunFaith ( char *ip, float version, bool playerresponsible );

	float GetVersion_Faith ();
	float GetVersion_Revelation ();

    void Infected ( char *ip );                                     // These manage the list of 
    void Disinfected ( char *ip );                                  // infected computer systems

    bool PlayerCompletedSpecialMission	( int missionID );
	int GetSpecialMissionsCompleted ();
    
    static char *SpecialMissionDescription ( int missionID );
    static char *SpecialMissionTitle ( int missionID );
    
	static bool IsPlotCompany ( const char *companyname ); 

	// 
	// Common functions

	bool Load  ( FILE *file );
	void Save  ( FILE *file );
	void Print ();
	void Update ();
	
	char *GetID ();

protected:

	void MessageIncompatibleSaveGame ( const char *fileassert, int lineassert );

	bool UplinkIncompatibleSaveGameAssert ( const bool &condition, const char *fileassert, int lineassert );

};



#endif

