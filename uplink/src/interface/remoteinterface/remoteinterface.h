
/*

  Remote Interface class object

	Part of the Interface sub-system
	Handles the interface projected by the current host machine -
	 for example a password screen, a log screen etc
	
  */

#ifndef _included_remoteinterface_h
#define _included_remoteinterface_h


#include "app/uplinkobject.h"

#include "interface/remoteinterface/remoteinterfacescreen.h"

#include "world/person.h"

// === List all screen codes here =============================================


#define  SCREEN_UNKNOWN					0
#define  SCREEN_MESSAGESCREEN			1
#define  SCREEN_PASSWORDSCREEN			2
#define  SCREEN_MENUSCREEN				3
#define  SCREEN_BBSSCREEN				4
#define  SCREEN_DIALOGSCREEN			5
#define  SCREEN_FILESERVERSCREEN		6
#define  SCREEN_LINKSSCREEN				7
#define  SCREEN_LOGSCREEN				8
#define  SCREEN_SWSALESSCREEN			9
#define  SCREEN_HWSALESSCREEN			10
#define  SCREEN_RECORDSCREEN			11
#define  SCREEN_USERIDSCREEN			12
#define  SCREEN_ACCOUNTSCREEN			13
#define  SCREEN_CONTACTSCREEN			14
#define  SCREEN_NEWSSCREEN				15
#define  SCREEN_CRIMINALSCREEN			16
#define	 SCREEN_SECURITYSCREEN			17
#define  SCREEN_ACADEMICSCREEN			18
#define  SCREEN_RANKINGSCREEN			19
#define  SCREEN_CONSOLESCREEN			20
#define  SCREEN_SOCSECSCREEN			21
#define	 SCREEN_LOANSSCREEN				22
#define  SCREEN_SHARESLISTSCREEN		23
#define  SCREEN_SHARESVIEWSCREEN		24
#define  SCREEN_FAITHSCREEN				25
#define  SCREEN_CYPHERSCREEN			26
#define  SCREEN_VOICEANALYSIS			27
#define  SCREEN_COMPANYINFO				28
#define  SCREEN_VOICEPHONE				29
#define  SCREEN_HIGHSECURITYSCREEN		30
#define  SCREEN_NEARESTGATEWAY			31
#define  SCREEN_CHANGEGATEWAY			32
#define  SCREEN_CODECARD                33
#define  SCREEN_DISCONNECTEDSCREEN      34
#define  SCREEN_PROTOVISION             35
#define  SCREEN_NUCLEARWAR              36
#define  SCREEN_RADIOTRANSMITTER        37


// ============================================================================

class Computer;


class RemoteInterface : public UplinkObject
{

protected:

	RemoteInterfaceScreen *screen;

public:

	int previousscreenindex;
	int currentscreenindex;
	char security_name [SIZE_PERSON_NAME];
	int  security_level;

public:

	RemoteInterface ();
	~RemoteInterface ();

	void RunNewLocation ();						// Looks at players location
	
	void RunScreen ( int screenindex, Computer *compref = NULL );			// Runs at existing location
	bool VerifyScreen ( int screenindex );

	void SetSecurity ( char *newname, int newvalue );

	void Create();
	void Remove ();
	bool IsVisible ();

	RemoteInterfaceScreen *GetInterfaceScreen ();			// Asserts screen
	ComputerScreen *GetComputerScreen ();					

	// Common functions

	bool Load   ( FILE *file );
	void Save   ( FILE *file );
	void Print  ();
	void Update ();

	char *GetID ();

};


#endif
