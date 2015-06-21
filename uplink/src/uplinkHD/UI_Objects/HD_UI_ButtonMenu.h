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

	std::vector<std::shared_ptr<HD_UI_Button>>buttons;
	std::string lastSelectionName;
	unsigned int lastSelectionID = 0;

public:
	HD_UI_ButtonMenu() {}
	~HD_UI_ButtonMenu(){}

	//Public creation methods

	//Creates a Button Menu with only captions and no anchor; the font and buttonHeight are standard!
	void CreateButtonMenu(const char* objectName, float fX, float fY, float maxWidth, std::vector<std::string> captions);

	//Creates a Button Menu with only captions; the font is 24 by standard
	void CreateAnchoredButtonMenu(const char* objectName, float buttonHeight, std::vector<std::string> captions,
		bool isAnchorHorizontal = false);

	//Creates a Button Menu with icons on the buttons; the font is 24 by standard
	//The first element of the icons vector is the atlas!
	void CreateAnchoredButtonWIconMenu(const char* objectName, float buttonHeight, std::vector<std::string> captions, std::vector<std::string> iconNames,
		bool isAnchorHorizontal = false);

	//General functions
	void Update();

	void defaultCallback(unsigned int id);
	void setCallbacks(std::vector<std::function<void()>> newCallbacks);
	void setCallbacks(int index, std::function<void()> newCallback);

	void showMenu(bool show);

	std::shared_ptr<HD_UI_Button> getButtonInList(int index) { return buttons[index]; }
};

#endif