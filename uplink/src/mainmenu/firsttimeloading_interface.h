
/*

	Loading Screen Interface
	For first time players
	
  */

#ifndef _included_firsttimeloadinginterface_h
#define _included_firsttimeloadinginterface_h

#include "mainmenu/mainmenuscreen.h"


class FirstTimeLoadingInterface : public MainMenuScreen
{

public:

	FirstTimeLoadingInterface ();
	virtual ~FirstTimeLoadingInterface ();

	void Create ();
	void Remove ();
	void Update ();
	bool IsVisible ();

	int  ScreenID ();			

};

#endif
