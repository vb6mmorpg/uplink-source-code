#include "HD_HUD_MainWindow.h"

#include "../../../HD_Screen.h"
#include "../../HD_Root.h"
#include "../../../HD_ColorPalettes.h"

#include "../../../UI_Objects/HD_UI_GraphicsObject.h"
#include "../../../UI_Objects/HD_UI_Button.h"
#include "../../../UI_Objects/HD_UI_TextObject.h"

//screens
#include "HD_HUD_LinksScreen.h"
#include "HD_HUD_MessageScreen.h"
#include "HD_HUD_MenuScreen.h"
#include "HD_HUD_DialogScreen.h"
#include "HD_HUD_DisconnectScreen.h"
#include "HD_HUD_PasswordScreen.h"
#include "HD_HUD_UseridScreen.h"
#include "HD_HUD_LogsScreen.h"

//Uplink
#include "../../../../game/game.h"
#include "../../../../game/data/data.h"
#include "../../../../world/world.h"
#include "../../../../world/player.h"
#include "../../../../world/vlocation.h"
#include "../../../../world/computer/computer.h"
#include "../../../../world/computer/computerscreen/computerscreen.h"
#include "../../../../world/computer/computerscreen/genericscreen.h"
#include "../../../../world/computer/computerscreen/linksscreen.h"
#include "../../../../interface/interface.h"
#include "../../../../interface/remoteinterface/remoteinterface.h"
#include "../../../../app/serialise.h"

HD_HUD_MainWindow::HD_HUD_MainWindow()
{
	setObjectProperties("mainWindow", root->width / 4.0f, 0.0f, root->width / 2.0f, root->height);
}

void HD_HUD_MainWindow::Create()
{
	//bg stroke
	//hopefully nothing gets drawn behind it
	std::shared_ptr<HD_UI_GraphicsObject> bgGfx = std::make_shared<HD_UI_GraphicsObject>();
	bgGfx->CreateRectangleObject("bgGfx", 0.0f, -1.0f, width, height, 1.0f, palette->bluesSat.cBlue2);
	addChild(bgGfx);

	//disconnect button
	std::shared_ptr<HD_UI_Button> btnDisconnect = std::make_shared<HD_UI_Button>();
	btnDisconnect->CreateImageButton("btnDisconnect", "btnDisconnect_s", "hud_atlas.png", "Disconnect from this computer.", 902.0f, 25.0f);
	addChild(btnDisconnect);
	btnDisconnect->setCallback(std::bind(&HD_HUD_MainWindow::Disconnect, this));

	//loading bar
	std::shared_ptr<HD_UI_GraphicsObject> loadingBar = std::make_shared<HD_UI_GraphicsObject>();
	loadingBar->CreateLineObject("loadingBar", 0.0f, 1.0f, width, 0.0f, 2.0f, palette->bluesSat.cBlue2);
	addChild(loadingBar);

	//loading container (circle + caption
	std::shared_ptr<HD_UI_Container> loadingContainer = std::make_shared<HD_UI_Container>("loadingContainer", width / 2.0f, height / 2.0f, 0.0f, 0.0f);
	addChild(loadingContainer);

	std::shared_ptr<HD_UI_GraphicsObject> loadingCircle = std::make_shared<HD_UI_GraphicsObject>();
	loadingCircle->CreateImageObject("loadingCircle", "loadingCircle", "misc_atlas.png", 0.0f, 0.0f);
	loadingCircle->x -= loadingCircle->width / 2.0f;
	loadingCircle->y -= loadingCircle->height / 2.0f;
	loadingContainer->addChild(loadingCircle);

	CDBTweener::CTween *rotateTween = new CDBTweener::CTween(&CDBTweener::TWEQ_LINEAR, CDBTweener::TWEA_INOUT, 1.0f, &loadingCircle->rotation, 360.0f);
	loadingCircle->addAnimation(rotateTween, true, 1);

	std::shared_ptr<HD_UI_GraphicsObject> logoCube = std::make_shared<HD_UI_GraphicsObject>();
	logoCube->CreateImageObject("logoCube", "logoCubeS", "misc_atlas.png", 0.0f, 0.0f);
	logoCube->x -= logoCube->width / 2.0f;
	logoCube->y -= logoCube->height / 2.0f;
	loadingContainer->addChild(logoCube);

	std::shared_ptr<HD_UI_TextObject> captionTxt = std::make_shared<HD_UI_TextObject>();
	captionTxt->CreateSinglelineText("captionTxt", 0.0f, loadingCircle->height / 2.0f + 10.0f, "Connecting to:", palette->bluesSat.cBlue2, HDResources->font24, ALLEGRO_ALIGN_CENTER);
	loadingContainer->addChild(captionTxt);

	std::shared_ptr<HD_UI_TextObject> ipTxt = std::make_shared<HD_UI_TextObject>();
	ipTxt->CreateSinglelineText("ipTxt", 0.0f, captionTxt->y + captionTxt->height + 10.0f, "IP", palette->bluesSat.cBlue2, HDResources->font30, ALLEGRO_ALIGN_CENTER);
	loadingContainer->addChild(ipTxt);

	std::shared_ptr<HD_UI_Object> clipMask = std::make_shared<HD_UI_Object>("clipMask", 0.0f, 0.0f, width + 1.0f, height);
	AddClippingMask(clipMask, true);

	ConnectTo(IP_LOCALHOST, 3);
}

void HD_HUD_MainWindow::Update()
{
	HD_UI_Container::Update();

	//update the disconnect button
	getChildByName("btnDisconnect")->visible = game->GetWorld()->GetPlayer()->IsConnected() ? true : false;

	//update loadingContainer
	if (getChildByName("loadingContainer")->x < -100.0f || getChildByName("loadingContainer")->x > width + 100.0f)
		getChildByName("loadingContainer")->visible = false;
	else
		getChildByName("loadingContainer")->visible = true;
}

//Connection
//===========
void HD_HUD_MainWindow::ConnectTo(char* ip, int nextScreen)
{
	//begins the "connecting" animation and sets up the screens to be used by the new computer
	//the connection will get done in the animation listener to fake "lag"

	//animation
	getChildByName("loadingContainer")->x = width + 50.0f;
	getContainerByName("loadingContainer")->getTextObjectByName("ipTxt")->setText(ip);
	CDBTweener::CTween *loadingContTween = new CDBTweener::CTween(&CDBTweener::TWEQ_CIRCULAR, CDBTweener::TWEA_OUT, 0.5f,
		&getChildByName("loadingContainer")->x, width / 2.0f);
	getChildByName("loadingContainer")->addAnimation(loadingContTween, true);

	getChildByName("loadingBar")->width = 1;
	getChildByName("loadingBar")->visible = true;
	int connectionSize = game->GetWorld()->GetPlayer()->GetConnection()->GetSize();
	float connectionTime = connectionSize < 2 ? 1.5f : 0.5f * connectionSize;
	CDBTweener::CTween *loadingBarTween = new CDBTweener::CTween(&CDBTweener::TWEQ_CIRCULAR, CDBTweener::TWEA_OUT, connectionTime,
		&getChildByName("loadingBar")->width, width);
	loadingBarTween->setUserData(ip);
	loadingBarTween->setUserIntData(nextScreen);
	loadingBarTween->addListener(&connectionComplete);
	getChildByName("loadingBar")->addAnimation(loadingBarTween, true);

	//get the current screen out of the way
	if (currentScreenID != -1)
	{
		CDBTweener::CTween *prevTween = new CDBTweener::CTween(&CDBTweener::TWEQ_CIRCULAR, CDBTweener::TWEA_OUT, 0.5f,
			&currentScreens[currentScreenID]->x, -width - 2.0f);
		currentScreens[currentScreenID]->addAnimation(prevTween, true);
		previousScreenID = currentScreenID;
		currentScreenID = -1;
	}

	if (!currentScreens.empty())
	{
		previousScreens = currentScreens;
		currentScreens.erase(currentScreens.begin(), currentScreens.end());
	}

	int noOfScreens = game->GetWorld()->GetVLocation(ip)->GetComputer()->screens.Size();
	
	for (int ii = 0; ii < noOfScreens; ii++)
		if (game->GetWorld()->GetVLocation(ip)->GetComputer()->screens.ValidIndex(ii))
		createScreen(game->GetWorld()->GetVLocation(ip)->GetComputer()->GetComputerScreen(ii), ii);

	addChild(getChildByName("btnDisconnect"));
}

void HD_HUD_MainWindow::Disconnect()
{
	//Goes back to Gateway
	ConnectTo(IP_LOCALHOST, 1);
	getChildByName("btnDisconnect")->visible = false;
}

void HD_HUD_MainWindow::GoToScreen(int nextScreen)
{
	//Create the next screen
	//createScreen(game->GetWorld()->GetPlayer()->GetRemoteHost()->GetComputer()->screens.GetData(nextScreen), nextScreen);

	//animate the current screen
	if (currentScreenID != -1)
	{
		CDBTweener::CTween *prevTween = new CDBTweener::CTween(&CDBTweener::TWEQ_CIRCULAR, CDBTweener::TWEA_OUT, 0.5f,
			&currentScreens[currentScreenID]->x, - width - 2.0f);
		currentScreens[currentScreenID]->addAnimation(prevTween, true);
	}

	//animate the next screen
	currentScreenID = nextScreen;
	currentScreens[currentScreenID]->x = width + 2.0f; //reset position
	CDBTweener::CTween *nextTween = new CDBTweener::CTween(&CDBTweener::TWEQ_CIRCULAR, CDBTweener::TWEA_OUT, 0.5f,
		&currentScreens[currentScreenID]->x, 0.0f);
	currentScreens[currentScreenID]->addAnimation(nextTween, true);
}

//Screen Creation
//===============
void HD_HUD_MainWindow::createScreen(ComputerScreen *cScreen, int screenIndex)
{
	//this is a big and ugly one!
	//recreates one of Uplinks' possible screens, mirroring the remote interfaces
	//idType is used to determine the screens type
	//screenIndex is used to add the screen to the correct position, as Uplink doesn't just add them sequentially

	if (!cScreen) return;

	int idType = cScreen->GetOBJECTID();
	std::vector<std::shared_ptr<HD_UI_Container>>::iterator it;
	currentScreens.resize(screenIndex + 1, nullptr);
	it = currentScreens.begin();

	switch (idType)
	{
	case OID_MESSAGESCREEN:
	{
		std::shared_ptr<HD_HUD_MessageScreen> newScreen = std::make_shared<HD_HUD_MessageScreen>();
		newScreen->Create(cScreen);
		newScreen->x = width + 2.0f;
		addChild(newScreen);
		currentScreens.insert(it + screenIndex, newScreen);
	}
		break;

	case OID_PASSWORDSCREEN:
	{
		std::shared_ptr<HD_HUD_PasswordScreen> newScreen = std::make_shared<HD_HUD_PasswordScreen>();
		newScreen->Create(cScreen);
		newScreen->x = width + 2.0f;
		addChild(newScreen);
		currentScreens.insert(it + screenIndex, newScreen);
	}
		break;

	case OID_MENUSCREEN:
	{
		std::shared_ptr<HD_HUD_MenuScreen> newScreen = std::make_shared<HD_HUD_MenuScreen>();
		newScreen->Create(cScreen);
		newScreen->x = width + 2.0f;
		addChild(newScreen);
		currentScreens.insert(it + screenIndex, newScreen);
	}
		break;

	case OID_DIALOGSCREEN:
	{
		std::shared_ptr<HD_HUD_DialogScreen> newScreen = std::make_shared<HD_HUD_DialogScreen>();
		newScreen->Create(cScreen);
		newScreen->x = width + 2.0f;
		addChild(newScreen);
		currentScreens.insert(it + screenIndex, newScreen);
	}
		break;

	case OID_USERIDSCREEN:
	{
		std::shared_ptr<HD_HUD_UseridScreen> newScreen = std::make_shared<HD_HUD_UseridScreen>();
		newScreen->Create(cScreen);
		newScreen->x = width + 2.0f;
		addChild(newScreen);
		currentScreens.insert(it + screenIndex, newScreen);
	}
		break;

	case OID_LOGSCREEN:
	{
		std::shared_ptr<HD_HUD_LogsScreen> newScreen = std::make_shared<HD_HUD_LogsScreen>();
		newScreen->Create(cScreen);
		newScreen->x = width + 2.0f;
		addChild(newScreen);
		currentScreens.insert(it + screenIndex, newScreen);
	}
		break;

	case OID_BBSSCREEN:
	{

	}
		break;

	case OID_LINKSSCREEN:
	{
		std::shared_ptr<HD_HUD_LinksScreen> newScreen = std::make_shared<HD_HUD_LinksScreen>();
		newScreen->Create(cScreen);
		newScreen->x = width + 2.0f;
		addChild(newScreen);
		currentScreens.insert(it + screenIndex, newScreen);
	}
		break;

	case OID_SHARESLISTSCREEN:
	{

	}
		break;

	case OID_CYPHERSCREEN:
	{

	}
		break;

	case OID_HIGHSECURITYSCREEN:
	{

	}
		break;

	case OID_DISCONNECTEDSCREEN:
	{
		std::shared_ptr<HD_HUD_DisconnectScreen> newScreen = std::make_shared<HD_HUD_DisconnectScreen>();
		newScreen->Create(cScreen);
		newScreen->x = width + 2.0f;
		addChild(newScreen);
		currentScreens.insert(it + screenIndex, newScreen);
	}
		break;

	case OID_GENERICSCREEN:

		switch (((GenericScreen *)cScreen)->SCREEN_TYPE)
		{
		case SCREEN_FILESERVERSCREEN:
			break;
		case SCREEN_SWSALESSCREEN:
			break;
		case SCREEN_HWSALESSCREEN:
			break;
		case SCREEN_RECORDSCREEN:
			break;
		case SCREEN_ACCOUNTSCREEN:
			break;
		case SCREEN_CONTACTSCREEN:
			break;
		case SCREEN_NEWSSCREEN:
			break;
		case SCREEN_CRIMINALSCREEN:
			break;
		case SCREEN_SECURITYSCREEN:
			break;
		case SCREEN_ACADEMICSCREEN:
			break;
		case SCREEN_RANKINGSCREEN:
			break;
		case SCREEN_CONSOLESCREEN:
			break;
		case SCREEN_SOCSECSCREEN:
			break;
		case SCREEN_LOANSSCREEN:
			break;
		case SCREEN_SHARESVIEWSCREEN:
			break;
		case SCREEN_FAITHSCREEN:
			break;
		case SCREEN_VOICEANALYSIS:
			break;
		case SCREEN_COMPANYINFO:
			break;
		case SCREEN_VOICEPHONE:
			break;
		case SCREEN_NEARESTGATEWAY:
			break;
		case SCREEN_CHANGEGATEWAY:
			break;
		case SCREEN_CODECARD:
			break;
		case SCREEN_PROTOVISION:
			break;
		case SCREEN_NUCLEARWAR:
			break;
		case SCREEN_RADIOTRANSMITTER:
			break;
		}
	
		break;

	}

	
}

void HD_HUD_MainWindow::removeScreens()
{
	currentScreenID = -1;
	std::vector<std::shared_ptr<HD_UI_Container>>::iterator it = currentScreens.begin();

	for (unsigned int ii = currentScreens.size(); ii > 0; ii--)
		if (currentScreens[ii - 1])
		{
			removeChildFromIndex(currentScreens[ii - 1]->index);
			currentScreens[ii - 1].reset();
			currentScreens.erase(it + ii - 1);
		}

	HDScreen->mouse->SetTarget(nullptr);
}

void HD_HUD_MainWindow::RemovePreviousScreens()
{
	if (previousScreens.empty())
		return; //nothing to do here

	std::vector<std::shared_ptr<HD_UI_Container>>::iterator it = previousScreens.begin();

	for (unsigned int ii = previousScreens.size(); ii > 0; ii--)
		if (previousScreens[ii - 1])
		{
			removeChildFromIndex(previousScreens[ii - 1]->index);
			previousScreens[ii - 1].reset();
			previousScreens.erase(it + ii - 1);
		}

	HDScreen->mouse->SetTarget(nullptr);
}

//Connection animation listener
//=============================
void HD_HUD_MainWindow::ConnectionComplete::onTweenFinished(CDBTweener::CTween *pTween)
{
	//Connection animation finished

	char* ip = (char*)pTween->getUserData();
	int nextScreen = pTween->getUserIntData();

	if (game->GetWorld()->GetPlayer()->IsConnected())
	{
		game->GetWorld()->GetPlayer()->GetConnection()->Disconnect();
		game->GetWorld()->GetPlayer()->GetConnection()->Reset();
		game->GetWorld()->GetPlayer()->GetConnection()->AddVLocation(ip);
		game->GetWorld()->GetPlayer()->GetConnection()->Connect();
	}
	else
	{
		game->GetWorld()->GetPlayer()->GetConnection()->AddVLocation(ip);
		game->GetWorld()->GetPlayer()->GetConnection()->Connect();
	}

	game->GetInterface()->GetRemoteInterface()->RunNewLocation();
	root->GetHUDMainWindow()->GoToScreen(nextScreen);

	//hide the loading container
	CDBTweener::CTween *contTween = new CDBTweener::CTween(&CDBTweener::TWEQ_CIRCULAR, CDBTweener::TWEA_OUT, 0.25f,
		&root->GetHUDMainWindow()->getChildByName("loadingContainer")->x, -110.0f);
	root->GetHUDMainWindow()->getChildByName("loadingContainer")->addAnimation(contTween, true);

	//hide the loading bar
	root->GetHUDMainWindow()->getChildByName("loadingBar")->visible = false;

	//delete the old screens
	root->GetHUDMainWindow()->RemovePreviousScreens();
}