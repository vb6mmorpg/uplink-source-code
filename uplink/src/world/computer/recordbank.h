

#ifndef _included_recordbank_h
#define _included_recordbank_h

/*

	Record Bank class

	Represents a bank of computer records
	ie a Database

  */

#include "app/uplinkobject.h"

class Record;

/*

  OP CODES

	=			:		Equals
	!			:		Not Equal

	+			:		Contains
	-			:		Does not contain


  */

#define RECORDBANK_NAME         "Name"
#define RECORDBANK_PASSWORD     "Password"
#define RECORDBANK_ACCNO        "Account Number"
#define RECORDBANK_SECURITY     "Security"
#define RECORDBANK_ADMIN        "admin"
#define RECORDBANK_READWRITE    "readwrite"
#define RECORDBANK_READONLY     "readonly"


class RecordBank : public UplinkObject
{

public:

	LList <Record *> records;

public:

	RecordBank ();
	~RecordBank ();

	// Data access functions

	void AddRecord ( Record *newrecord );
	void AddRecordSorted ( Record *newrecord, char *sortfield = RECORDBANK_NAME );

	Record *GetRecord			  ( int index );							// Returns NULL if not found
	Record *GetRecord			  ( char *query );							// Assumes there is only 1 match
	LList <Record *> *GetRecords  ( char *query );
	Record *GetRandomRecord		  ( char *query );
    
    Record *GetRecordFromName           ( char *name );
    Record *GetRecordFromNamePassword   ( char *name, char *password );
    Record *GetRecordFromAccountNumber  ( char *accNo );

	int FindNextRecordIndexNameNotSystemAccount ( int curindex = -1);


	// Common functions

	bool Load  ( FILE *file );			
	void Save  ( FILE *file );			
	void Print ();		
	void Update ();			
	
	char *GetID ();			
	int GetOBJECTID ();

private:
	char *MakeSafeField( char * fieldval );

};


// ============================================================================

/*
	Computer Record class
	Eg. A criminal record, a bank account, an uplink account etc

	  */


class Record : public UplinkObject  
{

public:

	BTree <char *> fields;

public:

	Record();
	virtual ~Record();

	void AddField    ( char *name, char *value );
	void AddField    ( char *name, int value );
	
	void ChangeField ( char *name, char *newvalue );
	void ChangeField ( char *name, int newvalue );

	char *GetField   ( char *name );	 
	void DeleteField ( char *name );

	// Common functions

	bool Load  ( FILE *file );			
	void Save  ( FILE *file );			
	void Print ();		
	void Update ();			
	
	char *GetID ();			
	int GetOBJECTID ();

};

#endif 
