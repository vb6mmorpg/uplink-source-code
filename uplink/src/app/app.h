// -*- tab-width:4 c-file-style:"cc-mode" -*-

/*
	App object

	Stores data about the app - eg path, version, etc

  */

#ifndef _included_app_h
#define _included_app_h

// ============================================================================

#include "app/uplinkobject.h"


class Options;
class Network;
class MainMenu;
class PhoneDialler;


#define SIZE_APP_PATH    256
#define SIZE_APP_VERSION 32
#define SIZE_APP_TYPE    32
#define SIZE_APP_DATE    32
#define SIZE_APP_TITLE   64
#define SIZE_APP_RELEASE 256

// ============================================================================


class App : public UplinkObject
{

public :

    char path    [SIZE_APP_PATH];
    char userpath [SIZE_APP_PATH];
    char usertmppath [SIZE_APP_PATH]; // Used by the crash reporter to store the current .usr
    char userretirepath [SIZE_APP_PATH]; // Used to store old agents ( .usr / .tmp )
    char version [SIZE_APP_VERSION];
    char type    [SIZE_APP_TYPE];
    char date    [SIZE_APP_DATE];
    char title   [SIZE_APP_TITLE];
    char release [SIZE_APP_RELEASE];          

    int starttime;
    bool closed;

    Options *options;
	Network *network;
	MainMenu *mainmenu;
	PhoneDialler *phoneDial;

	char *nextLoadGame;

	bool askCodeCard;

public:

    App ();
    ~App ();
    
    void Set ( char *newpath, char *newversion, char *newtype,
			   char *newdate, char *newtitle );
    
	void Initialise ();	
	void Close ();			// Shuts down the app	
    bool Closed ();         // True if in the process of shutting down

	void SetNextLoadGame ( const char *username );       // Set the username to load with the next call to LoadGame
	void LoadGame ();                                    // Use the username set with SetNextLoadGame
	void LoadGame ( char *username );
	void SaveGame ( char *username );
	void RetireGame ( char *username );
	static DArray <char *> *ListExistingGames ();

	Options *GetOptions ();
	Network *GetNetwork ();
	MainMenu *GetMainMenu ();
	void RegisterPhoneDialler ( PhoneDialler *phoneDiallerScreen );
	void UnRegisterPhoneDialler ( PhoneDialler *phoneDiallerScreen );

    static void CoreDump ();             

	// Common functions

	bool Load   ( FILE *file );
	void Save   ( FILE *file );
	void Print  ();
	void Update ();
	char *GetID ();

};

extern App *app;

#endif


