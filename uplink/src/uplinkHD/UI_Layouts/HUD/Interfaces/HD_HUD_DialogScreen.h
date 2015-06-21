#ifndef HD_HUD_DIALOGSCREEN_H
#define HD_HUD_DIALOGSCREEN_H

#include "../../../UI_Objects/HD_UI_Container.h"

#include "../../../../world/computer/computerscreen/computerscreen.h"

class HD_HUD_DialogScreen : public HD_UI_Container
{
protected:
	void NextPageClick(int nextPage);
	void ScriptButtonClick(int scriptId, int nextPage);

public:
	HD_HUD_DialogScreen();
	~HD_HUD_DialogScreen() {}

	void Create(ComputerScreen *cScreen);
};

#endif