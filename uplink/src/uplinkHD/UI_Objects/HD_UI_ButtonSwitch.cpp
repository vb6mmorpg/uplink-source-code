// Implementation of ButtonSwitch
//===============================

#include "HD_UI_ButtonSwitch.h"

//============================
// Protected Functions
//============================

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

//HD_UI_ButtonSwitch::HD_UI_ButtonSwitch(char *objectName, int index, float fX, float fY, float fWidth, float fHeight, ALLEGRO_COLOR colors[7],
//	char *caption, char *tooltip, ALLEGRO_FONT *captionFont, bool isFilled, bool initState, HD_UI_Container *newParent)
void HD_UI_ButtonSwitch::CreateSwitchButton(const char *objectName, float fX, float fY, float fWidth, float fHeight, ALLEGRO_COLOR colors[7],
	const char *caption, const char *tooltip, bool isFilled, ALLEGRO_FONT *captionFont,	bool initState)
{
	setObjectProperties(objectName, fX, fY, fWidth, fHeight);

	gfxObject = std::make_shared<HD_UI_GraphicsObject>();

	if (isFilled)
		gfxObject->CreateRectangleObject("baseGFX", 0.0f, 0.0f, fWidth, fHeight, -1.0f, colors[0]);
	else
		gfxObject->CreateSFRectangleObject("baseGFX", 0.0f, 0.0f, fWidth, fHeight, 1.0f, colors[0], colors[5]);

	addChild(gfxObject);

	textObject = std::make_shared<HD_UI_TextObject>();
	textObject->CreateSinglelineText("caption", 5.0f, 0.0f, caption, colors[3], captionFont, ALLEGRO_ALIGN_LEFT);
	textObject->y = height / 2 - textObject->height / 2;
	addChild(textObject);

	isOn = initState;
	switchObject = std::make_shared<HD_UI_GraphicsObject>();

	if (isOn)
		switchObject->CreateRectangleObject("switchGFX",gfxObject->width - 40.0f, 0.0f, 40.0f, gfxObject->height, -1.0f, colors[5]);
	else
		switchObject->CreateRectangleObject("switchGFX", gfxObject->width - 80.0f, 0.0f, 40.0f, gfxObject->height, -1.0f, colors[6]);

	addChild(switchObject);

	std::shared_ptr<HD_UI_TextObject> onText = std::make_shared<HD_UI_TextObject>();
	onText->CreateSinglelineText("onTxt", gfxObject->width - switchObject->width * 1.5f, 0.0f, "On", colors[5], captionFont, ALLEGRO_ALIGN_CENTER);

	std::shared_ptr<HD_UI_TextObject> offText = std::make_shared<HD_UI_TextObject>();
	offText->CreateSinglelineText("offTxt", gfxObject->width - switchObject->width / 2.0f, 0.0f, "Off", colors[6], captionFont, ALLEGRO_ALIGN_CENTER);
	onText->y = height / 2 - onText->height / 2;
	offText->y = height / 2 - offText->height / 2;

	addChildAt(onText, switchObject->index - 1);
	addChildAt(offText, switchObject->index - 1);

	for (unsigned int ii = 0; ii < 7; ii++)
		stateColors[ii] = colors[ii];

	onCallback = offCallback = buttonCallback = std::bind(&HD_UI_Button::defaultCallback, this);

	sTooltip = tooltip;
	//setTooltip(tooltip);
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