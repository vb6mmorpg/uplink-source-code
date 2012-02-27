
/*

  Warez Game Over screen

    We've detected you are A FUCKING PIRATING MUTHERFUCKER
    SO FUCK OFF 

  */


#ifndef _included_warezgameoverinterface_h
#define _included_warezgameoverinterface_h

#include "mainmenu/mainmenuscreen.h"


class WarezGameOverInterface : public MainMenuScreen
{

protected:

	time_t starttime;

protected:

	static void ReturnToMainMenuClick ( Button *button );

public:

	WarezGameOverInterface ();
	~WarezGameOverInterface ();

	void Create ();
	void Remove ();
	void Update ();
	bool IsVisible ();

	int  ScreenID ();			

};




#endif

