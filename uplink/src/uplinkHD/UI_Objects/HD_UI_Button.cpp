// Implementation of the Button UI Object
//=======================================

#include <string>
#include <functional>
#include <allegro5/allegro.h>

#include "../HD_Resources.h"
#include "HD_UI_GraphicsObject.h"
#include "HD_UI_TextObject.h"

#include "HD_UI_Button.h"

//============================
// Protected Functions
//============================

void HD_UI_Button::mouseOver()
{
	//change GFX
	if (standardImage == NULL)
		gfxObject->setColors(stateColors[1], stateColors[5]);
	else
		gfxObject->setGfxImage(hoverImage);

	btnState = over;
}

void HD_UI_Button::mouseOut()
{
	if (standardImage == NULL)
		gfxObject->setColors(stateColors[0], stateColors[5]);
	else
		gfxObject->setGfxImage(standardImage);

	btnState = standard;
}

void HD_UI_Button::mouseClick()
{
	if (standardImage == NULL)
	{
		gfxObject->setColors(stateColors[2], stateColors[5]);
		textObject->setTextColor(stateColors[4]);
	}
	else
		gfxObject->setGfxImage(clickImage);

	btnState = clicked;

	CDBTweener::CTween *tween1 = new CDBTweener::CTween(&CDBTweener::TWEQ_BACK, CDBTweener::TWEA_OUT, 0.5f, &gfxObject->scaleX, 0.7f);
	gfxObject->addAnimation(tween1, true);
}

void HD_UI_Button::mouseRelease()
{
	if (standardImage == NULL)
	{
		gfxObject->setColors(stateColors[1], stateColors[5]);
		textObject->setTextColor(stateColors[3]);
	}
	else
		gfxObject->setGfxImage(hoverImage);

	btnState = standard;

	CDBTweener::CTween *tween1 = new CDBTweener::CTween(&CDBTweener::TWEQ_BACK, CDBTweener::TWEA_OUT, 0.5f, &gfxObject->scaleX, 1.0f);
	gfxObject->addAnimation(tween1, true);

	//buttonCallback();
}

bool HD_UI_Button::checkMouseOver(int mX, int mY)
{
	//TO-DO: Use the actual hitzone
	if (mX > x &&
		mX < x + width &&
		mY > y &&
		mY < y + height)
		return true;
	else
		return false;
}

//============================
// Public Creation Functions
//============================

//Image button
HD_UI_Button::HD_UI_Button(char *objectName, int index, char *standardImageName, char *atlasName, void(*callback)(), char *tooltip,
	float fX, float fY, float hzWidth, float hzHeight, HD_UI_Container *newParent)
{
	std::string stndImgName = standardImageName;
	std::string hvrImgName = stndImgName;
	hvrImgName[hvrImgName.size() - 6] = 'h';
	std::string clkImgName = stndImgName;
	clkImgName[clkImgName.size() - 6] = 'c';

	if (atlasName)
	{
		standardImage = HDResources->hd_getSubImage(stndImgName.c_str(), atlasName);
		hoverImage = HDResources->hd_getSubImage(hvrImgName.c_str(), atlasName);
		clickImage = HDResources->hd_getSubImage(clkImgName.c_str(), atlasName);
	}
	else
	{
		standardImage = HDResources->hd_getImage(stndImgName.c_str());
		hoverImage = HDResources->hd_getImage(hvrImgName.c_str());
		clickImage = HDResources->hd_getImage(clkImgName.c_str());
	}

	setObjectProperties(objectName, fX, fY, al_get_bitmap_width(standardImage), al_get_bitmap_height(standardImage), newParent, index);

	gfxObject = new HD_UI_GraphicsObject("baseGFX", -1, standardImageName, atlasName, 0.0f, 0.0f, this);

	hitzone.width = hzWidth;
	hitzone.height = hzHeight;

	buttonCallback = std::bind(callback);
	tooltipText = tooltip;
}

//Filled Rect or Filled & Stroked Rect button; has a text caption
HD_UI_Button::HD_UI_Button(char *objectName, int index, char *caption, void(*callback)(), char *tooltip,
	float fX, float fY, float fWidth, float fHeight, ALLEGRO_COLOR colors[6], bool isFilled, ALLEGRO_FONT *captionFont,
	float hzWidth, float hzHeight, HD_UI_Container *newParent)
{
	setObjectProperties(objectName, fX, fY, fWidth, fHeight, newParent, index);

	if (isFilled)
		gfxObject = new HD_UI_GraphicsObject("baseGFX", -1, 0.0f, 0.0f, fWidth, fHeight, -1.0f, colors[0], this);
	else
		gfxObject = new HD_UI_GraphicsObject("baseGFX", -1, 0.0f, 0.0f, fWidth, fHeight, 2.0f, colors[0], colors[5], this);

	textObject = new HD_UI_TextObject("caption", -1, 0.0f, 0.0f, caption, captionFont, colors[3], ALLEGRO_ALIGN_CENTER, this);
	textObject->x = x + width / 2;
	textObject->y = y + height / 2 - textObject->height / 2;

	for (unsigned int ii = 0; ii < 6; ii++)
		stateColors[ii] = colors[ii];

	hitzone.width = hzWidth;
	hitzone.height = hzHeight;

	buttonCallback = std::bind(callback);
	tooltipText = tooltip;
}

//============================
// General Functions
//============================

//Destruction
HD_UI_Button::~HD_UI_Button()
{
	al_destroy_bitmap(standardImage);
	al_destroy_bitmap(hoverImage);
	al_destroy_bitmap(clickImage);
}

void HD_UI_Button::Update(ALLEGRO_MOUSE_STATE *mouseState, double timeSpeed)
{
	HD_UI_Container::Update(mouseState, timeSpeed);
	
	if (checkMouseOver(mouseState->x, mouseState->y)
						&& btnState == standard)
		mouseOver();	//To-Do: Start tooltip timer

	else if (!checkMouseOver(mouseState->x, mouseState->y) &&
				(btnState == over || btnState == clicked))
		mouseOut();		//To-Do: Stop tooltip timer

	bool isPrimaryDown = (mouseState->buttons & 1);
	bool isPrimaryReleased = !isPrimaryDown;

	if (isPrimaryDown && btnState == over)
			mouseClick();
	if (isPrimaryReleased && btnState == clicked )
			mouseRelease();

	//To-Do: Show tooltip if timer is done!
}

void HD_UI_Button::Draw()
{
	HD_UI_Container::Draw();
}

void HD_UI_Button::Clear()
{
	HD_UI_Container::Clear();

	tooltipText.clear();
	captionText.clear();

	delete this;
}