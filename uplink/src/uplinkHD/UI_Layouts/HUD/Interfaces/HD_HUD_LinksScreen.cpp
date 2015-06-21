//Implementation of the Links Screen
//==================================

#include "HD_HUD_LinksScreen.h"

#include "../../HD_Root.h"
#include "../../../HD_ColorPalettes.h"

#include "../../../UI_Objects/HD_UI_ButtonInput.h"
#include "../../../UI_Objects/HD_UI_TextObject.h"
#include "../../../UI_Objects/HD_UI_ScrollableContainer.h"
#include "../Elements/HD_HUD_LinkObject.h"

#include "../../../../game/game.h"
#include "../../../../world/world.h"
#include "../../../../world/player.h"
#include "../../../../world/vlocation.h"
#include "../../../../world/computer/computer.h"
#include "../../../../world/computer/computerscreen/computerscreen.h"
#include "../../../../interface/remoteinterface/remoteinterface.h"
#include "../../../../game/data/data.h"

//Protected methods
void HD_HUD_LinksScreen::filterClick()
{
	
}

//Constructor
HD_HUD_LinksScreen::HD_HUD_LinksScreen()
{
	setObjectProperties("linksScreen", 0.0f, 0.0f, root->width / 2.0f, root->height);
}

void HD_HUD_LinksScreen::Create(ComputerScreen *cScreen)
{
	LinksScreen* linksScreen = (LinksScreen*)cScreen;

	if (linksScreen->SCREENTYPE == LINKSSCREENTYPE_PLAYERLINKS)
		CreatePlayerLinks(linksScreen);
	else if (linksScreen->SCREENTYPE == LINKSSCREENTYPE_ALLLINKS)
		CreateInterNICLinks(linksScreen);
	else
		CreateComputerLinks(linksScreen);
}

void HD_HUD_LinksScreen::CreatePlayerLinks(LinksScreen *cScreen)
{
	//player gateway

	//subtitle
	std::shared_ptr<HD_UI_TextObject> subtitleTxt = std::make_shared<HD_UI_TextObject>();
	subtitleTxt->CreateSinglelineText("subtitleTxt", 12.0f, 94.0f, cScreen->subtitle,
		palette->blues.cBlue3);
	addChild(subtitleTxt);

	//filter button
	std::shared_ptr<HD_UI_ButtonInput> btnFilter = std::make_shared<HD_UI_ButtonInput>();
	btnFilter->CreateSinglelineInput("btnFilter", "Filter", "Filter the list.", 12.0f, 125.0f, 308.0f, 45.0f, true, palette->btnInputColors_blueSat, HDResources->font30);
	addChild(btnFilter);
	btnFilter->setCallback(std::bind(&HD_HUD_LinksScreen::filterClick, this));

	std::shared_ptr<HD_UI_GraphicsObject> filterIcon = std::make_shared<HD_UI_GraphicsObject>();
	filterIcon->CreateImageObject("filterIcon", "findIcon", "hud_atlas.png", btnFilter->width, btnFilter->height);
	filterIcon->x -= filterIcon->width + 2.0f;
	filterIcon->y -= filterIcon->height + 3.0f;
	btnFilter->addChild(filterIcon);

	//the list!
	std::shared_ptr<HD_UI_ScrollableContainer> linksList = std::make_shared<HD_UI_ScrollableContainer>();
	linksList->CreateListContainer("linksList", 12.0f, 180.0f, width - 12.0f, 838.0f, vertical);
	addChild(linksList);

	int totalLinks = game->GetWorld()->GetPlayer()->links.Size();
	for (int ii = 0; ii < totalLinks; ++ii)
		if (game->GetWorld()->GetPlayer()->links.ValidIndex(ii))
		{
			std::string itemName = "link" + std::to_string(ii);
			std::shared_ptr<HD_HUD_LinkObject> link = std::make_shared<HD_HUD_LinkObject>(itemName.c_str(), game->GetWorld()->GetPlayer()->links.GetData(ii));
			link->CreatePlayerLink();

			linksList->AddElement(link, true);
		}
}

void HD_HUD_LinksScreen::CreateInterNICLinks(LinksScreen *cScreen)
{
	//InterNIC

	//maintitle
	std::shared_ptr<HD_UI_TextObject> mainTitle = std::make_shared<HD_UI_TextObject>();
	mainTitle->CreateSinglelineText("mainTitle", 12.0f, 132.0f, cScreen->maintitle, palette->bluesSat.cBlue2, HDResources->font48);
	addChild(mainTitle);

	//subtitle
	std::shared_ptr<HD_UI_TextObject> subTitle = std::make_shared<HD_UI_TextObject>();
	subTitle->CreateSinglelineText("subTitle", 12.0f, 186.0f, cScreen->subtitle, palette->blues.cBlue3, HDResources->font30);
	addChild(subTitle);

	//filter button
	std::shared_ptr<HD_UI_ButtonInput> btnFilter = std::make_shared<HD_UI_ButtonInput>();
	btnFilter->CreateSinglelineInput("btnFilter", "Filter", "Filter the list.", 12.0f, 226.0f, 308.0f, 45.0f, true, palette->btnColors_blueSat, HDResources->font30);
	addChild(btnFilter);
	btnFilter->setCallback(std::bind(&HD_HUD_LinksScreen::filterClick, this));

	std::shared_ptr<HD_UI_GraphicsObject> filterIcon = std::make_shared<HD_UI_GraphicsObject>();
	filterIcon->CreateImageObject("filterIcon", "findIcon", "hud_atlas.png", btnFilter->width, btnFilter->height);
	filterIcon->x -= filterIcon->width + 2.0f;
	filterIcon->y -= filterIcon->height + 3.0f;
	btnFilter->addChild(filterIcon);

	//the list!
	std::shared_ptr<HD_UI_ScrollableContainer> linksList = std::make_shared<HD_UI_ScrollableContainer>();
	linksList->CreateListContainer("linksList", 12.0f, 281.0f, width - 12.0f, 783.0f, vertical);
	addChild(linksList);

	DArray <VLocation *> *locs = game->GetWorld()->locations.ConvertToDArray();
	int totalLinks = locs->Size();
	for (int ii = 0; ii < totalLinks; ++ii)
		if (locs->ValidIndex(ii))
			if (locs->GetData(ii)->listed)
			{
				std::string itemName = "link" + std::to_string(ii);
				std::shared_ptr<HD_HUD_LinkObject> link = std::make_shared<HD_HUD_LinkObject>(itemName.c_str(), locs->GetData(ii)->ip);
				link->CreateGenericLink();

				linksList->AddElement(link, true);
			}
	delete locs;
}

void HD_HUD_LinksScreen::CreateComputerLinks(LinksScreen *cScreen)
{
	//Generic Computer
}

void HD_HUD_LinksScreen::Update()
{
	HD_UI_Container::Update();
}