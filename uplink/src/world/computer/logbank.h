
/*
	Log Bank object

		represents a set of logs for a 
		particular computer

  */

#ifndef _included_logbank_h
#define _included_logbank_h

// ============================================================================

#include "app/uplinkobject.h"

#include "world/date.h"
#include "world/person.h"

class AccessLog;
	
// ============================================================================


class LogBank : public UplinkObject
{

public:

	DArray <AccessLog *> logs;
	DArray <AccessLog *> internallogs;						// Never delete from here

public:

	LogBank ();
	~LogBank ();

	void AddLog ( AccessLog *log, int index = -1 );			// Adds to both

	bool LogModified ( int index );							// Is the log in internallogs different to that in logs?

	char *TraceLog ( char *to_ip, char *logbank_ip, Date *date, int uplinkrating );		
															// ie source->logbank_ip->to_ip; lookup source and return (recursive)

    void Empty ();

	// Common functions

	bool Load  ( FILE *file );
	void Save  ( FILE *file );
	void Print ();
	void Update ();
	
	char *GetID ();	
	int GetOBJECTID ();

};


// ============================================================================


#define LOG_TYPE_DELETED			-1						// Undeleted version may exist in internallogs
#define LOG_TYPE_NONE				0
#define LOG_TYPE_TEXT				1						// Caption included in 'data1'
#define	LOG_TYPE_CONNECTIONOPENED   2						// Connection received from 'fromip'
#define LOG_TYPE_CONNECTIONCLOSED   3						// Connection closed from 'fromip'
#define LOG_TYPE_BOUNCEBEGIN		4						// Start point for a bounced call to ip 'data1'
#define LOG_TYPE_BOUNCE				5						// User bounces to ip 'data1'
#define LOG_TYPE_TRANSFERTO			6						// Transfer money to 'data1' [IP ACCNO], amount 'data2', person 'data3'
#define LOG_TYPE_TRANSFERFROM		7						// Transfer from     'data1' [IP ACCNO], amount 'data2', person 'data3']

/*
	Most logs are not suspicious.
	If a suspicious activity occurs, it becomes LOG_SUSPICIOUS.
	Every [few] hours, these logs are changed to NOTICEDANDSUSPICIOUS.
	[A few] hours later, they become LOG_UNDERINVESTIGATION and are dealt with
	They are then changed back to LOG_NOTSUSPICIOUS by the agent
	 
	This means you have between [few] and [few+few] hours before you are traced.

  */

#define LOG_NOTSUSPICIOUS		 0
#define LOG_SUSPICIOUS			 1
#define LOG_SUSPICIOUSANDNOTICED 2
#define LOG_UNDERINVESTIGATION	 3


class AccessLog : public UplinkObject  
{

public:

	int TYPE;

	Date date;											// Time of access
	char fromip [SIZE_VLOCATION_IP];					// The IP the access came from
	char fromname [SIZE_PERSON_NAME];					// The person who created the log

	int SUSPICIOUS;										// Was this a suspicious action?

	char *data1;										// Misc data
	char *data2;										// Check before Dereferencing.
	char *data3;										// Left foot before right.

public:

	AccessLog();
	virtual ~AccessLog();

	void SetProperties ( Date *newdate, char *newfromip, char *newfromname,
						 int newSUSPICIOUS = LOG_NOTSUSPICIOUS,
						 int newTYPE = LOG_TYPE_TEXT );
	
	void SetProperties ( AccessLog *copyme );

	void SetTYPE ( int newTYPE );
	void SetFromIP ( char *newfromip );
	void SetSuspicious ( int newSUSPICIOUS );

	void SetData1 ( char *newdata );
	void SetData2 ( char *newdata );
	void SetData3 ( char *newdata );

	char *GetDescription ();							// Must remember to delete result

	// Common functions

	bool Load  ( FILE *file );
	void Save  ( FILE *file );
	void Print ();
	
	char *GetID ();
	int   GetOBJECTID ();

};

#endif 
