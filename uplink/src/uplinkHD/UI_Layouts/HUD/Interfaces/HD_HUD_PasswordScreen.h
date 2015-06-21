#ifndef HD_HUD_PASSWORDSCREEN_H
#define HD_HUD_PASSWORDSCREEN_H

#include "../../../UI_Objects/HD_UI_Container.h"

#include "../../../../world/computer/computerscreen/computerscreen.h"
#include "../../../../world/computer/computerscreen/passwordscreen.h"

class HD_HUD_PasswordScreen : public HD_UI_Container
{
protected:
	void submitClick(PasswordScreen* passScreen);
	void codeClick(const char* code);
	void bypassClick(int nextPage);

public:
	HD_HUD_PasswordScreen();
	~HD_HUD_PasswordScreen() {}

	void Create(ComputerScreen* cScreen);
};


#endif