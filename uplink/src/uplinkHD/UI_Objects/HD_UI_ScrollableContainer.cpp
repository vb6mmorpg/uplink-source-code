//Implementation of the Scrollable Container
//==========================================

#include "HD_UI_ScrollableContainer.h"

// Private
//==========
void HD_UI_ScrollableContainer::repositionElements(bool allElements, bool useAnimations)
{
	unsigned int totalElements = elementsContainer->getChildrenSize();

	if (allElements)
	{
		//needs to be recalculated
		listSize = 0.0f;

		//take into consideration the last visible element
		unsigned int lastVisibleIndex = totalElements;

		//all elements
		for (unsigned int ii = totalElements; ii > 0; ii--)
		{
			std::shared_ptr<HD_UI_Object> child = elementsContainer->getChildByIndex(ii - 1);

			if (!child->visible)
				continue; //skip this one

			float &childPos = type == vertical ? child->y : child->x;
			float endPos = 0.0f;

			if (lastVisibleIndex < totalElements)
			{
				float lvcPos = type == vertical ? elementsContainer->getChildByIndex(lastVisibleIndex)->y : elementsContainer->getChildByIndex(lastVisibleIndex)->x;
				float lvcSize = type == vertical ? elementsContainer->getChildByIndex(lastVisibleIndex)->height : elementsContainer->getChildByIndex(lastVisibleIndex)->width;

				endPos = lvcPos + lvcSize + 2.0f;
			}

			if (useAnimations)
			{
				CDBTweener::CTween *anim = new CDBTweener::CTween(&CDBTweener::TWEQ_EXPONENTIAL, CDBTweener::TWEA_IN, 0.5f, &childPos, endPos);
				elementsContainer->getChildByIndex(ii)->addAnimation(anim, true);
			}
			else
				childPos = endPos;
				
			float childSize = type == vertical ? child->height : child->width;
			listSize += childSize + 2.0f;

			lastVisibleIndex = ii - 1;
		}
	}
	else
	{
		//just the last element
		//doesn't take into account visibility

		//if its the first element add it at 0
		//if not, get the previous elements' pos & size
		float &childPos = type == vertical ? elementsContainer->getChildByIndex(0)->y : elementsContainer->getChildByIndex(0)->x;
		float endPos = 0.0f;

		if (totalElements > 1)
			endPos = type == vertical ? elementsContainer->getChildByIndex(1)->y + elementsContainer->getChildByIndex(1)->height + 2.0f :
				elementsContainer->getChildByIndex(1)->x + elementsContainer->getChildByIndex(1)->width + 2.0f;

		childPos = endPos;

		float childSize = type == vertical ? elementsContainer->getChildByIndex(0)->height : elementsContainer->getChildByIndex(0)->width;
		listSize += childSize + 2.0f;
	}

	//update scrollbar
	if (type == vertical)
	{
		scrollbar->visible = listSize > height ? true : false;
		scrollbar->SetHandleSize(height / listSize);
	}
	else
	{
		scrollbar->visible = listSize > width ? true : false;
		scrollbar->SetHandleSize(width / listSize);
	}
}

void HD_UI_ScrollableContainer::moveElements()
{
	float difference = type == vertical ? (height - listSize) : (listSize - width);;

	float endPos = scrollbar->position * difference;
	float &posToMove = type == vertical ? elementsContainer->y : elementsContainer->x;

	CDBTweener::CTween *anim = new CDBTweener::CTween(&CDBTweener::TWEQ_CIRCULAR, CDBTweener::TWEA_OUT, 0.5f, &posToMove, endPos);
	elementsContainer->addAnimation(anim, true);
}

// Public
//==========

void HD_UI_ScrollableContainer::CreateListContainer(const char* objectName, float fX, float fY, float fWidth, float fHeight, ScrollableType scrollType)
{
	setObjectProperties(objectName, fX, fY, fWidth, fHeight);
	type = scrollType;

	//create the scrollbar
	scrollbar = std::make_shared<HD_UI_Scrollbar>();
	if (type == vertical)
		scrollbar->CreateScrollbar("scrollbar", width - 14.0f, 0.0f, height, true);
	else if (type == horizontal)
		scrollbar->CreateScrollbar("scrollbar", 0.0f, height - 14.0f, width, false);

	addChild(scrollbar);

	//create the container in wich all elements are added
	elementsContainer = std::make_shared<HD_UI_Container>("elementsContainer", 0.0f, 0.0f, 0.0f, 0.0f);
	addChild(elementsContainer);

	//mask
	std::shared_ptr<HD_UI_Object> mask = std::make_shared<HD_UI_Object>("mask", 0.0f, 0.0f, width, height);
	AddClippingMask(mask, true);
}

void HD_UI_ScrollableContainer::Update()
{
	HD_UI_Container::Update();

	if (lastScrollPos < scrollbar->position)
		moveElements();
	else if (lastScrollPos > scrollbar->position)
		moveElements();

	lastScrollPos = scrollbar->position;
}

void HD_UI_ScrollableContainer::AddElement(std::shared_ptr<HD_UI_Object> element, bool atTop)
{
	if (atTop)
		elementsContainer->addChild(element);
	else
		elementsContainer->addChildAt(element, 0);

	//reposition elements
	repositionElements(atTop);
}

void HD_UI_ScrollableContainer::ShowElement(int id, bool show)
{
	elementsContainer->getChildByIndex(id)->visible = show;

	repositionElements(true, true);
}

void HD_UI_ScrollableContainer::RemoveElement(int id)
{
	elementsContainer->removeChildFromIndex(id);
	repositionElements(true, true);
}

std::shared_ptr<HD_UI_Object> HD_UI_ScrollableContainer::GetElement(int id)
{
	return elementsContainer->getChildByIndex(id);
}