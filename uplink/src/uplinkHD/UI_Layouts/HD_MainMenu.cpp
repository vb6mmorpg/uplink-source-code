//======================================
// The Maine Menu Layout.
// This is the first screen that appears
// upon opening the game. For now...
//======================================

#include "HD_MainMenu.h"
#include "HD_Root.h"

#include "../HD_Screen.h"
#include "../HD_ColorPalettes.h"
#include "../HD_Resources.h"
#include "../UI_Objects/HD_UI_GraphicsObject.h"
#include "../UI_Objects/HD_UI_Button.h"
#include "../UI_Objects/HD_UI_TextObject.h"
#include "../UI_Objects/HD_UI_PopUp.h"
#include "../UI_Objects/HD_UI_ButtonSwitch.h"
#include "../UI_Objects/HD_UI_ButtonDropdown.h"
#include "HD_Loading.h"

#include "../../app/app.h"
#include "../../game/game.h"
#include "../../world/world.h"

//Callback Functions
//==================
void HD_MainMenu::registerClicked()
{
	//To-Do when intro gets done
}

void HD_MainMenu::submitClicked(HD_UI_ButtonInput *btnInUser)
{
	std::string inputUser = btnInUser->getCaption();

	if (userExists(inputUser.c_str()))
	{
		app->SetNextLoadGame(btnInUser->getCaption());
		root->SetNewLayout("HUD", true, "Connecting to Gateway");
	}
	else
	{
		btnInUser->setCaption("Invalid Username");
		btnInUser->getTextObjectByName("caption")->setTextColor(palette->cRed);
	}
}

void HD_MainMenu::optionsClicked()
{
	options->ShowScreen(true);
}

void HD_MainMenu::exitClicked()
{
	getPopUpByName("quitPopup")->ShowPopUp(true);
}

void HD_MainMenu::Quit()
{
	app->Close();
	HDScreen->HD_Quit();
}

//Public Functions
//==================

HD_MainMenu::HD_MainMenu()
{
	setObjectProperties("MainMenu", 0.0f, 0.0f, root->width, root->height);
	index = gIndex = 0;
	HDResources->hd_loadAtlasImage("mainMenu_atlas.png");
}

HD_MainMenu::~HD_MainMenu()
{
	HDResources->hd_removeImage("mainMenu_atlas.png");
}

void HD_MainMenu::Create()
{
	//Background
	std::shared_ptr<HD_UI_GraphicsObject> bgGfx = std::make_shared<HD_UI_GraphicsObject>();
	bgGfx->CreateGradientRectangleObject("background", 0.0f, 0.0f, width, height, true, 0.6f, palette->cBg1, palette->cBg2);
	addChild(bgGfx);

	//Title & logo
	std::shared_ptr<HD_UI_GraphicsObject> logo = std::make_shared<HD_UI_GraphicsObject>();
	std::shared_ptr<HD_UI_GraphicsObject> title = std::make_shared<HD_UI_GraphicsObject>();
	logo->CreateImageObject("logo", "logo", "mainMenu_atlas.png", width / 2.0f, 0.0f);
	title->CreateImageObject("title", "logoTitle", "mainMenu_atlas.png", width / 2.0f, height * 0.32f);
	logo->x -= logo->width / 2;
	title->x -= title->width / 2;
	addChild(logo);
	addChild(title);

	//Quit & Options buttons
	std::shared_ptr<HD_UI_Button> btnExit = std::make_shared<HD_UI_Button>();
	std::shared_ptr<HD_UI_Button> btnOptions = std::make_shared<HD_UI_Button>();
	btnExit->CreateImageButton("btnExit", "btnExit_s", "mainMenu_atlas.png", "Quit Uplink.", width * 0.05f, height * 0.05f);
	btnExit->setCallback(std::bind(&HD_MainMenu::exitClicked, this));
	addChild(btnExit);

	btnOptions->CreateImageButton("btnOptions", "btnOptions_s", "mainMenu_atlas.png", "Interface options.", btnExit->x + btnExit->width + 2.0f, btnExit->y);
	btnOptions->setCallback(std::bind(&HD_MainMenu::optionsClicked, this));
	addChild(btnOptions);

	//Password objects
	std::shared_ptr<HD_UI_GraphicsObject> userPhoto = std::make_shared<HD_UI_GraphicsObject>();
	userPhoto->CreateImageObject("userPhoto", "userPhoto_big", "mainMenu_atlas.png", width * 0.37f, height * 0.42f);
	addChild(userPhoto);

	std::shared_ptr<HD_UI_ButtonInput> btnInUser = std::make_shared<HD_UI_ButtonInput>();
	btnInUser->CreateSinglelineInput("btnInUser", "Username", "Enter your Username.",
		width / 2.0f, height * 0.43f, 248.0f, 45.0f, true, palette->btnInputColors_blueSat, HDResources->font30);
	btnInUser->setCallback(std::bind(&HD_MainMenu::submitClicked, this, btnInUser.get()));
	addChild(btnInUser);

	std::shared_ptr<HD_UI_ButtonInput> btnInPass = std::make_shared<HD_UI_ButtonInput>();
	btnInPass->CreateSinglelineInput("btnInPass", "Password", "Enter your Password.", width / 2.0f,
		height * 0.52f, 248.0f, 45.0f, true, palette->btnInputColors_blueSat, HDResources->font30);
	btnInPass->isPassword = true;
	btnInPass->setCallback(std::bind(&HD_MainMenu::submitClicked, this, btnInUser.get()));
	addChild(btnInPass);

	std::shared_ptr<HD_UI_Button> btnRegister = std::make_shared<HD_UI_Button>();
	btnRegister->CreateRectangleButton("btnRegister", "Register", "Register a new User.", btnInUser->x, height * 0.61f,
		160.0f, 30.0f, palette->btnColors_blueSat, false, HDResources->font24, ALLEGRO_ALIGN_CENTER);
	btnRegister->setCallback(std::bind(&HD_MainMenu::registerClicked, this));
	addChild(btnRegister);

	std::shared_ptr<HD_UI_Button> btnSubmit = std::make_shared<HD_UI_Button>();
	btnSubmit->CreateImageButton("btnSubmit", "btnSubmit_s", "mainMenu_atlas.png", "Submit.", width * 0.61f, height * 0.605f);
	btnSubmit->setCallback(std::bind(&HD_MainMenu::submitClicked, this, btnInUser.get()));
	addChild(btnSubmit);

	//Previous users title
	std::shared_ptr<HD_UI_GraphicsObject> lines = std::make_shared<HD_UI_GraphicsObject>();
	lines->CreateRectangleObject("lines", -2.0f, height * 0.69, width + 4.0f, 60.0f, 1.0f, palette->bluesSat.cBlue2);
	addChild(lines);

	std::shared_ptr<HD_UI_TextObject> usersDesc = std::make_shared<HD_UI_TextObject>();
	usersDesc->CreateSinglelineText("usersDescription", width / 2.0f, lines->y + lines->height / 2.0f, "Other users from this Terminal:",
		palette->bluesSat.cBlue2, HDResources->font30, ALLEGRO_ALIGN_CENTER);
	usersDesc->y -= usersDesc->height / 2.0f;
	addChild(usersDesc);

	//User buttons; 4 maximum
	if (App::ListExistingGames()->ValidIndex(0))
	{
		std::shared_ptr<HD_UI_TextObject> usr0 = std::make_shared<HD_UI_TextObject>();
		usr0->CreateSinglelineText("user0", width / 2.0f, 900.0f, App::ListExistingGames()->GetData(0),
			palette->bluesSat.cBlue2, HDResources->font30, ALLEGRO_ALIGN_CENTER);
		addChild(usr0);
	}

	//Options
	options = std::make_shared<HD_MMOptions>();
	options->Create();
	options->x = 0.0f - options->width - 5.0f;
	addChild(options);

	//PopUps
	std::shared_ptr<HD_UI_PopUp> popQuit = std::make_shared<HD_UI_PopUp>();
	popQuit->Create("quitPopup", YESNO, "Shutdown", "Are you sure you want to shutdown Uplink?", std::bind(&HD_MainMenu::Quit, this));
	addChild(popQuit);
}

//general functions
void HD_MainMenu::Clear()
{
	HD_UI_Container::Clear();
	HDResources->hd_removeImage("mainMenu_atlas.png");
}

bool HD_MainMenu::userExists(const char* user)
{
	int length = App::ListExistingGames()->Size();

	if (length == 0) return false;

	for (int ii = 0; ii < length; ii++)
	{
		if (App::ListExistingGames()->ValidIndex(ii))
		{
			if (strcmp(user, App::ListExistingGames()->GetData(ii)) == 0)
				return true;
		}
	}

	return false;
}