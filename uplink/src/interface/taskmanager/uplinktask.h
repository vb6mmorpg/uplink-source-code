// UplinkTask.h: interface for the UplinkTask class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _included_uplinktask_h
#define _included_uplinktask_h

// ============================================================================

#include "task.h"
#include "app/uplinkobject.h"

// ============================================================================


class UplinkTask : public Task  
{

protected:

	UplinkObject *targetobject;
	char		 *targetstring;
	int			  targetint;
	
public:

	float version;
	bool followmouse;

public:

	UplinkTask();
	virtual ~UplinkTask();

	void SetVersion ( float newversion );
	void SetFollowMouse ( bool newfollowmouse );

	virtual void SetTarget ( UplinkObject *newtargetobject, char *newtargetstring, int targetint );
	virtual void MoveTo ( int x, int y, int time_ms );	
	
};

#endif 
