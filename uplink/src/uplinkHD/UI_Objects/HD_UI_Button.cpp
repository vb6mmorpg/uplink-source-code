// Implementation of the Button UI Object
//=======================================

#include <string>
#include <functional>
#include <allegro5/allegro.h>

#include "../HD_Screen.h"
#include "../HD_Resources.h"

#include "../UI_Layouts/HD_Root.h"

#include "HD_UI_GraphicsObject.h"
#include "HD_UI_TextObject.h"

#include "HD_UI_Button.h"

//============================
// Protected Functions
//============================

void HD_UI_Button::mouseOver()
{
	//change GFX
	if (standardImage == NULL)	//it's made out of shapes
		gfxObject->setColors(stateColors[1], stateColors[5]);
	else if (standardImage != NULL && standardImage2 == NULL)	//it has images, but is not selectable
		gfxObject->setGfxImage(hoverImage);
	else if (standardImage != NULL && standardImage2 != NULL)	//it has images and is selectable
		if (isSelected)
			gfxObject->setGfxImage(hoverImage2);
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
	else if (standardImage != NULL && standardImage2 == NULL)
		gfxObject->setGfxImage(standardImage);
	else if (standardImage != NULL && standardImage2 != NULL)
		if (isSelected)
			gfxObject->setGfxImage(standardImage2);
		else
			gfxObject->setGfxImage(standardImage);

	if (!sTooltip.empty())
	{
		//if (tooltipObject->visible)
		//	tooltipObject->ShowTooltip(false);
		root->ShowTooltip(false);
		tooltipTimer = 0.0f;
	}

	btnState = standard;
}

void HD_UI_Button::mouseClick()
{
	if (standardImage == NULL)
	{
		gfxObject->setColors(stateColors[2], stateColors[5]);
		textObject->setTextColor(stateColors[4]);
	}
	else if (standardImage != NULL && standardImage2 == NULL)
		gfxObject->setGfxImage(clickImage);
	else if (standardImage != NULL && standardImage2 != NULL)
		if (isSelected)
			gfxObject->setGfxImage(clickImage2);
		else
			gfxObject->setGfxImage(clickImage);

	if (!sTooltip.empty())
	{
		//if (tooltipObject->visible)
		//	tooltipObject->ShowTooltip(false);
		root->ShowTooltip(false);
		tooltipTimer = 0.0f;
	}

	btnState = clicked;

	isSelected = !isSelected;
}

void HD_UI_Button::mouseRelease()
{
	if (standardImage == NULL)
	{
		gfxObject->setColors(stateColors[1], stateColors[5]);
		textObject->setTextColor(stateColors[3]);
	}
	else if (standardImage != NULL && standardImage2 == NULL)
		gfxObject->setGfxImage(hoverImage);
	else if (standardImage != NULL && standardImage2 != NULL)
		if (isSelected)
			gfxObject->setGfxImage(hoverImage2);
		else
			gfxObject->setGfxImage(hoverImage);

	btnState = over;

	if (buttonCallback != NULL)
		buttonCallback();
}

/*void HD_UI_Button::setTooltip(const char* tooltip)
{
	if (strcmp(tooltip, "") != 0)
	{
		tooltipObject = std::make_shared<HD_UI_Tooltip>();
		tooltipObject->Create(tooltip, width + 2.0f, height * 0.75f);
		addChild(tooltipObject);
		tooltipObject->visible = false;
	}
}*/

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
void HD_UI_Button::CreateImageButton(const char *objectName, const char *standardImageName, const char *atlasName, const char *tooltip,
	float fX, float fY)
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

	setObjectProperties(objectName, fX, fY, al_get_bitmap_width(standardImage), al_get_bitmap_height(standardImage));

	gfxObject = std::make_shared<HD_UI_GraphicsObject>();
	gfxObject->CreateImageObject("baseGFX", standardImageName, atlasName, 0.0f, 0.0f);
	addChild(gfxObject);

	buttonCallback = std::bind(&HD_UI_Button::defaultCallback, this);

	sTooltip = tooltip;
	//setTooltip(tooltip);
}

//Image based selection button
void HD_UI_Button::CreateImageSelectionButton(const char *objectName, const char *standardImageName, const char *atlasName, const char *tooltip,
	float fX, float fY, bool startSelected)
{
	std::string stndImgName = standardImageName;
	std::string stndImgName2 = stndImgName + '2';

	std::string hvrImgName = stndImgName;
	hvrImgName[hvrImgName.size() - 1] = 'h';
	std::string hvrImgName2 = hvrImgName + '2';

	std::string clkImgName = stndImgName;
	clkImgName[clkImgName.size() - 1] = 'c';
	std::string clkImgName2 = clkImgName + '2';

	if (atlasName)
	{
		standardImage = HDResources->hd_getSubImage(stndImgName.c_str(), atlasName);
		standardImage2 = HDResources->hd_getSubImage(stndImgName2.c_str(), atlasName);
		hoverImage = HDResources->hd_getSubImage(hvrImgName.c_str(), atlasName);
		hoverImage2 = HDResources->hd_getSubImage(hvrImgName2.c_str(), atlasName);
		clickImage = HDResources->hd_getSubImage(clkImgName.c_str(), atlasName);
		clickImage2 = HDResources->hd_getSubImage(clkImgName2.c_str(), atlasName);
	}
	else
	{
		standardImage = HDResources->hd_getImage(stndImgName.c_str());
		standardImage2 = HDResources->hd_getImage(stndImgName2.c_str());
		hoverImage = HDResources->hd_getImage(hvrImgName.c_str());
		hoverImage2 = HDResources->hd_getImage(hvrImgName2.c_str());
		clickImage = HDResources->hd_getImage(clkImgName.c_str());
		clickImage2 = HDResources->hd_getImage(clkImgName2.c_str());
	}

	setObjectProperties(objectName, fX, fY, al_get_bitmap_width(standardImage), al_get_bitmap_height(standardImage));

	gfxObject = std::make_shared<HD_UI_GraphicsObject>();
	gfxObject->CreateImageObject("baseGFX", standardImageName, atlasName, 0.0f, 0.0f);
	addChild(gfxObject);

	buttonCallback = std::bind(&HD_UI_Button::defaultCallback, this);

	//init selection
	isSelected = startSelected;
	if (isSelected)
		gfxObject->setGfxImage(standardImage2);
	else
		gfxObject->setGfxImage(standardImage);

	sTooltip = tooltip;
	//setTooltip(tooltip);
}

//Filled Rect or Filled & Stroked Rect button; has a text caption
void HD_UI_Button::CreateRectangleButton(const char *objectName, const char *caption, const char *tooltip, float fX, float fY,
	float fWidth, float fHeight, ALLEGRO_COLOR colors[6], bool isFilled, ALLEGRO_FONT *captionFont, int captionAlign)
{
	setObjectProperties(objectName, fX, fY, fWidth, fHeight);

	gfxObject = std::make_shared<HD_UI_GraphicsObject>();

	if (isFilled)
		gfxObject->CreateRectangleObject("baseGFX",0.0f, 0.0f, fWidth, fHeight, -1.0f, colors[0]);
	else
		gfxObject->CreateSFRectangleObject("baseGFX", 0.0f, 0.0f, fWidth, fHeight, 1.0f, colors[0], colors[5]);

	addChild(gfxObject);

	textObject = std::make_shared<HD_UI_TextObject>();
	textObject->CreateSinglelineText("caption", 0.0f, 0.0f, caption, colors[3], captionFont, captionAlign);
	addChild(textObject);

	if (captionAlign == ALLEGRO_ALIGN_CENTER)
		textObject->x = width / 2;
	else if (captionAlign == ALLEGRO_ALIGN_LEFT)
		textObject->x = 5.0f;
	else if (captionAlign == ALLEGRO_ALIGN_RIGHT)
		textObject->x = width - 5.0f;

	textObject->y = height / 2 - textObject->height / 2;

	if (textObject->x + textObject->width > gfxObject->width)
		gfxObject->width = textObject->x + textObject->width + 5.0f;

	for (unsigned int ii = 0; ii < 6; ii++)
		stateColors[ii] = colors[ii];

	buttonCallback = std::bind(&HD_UI_Button::defaultCallback, this);

	sTooltip = tooltip;
	//setTooltip(tooltip);
}

//Like the one above, only this one has an icon
void HD_UI_Button::CreateRectangleWIconButton(const char *objectName, const char *caption, const char *tooltip, float fX, float fY, float fWidth, float fHeight,
	ALLEGRO_COLOR colors[6], bool isFilled, const char* iconName, const char *atlasName, ALLEGRO_FONT *captionFont)
{
	setObjectProperties(objectName, fX, fY, fWidth, fHeight);

	gfxObject = std::make_shared<HD_UI_GraphicsObject>();

	if (isFilled)
		gfxObject->CreateRectangleObject("baseGFX", 0.0f, 0.0f, fWidth, fHeight, -1.0f, colors[0]);
	else
		gfxObject->CreateSFRectangleObject("baseGFX", 0.0f, 0.0f, fWidth, fHeight, 1.0f, colors[0], colors[5]);

	addChild(gfxObject);

	iconObject = std::make_shared<HD_UI_GraphicsObject>();
	iconObject->CreateImageObject("iconGFX", iconName, atlasName, 5.0f, 0.0f);
	iconObject->y = gfxObject->height / 2.0f - iconObject->height / 2.0f;
	addChild(iconObject);

	textObject = std::make_shared<HD_UI_TextObject>();
	textObject->CreateSinglelineText("caption", 0.0f, 0.0f, caption, colors[3], captionFont, 0);
	textObject->x = iconObject->x + iconObject->width + 5.0f;
	textObject->y = height / 2 - textObject->height / 2;
	addChild(textObject);

	if (textObject->x + textObject->width > gfxObject->width)
		gfxObject->width = textObject->x + textObject->width + 5.0f;

	for (unsigned int ii = 0; ii < 6; ii++)
		stateColors[ii] = colors[ii];

	buttonCallback = std::bind(&HD_UI_Button::defaultCallback, this);

	sTooltip = tooltip;
	//setTooltip(tooltip);
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

void HD_UI_Button::SetActive(bool active)
{
	if (active)
	{
		alpha = 1.0f;
		btnState = standard;
	}
	else
	{
		alpha = 0.25f;
		btnState = deactivated;
	}
}

void HD_UI_Button::Update()
{
	HD_UI_Container::Update();

	if (isMouseTarget() && btnState == standard)
		mouseOver();

	if (!isMouseTarget() && (btnState == over || btnState == clicked))
		mouseOut();

	bool isPrimaryDown = (HDScreen->mouse->GetState()->buttons & 1);
	bool isPrimaryReleased = !isPrimaryDown;

	if (isPrimaryDown && btnState == over)
		mouseClick();
	if (isPrimaryReleased && btnState == clicked && setMouseOver())
		mouseRelease();

	//Tooltip
	if (!sTooltip.empty() && btnState != deactivated)
	{
		if (tooltipTimer > 1.5f) // && !tooltipObject->visible)
			root->ShowTooltip(true);

		if (btnState == over)
			tooltipTimer += HDScreen->deltaTime;
	}
}

void HD_UI_Button::Draw()
{
	HD_UI_Container::Draw();

	if (HDScreen->DebugInfo && isVisible)
	{
		if (isMouseTarget())
			al_draw_filled_rectangle(globalX, globalY, globalX + drawWidth, globalY + drawHeight, al_map_rgba_f(0.25f, 0.25f, 0.0f, 0.25f));
		else
			al_draw_filled_rectangle(globalX, globalY, globalX + drawWidth, globalY + drawHeight, al_map_rgba_f(0.25f, 0.0f, 0.0f, 0.25f));
	}
}

void HD_UI_Button::Clear()
{
	/*if (standardImage)
	{
		if (al_is_sub_bitmap(standardImage))
		{
			al_destroy_bitmap(standardImage);
			standardImage = NULL;
		}
		if (al_is_sub_bitmap(hoverImage))
		{
			al_destroy_bitmap(hoverImage);
			hoverImage = NULL;
		}
		if (al_is_sub_bitmap(clickImage))
		{
			al_destroy_bitmap(clickImage);
			clickImage = NULL;
		}
		gfxObject->setGfxImage(NULL);
	}

	if (iconImage)
	if (al_is_sub_bitmap(iconImage))
	{
		al_destroy_bitmap(iconImage);
		iconImage = NULL;
		iconObject->setGfxImage(NULL);
	}*/

	gfxObject = NULL;
	textObject = NULL;
	iconObject = NULL;
	//tooltipObject = NULL;

	HD_UI_Container::Clear();
}