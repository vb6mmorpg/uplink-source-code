
/*

  Graphic Options Screen

	Screen resolution settings etc

  */
  

#ifndef _included_graphicoptionsinterface_h
#define _included_graphicoptionsinterface_h

#include "eclipse.h"

#include "mainmenu/mainmenuscreen.h"




class GraphicOptionsInterface : public MainMenuScreen
{

protected:

	char optionTYPE [16];

protected:

	static void ApplyClick ( Button *button );
	static void ReturnToMainMenuClick ( Button *button );

	static void ToggleBoxDraw ( Button *button, bool highlighted, bool clicked );
	static void ToggleBoxClick ( Button *button );

    static void ScreenOptionDraw ( Button *button, bool highlighted, bool clicked );
    static void ScreenOptionClick ( Button *button );

    static void ExitGameClick ( Button *button );

    static int newScreenWidth;
    static int newScreenHeight;
    static int newColourDepth;
    static int newRefresh;
    static bool screenSettingsChanged;

public:

	GraphicOptionsInterface ();
	~GraphicOptionsInterface ();

	void SetOptionTYPE ( char *newtype );						// eg GAME, SOUND etc
	bool ChangeOptionValue ( char *option, int newvalue );      // Returns true if shutdown required

	void Create ();
	//void Remove ();
	void Update ();
	bool IsVisible ();

	int  ScreenID ();		

};



#endif

