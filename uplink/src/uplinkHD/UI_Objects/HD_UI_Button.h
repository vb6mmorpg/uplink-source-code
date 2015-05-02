//========================================
// Finally some input!
// This creates a normal clickable button.
//========================================

#ifndef HD_UI_BUTTON_H
#define HD_UI_BUTTON_H

#include <string>
#include <functional>
#include <allegro5/allegro.h>

#include "HD_UI_Object.h"
#include "HD_UI_Container.h"
#include "HD_UI_GraphicsObject.h"
#include "HD_UI_TextObject.h"
#include "HD_UI_Tooltip.h"

class HD_UI_Button : public HD_UI_Container
{
protected:
	enum ButtonState
	{
		standard,
		over,
		clicked,
		deactivated
	};

	ALLEGRO_BITMAP *standardImage = NULL;
	ALLEGRO_BITMAP *hoverImage = NULL;
	ALLEGRO_BITMAP *clickImage = NULL;

	//Colors for primitive based buttons
	//0 - standard; 1 - hover; 2 - clicked;
	//3 - standardText; 4 - clickedText; 5 - stroke;
	ALLEGRO_COLOR stateColors[8];
	ALLEGRO_BITMAP *iconImage;

	HD_UI_GraphicsObject *gfxObject = NULL;
	HD_UI_TextObject *textObject = NULL;
	HD_UI_GraphicsObject *iconObject = NULL;
	//To-Do: Right-Click Context menu object

	//Tooltip
	void setTooltip(const char* tooltip);
	HD_UI_Tooltip *tooltipObject = NULL;
	std::string tooltipText;
	float tooltipTimer = 0.0f;

	ButtonState btnState = standard;
	std::function<void()>buttonCallback;

	//Button functions
	void virtual mouseOver();
	void virtual mouseOut();
	void virtual mouseClick();
	void virtual mouseRelease();

	//Construction/destruction
	HD_UI_Button() { }
	~HD_UI_Button();

public:
	//Creation functions

	//Image button
	//based on the standard image, the hover and clicked ones are found auto-magically
	HD_UI_Button(const char *objectName, int index, const char *standardImageName, const char *atlasName, const char *tooltip,
		float fX, float fY, HD_UI_Container *newParent);

	//Filled Rect or Filled & Stroked Rect button; has a text caption
	//the first 3 colors are for standard, hover and clicked states of the bg
	//the next 2 colors are for the text's standard and clicked color; and the last one is for the stroke
	HD_UI_Button(const char *objectName, int index, const char *caption, const char *tooltip, float fX, float fY,
		float fWidth, float fHeight, ALLEGRO_COLOR colors[6], bool isFilled, ALLEGRO_FONT *captionFont, int captionAlign, HD_UI_Container *newParent);

	//Just like the one above, only this one has an icon
	HD_UI_Button(const char *objectName, int index, const char *caption, const char *tooltip, float fX, float fY, float fWidth, float fHeight,
		ALLEGRO_COLOR colors[6], bool isFilled, ALLEGRO_FONT *captionFont, const char* iconName, const char *atlasName, HD_UI_Container *newParent);

	//General functions
	void virtual Update();
	void virtual Clear();

	//Callback
	void virtual defaultCallback();
	void virtual setCallback(std::function<void()>newCallback);
	const char* getCaption(){ return textObject->getText(); }
	void setCaption(const char* newCaption){ textObject->setText(newCaption); }
};

#endif