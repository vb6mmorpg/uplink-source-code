//Implementation of MMOptions
//===========================

#include "HD_MMOptions.h"
#include "HD_Root.h"

#include "options/options.h"
#include "app/app.h"
#include "soundgarden.h"

#include "../UI_Objects/HD_UI_TextObject.h"
#include "../UI_Objects/HD_UI_Button.h"
#include "../UI_Objects/HD_UI_GraphicsObject.h"

#include "../HD_ColorPalettes.h"
#include "../HD_Resources.h"
#include "../HD_Screen.h"

//Private functions
//=================

std::vector<std::string> HD_MMOptions::getScreenModesStrings()
{
	std::vector<std::string> screenModesStrings;
	ALLEGRO_DISPLAY_MODE displayMode;
	int length = al_get_num_display_modes();

	screenModesStrings.push_back("Resolution");

	for (int ii = 0; ii < length; ii++)
	{
		al_get_display_mode(ii, &displayMode);
		std::string modeS = to_string(displayMode.width);
		modeS += "x";
		modeS += to_string(displayMode.height);
		modeS += " @";
		modeS += to_string(displayMode.refresh_rate);
		modeS += "HZ";

		screenModesStrings.push_back(modeS);
	}

	return screenModesStrings;
}

//Callbacks
//=================
void HD_MMOptions::applyClick()
{
	bool isFrameless = btnFrameless->getSwitchState();
	bool playMusic = btnMusic->getSwitchState();
	int displayModeID = btnResolution->getSelectionID();
	ALLEGRO_DISPLAY_MODE displayMode;
	al_get_display_mode(displayModeID, &displayMode);

	if (displayModeID >= 0)
		HDScreen->hd_setResolution(displayMode.width, displayMode.height);

	if (isFrameless != HDScreen->bFrameless)
		HDScreen->hd_setFrameless(isFrameless);

	if (playMusic)
	{
		SgPlaylist_Play("main");
	}
	else
	{
		SgPlaylist_Stop();
	}
	app->GetOptions()->SetOptionValue("sound_musicenabled", playMusic);

	app->GetOptions()->Save(NULL);

	ShowScreen(false);
}


//Public
//=================
void HD_MMOptions::Create()
{
	setObjectProperties("MMOptions", 0.0f, 0.0f, 320.0f, 1200.0f);

	std::shared_ptr<HD_UI_GraphicsObject> bgGfx = std::make_shared<HD_UI_GraphicsObject>();
	std::shared_ptr<HD_UI_GraphicsObject> bgStroke = std::make_shared<HD_UI_GraphicsObject>();
	std::shared_ptr<HD_UI_TextObject> settingsTitle = std::make_shared<HD_UI_TextObject>();
	std::shared_ptr<HD_UI_TextObject> displayTitle = std::make_shared<HD_UI_TextObject>();
	std::shared_ptr<HD_UI_TextObject> soundsTitle = std::make_shared<HD_UI_TextObject>();
	std::shared_ptr<HD_UI_Button> btnApply = std::make_shared<HD_UI_Button>();
	btnMusic = std::make_shared<HD_UI_ButtonSwitch>();
	btnFrameless = std::make_shared<HD_UI_ButtonSwitch>();
	btnResolution = std::make_shared<HD_UI_ButtonDropdown>();

	//BG
	bgGfx->CreateGradientRectangleObject("bgGfx", 0.0f, 0.0f, width, height, true, 0.6f, palette->cBg1, palette->cBg2);
	bgStroke->CreateRectangleObject("bgStroke", -1.0f, -1.0f, width + 1.0f, height + 1.0f, 1.0f, palette->bluesSat.cBlue2);
	addChild(bgGfx);
	addChild(bgStroke);

	//Titles
	settingsTitle->CreateSinglelineText("settingsTitle", 20.0f, 63.0f, "Settings", palette->bluesSat.cBlue2, HDResources->font30, ALLEGRO_ALIGN_LEFT);
	displayTitle->CreateSinglelineText("displayTitle", 20.0f, 154.0f, "Display", palette->bluesSat.cBlue2, HDResources->font30, ALLEGRO_ALIGN_LEFT);
	soundsTitle->CreateSinglelineText("soundsTitle", 20.0f, 424.0f, "Sounds", palette->bluesSat.cBlue2, HDResources->font30, ALLEGRO_ALIGN_LEFT);
	addChild(settingsTitle);
	addChild(displayTitle);
	addChild(soundsTitle);

	//Buttons
	bool isMusicOn = app->GetOptions()->IsOptionEqualTo("sound_musicenabled", 1);
	btnMusic->CreateSwitchButton("btnMusic", 20.0f, 478.0f, 275.0f, 30.0f, palette->btnSwitchColors_blueSat, "Music", "Turn Music On or Off",
		true, HDResources->font24, isMusicOn);
	addChild(btnMusic);

	bool isFrameless = app->GetOptions()->IsOptionEqualTo("graphics_fullscreen", 1);
	btnFrameless->CreateSwitchButton("btnFrameless", 20.0f, 270.0f, 275.0f, 30.0f, palette->btnSwitchColors_blueSat, "Frameless", "Frameless display?",
		true, HDResources->font24, isFrameless);
	addChild(btnFrameless);

	btnResolution->CreateDropdownButton("btnResolution", getScreenModesStrings(), "Display Resolution.", 20.0f, 210.0f,
		275.0f, 30.0f, palette->btnColors_blueSat, true, HDResources->font24);
	addChild(btnResolution);

	btnApply->CreateRectangleButton("btnApply", "Apply", "Apply selected settings.", 79.0f, 927.0f, 160.0f, 30.0f, palette->btnColors_blueSat, false,
		HDResources->font24, ALLEGRO_ALIGN_CENTER);
	btnApply->setCallback(std::bind(&HD_MMOptions::applyClick, this));
	addChild(btnApply);
}

void HD_MMOptions::Update()
{
	HD_UI_Container::Update();
	
	if (rootIsMouseTarget() && HDScreen->mouse->GetState()->buttons & 1)
		ShowScreen(false);
}

void HD_MMOptions::ShowScreen(bool show)
{
	if (show)
	{
		CDBTweener::CTween *tween = new CDBTweener::CTween(&CDBTweener::TWEQ_CUBIC, CDBTweener::TWEA_OUT, 0.35f, &x, 0.0f);
		addAnimation(tween, true);
	}
	else
	{
		CDBTweener::CTween *tween = new CDBTweener::CTween(&CDBTweener::TWEQ_CUBIC, CDBTweener::TWEA_OUT, 0.35f, &x, 0.0f - width - 5.0f);
		addAnimation(tween, true);
	}
}