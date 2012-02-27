
/*

  Demo Game Over Interface

	Appears when the demo has come to an end

  */


#ifndef _included_demogameoverinterface_h
#define _included_demogameoverinterface_h

#include "mainmenu/mainmenuscreen.h"


class DemoGameOverInterface : public MainMenuScreen
{

protected:

	time_t starttime;

protected:

	static void ReturnToMainMenuClick ( Button *button );

public:

	DemoGameOverInterface ();
	~DemoGameOverInterface ();

	void Create ();
	void Remove ();
	void Update ();
	bool IsVisible ();

	int  ScreenID ();			

};




#endif

