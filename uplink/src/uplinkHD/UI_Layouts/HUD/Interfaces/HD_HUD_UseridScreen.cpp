#include "HD_HUD_UseridScreen.h"

#include "../../HD_Root.h"
#include "../../../HD_ColorPalettes.h"
#include "../../../HD_Resources.h"

#include "../../../UI_Objects/HD_UI_Button.h"
#include "../../../UI_Objects/HD_UI_ButtonInput.h"
#include "../../../UI_Objects/HD_UI_TextObject.h"

#include "game/game.h"
#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"

#include "world/world.h"
#include "world/player.h"
#include "world/computer/computer.h"
#include "world/computer/logbank.h"

void HD_HUD_UseridScreen::submitClick(UserIDScreen* usrScreen)
{
	const char* name = getButtonInputByName("btnUsername")->getInputText();
	const char* code = getButtonInputByName("btnPassword")->getInputText();

	//this cast = bad; i know
	Record *rec = usrScreen->GetComputer()->recordbank.GetRecordFromNamePassword((char*)name, (char*)code);
	
	if (rec)
	{
		//we have access! and Copy! And Paste!
		int security;
		char *securitytext = rec->GetField(RECORDBANK_SECURITY);
		if (securitytext)
			sscanf(securitytext, "%d", &security);
		else
			security = 10;

		game->GetInterface()->GetRemoteInterface()->SetSecurity((char*)name, security);

		// Add this into the computer's logs

		AccessLog *log = new AccessLog();
		std::string logDetails = "User [";
		logDetails += name;
		logDetails += "] logged on (level " + security;
		logDetails += ")";

		//UplinkSnprintf(logdetails, sizeof(logdetails), "User [%s] logged on (level %d)", name, security);

		log->SetProperties(&(game->GetWorld()->date),
			game->GetWorld()->GetPlayer()->GetConnection()->GetGhost(), "PLAYER");
		log->SetData1((char*)logDetails.c_str());

		usrScreen->GetComputer()->logbank.AddLog(log);

		if (strcmp(name, "admin") == 0)
			game->GetWorld()->GetPlayer()->GetConnection()->BeginTrace();

		if (usrScreen->nextpage != -1)
			root->GetHUDMainWindow()->GoToScreen(usrScreen->nextpage);
	}
	else
	{
		//no we don't!
		getButtonInputByName("btnUsername")->setCaption("Incorrect username");
		getButtonInputByName("btnPassword")->setCaption("or password");

		//shake-it!
		CDBTweener::CTween *shakePass = new CDBTweener::CTween(&CDBTweener::TWEQ_CIRCULAR, CDBTweener::TWEA_OUT, 0.10f,
			&getChildByName("btnPassword")->x, getChildByName("btnPassword")->x - 20.0f);
		getChildByName("btnPassword")->addAnimation(shakePass, true, 2, 3);

		CDBTweener::CTween *shakeUser = new CDBTweener::CTween(&CDBTweener::TWEQ_CIRCULAR, CDBTweener::TWEA_OUT, 0.10f,
			&getChildByName("btnUsername")->x, getChildByName("btnUsername")->x - 20.0f);
		getChildByName("btnUsername")->addAnimation(shakeUser, true, 2, 3);
	}
}

void HD_HUD_UseridScreen::codeClick(const char* codeCaption)
{
	std::string caption = codeCaption;

	/*Uplink's format is
		1. ... 'name' ... 'code' ...
		2. ... 'code' ...

		So let's look at how many 's there are.
	*/

	unsigned int numDits = 0;
	unsigned int ditPositions[4] = { 0 };
	for (std::size_t ditPos = 0; ditPos < 256;)
	{
		if (ditPos + 1 < caption.length())
		{
			ditPos = caption.find('\'', ditPos + 1);
			numDits++;
			ditPositions[numDits - 1] = ditPos;
		}
		else
			ditPos = 257;
	}

	if (numDits == 2)
	{
		//we only have the code
		std::string code = caption.substr(ditPositions[0] + 1, ditPositions[1] - ditPositions[0] - 1);

		getButtonInputByName("btnUsername")->setInputText("admin");
		getButtonInputByName("btnPassword")->setInputText(code.c_str());
	}
	else if (numDits == 4)
	{
		//we have both name & code
		std::string name = caption.substr(ditPositions[0] + 1, ditPositions[1] - ditPositions[0] - 1);
		std::string code = caption.substr(ditPositions[2] + 1, ditPositions[3] - ditPositions[2] - 1);

		getButtonInputByName("btnUsername")->setInputText(name.c_str());
		getButtonInputByName("btnPassword")->setInputText(code.c_str());
	}

	//Thank God for strings!
}

void HD_HUD_UseridScreen::bypassClick(int nextPage)
{
	//start trace if needed
	//game->GetWorld()->GetPlayer()->GetConnection()->BeginTrace();

	if (nextPage != -1)
		root->GetHUDMainWindow()->GoToScreen(nextPage);
}

HD_HUD_UseridScreen::HD_HUD_UseridScreen()
{
	setObjectProperties("useridScreen", 0.0f, 0.0f, root->width / 2.0f, root->height);
}

void HD_HUD_UseridScreen::Create(ComputerScreen* cScreen)
{
	UserIDScreen* usrScreen = (UserIDScreen*)cScreen;

	//maintitle
	std::shared_ptr<HD_UI_TextObject> mainTitle = std::make_shared<HD_UI_TextObject>();
	mainTitle->CreateSinglelineText("mainTitle", 12.0f, 132.0f, usrScreen->maintitle, palette->bluesSat.cBlue2, HDResources->font48);
	addChild(mainTitle);

	//subtitle
	std::shared_ptr<HD_UI_TextObject> subTitle = std::make_shared<HD_UI_TextObject>();
	subTitle->CreateSinglelineText("subTitle", 12.0f, 186.0f, usrScreen->subtitle, palette->blues.cBlue3, HDResources->font30);
	addChild(subTitle);

	//lock icon
	std::shared_ptr<HD_UI_GraphicsObject> lockIcon = std::make_shared<HD_UI_GraphicsObject>();
	lockIcon->CreateImageObject("lockIcon", "wi_lock", "hud_atlas.png", width / 2.0f, height / 2.0f);
	lockIcon->x -= lockIcon->width / 2.0f;
	lockIcon->y -= lockIcon->height / 2.0f;
	addChild(lockIcon);

	//message
	std::shared_ptr<HD_UI_TextObject> messageTxt = std::make_shared<HD_UI_TextObject>();
	messageTxt->CreateSinglelineText("messageTxt", width / 2.0f, height / 3.0f, "Enter your credentials to continue.",
		palette->bluesSat.cBlue2, HDResources->font30, ALLEGRO_ALIGN_CENTER);
	addChild(messageTxt);

	//user photo
	std::shared_ptr<HD_UI_GraphicsObject> userPhoto = std::make_shared<HD_UI_GraphicsObject>();
	userPhoto->CreateImageObject("userPhoto", "userPhoto_big", "hud_atlas.png", width / 4.0f, height / 2.0f);
	userPhoto->x -= userPhoto->width / 2.0f;
	userPhoto->y -= userPhoto->height / 2.0f;
	addChild(userPhoto);

	//username
	std::shared_ptr<HD_UI_ButtonInput> btnUsername = std::make_shared<HD_UI_ButtonInput>();
	btnUsername->CreateSinglelineInput("btnUsername", "Username", "Enter your username.", width / 2.0f, height / 2.0f - 68.0f, 248.0f, 45.0f,
		true, palette->btnInputColors_blueSat, HDResources->font30);
	btnUsername->setCallback(std::bind(&HD_HUD_UseridScreen::submitClick, this, usrScreen));
	addChild(btnUsername);

	//password
	std::shared_ptr<HD_UI_ButtonInput> btnPassword = std::make_shared<HD_UI_ButtonInput>();
	btnPassword->CreateSinglelineInput("btnPassword", "Password", "Enter your password.", width / 2.0f, height / 2.0f + 24.0f, 248.0f, 45.0f,
		true, palette->btnInputColors_blueSat, HDResources->font30);
	btnPassword->isPassword = true;
	btnPassword->setCallback(std::bind(&HD_HUD_UseridScreen::submitClick, this, usrScreen));
	addChild(btnPassword);

	//submit
	std::shared_ptr<HD_UI_Button> btnSubmit = std::make_shared<HD_UI_Button>();
	btnSubmit->CreateImageButton("btnSubmit", "btnSubmit_s", "hud_atlas.png", "Submit.", btnPassword->x + btnPassword->width + 32.0f, btnPassword->y);
	btnSubmit->y += btnPassword->height / 2.0f - btnSubmit->height / 2.0f;
	btnSubmit->setCallback(std::bind(&HD_HUD_UseridScreen::submitClick, this, usrScreen));
	addChild(btnSubmit);

	//bypass button
	if (HDResources->USECHEATS)
	{
		std::shared_ptr<HD_UI_Button> btnBypass = std::make_shared<HD_UI_Button>();
		btnBypass->CreateRectangleButton("btnBypass", "BYPASS", "Bypass screen.", width / 2.0f, height - 100.0f, 80.0f, 30.0f, palette->btnColors_blueSat, false);
		btnBypass->setCallback(std::bind(&HD_HUD_UseridScreen::bypassClick, this, usrScreen->nextpage));
		addChild(btnBypass);
	}

	//known codes - Copy&Paste!
	if (game->GetWorld()->GetPlayer()->codes.LookupTree(usrScreen->GetComputer()->ip)) {

		DArray <char *> *codes = game->GetWorld()->GetPlayer()->codes.ConvertToDArray();
		DArray <char *> *ips = game->GetWorld()->GetPlayer()->codes.ConvertIndexToDArray();

		std::shared_ptr<HD_UI_TextObject> codesTxt = std::make_shared<HD_UI_TextObject>();
		codesTxt->CreateSinglelineText("codesTxt", width / 2.0f, height / 1.5f, "Known access codes:",
			palette->bluesSat.cBlue2, HDResources->font24, ALLEGRO_ALIGN_CENTER);
		addChild(codesTxt);

		int currentCode = 0;

		for (int i = 0; i < codes->Size(); ++i) {
			if (codes->ValidIndex(i) && ips->ValidIndex(i)) {
				if (strcmp(ips->GetData(i), usrScreen->GetComputer()->ip) == 0)
				{
					std::string btnName = "btnCode" + std::to_string(i);

					std::shared_ptr<HD_UI_Button> btnCode = std::make_shared<HD_UI_Button>();
					btnCode->CreateRectangleWIconButton(btnName.c_str(), codes->GetData(i), "Use this code.", 320.0f,
						codesTxt->y + 30.0f + currentCode * 45.0f + 2.0f, 320.0f, 45.0f, palette->btnColors_blueSat, true, "icon_User", "hud_atlas.png");
					btnCode->setCallback(std::bind(&HD_HUD_UseridScreen::codeClick, this, btnCode->getCaption()));
					addChild(btnCode);

					++currentCode;
				}
			}
		}

		delete codes;
		delete ips;

	}
}