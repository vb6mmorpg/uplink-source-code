// Implementation of EmailNotification
//====================================

#include "HD_HUD_EmailNotif.h"

#include "../../../HD_ColorPalettes.h"
#include "../../../HD_Resources.h"
#include "../../../HD_Screen.h"

#include "../../../UI_Objects/HD_UI_GraphicsObject.h"
#include "../../../UI_Objects/HD_UI_TextObject.h"

#include "../../../../game/game.h"
#include "../../../../world/world.h"
#include "../../../../world/player.h"
#include "../../../../world/company/mission.h"
#include "../../../../world/message.h"

HD_HUD_EmailNotif::HD_HUD_EmailNotif()
{
	setObjectProperties("emailNotif", 0.0f, 0.0f, 0.0f, 0.0f);
}

void HD_HUD_EmailNotif::Create()
{
	//icon notification
	std::shared_ptr<HD_UI_Container> iconNotifContainer = std::make_shared<HD_UI_Container>("iconNotifContainer", -2.0f, 4.0f, 0.0f, 0.0f);
	addChild(iconNotifContainer);

	std::shared_ptr<HD_UI_GraphicsObject> iconBg = std::make_shared<HD_UI_GraphicsObject>();
	iconBg->CreateImageObject("iconBg", "emailNotif", "hud_atlas.png", 0.0f, 0.0f);
	iconNotifContainer->addChild(iconBg);

	std::shared_ptr<HD_UI_TextObject> iconTxt = std::make_shared<HD_UI_TextObject>();
	iconTxt->CreateSinglelineText("iconTxt", iconBg->width / 2.0f, iconBg->height / 2.0f, "0",
		palette->warmGreys.cGrey3, HDResources->font24, ALLEGRO_ALIGN_CENTER);
	iconTxt->y -= iconTxt->height / 2.0f;
	iconNotifContainer->addChild(iconTxt);

	//text notification
	std::shared_ptr<HD_UI_Container> textNotifContainer = std::make_shared<HD_UI_Container>("textNotifContainer", -288.0f, -142.0f, 331.0f, 124.0f);
	addChild(textNotifContainer);

	std::shared_ptr<HD_UI_GraphicsObject> anchorGfx = std::make_shared<HD_UI_GraphicsObject>();
	anchorGfx->CreateDiamondObject("anchorGfx", textNotifContainer->width - 20.0f, textNotifContainer->height - 10.0f, 20.0f, 20.0f, true, palette->bluesSat.cBlue2);
	textNotifContainer->addChild(anchorGfx);

	std::shared_ptr<HD_UI_GraphicsObject> baseGfx = std::make_shared<HD_UI_GraphicsObject>();
	baseGfx->CreateBGGradientRectangleObject("baseGfx", 0.0f, 0.0f, textNotifContainer->width, textNotifContainer->height, true, palette->bluesSat.cBlue2);
	textNotifContainer->addChild(baseGfx);

	std::shared_ptr<HD_UI_GraphicsObject> topLine = std::make_shared<HD_UI_GraphicsObject>();
	topLine->CreateLineObject("topLine", 0.0f, -1.0f, textNotifContainer->width, 0.0f, 2.0f, palette->bluesSat.cBlue2);
	textNotifContainer->addChild(topLine);

	std::shared_ptr<HD_UI_GraphicsObject> subjectLine = std::make_shared<HD_UI_GraphicsObject>();
	subjectLine->CreateLineObject("subjectLine", 10.0f, 50.5f, textNotifContainer->width - 20.0f, 0.0f, 1.0f, palette->bluesSat.cBlue2);
	textNotifContainer->addChild(subjectLine);

	std::shared_ptr<HD_UI_TextObject> subjectTxt = std::make_shared<HD_UI_TextObject>();
	subjectTxt->CreateSinglelineText("subjectTxt", 10.0f, 8.0f, "Subject Line t...", palette->bluesSat.cBlue2, HDResources->font30);
	textNotifContainer->addChild(subjectTxt);

	std::shared_ptr<HD_UI_TextObject> dateTxt = std::make_shared<HD_UI_TextObject>();
	dateTxt->CreateSinglelineText("dateTxt", textNotifContainer->width - 10.0f, 18.0f, "99:99, 9.09.9999", palette->bluesSat.cBlue2, HDResources->font18, ALLEGRO_ALIGN_RIGHT);
	textNotifContainer->addChild(dateTxt);

	std::shared_ptr<HD_UI_TextObject> detailsTxt = std::make_shared<HD_UI_TextObject>();
	detailsTxt->CreateMultilineText("detailsTxt", 10.0f, 60.0f, "Message details long string. This is a dummy text. For real, yo. Waaaaay longer than that.", palette->bluesSat.cBlue2,
		textNotifContainer->width - 20.0f, 2, -1.0f);
	textNotifContainer->addChild(detailsTxt);

	textNotifContainer->alpha = 0; //no need to show it after creation
}

void HD_HUD_EmailNotif::Update()
{
	HD_UI_Container::Update();

	//number of messages/missions
	int noOfMsgs = game->GetWorld()->GetPlayer()->messages.Size();
	int noOfMssns = game->GetWorld()->GetPlayer()->missions.Size();

	//update icon
	std::string sMsgCount = std::to_string(noOfMsgs + noOfMssns);
	getContainerByName("iconNotifContainer")->getTextObjectByName("iconTxt")->setText(sMsgCount.c_str());
	getContainerByName("iconNotifContainer")->visible = noOfMsgs + noOfMssns > 0 ? true : false; //don't show it if there are no emails

	//animate it if needed
	if (noOfMsgs + noOfMssns > 0)
	{
		iconStayTimer += HDScreen->deltaTime;

		if (iconStayTimer > iconStayStill)
		{
			iconStayTimer = 0;
			bounceIcon();
		}
	}

	//update text notification
	if (noOfMsgs > messageCount)
	{
		//received a new message
		messageCount = noOfMsgs;
		setNotifTexts(false);
		showNotification(true);
	}
	else messageCount = noOfMsgs;

	if (noOfMssns > missionsCount)
	{
		//received a new mission
		//this overwrites messages notifs
		missionsCount = noOfMssns;
		setNotifTexts(true);
		showNotification(true);
	}
	else missionsCount = noOfMssns;

	//update text notification timer
	if (getContainerByName("textNotifContainer")->visible)
	{
		notifTimeOnScreen += HDScreen->deltaTime;

		if (notifTimeOnScreen > notifStayOnScreen)
		{
			showNotification(false);
			notifTimeOnScreen = 0;
		}
	}
}

void HD_HUD_EmailNotif::setNotifTexts(bool isMission)
{
	std::shared_ptr<HD_UI_Container> txtNotifCont = getContainerByName("textNotifContainer");

	std::string sDate = game->GetWorld()->date.GetShortString();
	txtNotifCont->getTextObjectByName("dateTxt")->setText(sDate.c_str());

	std::string sSubject;
	std::string sDetails;

	if (isMission)
	{	//get mission message data
		int missionIndex = game->GetWorld()->GetPlayer()->missions.Size() - 1;

		sSubject = game->GetWorld()->GetPlayer()->missions.GetData(missionIndex)->description;
		sDetails = game->GetWorld()->GetPlayer()->missions.GetData(missionIndex)->GetFullDetails();
	}
	else
	{	//get email message data
		int messageIndex = game->GetWorld()->GetPlayer()->messages.Size() - 1;

		sSubject = game->GetWorld()->GetPlayer()->messages.GetData(messageIndex)->GetSubject();
		sDetails = game->GetWorld()->GetPlayer()->messages.GetData(messageIndex)->GetBody();
	}

	//shorten subject line
	if (sSubject.size() > 13) sSubject = sSubject.substr(0, 13) + "...";
	txtNotifCont->getTextObjectByName("subjectTxt")->setText(sSubject.c_str());

	txtNotifCont->getTextObjectByName("detailsTxt")->setText(sDetails.c_str(), txtNotifCont->width - 20.0f, 2);

	notifTimeOnScreen = 0.0f; //reset timer
}

void HD_HUD_EmailNotif::showNotification(bool show)
{
	std::shared_ptr<HD_UI_Container> txtNotifCont = getContainerByName("textNotifContainer");

	if (show)
	{
		txtNotifCont->visible = true;
		txtNotifCont->y = -142.0f;
		CDBTweener::CTween *introTween = new CDBTweener::CTween(&CDBTweener::TWEQ_CIRCULAR, CDBTweener::TWEA_OUT, 0.5f, &txtNotifCont->alpha, 1.0f);
		txtNotifCont->addAnimation(introTween, true);
	}
	else
	{
		CDBTweener::CTween *outroTween = new CDBTweener::CTween(&CDBTweener::TWEQ_CIRCULAR, CDBTweener::TWEA_IN, 0.5f, &txtNotifCont->alpha, 0.0f);
		outroTween->addValue(&txtNotifCont->y, -120.0f);
		txtNotifCont->addAnimation(outroTween, true);
	}
}

void HD_HUD_EmailNotif::bounceIcon()
{
	std::shared_ptr<HD_UI_Container> iconNotifCont = getContainerByName("iconNotifContainer");

	CDBTweener::CTween *bounce = new CDBTweener::CTween(&CDBTweener::TWEQ_BACK, CDBTweener::TWEA_OUT, 0.25f, &iconNotifCont->y, -30.0f);
	iconNotifCont->addAnimation(bounce, true, 2, 1);
}