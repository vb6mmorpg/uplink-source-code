//========================================
// This creates a list menu filled with
// buttons (ex: right-click menu)
//========================================

#ifndef HD_UI_BUTTONMENU_H
#define HD_UI_BUTTONMENU_H

#include "HD_UI_Container.h"
#include "HD_UI_Button.h"

class HD_UI_ButtonMenu : public HD_UI_Container
{
protected:
	HD_UI_ButtonMenu() {}

public:
	~HD_UI_ButtonMenu(){}

	//Public creation method
	//Creates a Button Menu with only captions; the font is 24 by standard
	HD_UI_ButtonMenu(char* objectName, int index, float buttonHeight, std::vector<std::string> captions, bool isAnchorHorizontal, HD_UI_Container *newParent);

	//Creates a Button Menu with icons on the buttons; the font is 24 by standard
	//The first element of the icons vector is the atlas!
	HD_UI_ButtonMenu(char* objectName, int index, float buttonHeight, std::vector<std::string> captions, std::vector<std::string> iconNames,
		bool isAnchorHorizontal, HD_UI_Container *newParent);

	//General functions
	//void Update();

	void setCallbacks(std::vector<std::function<void()>> newCallbacks);
	void showMenu(bool show);
};

#endif