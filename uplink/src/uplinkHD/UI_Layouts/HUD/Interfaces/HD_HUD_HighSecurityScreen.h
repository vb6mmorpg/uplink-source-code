#ifndef HD_HUD_HIGHSECURITYSCREEN_H
#define HD_HUD_HIGHSECURITYSCREEN_H

#include "../../../UI_Objects/HD_UI_Container.h"

#include "world/computer/computerscreen/computerscreen.h"

class HD_HUD_HighSecurityScreen : public HD_UI_Container
{
public:
	HD_HUD_HighSecurityScreen();
	~HD_HUD_HighSecurityScreen() {}

	void Create(ComputerScreen *cScreen);
};

#endif