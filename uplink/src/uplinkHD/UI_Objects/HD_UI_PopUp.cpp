// Implementation of the PopUp
//============================

#include "HD_UI_PopUp.h"

#include "HD_UI_GraphicsObject.h"
#include "HD_UI_TextObject.h"
#include "HD_UI_Button.h"

#include "../HD_Screen.h"
#include "../HD_ColorPalettes.h"
#include "../HD_Resources.h"

void HD_UI_PopUp::defaultCallback()
{
	//Just closes the PopUp

	alpha = 1.0f;
	CDBTweener::CTween *tween = new CDBTweener::CTween(&CDBTweener::TWEQ_CUBIC, CDBTweener::TWEA_OUT, 0.5f, &alpha, 0);
	addAnimation(tween, true);
}

HD_UI_PopUp::HD_UI_PopUp(char* objectName, PopUpType type, char *title, char *contents, std::function<void()> yesCallback)
{
	setObjectProperties(objectName, 0.0f, 0.0f, HDScreen->nScreenW, HDScreen->nScreenH, NULL, -1);

	ALLEGRO_COLOR c = palette->addAlphaToColor(palette->warmGreys.cGrey0, 0.50f);
	HD_UI_GraphicsObject *overlay = new HD_UI_GraphicsObject("overlay", -1, 0.0f, 0.0f, width, height, 0.0f,
		c, this);

	//title
	HD_UI_GraphicsObject *titleBG = new HD_UI_GraphicsObject("titleBG", -1, width / 2.0f - 240.0f, height / 2.0f - 135.0f,
		480.0f, 45.0f, true, topLeft, 30.0f, palette->bluesSat.cBlue2, this);
	HD_UI_TextObject *titleTxt = new HD_UI_TextObject("titleTxt", -1, titleBG->x + titleBG->width / 2.0f, titleBG->y + titleBG->height / 2.0f,
		title, HDResources->font30, palette->warmGreys.cGrey3, ALLEGRO_ALIGN_CENTER, this);
	titleTxt->y -= titleTxt->height / 2.0f;

	//body
	HD_UI_GraphicsObject *bodyBG = new HD_UI_GraphicsObject("bodyBG", -1, titleBG->x, titleBG->y + titleBG->height, 480.0f, 225.0f, c,
		false, 1.0f, palette->bluesSat.cBlue2, this);
	HD_UI_TextObject *bodyTxt = new HD_UI_TextObject("bodyTxt", -1, titleTxt->x, titleBG->y + titleBG->height + 30.0f, contents,
		HDResources->font24, palette->bluesSat.cBlue2, ALLEGRO_ALIGN_CENTER, bodyBG->width - 40.0f, 0.0f, this);

	//buttons
	if (type == OK)
	{
		HD_UI_Button *btnOK = new HD_UI_Button("btnOK", -1, "OK", "OK.", bodyBG->width / 2.0f - 80.0f, 60.0f, 160.0f, 30.0f, palette->btnColors_blueSat, false,
			HDResources->font24, ALLEGRO_ALIGN_CENTER, this);
		btnOK->setCallback(std::bind(&HD_UI_PopUp::defaultCallback, this));
	}
	else if (type == OKCANCEL)
	{
		HD_UI_Button *btnOK = new HD_UI_Button("btnOK", -1, "OK", "OK.", bodyBG->width * 0.75f - 80.0f, 60.0f,
			160.0f, 30.0f, palette->btnColors_blueSat, false, HDResources->font24, ALLEGRO_ALIGN_CENTER, this);
		btnOK->setCallback(yesCallback);

		HD_UI_Button *btnCancel = new HD_UI_Button("btnCancel", -1, "Cancel", "Cancel.", bodyBG->width / 4.0f - 80.0f, 60.0f,
			160.0f, 30.0f, palette->btnColors_blueSat, false, HDResources->font24, ALLEGRO_ALIGN_CENTER, this);
		btnCancel->setCallback(std::bind(&HD_UI_PopUp::defaultCallback, this));
	}
	else if (type == YESNO)
	{
		HD_UI_Button *btnYes = new HD_UI_Button("btnYes", -1, "Yes", "Yes.", bodyBG->x + bodyBG->width * 0.75f - 80.0f, bodyBG->y + bodyBG->height - 60.0f,
			160.0f, 30.0f, palette->btnColors_blueSat, false, HDResources->font24, ALLEGRO_ALIGN_CENTER, this);
		btnYes->setCallback(yesCallback);

		HD_UI_Button *btnNo = new HD_UI_Button("btnNo", -1, "No", "Maybe not.", bodyBG->x + bodyBG->width / 4.0f - 80.0f, bodyBG->y + bodyBG->height - 60.0f,
			160.0f, 30.0f, palette->btnColors_blueSat, false, HDResources->font24, ALLEGRO_ALIGN_CENTER, this);
		btnNo->setCallback(std::bind(&HD_UI_PopUp::defaultCallback, this));
	}
}