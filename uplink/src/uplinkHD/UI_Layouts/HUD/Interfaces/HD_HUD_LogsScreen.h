#ifndef HD_HUD_LOGSSCREEN_H
#define HD_HUD_LOGSSCREEN_H

#include "../../../UI_Objects/HD_UI_Container.h"

#include "world/computer/computerscreen/computerscreen.h"

class HD_HUD_LogsScreen : public HD_UI_Container
{
protected:
	void backClick(int nextPage);

public:
	HD_HUD_LogsScreen();
	~HD_HUD_LogsScreen() {}

	void Create(ComputerScreen* cScreen);
};

#endif