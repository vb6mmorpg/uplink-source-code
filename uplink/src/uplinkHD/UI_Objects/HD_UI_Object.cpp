#include <allegro5/allegro_primitives.h>

#include "HD_UI_Object.h"
#include "HD_UI_Container.h"
#include "../HD_Screen.h"
#include "../HD_Resources.h"
#include "../UI_Layouts/HD_Root.h"

void HD_UI_Object::setObjectProperties(const char *objectName, float fX, float fY, float fWidth, float fHeight)
{
	//setParent(newParent, nIndex);

	name = objectName;

	x = fX;
	y = fY;
	globalX = x;// parent->globalX + x;
	globalY = y;// parent->globalY + y;

	drawWidth = width = fWidth;
	drawHeight = height = fHeight;

	scaleX = 1.0f;
	scaleY = 1.0f;

	alpha = 1.0f;
}

bool HD_UI_Object::isInClippingMask()
{
	int mX, mY, mW, mH = 0;
	al_get_clipping_rectangle(&mX, &mY, &mW, &mH);

	//overlay
	int overW, overH = 0;
	overW = max(0, min(mX + mW, globalX + drawWidth) - max(mX, globalX));
	overH = max(0, min(mY + mH, globalY + drawHeight) - max(mY, globalY));

	if (overW > 0 || overH > 0)
		return true;
	else
		return false;
}

//creates an empty object
HD_UI_Object::HD_UI_Object(const char* objectName, float fX, float fY, float fWidth, float fHeight)
{
	setObjectProperties(objectName, fX, fY, fWidth, fHeight);
}

//functions
void HD_UI_Object::Update()
{
	if (!parent) return;

	isVisible = parent->isVisible && visible;

	if (isVisible)
	{
		//animations update
		tweensContainer.step(HDScreen->deltaTime);

		gIndex = index + parent->gIndex;

		//Update according to parent
		globalScaleX = parent->globalScaleX * scaleX;
		globalScaleY = parent->globalScaleY * scaleY;

		//positioning gets done by both width & height! To-Do
		globalX = parent->globalX + x;
		globalY = parent->globalY + y;

		//rescaling is done by the width of the screen! To-Do
		drawWidth = width * globalScaleX;
		drawHeight = height * globalScaleY;

		drawAlpha = alpha * parent->drawAlpha;
		if (drawAlpha > 1.0f) drawAlpha = 1.0f;
		else if (drawAlpha <= 0.0f) drawAlpha = 0.0f;

		if (alpha <= 0.0f) visible = false;

		redraw = true;
	}
}

void HD_UI_Object::Draw()
{
	if (HDScreen->DebugInfo && redraw)
		DrawDebugInfo(al_map_rgb_f(1.0f, 0.0f, 1.0f));
}

void HD_UI_Object::Clear()
{
	parent = nullptr;
	tweensContainer.removeAllTweens();
}

//animations
//Adds a CTween animation; if removeAnims, it stops all other animations on this object
void HD_UI_Object::addAnimation(CDBTweener::CTween *newTween, bool removeAnims, int loop, int replays)
{
	if (removeAnims)
		tweensContainer.removeAllTweens();

	if (loop == 1)
	{
		newTween->addListener(&m_tweenLoop);
		newTween->setUserData(this);
		newTween->setUserIntData(replays);
	}
	else if (loop == 2)
	{
		newTween->addListener(&m_tweenPingPong);
		newTween->setUserData(this);
		newTween->setUserIntData(replays);
	}

	tweensContainer.addTween(newTween);
}

//DEBUG INFO
void HD_UI_Object::DrawDebugInfo(ALLEGRO_COLOR c)
{
	ALLEGRO_FONT *f = HDResources->debugFont;
	int info = HDScreen->DebugInfo;

	al_draw_line(globalX, globalY, globalX + drawWidth, globalY, c, 1.0f);
	al_draw_line(globalX, globalY, globalX, globalY + drawHeight, c, 1.0f);
	al_draw_line(globalX, globalY, globalX + drawWidth, globalY + drawHeight, c, 1.0f);

	std::string sGX = "x:" + std::to_string(globalX);
	std::string sGY = "y:" + std::to_string(globalY);
	std::string sDW = "dW:" + std::to_string(drawWidth);
	std::string sDH = "dH:" + std::to_string(drawHeight);
	std::string sGSX = "gSX:" + std::to_string(globalScaleX);
	std::string sGSY = "gSY:" + std::to_string(globalScaleY);
	std::string sGI = "gID:" + std::to_string(gIndex);

	if (info == 1 || info == 4)
	{
		al_draw_text(f, c, globalX - al_get_text_width(f, sGX.c_str()) - 2.0f, globalY, 0, sGX.c_str());
		al_draw_text(f, c, globalX, globalY - 12.0f, 0, sGY.c_str());
		al_draw_text(f, c, globalX + drawWidth / 2.0f, globalY + drawHeight / 2.0f, 1, sGI.c_str());
	}

	if (info == 2 || info == 4)
	{
		al_draw_text(f, c, globalX - al_get_text_width(f, sDW.c_str()) + drawWidth, globalY - 12.0f, 0, sDW.c_str());
		al_draw_text(f, c, globalX - al_get_text_width(f, sDH.c_str()) + drawWidth, globalY + drawHeight / 2.0f, 0, sDH.c_str());
	}

	if (info == 3 || info == 4)
	{
		al_draw_text(f, c, globalX + drawWidth + 2.0f, globalY, 0, sGSX.c_str());
		al_draw_text(f, c, globalX + drawWidth + 2.0f, globalY + 12.0f, 0, sGSY.c_str());
	}
}