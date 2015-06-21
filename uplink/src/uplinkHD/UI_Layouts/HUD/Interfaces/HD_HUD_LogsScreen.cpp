#include "HD_HUD_LogsScreen.h"
#include "../Elements/HD_HUD_LogObject.h"

#include "../../../UI_Objects/HD_UI_Button.h"
#include "../../../UI_Objects/HD_UI_TextObject.h"
#include "../../../UI_Objects/HD_UI_ScrollableContainer.h"

#include "../../HD_Root.h"
#include "../../../HD_ColorPalettes.h"
#include "../../../HD_Resources.h"

#include "world/computer/logbank.h"
#include "world/computer/computerscreen/logscreen.h"

void HD_HUD_LogsScreen::backClick(int nextPage)
{
	if (nextPage != -1)
		root->GetHUDMainWindow()->GoToScreen(nextPage);
}

HD_HUD_LogsScreen::HD_HUD_LogsScreen()
{
	setObjectProperties("logsScreen", 0.0f, 0.0f, root->width / 2.0f, root->height);
}

void HD_HUD_LogsScreen::Create(ComputerScreen* cScreen)
{
	LogScreen* logScreen = (LogScreen*)cScreen;
	LogBank* logbank = logScreen->GetTargetLogBank();

	//maintitle
	std::shared_ptr<HD_UI_TextObject> mainTitle = std::make_shared<HD_UI_TextObject>();
	mainTitle->CreateSinglelineText("mainTitle", 12.0f, 132.0f, logScreen->maintitle, palette->bluesSat.cBlue2, HDResources->font48);
	addChild(mainTitle);

	//subtitle
	std::shared_ptr<HD_UI_TextObject> subTitle = std::make_shared<HD_UI_TextObject>();
	subTitle->CreateSinglelineText("subTitle", 12.0f, 186.0f, logScreen->subtitle, palette->blues.cBlue3, HDResources->font30);
	addChild(subTitle);

	//back button
	std::shared_ptr<HD_UI_Button> btnBack = std::make_shared<HD_UI_Button>();
	btnBack->CreateImageButton("btnBack", "btnBack_s", "hud_atlas.png", "Go back.", 852.0f, 25.0f);
	btnBack->setCallback(std::bind(&HD_HUD_LogsScreen::backClick, this, logScreen->nextpage));
	addChild(btnBack);

	//details
	std::shared_ptr<HD_UI_GraphicsObject> listSeparator = std::make_shared<HD_UI_GraphicsObject>();
	listSeparator->CreateLineObject("listSeparator", 12.0f, 313.0f, width - 24.0f, 0.0f, 2.0f, palette->bluesSat.cBlue2);
	addChild(listSeparator);

	std::shared_ptr<HD_UI_TextObject> txtDate = std::make_shared<HD_UI_TextObject>();
	txtDate->CreateSinglelineText("txtDate", 12.0f, 280.0f, "Date", palette->bluesSat.cBlue2);
	addChild(txtDate);

	std::shared_ptr<HD_UI_TextObject> txtDescription = std::make_shared<HD_UI_TextObject>();
	txtDescription->CreateSinglelineText("txtDescription", 252.0f, 280.0f, "Description", palette->bluesSat.cBlue2);
	addChild(txtDescription);

	//the list!
	std::shared_ptr<HD_UI_ScrollableContainer> logsList = std::make_shared<HD_UI_ScrollableContainer>();
	logsList->CreateListContainer("logsList", 12.0f, 315.0f, width - 12.0f, 700.0f, vertical);
	addChild(logsList);

	for (int ii = 0; ii < logbank->logs.Size(); ii++)
		if (logbank->logs.ValidIndex(ii))
		{
			std::string logName = "logItem" + std::to_string(ii);
			AccessLog* log = logbank->logs.GetData(ii);

			std::shared_ptr<HD_HUD_LogObject> logItem = std::make_shared<HD_HUD_LogObject>();
			logItem->Create(logName.c_str(), log->date.GetShortString(), log->GetDescription());
			logsList->AddElement(logItem, true);
		}
}