// Implementation of the Top Bar
//==============================

#include "HD_HUD_TopBar.h"

#include "../../HD_Root.h"
#include "../../../HD_ColorPalettes.h"
#include "../../../HD_Screen.h"

#include "../../../UI_Objects/HD_UI_Button.h"
#include "../../../UI_Objects/HD_UI_GraphicsObject.h"
#include "../../../UI_Objects/HD_UI_ButtonMenu.h"
#include "../../../UI_Objects/HD_UI_PopUp.h"

//Old Uplink stuff
#include "../../../../game/game.h"
#include "../../../../world/world.h"
#include "../../../../interface/interface.h"
#include "../../../../interface/remoteinterface/remoteinterface.h"
#include "../../../../app/app.h"
#include "../../../../options/options.h"
#include "../../../../mainmenu/mainmenu.h"
#include "../../../../world/player.h"
#include "../../../../world/vlocation.h"

//Callbacks
//============
//shutdown button
void HD_HUD_TopBar::exitClicked()
{
	getButtonByName("btnExit")->getButtonMenuByName("btnExitMenu")->showMenu(true);
}
void HD_HUD_TopBar::exitMenuClicked(unsigned int id)
{
	if (id == 1) return; //don't work right now

	// Normal Uplink Cleanup
	//SgPlaySound(RsArchiveFileOpen("sounds/close.wav"), "sounds/close.wav", false);

	// Close any connections
	game->GetWorld()->GetPlayer()->GetConnection()->Disconnect();
	game->GetWorld()->GetPlayer()->GetConnection()->Reset();

	game->GetInterface()->GetRemoteInterface()->RunNewLocation();
	game->GetInterface()->GetRemoteInterface()->RunScreen(0);

	// Save game and log off
	app->SaveGame(game->GetWorld()->GetPlayer()->handle);
	game->SetGameSpeed(GAMESPEED_PAUSED);
	app->GetMainMenu()->RunScreen(MAINMENU_LOGIN);
	EclReset(app->GetOptions()->GetOptionValue("graphics_screenwidth"),
		app->GetOptions()->GetOptionValue("graphics_screenheight"));

	if (id == 0)
	{
		//log out
		root->SetNewLayout("MainMenu", false, NULL);
	}
	else if (id == 1)
	{
		//shut down TO-DO
		//app->Close();
		//HDScreen->HD_Quit();
	}
}
//speed buttons
void HD_HUD_TopBar::speedButtonClicked(unsigned int id)
{
	switch (id)
	{
		case 0:
			//pause
			game->SetGameSpeed(GAMESPEED_PAUSED);
			root->GetLayout()->getPopUpByName("pausePopup")->ShowPopUp(true);
			break;
		case 1:
			game->SetGameSpeed(GAMESPEED_NORMAL);
			break;
		case 2:
			game->SetGameSpeed(GAMESPEED_FAST);
			break;
		case 3:
			game->SetGameSpeed(GAMESPEED_MEGAFAST);
			break;
		case 4:
			//unpause
			game->SetGameSpeed(GAMESPEED_NORMAL);
			root->GetLayout()->getPopUpByName("pausePopup")->ShowPopUp(false);
			break;
	}
}

//Generic
//============
HD_HUD_TopBar::HD_HUD_TopBar()
{
	setObjectProperties("topBar", 0.0f, 0.0f, 800.0f, 36.0f);
}

void HD_HUD_TopBar::Create()
{
	//bg
	std::shared_ptr<HD_UI_GraphicsObject> bgGfx = std::make_shared<HD_UI_GraphicsObject>();
	bgGfx->CreateShavedSFRectangleObject("bgGfx", -1.0f, -1.0f, width + 1.0f, height + 1.0f, bottomRight, height,
		palette->cBg1, palette->bluesSat.cBlue2);
	addChild(bgGfx);

	//Shutdown button
	std::shared_ptr<HD_UI_Button> btnExit = std::make_shared<HD_UI_Button>();
	std::shared_ptr<HD_UI_ButtonMenu> btnExitMenu = std::make_shared<HD_UI_ButtonMenu>();
	btnExit->CreateImageButton("btnExit", "btnExit_s", "hud_atlas.png", "Quit or Log Out.", 3.0f, 0.0f);
	btnExit->setCallback(std::bind(&HD_HUD_TopBar::exitClicked, this));
	addChild(btnExit);

	std::vector<std::string> exitCaptions = { "Log Out", "Shutdown" };
	btnExit->addChild(btnExitMenu);
	btnExitMenu->CreateAnchoredButtonMenu("btnExitMenu", 40.0f, exitCaptions, false);
	btnExitMenu->setCallbacks(0, std::bind(&HD_HUD_TopBar::exitMenuClicked, this, 0));
	btnExitMenu->setCallbacks(1, std::bind(&HD_HUD_TopBar::exitMenuClicked, this, 1));

	//Settings button
	std::shared_ptr<HD_UI_Button> btnSettings = std::make_shared<HD_UI_Button>();
	btnSettings->CreateImageButton("btnSettings", "btnSettings_s", "hud_atlas.png", "Hardware manager.", 45.0f, 0.0f);
	addChild(btnSettings);

	//Time & Date
	timeTxt = std::make_shared<HD_UI_TextObject>();
	timeTxt->CreateSinglelineText("timeTxt", 93.0f, 0.0f, "TIME", palette->bluesSat.cBlue2, HDResources->font18);
	addChild(timeTxt);

	dateTxt = std::make_shared<HD_UI_TextObject>();
	dateTxt->CreateSinglelineText("dateTxt", 93.0f, 16.0f, "DATE", palette->bluesSat.cBlue2, HDResources->font18);
	addChild(dateTxt);

	//Gamespeed buttons
	std::shared_ptr<HD_UI_Button> btnSpeed0 = std::make_shared<HD_UI_Button>();
	std::shared_ptr<HD_UI_Button> btnSpeed1 = std::make_shared<HD_UI_Button>();
	std::shared_ptr<HD_UI_Button> btnSpeed2 = std::make_shared<HD_UI_Button>();
	std::shared_ptr<HD_UI_Button> btnSpeed3 = std::make_shared<HD_UI_Button>();
	btnSpeed0->CreateImageButton("btnSpeed0", "btnSpeed0_s", "hud_atlas.png", "Pause Game.", 263.0f, 6.0f);
	btnSpeed1->CreateImageButton("btnSpeed1", "btnSpeed1_s", "hud_atlas.png", "Normal speed.", 287.0f, 6.0f);
	btnSpeed2->CreateImageButton("btnSpeed2", "btnSpeed2_s", "hud_atlas.png", "Double speed.", 311.0f, 6.0f);
	btnSpeed3->CreateImageButton("btnSpeed3", "btnSpeed3_s", "hud_atlas.png", "Triple speed.", 335.0f, 6.0f);
	addChild(btnSpeed0); addChild(btnSpeed1); addChild(btnSpeed2); addChild(btnSpeed3);

	btnSpeed0->setCallback(std::bind(&HD_HUD_TopBar::speedButtonClicked, this, 0));
	btnSpeed1->setCallback(std::bind(&HD_HUD_TopBar::speedButtonClicked, this, 1));
	btnSpeed2->setCallback(std::bind(&HD_HUD_TopBar::speedButtonClicked, this, 2));
	btnSpeed3->setCallback(std::bind(&HD_HUD_TopBar::speedButtonClicked, this, 3));

	//CPU Usage
	std::shared_ptr<HD_UI_TextObject> cpuTxt = std::make_shared<HD_UI_TextObject>();
	std::shared_ptr<HD_UI_GraphicsObject> cpuBase = std::make_shared<HD_UI_GraphicsObject>();
	std::shared_ptr<HD_UI_GraphicsObject> cpuFill = std::make_shared<HD_UI_GraphicsObject>();
	cpuTxt->CreateSinglelineText("cpuTxt", 377.0f, 1.0f, "CPU Usage", palette->bluesSat.cBlue2, HDResources->font18);
	cpuBase->CreateRectangleObject("cpuBase", 377.0f, 23.0f, 92.0f, 8.0f, -1.0f, palette->blues.cBlue3);
	cpuFill->CreateRectangleObject("cpuFill", 377.0f, 23.0f, 92.0f, 8.0f, -1.0f, palette->bluesSat.cBlue2);
	addChildAt(cpuTxt, 1); addChildAt(cpuBase, 1); addChildAt(cpuFill, 2);

	//Location
	locNameTxt = std::make_shared<HD_UI_TextObject>();
	locIPTxt = std::make_shared<HD_UI_TextObject>();
	locNameTxt->CreateSinglelineText("locNameTxt", 490.0f, 3.0f, "LOCATION", palette->bluesSat.cBlue2);
	locIPTxt->CreateSinglelineText("locIPTxt", 490.0f, 43.0f, "LOCATION IP", palette->blues.cBlue3, HDResources->font18);
	addChildAt(locNameTxt, 1); addChildAt(locIPTxt, 1);

	//Pause PopUp
	std::shared_ptr<HD_UI_PopUp> pausePopup = std::make_shared<HD_UI_PopUp>();
	pausePopup->Create("pausePopup", OK, "Game paused", "The Game is now paused. Click OK to continue.",
		std::bind(&HD_HUD_TopBar::speedButtonClicked, this, 4));
	root->GetLayout()->addChild(pausePopup);
}

void HD_HUD_TopBar::Update()
{
	HD_UI_Container::Update();

	std::string sHour = game->GetWorld()->date.GetHour() < 10 ? "0" + std::to_string(game->GetWorld()->date.GetHour()) : std::to_string(game->GetWorld()->date.GetHour());
	std::string sMinute = game->GetWorld()->date.GetMinute() < 10 ? "0" + std::to_string(game->GetWorld()->date.GetMinute()) : std::to_string(game->GetWorld()->date.GetMinute());
	std::string sTime = sHour + ":" + sMinute;
	timeTxt->setText(sTime.c_str());
	
	std::string sDate = game->GetWorld()->date.GetLongString();
	sDate = sDate.substr(10);
	dateTxt->setText(sDate.c_str());

	std::string slocName = game->GetWorld()->GetPlayer()->GetRemoteHost()->computer;
	std::string slocIP = game->GetWorld()->GetPlayer()->GetRemoteHost()->ip;
	locNameTxt->setText(slocName.c_str());
	locIPTxt->setText(slocIP.c_str());

	//ToDo: cpu usage
}