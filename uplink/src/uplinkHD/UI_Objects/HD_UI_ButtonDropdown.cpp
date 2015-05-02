// Implementation of the button dropdown
//======================================

#include "HD_UI_ButtonDropdown.h"

#include "../HD_Screen.h"

//button creation; the first caption in the vector is the initial one on the button
HD_UI_ButtonDropdown::HD_UI_ButtonDropdown(const char *objectName, int index, std::vector<std::string> captions, const char *tooltip, float fX, float fY,
	float fWidth, float fHeight, ALLEGRO_COLOR colors[6], bool isFilled, ALLEGRO_FONT *captionFont, HD_UI_Container *newParent)
{
	setObjectProperties(objectName, fX, fY, fWidth, fHeight, newParent, index);

	button = new HD_UI_Button("button", -1, captions[0].c_str(), tooltip, 0.0f, 0.0f, width, height, colors, isFilled, captionFont, ALLEGRO_ALIGN_LEFT, this);
	button->setCallback(std::bind(&HD_UI_ButtonDropdown::buttonClick, this));

	HD_UI_GraphicsObject *arrow = new HD_UI_GraphicsObject("arrow", -1, "arrowS_D", "misc_atlas.xml", 0.0f, 0.0f, this);
	arrow->x = width - arrow->width - 5.0f;
	arrow->y = height / 2.0f - arrow->height / 2.0f;
	
	captions.erase(captions.begin());
	buttonList = new HD_UI_ButtonMenu("buttonList", -1, captions, button);
	for (unsigned int ii = 0; ii < captions.size(); ii++)
	{
		buttonList->setCallbacks(ii, std::bind(&HD_UI_ButtonDropdown::listClick, this, ii));
	}
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
	HD_UI_Button *btn = buttonList->getButtonInList(index);

	selectionID = index;
	selectionCaption = btn->getCaption();

	button->setCaption(selectionCaption);

	buttonClick();
}