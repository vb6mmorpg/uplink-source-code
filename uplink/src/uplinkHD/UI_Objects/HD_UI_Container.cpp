#include "HD_UI_Container.h"
#include "../HD_Screen.h"
#include "../UI_Layouts/HD_Root.h"

#include "HD_UI_Button.h"
#include "HD_UI_ButtonInput.h"
#include "HD_UI_ButtonMenu.h"
#include "HD_UI_PopUp.h"
#include "HD_UI_TextObject.h"

//============================
// Public Functions
//============================

HD_UI_Container::HD_UI_Container()
{
	//Empty container
	setObjectProperties("emptyContainer", 0.0f, 0.0f, 0.0f, 0.0f);
}

HD_UI_Container::HD_UI_Container(const char* name, float fX, float fY, float fWidth, float fHeight)
{
	setObjectProperties(name, fX, fY, fWidth, fHeight);
}

void HD_UI_Container::Create()
{
	//To be replaced by derived layouts
}

void HD_UI_Container::Update()
{
	HD_UI_Object::Update();

	//get a reference to the parent's clipping mask if this object has none
	if (!hasClippingMask && !clippingMask && parent)
		AddClippingMask(parent->GetClippingMask(), false);

	//setMouseOver();

	bool isPrimaryDown = (HDScreen->mouse->GetState()->buttons & 1);
	bool isPrimaryReleased = !isPrimaryDown;

	if (isPrimaryDown && isMouseTarget())
		HDScreen->mouse->SetFocused(true);
	else if (isPrimaryReleased && isMouseTarget())
		HDScreen->mouse->SetFocused(false);

	//update children
	for (unsigned int ii = 0; ii < children.size(); ii++)
	{
		children[ii]->Update();
	}
}

void HD_UI_Container::Draw()
{
	if (!isVisible) return;

	if (hasClippingMask)
		setClippingMask();

	//doing this on the draw loop since it needs to know the clipping mask
	setMouseOver();

	for (unsigned int ii = 0; ii < children.size(); ii++)
		children[ii]->Draw();

	if (hasClippingMask)
		resetClippingMask();

	if (HDScreen->DebugInfo)
		DrawDebugInfo(al_map_rgb_f(0.0f, 1.0f, 0.0f));
	
}

void HD_UI_Container::Clear()
{
	//base clear function
	removeAllChildren();
}

//Mouse functions
//===============
bool HD_UI_Container::setMouseOver()
{
	int mX = HDScreen->mouse->GetState()->x;
	int mY = HDScreen->mouse->GetState()->y;

	float bX = globalX;
	float bY = globalY;
	float bW = globalX + drawWidth;
	float bH = globalY + drawHeight;
	
	// modify the bounding box if there's a clipping mask
	if (clippingMask)
	{
		int maskX, maskY, maskW, maskH = 0;
		al_get_clipping_rectangle(&maskX, &maskY, &maskW, &maskH);

		bX = globalX > maskX ? globalX : maskX;
		bY = globalY > maskY ? globalY : maskY;
		//the overlay between this and the mask
		float oW = max(0, min(globalX + drawWidth, maskX + maskW) - max(globalX, maskX));
		float oH = max(0, min(globalY + drawHeight, maskY + maskH) - max(globalY, maskY));
		bW = bX + oW;
		bH = bY + oH;
	}

	if (!isVisible)
	{
		if (HDScreen->mouse->GetTarget() == shared_from_this())
			HDScreen->mouse->SetTarget(nullptr);
		return false;
	}

	if (mX > bX && mX < bW &&
		mY > bY && mY < bH)
	{
		//the mouse is over this
		//set its target to this
		//if it succeeds, we have contact!
		if (HDScreen->mouse->SetTarget(shared_from_this()))
			return true;
		else
			return false;
	}
	else
	{
		//the mouse is out
		//reset the target if it has been set by this
		if (HDScreen->mouse->GetTarget() == shared_from_this())
			HDScreen->mouse->SetTarget(nullptr);
		return false;
	}
}

bool HD_UI_Container::isMouseTarget()
{
	std::shared_ptr<HD_UI_Container> mouseTarget = HDScreen->mouse->GetTarget();

	if (mouseTarget == shared_from_this())
		return true;
	else
		return false;
}

bool HD_UI_Container::hasMouseFocus()
{
	return isMouseTarget() && HDScreen->mouse->IsFocused();
}

bool HD_UI_Container::rootIsMouseTarget()
{
	std::shared_ptr<HD_UI_Container> mouseTarget = HDScreen->mouse->GetTarget();

	if (mouseTarget == root->GetLayout())
		return true;
	else
		return false;
}

//Clipping Mask
//===============
void HD_UI_Container::AddClippingMask(std::shared_ptr<HD_UI_Object> newMask, bool takeOwnership)
{
	//adds a clipping mask to this container
	//if takeOwnership it means that this object will control it
	//if not, it means it's being controlled by a parent and we only hold a reference to it

	if (takeOwnership)
	{
		clippingMask = newMask;
		addChild(clippingMask);
		hasClippingMask = true;
	}
	else
	{
		clippingMask = newMask;
	}
}

std::shared_ptr<HD_UI_Object> HD_UI_Container::GetClippingMask()
{
	return clippingMask;
}

bool HD_UI_Container::HasClippingMask()
{
	return hasClippingMask;
}

void HD_UI_Container::setClippingMask()
{
	int mX, mY, mW, mH = 0;
	al_get_clipping_rectangle(&mX, &mY, &mW, &mH);

	//composed clipping mask
	int compX, compY, compW, compH = 0;
	compX = mX > clippingMask->GlobalX() ? mX : clippingMask->GlobalX();
	compY = mY > clippingMask->GlobalY() ? mY : clippingMask->GlobalY();
	compW = max(0, min(mX + mW, clippingMask->GlobalX() + clippingMask->width) - max(mX, clippingMask->GlobalX()));
	compH = max(0, min(mY + mH, clippingMask->GlobalY() + clippingMask->height) - max(mY, clippingMask->GlobalY()));

	al_set_clipping_rectangle(compX, compY, compW, compH);
}

void HD_UI_Container::resetClippingMask()
{
	//reset it to its previous state
//	if (!parent->HasClippingMask())
//		al_reset_clipping_rectangle();
	if (parent)
	{
		int parentMX, parentMY, parentMW, parentMH = 0;
		parentMX = parent->GetClippingMask()->GlobalX();
		parentMY = parent->GetClippingMask()->GlobalY();
		parentMW = parent->GetClippingMask()->width;
		parentMH = parent->GetClippingMask()->height;
		al_set_clipping_rectangle(parentMX, parentMY, parentMW, parentMH);
	}
	else
	{
		al_reset_clipping_rectangle();
	}
}

void HD_UI_Container::RemoveClippingMask()
{
	removeChildFromIndex(clippingMask->index);
	clippingMask = nullptr;
	hasClippingMask = false;

	al_reset_clipping_rectangle();
}

//Child functions
//===============
void HD_UI_Container::addChild(std::shared_ptr<HD_UI_Object> child)
{
	children.push_back(child);

	//Set the index
	children[children.size() - 1]->index = children.size() - 1;
	children[children.size() - 1]->gIndex = children[children.size() - 1]->index + gIndex;
	children[children.size() - 1]->setParent(shared_from_this());
}

void HD_UI_Container::addChildAt(std::shared_ptr<HD_UI_Object> child, unsigned index)
{
	//add the child directly at index
	std::vector<std::shared_ptr<HD_UI_Object>>::iterator it = children.begin();
	children.insert(it + index, child);

	children[index]->index = index;
	children[index]->gIndex = children[index]->index + gIndex;
	children[index]->setParent(shared_from_this());

	//update the rest of the children's index
	for (unsigned int ii = index + 1; ii < children.size(); ii++)
	{
		children[ii]->index = ii;
		children[ii]->gIndex = children[ii]->index + gIndex;
	}
}

//Child retreaval!
std::shared_ptr<HD_UI_Object> HD_UI_Container::getChildByIndex(unsigned int index)
{
	if (children.size() < index + 1)
		return nullptr;
	else
		return children[index];
}

std::shared_ptr<HD_UI_Object> HD_UI_Container::getChildByName(const char *name)
{
	bool found = false;
	unsigned int index = 0;

	for (unsigned ii = 0; ii < children.size(); ii++)
	{
		if (strcmp(children[ii]->name.c_str(), name) == 0)
		{
			found = true;
			index = ii;
			break;
		}
	}

	if (found)
		return children[index];
	else
		return nullptr;
}

std::shared_ptr<HD_UI_Container> HD_UI_Container::getContainerByIndex(unsigned int index)
{
	return std::static_pointer_cast<HD_UI_Container>(getChildByIndex(index));
}

std::shared_ptr<HD_UI_Container> HD_UI_Container::getContainerByName(const char *name)
{
	return std::static_pointer_cast<HD_UI_Container>(getChildByName(name));
}

std::shared_ptr<HD_UI_Button> HD_UI_Container::getButtonByName(const char *name)
{
	return std::static_pointer_cast<HD_UI_Button>(getChildByName(name));
}

std::shared_ptr<HD_UI_ButtonInput> HD_UI_Container::getButtonInputByName(const char *name)
{
	return std::static_pointer_cast<HD_UI_ButtonInput>(getChildByName(name));
}

std::shared_ptr<HD_UI_ButtonMenu> HD_UI_Container::getButtonMenuByName(const char *name)
{
	return std::static_pointer_cast<HD_UI_ButtonMenu>(getChildByName(name));
}

std::shared_ptr<HD_UI_PopUp> HD_UI_Container::getPopUpByName(const char *name)
{
	return std::static_pointer_cast<HD_UI_PopUp>(getChildByName(name));
}

std::shared_ptr<HD_UI_TextObject> HD_UI_Container::getTextObjectByName(const char *name)
{
	return std::static_pointer_cast<HD_UI_TextObject>(getChildByName(name));
}

//Child removal!
void HD_UI_Container::removeChild(std::shared_ptr<HD_UI_Object> child)
{
	removeChildFromIndex(child->index);
}

void HD_UI_Container::removeChildByName(char *childName)
{
	//Search for the child with name "childName", then erase it
	for (unsigned ii = 0; ii < children.size(); ii++)
	{
		if (strcmp(children[ii]->name.c_str(), childName) == 0)
		{
			removeChildFromIndex(ii);
			break;
		}
	}
}

void HD_UI_Container::removeChildFromIndex(unsigned int index)
{
	//Remove child at index
	std::vector<std::shared_ptr<HD_UI_Object>>::iterator it = children.begin();
	children[index]->removeAnimations();
	children[index]->Clear();
	children[index].reset();
	children.erase(it + index);

	//update the rest of the children's index
	for (unsigned int ii = index; ii < children.size(); ii++)
	{
		children[ii]->index = ii;
		children[ii]->gIndex = children[ii]->index + gIndex;
	}
}

void HD_UI_Container::removeAllChildren()
{
	//Remove all child objects
	//Only called when the layout is destroyed
	unsigned int size = children.size();
	for (unsigned int ii = size; ii > 0; ii--)
	{
		removeChildFromIndex(ii - 1);
	}
}