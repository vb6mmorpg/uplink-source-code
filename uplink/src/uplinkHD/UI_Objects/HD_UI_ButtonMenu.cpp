// Implementation of the Button Menu
//==================================

#include "HD_UI_ButtonMenu.h"
#include "../HD_Resources.h"
#include "../HD_Screen.h"
#include "../HD_ColorPalettes.h"

void HD_UI_ButtonMenu::setCorrectPosition()
{
	if (isHorizontal)
	{
		if (globalX + parent->width + width > HDScreen->nScreenW - width - 20.0f)
		{
			x = -10.0f - width;
			anchorPos.isOnRight = false;
		}
		else
		{
			x = 10.0f;
			anchorPos.isOnRight = true;
		}

		if (globalY + parent->height / 2.0f + height > HDScreen->nScreenH - height - 20.0f)
		{
			y = -height + parent->height / 2.0f;
			anchorPos.isOnTop = false;
		}
		else
		{
			y = parent->height / 2.0f;
			anchorPos.isOnTop = true;
		}
	}
	else
	{
		x = 20.0f;
		anchorPos.isOnRight = false;

		if (globalY + parent->height + height > HDScreen->nScreenH - height - 20.0f)
		{
			y = -height - 10.0f;
			anchorPos.isOnTop = false;
		}
		else
		{
			y = parent->height + 10.0f;
			anchorPos.isOnTop = true;
		}
	}
}

void HD_UI_ButtonMenu::createAnchorGfx()
{
	HD_UI_GraphicsObject *anchorGfx = NULL;

	if (isHorizontal)
	{
		if (anchorPos.isOnRight && anchorPos.isOnTop)			//top-right
			anchorGfx = new HD_UI_GraphicsObject("anchorGfx", -1, width - 10.0f, 5.0f, 20.0f, 20.0f, true, palette->bluesSat.cBlue2, this);
		else if (!anchorPos.isOnRight && !anchorPos.isOnTop)	//bottom-left
			anchorGfx = new HD_UI_GraphicsObject("anchorGfx", -1, -10.0f, height - 25.0f, 20.0f, 20.0f, true, palette->bluesSat.cBlue2, this);
		else if (anchorPos.isOnRight && !anchorPos.isOnTop)		//bottom-right
			anchorGfx = new HD_UI_GraphicsObject("anchorGfx", -1, width - 10.0f, height - 25.0f, 20.0f, 20.0f, true, palette->bluesSat.cBlue2, this);
		else if (!anchorPos.isOnRight && anchorPos.isOnTop)		//top-left
			anchorGfx = new HD_UI_GraphicsObject("anchorGfx", -1, 10.0f, 5.0f, 20.0f, 20.0f, true, palette->bluesSat.cBlue2, this);
	}
	else
	{
		if (anchorPos.isOnTop)
			anchorGfx = new HD_UI_GraphicsObject("anchorGfx", -1, 5.0f, -10.0f, 20.0f, 20.0f, true, palette->bluesSat.cBlue2, this);
		else
			anchorGfx = new HD_UI_GraphicsObject("anchorGfx", -1, 5.0f, height - 10.0f, 20.0f, 20.0f, true, palette->bluesSat.cBlue2, this);
	}
}

void HD_UI_ButtonMenu::createBgGfx()
{
	//create the bg based on the y position
	//error prone if the bg is modified :( let's hope not
	if (globalY + height > HDScreen->nScreenH * 0.6f)
	{
		//create it with a gradient
		float pos1 = (globalY - HDScreen->nScreenH * 0.6f) / (HDScreen->nScreenH * 0.4f);
		float pos2 = ((globalY + height) - HDScreen->nScreenH * 0.6f) / (HDScreen->nScreenH * 0.4f);
		ALLEGRO_COLOR c1 = palette->getColorFromRange(palette->cBg1, palette->cBg2, pos1);
		ALLEGRO_COLOR c2 = palette->getColorFromRange(palette->cBg1, palette->cBg2, pos2);

		HD_UI_GraphicsObject *bgGfx = new HD_UI_GraphicsObject("bgGfx", -1, 0.0f, 0.0f, width, height, true, 0.0f, c1, c2, this);
	}
	else
	{
		HD_UI_GraphicsObject *bgGfx = new HD_UI_GraphicsObject("bgGfx", -1, 0.0f, 0.0f, width, height, -1.0f, palette->cBg1, this);
	}

	HD_UI_GraphicsObject *bgStroke = new HD_UI_GraphicsObject("bgStroke", -1, -1.0f, -1.0f, width + 1.0f, height, 1.0f, palette->bluesSat.cBlue2, this);
}

// Public methods
//=================

//Creates a Button Menu with only captions and no anchor; the font and buttonHeight are standard!
//Used by dropdown buttons
HD_UI_ButtonMenu::HD_UI_ButtonMenu(const char* objectName, int index, std::vector<std::string> captions, HD_UI_Container *newParent)
{
	float maxWidth = newParent->width;
	float maxHeight = 30.0f * captions.size();

	setObjectProperties(objectName, 0.0f, newParent->height + 1.0f, maxWidth, maxHeight, newParent, index);

	//GFX
	//BG
	createBgGfx();

	//Top Line
	HD_UI_GraphicsObject *line = new HD_UI_GraphicsObject("lineGfx", -1, -1.0f, -1.0f, width + 1.0f, -1.0f, 1.0f, palette->bluesSat.cBlue2, this);

	//buttons!
	for (unsigned int ii = 0; ii < captions.size(); ii++)
	{
		std::string btnName = "button";
		btnName.append(std::to_string(ii));

		HD_UI_Button *btn = new HD_UI_Button(btnName.c_str(), -1, captions[ii].c_str(), "", 0.0f, ii * 30.0f, width, 30.0f, palette->btnColors_blueSat, true,
			HDResources->font24, ALLEGRO_ALIGN_LEFT, this);
		btn->setCallback(std::bind(&HD_UI_ButtonMenu::defaultCallback, this));

		buttons.push_back(btn);
	}

	visible = false; //no need to show it after creation
}

//Creates a Button Menu with only captions; the font is 24 by standard
HD_UI_ButtonMenu::HD_UI_ButtonMenu(const char* objectName, int index, float buttonHeight, std::vector<std::string> captions, bool isAnchorHorizontal, HD_UI_Container *newParent)
{
	//first get the maximum size
	float maxWidth = 0.0f;
	float maxHeight = buttonHeight * captions.size();

	for (unsigned int ii = 0; ii < captions.size(); ii++)
	{
		float w = al_get_text_width(HDResources->font24, captions[ii].c_str());
		maxWidth = w > maxWidth ? w : maxWidth;
	}

	//add some padding
	maxWidth += 25.0f;

	setObjectProperties(objectName, 0.0f, 0.0f, maxWidth, maxHeight, newParent, index);

	//position it where there's enough space
	isHorizontal = isAnchorHorizontal;
	setCorrectPosition();

	//GFX
	
	//Anchor
	createAnchorGfx();

	//BG
	createBgGfx();

	//buttons!
	for (unsigned int ii = 0; ii < captions.size(); ii++)
	{
		std::string btnName = "button";
		btnName.append(std::to_string(ii));

		HD_UI_Button *btn = new HD_UI_Button(btnName.c_str(), -1, captions[ii].c_str(), "", 0.0f, ii * buttonHeight, width, buttonHeight, palette->btnColors_blueSat, true,
			HDResources->font24, ALLEGRO_ALIGN_LEFT, this);

		buttons.push_back(btn);
	}

	visible = false; //no need to show it after creation
}

//Creates a Button Menu with icons & captions on the buttons; the font is 24 by standard
//The first element of the icons vector is the atlas!
HD_UI_ButtonMenu::HD_UI_ButtonMenu(const char* objectName, int index, float buttonHeight, std::vector<std::string> captions, std::vector<std::string> iconNames,
	bool isAnchorHorizontal, HD_UI_Container *newParent)
{
	float maxWidth = 0.0f;
	float maxHeight = buttonHeight * captions.size();

	for (unsigned int ii = 0; ii < captions.size(); ii++)
	{
		float w = al_get_text_width(HDResources->font24, captions[ii].c_str());
		maxWidth = w > maxWidth ? w : maxWidth;
	}

	maxWidth += 25.0f;

	setObjectProperties(objectName, 0.0f, 0.0f, maxWidth, maxHeight, newParent, index);

	isHorizontal = isAnchorHorizontal;
	setCorrectPosition();

	//GFX
	createAnchorGfx();

	createBgGfx();

	for (unsigned int ii = 0; ii < captions.size(); ii++)
	{
		std::string btnName = "button";
		btnName.append(std::to_string(ii));

		HD_UI_Button *btn = new HD_UI_Button(btnName.c_str(), -1, captions[ii].c_str(), "", 0.0f, ii * buttonHeight, width, height, palette->btnColors_blueSat, true,
			HDResources->font24, iconNames[ii + 1].c_str(), iconNames[0].c_str(), this);

		buttons.push_back(btn);
	}

	visible = false;
}

//General functions
void HD_UI_ButtonMenu::Update()
{
	HD_UI_Container::Update();

	if (HDScreen->mouse->GetTarget() != this &&
		HDScreen->mouse->GetState()->buttons & 1)
		showMenu(false);
}

void HD_UI_ButtonMenu::defaultCallback()
{
	//keeps track of the pressed button in the selection
	//then hides the message
	lastSelection = name;
	showMenu(false);

	std::string debug = lastSelection;
	debug.append(" was the last selection.\n");
	OutputDebugStringA(debug.c_str());
}

void HD_UI_ButtonMenu::setCallbacks(std::vector<std::function<void()>> newCallbacks)
{
	for (unsigned int ii = 0; ii < newCallbacks.size(); ii++)
	{
		buttons[ii]->setCallback(newCallbacks[ii]);
	}
}

void HD_UI_ButtonMenu::setCallbacks(int index, std::function<void()> newCallback)
{
	buttons[index]->setCallback(newCallback);
}

void HD_UI_ButtonMenu::showMenu(bool show)
{
	if (show && !visible)
	{
		scaleY = 0.1f;
		alpha = 0.1f;

		CDBTweener::CTween *tween = new CDBTweener::CTween(&CDBTweener::TWEQ_CUBIC, CDBTweener::TWEA_OUT, 0.35f, &scaleY, 1.0f);
		tween->addValue(&alpha, 1.0f);
		addAnimation(tween, true);

		visible = true;
	}
	else if (!show && visible)
	{
		scaleY = 1.0f;
		alpha = 1.0f;

		CDBTweener::CTween *tween = new CDBTweener::CTween(&CDBTweener::TWEQ_CUBIC, CDBTweener::TWEA_OUT, 0.35f, &scaleY, 0.1f);
		tween->addValue(&alpha, 0.0f);
		addAnimation(tween, true);
	}
}