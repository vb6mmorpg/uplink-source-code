#ifndef HD_HUD_DISCONNECTSCREEN_H
#define HD_HUD_DISCONNECTSCREEN_H

#include "../../../UI_Objects/HD_UI_Container.h"

#include "../../../../world/computer/computerscreen/computerscreen.h"

class HD_HUD_DisconnectScreen : public HD_UI_Container
{
public:
	HD_HUD_DisconnectScreen();
	~HD_HUD_DisconnectScreen() {}

	void Create(ComputerScreen* cScreen);

protected:
	void buttonClick(int nextScreen);
};

#endif