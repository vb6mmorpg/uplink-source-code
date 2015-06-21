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

protected:
	//Graphics drawing functions
	std::function<void()>drawGraphicObject;
	
	void DrawImageGfx();
	
	void DrawRectGfx(float fThickness);

	void DrawDiamondGfx(bool isFilled);

	void DrawStrokedFillRectGFX(float fThickness);
	
	void DrawShavedRectGfx(bool isFilled, float fCornerWidth, ShavedRectCornerLoc corner);

	void DrawStrokedFillShavedRectGfx(float fCornerWidth, ShavedRectCornerLoc corner);
	
	void DrawLineGfx(float fThickness);
	
	void DrawGradientRectGFX(bool bIsVertical, float fStartGrad);

	void DrawGradientBGRectGFX(bool useStroke);

public:
	//Construction/Destruction
	HD_UI_GraphicsObject() {}
	~HD_UI_GraphicsObject() {}

	// Creation functions
	//Image Object
	void CreateImageObject(const char *objectName, const char *imageName, const char *atlasName,
		float fX, float fY);

	//Rectangle Stroked/Filled object
	void CreateRectangleObject(const char *objectName, float fX, float fY, float fWidth, float fHeight,
		float fThickness, ALLEGRO_COLOR color);

	//Diamond Stroked/Filled object
	void CreateDiamondObject(const char *objectName, float fX, float fY, float fWidth, float fHeight,
		bool isFilled, ALLEGRO_COLOR color);

	//Rectangle Filled & Stroked object
	void CreateSFRectangleObject(const char *objectName, float fX, float fY, float fWidth, float fHeight,
		float fThickness, ALLEGRO_COLOR fillColor, ALLEGRO_COLOR lineColor);

	//Shaved Rectangle Stroked/Filled object
	void CreateShavedRectangleObject(const char *objectName, float fX, float fY, float fWidth, float fHeight,
		bool isFilled, ShavedRectCornerLoc corner, float fCornerWidth, ALLEGRO_COLOR color);

	//Shaved Rectangle Stroked & Filled object
	void CreateShavedSFRectangleObject(const char *objectName, float fX, float fY, float fWidth, float fHeight,
		ShavedRectCornerLoc corner, float fCornerWidth, ALLEGRO_COLOR fillColor, ALLEGRO_COLOR strokeColor);
	
	//Line object
	void CreateLineObject(const char *objectName, float fX, float fY, float fX2, float fY2,
		float fThickness, ALLEGRO_COLOR color);
	
	//Gradient Rect object
	void CreateGradientRectangleObject(const char *objectName, float fX, float fY, float fWidth, float fHeight,
		bool bIsVertical, float fStartGrad, ALLEGRO_COLOR color01, ALLEGRO_COLOR color02);

	//Gradient Background Rect object - the colors get updated by the Y position of the object
	void CreateBGGradientRectangleObject(const char *objectName, float fX, float fY, float fWidth, float fHeight,
		bool useStroke, ALLEGRO_COLOR strokeColor);

	//Property setters
	void setGfxImage(ALLEGRO_BITMAP *newImage) { gfxImage = newImage; }
	void setColors(ALLEGRO_COLOR newColor1, ALLEGRO_COLOR newColor2 = al_map_rgb_f(1.0f, 0, 1.0f))
		{ color1 = newColor1; color2 = newColor2; }

	//General object functions
	void Draw();
	void Clear();

};

#endif