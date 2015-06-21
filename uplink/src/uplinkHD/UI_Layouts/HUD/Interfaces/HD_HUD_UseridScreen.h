#ifndef HD_HUD_USERIDSCREEN_H
#define HD_HUD_USERIDSCREEN_H

#include "../../../UI_Objects/HD_UI_Container.h"

#include "../../../../world/computer/computerscreen/computerscreen.h"
#include "../../../../world/computer/computerscreen/useridscreen.h"

class HD_HUD_UseridScreen : public HD_UI_Container
{
protected:
	void submitClick(UserIDScreen* usrScreen);
	void codeClick(const char* codeCaption);
	void bypassClick(int nextPage);

public:
	HD_HUD_UseridScreen();
	~HD_HUD_UseridScreen() {}

	void Create(ComputerScreen* cScreen);
};

#endif