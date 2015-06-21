//======================================
// The Maine Menu Layout.
// This is the first screen that appears
// upon opening the game.
//======================================

#ifndef HD_MAINMENU_H
#define HD_MAINMENU_H

#include "../UI_Objects/HD_UI_Container.h"
#include "../UI_Objects/HD_UI_ButtonInput.h"
#include "HD_MMOptions.h"
#include "HD_Loading.h"

class HD_MainMenu : public HD_UI_Container
{
private:
	std::shared_ptr<HD_MMOptions> options;

	bool userExists(const char* user);

protected:
	//Callback Functions
	void registerClicked();
	void submitClicked(HD_UI_ButtonInput *btnInUser);
	void optionsClicked();
	void exitClicked();
	void Quit();

public:
	//Creation/Destruction
	HD_MainMenu();
	~HD_MainMenu();

	void Create();
	void Clear();

};

#endif