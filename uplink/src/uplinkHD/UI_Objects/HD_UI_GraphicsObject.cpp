// Implementation of the Graphics UI Object
//=========================================

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <functional>

#include "HD_UI_GraphicsObject.h"
#include "../HD_Screen.h"
#include "../HD_Resources.h"

//============================
// Protected Drawing Functions
//============================

void HD_UI_GraphicsObject::DrawImageGfx()
{
	//Image Graphics drawing
	if (gfxImage)
	{
		float imageW = al_get_bitmap_width(gfxImage);
		float imageH = al_get_bitmap_height(gfxImage);
		al_draw_scaled_bitmap(gfxImage, 0.0f, 0.0f, imageW, imageH, x, y, width, height, 0);
	}
	else {
		//IF the image doesn't exist DRAW TEH HORROR!
		al_draw_filled_rectangle(x, y, x + width, y + height, al_map_rgb(255, 0, 255));
		al_draw_line(x, y, x + width, y + height, al_map_rgb(255, 255, 255), 1);
		al_show_native_message_box(NULL, "Warning!", "", "Could not find image! It will now turn into PINK!", NULL, ALLEGRO_MESSAGEBOX_WARN);
	}
}

void HD_UI_GraphicsObject::DrawStrokedRectGfx(int nThickness)
{
	//Stroke Rectangle
	al_draw_rectangle(x, y, x + width, y + height, color1, nThickness);
}

void HD_UI_GraphicsObject::DrawFillRectGfx()
{
	//Filled Rectangle
	al_draw_filled_rectangle(x, y, x + width, y + height, color1);
}

void HD_UI_GraphicsObject::DrawShavedStrokedRectGfx(float nThickness, ShavedRectCornerLoc corner, float nCornerWidth)
{
	//TO-DO
}

void HD_UI_GraphicsObject::DrawShavedFillRectGfx(ShavedRectCornerLoc corner, float nCornerWidth)
{
	//TO-DO
}

void HD_UI_GraphicsObject::DrawLineGfx(float nThickness)
{
	//A simple Line!
	al_draw_line(x, y, width, height, color1, nThickness);
}

void HD_UI_GraphicsObject::DrawFillRectLineHeaderFooterGfx(bool bIsLineHeader, float nLineThickness)
{
	//First draws a Filled Rectangle, than a line as it's Header/Footer
	al_draw_filled_rectangle(x, y, x + width, y + height, color1);

	if (bIsLineHeader)
	{
		al_draw_line(x, y, x + width, y, color2, nLineThickness);
	}
	else {
		al_draw_line(x, y + height, x + width, y + height, color2, nLineThickness);
	}
}

void HD_UI_GraphicsObject::DrawGradientRectGFX(bool bIsVertical, float gradMidPoint)
{
	//TO-DO
}

//============================
// Public Creation Functions
//============================

//Image Object
HD_UI_GraphicsObject::HD_UI_GraphicsObject(char *objectName, int nIndex, char *imageName, char *atlasName,
	float nX, float nY, float nWidth, float nHeight, HD_UI_Layout *newParent)
{
	name = objectName;
	index = nIndex;

	//HD_Screen *screen = HD_Screen::HD_GetScreen();
	HD_Resources *res = HD_Resources::HD_GetResources();

	if (atlasName)
	{
		gfxImage = res->hd_getSubImage(imageName, atlasName);
	}
	else
	{
		gfxImage = res->hd_getImage(imageName);
	}

	//If a parent isn't supplied, the object's
	//gonna get assigned to the currentLayout
	if (newParent)
		setParent(newParent, nIndex);
	//else
		//setParent(screen->hd_getCurrentLayout(), nIndex);

	x = parent->x + nX;
	y = parent->y + nY;

	width = nWidth;
	height = nHeight;

	//scale values
	//screen->hd_scaleByWidth(x);
	//screen->hd_scaleByWidth(y);
	//screen->hd_scaleByWidth(x);
	//screen->hd_scaleByWidth(x);

	//Set the draw function
	drawFunction = std::bind(&HD_UI_GraphicsObject::DrawImageGfx, this);
}

//Stroked Rect object
HD_UI_GraphicsObject::HD_UI_GraphicsObject(char *objectName, int nIndex, float nX, float nY, float nWidth, float nHeight,
	int nThickness, ALLEGRO_COLOR color, HD_UI_Layout *newParent)
{

}

//Filled Rect object
HD_UI_GraphicsObject::HD_UI_GraphicsObject(char *objectName, int nIndex, float nX, float nY, float nWidth, float nHeight,
	ALLEGRO_COLOR color, HD_UI_Layout *newParent)
{

}

//Shaved Stroked Rect object
HD_UI_GraphicsObject::HD_UI_GraphicsObject(char *objectName, int nIndex, float nX, float nY, float nWidth, float nHeight,
	float nThickness, ShavedRectCornerLoc corner, float nCornerWidth, ALLEGRO_COLOR color, HD_UI_Layout *newParent)
{

}

//Shaved Filled Rect object
HD_UI_GraphicsObject::HD_UI_GraphicsObject(char *objectName, int nIndex, float nX, float nY, float nWidth, float nHeight,
	ShavedRectCornerLoc corner, float nCornerWidth, ALLEGRO_COLOR color, HD_UI_Layout *newParent)
{

}

//Line object
HD_UI_GraphicsObject::HD_UI_GraphicsObject(char *objectName, int nIndex, float nX, float nY, float nX2, float nY2,
	float nThickness, ALLEGRO_COLOR color, HD_UI_Layout *newParent)
{

}

//Filled Rect with Line Header/Footer object
HD_UI_GraphicsObject::HD_UI_GraphicsObject(char *objectName, int nIndex, float nX, float nY, float nWidth, float nHeight,
	ALLEGRO_COLOR rectColor, bool bIsLineHeader, float nLineThickness, ALLEGRO_COLOR lineColor, HD_UI_Layout *newParent)
{

}

//Gradient Rect object
HD_UI_GraphicsObject::HD_UI_GraphicsObject(char *objectName, int nIndex, float nX, float nY, float nWidth, float nHeight,
	bool bIsVertical, float gradMidPoint, ALLEGRO_COLOR color1, ALLEGRO_COLOR color2, HD_UI_Layout *newParent)
{

}

//destruction
HD_UI_GraphicsObject::~HD_UI_GraphicsObject()
{
	al_destroy_bitmap(gfxImage);
}

//============================
// General Functions
//============================

void HD_UI_GraphicsObject::Update()
{

}

void HD_UI_GraphicsObject::Draw()
{

}

void HD_UI_GraphicsObject::Clear()
{
	index = 0;
	name = NULL;
	x = 0;
	y = 0;
	width = 0;
	height = 0;
	parent = NULL;

	delete this;
}