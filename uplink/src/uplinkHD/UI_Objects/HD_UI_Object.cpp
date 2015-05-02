#include <allegro5/allegro_primitives.h>

#include "HD_UI_Object.h"
#include "HD_UI_Container.h"
#include "../HD_Screen.h"

//constructor/destructor
HD_UI_Object::HD_UI_Object()
{
	x = 0;
	y = 0;
	width = 0;
	height = 0;
	index = 0;
	name = NULL;
}
HD_UI_Object::~HD_UI_Object()
{
	parent = NULL;
}

//functions
void HD_UI_Object::Update()
{
	if (parent)
		isVisible = parent->isVisible && visible;
	else
		isVisible = visible;

	if (parent != NULL && isVisible)
	{
		//Child object

		//animations update
		tweensContainer.step(HDScreen->deltaTime);

		//Update according to parent
		globalX = parent->globalX + x;
		globalY = parent->globalY + y;

		globalScaleX = parent->globalScaleX * scaleX;
		globalScaleY = parent->globalScaleY * scaleY;

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
	//draw error rect
	al_draw_filled_rectangle(x, y, x + width, y + height, al_map_rgb(255, 0, 255));
}

void HD_UI_Object::Clear()
{
	delete this;
}

void HD_UI_Object::setObjectProperties(const char *objectName, float fX, float fY, float fWidth, float fHeight, HD_UI_Container *newParent, int nIndex)
{
	setParent(newParent, nIndex);

	name = objectName;

	x = fX;
	y = fY;
	globalX = parent->globalX + x;
	globalY = parent->globalY + y;

	drawWidth = width = fWidth;
	drawHeight = height = fHeight;

	scaleX = 1.0f;
	scaleY = 1.0f;

	alpha = 1.0f;
}

//parent functions
void HD_UI_Object::setParent(HD_UI_Container *newParent, int index)
{
	//If the parent is NULL, the object's
	//gonna get assigned to the currentLayout
	if (newParent)
		parent = newParent;
	else
		parent = HDScreen->hd_getCurrentLayout();
	
	//if index < 0, the child is added at the back of the layout's child list
	//else it gets added at the specified index
	if (index < 0)
		parent->addChild(this);
	else
		parent->addChildAt(this, index);
}

//animations
//Adds a CTween animation; if removeAnims, it stops all other animations on this object
void HD_UI_Object::addAnimation(CDBTweener::CTween *newTween, bool removeAnims)
{
	if (removeAnims)
		tweensContainer.removeAllTweens();

	tweensContainer.addTween(newTween);
}