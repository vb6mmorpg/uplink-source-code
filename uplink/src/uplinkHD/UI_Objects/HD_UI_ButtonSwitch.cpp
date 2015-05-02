// Implementation of ButtonSwitch
//===============================

#include "HD_UI_ButtonSwitch.h"

//============================
// Protected Functions
//============================

//Destruction
HD_UI_ButtonSwitch::~HD_UI_ButtonSwitch()
{
	al_destroy_bitmap(standardImage);
	al_destroy_bitmap(hoverImage);
	al_destroy_bitmap(clickImage);
	al_destroy_bitmap(iconImage);
}

void HD_UI_ButtonSwitch::mouseRelease()
{
	gfxObject->setColors(stateColors[1], stateColors[5]);
	textObject->setTextColor(stateColors[3]);

	btnState = standard;

	isOn = !isOn;

	if (isOn)
	{
		switchObject->setColors(stateColors[5]);
		CDBTweener::CTween *tween = new CDBTweener::CTween(&CDBTweener::TWEQ_CUBIC, CDBTweener::TWEA_OUT, 0.25f, &switchObject->x, gfxObject->width - 40.0f);
		switchObject->addAnimation(tween, true);
	}
	else
	{
		switchObject->setColors(stateColors[6]);
		CDBTweener::CTween *tween = new CDBTweener::CTween(&CDBTweener::TWEQ_CUBIC, CDBTweener::TWEA_OUT, 0.25f, &switchObject->x, gfxObject->width - 80.0f);
		switchObject->addAnimation(tween, true);
	}

	if (isOn && !onCallback._Empty())
		onCallback();
	else if (!isOn && !offCallback._Empty())
		offCallback();
}

//============================
// Public Creation Functions
//============================

HD_UI_ButtonSwitch::HD_UI_ButtonSwitch(char *objectName, int index, float fX, float fY, float fWidth, float fHeight, ALLEGRO_COLOR colors[7],
	char *caption, char *tooltip, ALLEGRO_FONT *captionFont, bool isFilled, bool initState, HD_UI_Container *newParent)
{
	setObjectProperties(objectName, fX, fY, fWidth, fHeight, newParent, index);

	if (isFilled)
		gfxObject = new HD_UI_GraphicsObject("baseGFX", -1, 0.0f, 0.0f, fWidth, fHeight, -1.0f, colors[0], this);
	else
		gfxObject = new HD_UI_GraphicsObject("baseGFX", -1, 0.0f, 0.0f, fWidth, fHeight, 1.0f, colors[0], colors[5], this);

	textObject = new HD_UI_TextObject("caption", -1, 5.0f, 0.0f, caption, captionFont, colors[3], ALLEGRO_ALIGN_LEFT, this);
	textObject->y = height / 2 - textObject->height / 2;

	isOn = initState;

	if (isOn)
		switchObject = new HD_UI_GraphicsObject("switchGFX", -1, gfxObject->width - 40.0f, 0.0f, 40.0f, gfxObject->height, -1.0f, colors[5], this);
	else
		switchObject = new HD_UI_GraphicsObject("switchGFX", -1, gfxObject->width - 80.0f, 0.0f, 40.0f, gfxObject->height, -1.0f, colors[6], this);

	onText = new HD_UI_TextObject("onTxt", switchObject->index - 1, gfxObject->width - switchObject->width * 1.5f, 0.0f, "On", captionFont, colors[5], ALLEGRO_ALIGN_CENTER, this);
	offText = new HD_UI_TextObject("offTxt", switchObject->index - 1, gfxObject->width - switchObject->width / 2.0f, 0.0f, "Off", captionFont, colors[6], ALLEGRO_ALIGN_CENTER, this);
	onText->y = height / 2 - onText->height / 2;
	offText->y = height / 2 - offText->height / 2;

	for (unsigned int ii = 0; ii < 7; ii++)
		stateColors[ii] = colors[ii];

	onCallback = offCallback = buttonCallback = std::bind(&HD_UI_Button::defaultCallback, this);

	tooltipText = tooltip;
	tooltipObject = new HD_UI_Tooltip(tooltip, this);
	tooltipObject->visible = false;
}

void HD_UI_ButtonSwitch::setCallback(std::function<void()>newOnCallback, std::function<void()>newOffCallback)
{
	if (newOnCallback._Empty())
		onCallback = std::bind(&HD_UI_Button::defaultCallback, this);
	else
		onCallback = newOnCallback;

	if (newOffCallback._Empty())
		offCallback = std::bind(&HD_UI_Button::defaultCallback, this);
	else
		offCallback = newOffCallback;
}