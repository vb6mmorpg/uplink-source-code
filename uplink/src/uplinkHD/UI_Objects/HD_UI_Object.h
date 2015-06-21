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
#include <memory>
#include "dbtweener.h"

class HD_UI_Container;

class HD_UI_Object
{
protected:
	//global members - used in updating/drawing
	//composed between parent & child
	float globalX = 0.0f;
	float globalY = 0.0f;
	float drawWidth = 0.0f;
	float drawHeight = 0.0f;
	float globalScaleX = 1.0f;
	float globalScaleY = 1.0f;
	float drawAlpha = 1.0f;
	bool redraw = false;
	bool isVisible = true;

	std::shared_ptr<HD_UI_Container> parent = nullptr;
	CDBTweener tweensContainer;

	//Sets up the normal values and its parent
	void setObjectProperties(const char *objectName, float fX, float fY,
		float fWidth, float fHeight);

	//Checks if this object is inside a clipping mask. Used for drawing
	bool isInClippingMask();

	void DrawDebugInfo(ALLEGRO_COLOR c);

public:
	//constructor/destructor
	HD_UI_Object() {}
	HD_UI_Object(const char* objectName, float fX, float fY, float fWidth, float fHeight);
	~HD_UI_Object() { }

	//members - relative to parent
	float x = 0.0f;
	float y = 0.0f;
	float width = 0.0f;
	float height = 0.0f;
	float scaleX = 1.0f;
	float scaleY = 1.0f;
	float rotation = 0.0f; //only used by images!
	float alpha = 1.0f;
	bool visible = true;

	//object identifiers
	std::string name;	//the name of this object
	int index;			//the index in the current container
	int gIndex = 0; //the global index

	//globals getters
	float GlobalX() { return globalX; }
	float GlobalY() { return globalY; }

	//Object update functions
	virtual void Update();
	virtual void Draw();
	virtual void Clear();

	//Parent/child functions
	void setParent(std::shared_ptr<HD_UI_Container> newParent) { parent = newParent; }

	//Animations
	//Adds a CTween animation; if removeAnims, it stops all other animations on this object
	//Loop: 0 - none; 1 - repeat; 2 - ping pong;
	//Replays: -1 - infinite; 
	void addAnimation(CDBTweener::CTween *newTween, bool removeAnims = false, int loop = 0, int replays = -1);
	void removeAnimations() { tweensContainer.removeAllTweens(); }

	class CLoop : public CDBTweener::IListener
	{
	public:
		void onTweenFinished(CDBTweener::CTween *pTween)
		{
			//check replays
			int replays = pTween->getUserIntData();
			if (replays > 0)
				replays--;
			else if (replays == 0)
				return;

			//get the needed values
			CDBTweener::SValue *values = pTween->getValues()[0];
			*values->m_fpValue = values->m_fValueStart;

			//create a new tween
			CDBTweener::CTween *bTween = new CDBTweener::CTween(pTween->getEquation(), pTween->getEasing(), pTween->getDurationSec(), values->m_fpValue, values->m_fTarget);
			
			//the object that plays this tween
			HD_UI_Object *object = static_cast<HD_UI_Object*>(pTween->getUserData());

			bTween->setUserData(object);
			bTween->setUserIntData(replays);
			bTween->addListener(this);
			object->addAnimation(bTween);
		}
	} m_tweenLoop;

	class CPingPong : public CDBTweener::IListener
	{
	public:
		void onTweenFinished(CDBTweener::CTween *pTween)
		{
			int replays = pTween->getUserIntData();
			if (replays > 0)
				replays--;
			else if (replays == 0)
				return;

			CDBTweener::SValue *values = pTween->getValues()[0];
			CDBTweener::CTween *bTween = new CDBTweener::CTween(pTween->getEquation(), pTween->getEasing(), pTween->getDurationSec(), values->m_fpValue, values->m_fValueStart);
			HD_UI_Object *object = static_cast<HD_UI_Object*>(pTween->getUserData());
			bTween->setUserData(object);
			bTween->setUserIntData(replays);
			bTween->addListener(this);
			object->addAnimation(bTween);
		}
	} m_tweenPingPong;
};

#endif