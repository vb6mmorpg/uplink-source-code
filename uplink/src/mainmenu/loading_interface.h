
/*

	Loading Screen Interface
	
  */

#ifndef _included_loadinginterface_h
#define _included_loadinginterface_h


#include "mainmenu/mainmenuscreen.h"


class LoadingInterface : public MainMenuScreen
{

public:

	LoadingInterface ();
	virtual ~LoadingInterface ();

	void Create ();
	void Remove ();
	void Update ();
	bool IsVisible ();

	int  ScreenID ();			

};

#endif
