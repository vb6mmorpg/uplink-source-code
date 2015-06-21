// Implementation of the Root object.
//===================================

#include "HD_Root.h"

#include "HD_MainMenu.h"
#include "HD_Loading.h"
#include "HUD/HD_HUD.h"

#include "../UI_Objects/HD_UI_Tooltip.h"

#include "../HD_Screen.h"

HD_Root::HD_Root()
{
	name = "root";
	parent = NULL;
	index = 0;
	gIndex = 0;

	x = 0.0f;
	y = 0.0f;
	globalX = 0.0f;
	globalY = 0.0f;

	drawWidth = width = HDScreen->nScreenW;
	drawHeight = height = HDScreen->nScreenH;

	globalScaleX = scaleX = 1.0f;// HDScreen->nScreenW / 1920.0f;
	globalScaleY = scaleY = 1.0f;// HDScreen->nScreenH / 1080.0f;

	drawAlpha = alpha = 1.0f;
	isVisible = visible = true;

	clippingMask = std::make_shared<HD_UI_Object>("rootMask", 0.0f, 0.0f, width, height);
}

void HD_Root::Create()
{
	//this is only called at startup, from HDScreen

	//To-Do if game_firsttime - start intro
	//else go to main menu
	SetNewLayout("MainMenu", false, NULL);

	//tooltip object
	std::shared_ptr<HD_UI_Tooltip> tooltip = std::make_shared<HD_UI_Tooltip>();
	tooltip->Create("Tooltip.", 0.0f, 0.0f);
	addChild(tooltip);
	tooltip->visible = false;
}

//scaling functions
float HD_Root::ScaleByWidth(float value)
{
	float origW = 1920; //the base W resolution of the UI
	value = (value / origW) * HDScreen->nScreenW;
	return value;
}

float HD_Root::ScaleByHeight(float value)
{
	float origH = 1080; //the base H resolution of the UI
	value = (value / origH) * HDScreen->nScreenH;
	return value;
}

void HD_Root::Rescale()
{
	drawWidth = width = HDScreen->nScreenW;
	drawHeight = height = HDScreen->nScreenH;

	globalScaleX = scaleX = HDScreen->nScreenW / 1920.0f;
	globalScaleY = scaleY = HDScreen->nScreenH / 1080.0f;
}

//layout functions
void HD_Root::SetNewLayout(const char *layoutName, bool useLoading, const char* loadingText)
{
	if (useLoading)
	{
		//first create the loading screen
		std::shared_ptr<HD_Loading> loading = std::make_shared<HD_Loading>(loadingText, layoutName, 4.0f);
		loading->Create();
		addChild(loading);

		//show the loading screen
		loading->alpha = 0.05f;
		CDBTweener::CTween* tween = new CDBTweener::CTween(&CDBTweener::TWEQ_CIRCULAR, CDBTweener::TWEA_OUT, 1.0f, &loading->alpha, 1.0f);
		loading->addAnimation(tween, true);
	}
	else
	{
		if (children.size() > 1)
		{
			//it means that the loading screen is also present
			//so erase only the old layout
			HDScreen->mouse->SetTarget(nullptr);
			removeChildFromIndex(layoutIndex);
			layoutIndex = 0;
		}

		if (strcmp(layoutName, "MainMenu") == 0)
		{
			std::shared_ptr<HD_MainMenu> layout = std::make_shared<HD_MainMenu>();
			addChild(layout);
			layout->Create();
			layoutIndex = layout->index;
		}
		else if (strcmp(layoutName, "HUD") == 0)
		{
			std::shared_ptr<HD_HUD> layout = std::make_shared<HD_HUD>();
			addChild(layout);
			layout->Create();
			layoutIndex = layout->index;
		}

		//reset the tooltip
		if (getChildByName("tooltip"))
			addChild(getChildByName("tooltip"));
	}
}

void HD_Root::ShowTooltip(bool show)
{
	std::static_pointer_cast<HD_UI_Tooltip>(getChildByName("tooltip"))->ShowTooltip(show);
}

std::shared_ptr<HD_Root> root;