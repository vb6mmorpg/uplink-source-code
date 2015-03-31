#include "HD_UI_TestLayout.h"

#include "../UI_Objects/HD_UI_Button.h"
#include "../UI_Objects/HD_UI_GraphicsObject.h"
#include "../UI_Objects/HD_UI_TextObject.h"

#include "../HD_Resources.h"

HD_UI_TestLayout::HD_UI_TestLayout()
{
	x = -200.0f;
	y = 0;
	width = 0;
	height = 0;
	scaleX = 1.0;
	scaleY = 1.0;
	index = 0;
	name = "TestLayout";
	parent = NULL;
}

HD_UI_TestLayout::~HD_UI_TestLayout()
{

}

void HD_UI_TestLayout::Create()
{
	//Object Creation Tests
	HD_UI_GraphicsObject *gfxObject1 = new HD_UI_GraphicsObject("gfxObj1", -1, "graphics/mousePointer.png", NULL, 10, 10, this); //image
	HD_UI_GraphicsObject *gfxObject2 = new HD_UI_GraphicsObject("gfxObj2", -1, 20.0f, 30.0f, 100.0f, 20.0f, 1.0f, al_map_rgba(255 * 50 / 255,0,0,50), this); //stroked rect
	HD_UI_GraphicsObject *gfxObject3 = new HD_UI_GraphicsObject("gfxObj3", -1, 20.0f, 60.0f, 100.0f, 20.0f, -1.0f, al_map_rgba(255, 0, 0, 150), this); //filled rect
	HD_UI_GraphicsObject *gfxObject4 = new HD_UI_GraphicsObject("gfxObj4", -1, 400.0f, 80.0f, 600.0f, 200.0f, 3, al_map_rgba(255,0,255,255), this); //line
	HD_UI_GraphicsObject *gfxObject5 = new HD_UI_GraphicsObject("gfxObj5", -1, 20.0f, 120.0f, 100.0f, 20.0f, 2.0f, al_map_rgb(255,0,0), al_map_rgba(255,255,255,45), this); //filled&stroked
	HD_UI_GraphicsObject *gfxObject6 = new HD_UI_GraphicsObject("gfxObj6", -1, 20.0f, 160.0f, 200.0f, 200.0f, al_map_rgb(255, 255, 200), true, 4.0f, al_map_rgb(0,200,200), this); //filled rect with header
	HD_UI_GraphicsObject *gfxObject7 = new HD_UI_GraphicsObject("gfxObj7", -1, 800.0f, 500.0f, 100.0f, 100.0f, true, 0.5f, al_map_rgb(100,100,100), al_map_rgb(200,200,200), this);	//vertical gradient
	HD_UI_GraphicsObject *gfxObject8 = new HD_UI_GraphicsObject("gfxObj8", -1, 900.0f, 300.0f, 100.0f, 100.0f, true, bottomRight, 20.0f, al_map_rgb(0,0,255), this); //shaved filled rect
	HD_UI_GraphicsObject *gfxObject9 = new HD_UI_GraphicsObject("gfxObj9", -1, 900.0f, 400.0f, 100.0f, 100.0f, false, topLeft, 20.0f, al_map_rgb(0, 0, 255), this); //shaved stoked rect

	HD_UI_GraphicsObject *gfxObject10 = new HD_UI_GraphicsObject("gfxObj10", -1, 800.0f, 300.0f, 100.0f, 100.0f, false, 0.2f, al_map_rgb(100, 100, 100), al_map_rgb(200, 200, 200), this);	//horizontal gradient

	//text objects
	HD_UI_TextObject *txtObject1 = new HD_UI_TextObject("textObj1", -1, 20.0f, 360.0f, "This is a singleline text!", HDResources->font30, al_map_rgb(255, 0, 255), ALLEGRO_ALIGN_LEFT, this);
	HD_UI_TextObject *txtObject2 = new HD_UI_TextObject("textObj2", -1, 400.0f, 300.0f, "This is a multiline text! This is a multiline text! This is a multiline text!",
		HDResources->font30, al_map_rgb(255, 0, 255), ALLEGRO_ALIGN_LEFT, 200.0f, 0.0f, this);

	//button!
	ALLEGRO_COLOR colors[6] =
	{
		al_map_rgb(100, 100, 100),
		al_map_rgb(150, 150, 150),
		al_map_rgb(255, 255, 255),
		al_map_rgb(255, 0, 0),
		al_map_rgb(0, 255, 0),
		al_map_rgb(255, 255, 255)
	};
	HD_UI_Button *btnObject = new HD_UI_Button("btnObject", -1, "Button!", NULL, "Tooltip", 400.0f, 200.0f, 100.0f, 60.0f, colors, false, HDResources->font24, 0, 0, this);

	//Animation Tests
	CDBTweener::CTween *tween1 = new CDBTweener::CTween(&CDBTweener::TWEQ_BACK, CDBTweener::TWEA_OUT, 1.0f, &this->x, 0.0f);
	this->addAnimation(tween1);

	for (unsigned int ii = 0; ii < children.size(); ii++)
	{
		CDBTweener::CTween *tween = new CDBTweener::CTween(&CDBTweener::TWEQ_BACK, CDBTweener::TWEA_OUT, ii * 0.25f, &children[ii]->_y, ii / 2 * 100.0f);
		children[ii]->addAnimation(tween);
	}
}