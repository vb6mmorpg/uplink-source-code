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
	ALLEGRO_BITMAP *standardImage2 = NULL;
	ALLEGRO_BITMAP *hoverImage = NULL;
	ALLEGRO_BITMAP *hoverImage2 = NULL;
	ALLEGRO_BITMAP *clickImage = NULL;
	ALLEGRO_BITMAP *clickImage2 = NULL;
	ALLEGRO_BITMAP *iconImage = NULL;

	//Colors for primitive based buttons
	//0 - standard; 1 - hover; 2 - clicked;
	//3 - standardText; 4 - clickedText; 5 - stroke;
	ALLEGRO_COLOR stateColors[8];

	std::shared_ptr<HD_UI_GraphicsObject> gfxObject = nullptr;
	std::shared_ptr<HD_UI_TextObject> textObject = nullptr;
	std::shared_ptr<HD_UI_GraphicsObject> iconObject = nullptr;

	//Tooltip
	std::string sTooltip;
	//void setTooltip(const char* tooltip);
	//std::shared_ptr<HD_UI_Tooltip> tooltipObject = nullptr;
	float tooltipTimer = 0.0f;

	ButtonState btnState = standard;
	bool isSelected = false;
	std::function<void()>buttonCallback;

	//Button functions
	void virtual mouseOver();
	void virtual mouseOut();
	void virtual mouseClick();
	void virtual mouseRelease();

public:
	//Construction/destruction
	HD_UI_Button() {}
	~HD_UI_Button() {}

	//Creation functions

	//Image button
	//based on the standard image, the hover and clicked ones are found auto-magically
	void CreateImageButton(const char *objectName, const char *standardImageName, const char *atlasName, const char *tooltip,
		float fX, float fY);

	//Image based selection button
	//Switches between 2 sets of images and sets itself as selected or not
	void CreateImageSelectionButton(const char *objectName, const char *standardImageName, const char *atlasName, const char *tooltip,
		float fX, float fY, bool startSelected);

	//Filled Rect or Filled & Stroked Rect button; has a text caption
	//the first 3 colors are for standard, hover and clicked states of the bg
	//the next 2 colors are for the text's standard and clicked color; and the last one is for the stroke
	void CreateRectangleButton(const char *objectName, const char *caption, const char *tooltip, float fX, float fY,
		float fWidth, float fHeight, ALLEGRO_COLOR colors[6], bool isFilled, ALLEGRO_FONT *captionFont = HDResources->font24,
		int captionAlign = ALLEGRO_ALIGN_CENTER);

	//Just like the one above, only this one has an icon
	void CreateRectangleWIconButton(const char *objectName, const char *caption, const char *tooltip, float fX, float fY, float fWidth, float fHeight,
		ALLEGRO_COLOR colors[6], bool isFilled, const char* iconName, const char *atlasName, ALLEGRO_FONT *captionFont = HDResources->font24);

	//General functions
	void virtual Update();
	void virtual Draw();
	void virtual Clear();

	//Callback
	void virtual defaultCallback();
	void virtual setCallback(std::function<void()>newCallback);

	//Caption
	const char* getCaption(){ return textObject->getText(); }
	void virtual setCaption(const char* newCaption){ textObject->setText(newCaption); }

	//Tooltip
	const char* getTooltip(){ return sTooltip.c_str(); }

	void SetActive(bool active);
};

#endif