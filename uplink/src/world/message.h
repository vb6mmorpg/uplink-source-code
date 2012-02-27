
/*

  Message class object

	Stores the data representing a message (system message, email)

  */


#ifndef _included_message_h
#define _included_message_h

// ============================================================================

#include "app/uplinkobject.h"

#include "world/person.h"
#include "world/date.h"

class Data;

// ============================================================================


class Message : public UplinkObject  
{

protected:

	char *subject;
	char *body;

	Data *data;							// An attached file/program

public:

	char to		 [SIZE_PERSON_NAME];
	char from    [SIZE_PERSON_NAME];

	LList <char *> links;				// All useful ip addresses (to be added to links)
	BTree <char *> codes;				// Access codes, indexed on ip

	Date date;							// Date of send

public:

	Message();
	virtual ~Message();

	void SetTo		( char *newto );
	void SetFrom	( char *newfrom );
	void SetSubject ( char *newsubject );
	void SetBody	( char *newbody );
	void SetDate    ( Date *newdate );

	void GiveLink ( char *ip );
	void GiveCode ( char *ip, char *code );

	void AttachData ( Data *newdata );
	Data *GetData ();

	char *GetSubject ();
	char *GetBody	 ();

	void Send ();


	// Common functions

	bool Load  ( FILE *file );
	void Save  ( FILE *file );
	void Print ();
	
	char *GetID ();
	int   GetOBJECTID ();

};

#endif
