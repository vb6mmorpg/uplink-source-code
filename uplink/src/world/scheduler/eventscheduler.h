
/*

  Event Scheduler class

	Responsible for keeping track of all future scheduled events,
	and running those events at the right time.

	It is not designed for high accuracy scheduling - events are run within 
	around 10 seconds of their target time.

	*/


#ifndef _included_eventscheduler_h
#define _included_eventscheduler_h


#include "tosser.h"

#include "app/uplinkobject.h"

#include "world/scheduler/uplinkevent.h"


class EventScheduler : public UplinkObject
{

public:

	LList <UplinkEvent *> events;

public:

	EventScheduler ();
	~EventScheduler ();

	void ScheduleEvent ( UplinkEvent *event );
	
	void ScheduleWarning ( UplinkEvent *event, Date *date );

	Date *GetDateOfNextEvent ();

	// Common functions

	bool Load  ( FILE *file );
	void Save  ( FILE *file );
	void Print ();
	void Update ();

	char *GetID ();

};


#endif
	
