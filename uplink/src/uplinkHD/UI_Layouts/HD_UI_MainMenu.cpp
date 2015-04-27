//======================================
// The Maine Menu Layout.
// This is the first screen that appears
// upon opening the game. For now...
//======================================

#include "HD_UI_MainMenu.h"

#include "../HD_Screen.h"
#include "../HD_ColorPalettes.h"
#include "../HD_Resources.h"
#include "../UI_Objects/HD_UI_GraphicsObject.h"
#include "../UI_Objects/HD_UI_Button.h"
#include "../UI_Objects/HD_UI_TextObject.h"
#include "../UI_Objects/HD_UI_ButtonInput.h"
#include "../UI_Objects/HD_UI_PopUp.h"
#include "../UI_Objects/HD_UI_ButtonSwitch.h"

#include "../../app/app.h"
#include "../../game/game.h"
#include "../../world/world.h"

//Callback Functions
//==================
void HD_UI_MainMenu::pressedQuit()
{
	HD_UI_Object *popQuit = HDScreen->hd_getCurrentLayout()->getChildByName("quitPopup");
	popQuit->visible = true;
	popQuit->alpha = 1.0f;
}
void HD_UI_MainMenu::Quit()
{
	app->Close();
	HDScreen->HD_Quit();
}

//Public Functions
//==================

HD_UI_MainMenu::HD_UI_MainMenu()
{
	name = "MainMenu";
	parent = NULL;
	index = 0;

	x = 0.0f;
	y = 0.0f;
	globalX = 0.0f;
	globalY = 0.0f;

	drawWidth = width = HDScreen->nScreenW;
	drawHeight = height = HDScreen->nScreenH;

	globalScaleX = scaleX = 1.0f;
	globalScaleY = scaleY = 1.0f;

	drawAlpha = alpha = 1.0f;
	isVisible = visible = true;
}

void HD_UI_MainMenu::Create()
{
	//Background
	HD_UI_GraphicsObject *bgGFX = new HD_UI_GraphicsObject("background", -1, 0.0f, 0.0f, HDScreen->nScreenW, HDScreen->nScreenH,
		true, 0.6f, palette->warmGreys.cGrey0, palette->blues.cBlue1, this);

	//Title & logo
	HD_UI_GraphicsObject *logo = new HD_UI_GraphicsObject("logo", -1, "logo", "mainMenu_atlas.xml", HDScreen->anchors.tc.x, 0.0f, this);
	HD_UI_GraphicsObject *title = new HD_UI_GraphicsObject("title", -1, "logoTitle", "mainMenu_atlas.xml", HDScreen->anchors.tc.x, 0.0f, this);
	logo->x = HDScreen->anchors.tc.x - logo->width / 2;
	title->x = HDScreen->anchors.tc.x - title->width / 2;
	title->y = HDScreen->nScreenH * 0.32f;

	//Quit & Options buttons
	HD_UI_Button *btnExit = new HD_UI_Button("btnExit", -1, "btnExit_s", "mainMenu_atlas.xml",
		"Quit Uplink.", HDScreen->nScreenW * 0.05f, HDScreen->nScreenH * 0.05f, this);
	btnExit->setCallback(std::bind(&HD_UI_MainMenu::pressedQuit, this));

	HD_UI_Button *btnOptions = new HD_UI_Button("btnOptions", -1, "btnOptions_s", "mainMenu_atlas.xml",
		"Interface options.", btnExit->x + btnExit->width + 2.0f, btnExit->y, this);

	//testing:
	HD_UI_ButtonSwitch *switchTest = new HD_UI_ButtonSwitch("switchTest", -1, 200.0f, 200.0f, 200.0f, 30.0f, palette->btnSwitchColors_blueSat,
		"Switch", "Turn the switch on or off.", HDResources->font24, true, true, this);
	//

	//Password objects
	HD_UI_GraphicsObject *userPhoto = new HD_UI_GraphicsObject("userPhoto", -1, "userPhoto_big", "mainMenu_atlas.xml",
		HDScreen->nScreenW * 0.37f, HDScreen->nScreenH * 0.42f, this);

	HD_UI_ButtonInput *btnInUser = new HD_UI_ButtonInput("btnInUser", -1, "Username", "Enter your Username.",
		HDScreen->anchors.c.x, HDScreen->nScreenH * 0.43f, 248.0f, 45.0f, true, palette->btnInputColors_blueSat, HDResources->font30, this);

	HD_UI_ButtonInput *btnInPass = new HD_UI_ButtonInput("btnInPass", -1, "Password", "Enter your Password.", HDScreen->anchors.c.x,
		HDScreen->nScreenH * 0.52f, 248.0f, 45.0f, true, palette->btnInputColors_blueSat, HDResources->font30, this);

	HD_UI_Button *btnRegister = new HD_UI_Button("btnRegister", -1, "Register", "Register a new User.", btnInUser->x, HDScreen->nScreenH * 0.61f,
		160.0f, 30.0f, palette->btnColors_blueSat, false, HDResources->font24, ALLEGRO_ALIGN_CENTER, this);

	HD_UI_Button *btnSubmit = new HD_UI_Button("btnSubmit", -1, "btnSubmit_s", "mainMenu_atlas.xml", "Submit.", HDScreen->nScreenW * 0.61f, HDScreen->nScreenH * 0.605f, this);
	btnInPass->isPassword = true;

	//Previous users title
	HD_UI_GraphicsObject *lines = new HD_UI_GraphicsObject("lines", -1, -2.0f, HDScreen->nScreenH * 0.69, HDScreen->nScreenW + 4.0f, 60.0f, 1.0f, palette->bluesSat.cBlue2, this);

	HD_UI_TextObject *usersDesc = new HD_UI_TextObject("usersDescription", -1, HDScreen->nScreenW / 2.0f, lines->y + lines->height / 2.0f, "Other users from this Terminal:",
		HDResources->font30, palette->bluesSat.cBlue2, ALLEGRO_ALIGN_CENTER, this);
	usersDesc->y -= usersDesc->height / 2.0f;

	//User buttons; 4 maximum
	if (App::ListExistingGames()->ValidIndex(0))
		HD_UI_TextObject *usr0 = new HD_UI_TextObject("user0", -1, HDScreen->nScreenW / 2.0f, 900.0f, App::ListExistingGames()->GetData(0),
			HDResources->font30, palette->bluesSat.cBlue2, ALLEGRO_ALIGN_CENTER, this);

	//PopUps
	HD_UI_PopUp *popQuit = new HD_UI_PopUp("quitPopup", YESNO, "Shutdown", "Are you sure you want to shutdown Uplink?", std::bind(&HD_UI_MainMenu::Quit, this));
	popQuit->visible = false;
}