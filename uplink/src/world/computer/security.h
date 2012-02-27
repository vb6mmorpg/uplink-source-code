
/*
	
  Security object

	Part of every computer system
	Represents the security systems in place

  */


#ifndef _included_security_h
#define _included_security_h

// ============================================================================


#include "app/uplinkobject.h"
#include "world/computer/securitymonitor.h"

class SecuritySystem;


// ============================================================================



class Security : public UplinkObject
{

protected:

	DArray <SecuritySystem *> systems;
	
public:

	Security ();
	~Security ();

	void AddSystem ( SecuritySystem *newsystem, int index = -1 );
	void AddSystem ( int TYPE, int level, int index = -1 );
    
    void RemoveAllSystems ();

	SecuritySystem *GetSystem (int index);
	int NumSystems ();
	int NumRunningSystems ();

	// Use these to determine status of security system

	bool IsRunning			  ( int TYPE );
	bool IsRunning_Proxy	  ();
	bool IsRunning_Firewall   ();
	bool IsRunning_Encryption ();
	bool IsRunning_Monitor	  ();

	bool IsAnythingEnabled    ();
	bool IsAnythingDisabled   ();							// Returns true if any system is disabled

	// Common functions

	bool Load  ( FILE *file );			
	void Save  ( FILE *file );			
	void Print ();		
	void Update ();			
	
	char *GetID ();			

};


#endif

