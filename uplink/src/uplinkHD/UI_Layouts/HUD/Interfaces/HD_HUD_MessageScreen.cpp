#include "HD_HUD_MessageScreen.h"

#include "../../../HD_Resources.h"
#include "../../../HD_ColorPalettes.h"

#include "../../HD_Root.h"
#include "../Interfaces/HD_HUD_MainWindow.h"

#include "../../../UI_Objects/HD_UI_Button.h"
#include "../../../UI_Objects/HD_UI_TextObject.h"

#include "../../../../world/message.h"
#include "../../../../world/computer/computer.h"

void HD_HUD_MessageScreen::MailMeClick(MessageScreen *cScreen)
{
	Message *m = new Message();
	m->SetTo("PLAYER");
	m->SetFrom(cScreen->GetComputer()->name);
	m->SetSubject(cScreen->subtitle);
	m->SetBody(cScreen->textmessage);
	m->Send();
}

void HD_HUD_MessageScreen::OKClick(MessageScreen *cScreen)
{
	int nextPage = cScreen->nextpage;

	if (nextPage != -1)
		root->GetHUDMainWindow()->GoToScreen(nextPage);
}

HD_HUD_MessageScreen::HD_HUD_MessageScreen()
{
	setObjectProperties("messageScreen", 0.0f, 0.0f, root->width / 2.0f, root->height);
}

void HD_HUD_MessageScreen::Create(ComputerScreen *cScreen)
{
	MessageScreen *msgScreen = (MessageScreen*) cScreen;

	//maintitle
	std::shared_ptr<HD_UI_TextObject> mainTitle = std::make_shared<HD_UI_TextObject>();
	mainTitle->CreateSinglelineText("mainTitle", 12.0f, 132.0f, msgScreen->maintitle, palette->bluesSat.cBlue2, HDResources->font48);
	addChild(mainTitle);

	//subtitle
	std::shared_ptr<HD_UI_TextObject> subTitle = std::make_shared<HD_UI_TextObject>();
	subTitle->CreateSinglelineText("subTitle", 12.0f, 186.0f, msgScreen->subtitle, palette->blues.cBlue3, HDResources->font30);
	addChild(subTitle);

	//message
	std::shared_ptr<HD_UI_TextObject> messageTxt = std::make_shared<HD_UI_TextObject>();
	messageTxt->CreateMultilineText("messageTxt", width / 2.0f, 360.0f, msgScreen->textmessage, palette->bluesSat.cBlue2, width - 160.0f, 0, 0, HDResources->font30, ALLEGRO_ALIGN_CENTER);
	addChild(messageTxt);


	//buttons
	if (msgScreen->buttonmessage)
	{
		std::shared_ptr<HD_UI_Button> btnMsg = std::make_shared<HD_UI_Button>();
		btnMsg->CreateRectangleButton("btnMsg", msgScreen->buttonmessage, msgScreen->buttonmessage, width / 2.0f - 80.0f, 660.0f, 160.0f, 30.0f, palette->btnColors_blueSat, false);
		addChild(btnMsg);
		btnMsg->setCallback(std::bind(&HD_HUD_MessageScreen::OKClick, this, msgScreen));
	}

	if (msgScreen->mailthistome)
	{
		std::shared_ptr<HD_UI_Button> btnMail = std::make_shared<HD_UI_Button>();
		btnMail->CreateRectangleButton("btnMail", "Mail this to me", "Send this to yourself.", width / 2.0f - 80.0f, 660.0f, 160.0f, 30.0f, palette->btnColors_blueSat, false);
		addChild(btnMail);
		btnMail->setCallback(std::bind(&HD_HUD_MessageScreen::MailMeClick, this, msgScreen));
	}

	//if both are present, position them correctly
	if (msgScreen->buttonmessage && msgScreen->mailthistome)
	{
		getChildByName("btnMsg")->x += width / 3.0f;
		getChildByName("btnMail")->x -= width / 3.0f;
	}

	//rearrange the layout
	if (messageTxt->getTextNumLines() > 6)
	{
		messageTxt->setTextAlign(ALLEGRO_ALIGN_LEFT);
		messageTxt->x = 80.0f;
		if (msgScreen->buttonmessage)
			getChildByName("btnMsg")->y = messageTxt->y + messageTxt->height + 60.0f;
		if (msgScreen->mailthistome)
			getChildByName("btnMail")->y = getChildByName("btnMsg")->y;
	}
}