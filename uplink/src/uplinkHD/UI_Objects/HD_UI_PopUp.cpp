// Implementation of the PopUp
//============================

#include "HD_UI_PopUp.h"

#include "HD_UI_GraphicsObject.h"
#include "HD_UI_TextObject.h"
#include "HD_UI_Button.h"

#include "../UI_Layouts/HD_Root.h"
#include "../HD_ColorPalettes.h"
#include "../HD_Resources.h"

void HD_UI_PopUp::defaultCallback()
{
	//Just closes the PopUp
	ShowPopUp(false);
}

void HD_UI_PopUp::Create(char* objectName, PopUpType type, char *title, char *contents, std::function<void()> yesCallback)
{
	setObjectProperties(objectName, 0.0f, 0.0f, root->width, root->height);

	std::shared_ptr<HD_UI_GraphicsObject> overlay = std::make_shared<HD_UI_GraphicsObject>();
	std::shared_ptr<HD_UI_GraphicsObject> titleBG = std::make_shared<HD_UI_GraphicsObject>();
	std::shared_ptr<HD_UI_GraphicsObject> bodyBG = std::make_shared<HD_UI_GraphicsObject>();
	std::shared_ptr<HD_UI_GraphicsObject> bodyLine = std::make_shared<HD_UI_GraphicsObject>();
	std::shared_ptr<HD_UI_TextObject> titleTxt = std::make_shared<HD_UI_TextObject>();
	std::shared_ptr<HD_UI_TextObject> bodyTxt = std::make_shared<HD_UI_TextObject>();

	ALLEGRO_COLOR c = palette->addAlphaToColor(palette->warmGreys.cGrey0, 0.50f);
	overlay->CreateRectangleObject("overlay", 0.0f, 0.0f, width, height + 100.0f, 0.0f, c);
	addChild(overlay);

	//title
	titleBG->CreateShavedRectangleObject("titleBG", width / 2.0f - 240.0f, height / 2.0f - 135.0f,
		480.0f, 45.0f, true, topLeft, 30.0f, palette->bluesSat.cBlue2);
	titleTxt->CreateSinglelineText("titleTxt", titleBG->x + titleBG->width / 2.0f, titleBG->y + titleBG->height / 2.0f,
		title, palette->warmGreys.cGrey3, HDResources->font30, ALLEGRO_ALIGN_CENTER);
	titleTxt->y -= titleTxt->height / 2.0f;
	addChild(titleBG);
	addChild(titleTxt);

	//body
	bodyBG->CreateRectangleObject("bodyBG", titleBG->x, titleBG->y + titleBG->height, 480.0f, 225.0f, -1.0f, c);
	bodyLine->CreateLineObject("bodyLine", bodyBG->x, bodyBG->y + bodyBG->height, bodyBG->width, 0.0f, 2.0f, palette->bluesSat.cBlue2);
	bodyTxt->CreateMultilineText("bodyTxt", titleTxt->x, titleBG->y + titleBG->height + 30.0f, contents, palette->bluesSat.cBlue2,
		bodyBG->width - 40.0f, 0, 0.0f, HDResources->font24, ALLEGRO_ALIGN_CENTER);
	addChild(bodyBG);
	addChild(bodyLine);
	addChild(bodyTxt);

	//buttons
	if (type == OK)
	{
		std::shared_ptr<HD_UI_Button> btnOK = std::make_shared<HD_UI_Button>();
		btnOK->CreateRectangleButton("btnOK", "OK", "OK.", bodyBG->x + bodyBG->width / 2.0f - 80.0f, bodyBG->y + bodyBG->height - 60.0f,
			160.0f, 30.0f, palette->btnColors_blueSat, false, HDResources->font24, ALLEGRO_ALIGN_CENTER);

		if (yesCallback)
			btnOK->setCallback(yesCallback);
		else
			btnOK->setCallback(std::bind(&HD_UI_PopUp::defaultCallback, this));

		addChild(btnOK);
	}
	else if (type == OKCANCEL)
	{
		std::shared_ptr<HD_UI_Button> btnOK = std::make_shared<HD_UI_Button>();
		std::shared_ptr<HD_UI_Button> btnCancel = std::make_shared<HD_UI_Button>();

		btnOK->CreateRectangleButton("btnOK", "OK", "OK.", bodyBG->x + bodyBG->width * 0.75f - 80.0f, bodyBG->y + bodyBG->height - 60.0f,
			160.0f, 30.0f, palette->btnColors_blueSat, false, HDResources->font24, ALLEGRO_ALIGN_CENTER);
		btnOK->setCallback(yesCallback);

		btnCancel->CreateRectangleButton("btnCancel", "Cancel", "Cancel.", bodyBG->x + bodyBG->width / 4.0f - 80.0f, bodyBG->y + bodyBG->height - 60.0f,
			160.0f, 30.0f, palette->btnColors_blueSat, false, HDResources->font24, ALLEGRO_ALIGN_CENTER);
		btnCancel->setCallback(std::bind(&HD_UI_PopUp::defaultCallback, this));

		addChild(btnOK);
		addChild(btnCancel);
	}
	else if (type == YESNO)
	{
		std::shared_ptr<HD_UI_Button> btnYes = std::make_shared<HD_UI_Button>();
		std::shared_ptr<HD_UI_Button> btnNo = std::make_shared<HD_UI_Button>();

		btnYes->CreateRectangleButton("btnYes", "Yes", "Yes.", bodyBG->x + bodyBG->width * 0.75f - 80.0f, bodyBG->y + bodyBG->height - 60.0f,
			160.0f, 30.0f, palette->btnColors_blueSat, false, HDResources->font24, ALLEGRO_ALIGN_CENTER);
		btnYes->setCallback(yesCallback);

		btnNo->CreateRectangleButton("btnNo", "No", "Maybe not.", bodyBG->x + bodyBG->width / 4.0f - 80.0f, bodyBG->y + bodyBG->height - 60.0f,
			160.0f, 30.0f, palette->btnColors_blueSat, false, HDResources->font24, ALLEGRO_ALIGN_CENTER);
		btnNo->setCallback(std::bind(&HD_UI_PopUp::defaultCallback, this));

		addChild(btnYes);
		addChild(btnNo);
	}

	visible = false; //no need to show it on creation
}

void HD_UI_PopUp::ShowPopUp(bool show)
{
	if (show)
	{
		visible = true;
		y = -100.0f;
		CDBTweener::CTween *tween = new CDBTweener::CTween(&CDBTweener::TWEQ_CIRCULAR, CDBTweener::TWEA_OUT, 0.40f, &alpha, 1.0f);
		tween->addValue(&y, 0.0f);
		addAnimation(tween, true);
	}
	else
	{
		CDBTweener::CTween *tween = new CDBTweener::CTween(&CDBTweener::TWEQ_CIRCULAR, CDBTweener::TWEA_IN, 0.35f, &alpha, 0.0f);
		tween->addValue(&y, -100.0f);
		addAnimation(tween, true);
	}
}