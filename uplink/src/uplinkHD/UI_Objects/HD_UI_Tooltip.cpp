// Implementation of the Tooltip Object
//=====================================

#include "HD_UI_Tooltip.h"
#include "../HD_Resources.h"
#include "../HD_ColorPalettes.h"

#include "HD_UI_Button.h"
#include "../HD_Screen.h"

//HD_UI_Tooltip::HD_UI_Tooltip(const char *tooltip, HD_UI_Container *newParent)
void HD_UI_Tooltip::Create(const char *tooltip, float fX, float fY)
{
	setObjectProperties("tooltip", fX, fY, 0.0f, 0.0f);

	std::shared_ptr<HD_UI_GraphicsObject> bgObjectL = std::make_shared<HD_UI_GraphicsObject>();
	bgObjectL->CreateImageObject("bgL", "tooltip_L", "misc_atlas.png", 0.0f, 0.0f);
	addChild(bgObjectL);

	std::shared_ptr<HD_UI_TextObject> txtObject = std::make_shared<HD_UI_TextObject>();
	txtObject->CreateSinglelineText("txt", bgObjectL->width, 0.0f, tooltip, al_map_rgb_f(1.0f, 1.0f, 1.0f), HDResources->font24, 0);
	txtObject->y = bgObjectL->height / 2.0f - txtObject->height / 2.0f;
	addChild(txtObject);

	std::shared_ptr<HD_UI_GraphicsObject> bgObjectM = std::make_shared<HD_UI_GraphicsObject>();
	std::shared_ptr<HD_UI_GraphicsObject> bgObjectR = std::make_shared<HD_UI_GraphicsObject>();
	bgObjectM->CreateRectangleObject("bgM", bgObjectL->width, 0.0f, txtObject->getTextWidth(), bgObjectL->height, -1.0f, palette->bluesSat.cBlue2);
	bgObjectR->CreateImageObject("bgR", "tooltip_R", "misc_atlas.png", bgObjectM->x + bgObjectM->width, 0.0f);
	addChildAt(bgObjectM, 0);
	addChildAt(bgObjectR, 0);

	//width = bgObjectL->width + bgObjectM->width + bgObjectR->width;
	//height = bgObjectL->height;
}

void HD_UI_Tooltip::ShowTooltip(bool show)
{
	//don't do anything if it's already beeing shown
	if (visible == show) return;

	if (show)
	{
		x = HDScreen->mouse->GetState()->x;
		y = HDScreen->mouse->GetState()->y;

		//only buttons can show this!
		HD_UI_Button &btnTarget = *std::static_pointer_cast<HD_UI_Button>(HDScreen->mouse->GetTarget());
		std::string newTooltip = btnTarget.getTooltip();

		if (newTooltip.empty()) return;

		getTextObjectByName("txt")->setText(newTooltip.c_str());
		resize();

		alpha = 0.05f;
		CDBTweener::CTween *tween = new CDBTweener::CTween(&CDBTweener::TWEQ_LINEAR, CDBTweener::TWEA_OUT, 0.5f, &alpha, 1.0f);
		addAnimation(tween, true);
	}
	else alpha = 0.05f;

	visible = show;
}

void HD_UI_Tooltip::resize()
{
	getChildByName("bgM")->width = getTextObjectByName("txt")->getTextWidth();
	getChildByName("bgR")->x = getChildByName("bgM")->x + getChildByName("bgM")->width;
}