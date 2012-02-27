
/*

	Login Interface

	Essentially the first main menu - Load New Exit etc

  */

#ifndef _included_logininterface_h
#define _included_logininterface_h

#include "eclipse.h"

#include "mainmenu/mainmenuscreen.h"


class LoginInterface : public MainMenuScreen
{

protected:

	static void CreateExistingGames ();
	static void RemoveExistingGames ();

	static void NewGameClick     ( Button *button );
	static void RetireAgentClick ( Button *button );
	static void RetireAgentMsgboxClick ( Button *button );
	static void ProceedClick     ( Button *button );
	static void OptionsClick     ( Button *button );
	static void ExitGameClick    ( Button *button );
	
	static void CommsClick	     ( Button *button );
	static void StatusClick	     ( Button *button );

	static void LargeTextBoxDraw ( Button *button, bool highlighted, bool clicked );
	static void UserIDButtonDraw ( Button *button, bool highlighted, bool clicked );
	static void CodeButtonDraw   ( Button *button, bool highlighted, bool clicked );

	static void UserNameClick    ( Button *button );

public:

	LoginInterface ();
	virtual ~LoginInterface ();

	void Create ();
	void Remove ();
	void Update ();
	bool IsVisible ();

	int  ScreenID ();			

	bool ReturnKeyPressed ();

};

#endif
