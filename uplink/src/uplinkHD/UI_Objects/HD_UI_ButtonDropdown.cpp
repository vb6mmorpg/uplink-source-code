// Implementation of the button dropdown
//======================================

#include "HD_UI_ButtonDropdown.h"

#include "../HD_Screen.h"

//button creation; the first caption in the vector is the initial one on the button
//HD_UI_ButtonDropdown::HD_UI_ButtonDropdown(const char *objectName, int index, std::vector<std::string> captions, const char *tooltip, float fX, float fY,
//	float fWidth, float fHeight, ALLEGRO_COLOR colors[6], bool isFilled, ALLEGRO_FONT *captionFont, HD_UI_Container *newParent)
void HD_UI_ButtonDropdown::CreateDropdownButton(const char *objectName, std::vector<std::string> captions, const char* tooltip, float fX, float fY, float fWidth, float fHeight,
	ALLEGRO_COLOR colors[6], bool isFilled, ALLEGRO_FONT *captionFont)
{
	setObjectProperties(objectName, fX, fY, fWidth, fHeight);

	button = std::make_shared<HD_UI_Button>();
	button->CreateRectangleButton("button", captions[0].c_str(), tooltip, 0.0f, 0.0f, width, height, colors, isFilled, captionFont, ALLEGRO_ALIGN_LEFT);
	button->setCallback(std::bind(&HD_UI_ButtonDropdown::buttonClick, this));
	addChild(button);

	std::shared_ptr<HD_UI_GraphicsObject> arrow = std::make_shared<HD_UI_GraphicsObject>();
	arrow->CreateImageObject("arrow", "arrowS_D", "misc_atlas.png", 0.0f, 0.0f);
	arrow->x = width - arrow->width - 5.0f;
	arrow->y = height / 2.0f - arrow->height / 2.0f;
	addChild(arrow);
	
	captions.erase(captions.begin());
	buttonList = std::make_shared<HD_UI_ButtonMenu>();
	buttonList->CreateButtonMenu("buttonList", 0.0f, height, width, captions);
	for (unsigned int ii = 0; ii < captions.size(); ii++)
	{
		buttonList->setCallbacks(ii, std::bind(&HD_UI_ButtonDropdown::listClick, this, ii));
	}
	addChild(buttonList);
}

//Callbacks
void HD_UI_ButtonDropdown::buttonClick()
{
	if (buttonList->visible)
		buttonList->showMenu(false);
	else
		buttonList->showMenu(true);
}

void HD_UI_ButtonDropdown::listClick(int index)
{
	std::shared_ptr<HD_UI_Button> btn = buttonList->getButtonInList(index);

	selectionID = index;
	selectionCaption = btn->getCaption();

	button->setCaption(selectionCaption.c_str());

	buttonClick();
}