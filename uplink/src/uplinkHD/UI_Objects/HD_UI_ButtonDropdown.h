//========================================
// This creates a dropdown button.
// On click, it shows a list of options.
//========================================

#ifndef HD_UI_BUTTONDROPDOWN_H
#define HD_UI_BUTTONDROPDOWN_H

#include "HD_UI_Container.h"
#include "HD_UI_Button.h"
#include "HD_UI_ButtonMenu.h"

class HD_UI_ButtonDropdown : public HD_UI_Container
{
private:
	HD_UI_Button *button = NULL;
	HD_UI_ButtonMenu *buttonList = NULL;
	int selectionID = -1;
	const char* selectionCaption;

protected:
	HD_UI_ButtonDropdown(){}

public:
	~HD_UI_ButtonDropdown(){}

	//button creation; the first caption in the vector is the initial one on the button
	HD_UI_ButtonDropdown(const char *objectName, int index, std::vector<std::string> captions, const char *tooltip, float fX, float fY,
		float fWidth, float fHeight, ALLEGRO_COLOR colors[6], bool isFilled, ALLEGRO_FONT *captionFont, HD_UI_Container *newParent);

	int getSelectionID() { return selectionID; }
	const char* getSelectionCaption() { return selectionCaption; }

	//Callbacks
	void buttonClick();
	void listClick(int index);
};

#endif