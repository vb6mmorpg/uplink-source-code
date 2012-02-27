
/*

  Event Queue Interface

	Debug screen for watching the Event queue

  */


#ifndef _included_evtqueueinterface_h
#define _included_evtqueueinterface_h

// ============================================================================

#include "eclipse.h"

#include "interface/localinterface/localinterfacescreen.h"

// ============================================================================


class EventQueueInterface : public LocalInterfaceScreen
{

protected:

	static int baseoffset;
	static int previousupdate;

protected:

	static void TitleClick ( Button *button );
	static void ScrollUpClick   ( Button *button );
	static void ScrollDownClick ( Button *button );

	static void EventDraw ( Button *button, bool highlighted, bool clicked );
	static void EventClick ( Button *button );

	static void DeleteEventDraw ( Button *button, bool highlighted, bool clicked );
	static void DeleteEventClick ( Button *button );
    
	static void FullDetailsCloseClick ( Button *button );

public:

	void Create ();
	void Remove ();
	void Update ();
	bool IsVisible ();

	int ScreenID ();

};


#endif

