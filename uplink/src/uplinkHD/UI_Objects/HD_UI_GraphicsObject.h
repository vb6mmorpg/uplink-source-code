//========================================
// The GFX Object class.
// Used for creating basic graphics that
// don't require player input.
// Contains pretty much all GFX drawing
// functions.
//========================================

#ifndef HD_UI_GRAPHICSOBJECT_H
#define HD_UI_GRAPHICSOBJECT_H

#include <functional>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "HD_UI_Object.h"

enum ShavedRectCornerLoc
{
	topLeft,
	topRight,
	bottomLeft,
	bottomRight
};

class HD_UI_GraphicsObject : public HD_UI_Object
{
private:
	ALLEGRO_BITMAP *gfxImage = NULL;								//the image that will get drawn in DrawImageGfx();
	ALLEGRO_COLOR color1 = al_map_rgba_f(1.0f, 0.0f, 1.0f, 1.0f);	//colors used for primitives
	ALLEGRO_COLOR color2 = al_map_rgba_f(1.0f, 0.0f, 1.0f, 1.0f);

	//ALLEGRO_VERTEX shavedRectVerts[5];		//vertex array used by shaved rectangles
	//ALLEGRO_VERTEX gradientRectVerts[12];	//vertex array used by gradient rectangles

protected:
	//Graphics drawing functions
	std::function<void()>drawGraphicObject;
	
	void DrawImageGfx();
	
	void DrawRectGfx(float fThickness);

	void DrawDiamondGfx(bool isFilled);

	void DrawStrokedFillRectGFX(float fThickness);
	
	void DrawShavedRectGfx(bool isFilled, float fCornerWidth, ShavedRectCornerLoc corner);
	
	void DrawLineGfx(int nThickness);
	
	void DrawFillRectLineHeaderFooterGfx(bool bIsLineHeader, float fLineThickness);
	
	void DrawGradientRectGFX(bool bIsVertical, float fStartGrad);

public:
	// Creation functions
	//Construction
	HD_UI_GraphicsObject();

	//Image Object
	HD_UI_GraphicsObject(const char *objectName, int index, const char *imageName, const char *atlasName,
		float fX, float fY, HD_UI_Container *newParent);

	//Stroked/Filled Rect object
	HD_UI_GraphicsObject(const char *objectName, int index, float fX, float fY, float fWidth, float fHeight,
		float fThickness, ALLEGRO_COLOR color, HD_UI_Container *newParent);

	//Stroked/Filled Diamond object
	HD_UI_GraphicsObject(const char *objectName, int index, float fX, float fY, float fWidth, float fHeight,
		bool isFilled, ALLEGRO_COLOR color, HD_UI_Container *newParent);

	//Filled & Stroked Rect object
	HD_UI_GraphicsObject(const char *objectName, int index, float fX, float fY, float fWidth, float fHeight,
		float fThickness, ALLEGRO_COLOR fillColor, ALLEGRO_COLOR lineColor, HD_UI_Container *newParent);

	//Shaved Stroked/Filled Rect object
	HD_UI_GraphicsObject(const char *objectName, int index, float fX, float fY, float fWidth, float fHeight,
		bool isFilled, ShavedRectCornerLoc corner, float fCornerWidth, ALLEGRO_COLOR color, HD_UI_Container *newParent);
	
	//Line object
	HD_UI_GraphicsObject(const char *objectName, int index, float fX, float fY, float fX2, float fY2,
		int nThickness, ALLEGRO_COLOR color, HD_UI_Container *newParent);
	
	//Filled Rect with Line Header/Footer object
	HD_UI_GraphicsObject(const char *objectName, int index, float fX, float fY, float fWidth, float fHeight,
		ALLEGRO_COLOR rectColor, bool bIsLineHeader, float fLineThickness, ALLEGRO_COLOR lineColor, HD_UI_Container *newParent);
	
	//Gradient Rect object
	HD_UI_GraphicsObject(const char *objectName, int index, float fX, float fY, float fWidth, float fHeight,
		bool bIsVertical, float fStartGrad, ALLEGRO_COLOR color01, ALLEGRO_COLOR color02, HD_UI_Container *newParent);

	//Destruction
	~HD_UI_GraphicsObject() { al_destroy_bitmap(gfxImage); }

	//Property setters
	void setGfxImage(ALLEGRO_BITMAP *newImage) { gfxImage = newImage; }
	void setColors(ALLEGRO_COLOR newColor1, ALLEGRO_COLOR newColor2 = al_map_rgb_f(1.0f, 0, 1.0f))
		{ color1 = newColor1; color2 = newColor2; }

	//General object functions
	void Draw();
	void Clear();

};

#endif