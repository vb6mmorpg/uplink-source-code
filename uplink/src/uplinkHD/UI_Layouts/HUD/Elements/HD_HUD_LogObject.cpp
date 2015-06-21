#include "HD_HUD_LogObject.h"

#include "../../../UI_Objects/HD_UI_GraphicsObject.h"
#include "../../../UI_Objects/HD_UI_Button.h"
#include "../../../UI_Objects/HD_UI_TextObject.h"

#include "../../HD_Root.h"
#include "../../../HD_ColorPalettes.h"
#include "../../../HD_Resources.h"

#include "game/game.h"
#include "world/world.h"
#include "world/player.h"
#include "world/computer/computer.h"

void HD_HUD_LogObject::logClick()
{
	if (game->GetWorld()->GetPlayer()->GetRemoteHost()->GetComputer()->security.IsRunning_Proxy())
	{
		//To-DO show a PopUp
		//root->GetHUDMainWindow()->
		return;
	}
}

void HD_HUD_LogObject::Create(const char* objectName, char* date, char* description)
{
	setObjectProperties(objectName, 0.0f, 0.0f, 932.0f, 44.0f);

	sDate = date;
	sDescription = description;

	//separator
	std::shared_ptr<HD_UI_GraphicsObject> lineGfx = std::make_shared<HD_UI_GraphicsObject>();
	lineGfx->CreateLineObject("lineGfx", 0.0f, 43.5f, width, 0.0f, 1.0f, palette->blues.cBlue3);
	addChild(lineGfx);

	//bgButton / connect
	std::shared_ptr<HD_UI_Button> btnBase = std::make_shared<HD_UI_Button>();
	btnBase->CreateRectangleButton("btnBase", "", "Select this Log.", 0.0f, 0.0f, width, height, palette->btnColors_blueSat, true);
	addChild(btnBase);
	btnBase->setCallback(std::bind(&HD_HUD_LogObject::logClick, this));

	//texts
	std::shared_ptr<HD_UI_TextObject> dateTxt = std::make_shared<HD_UI_TextObject>();
	dateTxt->CreateSinglelineText("dateTxt", 0.0f, 6.0f, sDate.c_str(), palette->blues.cBlue3, HDResources->font30);
	addChild(dateTxt);

	std::shared_ptr<HD_UI_TextObject> descriptionTxt = std::make_shared<HD_UI_TextObject>();
	descriptionTxt->CreateSinglelineText("descriptionTxt", 240.0f, 6.0f, sDescription.c_str(), palette->bluesSat.cBlue2, HDResources->font30);
	addChild(descriptionTxt);
}

void HD_HUD_LogObject::Update()
{
	HD_UI_Container::Update();

	if (getContainerByName("btnBase")->hasMouseFocus() && !textColorsSet)
	{
		getTextObjectByName("dateTxt")->setTextColor(palette->warmGreys.cGrey3);
		getTextObjectByName("descriptionTxt")->setTextColor(palette->warmGreys.cGrey3);
		textColorsSet = true;
	}
	else if (!getContainerByName("btnBase")->hasMouseFocus() && textColorsSet)
	{
		getTextObjectByName("dateTxt")->setTextColor(palette->blues.cBlue3);
		getTextObjectByName("descriptionTxt")->setTextColor(palette->bluesSat.cBlue2);
		textColorsSet = false;
	}
}