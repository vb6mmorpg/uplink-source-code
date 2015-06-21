#ifndef HD_HUD_MENUSCREEN_H
#define HD_HUD_MENUSCREEN_H

#include "../../../UI_Objects/HD_UI_Container.h"
#include "../../../../world/computer/computerscreen/computerscreen.h"

class HD_HUD_MenuScreen : public HD_UI_Container
{
protected:
	void MenuItemClick(int nextPage);

public:
	HD_HUD_MenuScreen();
	~HD_HUD_MenuScreen() {}

	void Create(ComputerScreen* cScreen);
};

#endif