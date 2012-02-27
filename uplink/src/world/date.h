
/*

  Date class object

	Represents the date and time in the Uplink world

  */


#ifndef _included_date_h
#define _included_date_h

#include <stdio.h>

#include "app/uplinkobject.h"


#define SIZE_DATE_SHORT    32
#define SIZE_DATE_LONG     64


class Date : public UplinkObject
{

protected:

	int second;
	int minute;
	int hour;
	int day;
	int month;
	int year;

	int previousupdate;

	bool updateme;						// Should the time be advanced each update?

public:

	Date();
	Date ( int newsecond, int newminute, int newhour,
		   int newday, int newmonth, int newyear );

	virtual ~Date();

	void SetDate ( int newsecond, int newminute, int newhour,
				   int newday, int newmonth, int newyear );
	void SetDate ( Date *copydate );

	void Activate ();					// Update me
	void DeActivate ();					// Don't update me

	bool Before ( Date *date );        // true if this.date < date
	bool After  ( Date *date );        // true if this.date > date
	bool Equal  ( Date *date );        // true if this.date = date

	void AdvanceSecond ( int n );		// These can be used
	void AdvanceMinute ( int n );		// to subtract time
	void AdvanceHour   ( int n );		// as well
	void AdvanceDay    ( int n );
	void AdvanceMonth  ( int n );
	void AdvanceYear   ( int n );

	int GetSecond ();
	int GetMinute ();
	int GetHour ();
	int GetDay ();
	int GetMonth ();
	int GetYear ();

	static char *GetMonthName ( int month );
	
	char *GetShortString ();       //  hh:mm, dd:mm:yyyy
	char *GetLongString ();		   //  hh:mm:ss, dd:month:yyyy

	// Common functions

	bool Load  ( FILE *file );
	void Save  ( FILE *file );
	void Print ();
	void Update ();

	char *GetID ();

};

#endif 
