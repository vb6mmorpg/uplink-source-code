// Implementation of Loading
//==========================

#include "HD_Loading.h"

#include "HD_Root.h"
#include "../HD_ColorPalettes.h"
#include "../HD_Resources.h"
#include "../HD_Screen.h"

#include "../../app/app.h"

HD_Loading::HD_Loading(const char* textCaption, const char* newLayout, float wait)
{
	setObjectProperties("Loading", 0.0f, 0.0f, root->width, root->height);
	caption = textCaption;
	loadingTime = wait;
	nextLayout = newLayout;
}

void HD_Loading::Create()
{
	std::shared_ptr<HD_UI_GraphicsObject> bgGfx = std::make_shared<HD_UI_GraphicsObject>();
	bgGfx->CreateGradientRectangleObject("background", 0.0f, 0.0f, width, height, true, 0.6f, palette->cBg1, palette->cBg2);
	addChild(bgGfx);

	std::shared_ptr<HD_UI_GraphicsObject> loadingCircle = std::make_shared<HD_UI_GraphicsObject>();
	loadingCircle->CreateImageObject("loadingCircle", "loadingCircle", "misc_atlas.png", root->width / 2.0f, root->height / 2.0f);
	loadingCircle->x -= loadingCircle->width / 2.0f;
	loadingCircle->y -= loadingCircle->height / 2.0f;
	addChild(loadingCircle);

	std::shared_ptr<HD_UI_GraphicsObject> logoCube = std::make_shared<HD_UI_GraphicsObject>();
	logoCube->CreateImageObject("logoCube", "logoCubeS", "misc_atlas.png", root->width / 2.0f, root->height / 2.0f);
	logoCube->x -= logoCube->width / 2.0f;
	logoCube->y -= logoCube->height / 2.0f;
	addChild(logoCube);

	captionObject = std::make_shared<HD_UI_TextObject>();
	captionObject->CreateSinglelineText("caption", width / 2.0f, 658.0f, caption.c_str(), palette->bluesSat.cBlue2, HDResources->font24, ALLEGRO_ALIGN_CENTER);
	addChild(captionObject);

	CDBTweener::CTween *tween = new CDBTweener::CTween(&CDBTweener::TWEQ_LINEAR, CDBTweener::TWEA_IN, 1.0f, &loadingCircle->rotation, 360.0f);
	loadingCircle->addAnimation(tween, false, 1);
}

void HD_Loading::Update()
{
	if (!isVisible) return;

	HD_UI_Container::Update();

	if (!stopTimer)
		screenTimer += HDScreen->deltaTime;

	if (screenTimer > loadingTime)
	{
		screenTimer = 0.0f;
		stopTimer = true;

		//also load the game!
		if (strcmp(nextLayout.c_str(), "HUD") == 0)
			app->LoadGame();

		root->SetNewLayout(nextLayout.c_str(), false, NULL);

		captionObject->setText("Done");

		//hide this screen
		CDBTweener::CTween* tween = new CDBTweener::CTween(&CDBTweener::TWEQ_CIRCULAR, CDBTweener::TWEA_OUT, 1.0f, &alpha, 0.0f);
		tween->addListener(&loadingComplete);
		addAnimation(tween, true);
	}
}

//animation listener
//gets rid of this screen
void HD_Loading::LoadingComplete::onTweenFinished(CDBTweener::CTween *pTween)
{
	root->removeChildFromIndex(root->getChildByName("Loading")->index);
}