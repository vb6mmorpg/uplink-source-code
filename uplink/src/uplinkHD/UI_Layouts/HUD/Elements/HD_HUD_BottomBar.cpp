#include "HD_HUD_BottomBar.h"

#include "../../HD_Root.h"
#include "../../../HD_ColorPalettes.h"

#include "../../../UI_Objects/HD_UI_GraphicsObject.h"
#include "../../../UI_Objects/HD_UI_Button.h"

#include "../Elements/HD_HUD_EmailNotif.h"

HD_HUD_BottomBar::HD_HUD_BottomBar()
{
	setObjectProperties("bottomBar", 0.0f, root->height - 60.0f, root->width, 60.0f);
}

void HD_HUD_BottomBar::Create()
{
	//bg
	std::shared_ptr<HD_UI_GraphicsObject> bgGfx = std::make_shared<HD_UI_GraphicsObject>();
	bgGfx->CreateSFRectangleObject("bgGfx", -1.0f, 0.0f, width + 1.0f, height, 1.0f, palette->blues.cBlue1, palette->bluesSat.cBlue2);
	addChild(bgGfx);

	//buttons
	std::shared_ptr<HD_UI_Button> btnSoftware = std::make_shared<HD_UI_Button>();
	btnSoftware->CreateImageButton("btnSoftware", "btnSoftware_s", "hud_atlas.png", "", 0.0f, 0.0f);
	addChild(btnSoftware);

	std::shared_ptr<HD_UI_Button> btnMemory = std::make_shared<HD_UI_Button>();
	btnMemory->CreateImageButton("btnMemory", "btnMemory_s", "hud_atlas.png", "Files.", 76.0f, 0.0f);
	addChild(btnMemory);

	std::shared_ptr<HD_UI_Button> btnStatus = std::make_shared<HD_UI_Button>();
	btnStatus->CreateImageButton("btnStatus", "btnStatus_s", "hud_atlas.png", "Files.", 146.0f, 0.0f);
	addChild(btnStatus);

	std::shared_ptr<HD_UI_Button> btnEmail = std::make_shared<HD_UI_Button>();
	btnEmail->CreateImageButton("btnEmail", "btnEmail_s", "hud_atlas.png", "Files.", 1860.0f, 0.0f);
	addChild(btnEmail);

	//e-mail notifs
	std::shared_ptr<HD_HUD_EmailNotif> emailNotif = std::make_shared<HD_HUD_EmailNotif>();
	emailNotif->Create();
	btnEmail->addChild(emailNotif);

	//To-Do: add upgrades; softwareMenu;
}

void HD_HUD_BottomBar::Update()
{
	HD_UI_Container::Update();

	//To-Do: update upgrades
}