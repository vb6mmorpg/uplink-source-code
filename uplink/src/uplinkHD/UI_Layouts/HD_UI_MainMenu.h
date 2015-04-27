//======================================
// The Maine Menu Layout.
// This is the first screen that appears
// upon opening the game.
//======================================

#ifndef HD_UI_MAINMENU_H
#define HD_UI_MAINMENU_H

#include "../UI_Objects/HD_UI_Container.h"

class HD_UI_MainMenu : public HD_UI_Container
{
protected:
	//Callback Functions
	void pressedQuit();
	void Quit();

public:
	//Creation/Destruction
	HD_UI_MainMenu();
	~HD_UI_MainMenu() { }

	void Create();

};

#endif