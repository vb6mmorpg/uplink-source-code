
/*

  Local Interface class object

	Part of the Interface subsystem
	Handles the interface projected by our local machine gateway
	 for example the current date/time, worldmap etc

  */


#ifndef _included_localinterface_h
#define _included_localinterface_h


#include "app/uplinkobject.h"


class LocalInterfaceScreen;
class HUDInterface;


// === List all screen codes here =============================================

#define  SCREEN_NONE		0
#define  SCREEN_HUD			1
#define  SCREEN_HW			3
#define  SCREEN_MAP			4
#define  SCREEN_MEMORY		6
#define  SCREEN_STATUS		7
#define  SCREEN_EMAIL		8
#define  SCREEN_FINANCE		9
#define  SCREEN_MISSION		10
#define  SCREEN_SENDMAIL	11
#define  SCREEN_CHEATS		12
#define  SCREEN_EVTQUEUE	13
#define  SCREEN_ANALYSER	14
#define	 SCREEN_GATEWAY     15
#define  SCREEN_IRC			16
#define  SCREEN_LAN         17

// ============================================================================


#define  PANELSIZE		 0.29						// Percentage of screen taken up by right panel



class LocalInterface : public UplinkObject  
{

protected:

	LocalInterfaceScreen *screen;	
	HUDInterface *hud;

public:

	int currentscreencode;
	int screenindex;					// Used for eg when SCREENCODE=SCREEN_EMAIL (index of email viewed)

public:

	LocalInterface();
	virtual ~LocalInterface();
	
	void Reset ();

	void Create ();
	void Remove ();
	bool IsVisible ();


	int InScreen ();									// Returns id code of current screen
	void RunScreen ( int SCREENCODE, int index = -1 );
	bool VerifyScreen ( int SCREENCODE, int index = -1 );

	LocalInterfaceScreen *GetInterfaceScreen ();		// Asserts screen
	HUDInterface *GetHUD ();							// Asserts hud


	// Common functions

	bool Load   ( FILE *file );
	void Save   ( FILE *file );
	void Print  ();
	void Update ();

	char *GetID ();

};

#endif 
