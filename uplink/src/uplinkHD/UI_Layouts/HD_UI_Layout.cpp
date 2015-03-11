// Implementation of the base UI Layout Class
//===========================================

#include "HD_UI_Layout.h"
#include "../UI_Objects/HD_UI_Object.h"

HD_UI_Layout::HD_UI_Layout()
{

}

HD_UI_Layout::~HD_UI_Layout()
{

}

void HD_UI_Layout::Create()
{

}

void HD_UI_Layout::Update()
{
	for (unsigned ii = 0; ii < children.size(); ii++)
	{
		children[ii].Update();
	}
}

void HD_UI_Layout::Draw()
{
	for (unsigned ii = 0; ii < children.size(); ii++)
	{
		children[ii].Draw();
	}
}

void HD_UI_Layout::Clear()
{
	removeAllChildren();

	delete this;
}

//Child objects adding and removing
void HD_UI_Layout::addChild(HD_UI_Object *child)
{
	children.push_back(*child);

	//child->setParent(this);
	child->index = children.size() - 1;
}

void HD_UI_Layout::addChildAt(HD_UI_Object *child, unsigned index)
{
	//add the child directly at index
	std::vector<HD_UI_Object>::iterator it = children.begin();
	children.insert(it + index, *child);

	//child->setParent(this);
	child->index = index;
}

void HD_UI_Layout::removeChild(HD_UI_Object *child)
{
	removeChildFromIndex(child->index);
}

void HD_UI_Layout::removeChildByName(char *childName)
{
	//Search for the child with name "childName", then erase it
	std::vector<HD_UI_Object>::iterator it;
	for (unsigned ii = 0; ii < children.size(); ii++)
	{
		if (strcmp(children[ii].name, childName))
		{
			children.erase(it + ii);
			break;
		}
	}
}

void HD_UI_Layout::removeChildFromIndex(int index)
{
	//Remove child at index
	std::vector<HD_UI_Object>::iterator it = children.begin();
	children.erase(it + index);
}

void HD_UI_Layout::removeAllChildren()
{
	//Remove all child objects
	//Only called when the layout is destroyed
	for (unsigned ii = 0; ii < children.size(); ii++)
	{
		children[ii].Clear();
	}

	children.clear();
}