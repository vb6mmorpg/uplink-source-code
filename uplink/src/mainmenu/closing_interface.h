
/*

  Closing interface

    Shows an advert as Uplink closes down

  */


#ifndef _included_closinginterface_h
#define _included_closinginterface_h


#include "mainmenu/mainmenuscreen.h"


class ClosingInterface : public MainMenuScreen
{

protected:

    static void LargeTextDraw ( Button *button, bool highlighted, bool clicked );

public:

    int timesync;
    int status;
    int count;

public:

    ClosingInterface ();
    ~ClosingInterface ();

	void Create ();
	void Remove ();
	void Update ();
	bool IsVisible ();

	int  ScreenID ();			

};


#endif

