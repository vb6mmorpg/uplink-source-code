#include "HD_HUD_DisconnectScreen.h"

#include "../../HD_Root.h"
#include "../../../HD_Resources.h"
#include "../../../HD_ColorPalettes.h"

#include "../../../UI_Objects/HD_UI_Button.h"
#include "../../../UI_Objects/HD_UI_TextObject.h"

#include "../../../../game/game.h"
#include "../../../../world/world.h"
#include "../../../../world/computer/computer.h"
#include "../../../../world/computer/computerscreen/disconnectedscreen.h"

HD_HUD_DisconnectScreen::HD_HUD_DisconnectScreen()
{
	setObjectProperties("disconnectScreen", 0.0f, 0.0f, root->width / 2.0f, root->height);
}

void HD_HUD_DisconnectScreen::Create(ComputerScreen* cScreen)
{
	DisconnectedScreen* discScreen = (DisconnectedScreen*)cScreen;

	//maintitle
	std::shared_ptr<HD_UI_TextObject> mainTitle = std::make_shared<HD_UI_TextObject>();
	mainTitle->CreateSinglelineText("mainTitle", 12.0f, 132.0f, discScreen->maintitle, palette->bluesSat.cBlue2, HDResources->font48);
	addChild(mainTitle);

	//subtitle
	std::shared_ptr<HD_UI_TextObject> subTitle = std::make_shared<HD_UI_TextObject>();
	subTitle->CreateSinglelineText("subTitle", 12.0f, 186.0f, discScreen->subtitle, palette->blues.cBlue3, HDResources->font30);
	addChild(subTitle);

	//message
	std::shared_ptr<HD_UI_TextObject> messageTxt = std::make_shared<HD_UI_TextObject>();
	messageTxt->CreateMultilineText("messageTxt", width / 2.0f, 360.0f, discScreen->textmessage, palette->bluesSat.cBlue2, width - 160.0f, 0, 0, HDResources->font30, ALLEGRO_ALIGN_CENTER);
	addChild(messageTxt);

	//Logins message
	if (DisconnectedScreen::loginslost.Size() > 0)
	{
		std::shared_ptr<HD_UI_TextObject> loginsLost = std::make_shared<HD_UI_TextObject>();
		loginsLost->CreateMultilineText("loginsLost", 160.0f, messageTxt->y + messageTxt->height + 60.0f, discScreen->textmessage,
			palette->bluesSat.cBlue2, width - 160.0f, 0, 0, HDResources->font30, ALLEGRO_ALIGN_LEFT);
		addChild(loginsLost);

		std::string sLogins = "Your username and password was revoked on these systems:\n\n";

		for (int i = 0; i < DisconnectedScreen::loginslost.Size(); ++i) {

			if (DisconnectedScreen::loginslost.ValidIndex(i)) {

				char *ip = DisconnectedScreen::loginslost.GetData(i);
				VLocation *vl = game->GetWorld()->GetVLocation(ip);

				if (vl) {
					Computer *comp = vl->GetComputer();

					sLogins += "    - ";
					sLogins += comp->name;
					sLogins +="\n";
				}

			}

		}

		loginsLost->setText(sLogins.c_str(), width - 160.0f, 0);

		DisconnectedScreen::ClearLoginsLost();

	}

	//OK button
	std::shared_ptr<HD_UI_Button> btnOK = std::make_shared<HD_UI_Button>();
	btnOK->CreateRectangleButton("btnOK", "OK", "OK.", width / 2.0f - 80.0f, 660.0f, 160.0f, 30.0f, palette->btnColors_blueSat, false);
	addChild(btnOK);
	btnOK->setCallback(std::bind(&HD_HUD_DisconnectScreen::buttonClick, this, discScreen->nextpage));
}

void HD_HUD_DisconnectScreen::buttonClick(int nextScreen)
{
	if (nextScreen != -1)
		root->GetHUDMainWindow()->GoToScreen(nextScreen);
}