
/*

  Game Obituary object

	Represents a game that has ended
	So the player can keep tabs on his old characters 

  */


#ifndef _included_gameobituary_h
#define _included_gameobituary_h

#include "app/uplinkobject.h"
#include "world/person.h"


class GameObituary : public UplinkObject
{

protected:

	char *gameoverreason;

public:

	char name [SIZE_PERSON_NAME];
	int money;
	int uplinkrating;
	int neuromancerrating;
	int specialmissionscompleted;
    
	int score_peoplefucked;
	int score_systemsfucked;
	int score_highsecurityhacks;
	
	int score;
	
	bool demogameover;                              // True if this occured due to demo game over
    bool warezgameover;                             // True if this occured due to warez game over
    
public:

	GameObituary ();
	~GameObituary ();

	void SetGameOverReason ( char *newreason );
	char *GameOverReason ();
    void SetDemoGameOver ( bool newvalue );
    void SetWarezGameOver ( bool newvalue );

	void Evaluate ();								// Looks at the player and stores his stats

	// Common functions

	bool Load   ( FILE *file );
	void Save   ( FILE *file );
	void Print  ();
	void Update ();
	char *GetID ();

};


#endif

