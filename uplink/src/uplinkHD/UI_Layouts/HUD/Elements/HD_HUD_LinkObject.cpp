// Implementation of the Link Object
//==================================

#include "HD_HUD_LinkObject.h"

#include "../../../HD_ColorPalettes.h"
#include "../../HD_Root.h"
#include "../Interfaces/HD_HUD_MainWindow.h"

#include "../../../UI_Objects/HD_UI_Button.h"
#include "../../../UI_Objects/HD_UI_GraphicsObject.h"
#include "../../../UI_Objects/HD_UI_TextObject.h"

#include "../../../../game/game.h"
#include "../../../../world/world.h"
#include "../../../../world/player.h"
#include "../../../../world/vlocation.h"
#include "../../../../world/computer/computer.h"

//Callbacks
//=========
void HD_HUD_LinkObject::connectClick()
{
	root->GetHUDMainWindow()->ConnectTo(ip);
}

void HD_HUD_LinkObject::addToFavoritesClick()
{
	//adds the link to the player's favorites
	game->GetWorld()->GetPlayer()->GiveLink(ip);
	getChildByName("btnAddToLinks")->visible = false;
}

void HD_HUD_LinkObject::addToMapClick()
{
	game->GetWorld()->GetVLocation(ip)->displayed = !game->GetWorld()->GetVLocation(ip)->displayed;
}

void HD_HUD_LinkObject::colorOnMapClick()
{
	game->GetWorld()->GetVLocation(ip)->colored = !game->GetWorld()->GetVLocation(ip)->colored;
}

void HD_HUD_LinkObject::removeClick()
{
	game->GetWorld()->GetPlayer()->RemoveLink(ip);
}

void HD_HUD_LinkObject::createGenericGfx()
{
	//separator
	std::shared_ptr<HD_UI_GraphicsObject> lineGfx = std::make_shared<HD_UI_GraphicsObject>();
	lineGfx->CreateLineObject("lineGfx", 0.0f, 43.5f, width, 0.0f, 1.0f, palette->blues.cBlue3);
	addChild(lineGfx);

	//bgButton / connect
	std::shared_ptr<HD_UI_Button> btnConnect = std::make_shared<HD_UI_Button>();
	btnConnect->CreateRectangleButton("btnConnect", "", "Connect to this Link.", 0.0f, 0.0f, width, height, palette->btnColors_blueSat, true);
	addChild(btnConnect);
	btnConnect->setCallback(std::bind(&HD_HUD_LinkObject::connectClick, this));

	//texts
	std::shared_ptr<HD_UI_TextObject> ipTxt = std::make_shared<HD_UI_TextObject>();
	ipTxt->CreateSinglelineText("ipTxt", 0.0f, 6.0f, ip, palette->blues.cBlue3, HDResources->font30);
	addChild(ipTxt);

	std::shared_ptr<HD_UI_TextObject> nameTxt = std::make_shared<HD_UI_TextObject>();
	nameTxt->CreateSinglelineText("nameTxt", 240.0f, 6.0f, game->GetWorld()->GetVLocation(ip)->computer, palette->bluesSat.cBlue2, HDResources->font30);
	addChild(nameTxt);
}

//Public
//=========
HD_HUD_LinkObject::HD_HUD_LinkObject(const char* objectName, char* ip)
{
	setObjectProperties(objectName, 0.0f, 0.0f, 932.0f, 44.0f);
	HD_HUD_LinkObject::ip = ip;
}

void HD_HUD_LinkObject::CreatePlayerLink()
{
	createGenericGfx();

	//buttons
	std::shared_ptr<HD_UI_Button> btnAddToMap = std::make_shared<HD_UI_Button>();
	btnAddToMap->CreateImageSelectionButton("btnAddToMap", "btnAddToMap_s", "hud_atlas.png", "Add this Link to the Map.", width - 145.0f, 0.0f,
		game->GetWorld()->GetVLocation(ip)->displayed);
	addChild(btnAddToMap);
	btnAddToMap->setCallback(std::bind(&HD_HUD_LinkObject::addToMapClick, this));

	std::shared_ptr<HD_UI_Button> btnColorOnMap = std::make_shared<HD_UI_Button>();
	btnColorOnMap->CreateImageSelectionButton("btnColorOnMap", "btnColorOnMap_s", "hud_atlas.png", "Show this Link colored, on the Map.", width - 95.0f, 0.0f,
		game->GetWorld()->GetVLocation(ip)->colored);
	addChild(btnColorOnMap);
	btnColorOnMap->setCallback(std::bind(&HD_HUD_LinkObject::colorOnMapClick, this));

	std::shared_ptr<HD_UI_Button> btnDeleteLink = std::make_shared<HD_UI_Button>();
	btnDeleteLink->CreateImageButton("btnDeleteLink", "btnDeleteLink_s", "hud_atlas.png", "Delete this Link.", width - 45.0f, 0.0f);
	addChild(btnDeleteLink);
	btnColorOnMap->setCallback(std::bind(&HD_HUD_LinkObject::removeClick, this));
}

void HD_HUD_LinkObject::CreateGenericLink()
{
	createGenericGfx();

	std::shared_ptr<HD_UI_Button> btnAddToLinks = std::make_shared<HD_UI_Button>();
	btnAddToLinks->CreateImageButton("btnAddToLinks", "btnAddToLinks_s", "hud_atlas.png", "Add this Link to your Favorites.", width - 45.0f, 0.0f);
	addChild(btnAddToLinks);
	btnAddToLinks->setCallback(std::bind(&HD_HUD_LinkObject::addToFavoritesClick, this));
	btnAddToLinks->visible = !game->GetWorld()->GetPlayer()->HasLink(ip);
}

void HD_HUD_LinkObject::Update()
{
	HD_UI_Container::Update();

	if (getContainerByName("btnConnect")->hasMouseFocus() && !textColorsSet)
	{
		getTextObjectByName("ipTxt")->setTextColor(palette->warmGreys.cGrey3);
		getTextObjectByName("nameTxt")->setTextColor(palette->warmGreys.cGrey3);
		textColorsSet = true;
	}
	else if (!getContainerByName("btnConnect")->hasMouseFocus() && textColorsSet)
	{
		getTextObjectByName("ipTxt")->setTextColor(palette->blues.cBlue3);
		getTextObjectByName("nameTxt")->setTextColor(palette->bluesSat.cBlue2);
		textColorsSet = false;
	}
}