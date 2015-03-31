//========================================
// The container class can hold children
// objects and update them.
// Used for everything that needs to have
// more functionality or graphic objects
// eg. buttons, windows, layouts
//========================================

#ifndef HD_UI_CONTAINER_H
#define HD_UI_CONTAINER_H

#include <vector>
#include <allegro5/allegro.h>
#include "HD_UI_Object.h"

class HD_UI_Container : public HD_UI_Object
{
protected:
	//Child UI objects
	//A list of HD_UI_Objects. Used to update and draw on a "layer" basis.
	//0-7 background gfx; 8-15 normal layers; 16-24 windows; 25+ i dunno
	std::vector<HD_UI_Object*> children;

	HD_UI_Container();

public:
	~HD_UI_Container();

	virtual void Create();
	virtual void Update(ALLEGRO_MOUSE_STATE *mouseState, double timeSpeed);
	virtual void Draw();
	virtual void Clear();

	void addChild(HD_UI_Object *child);
	void addChildAt(HD_UI_Object *child, unsigned index);

	void removeChild(HD_UI_Object *child);
	void removeChildByName(char *childName);
	void removeChildFromIndex(int index);
	void removeAllChildren();
};

#endif