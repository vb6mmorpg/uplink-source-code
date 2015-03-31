#include "HD_UI_Container.h"

HD_UI_Container::HD_UI_Container()
{

}

HD_UI_Container::~HD_UI_Container()
{

}

//============================
// Public Functions
//============================

void HD_UI_Container::Create()
{
	//To be replaced by derived objects
}

void HD_UI_Container::Update(ALLEGRO_MOUSE_STATE *mouseState, double timeSpeed)
{
	HD_UI_Object::Update(mouseState, timeSpeed);

	//base update function
	for (unsigned int ii = 0; ii < children.size(); ii++)
	{
		children[ii]->Update(mouseState, timeSpeed);
	}
}

void HD_UI_Container::Draw()
{
	//base draw function
	for (unsigned int ii = 0; ii < children.size(); ii++)
	{
		children[ii]->Draw();
	}
}

void HD_UI_Container::Clear()
{
	//base clear function
	removeAllChildren();

	delete this;
}

void HD_UI_Container::addChild(HD_UI_Object *child)
{
	children.push_back(child);

	//child->setParent(this);
	child->index = children.size() - 1;
}

void HD_UI_Container::addChildAt(HD_UI_Object *child, unsigned index)
{
	//add the child directly at index
	std::vector<HD_UI_Object*>::iterator it = children.begin();
	children.insert(it + index, child);

	//child->setParent(this);
	child->index = index;
}

//Child removal!
void HD_UI_Container::removeChild(HD_UI_Object *child)
{
	removeChildFromIndex(child->index);
}

void HD_UI_Container::removeChildByName(char *childName)
{
	//Search for the child with name "childName", then erase it
	std::vector<HD_UI_Object*>::iterator it;
	for (unsigned ii = 0; ii < children.size(); ii++)
	{
		if (strcmp(children[ii]->name, childName))
		{
			children.erase(it + ii);
			break;
		}
	}
}

void HD_UI_Container::removeChildFromIndex(int index)
{
	//Remove child at index
	std::vector<HD_UI_Object*>::iterator it = children.begin();
	children.erase(it + index);
}

void HD_UI_Container::removeAllChildren()
{
	//Remove all child objects
	//Only called when the layout is destroyed
	for (unsigned ii = 0; ii < children.size(); ii++)
	{
		children[ii]->Clear();
	}

	children.clear();
}