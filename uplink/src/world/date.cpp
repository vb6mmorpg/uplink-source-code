// Date.cpp: implementation of the Date class.
//
//////////////////////////////////////////////////////////////////////

#include "gucci.h"
#include "eclipse.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/serialise.h"

#include "game/game.h"

#include "world/world.h"
#include "world/date.h"
#include "world/player.h"
#include "world/company/company.h"

#include "world/generator/numbergenerator.h"


#include "mmgr.h"

static char monthname [][12] = 
		{ "January",   "February", "March",    "April",
		  "May",	   "June",	   "July",     "August",
		  "September", "October",  "November", "December" };

static char tempdate [SIZE_DATE_LONG];                              // Used to return new strings


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Date::Date()
{

	second = 1;
	minute = 1;
	hour = 1;
	day = 1;
	month = 1;
	year = 1000;

	previousupdate = 0;

	updateme = false;

}

Date::Date ( int newsecond, int newminute, int newhour, int newday, int newmonth, int newyear )
{

	SetDate ( newsecond, newminute, newhour, newday, newmonth, newyear );

	previousupdate = 0;

}

Date::~Date()
{

}

void Date::SetDate ( Date *copydate )
{

	UplinkAssert (copydate);
	SetDate ( copydate->GetSecond (), copydate->GetMinute (), copydate->GetHour (), copydate->GetDay (), copydate->GetMonth (), copydate->GetYear () );

}

void Date::SetDate ( int newsecond, int newminute, int newhour, int newday, int newmonth, int newyear )
{

	second = newsecond;
	minute = newminute;
	hour = newhour;
	day = newday;
	month = newmonth;
	year = newyear;

	AdvanceSecond ( 0 );					// Do this to "roll over" any dodgy times

}

void Date::Activate ()
{

	updateme = true;

}

void Date::DeActivate ()
{

	updateme = false;

}

bool Date::Before ( Date *date )
{

	return ( ( year <  date->year ) || 
			 ( year == date->year && month < date->month ) ||
			 ( year == date->year && month == date->month && day < date->day ) ||
			 ( year == date->year && month == date->month && day == date->day && hour < date->hour ) ||
			 ( year == date->year && month == date->month && day == date->day && hour == date->hour && minute < date->minute ) || 
			 ( year == date->year && month == date->month && day == date->day && hour == date->hour && minute == date->minute && second < date->second ) );

}

bool Date::After ( Date *date )
{

	return ( ( year >  date->year ) || 
		     ( year == date->year && month > date->month ) ||
		     ( year == date->year && month == date->month && day > date->day ) ||
		     ( year == date->year && month == date->month && day == date->day && hour > date->hour ) ||
		     ( year == date->year && month == date->month && day == date->day && hour == date->hour && minute > date->minute ) ||
			 ( year == date->year && month == date->month && day == date->day && hour == date->hour && minute == date->minute && second > date->second ) );

}

bool Date::Equal ( Date *date )
{

	return ( year   == date->year   &&
			 month  == date->month  &&
			 day    == date->day    &&
			 hour   == date->hour   &&
			 minute == date->minute &&
			 second == date->second );

}

void Date::AdvanceSecond ( int n )
{

	second += n;

	if ( second > 59 ) {
		int numminutes = 1 + ( second - 60 ) / 60;
		second = second % 60;
		AdvanceMinute ( numminutes );
	}

	if ( second < 0 ) {
		int numminutes = 1 + ( second * -1 - 1 ) / 60;
		second = second + ( 60 * numminutes );
		AdvanceMinute ( numminutes * -1 );
	}

}

void Date::AdvanceMinute ( int n )
{

	minute += n;

	if ( minute > 59 ) {
		int numhours = 1 + ( minute - 60 ) / 60;
		minute = minute % 60;
		AdvanceHour ( numhours );
	}

	if ( minute < 0 ) {
		int numhours = 1 + ( minute * -1 - 1 ) / 60;
		minute = minute + ( 60 * numhours );
		AdvanceHour ( numhours * -1 );
	}

}

void Date::AdvanceHour   ( int n )
{

	hour += n;

	if ( hour > 23 ) {
		int numdays = 1 + ( hour - 24 ) / 24;
		hour = hour % 24;
		AdvanceDay ( numdays );
	}

	if ( hour < 0 ) {
		int numdays = 1 + ( hour * -1 - 1 ) / 24;
		hour = hour + ( 24 * numdays );
		AdvanceDay ( numdays * -1 );
	}

}

void Date::AdvanceDay    ( int n )
{

	day += n;

	if ( day > 30 ) {
		int nummonths = 1 + ( day - 30 ) / 30;
		day = day % 30;
		AdvanceMonth ( nummonths );
	}

	if ( day < 1 ) {
		int nummonths = 1 + ( day * -1 ) / 30;
		day = day + ( 30 * nummonths );
		AdvanceMonth ( nummonths * -1 );
	}

}

void Date::AdvanceMonth  ( int n )
{

	month += n;

	if ( month > 12 ) {
		int numyears = 1 + ( month - 12 ) / 12;
		month = month % 12 ;
		AdvanceYear ( numyears );
	}

	if ( month < 1 ) {
		int numyears = 1 + ( month * -1 ) / 12;
		month = month + ( 12 * numyears );
		AdvanceYear ( numyears * -1 );
	}

}

void Date::AdvanceYear   ( int n )
{

	year += n;

}

int Date::GetSecond ()
{
	return second;
}

int Date::GetMinute ()
{
	return minute;
}

int Date::GetHour ()
{
	return hour;
}

int Date::GetDay ()
{
	return day;
}

int Date::GetMonth ()
{
	return month;
}

int Date::GetYear ()
{
	return year;
}

char *Date::GetMonthName ( int month )
{

	UplinkAssert ( month > 0 && month < 13 );
	return monthname [month-1];

}

char *Date::GetShortString ()
{

	char result [SIZE_DATE_SHORT];
	UplinkSnprintf ( result, sizeof ( result ), "%.2d:%.2d, %d-%d-%d", hour, minute, day, month, year ); 
	UplinkStrncpy ( tempdate, result, sizeof ( tempdate ) );
    return tempdate;

}

char *Date::GetLongString ()
{

	char result [SIZE_DATE_LONG];
	UplinkSnprintf ( result, sizeof ( result ), "%.2d:%.2d.%.2d, %d %s %d", hour, minute, second, day, GetMonthName (month), year ); 
    UplinkStrncpy ( tempdate, result, sizeof ( tempdate ) );
	return tempdate;

}


bool Date::Load ( FILE *file )
{

	LoadID ( file );

	if ( !FileReadData ( &second, sizeof (second), 1, file ) ) return false;
	if ( !FileReadData ( &minute, sizeof (minute), 1, file ) ) return false;
	if ( !FileReadData ( &hour,   sizeof (hour),   1, file ) ) return false;
	if ( !FileReadData ( &day,    sizeof (day),    1, file ) ) return false;
	if ( !FileReadData ( &month,  sizeof (month),  1, file ) ) return false;
	if ( !FileReadData ( &year,   sizeof (year),   1, file ) ) return false;

	if ( !FileReadData ( &updateme, sizeof (updateme), 1, file ) ) return false;

	LoadID_END ( file );

	return true;

}

void Date::Save ( FILE *file )
{

	SaveID ( file );

	fwrite ( &second, sizeof (second), 1, file );
	fwrite ( &minute, sizeof (minute), 1, file );
	fwrite ( &hour,   sizeof (hour),   1, file );
	fwrite ( &day,    sizeof (day),    1, file );
	fwrite ( &month,  sizeof (month),  1, file );
	fwrite ( &year,   sizeof (year),   1, file );

	fwrite ( &updateme, sizeof (updateme), 1, file );

	SaveID_END ( file );

}

void Date::Print ()
{

	printf ( "Date: " );
	printf ( "%d:%d:%d, %d/%d/%d\n", hour, minute, second, day, month, year ); 
	if ( updateme ) printf ( "Syncronised with real world time\n" );
	else			printf ( "Not Syncronised with real world time\n" );

}

void Date::Update ()
{

	if ( updateme ) {									// Update the time

		int nummilliseconds = (int)EclGetAccurateTime () - previousupdate;
		
		if ( game->GameSpeed () == GAMESPEED_NORMAL ) {
		
			if ( nummilliseconds > 1000 ) {
				
				AdvanceSecond ( 1 );				
				previousupdate = (int)EclGetAccurateTime ();

			}
				
		}
		else {

			if ( nummilliseconds > 150 ) {

				Date newdate;
				newdate.SetDate ( this );
				
				switch ( game->GameSpeed () ) {

					case GAMESPEED_FAST:

						newdate.AdvanceMinute ( 5 );
						newdate.AdvanceSecond ( 3 );
						break;

					case GAMESPEED_MEGAFAST:

						newdate.AdvanceHour ( 1 );
						newdate.AdvanceMinute ( 5 );
						newdate.AdvanceSecond ( 3 );
						break;

					case GAMESPEED_OHMYGOD:

						newdate.AdvanceHour ( 24 );
						newdate.AdvanceMinute ( 3 );
						newdate.AdvanceSecond ( 3 );
						break;

					default:

						UplinkAbort ( "Unrecognised Game Speed" );

				}

				Date *nextevent = game->GetWorld ()->scheduler.GetDateOfNextEvent ();
				
				if ( nextevent && nextevent->Before (&newdate) ) {
					nextevent->AdvanceSecond ( 1 );
					SetDate ( nextevent->GetSecond () + 1, nextevent->GetMinute (), nextevent->GetHour (),
							  nextevent->GetDay (), nextevent->GetMonth (), nextevent->GetYear () );
				}
				else
					SetDate ( &newdate );

				previousupdate = (int)EclGetAccurateTime ();

			}

		}
		
	}

#ifdef _DEBUG

	// Verify Date is valid

	UplinkAssert ( second >= 0 && second < 60 );
	UplinkAssert ( minute >= 0 && minute < 60 );
	UplinkAssert ( hour   >= 0 && hour   < 24 );
	
	UplinkAssert ( day    >  0 && day    <= 31 );
	UplinkAssert ( month  >  0 && month  <= 12 );
	UplinkAssert ( year   >  0 );

#endif

}

char *Date::GetID ()
{

	return "DATE";

}

