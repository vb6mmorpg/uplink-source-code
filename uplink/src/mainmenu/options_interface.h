
/*

	Options Screen Interface	

  */

#ifndef _included_optionsinterface_h
#define _included_optionsinterface_h

#include "eclipse.h"

#include "mainmenu/mainmenuscreen.h"


class OptionsInterface : public MainMenuScreen
{

protected:
	
	static void GameOptionsClick		( Button *button );
	static void GraphicsOptionsClick	( Button *button );
	static void SoundOptionsClick		( Button *button );
	static void ThemeOptionsClick       ( Button *button );
    static void NetworkOptionsClick		( Button *button );

	static void ReturnToMainMenuClick	( Button *button );

public:

	OptionsInterface ();
	virtual ~OptionsInterface ();

	void Create ();
	void Remove ();
	void Update ();
	bool IsVisible ();

	int  ScreenID ();			

};

#endif
