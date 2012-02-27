
/*

  Generic Options Screen

	Picks out all options of a certain type
	and allows you to edit them

  */
  

#ifndef _included_genericoptionsinterface_h
#define _included_genericoptionsinterface_h

#include "eclipse.h"

#include "mainmenu/mainmenuscreen.h"




class GenericOptionsInterface : public MainMenuScreen
{

protected:

	char optionTYPE [16];

protected:

	static void ApplyClick ( Button *button );
	static void ReturnToMainMenuClick ( Button *button );

	static void ToggleBoxDraw ( Button *button, bool highlighted, bool clicked );
	static void ToggleBoxClick ( Button *button );

    static void ExitGameClick ( Button *button );

public:

	GenericOptionsInterface ();
	~GenericOptionsInterface ();

	void SetOptionTYPE ( char *newtype );						// eg GAME, SOUND etc
	void ChangeOptionValue ( char *option, int newvalue );

	void Create ();
	void Remove ();
	void Update ();
	bool IsVisible ();

	int  ScreenID ();		

};



#endif


