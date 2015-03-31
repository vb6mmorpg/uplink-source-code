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
void HD_UI_Object::Update(ALLEGRO_MOUSE_STATE *mouseState, double timeSpeed)
{
	//animations update
	tweensContainer.step(timeSpeed);

	//Update according to parent
	if (parent != NULL)
	{
		if (parent->x != lastParentX)
			x = x + (parent->x - lastParentX);
		if (parent->y != lastParentY)
			y = y + (parent->y - lastParentY);

		lastParentX = parent->x;
		lastParentY = parent->y;

		//local coordinates update
		if (_x != last_x)
			x = _x + parent->x;
		if (_y != last_y)
			y = _y + parent->y;

		last_x = _x;
		last_y = _y;

		//scaleX = parent->scaleX;
		//scaleY = parent->scaleY;

		width = createW * scaleX;
		height = createH * scaleY;
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

void HD_UI_Object::setObjectProperties(char *objectName, float fX, float fY, float fWidth, float fHeight, HD_UI_Container *newParent, int nIndex)
{
	setParent(newParent, nIndex);

	name = objectName;

	//To-DO: these values get scaled and are relative to the parent!
	lastParentX = parent->x;
	lastParentY = parent->y;
	x = fX + lastParentX;
	y = fY + lastParentY;

	//local
	_x = fX;
	_y = fY;
	last_x = _x;
	last_y = _y;

	createW = width = fWidth;
	createH = height = fHeight;

	scaleX = 1.0f;
	scaleY = 1.0f;
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