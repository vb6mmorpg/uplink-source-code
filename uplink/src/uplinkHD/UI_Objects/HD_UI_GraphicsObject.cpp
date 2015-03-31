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
		static float imageW = al_get_bitmap_width(gfxImage);
		static float imageH = al_get_bitmap_height(gfxImage);
		al_draw_scaled_bitmap(gfxImage, 0.0f, 0.0f, imageW, imageH, x, y, width, height, 0);
	}
	else {
		//IF the image doesn't exist DRAW TEH HORROR!
		al_draw_filled_rectangle(x, y, x + width, y + height, al_map_rgb(255, 0, 255));
		al_draw_line(x, y, x + width, y + height, al_map_rgb(255, 255, 255), 1);
		al_show_native_message_box(NULL, "Warning!", "", "Could not find image! It will now turn into PINK!", NULL, ALLEGRO_MESSAGEBOX_WARN);
	}
}

void HD_UI_GraphicsObject::DrawRectGfx(float fThickness)
{
	if (fThickness < 0.0f)
		al_draw_filled_rectangle(x, y, x + width, y + height, color1);		//Filled Rectangle
	else
		al_draw_rectangle(x, y, x + width, y + height, color1, fThickness); //Stroked Rectangle
}

void HD_UI_GraphicsObject::DrawStrokedFillRectGFX(float fThickness)
{
	//First a filled rectangle than a stroked one
	al_draw_filled_rectangle(x, y, x + width, y + height, color1);
	al_draw_rectangle(x, y, x + width, y + height, color2, fThickness);
}

void HD_UI_GraphicsObject::DrawShavedRectGfx(bool isFilled, float fCornerWidth, ShavedRectCornerLoc corner)
{
	//Test to see if the fCornerWidth is > width or height
	if (fCornerWidth > width)
		fCornerWidth = width;
	if (fCornerWidth > height)
		fCornerWidth = height;

	//Update the vertexes
	if (corner == topLeft)
	{
		shavedRectVerts[0] = { x, y + fCornerWidth, 0, 0, 0, color1 };
		shavedRectVerts[1] = { x + fCornerWidth, y, 0, 0, 0, color1 };
		shavedRectVerts[2] = { x + width, y, 0, 0, 0, color1 };
		shavedRectVerts[3] = { x + width, y + height, 0, 0, 0, color1 };
		shavedRectVerts[4] = { x, y + height, 0, 0, 0, color1 };
	}
	else if (corner == topRight)
	{
		shavedRectVerts[0] = { x, y, 0, 0, 0, color1 };
		shavedRectVerts[1] = { x + width - fCornerWidth, y, 0, 0, 0, color1 };
		shavedRectVerts[2] = { x + width, y + fCornerWidth, 0, 0, 0, color1 };
		shavedRectVerts[3] = { x + width, y + height, 0, 0, 0, color1 };
		shavedRectVerts[4] = { x, y + height, 0, 0, 0, color1 };
	}
	else if (corner == bottomLeft)
	{
		shavedRectVerts[0] = { x, y, 0, 0, 0, color1 };
		shavedRectVerts[1] = { x + width, y, 0, 0, 0, color1 };
		shavedRectVerts[2] = { x + width, y + height, 0, 0, 0, color1 };
		shavedRectVerts[3] = { x + fCornerWidth, y + height, 0, 0, 0, color1 };
		shavedRectVerts[4] = { x, y + height - fCornerWidth, 0, 0, 0, color1 };
	}
	else if (corner == bottomRight)
	{
		shavedRectVerts[0] = { x, y, 0, 0, 0, color1 };
		shavedRectVerts[1] = { x + width, y, 0, 0, 0, color1 };
		shavedRectVerts[2] = { x + width, y + height - fCornerWidth, 0, 0, 0, color1 };
		shavedRectVerts[3] = { x + width - fCornerWidth, y + height, 0, 0, 0, color1 };
		shavedRectVerts[4] = { x, y + height, 0, 0, 0, color1 };
	}

	//Draw them shaved juicy corners (+18)
	if (isFilled)
		al_draw_prim(shavedRectVerts, NULL, NULL, 0, 5, ALLEGRO_PRIM_TRIANGLE_FAN);
	else
		al_draw_prim(shavedRectVerts, NULL, NULL, 0, 5, ALLEGRO_PRIM_LINE_LOOP);
}

void HD_UI_GraphicsObject::DrawLineGfx(int nThickness)
{
	//A simple Line!
	al_draw_line(x, y, x + width, y + height, color1, nThickness);
}

void HD_UI_GraphicsObject::DrawFillRectLineHeaderFooterGfx(bool bIsLineHeader, float fLineThickness)
{
	//First draws a Filled Rectangle, than a line as it's Header/Footer
	al_draw_filled_rectangle(x, y, x + width, y + height, color1);

	if (bIsLineHeader)
		al_draw_line(x, y, x + width, y, color2, fLineThickness);
	else
		al_draw_line(x, y + height, x + width, y + height, color2, fLineThickness);
}

void HD_UI_GraphicsObject::DrawGradientRectGFX(bool bIsVertical, float fStartGrad)
{
	//Creates a horizontal/vertical gradient filled rectangle primitive
	//with the gradient's start point being fStartGrad% of width/height
	//TO-DO - A case were fStartGrad == 0 or 1
	if (!bIsVertical)
	{
		//1st triangle
		gradientRectVerts[0] = { x, y, 0, 0, 0, color1 };
		gradientRectVerts[1] = { x + width * fStartGrad, y + height, 0, 0, 0, color1 };
		gradientRectVerts[2] = { x, y + height, 0, 0, 0, color1 };
		//2nd triangle
		gradientRectVerts[3] = { x, y, 0, 0, 0, color1 };
		gradientRectVerts[4] = { x + width * fStartGrad, y, 0, 0, 0, color1 };
		gradientRectVerts[5] = { x + width * fStartGrad, y + height, 0, 0, 0, color1 };
		//3rd triangle
		gradientRectVerts[6] = { x + width * fStartGrad, y, 0, 0, 0, color1 };
		gradientRectVerts[7] = { x + width, y + height, 0, 0, 0, color2 };
		gradientRectVerts[8] = { x + width * fStartGrad, y + height, 0, 0, 0, color1 };
		//4th triangle
		gradientRectVerts[9] = { x + width * fStartGrad, y, 0, 0, 0, color1 };
		gradientRectVerts[10] = { x + width, y, 0, 0, 0, color2 };
		gradientRectVerts[11] = { x + width, y + height, 0, 0, 0, color2 };
	}
	else
	{
		//1st triangle
		gradientRectVerts[0] = { x, y, 0, 0, 0, color1 };
		gradientRectVerts[1] = { x + width, y, 0, 0, 0, color1 };
		gradientRectVerts[2] = { x + width, y + height * fStartGrad, 0, 0, 0, color1 };
		//2nd triangle
		gradientRectVerts[3] = { x, y, 0, 0, 0, color1 };
		gradientRectVerts[4] = { x + width, y + height * fStartGrad, 0, 0, 0, color1 };
		gradientRectVerts[5] = { x, y + height * fStartGrad, 0, 0, 0, color1 };
		//3rd triangle
		gradientRectVerts[6] = { x, y + height * fStartGrad, 0, 0, 0, color1 };
		gradientRectVerts[7] = { x + width, y + height * fStartGrad, 0, 0, 0, color1 };
		gradientRectVerts[8] = { x + width, y + height, 0, 0, 0, color2 };
		//4th triangle
		gradientRectVerts[9] = { x, y + height * fStartGrad, 0, 0, 0, color1 };
		gradientRectVerts[10] = { x + width, y + height, 0, 0, 0, color2 };
		gradientRectVerts[11] = { x, y + height, 0, 0, 0, color2 };
	}

	al_draw_prim(gradientRectVerts, NULL, NULL, 0, 12, ALLEGRO_PRIM_TRIANGLE_LIST);
}

//============================
// Public Creation Functions
//============================

//Image Object
HD_UI_GraphicsObject::HD_UI_GraphicsObject(char *objectName, int index, char *imageName, char *atlasName,
	float fX, float fY, HD_UI_Container *newParent)
{
	if (atlasName)
	{
		gfxImage = HDResources->hd_getSubImage(imageName, atlasName);
	}
	else
	{
		gfxImage = HDResources->hd_getImage(imageName);
	}

	//To-Do: this is error prone!
	float imageWidth = al_get_bitmap_width(gfxImage);
	float imageHeight = al_get_bitmap_height(gfxImage);

	setObjectProperties(objectName, fX, fY, imageWidth, imageHeight, newParent, index);

	//Set the draw function
	drawGraphicObject = std::bind(&HD_UI_GraphicsObject::DrawImageGfx, this);
}

//Stroked/Filled Rect object
HD_UI_GraphicsObject::HD_UI_GraphicsObject(char *objectName, int index, float fX, float fY, float fWidth, float fHeight,
	float fThickness, ALLEGRO_COLOR color, HD_UI_Container *newParent)
{
	if (fThickness > 0.0f)
	{
		fX += fThickness / 2.0f;		//allegro has float coordinates and draws strokes inside & outside of the
		fY += fThickness / 2.0f;		//generated line; this makes sure the lines will be pixel perfect
	}

	setObjectProperties(objectName, fX, fY, fWidth, fHeight, newParent, index);

	color1 = color;

	drawGraphicObject = std::bind(&HD_UI_GraphicsObject::DrawRectGfx, this, fThickness);
}

//Filled & Stroked Rect object
HD_UI_GraphicsObject::HD_UI_GraphicsObject(char *objectName, int index, float fX, float fY, float fWidth, float fHeight,
	float fThickness, ALLEGRO_COLOR fillColor, ALLEGRO_COLOR lineColor, HD_UI_Container *newParent)
{
	fX += fThickness / 2.0f;
	fY += fThickness / 2.0f;

	setObjectProperties(objectName, fX, fY, fWidth, fHeight, newParent, index);

	color1 = fillColor;
	color2 = lineColor;

	drawGraphicObject = std::bind(&HD_UI_GraphicsObject::DrawStrokedFillRectGFX, this, fThickness);
}

//Shaved Stroked/Filled Rect object
HD_UI_GraphicsObject::HD_UI_GraphicsObject(char *objectName, int index, float fX, float fY, float fWidth, float fHeight,
	bool isFilled, ShavedRectCornerLoc corner, float fCornerWidth, ALLEGRO_COLOR color, HD_UI_Container *newParent)
{
	if (isFilled)
	{
		fX += 0.5f;
		fY += 0.5f;
	}

	setObjectProperties(objectName, fX, fY, fWidth, fHeight, newParent, index);

	color1 = color;

	drawGraphicObject = std::bind(&HD_UI_GraphicsObject::DrawShavedRectGfx, this, isFilled, fCornerWidth, corner);
}

//Line object
HD_UI_GraphicsObject::HD_UI_GraphicsObject(char *objectName, int index, float fX, float fY, float fX2, float fY2,
	int nThickness, ALLEGRO_COLOR color, HD_UI_Container *newParent)
{
	fX += float(nThickness) / 2.0f;
	fY += float(nThickness) / 2.0f;

	setObjectProperties(objectName, fX, fY, fX2, fY2, newParent, index);

	color1 = color;

	drawGraphicObject = std::bind(&HD_UI_GraphicsObject::DrawLineGfx, this, nThickness);
}

//Filled Rect with Line Header/Footer object
HD_UI_GraphicsObject::HD_UI_GraphicsObject(char *objectName, int index, float fX, float fY, float fWidth, float fHeight,
	ALLEGRO_COLOR rectColor, bool bIsLineHeader, float fLineThickness, ALLEGRO_COLOR lineColor, HD_UI_Container *newParent)
{
	setObjectProperties(objectName, fX, fY, fWidth, fHeight, newParent, index);

	color1 = rectColor;
	color2 = lineColor;

	//the pixel perfect correction is done in the drawing function for this one!
	drawGraphicObject = std::bind(&HD_UI_GraphicsObject::DrawFillRectLineHeaderFooterGfx, this, bIsLineHeader, fLineThickness);
}

//Gradient Rect object
HD_UI_GraphicsObject::HD_UI_GraphicsObject(char *objectName, int index, float fX, float fY, float fWidth, float fHeight,
	bool bIsVertical, float fStartGrad, ALLEGRO_COLOR color01, ALLEGRO_COLOR color02, HD_UI_Container *newParent)
{
	setObjectProperties(objectName, fX, fY, fWidth, fHeight, newParent, index);

	color1 = color01;
	color2 = color02;

	drawGraphicObject = std::bind(&HD_UI_GraphicsObject::DrawGradientRectGFX, this, bIsVertical, fStartGrad);
}

//============================
// General Functions
//============================

void HD_UI_GraphicsObject::Draw()
{
	if (isVisible)
		drawGraphicObject();
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