
/*

    Revelation Lost Interface

    The player tried to use Revelation but failed
    He is now arrested, along with all of ARC


  */


#ifndef _included_revelationlostinterface_h
#define _included_revelationlostinterface_h

#include "mainmenu/mainmenuscreen.h"


class RevelationLostInterface : public MainMenuScreen
{

protected:

	time_t starttime;

protected:

	static void ReturnToMainMenuClick ( Button *button );

public:

	RevelationLostInterface ();
	~RevelationLostInterface ();

	void Create ();
	void Remove ();
	void Update ();
	bool IsVisible ();

	int  ScreenID ();			

};




#endif

