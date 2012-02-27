
/*

	Network Options Interface

  */

#ifndef _included_networkoptionsinterface_h
#define _included_networkoptionsinterface_h

#include "eclipse.h"

#include "mainmenu/mainmenuscreen.h"

class NetworkOptionsInterface : public MainMenuScreen
{

protected:

	static int oldNetworkStatus;

	static void CloseClick ( Button *button );

	static void ServerButtonClick ( Button *button );
	static void ClientButtonClick ( Button *button );

	static void StatusButtonDraw ( Button *button, bool highlighted, bool clicked );
	static void ServerButtonDraw ( Button *button, bool highlighted, bool clicked );
	static void ClientButtonDraw ( Button *button, bool highlighted, bool clicked );

public:

	NetworkOptionsInterface ();
	virtual ~NetworkOptionsInterface ();

	void Create ();
	void Remove ();
	void Update ();
	bool IsVisible ();

	int  ScreenID ();			

	bool ReturnKeyPressed ();

};

#endif
