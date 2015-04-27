// Implementation of the Button Menu
//==================================

#include "HD_UI_ButtonMenu.h"
#include "../HD_Resources.h"

//Creates a Button Menu with only captions; the font is 24 by standard
HD_UI_ButtonMenu::HD_UI_ButtonMenu(char* objectName, int index, float buttonHeight, std::vector<std::string> captions, bool isAnchorHorizontal, HD_UI_Container *newParent)
{
	//first get the maximum size
	float maxWidth = 0.0f;
	float maxHeight = buttonHeight * captions.size();

	for (unsigned int ii = 0; ii < captions.size(); ii++)
	{
		float w = al_get_text_width(HDResources->font24, captions[ii].c_str());
		maxWidth = w > maxWidth ? w : maxWidth;
	}

	maxWidth += 25.0f;

	setObjectProperties(objectName, 0.0f, 0.0f, maxWidth, maxHeight, newParent, index);
}

//Creates a Button Menu with icons on the buttons; the font is 24 by standard
//The first element of the icons vector is the atlas!
HD_UI_ButtonMenu::HD_UI_ButtonMenu(char* objectName, int index, float buttonHeight, std::vector<std::string> captions, std::vector<std::string> iconNames,
	bool isAnchorHorizontal, HD_UI_Container *newParent)
{

}

//General functions
void HD_UI_ButtonMenu::setCallbacks(std::vector<std::function<void()>> newCallbacks)
{

}

void HD_UI_ButtonMenu::showMenu(bool show)
{

}