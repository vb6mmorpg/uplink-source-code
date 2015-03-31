//========================================
// The base class of a UI Object. GFX,
// buttons, text, containers, windows, all
// inherit from this.
// Not really used in actual layout
// creation.
//========================================

#ifndef HD_UI_OBJECT_H
#define HD_UI_OBJECT_H

#include <allegro5/allegro.h>
#include "dbtweener.h"

class HD_UI_Container;

class HD_UI_Object
{
private:
	float lastParentX;
	float lastParentY;
	float last_x;
	float last_y;
	float createW;	//the W & H on creation
	float createH;

protected:

	HD_UI_Container *parent;
	CDBTweener tweensContainer;

	//Sets up the normal values and its parent
	void virtual setObjectProperties(char *objectName, float fX, float fY,
		float fWidth, float fHeight, HD_UI_Container *newParent, int nIndex);

public:

	//constructor/destructor
	HD_UI_Object();
	~HD_UI_Object();

	//members
	float x;		//global x & y
	float y;
	float _x;		//local x & y
	float _y;
	float width;	//even width and height?
	float height;
	float scaleX;	//actually the scale is relative to parent
	float scaleY;

	char *name;	//the name of this object
	int index;	//the index this object has

	bool isVisible = true; //to draw or not to draw

	//functions
	virtual void Update(ALLEGRO_MOUSE_STATE *mouseState, double timeSpeed);
	virtual void Draw();
	virtual void Clear();

	//parent/child functions
	HD_UI_Container* getParent() { return parent; }
	void setParent(HD_UI_Container *newParent, int index);

	//animations
	void addAnimation(CDBTweener::CTween *newTween, bool removeAnims = false);
};

#endif