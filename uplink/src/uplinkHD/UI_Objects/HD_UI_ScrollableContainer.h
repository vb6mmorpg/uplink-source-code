//================================
// Creates a clipped Container,
// that can be scrolled.
// Easy right?
//================================

#ifndef HD_UI_SCROLLABLECONTAINER_H
#define HD_UI_SCROLLABLECONTAINER_H

#include "HD_UI_Container.h"
#include "HD_UI_Scrollbar.h"

enum ScrollableType 
{
	vertical,
	horizontal,
	both
};

class HD_UI_ScrollableContainer : public HD_UI_Container
{
private:
	ScrollableType type;
	std::shared_ptr<HD_UI_Scrollbar> scrollbar = nullptr;
	std::shared_ptr<HD_UI_Container> elementsContainer = nullptr;

	//used by table container
	int rows = 0;
	int columns = 0;

	void repositionElements(bool allElements = false, bool useAnimations = false);
	void moveElements();
	float lastScrollPos = 0.0f;
	float listSize = 0.0f;

public:
	HD_UI_ScrollableContainer() {}
	~HD_UI_ScrollableContainer() {}

	//Creates a scrollable container that has a clipping mask set to its w & h
	//arranges its elements in a list manner
	void CreateListContainer(const char* objectName, float fX, float fY, float fWidth, float fHeight, ScrollableType scrollType);

	//arranges its elements in a table manner
	//void CreateTableContainer(const char* objectName, float fX, float fY, float fWidth, float fHeight, int iRows, int iColumns, ScrollableType scrollType);

	void Update();

	//Adds an element to the list. Also assigns it as a child!
	void AddElement(std::shared_ptr<HD_UI_Object> element, bool atTop = false);
	//Show or hide an element from the list
	void ShowElement(int id, bool show);
	//Remove an element from the list
	void RemoveElement(int id);
	//Get an element from the list
	std::shared_ptr<HD_UI_Object> GetElement(int id);
};

#endif