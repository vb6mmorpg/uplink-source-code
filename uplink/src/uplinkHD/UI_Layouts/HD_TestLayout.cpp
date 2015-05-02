#include "HD_TestLayout.h"

#include "../UI_Objects/HD_UI_Button.h"
#include "../UI_Objects/HD_UI_GraphicsObject.h"
#include "../UI_Objects/HD_UI_TextObject.h"
#include "../UI_Objects/HD_UI_ButtonInput.h"

#include "../HD_Resources.h"
#include "../HD_ColorPalettes.h"
#include "../HD_Screen.h"

HD_TestLayout::HD_TestLayout()
{
	x = 0.0f;
	y = 0.0f;
	width = 0;
	height = 0;
	scaleX = 1.0;
	scaleY = 1.0;
	index = 0;
	name = "TestLayout";
	parent = NULL;
}

HD_TestLayout::~HD_TestLayout()
{

}

void HD_TestLayout::Create()
{
	//Object Creation Tests
	HD_UI_GraphicsObject *bgGradient = new HD_UI_GraphicsObject("bgGradient", -1, 0.0f, 0.0f, HDScreen->nScreenW, HDScreen->nScreenH, true, 0.4f, palette->warmGreys.cGrey0, palette->blues.cBlue1, this);

	HD_UI_GraphicsObject *gfxObject1 = new HD_UI_GraphicsObject("gfxObj1", -1, "mousePointer.png", NULL, 10, 10, this); //image
	HD_UI_GraphicsObject *gfxObject2 = new HD_UI_GraphicsObject("gfxObj2", -1, 20.0f, 30.0f, 100.0f, 20.0f, 1.0f, al_map_rgba(255 * 50 / 255,0,0,50), this); //stroked rect
	HD_UI_GraphicsObject *gfxObject3 = new HD_UI_GraphicsObject("gfxObj3", -1, 20.0f, 60.0f, 100.0f, 20.0f, -1.0f, al_map_rgba(255, 0, 0, 150), this); //filled rect
	HD_UI_GraphicsObject *gfxObject4 = new HD_UI_GraphicsObject("gfxObj4", -1, 400.0f, 80.0f, 600.0f, 200.0f, 3, al_map_rgba(255,0,255,255), this); //line
	HD_UI_GraphicsObject *gfxObject5 = new HD_UI_GraphicsObject("gfxObj5", -1, 20.0f, 120.0f, 100.0f, 20.0f, 2.0f, al_map_rgb(255,0,0), al_map_rgba(255,255,255,45), this); //filled&stroked
	HD_UI_GraphicsObject *gfxObject6 = new HD_UI_GraphicsObject("gfxObj6", -1, 20.0f, 160.0f, 200.0f, 200.0f, al_map_rgb(255, 255, 200), true, 4.0f, al_map_rgb(0,200,200), this); //filled rect with header
	HD_UI_GraphicsObject *gfxObject7 = new HD_UI_GraphicsObject("gfxObj7", -1, 800.0f, 500.0f, 100.0f, 100.0f, true, 0.5f, al_map_rgb(100,100,100), al_map_rgb(200,200,200), this);	//vertical gradient
	HD_UI_GraphicsObject *gfxObject8 = new HD_UI_GraphicsObject("gfxObj8", -1, 900.0f, 300.0f, 100.0f, 100.0f, true, bottomRight, 20.0f, al_map_rgb(0,0,255), this); //shaved filled rect
	HD_UI_GraphicsObject *gfxObject9 = new HD_UI_GraphicsObject("gfxObj9", -1, 900.0f, 400.0f, 100.0f, 100.0f, false, topLeft, 20.0f, al_map_rgb(0, 0, 255), this); //shaved stoked rect

	HD_UI_GraphicsObject *gfxObject10 = new HD_UI_GraphicsObject("gfxObj10", -1, 800.0f, 300.0f, 100.0f, 100.0f, false, 0.2f, al_map_rgb(100, 100, 100), al_map_rgb(200, 200, 200), this);	//horizontal gradient

	HD_UI_GraphicsObject *gfxObject11 = new HD_UI_GraphicsObject("gfxObj11", -1, "btnExit_c", "mainMenu_atlas.xml", 100.0f, 600.0f, this);

	//text objects
	HD_UI_TextObject *txtObject1 = new HD_UI_TextObject("textObj1", -1, 20.0f, 360.0f, "This is a singleline\n text!", HDResources->font30, al_map_rgb(255, 0, 255), ALLEGRO_ALIGN_LEFT, this);
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
	HD_UI_Button *btnObject = new HD_UI_Button("btnObject", -1, "Button!", "Tooltip", 400.0f, 200.0f, 100.0f, 60.0f, palette->btnColors_white, false, HDResources->font24, ALLEGRO_ALIGN_CENTER, this);
	HD_UI_Button *btnObject2 = new HD_UI_Button("btnObject2", -1, "Button2!", "Tooltip", 350.0f, 230.0f, 100.0f, 60.0f, palette->btnColors_red, false, HDResources->font24, ALLEGRO_ALIGN_LEFT, this);
	HD_UI_Button *btnObject3 = new HD_UI_Button("btnObject3", -1, "Button3!", "Tooltip", 100.0f, 50.0f, 100.0f, 30.0f, palette->btnColors_blue, false, HDResources->font24, ALLEGRO_ALIGN_CENTER, this);
	HD_UI_Button *btnObject4 = new HD_UI_Button("btnObject4", -1, "Button4!", "Tooltip", 560.0f, 20.0f, 100.0f, 30.0f, palette->btnColors_orange, true, HDResources->font24, ALLEGRO_ALIGN_RIGHT, this);

	HD_UI_ButtonInput *txtInputObject = new HD_UI_ButtonInput("textInput", -1, "Search", "Tooltip", 520.0f, 200.0f, 200.0f, 30.0f, true, palette->btnColors_white, HDResources->font24, this);

	//Animation Tests
	CDBTweener::CTween *tween1 = new CDBTweener::CTween(&CDBTweener::TWEQ_BACK, CDBTweener::TWEA_OUT, 1.0f, &this->x, 0.0f);
	this->addAnimation(tween1);

	for (unsigned int ii = 0; ii < children.size(); ii++)
	{
		CDBTweener::CTween *tween = new CDBTweener::CTween(&CDBTweener::TWEQ_BACK, CDBTweener::TWEA_OUT, ii * 0.25f, &children[ii]->y, ii / 2 * 100.0f);
		//children[ii]->addAnimation(tween);
	}
}