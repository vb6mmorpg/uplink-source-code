
/*

  Theme Interface

	Allows the player to preview and select
    themes from those available

  */
  

#ifndef _included_themeinterface_h
#define _included_themeinterface_h

#include "eclipse.h"

#include "mainmenu/mainmenuscreen.h"




class ThemeInterface : public MainMenuScreen
{

protected:

	static void ApplyClick ( Button *button );
	static void DefaultClick ( Button *button );
    static void ReturnToMainMenuClick ( Button *button );

    static void ThemeNameDraw ( Button *button, bool highlighted, bool clicked );
    static void ThemeNameClick ( Button *button );

    static DArray <char *> *ListAvailableThemes ();
    static DArray <char *> *themes;

    static int currentSelect;

public:

	ThemeInterface ();
	~ThemeInterface ();

	void Create ();
	void Remove ();
	void Update ();
	bool IsVisible ();

	int  ScreenID ();		

};



#endif

