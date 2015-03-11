//========================================
// The base class of a Layout. All other
// layouts/containers derive from this.
// Keeps track of buttons, windows and
// other GFX that are children objects.
// Kinda like the old Uplink interface.
//========================================

#ifndef HD_UI_LAYOUT_H
#define HD_UI_LAYOUT_H

#define MAX_CHILDREN 64

#include <vector>
#include "uplinkHD/UI_Objects/HD_UI_Object.h"

//class HD_UI_Object;

class HD_UI_Layout
{
protected:

	//Child UI objects
	//A list of HD_UI_Objects. Used to update and draw on a "layer" basis.
	//0-7 background gfx; 8-15 normal layers; 16-24 windows; 25+ i dunno
	std::vector<HD_UI_Object> children;

public:

	//dunno if i'll actually use these,
	//but they're meant for animations
	//and for relative placement of childs.
	int x;
	int y;
	int width;
	int height;

	//constructor/destructor
	HD_UI_Layout();
	~HD_UI_Layout();

	void virtual Create();	//create everything for this layout!
	void virtual Update();	//updates itself and children
	void virtual Draw();	//draws children
	void virtual Clear();	//clears everything up before being deleted

	void addChild(HD_UI_Object *child);
	void addChildAt(HD_UI_Object *child, unsigned index);

	void removeChild(HD_UI_Object *child);
	void removeChildByName(char *childName);
	void removeChildFromIndex(int index);
	void removeAllChildren();
};

#endif