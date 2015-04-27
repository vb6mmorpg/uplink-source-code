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
protected:
	//global members - used in updating/drawing
	float globalX = 0.0f;
	float globalY = 0.0f;
	float drawWidth = 0.0f;
	float drawHeight = 0.0f;
	float globalScaleX = 1.0f;
	float globalScaleY = 1.0f;
	float drawAlpha = 1.0f; //composed alpha between parent & child; used for drawing
	bool redraw = false;
	bool isVisible = true;

	HD_UI_Container *parent;
	CDBTweener tweensContainer;

	//Sets up the normal values and its parent
	void virtual setObjectProperties(char *objectName, float fX, float fY,
		float fWidth, float fHeight, HD_UI_Container *newParent, int nIndex);

public:
	//constructor/destructor
	HD_UI_Object();
	~HD_UI_Object();

	//members - relative to parent
	float x = 0.0f;
	float y = 0.0f;
	float width = 0.0f;
	float height = 0.0f;
	float scaleX = 1.0f;
	float scaleY = 1.0f;
	float alpha = 1.0f;
	bool visible = true;

	//object identifiers
	char *name;	//the name of this object
	int index;	//the index this object has
	int gIndex; //the global index of this object

	//functions
	virtual void Update();
	virtual void Draw();
	virtual void Clear();

	//parent/child functions
	HD_UI_Container* getParent() { return parent; }
	void setParent(HD_UI_Container *newParent, int index);

	//animations
	void addAnimation(CDBTweener::CTween *newTween, bool removeAnims = false);
};

#endif