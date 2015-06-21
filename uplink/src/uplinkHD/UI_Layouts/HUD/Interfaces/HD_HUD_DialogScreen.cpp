#include "HD_HUD_DialogScreen.h"

#include "../../HD_Root.h"
#include "../../../HD_Resources.h"
#include "../../../HD_ColorPalettes.h"

#include "../../../UI_Objects/HD_UI_TextObject.h"
#include "../../../UI_Objects/HD_UI_Button.h"
#include "../../../UI_Objects/HD_UI_ButtonInput.h"

#include "../../../../game/game.h"
#include "../../../../game/scriptlibrary.h"
#include "../../../../interface/interface.h"
#include "../../../../interface/remoteinterface/remoteinterface.h"
#include "../../../../world/computer/computer.h"
#include "../../../../world/computer/computerscreen/dialogscreen.h"
#include "../../../../world/computer/recordbank.h"

void HD_HUD_DialogScreen::NextPageClick(int nextPage)
{
	if (nextPage != -1)
		root->GetHUDMainWindow()->GoToScreen(nextPage);
}

void HD_HUD_DialogScreen::ScriptButtonClick(int scriptId, int nextPage)
{
	if (scriptId != -1)
		ScriptLibrary::RunScript(scriptId);

	if (nextPage != -1)
		root->GetHUDMainWindow()->GoToScreen(nextPage);
}

HD_HUD_DialogScreen::HD_HUD_DialogScreen()
{
	setObjectProperties("dialogScreen", 0.0f, 0.0f, root->width / 2.0f, root->height);
}

void HD_HUD_DialogScreen::Create(ComputerScreen *cScreen)
{
	DialogScreen* dlgScreen = (DialogScreen*)cScreen;
	Computer* comp = dlgScreen->GetComputer();

	//maintitle
	std::shared_ptr<HD_UI_TextObject> mainTitle = std::make_shared<HD_UI_TextObject>();
	mainTitle->CreateSinglelineText("mainTitle", 12.0f, 132.0f, dlgScreen->maintitle, palette->bluesSat.cBlue2, HDResources->font48);
	addChild(mainTitle);

	//subtitle
	std::shared_ptr<HD_UI_TextObject> subTitle = std::make_shared<HD_UI_TextObject>();
	subTitle->CreateSinglelineText("subTitle", 12.0f, 186.0f, dlgScreen->subtitle, palette->blues.cBlue3, HDResources->font30);
	addChild(subTitle);

	for (int ii = 0; ii < dlgScreen->widgets.Size(); ii++)
	{
		DialogScreenWidget* widget = dlgScreen->widgets.GetData(ii);

		switch (widget->WIDGETTYPE)
		{
		case WIDGET_BASIC:
		{
			std::shared_ptr<HD_UI_TextObject> basicText = std::make_shared<HD_UI_TextObject>();
			basicText->CreateSinglelineText(widget->GetName(), widget->x, widget->y, widget->GetCaption(), palette->bluesSat.cBlue2);
			addChild(basicText);
		}
			break;

		case WIDGET_CAPTION:
		{
			std::shared_ptr<HD_UI_TextObject> caption = std::make_shared<HD_UI_TextObject>();
			caption->CreateMultilineText(widget->GetName(), widget->x, widget->y, widget->GetCaption(), palette->bluesSat.cBlue2, width - 160.0f, 0, 0);
			addChild(caption);
		}
			break;

		case WIDGET_TEXTBOX:
		{
			std::shared_ptr<HD_UI_ButtonInput> input = std::make_shared<HD_UI_ButtonInput>();
			input->CreateSinglelineInput(widget->GetName(), widget->GetCaption(), widget->GetTooltip(), widget->x, widget->y, widget->width, widget->height, true, palette->btnInputColors_blueSat);
			addChild(input);
		}
			break;

		case WIDGET_PASSWORDBOX:
		{
			std::shared_ptr<HD_UI_ButtonInput> passInput = std::make_shared<HD_UI_ButtonInput>();
			passInput->CreateSinglelineInput(widget->GetName(), widget->GetCaption(), widget->GetTooltip(), widget->x, widget->y, widget->width, widget->height, true, palette->btnInputColors_blueSat);
			addChild(passInput);
			passInput->isPassword = true;
		}
			break;

		case WIDGET_NEXTPAGE:
		{
			std::shared_ptr<HD_UI_Button> btnNext = std::make_shared<HD_UI_Button>();
			btnNext->CreateRectangleButton(widget->GetName(), widget->GetCaption(), widget->GetTooltip(), widget->x, widget->y,
				widget->width, widget->height, palette->btnColors_blueSat, false);
			addChild(btnNext);
			btnNext->setCallback(std::bind(&HD_HUD_DialogScreen::NextPageClick, this, widget->data1));
		}
			break;

		case WIDGET_IMAGE:
			//not used
			break;

		case WIDGET_IMAGEBUTTON:
			//not used
			break;

		case WIDGET_SCRIPTBUTTON:
		{
			std::shared_ptr<HD_UI_Button> btnScript = std::make_shared<HD_UI_Button>();
			btnScript->CreateRectangleButton(widget->GetName(), widget->GetCaption(), widget->GetTooltip(), widget->x, widget->y,
				widget->width, widget->height, palette->btnColors_blueSat, false);
			addChild(btnScript);
			btnScript->setCallback(std::bind(&HD_HUD_DialogScreen::ScriptButtonClick, this, widget->data1, widget->data2));
		}
			break;

		case WIDGET_FIELDVALUE:
		{
			std::shared_ptr<HD_UI_TextObject> fieldValue = std::make_shared<HD_UI_TextObject>();
			fieldValue->CreateSinglelineText(widget->GetName(), widget->x, widget->y, widget->GetCaption(), palette->bluesSat.cBlue2);
			addChild(fieldValue);

			Record *rec = dlgScreen->GetComputer()->recordbank.GetRecordFromName(game->GetInterface()->GetRemoteInterface()->security_name);
			if (rec) {
				char *value = rec->GetField(widget->GetStringData1());
				if (value) fieldValue->setText(value);
			}
		}
			break;
		}
	}
}