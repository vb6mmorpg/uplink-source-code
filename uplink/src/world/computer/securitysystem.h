
/*

  Security System class object

	Represents a single security device on a remote computer
	eg Firewall, Proxy etc

	Part of the World / Computer subsystem

  */



#ifndef _included_securitysystem_h
#define _included_securitysystem_h

// ============================================================================

#include "app/uplinkobject.h"

// Do not renumber these to be ^2 - eg 1 2 4 8
// As they are used as indexes into an array

#define SECURITY_TYPE_NONE			0
#define SECURITY_TYPE_PROXY			1					// Stops data changes
#define SECURITY_TYPE_FIREWALL		2					// Stops file access
#define SECURITY_TYPE_ENCRYPTION	3					// Stops human readability
#define SECURITY_TYPE_MONITOR		4					// Checks for attacks


// ============================================================================



class SecuritySystem : public UplinkObject
{

public:

	int TYPE;
	int level;								// Difficulty level

	bool enabled;	
	bool bypassed;

public:

	SecuritySystem ();
	~SecuritySystem ();

	void SetTYPE ( int newTYPE );
	void SetLevel ( int newlevel );

	void Enable ();
	void Disable ();

	void Bypass ();
	void EndBypass ();

	char *GetName ();

	// Common functions

	bool Load  ( FILE *file );			
	void Save  ( FILE *file );			
	void Print ();		
	void Update ();			
	
	char *GetID ();			
	int GetOBJECTID ();

};


#endif

