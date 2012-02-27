
/*

  Computer class object

	Part of the World sub-system
	Describes a computer in the Uplink World
	This computer belongs to one company and is located at a virtual location.

  */


#ifndef _included_computer_h
#define _included_computer_h

// ============================================================================

#include "app/uplinkobject.h"

#include "world/computer/databank.h"
#include "world/computer/logbank.h"
#include "world/computer/recordbank.h"
#include "world/computer/security.h"
#include "world/company/company.h"
#include "world/vlocation.h"

#define SIZE_COMPUTER_NAME   64

// Computer types

#define COMPUTER_TYPE_UNKNOWN                 0
#define COMPUTER_TYPE_PUBLICACCESSSERVER      1
#define COMPUTER_TYPE_INTERNALSERVICESMACHINE 2
#define COMPUTER_TYPE_CENTRALMAINFRAME        4
#define COMPUTER_TYPE_PUBLICBANKSERVER		  8
#define COMPUTER_TYPE_PERSONALCOMPUTER		  16
#define COMPUTER_TYPE_VOICEPHONESYSTEM		  32
#define COMPUTER_TYPE_LAN                     64


#define COMPUTER_TYPE_ANY     COMPUTER_TYPE_PUBLICACCESSSERVER          | \
                              COMPUTER_TYPE_INTERNALSERVICESMACHINE     | \
                              COMPUTER_TYPE_CENTRALMAINFRAME            | \
                              COMPUTER_TYPE_PUBLICBANKSERVER            | \
                              COMPUTER_TYPE_PERSONALCOMPUTER            | \
                              COMPUTER_TYPE_VOICEPHONESYSTEM            | \
                              COMPUTER_TYPE_LAN

    
// Trace actions

#define COMPUTER_TRACEACTION_NONE				0
#define COMPUTER_TRACEACTION_DISCONNECT			1
#define COMPUTER_TRACEACTION_WARNINGMAIL		2
#define COMPUTER_TRACEACTION_FINE				4
#define COMPUTER_TRACEACTION_LEGAL				8
#define COMPUTER_TRACEACTION_TACTICAL			16
#define COMPUTER_TRACEACTION_COVERT				32


class ComputerScreen;

// ============================================================================


class Computer : public UplinkObject  
{

protected:

	int numrecenthacks;
	int numhacksthismonth;
	int numhackslastmonth;

public:

	int TYPE;

	char name		 [SIZE_COMPUTER_NAME];			
	char companyname [SIZE_COMPANY_NAME];
	char ip			 [SIZE_VLOCATION_IP];

	int tracespeed;
	int traceaction;

	bool istargetable;								// Can I be targetted by random events
	bool isexternallyopen;							// Can someone connect to me from outside my trusted network
	bool isrunning;
	float isinfected_revelation;	                // Version number
    Date infectiondate; 

	DArray <ComputerScreen *> screens;

	DataBank   databank;	
	LogBank    logbank;
	RecordBank recordbank;
	Security   security;

public:

	Computer();
	virtual ~Computer();

	void SetTYPE        ( int newTYPE );
	void SetName		( char *newname );
	void SetCompanyName ( char *newname );
	void SetIP			( char *newip );
	void SetTraceSpeed  ( int newtracespeed );
	void SetTraceAction ( int newtraceaction );

	void SetIsTargetable		( bool value );
	void SetIsExternallyOpen	( bool value );
	void SetIsRunning			( bool value );

	void DisinfectRevelation ();
	void InfectWithRevelation ( float version );					

	int				AddComputerScreen ( ComputerScreen *cs, int index = -1 );	
	ComputerScreen *GetComputerScreen ( int index );


	void CheckForSecurityBreaches ();				// Call me frequently
	void ManageOldLogs ();							// Call me frequently
    bool ChangeSecurityCodes ();                    // Changes passwords, returns true if changes made

	void AddToRecentHacks ( int n );				// Call when this is hacked
	void UpdateRecentHacks ();						// Call me 4 times a month
	int  NumRecentHacks ();

    static void GenerateAccessCode( char *code, char *result, size_t resultsize );
    static void GenerateAccessCode( char *name, char *code, char *result, size_t resultsize );
    static void GenerateAccessCode( int accNo, char *code, char *result, size_t resultsize );

	// Common functions

	bool Load  ( FILE *file );
	void Save  ( FILE *file );
	void Print ();
	void Update ();
	
	char *GetID ();
	int   GetOBJECTID ();

};

#endif 
