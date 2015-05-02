// Implementation of ButtonInput
//==============================

#define TOPPADDING 5.0f
#define LEFTPADDING 5.0f
#define RIGHTPADDING 5.0f

#include "../HD_Screen.h"
#include "HD_UI_ButtonInput.h"

//============================
// Protected Functions
//============================

HD_UI_ButtonInput::~HD_UI_ButtonInput()
{
	al_destroy_bitmap(standardImage);
	al_destroy_bitmap(hoverImage);
	al_destroy_bitmap(clickImage);
	al_destroy_bitmap(iconImage);
}

void HD_UI_ButtonInput::mouseOut()
{
	HD_UI_Button::mouseOut();

	if (inputString.empty())
		textObject->setText(caption.c_str());
	else
		textObject->setTextColor(stateColors[5]);

	textCaret->visible = false;
}

void HD_UI_ButtonInput::mouseClick()
{
	HD_UI_Button::mouseClick();

	if (inputString.empty())
		textObject->setText(inputString.c_str());
		
	caretIndex = inputString.size();
	textCaret->x = textObject->x + textObject->getTextWidth(caretIndex);
}

void HD_UI_ButtonInput::mouseRelease()
{
	gfxObject->setColors(stateColors[0], stateColors[5]);

	if (inputString.empty())
	{
		textObject->setText(caption.c_str());
		textObject->setTextColor(stateColors[3]);
	}
	else
		textObject->setTextColor(stateColors[5]);

	btnState = over;

	textCaret->visible = false;

	if (buttonCallback != NULL && !inputString.empty())
		buttonCallback();
}

void HD_UI_ButtonInput::getInput()
{
	char uniKey = HDScreen->keyboard.uniKey;
	int key = HDScreen->keyboard.key;

	if (uniKey >= 32 && uniKey <= 126)
	{
		if (isPassword)
			inputString.insert(inputString.begin() + caretIndex, '*');
		else
			inputString.insert(inputString.begin() + caretIndex, uniKey);

		textObject->setText(inputString.c_str());
		caretIndex++;
		textCaret->visible = true;
	}

	//Special Keys
	if (key == ALLEGRO_KEY_BACKSPACE)
	{
		if (!inputString.empty() && caretIndex > 0)
		{
			inputString.erase(inputString.begin() + caretIndex - 1);
			caretIndex--;
		}
			
		textObject->setText(inputString.c_str());
		textCaret->visible = true;
	}

	if (key == ALLEGRO_KEY_DELETE)
	{
		if (!inputString.empty() && caretIndex < inputString.size())
			inputString.erase(inputString.begin() + caretIndex);

		textObject->setText(inputString.c_str());
		textCaret->visible = true;
	}

	if (key == ALLEGRO_KEY_END)
		caretIndex = inputString.size();
	if (key == ALLEGRO_KEY_HOME)
		caretIndex = 0;

	//Caret Handling
	//To-DO: Multiline support!
	if (key == ALLEGRO_KEY_LEFT && caretIndex > 0)
	{
		caretIndex--;
		textCaret->visible = true;
	}
	else if (key == ALLEGRO_KEY_RIGHT && caretIndex < inputString.size())
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
HD_UI_ButtonInput::HD_UI_ButtonInput(char *objectName, int index, char *caption, char *tooltip, float fX, float fY,
	float fWidth, float fHeight, bool isShaved, ALLEGRO_COLOR colors[6], ALLEGRO_FONT *captionFont, HD_UI_Container *newParent)
{
	setObjectProperties(objectName, fX, fY, fWidth, fHeight, newParent, index);

	if (isShaved)
	{
		gfxObject = new HD_UI_GraphicsObject("baseGFX", -1, 0.0f, 0.0f, fWidth, fHeight, true, bottomRight, fHeight / 2, colors[0], this);
		shavedStrokeObject = new HD_UI_GraphicsObject("strokeGFX", -1, 0.0f, 0.0f, fWidth, fHeight, false, bottomRight, fHeight / 2, colors[5], this);
	}
	else
		gfxObject = new HD_UI_GraphicsObject("baseGFX", -1, 0.0f, 0.0f, fWidth, fHeight, 1.0f, colors[0], colors[5], this);

	textObject = new HD_UI_TextObject("caption", -1, 0.0f, 0.0f, caption, captionFont, colors[3], ALLEGRO_ALIGN_LEFT, this);
	textObject->x = LEFTPADDING;
	textObject->y = height / 2 - textObject->height / 2;

	for (unsigned int ii = 0; ii < 6; ii++)
		stateColors[ii] = colors[ii];

	buttonCallback = std::bind(&HD_UI_Button::defaultCallback, this);

	HD_UI_ButtonInput::caption = caption;
	tooltipText = tooltip;
	tooltipObject = new HD_UI_Tooltip(tooltip, this);
	tooltipObject->visible = false;

	textCaret = new HD_UI_GraphicsObject("caret", -1, 0.0f, 0.0f, 0.0f, textObject->height, 2.0f, colors[4], this);
	textCaret->y = textObject->y;
	textCaret->visible = false;
}

//Multiline input
HD_UI_ButtonInput::HD_UI_ButtonInput(char *objectName, int index, char *caption, char *tooltip, float fX, float fY,
	float fWidth, float fHeight, ALLEGRO_COLOR colors[6], ALLEGRO_FONT *captionFont, HD_UI_Container *newParent)
{
	setObjectProperties(objectName, fX, fY, fWidth, fHeight, newParent, index);

	gfxObject = new HD_UI_GraphicsObject("baseGFX", -1, 0.0f, 0.0f, fWidth, fHeight, 1.0f, colors[0], colors[5], this);
	textObject = new HD_UI_TextObject("caption", -1, 0.0f, 0.0f, caption, captionFont, colors[3], ALLEGRO_ALIGN_LEFT, fWidth - RIGHTPADDING, 0.0f, this);
	textObject->x = LEFTPADDING;
	textObject->y = TOPPADDING;

	for (unsigned int ii = 0; ii < 6; ii++)
		stateColors[ii] = colors[ii];

	buttonCallback = std::bind(&HD_UI_Button::defaultCallback, this);

	HD_UI_ButtonInput::caption = caption;
	tooltipText = tooltip;
	tooltipObject = new HD_UI_Tooltip(tooltip, this);
	tooltipObject->visible = false;

	textCaret = new HD_UI_GraphicsObject("caret", -1, 0.0f, 0.0f, 0.0f, textObject->height, 2.0f, colors[4], this);
	textCaret->y = textObject->y;
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
	if (tooltipObject != NULL)
	{
		if (tooltipTimer > 1.5f && !tooltipObject->visible)
			tooltipObject->ShowTooltip(true);

		if (btnState == over)
			tooltipTimer += HDScreen->deltaTime;
	}
}

void HD_UI_ButtonInput::Clear()
{
	HD_UI_Container::Clear();

	caption.clear();
	inputString.clear();
}