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
	ALLEGRO_BITMAP *gfxImage = NULL;							//the image that will get drawn in DrawImageGfx();
	ALLEGRO_COLOR color1 = al_map_rgba(255, 0, 255, 255);		//colors used for primitives
	ALLEGRO_COLOR color2 = al_map_rgba(255, 0, 255, 255);

	HD_UI_GraphicsObject() { }			//don't want this to get created without parameters!

protected:
	//Graphics drawing functions
	std::function<void()>drawFunction;
	
	void DrawImageGfx();
	
	void DrawStrokedRectGfx(int nThickness);
	
	void DrawFillRectGfx();
	
	void DrawShavedStrokedRectGfx(float nThickness, ShavedRectCornerLoc corner, float nCornerWidth);
	
	void DrawShavedFillRectGfx(ShavedRectCornerLoc corner, float nCornerWidth);
	
	void DrawLineGfx(float nThickness);
	
	void DrawFillRectLineHeaderFooterGfx(bool bIsLineHeader, float nLineThickness);
	
	void DrawGradientRectGFX(bool bIsVertical, float gradMidPoint);

public:
	// Creation functions

	//Image Object
	HD_UI_GraphicsObject(char *objectName, int nIndex, char *imageName, char *atlasName,
		float nX, float nY, float nWidth, float nHeight, HD_UI_Layout *newParent);

	//Stroked Rect object
	HD_UI_GraphicsObject(char *objectName, int nIndex, float nX, float nY, float nWidth, float nHeight,
		int nThickness, ALLEGRO_COLOR color, HD_UI_Layout *newParent);

	//Filled Rect object
	HD_UI_GraphicsObject(char *objectName, int nIndex, float nX, float nY, float nWidth, float nHeight,
		ALLEGRO_COLOR color, HD_UI_Layout *newParent);

	//Shaved Stroked Rect object
	HD_UI_GraphicsObject(char *objectName, int nIndex, float nX, float nY, float nWidth, float nHeight,
		float nThickness, ShavedRectCornerLoc corner, float nCornerWidth, ALLEGRO_COLOR color, HD_UI_Layout *newParent);

	//Shaved Filled Rect object
	HD_UI_GraphicsObject(char *objectName, int nIndex, float nX, float nY, float nWidth, float nHeight,
		ShavedRectCornerLoc corner, float nCornerWidth, ALLEGRO_COLOR color, HD_UI_Layout *newParent);
	
	//Line object
	HD_UI_GraphicsObject(char *objectName, int nIndex, float nX, float nY, float nX2, float nY2,
		float nThickness, ALLEGRO_COLOR color, HD_UI_Layout *newParent);
	
	//Filled Rect with Line Header/Footer object
	HD_UI_GraphicsObject(char *objectName, int nIndex, float nX, float nY, float nWidth, float nHeight,
		ALLEGRO_COLOR rectColor, bool bIsLineHeader, float nLineThickness, ALLEGRO_COLOR lineColor, HD_UI_Layout *newParent);
	
	//Gradient Rect object
	HD_UI_GraphicsObject(char *objectName, int nIndex, float nX, float nY, float nWidth, float nHeight,
		bool bIsVertical, float gradMidPoint, ALLEGRO_COLOR color1, ALLEGRO_COLOR color2, HD_UI_Layout *newParent);

	//destruction
	~HD_UI_GraphicsObject();


	//general functions
	void virtual Update();
	void virtual Draw();
	void virtual Clear();

};

#endif