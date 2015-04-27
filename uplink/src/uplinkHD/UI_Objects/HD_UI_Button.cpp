// Implementation of the Button UI Object
//=======================================

#include <string>
#include <functional>
#include <allegro5/allegro.h>

#include "../HD_Screen.h"
#include "../HD_Resources.h"

#include "HD_UI_GraphicsObject.h"
#include "HD_UI_TextObject.h"

#include "HD_UI_Button.h"

//============================
// Protected Functions
//============================

//Destruction
HD_UI_Button::~HD_UI_Button()
{
	al_destroy_bitmap(standardImage);
	al_destroy_bitmap(hoverImage);
	al_destroy_bitmap(clickImage);
	al_destroy_bitmap(iconImage);
}

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
	{
		gfxObject->setColors(stateColors[0], stateColors[5]);
		textObject->setTextColor(stateColors[3]);
	}
	else
		gfxObject->setGfxImage(standardImage);

	if (tooltipObject->visible)
		tooltipObject->ShowTooltip(false);
	tooltipTimer = 0.0f;

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

	if (tooltipObject->visible)
		tooltipObject->ShowTooltip(false);
	tooltipTimer = 0.0f;

	btnState = clicked;
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

	if (buttonCallback != NULL)
		buttonCallback();
}

bool HD_UI_Button::checkMouseOver()
{
	int mX = HDScreen->mouse->GetState()->x;
	int mY = HDScreen->mouse->GetState()->y;

	if (mX > x && mX < x + width &&
		mY > y && mY < y + height)
	{
		//the mouse is over this
		//set it's target to this
		//if it succeeds, we have contact!
		if (HDScreen->mouse->SetTarget(this))
			return true;
		else
			return false;
		
	}
	else
	{
		//the mouse is out
		//reset the target if it has been set by this
		if (HDScreen->mouse->GetTarget() == this)
			HDScreen->mouse->SetTarget(NULL);
		return false;
	}
}

void HD_UI_Button::defaultCallback()
{
	std::string printS = this->name;
	printS.append(" has no callback set. This is the default.\n");
	OutputDebugStringA(printS.c_str());
}

//============================
// Public Creation Functions
//============================

//Image button
HD_UI_Button::HD_UI_Button(char *objectName, int index, char *standardImageName, char *atlasName, char *tooltip,
	float fX, float fY, HD_UI_Container *newParent)
{
	std::string stndImgName = standardImageName;
	std::string hvrImgName = stndImgName;
	hvrImgName[hvrImgName.size() - 1] = 'h';
	std::string clkImgName = stndImgName;
	clkImgName[clkImgName.size() - 1] = 'c';

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

	buttonCallback = std::bind(&HD_UI_Button::defaultCallback, this);

	tooltipText = tooltip;
	tooltipObject = new HD_UI_Tooltip(tooltip, this);
	tooltipObject->visible = false;
}

//Filled Rect or Filled & Stroked Rect button; has a text caption
HD_UI_Button::HD_UI_Button(char *objectName, int index, char *caption, char *tooltip,
	float fX, float fY, float fWidth, float fHeight, ALLEGRO_COLOR colors[6], bool isFilled, ALLEGRO_FONT *captionFont,
	int captionAlign, HD_UI_Container *newParent)
{
	setObjectProperties(objectName, fX, fY, fWidth, fHeight, newParent, index);

	if (isFilled)
		gfxObject = new HD_UI_GraphicsObject("baseGFX", -1, 0.0f, 0.0f, fWidth, fHeight, -1.0f, colors[0], this);
	else
		gfxObject = new HD_UI_GraphicsObject("baseGFX", -1, 0.0f, 0.0f, fWidth, fHeight, 1.0f, colors[0], colors[5], this);

	textObject = new HD_UI_TextObject("caption", -1, 0.0f, 0.0f, caption, captionFont, colors[3], captionAlign, this);

	if (captionAlign == ALLEGRO_ALIGN_CENTER)
		textObject->x = width / 2;
	else if (captionAlign == ALLEGRO_ALIGN_LEFT)
		textObject->x = 5.0f;
	else if (captionAlign == ALLEGRO_ALIGN_RIGHT)
		textObject->x = width - 5.0f;

	textObject->y = height / 2 - textObject->height / 2;

	for (unsigned int ii = 0; ii < 6; ii++)
		stateColors[ii] = colors[ii];

	buttonCallback = std::bind(&HD_UI_Button::defaultCallback, this);

	tooltipText = tooltip;
	tooltipObject = new HD_UI_Tooltip(tooltip, this);
	tooltipObject->visible = false;
}

//Like the one above, only this one has an icon
HD_UI_Button::HD_UI_Button(char *objectName, int index, char *caption, char *tooltip, float fX, float fY, float fWidth, float fHeight,
	ALLEGRO_COLOR colors[6], bool isFilled, ALLEGRO_FONT *captionFont, char* iconName, char *atlasName, HD_UI_Container *newParent)
{
	setObjectProperties(objectName, fX, fY, fWidth, fHeight, newParent, index);

	if (isFilled)
		gfxObject = new HD_UI_GraphicsObject("baseGFX", -1, 0.0f, 0.0f, fWidth, fHeight, -1.0f, colors[0], this);
	else
		gfxObject = new HD_UI_GraphicsObject("baseGFX", -1, 0.0f, 0.0f, fWidth, fHeight, 1.0f, colors[0], colors[5], this);

	iconObject = new HD_UI_GraphicsObject("iconGFX", -1, iconName, atlasName, 5.0f, 0.0f, this);
	iconObject->y = gfxObject->height / 2.0f - iconObject->height / 2.0f;

	textObject = new HD_UI_TextObject("caption", -1, 0.0f, 0.0f, caption, captionFont, colors[3], ALLEGRO_ALIGN_LEFT, this);
	textObject->x = iconObject->x + 5.0f;
	textObject->y = height / 2 - textObject->height / 2;

	for (unsigned int ii = 0; ii < 6; ii++)
		stateColors[ii] = colors[ii];

	buttonCallback = std::bind(&HD_UI_Button::defaultCallback, this);

	tooltipText = tooltip;
	tooltipObject = new HD_UI_Tooltip(tooltip, this);
	tooltipObject->visible = false;
}

//============================
// General Functions
//============================

void HD_UI_Button::setCallback(std::function<void()>newCallback)
{
	if (newCallback._Empty())
		buttonCallback = std::bind(&HD_UI_Button::defaultCallback, this);
	else
		buttonCallback = newCallback;
}

void HD_UI_Button::Update()
{
	HD_UI_Container::Update();

	if (!isVisible) return;
	
	if (checkMouseOver() && btnState == standard)
		mouseOver();

	else if (!checkMouseOver() && (btnState == over || btnState == clicked))
		mouseOut();

	bool isPrimaryDown = (HDScreen->mouse->GetState()->buttons & 1);
	bool isPrimaryReleased = !isPrimaryDown;

	if (isPrimaryDown && btnState == over)
			mouseClick();
	if (isPrimaryReleased && btnState == clicked )
			mouseRelease();

	//Tooltip
	if (tooltipTimer > 1.5f && !tooltipObject->visible)
		tooltipObject->ShowTooltip(true);

	if (btnState == over)
		tooltipTimer += HDScreen->deltaTime;
}

void HD_UI_Button::Clear()
{
	HD_UI_Container::Clear();

	tooltipText.clear();

	delete this;
}