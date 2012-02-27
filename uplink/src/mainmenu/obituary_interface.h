
/*

  Obituary Interface

	Allows you to view the final condition of a
	fallen agent

  */

#ifndef _included_obituaryinterface_h
#define _included_obituaryinterface_h

#include "mainmenu/mainmenuscreen.h"


class ObituaryInterface : public MainMenuScreen
{

protected:

	static void MediumTextDraw ( Button *button, bool highlighted, bool clicked );
	static void LargeTextDraw  ( Button *button, bool highlighted, bool clicked );

	static void ReturnToMainMenuClick ( Button *button );

	static void PiClick ( Button *button );

public:

	ObituaryInterface ();
	~ObituaryInterface ();

	void Create ();
	void Remove ();
	void Update ();
	bool IsVisible ();

	int  ScreenID ();			

};

#endif
