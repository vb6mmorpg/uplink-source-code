#include "HD_UI_Object.h"
#include "../UI_Layouts/HD_UI_Layout.h"

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

}

//functions
void HD_UI_Object::Update()
{

}

void HD_UI_Object::Draw()
{

}

void HD_UI_Object::Clear()
{
	delete this;
}

//parent functions
HD_UI_Layout* HD_UI_Object::getParent()
{
	return parent;
}

void HD_UI_Object::setParent(HD_UI_Layout *newParent, int index)
{
	parent = newParent;
	
	if (index < 0)
		parent->addChild(this);
	else
		parent->addChildAt(this, index);
}