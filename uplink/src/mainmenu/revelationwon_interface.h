
/*

    Revelation Won Interface

    Revelation has taken over the Internet and totally destroyed it!


  */


#ifndef _included_revelationwoninterface_h
#define _included_revelationwoninterface_h

#include "mainmenu/mainmenuscreen.h"


class RevelationWonInterface : public MainMenuScreen
{

protected:

	time_t starttime;

protected:

	static void ReturnToMainMenuClick ( Button *button );

public:

	RevelationWonInterface ();
	~RevelationWonInterface ();

	void Create ();
	void Remove ();
	void Update ();
	bool IsVisible ();

	int  ScreenID ();			

};




#endif

