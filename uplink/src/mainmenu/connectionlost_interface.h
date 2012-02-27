
/*

  Connection Lost Interface

	Appears after your connection from your home PC to your Gateway is lost
	after you nuke it usually

  */


#ifndef _included_connectionlostinterface_h
#define _included_connectionlostinterface_h

#include "mainmenu/mainmenuscreen.h"


class ConnectionLostInterface : public MainMenuScreen
{

protected:

	time_t starttime;

protected:

	static void ReturnToMainMenuClick ( Button *button );

public:

	ConnectionLostInterface ();
	~ConnectionLostInterface ();

	void Create ();
	void Remove ();
	void Update ();
	bool IsVisible ();

	int  ScreenID ();			

};




#endif

