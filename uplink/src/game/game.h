
/* 

  Game object

	Top level glue object of Uplink
	Owns all of the other major components - 
	World, View, Interface and Player

  */


#ifndef _included_game_h
#define _included_game_h

#include <stdio.h>
#include <time.h>

// ============================================================================

#include "app/uplinkobject.h"

class Interface;
class View;
class World;
class GameObituary;

#define GAMESPEED_GAMEOVER		-1
#define GAMESPEED_PAUSED		0
#define GAMESPEED_NORMAL		1
#define GAMESPEED_FAST			2
#define GAMESPEED_MEGAFAST		3
#define GAMESPEED_OHMYGOD		4						// Only in DEBUG mode


// ============================================================================


class Game : public UplinkObject
{

protected:

	Interface *ui;
	View      *view;
	World     *world;

	int gamespeed;

	GameObituary *gob;                     // Only used if this is a dead person
	time_t lastsave;                       // Used in autosaving

	char *loadedSavefileVer;               // The game is/was loaded from a savefile from that version, if NULL assume current

	char *createdSavefileVer;              // The game was created in a savefile from that version

	int   winningCodeWon;                  // Does what needed to win the code was done?
	char *winningCodeDesc;                 // Winning code description
	char *winningCodeExtra;                // Extra code in front of the winning code
	unsigned int winningCodeRandom;        // The game winning code random

public:

	enum WorldMapType {
		defaultworldmap = 0,
		defconworldmap = 1
	};

protected:

	enum WorldMapType whichWorldMap;        // The world map this game use

public:

	Game ();
	~Game ();

	void NewGame ();
	void ExitGame ();

	void SetGameSpeed ( int newspeed );
	int  GameSpeed ();
	bool IsRunning ();
	
	void GameOver ( char *reason );			//  Ends the current game
    void DemoGameOver ();                   //  This demo game has come to an end
    void WarezGameOver ();                  //  This WAREZ copy has come to an end

											//  These functions will 
	Interface *GetInterface ();				//  stop the program if their
	View      *GetView ();					//  values are NULL, so check
	World     *GetWorld ();					//  Game::IsRunning before using

	GameObituary *GetObituary ();			//  Asserts it exists

    bool LoadGame ( FILE *file );           //  Use this rather than Load

	// Common functions

	bool Load   ( FILE *file );
	void Save   ( FILE *file );
	void Print  ();
	void Update ();
	char *GetID ();

	const char *GetLoadedSavefileVer () const;   // Return the savefile version the game is/was loaded from

	// Winning Code functions

	void WinCode ( const char *desc, const char *codeExtra = NULL ); // The player won the code

	bool  IsCodeWon          ();                                     // Is the winning code was won
	char *GetWinningCodeDesc ();                                     // Return the winning code description
	char *GetWinningCode     ();                                     // Return a printable _encrypted_ winning code

	// World map functions

	enum WorldMapType GetWorldMapType ();   // Get the world map this game is using

};


extern Game *game;


#endif

