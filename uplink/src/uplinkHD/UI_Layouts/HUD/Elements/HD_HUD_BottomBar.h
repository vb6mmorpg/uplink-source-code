// The bottom/task bar
//====================

#ifndef HD_HUD_BOTTOMBAR_H
#define HD_HUD_BOTTOMBAR_H

#include "../../../UI_Objects/HD_UI_Container.h"

class HD_HUD_BottomBar : public HD_UI_Container
{
private:
	//members

protected:
	//callbacks

public:
	HD_HUD_BottomBar();
	~HD_HUD_BottomBar() {}

	void Create();
	void Update();
};

#endif