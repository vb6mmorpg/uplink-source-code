
/*

  Person class object
  Part of the World sub-system

	Defines a non-player-character in the Uplink World
	including his physical behavoir, attributes, skills etc.

	The player is also a Person.

  */


#ifndef _included_person_h
#define _included_person_h

// ============================================================================

#include "app/uplinkobject.h"

#include "world/rating.h"
#include "world/connection.h"

#define SIZE_PERSON_NAME	 128

#define SIZE_VLOCATION_IP  24

#define PERSON_STATUS_NONE			0
#define PERSON_STATUS_INJAIL		1
#define PERSON_STATUS_DEAD			2


class Message;
class VLocation;

// ============================================================================


class Person : public UplinkObject  
{

public:

	char name	   [SIZE_PERSON_NAME];
	int age;

	int photoindex;							// Which picture to use
    int voiceindex;                         // Which voice samples to use

	char localhost  [SIZE_VLOCATION_IP];
	char remotehost [SIZE_VLOCATION_IP];
	char phonenumber [SIZE_VLOCATION_IP];

	LList <Message *> messages;
	LList <char *> accounts;				// List of [IP ACCNO]
	int currentaccount;						// Account for all current transfers

	Connection connection;
	Rating rating;
	
	bool istargetable;						// Can I be targeted by Random missions?

private:

	int STATUS;								// Bit field - see above

public:

	Person ();
	virtual ~Person ();

	void SetName	    ( char *newname );
	void SetAge	        ( int newage );
	void SetPhotoIndex  ( int newindex );
    void SetVoiceIndex  ( int newindex );
	void SetLocalHost   ( char *newip );
	void SetRemoteHost  ( char *newip );
	void SetPhoneNumber ( char *newphone );	
	void SetStatus	    ( int newSTATUS );
	int  GetStatus	    ( );

	virtual void GiveMessage ( Message *message );

	bool IsConnected ();								// true if localhost != remotehost
	Connection *GetConnection ();
	VLocation *GetLocalHost ();	
	VLocation *GetRemoteHost ();						

	virtual int  CreateNewAccount ( char *bankip, char *accname, char *password, 
									int balance, int loan );						// Returns account number	

	int  GetBalance ();																// Overall bank balance
	void ChangeBalance ( int amount, char *description );
	void SetCurrentAccount ( int index );

	void SetIsTargetable ( bool newvalue );

	bool HasMessageLink ( const char *ip );

	// Common functions

	virtual bool Load  ( FILE *file );
	virtual void Save  ( FILE *file );
	virtual void Print ();
	virtual void Update ();
	
	virtual char *GetID ();
	virtual int   GetOBJECTID ();

};

#endif 
