// Implementation of the Graphics UI Object
//=========================================

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <functional>

#include "HD_UI_GraphicsObject.h"
#include "../HD_Screen.h"
#include "../HD_Resources.h"
#include "../HD_ColorPalettes.h"

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

		ALLEGRO_COLOR c = palette->addAlphaToColor(al_map_rgb_f(1.0f, 1.0f, 1.0f), drawAlpha);
		float r = (rotation * ALLEGRO_PI) / 180.0f;

		//Images get scaled by the X scale!
		al_draw_tinted_scaled_rotated_bitmap(gfxImage, c, imageW / 2.0f, imageH / 2.0f, globalX + drawWidth / 2.0f, globalY + drawHeight / 2.0f, globalScaleX, globalScaleX, r, 0);
	}
	else {
		//IF the image doesn't exist DRAW TEH HORROR!
		al_draw_filled_rectangle(globalX, globalY, globalX + drawWidth, globalY + drawHeight, al_map_rgb(255, 0, 255));
		al_draw_line(globalX, globalY, globalX + drawWidth, globalY + drawHeight, al_map_rgb(255, 255, 255), 1);
		al_show_native_message_box(NULL, "Warning!", "", "Could not find image! It will now turn into PINK!", NULL, ALLEGRO_MESSAGEBOX_WARN);
	}
}

void HD_UI_GraphicsObject::DrawRectGfx(float fThickness)
{
	ALLEGRO_COLOR c1 = palette->addAlphaToColor(color1, color1.a * drawAlpha);

	if (fThickness <= 0.0f)
		al_draw_filled_rectangle(globalX, globalY, globalX + drawWidth, globalY + drawHeight, c1);		//Filled Rectangle
	else
		al_draw_rectangle(globalX, globalY, globalX + drawWidth, globalY + drawHeight, c1, fThickness);	//Stroked Rectangle
}

void HD_UI_GraphicsObject::DrawDiamondGfx(bool isFilled)
{
	ALLEGRO_COLOR c1 = palette->addAlphaToColor(color1, color1.a * drawAlpha);
	ALLEGRO_VERTEX diamondVerts[4];		//vertex array

	diamondVerts[0] = { globalX, globalY + drawHeight / 2.0f, 0, 0, 0, c1 };
	diamondVerts[1] = { globalX + drawWidth / 2.0f, globalY, 0, 0, 0, c1 };
	diamondVerts[2] = { globalX + drawWidth, globalY + drawHeight / 2.0f, 0, 0, 0, c1 };
	diamondVerts[3] = { globalX + drawWidth / 2.0f, globalY + drawHeight, 0, 0, 0, c1 };

	if (isFilled)
		al_draw_prim(diamondVerts, NULL, NULL, 0, 4, ALLEGRO_PRIM_TRIANGLE_FAN);
	else
		al_draw_prim(diamondVerts, NULL, NULL, 0, 4, ALLEGRO_PRIM_LINE_LOOP);
}

void HD_UI_GraphicsObject::DrawStrokedFillRectGFX(float fThickness)
{
	ALLEGRO_COLOR c1 = palette->addAlphaToColor(color1, color1.a * drawAlpha);
	ALLEGRO_COLOR c2 = palette->addAlphaToColor(color2, color2.a * drawAlpha);

	//First a filled rectangle than a stroked one
	al_draw_filled_rectangle(globalX, globalY, globalX + drawWidth, globalY + drawHeight, c1);
	al_draw_rectangle(globalX, globalY, globalX + drawWidth, globalY + drawHeight, c2, fThickness);
}

void HD_UI_GraphicsObject::DrawShavedRectGfx(bool isFilled, float fCornerWidth, ShavedRectCornerLoc corner)
{
	//Test to see if the fCornerWidth is > width or height
	if (fCornerWidth > drawWidth)
		fCornerWidth = drawWidth;
	if (fCornerWidth > drawHeight)
		fCornerWidth = drawHeight;

	ALLEGRO_COLOR c1 = palette->addAlphaToColor(color1, color1.a * drawAlpha);
	ALLEGRO_VERTEX shavedRectVerts[5];		//vertex array

	//Update the verts
	if (corner == topLeft)
	{
		shavedRectVerts[0] = { globalX, globalY + fCornerWidth, 0, 0, 0, c1 };
		shavedRectVerts[1] = { globalX + fCornerWidth, globalY, 0, 0, 0, c1 };
		shavedRectVerts[2] = { globalX + drawWidth, globalY, 0, 0, 0, c1 };
		shavedRectVerts[3] = { globalX + drawWidth, globalY + drawHeight, 0, 0, 0, c1 };
		shavedRectVerts[4] = { globalX, globalY + drawHeight, 0, 0, 0, c1 };
	}
	else if (corner == topRight)
	{
		shavedRectVerts[0] = { globalX, globalY, 0, 0, 0, color1 };
		shavedRectVerts[1] = { globalX + drawWidth - fCornerWidth, globalY, 0, 0, 0, c1 };
		shavedRectVerts[2] = { globalX + drawWidth, globalY + fCornerWidth, 0, 0, 0, c1 };
		shavedRectVerts[3] = { globalX + drawWidth, globalY + drawHeight, 0, 0, 0, c1 };
		shavedRectVerts[4] = { globalX, globalY + drawHeight, 0, 0, 0, c1 };
	}
	else if (corner == bottomLeft)
	{
		shavedRectVerts[0] = { globalX, globalY, 0, 0, 0, c1 };
		shavedRectVerts[1] = { globalX + drawWidth, globalY, 0, 0, 0, c1 };
		shavedRectVerts[2] = { globalX + drawWidth, globalY + drawHeight, 0, 0, 0, c1 };
		shavedRectVerts[3] = { globalX + fCornerWidth, globalY + drawHeight, 0, 0, 0, c1 };
		shavedRectVerts[4] = { globalX, globalY + drawHeight - fCornerWidth, 0, 0, 0, c1 };
	}
	else if (corner == bottomRight)
	{
		shavedRectVerts[0] = { globalX, globalY, 0, 0, 0, c1 };
		shavedRectVerts[1] = { globalX + drawWidth, globalY, 0, 0, 0, c1 };
		shavedRectVerts[2] = { globalX + drawWidth, globalY + drawHeight - fCornerWidth, 0, 0, 0, c1 };
		shavedRectVerts[3] = { globalX + drawWidth - fCornerWidth, globalY + drawHeight, 0, 0, 0, c1 };
		shavedRectVerts[4] = { globalX, globalY + drawHeight, 0, 0, 0, c1 };
	}

	//Draw them shaved juicy corners (+18)
	if (isFilled)
		al_draw_prim(shavedRectVerts, NULL, NULL, 0, 5, ALLEGRO_PRIM_TRIANGLE_FAN);
	else
		al_draw_prim(shavedRectVerts, NULL, NULL, 0, 5, ALLEGRO_PRIM_LINE_LOOP);
}

void HD_UI_GraphicsObject::DrawStrokedFillShavedRectGfx(float fCornerWidth, ShavedRectCornerLoc corner)
{
	if (fCornerWidth > drawWidth)
		fCornerWidth = drawWidth;
	if (fCornerWidth > drawHeight)
		fCornerWidth = drawHeight;

	ALLEGRO_COLOR c1 = palette->addAlphaToColor(color1, color1.a * drawAlpha);
	ALLEGRO_COLOR c2 = palette->addAlphaToColor(color2, color2.a * drawAlpha);
	ALLEGRO_VERTEX shavedRectVerts[5];

	//Update the verts
	if (corner == topLeft)
	{
		shavedRectVerts[0] = { globalX, globalY + fCornerWidth, 0, 0, 0, c1 };
		shavedRectVerts[1] = { globalX + fCornerWidth, globalY, 0, 0, 0, c1 };
		shavedRectVerts[2] = { globalX + drawWidth, globalY, 0, 0, 0, c1 };
		shavedRectVerts[3] = { globalX + drawWidth, globalY + drawHeight, 0, 0, 0, c1 };
		shavedRectVerts[4] = { globalX, globalY + drawHeight, 0, 0, 0, c1 };
	}
	else if (corner == topRight)
	{
		shavedRectVerts[0] = { globalX, globalY, 0, 0, 0, color1 };
		shavedRectVerts[1] = { globalX + drawWidth - fCornerWidth, globalY, 0, 0, 0, c1 };
		shavedRectVerts[2] = { globalX + drawWidth, globalY + fCornerWidth, 0, 0, 0, c1 };
		shavedRectVerts[3] = { globalX + drawWidth, globalY + drawHeight, 0, 0, 0, c1 };
		shavedRectVerts[4] = { globalX, globalY + drawHeight, 0, 0, 0, c1 };
	}
	else if (corner == bottomLeft)
	{
		shavedRectVerts[0] = { globalX, globalY, 0, 0, 0, c1 };
		shavedRectVerts[1] = { globalX + drawWidth, globalY, 0, 0, 0, c1 };
		shavedRectVerts[2] = { globalX + drawWidth, globalY + drawHeight, 0, 0, 0, c1 };
		shavedRectVerts[3] = { globalX + fCornerWidth, globalY + drawHeight, 0, 0, 0, c1 };
		shavedRectVerts[4] = { globalX, globalY + drawHeight - fCornerWidth, 0, 0, 0, c1 };
	}
	else if (corner == bottomRight)
	{
		shavedRectVerts[0] = { globalX, globalY, 0, 0, 0, c1 };
		shavedRectVerts[1] = { globalX + drawWidth, globalY, 0, 0, 0, c1 };
		shavedRectVerts[2] = { globalX + drawWidth, globalY + drawHeight - fCornerWidth, 0, 0, 0, c1 };
		shavedRectVerts[3] = { globalX + drawWidth - fCornerWidth, globalY + drawHeight, 0, 0, 0, c1 };
		shavedRectVerts[4] = { globalX, globalY + drawHeight, 0, 0, 0, c1 };
	}

	al_draw_prim(shavedRectVerts, NULL, NULL, 0, 5, ALLEGRO_PRIM_TRIANGLE_FAN);

	//stroke: replace color
	//error prone but... meh
	for (unsigned int ii = 0; ii < 5; ii++)
	{
		shavedRectVerts[ii].color = c2;
	}
	al_draw_prim(shavedRectVerts, NULL, NULL, 0, 5, ALLEGRO_PRIM_LINE_LOOP);
}

void HD_UI_GraphicsObject::DrawLineGfx(float fThickness)
{
	ALLEGRO_COLOR c1 = palette->addAlphaToColor(color1, color1.a * drawAlpha);
	
	//A simple Line!
	al_draw_line(globalX, globalY, globalX + drawWidth, globalY + drawHeight, c1, fThickness);
}

void HD_UI_GraphicsObject::DrawGradientRectGFX(bool bIsVertical, float fStartGrad)
{
	ALLEGRO_COLOR c1 = palette->addAlphaToColor(color1, color1.a * drawAlpha);
	ALLEGRO_COLOR c2 = palette->addAlphaToColor(color2, color2.a * drawAlpha);
	ALLEGRO_VERTEX gradientRectVerts[12];	//vertex array

	//Creates a horizontal/vertical gradient filled rectangle primitive
	//with the gradient's start point being fStartGrad% of width/height
	//TO-DO - A case were fStartGrad == 0 or 1
	if (!bIsVertical)
	{
		//1st triangle
		gradientRectVerts[0] = { globalX, globalY, 0, 0, 0, c1 };
		gradientRectVerts[1] = { globalX + drawWidth * fStartGrad, globalY + drawHeight, 0, 0, 0, c1 };
		gradientRectVerts[2] = { globalX, globalY + drawHeight, 0, 0, 0, c1 };
		//2nd triangle
		gradientRectVerts[3] = { globalX, globalY, 0, 0, 0, c1 };
		gradientRectVerts[4] = { globalX + drawWidth * fStartGrad, globalY, 0, 0, 0, c1 };
		gradientRectVerts[5] = { globalX + drawWidth * fStartGrad, globalY + drawHeight, 0, 0, 0, c1 };
		//3rd triangle
		gradientRectVerts[6] = { globalX + drawWidth * fStartGrad, globalY, 0, 0, 0, c1 };
		gradientRectVerts[7] = { globalX + drawWidth, globalY + drawHeight, 0, 0, 0, c2 };
		gradientRectVerts[8] = { globalX + drawWidth * fStartGrad, globalY + drawHeight, 0, 0, 0, c1 };
		//4th triangle
		gradientRectVerts[9] = { globalX + drawWidth * fStartGrad, globalY, 0, 0, 0, c1 };
		gradientRectVerts[10] = { globalX + drawWidth, globalY, 0, 0, 0, c2 };
		gradientRectVerts[11] = { globalX + drawWidth, globalY + drawHeight, 0, 0, 0, c2 };
	}
	else
	{
		//1st triangle
		gradientRectVerts[0] = { globalX, globalY, 0, 0, 0, c1 };
		gradientRectVerts[1] = { globalX + drawWidth, globalY, 0, 0, 0, c1 };
		gradientRectVerts[2] = { globalX + drawWidth, globalY + drawHeight * fStartGrad, 0, 0, 0, c1 };
		//2nd triangle
		gradientRectVerts[3] = { globalX, globalY, 0, 0, 0, c1 };
		gradientRectVerts[4] = { globalX + drawWidth, globalY + drawHeight * fStartGrad, 0, 0, 0, c1 };
		gradientRectVerts[5] = { globalX, globalY + drawHeight * fStartGrad, 0, 0, 0, c1 };
		//3rd triangle
		gradientRectVerts[6] = { globalX, globalY + drawHeight * fStartGrad, 0, 0, 0, c1 };
		gradientRectVerts[7] = { globalX + drawWidth, globalY + drawHeight * fStartGrad, 0, 0, 0, c1 };
		gradientRectVerts[8] = { globalX + drawWidth, globalY + drawHeight, 0, 0, 0, c2 };
		//4th triangle
		gradientRectVerts[9] = { globalX, globalY + drawHeight * fStartGrad, 0, 0, 0, c1 };
		gradientRectVerts[10] = { globalX + drawWidth, globalY + drawHeight, 0, 0, 0, c2 };
		gradientRectVerts[11] = { globalX, globalY + drawHeight, 0, 0, 0, c2 };
	}

	al_draw_prim(gradientRectVerts, NULL, NULL, 0, 12, ALLEGRO_PRIM_TRIANGLE_LIST);
}

void HD_UI_GraphicsObject::DrawGradientBGRectGFX(bool useStroke)
{
	//get the y position in the screen bg gradient
	float pos1 = (globalY - HDScreen->nScreenH * 0.6f) / (HDScreen->nScreenH * 0.4f);
	float pos2 = ((globalY + height) - HDScreen->nScreenH * 0.6f) / (HDScreen->nScreenH * 0.4f);
	ALLEGRO_COLOR c1 = palette->addAlphaToColor( palette->getColorFromRange(palette->cBg1, palette->cBg2, pos1), drawAlpha );
	ALLEGRO_COLOR c2 = palette->addAlphaToColor( palette->getColorFromRange(palette->cBg1, palette->cBg2, pos2), drawAlpha );
	ALLEGRO_COLOR c3 = palette->addAlphaToColor(color1, color1.a * drawAlpha);

	ALLEGRO_VERTEX gradientRectVerts[6];

	//Vertexes::
	//1st triangle
	gradientRectVerts[0] = { globalX, globalY, 0, 0, 0, c1 };
	gradientRectVerts[1] = { globalX + drawWidth, globalY, 0, 0, 0, c1 };
	gradientRectVerts[2] = { globalX + drawWidth, globalY + drawHeight, 0, 0, 0, c2 };
	//2nd triangle
	gradientRectVerts[3] = { globalX, globalY, 0, 0, 0, c1 };
	gradientRectVerts[4] = { globalX + drawWidth, globalY + drawHeight, 0, 0, 0, c2 };
	gradientRectVerts[5] = { globalX, globalY + drawHeight, 0, 0, 0, c2 };

	al_draw_prim(gradientRectVerts, NULL, NULL, 0, 6, ALLEGRO_PRIM_TRIANGLE_LIST);

	if (useStroke)
	{
		al_draw_rectangle(globalX + 0.5f, globalY + 0.5f, globalX + drawWidth - 0.5f, globalY + drawHeight - 0.5f, c3, 1.0f); //0.5f added for pixel perfectness
	}
}

//============================
// Public Creation Functions
//============================

//Image Object
void HD_UI_GraphicsObject::CreateImageObject(const char *objectName, const char *imageName, const char *atlasName,
	float fX, float fY)
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

	setObjectProperties(objectName, fX, fY, imageWidth, imageHeight);

	//Set the draw function
	drawGraphicObject = std::bind(&HD_UI_GraphicsObject::DrawImageGfx, this);
}

//Stroked/Filled Rect object
void HD_UI_GraphicsObject::CreateRectangleObject(const char *objectName, float fX, float fY, float fWidth, float fHeight,
	float fThickness, ALLEGRO_COLOR color)
{
	if (fThickness > 0.0f)
	{
		fX += fThickness / 2.0f;		//allegro has float coordinates and draws strokes inside & outside of the
		fY += fThickness / 2.0f;		//generated line; this makes sure the lines will be pixel perfect
	}

	setObjectProperties(objectName, fX, fY, fWidth, fHeight);

	color1 = color;

	drawGraphicObject = std::bind(&HD_UI_GraphicsObject::DrawRectGfx, this, fThickness);
}

//Stroked/Filled Diamond object
void HD_UI_GraphicsObject::CreateDiamondObject(const char *objectName, float fX, float fY, float fWidth, float fHeight,
	bool isFilled, ALLEGRO_COLOR color)
{
	setObjectProperties(objectName, fX, fY, fWidth, fHeight);

	color1 = color;

	drawGraphicObject = std::bind(&HD_UI_GraphicsObject::DrawDiamondGfx, this, isFilled);
}

//Filled & Stroked Rect object
void HD_UI_GraphicsObject::CreateSFRectangleObject(const char *objectName, float fX, float fY, float fWidth, float fHeight,
	float fThickness, ALLEGRO_COLOR fillColor, ALLEGRO_COLOR lineColor)
{
	fX += fThickness / 2.0f;
	fY += fThickness / 2.0f;

	setObjectProperties(objectName, fX, fY, fWidth, fHeight);

	color1 = fillColor;
	color2 = lineColor;

	drawGraphicObject = std::bind(&HD_UI_GraphicsObject::DrawStrokedFillRectGFX, this, fThickness);
}

//Shaved Stroked/Filled Rect object
void HD_UI_GraphicsObject::CreateShavedRectangleObject(const char *objectName, float fX, float fY, float fWidth, float fHeight,
	bool isFilled, ShavedRectCornerLoc corner, float fCornerWidth, ALLEGRO_COLOR color)
{
	if (!isFilled)
	{
		fX += 0.5f;
		fY += 0.5f;
	}

	setObjectProperties(objectName, fX, fY, fWidth, fHeight);

	color1 = color;

	drawGraphicObject = std::bind(&HD_UI_GraphicsObject::DrawShavedRectGfx, this, isFilled, fCornerWidth, corner);
}

//Shaved Rectangle Stroked & Filled object
void HD_UI_GraphicsObject::CreateShavedSFRectangleObject(const char *objectName, float fX, float fY, float fWidth, float fHeight,
	ShavedRectCornerLoc corner, float fCornerWidth, ALLEGRO_COLOR fillColor, ALLEGRO_COLOR strokeColor)
{
	fX += 0.5f;
	fY += 0.5f;

	setObjectProperties(objectName, fX, fY, fWidth, fHeight);

	color1 = fillColor;
	color2 = strokeColor;

	drawGraphicObject = std::bind(&HD_UI_GraphicsObject::DrawStrokedFillShavedRectGfx, this, fCornerWidth, corner);
}

//Line object
void HD_UI_GraphicsObject::CreateLineObject(const char *objectName, float fX, float fY, float fX2, float fY2,
	float fThickness, ALLEGRO_COLOR color)
{
	setObjectProperties(objectName, fX, fY, fX2, fY2);

	color1 = color;

	drawGraphicObject = std::bind(&HD_UI_GraphicsObject::DrawLineGfx, this, fThickness);
}

//Gradient Rect object
void HD_UI_GraphicsObject::CreateGradientRectangleObject(const char *objectName, float fX, float fY, float fWidth, float fHeight,
	bool bIsVertical, float fStartGrad, ALLEGRO_COLOR color01, ALLEGRO_COLOR color02)
{
	setObjectProperties(objectName, fX, fY, fWidth, fHeight);

	color1 = color01;
	color2 = color02;

	drawGraphicObject = std::bind(&HD_UI_GraphicsObject::DrawGradientRectGFX, this, bIsVertical, fStartGrad);
}

//Gradient Background Rect object - the colors get updated by the Y position of the object
void HD_UI_GraphicsObject::CreateBGGradientRectangleObject(const char *objectName, float fX, float fY, float fWidth, float fHeight,
	bool useStroke, ALLEGRO_COLOR strokeColor)
{
	setObjectProperties(objectName, fX, fY, fWidth, fHeight);

	color1 = strokeColor;

	drawGraphicObject = std::bind(&HD_UI_GraphicsObject::DrawGradientBGRectGFX, this, useStroke);
}

//============================
// General Functions
//============================

void HD_UI_GraphicsObject::Draw()
{
	if (isVisible && redraw && isInClippingMask())
	{
		HD_UI_Object::Draw();
		drawGraphicObject();
		redraw = false;
	}
}

void HD_UI_GraphicsObject::Clear()
{
	HD_UI_Object::Clear();
}