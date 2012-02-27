
/*

	Main Menu controller class

  */


#ifndef _included_mainmenu_h
#define _included_mainmenu_h


#include "app/uplinkobject.h"

class MainMenuScreen;

// === List all screen codes here =============================================


#define  MAINMENU_UNKNOWN			0
#define  MAINMENU_LOGIN				1
#define  MAINMENU_LOADING			2
#define  MAINMENU_FIRSTLOAD			3
#define  MAINMENU_OPTIONS			4
#define  MAINMENU_OBITUARY			5
#define  MAINMENU_CONNECTIONLOST	6
#define  MAINMENU_DISAVOWED			7
#define	 MAINMENU_THETEAM			8
#define  MAINMENU_CLOSING           9
#define  MAINMENU_DEMOGAMEOVER      10
#define  MAINMENU_REVELATIONWON     11
#define  MAINMENU_REVELATIONLOST    12
#define  MAINMENU_WAREZGAMEOVER     13

#define  MAINMENU_GENERICOPTIONS	20
#define  MAINMENU_NETWORKOPTIONS	21
#define  MAINMENU_THEME             22
#define  MAINMENU_GRAPHICOPTIONS	23


// ============================================================================



class MainMenu : public UplinkObject  
{

protected:

	int currentscreencode;
	MainMenuScreen *screen;		

public:

	MainMenu();
	virtual ~MainMenu();
	
	void Create ();
	void Remove ();
	bool IsVisible ();


	int InScreen ();									// Returns id code of current screen
	void RunScreen ( int SCREENCODE );

	MainMenuScreen *GetMenuScreen ();				// Asserts screen
	

	// Common functions

	bool Load   ( FILE *file );
	void Save   ( FILE *file );
	void Print  ();
	void Update ();

	char *GetID ();

};

#endif 
