// Implementation of ButtonInput
//==============================

#define TOPPADDING 5.0f
#define LEFTPADDING 5.0f
#define RIGHTPADDING 5.0f

#include "../HD_Screen.h"
#include "../UI_Layouts/HD_Root.h"

#include "HD_UI_ButtonInput.h"

//============================
// Protected Functions
//============================
void HD_UI_ButtonInput::mouseOut()
{
	HD_UI_Button::mouseOut();

	if (sInput.empty())
		textObject->setText(sCaption.c_str());
	else
		textObject->setTextColor(stateColors[5]);

	textCaret->visible = false;
}

void HD_UI_ButtonInput::mouseClick()
{
	HD_UI_Button::mouseClick();

	if (sInput.empty())
		textObject->setText(sInput.c_str());
		
	caretIndex = sInput.size();
	textCaret->x = textObject->x + textObject->getTextWidth(caretIndex);
}

void HD_UI_ButtonInput::mouseRelease()
{
	gfxObject->setColors(stateColors[0], stateColors[5]);

	if (sInput.empty())
	{
		textObject->setText(sCaption.c_str());
		textObject->setTextColor(stateColors[3]);
	}
	else
		textObject->setTextColor(stateColors[5]);

	btnState = over;

	textCaret->visible = false;

	if (buttonCallback != NULL && !sInput.empty())
		buttonCallback();
}

void HD_UI_ButtonInput::getInput()
{
	char uniKey = HDScreen->keyboard.uniKey;
	int key = HDScreen->keyboard.key;

	if (uniKey >= 32 && uniKey <= 126)
	{
		if (isPassword)
			sPassword.insert(sPassword.begin() + caretIndex, '*');
		
		sInput.insert(sInput.begin() + caretIndex, uniKey);

		//if (isPassword)
		//	textObject->setText(sPassword.c_str());
		//else
		//	textObject->setText(sInput.c_str());

		textObject->setText(isPassword ? sPassword.c_str() : sInput.c_str());

		caretIndex++;
		textCaret->visible = true;
	}

	//Special Keys
	if (key == ALLEGRO_KEY_BACKSPACE)
	{
		if (!sInput.empty() && caretIndex > 0)
		{
			if (isPassword)
				sPassword.erase(sPassword.begin() + caretIndex - 1);

			sInput.erase(sInput.begin() + caretIndex - 1);
			caretIndex--;
		}
			
		textObject->setText(isPassword ? sPassword.c_str() : sInput.c_str());
		textCaret->visible = true;
	}

	if (key == ALLEGRO_KEY_DELETE)
	{
		if (!sInput.empty() && caretIndex < sInput.size())
		{
			if (isPassword)
				sPassword.erase(sPassword.begin() + caretIndex);

			sInput.erase(sInput.begin() + caretIndex);
		}

		textObject->setText(isPassword ? sPassword.c_str() : sInput.c_str());
		textCaret->visible = true;
	}

	if (key == ALLEGRO_KEY_END)
		caretIndex = sInput.size();
	if (key == ALLEGRO_KEY_HOME)
		caretIndex = 0;

	//Caret Handling
	//To-DO: Multiline support!
	if (key == ALLEGRO_KEY_LEFT && caretIndex > 0)
	{
		caretIndex--;
		textCaret->visible = true;
	}
	else if (key == ALLEGRO_KEY_RIGHT && caretIndex < sInput.size())
	{
		caretIndex++;
		textCaret->visible = true;
	}

	textCaret->x = textObject->x + textObject->getTextWidth(caretIndex);
}

//============================
// Public Creation Functions
//============================

//Singleline input
void HD_UI_ButtonInput::CreateSinglelineInput(const char *objectName, const char *caption, const char *tooltip, float fX, float fY,
	float fWidth, float fHeight, bool isShaved, ALLEGRO_COLOR colors[6], ALLEGRO_FONT *captionFont)
{
	setObjectProperties(objectName, fX, fY, fWidth, fHeight);

	gfxObject = std::make_shared<HD_UI_GraphicsObject>();

	if (isShaved)
	{
		gfxObject->CreateShavedSFRectangleObject("baseGFX", 0.0f, 0.0f, fWidth, fHeight, bottomRight, fHeight / 2, colors[0], colors[5]);
	}
	else
		gfxObject->CreateSFRectangleObject("baseGFX", 0.0f, 0.0f, fWidth, fHeight, 1.0f, colors[0], colors[5]);

	addChild(gfxObject);

	textObject = std::make_shared<HD_UI_TextObject>();
	textObject->CreateSinglelineText("caption", 0.0f, 0.0f, caption, colors[3], captionFont, ALLEGRO_ALIGN_LEFT);
	textObject->x = LEFTPADDING;
	textObject->y = height / 2 - textObject->height / 2;
	addChild(textObject);

	for (unsigned int ii = 0; ii < 6; ii++)
		stateColors[ii] = colors[ii];

	buttonCallback = std::bind(&HD_UI_Button::defaultCallback, this);

	sCaption = caption;
	sTooltip = tooltip;
	//setTooltip(tooltip);

	textCaret = std::make_shared<HD_UI_GraphicsObject>();
	textCaret->CreateLineObject("caret", 0.0f, 0.0f, 0.0f, textObject->height, 2.0f, colors[4]);
	textCaret->y = textObject->y;
	addChild(textCaret);
	textCaret->visible = false;
}

//Multiline input
void HD_UI_ButtonInput::CreateMultilineInput(const char *objectName, const char *caption, const char *tooltip, float fX, float fY,
	float fWidth, float fHeight, ALLEGRO_COLOR colors[6], ALLEGRO_FONT *captionFont)
{
	setObjectProperties(objectName, fX, fY, fWidth, fHeight);

	gfxObject = std::make_shared<HD_UI_GraphicsObject>();
	gfxObject->CreateSFRectangleObject("baseGFX", 0.0f, 0.0f, fWidth, fHeight, 1.0f, colors[0], colors[5]);
	addChild(gfxObject);

	textObject = std::make_shared<HD_UI_TextObject>();
	textObject->CreateMultilineText("caption", 0.0f, 0.0f, caption, colors[3], fWidth - RIGHTPADDING, 0.0f, 0, captionFont);
	textObject->x = LEFTPADDING;
	textObject->y = TOPPADDING;
	addChild(textObject);

	for (unsigned int ii = 0; ii < 6; ii++)
		stateColors[ii] = colors[ii];

	buttonCallback = std::bind(&HD_UI_Button::defaultCallback, this);

	sCaption = caption;
	sTooltip = tooltip;
	//setTooltip(tooltip);

	textCaret = std::make_shared<HD_UI_GraphicsObject>();
	textCaret->CreateLineObject("caret", 0.0f, 0.0f, 0.0f, textObject->height, 2.0f, colors[4]);
	textCaret->y = textObject->y;
	addChild(textCaret);
	textCaret->visible = false;
}

//============================
// General Functions
//============================

void HD_UI_ButtonInput::Update()
{
	HD_UI_Container::Update();

	if (isMouseTarget() && btnState == standard)
		mouseOver();

	if (!isMouseTarget() && btnState == over)
		mouseOut();

	bool isPrimaryDown = (HDScreen->mouse->GetState()->buttons & 1);
	bool isPrimaryReleased = !isPrimaryDown;

	if (isPrimaryDown && btnState == over)
		mouseClick();

	//Get input from user
	if (btnState == clicked)
	{
		getInput();

		static float caretTimer = 0.0f;
		if (caretTimer > 0.75f)
		{
			textCaret->visible = !textCaret->visible;
			caretTimer = 0.0f;
		}

		caretTimer += HDScreen->deltaTime;
	}

	//If ENTER is pressed, fire the callback
	if (btnState == clicked && HDScreen->keyboard.key == ALLEGRO_KEY_ENTER)
		mouseRelease();

	//If the user clicks anywhere else or hits Escape: exit input
	if (isPrimaryDown && btnState == clicked && !isMouseTarget() ||
		HDScreen->keyboard.key == ALLEGRO_KEY_ESCAPE && btnState == clicked)
		mouseOut();

	//Tooltip
	if (!sTooltip.empty())
	{
		if (tooltipTimer > 1.5f)// && !tooltipObject->visible)
			root->ShowTooltip(true);

		if (btnState == over)
			tooltipTimer += HDScreen->deltaTime;
	}
}

void HD_UI_ButtonInput::setCaption(const char* newCaption)
{
	sCaption = newCaption;
	sInput.clear();
	textObject->setText(sCaption.c_str());
	mouseOut();
}

void HD_UI_ButtonInput::setInputText(const char* newInput)
{
	sInput.clear();
	sPassword.clear();

	sInput = newInput;

	if (isPassword)
	{
		for (unsigned ii = 0; ii < sInput.length(); ii++)
			sPassword += '*';

		textObject->setText(sPassword.c_str());
	}
	else
		textObject->setText(sInput.c_str());

	mouseOut();
}