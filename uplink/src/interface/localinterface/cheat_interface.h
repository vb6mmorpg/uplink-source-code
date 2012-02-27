
/*

	Interface definition for cheat codes 

  */

#ifndef _included_cheatinterface_h
#define _included_cheatinterface_h


#include "interface/localinterface/localinterfacescreen.h"


class CheatInterface : public LocalInterfaceScreen  
{

public:

	static void TitleClick ( Button *button );

	static void AllLinksClick		( Button *button );
	static void AllSoftwareClick	( Button *button );
	static void AllHardwareClick	( Button *button );
	static void LotsOfMoneyClick	( Button *button );
	static void MaxRatingsClick		( Button *button );
	static void NextRatingClick		( Button *button );
	static void DebugPrintClick		( Button *button );
	static void EndGameClick		( Button *button );
	static void CancelTraceClick    ( Button *button );
	static void EventQueueClick		( Button *button );
    static void RevelationClick     ( Button *button );
	static void ShowLANClick		( Button *button );

public:

	void Create ();
	void Remove ();
	void Update ();
	bool IsVisible ();

	int ScreenID ();

};

#endif 
