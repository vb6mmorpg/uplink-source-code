
/*

  Disavowed Interface

	Appears after your connection from your home PC to your Gateway is lost
	Because you have been disavowed

  */


#ifndef _included_disavowedinterface_h
#define _included_disavowedinterface_h

#include "mainmenu/mainmenuscreen.h"


class DisavowedInterface : public MainMenuScreen
{

protected:

	time_t starttime;

protected:

	static void ReturnToMainMenuClick ( Button *button );

public:

	DisavowedInterface ();
	~DisavowedInterface ();

	void Create ();
	void Remove ();
	void Update ();
	bool IsVisible ();

	int  ScreenID ();			

};




#endif

