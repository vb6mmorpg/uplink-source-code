//======================================
// The Maine Menu Layout.
// This is the first screen that appears
// upon opening the game.
//======================================

#ifndef HD_MAINMENU_H
#define HD_MAINMENU_H

#include "../UI_Objects/HD_UI_Container.h"
#include "HD_MMOptions.h"

class HD_MainMenu : public HD_UI_Container
{
private:
	HD_MMOptions *options;

protected:
	//Callback Functions
	void optionsClicked();
	void exitClicked();
	void Quit();

public:
	//Creation/Destruction
	HD_MainMenu();
	~HD_MainMenu() { }

	void Create();

};

#endif