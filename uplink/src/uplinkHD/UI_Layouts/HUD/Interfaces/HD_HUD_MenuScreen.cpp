#include "HD_HUD_MenuScreen.h"

#include "../../HD_Root.h"
#include "../Interfaces/HD_HUD_MainWindow.h"
#include "../../../HD_ColorPalettes.h"

#include "../../../UI_Objects/HD_UI_Button.h"
#include "../../../UI_Objects/HD_UI_TextObject.h"

#include "../../../../game/game.h"
#include "../../../../interface/interface.h"
#include "../../../../interface/remoteinterface/remoteinterface.h"
#include "../../../../world/computer/computerscreen/menuscreen.h"

void HD_HUD_MenuScreen::MenuItemClick(int nextPage)
{
	if (nextPage != -1)
		root->GetHUDMainWindow()->GoToScreen(nextPage);
}

HD_HUD_MenuScreen::HD_HUD_MenuScreen()
{
	setObjectProperties("menuScreen", 0.0f, 0.0f, root->width / 2.0f, root->height);
}

void HD_HUD_MenuScreen::Create(ComputerScreen* cScreen)
{
	MenuScreen* menuScreen = (MenuScreen*)cScreen;

	//maintitle
	std::shared_ptr<HD_UI_TextObject> mainTitle = std::make_shared<HD_UI_TextObject>();
	mainTitle->CreateSinglelineText("mainTitle", 12.0f, 132.0f, menuScreen->maintitle, palette->bluesSat.cBlue2, HDResources->font48);
	addChild(mainTitle);

	//subtitle
	std::shared_ptr<HD_UI_TextObject> subTitle = std::make_shared<HD_UI_TextObject>();
	subTitle->CreateSinglelineText("subTitle", 12.0f, 186.0f, menuScreen->subtitle, palette->blues.cBlue3, HDResources->font30);
	addChild(subTitle);

	//buttons
	for (int ii = 0; ii < menuScreen->NumOptions(); ii++)
	{
		std::string btnName = "btnOption" + std::to_string(ii);

		std::shared_ptr<HD_UI_Button> btnOption = std::make_shared<HD_UI_Button>();
		btnOption->CreateRectangleWIconButton(btnName.c_str(), menuScreen->GetCaption(ii), menuScreen->GetTooltip(ii), 80.0f, 270.0f + 50.0f * ii,
			880.0f, 45.0f, palette->btnColors_blueSat, true, "icon_menuOption", "hud_atlas.png", HDResources->font30);
		addChild(btnOption);
		btnOption->setCallback(std::bind(&HD_HUD_MenuScreen::MenuItemClick, this, menuScreen->GetNextPage(ii)));

		//Does the player have clearence?
		bool hasClearence = game->GetInterface()->GetRemoteInterface()->security_level <= menuScreen->GetSecurity(ii) ? true : false;
		btnOption->SetActive(hasClearence);
	}
}