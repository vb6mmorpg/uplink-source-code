// Implementation of the HUD
//==========================

#include "HD_HUD.h"
#include "../HD_Root.h"

#include "../../HD_Resources.h"
#include "../../HD_ColorPalettes.h"

#include "../../UI_Objects/HD_UI_GraphicsObject.h"
#include "../../UI_Objects/HD_UI_Button.h"

HD_HUD::HD_HUD()
{
	setObjectProperties("HUD", 0.0f, 0.0f, root->width, root->height);
	HDResources->hd_loadAtlasImage("hud_atlas.png");
}

HD_HUD::~HD_HUD()
{
	HDResources->hd_removeImage("hud_atlas.png");
}

void HD_HUD::Create()
{
	std::shared_ptr<HD_UI_GraphicsObject> bgGfx = std::make_shared<HD_UI_GraphicsObject>();
	bgGfx->CreateGradientRectangleObject("bgGfx", 0.0f, 0.0f, width, height, true, 0.6f, palette->cBg1, palette->cBg2);
	addChild(bgGfx);

	topBar = std::make_shared<HD_HUD_TopBar>();
	topBar->Create();
	addChild(topBar);

	mainWindow = std::make_shared<HD_HUD_MainWindow>();
	mainWindow->Create();
	addChild(mainWindow);

	bottomBar = std::make_shared<HD_HUD_BottomBar>();
	bottomBar->Create();
	addChild(bottomBar);

	//To-DO: world map; software menu; pop-ups
}

void HD_HUD::Update()
{
	HD_UI_Container::Update();

	//uplink update
	//world map
	//software menu

}

void HD_HUD::Clear()
{
	HD_UI_Container::Clear();
	HDResources->hd_removeImage("hud_atlas.png");
}