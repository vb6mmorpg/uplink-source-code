#include "HD_HUD_HighSecurityScreen.h"

#include "../../HD_Root.h"
#include "../../../HD_ColorPalettes.h"
#include "../../../HD_Resources.h"

#include "../../../UI_Objects/HD_UI_Button.h"
#include "../../../UI_Objects/HD_UI_GraphicsObject.h"
#include "../../../UI_Objects/HD_UI_TextObject.h"

#include "game/game.h"
#include "interface/interface.h"
#include "interface/remoteinterface/remoteinterface.h"

#include "world/world.h"
#include "world/player.h"
#include "world/computer/computer.h"
#include "world/computer/computerscreen/menuscreen.h"
#include "world/computer/computerscreen/highsecurityscreen.h"

HD_HUD_HighSecurityScreen::HD_HUD_HighSecurityScreen()
{
	setObjectProperties("highSecurityScreen", 0.0f, 0.0f, root->width / 2.0f, root->height);
}

void HD_HUD_HighSecurityScreen::Create(ComputerScreen *cScreen)
{
	HighSecurityScreen* hsScreen = (HighSecurityScreen*)cScreen;

	//maintitle
	std::shared_ptr<HD_UI_TextObject> mainTitle = std::make_shared<HD_UI_TextObject>();
	mainTitle->CreateSinglelineText("mainTitle", 12.0f, 132.0f, hsScreen->maintitle, palette->bluesSat.cBlue2, HDResources->font48);
	addChild(mainTitle);

	//subtitle
	std::shared_ptr<HD_UI_TextObject> subTitle = std::make_shared<HD_UI_TextObject>();
	subTitle->CreateSinglelineText("subTitle", 12.0f, 186.0f, hsScreen->subtitle, palette->blues.cBlue3, HDResources->font30);
	addChild(subTitle);

	//systems buttons
	for (int ii = 0; ii < hsScreen->systems.Size(); ii++)
	{
		MenuScreenOption* mso = hsScreen->systems.GetData(ii);

		//check if this has been already breached
		if (root->GetHUDMainWindow()->PreviousScreenID() == mso->nextpage &&
			game->GetInterface()->GetRemoteInterface()->security_level < 10 )
				mso->security = game->GetInterface()->GetRemoteInterface()->security_level;

		//start trace
		if (mso->security != 10)
			if (hsScreen->GetComputer()->security.IsRunning_Monitor())
				game->GetWorld()->GetPlayer()->GetConnection()->BeginTrace();
		
		//button
		std::shared_ptr<HD_UI_Button> btnSystem = std::make_shared<HD_UI_Button>();

		if (strcmp(mso->caption, "UserID / password verification") == 0 ||
			strcmp(mso->caption, "Admin Password verification") == 0)
				btnSystem->CreateImageButton("btnPassword", "btnPassword_s", "hud_atlas.png", "Verify your Credentials.", 70.0f, height / 2.0f);
		else if (strcmp(mso->caption, "Elliptic-Curve Encryption Cypher") == 0)
			btnSystem->CreateImageButton("btnCypher", "btnCypher_s", "hud_atlas.png", "Input your Encryption Cypher.", 390.0f, height / 2.0f);
		else
			btnSystem->CreateImageButton("btnVoice", "btnVoice_s", "hud_atlas.png", "Voice verification.", 710.0f, height / 2.0f);

		btnSystem->y -= btnSystem->height / 2.0f;
		addChild(btnSystem);

		//systemStatus
		std::shared_ptr<HD_UI_GraphicsObject> systemStatus = std::make_shared<HD_UI_GraphicsObject>();

		if (mso->security != 10)
			systemStatus->CreateImageObject("systemStatus", "icon_doneFill40x", "hud_atlas.png", 160.0f, -20.0f);
		else
			systemStatus->CreateImageObject("systemStatus", "icon_notDoneFill40x", "hud_atlas.png", 160.0f, -20.0f);

		addChild(systemStatus);
	}

	//proceed button
	std::shared_ptr<HD_UI_Button> btnProceed = std::make_shared<HD_UI_Button>();

	if (game->GetInterface()->GetRemoteInterface()->security_level < 10)
		btnProceed->CreateRectangleButton("btnProceed", "ACCESS GRANTED", "Proceed.", 320.0f, 785.0f, 320.0f, 45.0f,
		palette->btnColors_blueSat, false, HDResources->font30);
	else
	{
		btnProceed->CreateRectangleButton("btnProceed", "ACCESS DENIED", "Proceed.", 320.0f, 785.0f, 320.0f, 45.0f,
			palette->btnColors_blueSat, false, HDResources->font30);
		btnProceed->SetActive(false);
	}

	addChild(btnProceed);
	
	//bypass button
	if (HDResources->USECHEATS)
	{
		std::shared_ptr<HD_UI_Button> btnBypass = std::make_shared<HD_UI_Button>();
		btnBypass->CreateRectangleButton("btnBypass", "BYPASS", "Bypass screen.", width / 2.0f, height - 100.0f, 80.0f, 30.0f, palette->btnColors_blueSat, false);
		//btnBypass->setCallback(std::bind(&HD_HUD_PasswordScreen::bypassClick, this, passScreen->nextpage));
		addChild(btnBypass);
	}
}