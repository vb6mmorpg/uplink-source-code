//==============================
// The Scrollbar handle.
// Can get dragged by the mouse
// within a range.
//==============================

#ifndef HD_UI_SCROLLBAR_H
#define HD_UI_SCROLLBAR_H

#include "HD_UI_Container.h"

class HD_UI_Scrollbar : public HD_UI_Container
{
private:
	bool isVertical = true;
	float stepSize = 0.0f; //how much does the handle move if the player uses the scroll wheel or the scrollbar (not the handle)
	bool followMouse = false;

protected:
	void baseClick();
	void moveHandle(float distance, bool useAnimation = true);

public:
	HD_UI_Scrollbar() {}
	~HD_UI_Scrollbar() {}

	float position = 0.0f;	//the position the handle is at in the range of 0.0 - 1.0

	//Create a vertical or horizontal scrollbar
	void CreateScrollbar(const char* objectName, float fX, float fY, float size, bool vertical);

	void Update();

	void SetHandleSize(float size); //the size is within the range of 0.0 - 1.0
};

#endif