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
private:
	struct AnchorPosition
	{
		bool isOnRight = false;
		bool isOnTop = false;
	};

	AnchorPosition anchorPos;
	bool isHorizontal;

	void setCorrectPosition();
	void createAnchorGfx();
	void createBgGfx();

	std::vector<HD_UI_Button*>buttons;

protected:
	HD_UI_ButtonMenu() {}

public:
	const char *lastSelection;

	~HD_UI_ButtonMenu(){}

	//Public creation methods
	//Creates a Button Menu with only captions and no anchor; the font and buttonHeight are standard!
	HD_UI_ButtonMenu(const char* objectName, int index, std::vector<std::string> captions, HD_UI_Container *newParent);

	//Creates a Button Menu with only captions; the font is 24 by standard
	HD_UI_ButtonMenu(const char* objectName, int index, float buttonHeight, std::vector<std::string> captions, bool isAnchorHorizontal, HD_UI_Container *newParent);

	//Creates a Button Menu with icons on the buttons; the font is 24 by standard
	//The first element of the icons vector is the atlas!
	HD_UI_ButtonMenu(const char* objectName, int index, float buttonHeight, std::vector<std::string> captions, std::vector<std::string> iconNames,
		bool isAnchorHorizontal, HD_UI_Container *newParent);

	//General functions
	void Update();

	void defaultCallback();
	void setCallbacks(std::vector<std::function<void()>> newCallbacks);
	void setCallbacks(int index, std::function<void()> newCallback);

	void showMenu(bool show);

	HD_UI_Button* getButtonInList(int index) { return buttons[index]; }
};

#endif