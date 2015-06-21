#ifndef HD_HUD_MESSAGESCREEN_H
#define HD_HUD_MESSAGESCREEN_H

#include "../../../UI_Objects/HD_UI_Container.h"

#include "../../../../world/computer/computerscreen/messagescreen.h"

class HD_HUD_MessageScreen : public HD_UI_Container
{
protected:
	void MailMeClick(MessageScreen *cScreen);
	void OKClick(MessageScreen *cScreen);

public:
	HD_HUD_MessageScreen();
	~HD_HUD_MessageScreen() {}

	void Create(ComputerScreen *cScreen);
};

#endif