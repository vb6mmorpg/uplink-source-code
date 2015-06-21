//==============================
// The Links interface.
// Used by the Player's Gateway,
// InterNIC etc.
//==============================

#ifndef HD_HUD_LINKSSCREEN_H
#define HD_HUD_LINKSSCREEN_H

#include "../../../UI_Objects/HD_UI_Container.h"

#include "../../../../world/computer/computerscreen/computerscreen.h"
#include "../../../../world/computer/computerscreen/linksscreen.h"

class HD_HUD_LinksScreen : public HD_UI_Container
{
protected:
	void filterClick();

public:
	HD_HUD_LinksScreen();
	~HD_HUD_LinksScreen() {}

	void Create(ComputerScreen *cScreen);
	void CreatePlayerLinks(LinksScreen *cScreen);
	void CreateInterNICLinks(LinksScreen *cScreen);
	void CreateComputerLinks(LinksScreen *cScreen);
	void Update();
};

#endif