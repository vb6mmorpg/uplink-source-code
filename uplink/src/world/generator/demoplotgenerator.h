
/*

    Demo Plot Generator

        Only one instance occurs, and only in the demo
        Responsible for ensuring there is always something
        interesting to do in the demo

  */


#ifndef _included_demoplotgenerator_h
#define _included_demoplotgenerator_h


#include "app/uplinkobject.h"


class DemoPlotGenerator : public UplinkObject 
{

protected:

    int scene;
    char arcmissiontarget [SIZE_PERSON_NAME];

protected:

    void MoveNewMissionToRandomLocation ( Mission *mission );
    
    bool NumAvailableMissions ( int missiontype );                      // Counts number available to player

    Mission *Generate_FileCopyMission ();
    Mission *Generate_FileDeleteMission ();
    Mission *Generate_ChangeAcademicMission ();
    Mission *Generate_ChangeSocialRecordARC ();

public:

    DemoPlotGenerator ();
    ~DemoPlotGenerator ();

    void Initialise ();

    void PlayerRatingChange ();
    void PlayerCompletedMission ( Mission *mission );

    void GenerateNewMission ( bool randomdate = false );                // Occurs every few hours

   	bool PlayerContactsARC ( Message *msg );					        // These handle all emails from the player to the main co's.  

    void RunScene ( int scene );


	// 
	// Common functions

	bool Load  ( FILE *file );
	void Save  ( FILE *file );
	void Print ();
	void Update ();
	
	char *GetID ();

};


#endif

