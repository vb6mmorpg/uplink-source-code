#include "HD_HUD_PasswordScreen.h"

#include "../../../UI_Objects/HD_UI_Button.h"
#include "../../../UI_Objects/HD_UI_ButtonInput.h"
#include "../../../UI_Objects/HD_UI_TextObject.h"

#include "../../HD_Root.h"
#include "../../../HD_ColorPalettes.h"
#include "../../../HD_Resources.h"

#include "../../../../game/game.h"
#include "../../../../interface/interface.h"
#include "../../../../interface/remoteinterface/remoteinterface.h"

#include "world/world.h"
#include "world/player.h"
#include "world/computer/computer.h"
#include "world/computer/logbank.h"
#include "world/computer/computerscreen/passwordscreen.h"

void HD_HUD_PasswordScreen::submitClick(PasswordScreen* passScreen)
{
	//check the password
	if (strcmp(getButtonInputByName("btnPassword")->getInputText(), passScreen->password) == 0)
	{
		// Give the user max security clearance (only 1 access code for password screen)

		game->GetInterface()->GetRemoteInterface()->SetSecurity("Admin", 1);

		// Add this into the computer's logs

		AccessLog *log = new AccessLog();
		log->SetProperties(&(game->GetWorld()->date),
			game->GetWorld()->GetPlayer()->GetConnection()->GetGhost(), "PLAYER");
		log->SetData1("Password authentication accepted");
		passScreen->GetComputer()->logbank.AddLog(log);

		if (passScreen->nextpage != -1)
			root->GetHUDMainWindow()->GoToScreen(passScreen->nextpage);
	}
	else
	{
		getButtonInputByName("btnPassword")->setCaption("Incorrect password");

		//shake-it!
		CDBTweener::CTween *shake = new CDBTweener::CTween(&CDBTweener::TWEQ_CIRCULAR, CDBTweener::TWEA_OUT, 0.10f,
			&getChildByName("btnPassword")->x, getChildByName("btnPassword")->x - 20.0f);
		getChildByName("btnPassword")->addAnimation(shake, true, 2, 3);
	}
}

void HD_HUD_PasswordScreen::codeClick(const char* code)
{
	getButtonInputByName("btnPassword")->setInputText(code);
}

void HD_HUD_PasswordScreen::bypassClick(int nextPage)
{
	//start trace if needed
	//game->GetWorld()->GetPlayer()->GetConnection()->BeginTrace();

	if (nextPage != -1)
		root->GetHUDMainWindow()->GoToScreen(nextPage);
}

HD_HUD_PasswordScreen::HD_HUD_PasswordScreen()
{
	setObjectProperties("passwordScreen", 0.0f, 0.0f, root->width / 2.0f, root->height);
}

void HD_HUD_PasswordScreen::Create(ComputerScreen* cScreen)
{
	PasswordScreen* passScreen = (PasswordScreen*)cScreen;

	//maintitle
	std::shared_ptr<HD_UI_TextObject> mainTitle = std::make_shared<HD_UI_TextObject>();
	mainTitle->CreateSinglelineText("mainTitle", 12.0f, 132.0f, passScreen->maintitle, palette->bluesSat.cBlue2, HDResources->font48);
	addChild(mainTitle);

	//subtitle
	std::shared_ptr<HD_UI_TextObject> subTitle = std::make_shared<HD_UI_TextObject>();
	subTitle->CreateSinglelineText("subTitle", 12.0f, 186.0f, passScreen->subtitle, palette->blues.cBlue3, HDResources->font30);
	addChild(subTitle);

	//lock icon
	std::shared_ptr<HD_UI_GraphicsObject> lockIcon = std::make_shared<HD_UI_GraphicsObject>();
	lockIcon->CreateImageObject("lockIcon", "wi_lock", "hud_atlas.png", width / 2.0f, height / 2.0f);
	lockIcon->x -= lockIcon->width / 2.0f;
	lockIcon->y -= lockIcon->height / 2.0f;
	addChild(lockIcon);

	//message
	std::shared_ptr<HD_UI_TextObject> messageTxt = std::make_shared<HD_UI_TextObject>();
	messageTxt->CreateSinglelineText("messageTxt", width / 2.0f, height / 2.0f - 76.0f, "Enter password to continue.",
		palette->bluesSat.cBlue2, HDResources->font30, ALLEGRO_ALIGN_CENTER);
	addChild(messageTxt);

	//password
	std::shared_ptr<HD_UI_ButtonInput> btnPassword = std::make_shared<HD_UI_ButtonInput>();
	btnPassword->CreateSinglelineInput("btnPassword", "Password", "Enter your password.", 316.0f, height / 2.0f, 248.0f, 45.0f,
		true, palette->btnInputColors_blueSat, HDResources->font30);
	btnPassword->isPassword = true;
	btnPassword->setCallback(std::bind(&HD_HUD_PasswordScreen::submitClick, this, passScreen));
	addChild(btnPassword);

	//submit
	std::shared_ptr<HD_UI_Button> btnSubmit = std::make_shared<HD_UI_Button>();
	btnSubmit->CreateImageButton("btnSubmit", "btnSubmit_s", "hud_atlas.png", "Submit.", messageTxt->x + messageTxt->width / 2.0f - 40.0f, btnPassword->y);
	btnSubmit->y += btnPassword->height / 2.0f - btnSubmit->height / 2.0f;
	btnSubmit->setCallback(std::bind(&HD_HUD_PasswordScreen::submitClick, this, passScreen));
	addChild(btnSubmit);

	//bypass button
	if (HDResources->USECHEATS)
	{
		std::shared_ptr<HD_UI_Button> btnBypass = std::make_shared<HD_UI_Button>();
		btnBypass->CreateRectangleButton("btnBypass", "BYPASS", "Bypass screen.", width / 2.0f, height - 100.0f, 80.0f, 30.0f, palette->btnColors_blueSat, false);
		btnBypass->setCallback(std::bind(&HD_HUD_PasswordScreen::bypassClick, this, passScreen->nextpage));
		addChild(btnBypass);
	}

	//known codes
	BTree <char *> *btree = game->GetWorld()->GetPlayer()->codes.LookupTree(passScreen->GetComputer()->ip);

	if (btree)
	{
		std::shared_ptr<HD_UI_TextObject> codesTxt = std::make_shared<HD_UI_TextObject>();
		codesTxt->CreateSinglelineText("codesTxt", width / 2.0f, height / 1.5f, "Known access codes:",
			palette->bluesSat.cBlue2, HDResources->font24, ALLEGRO_ALIGN_CENTER);
		addChild(codesTxt);

		std::shared_ptr<HD_UI_Button> btnCode = std::make_shared<HD_UI_Button>();
		btnCode->CreateRectangleWIconButton("btnCode", btree->data, "Use this code.", 320.0f, codesTxt->y + 30.0f, 320.0f, 45.0f,
			palette->btnColors_blueSat, true, "icon_lock", "hud_atlas.png");
		btnCode->setCallback(std::bind(&HD_HUD_PasswordScreen::codeClick, this, btnCode->getCaption()));
		addChild(btnCode);
	}
}