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
		x = 10.0f;
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
	std::shared_ptr<HD_UI_GraphicsObject> anchorGfx = std::make_shared<HD_UI_GraphicsObject>();

	if (isHorizontal)
	{
		if (anchorPos.isOnRight && anchorPos.isOnTop)			//top-right
			anchorGfx->CreateDiamondObject("anchorGfx", width - 10.0f, 0.0f, 20.0f, 20.0f, true, palette->bluesSat.cBlue2);
		else if (!anchorPos.isOnRight && !anchorPos.isOnTop)	//bottom-left
			anchorGfx->CreateDiamondObject("anchorGfx", -10.0f, height - 20.0f, 20.0f, 20.0f, true, palette->bluesSat.cBlue2);
		else if (anchorPos.isOnRight && !anchorPos.isOnTop)		//bottom-right
			anchorGfx->CreateDiamondObject("anchorGfx", width - 10.0f, height - 20.0f, 20.0f, 20.0f, true, palette->bluesSat.cBlue2);
		else if (!anchorPos.isOnRight && anchorPos.isOnTop)		//top-left
			anchorGfx->CreateDiamondObject("anchorGfx", 10.0f, 0.0f, 20.0f, 20.0f, true, palette->bluesSat.cBlue2);
	}
	else
	{
		if (anchorPos.isOnTop)
			anchorGfx->CreateDiamondObject("anchorGfx", 0.0f, -10.0f, 20.0f, 20.0f, true, palette->bluesSat.cBlue2);
		else
			anchorGfx->CreateDiamondObject("anchorGfx", 0.0f, height - 10.0f, 20.0f, 20.0f, true, palette->bluesSat.cBlue2);
	}

	addChild(anchorGfx);
}

// Public methods
//=================

//Creates a Button Menu with only captions and no anchor; the font and buttonHeight are standard!
//Used by dropdown buttons
void HD_UI_ButtonMenu::CreateButtonMenu(const char* objectName, float fX, float fY, float maxWidth, std::vector<std::string> captions)
{
	float maxHeight = 30.0f * captions.size();

	setObjectProperties(objectName, fX, fY, maxWidth, maxHeight);

	//GFX
	//BG
	std::shared_ptr<HD_UI_GraphicsObject> baseGfx = std::make_shared<HD_UI_GraphicsObject>();
	baseGfx->CreateBGGradientRectangleObject("baseGfx", 0.0f, 0.0f, width, height, true, palette->bluesSat.cBlue2);
	addChild(baseGfx);

	//Top Line
	std::shared_ptr<HD_UI_GraphicsObject> line = std::make_shared<HD_UI_GraphicsObject>();
	line->CreateLineObject("lineGfx", 0.0f, -0.5f, width, 0.0f, 1.0f, palette->bluesSat.cBlue2);
	addChild(line);

	//buttons!
	for (unsigned int ii = 0; ii < captions.size(); ii++)
	{
		std::string *btnName = new std::string("button");
		btnName->append(std::to_string(ii));

		std::shared_ptr<HD_UI_Button> btn = std::make_shared<HD_UI_Button>();
		btn->CreateRectangleButton(btnName->c_str(), captions[ii].c_str(), "", 0.0f, ii * 30.0f, width, 30.0f, palette->btnColors_blueSat, true,
			HDResources->font24, ALLEGRO_ALIGN_LEFT);

		buttons.push_back(btn);
		buttons[buttons.size() - 1]->setCallback(std::bind(&HD_UI_ButtonMenu::defaultCallback, this, ii));
		addChild(btn);
	}

	visible = false; //no need to show it after creation
}

//Creates a Button Menu with only captions; the font is 24 by standard
void HD_UI_ButtonMenu::CreateAnchoredButtonMenu(const char* objectName, float buttonHeight, std::vector<std::string> captions,
	bool isAnchorHorizontal)
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

	setObjectProperties(objectName, 0.0f, 0.0f, maxWidth, maxHeight);

	//position it where there's enough space
	isHorizontal = isAnchorHorizontal;
	setCorrectPosition();

	//GFX
	
	//Anchor
	createAnchorGfx();

	//BG
	std::shared_ptr<HD_UI_GraphicsObject> baseGfx = std::make_shared<HD_UI_GraphicsObject>();
	baseGfx->CreateBGGradientRectangleObject("baseGfx", 0.0f, 0.0f, width, height, true, palette->bluesSat.cBlue2);
	addChild(baseGfx);

	//buttons!
	for (unsigned int ii = 0; ii < captions.size(); ii++)
	{
		std::string *btnName = new std::string("button");
		btnName->append(std::to_string(ii));

		std::shared_ptr<HD_UI_Button> btn = std::make_shared<HD_UI_Button>();
		btn->CreateRectangleButton(btnName->c_str(), captions[ii].c_str(), "", 0.0f, ii * buttonHeight, width, buttonHeight, palette->btnColors_blueSat, true,
			HDResources->font24, ALLEGRO_ALIGN_LEFT);

		buttons.push_back(btn);
		buttons[buttons.size() - 1]->setCallback(std::bind(&HD_UI_ButtonMenu::defaultCallback, this, ii));
		addChild(btn);
	}

	visible = false; //no need to show it after creation
}

//Creates a Button Menu with icons & captions on the buttons; the font is 24 by standard
//The first element of the icons vector is the atlas!
void HD_UI_ButtonMenu::CreateAnchoredButtonWIconMenu(const char* objectName, float buttonHeight, std::vector<std::string> captions, std::vector<std::string> iconNames,
	bool isAnchorHorizontal)
{
	float maxWidth = 0.0f;
	float maxHeight = buttonHeight * captions.size();

	for (unsigned int ii = 0; ii < captions.size(); ii++)
	{
		float w = al_get_text_width(HDResources->font24, captions[ii].c_str());
		maxWidth = w > maxWidth ? w : maxWidth;
	}

	maxWidth += 25.0f;

	setObjectProperties(objectName, 0.0f, 0.0f, maxWidth, maxHeight);

	isHorizontal = isAnchorHorizontal;
	setCorrectPosition();

	//GFX
	createAnchorGfx();

	std::shared_ptr<HD_UI_GraphicsObject> baseGfx = std::make_shared<HD_UI_GraphicsObject>();
	baseGfx->CreateBGGradientRectangleObject("baseGfx", 0.0f, 0.0f, width, height, true, palette->bluesSat.cBlue2);
	addChild(baseGfx);

	for (unsigned int ii = 0; ii < captions.size(); ii++)
	{
		std::string *btnName = new std::string("button");
		btnName->append(std::to_string(ii));

		std::shared_ptr<HD_UI_Button> btn = std::make_shared<HD_UI_Button>();
		btn->CreateRectangleButton(btnName->c_str(), captions[ii].c_str(), "", 0.0f, ii * buttonHeight, width, buttonHeight, palette->btnColors_blueSat, true,
			HDResources->font24, ALLEGRO_ALIGN_LEFT);

		buttons.push_back(btn);
		buttons[buttons.size() - 1]->setCallback(std::bind(&HD_UI_ButtonMenu::defaultCallback, this, ii));
		addChild(btn);
	}

	visible = false;
}

//General functions
void HD_UI_ButtonMenu::Update()
{
	HD_UI_Container::Update();

	if (!isMouseTarget() &&	HDScreen->mouse->GetState()->buttons & 1)
		showMenu(false);
}

void HD_UI_ButtonMenu::defaultCallback(unsigned int id)
{
	//keeps track of the pressed button in the selection
	//then hides the message
	lastSelectionName = buttons[id]->name;
	lastSelectionID = id;
	showMenu(false);

	std::string debug = lastSelectionName;
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