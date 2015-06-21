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

class HD_UI_Button;
class HD_UI_ButtonInput;
class HD_UI_ButtonMenu;
class HD_UI_PopUp;
class HD_UI_TextObject;

class HD_UI_Container : public HD_UI_Object, public std::enable_shared_from_this<HD_UI_Container>
{
protected:
	//Child UI objects
	//A list of HD_UI_Objects. Used to update and draw on a "layer" basis.
	std::vector<std::shared_ptr<HD_UI_Object>> children;

	//Clipping Mask
	std::shared_ptr<HD_UI_Object> clippingMask = nullptr;
	bool hasClippingMask = false;
	void setClippingMask();
	void resetClippingMask();

	//Mouse target
	bool setMouseOver();
	bool rootIsMouseTarget();

public:
	//Create an empty container
	HD_UI_Container();
	HD_UI_Container(const char* name, float fX, float fY, float fWidth, float fHeight);
	~HD_UI_Container() {}

	virtual void Create();
	virtual void Update();
	virtual void Draw();
	virtual void Clear();

	void AddClippingMask(std::shared_ptr<HD_UI_Object> newMask, bool takeOwnership = true);
	std::shared_ptr<HD_UI_Object> GetClippingMask();
	bool HasClippingMask();
	void RemoveClippingMask();

	bool isMouseTarget();	//is this the mouse target?
	bool hasMouseFocus();

	unsigned int getChildrenSize() { return children.size(); }
	void addChild(std::shared_ptr<HD_UI_Object> child);
	void addChildAt(std::shared_ptr<HD_UI_Object> child, unsigned index);

	std::shared_ptr<HD_UI_Object> getChildByIndex(unsigned int index);
	std::shared_ptr<HD_UI_Object> getChildByName(const char *name);
	std::shared_ptr<HD_UI_Container> getContainerByIndex(unsigned int index);
	std::shared_ptr<HD_UI_Container> getContainerByName(const char *name);
	std::shared_ptr<HD_UI_Button> getButtonByName(const char *name);
	std::shared_ptr<HD_UI_ButtonInput> getButtonInputByName(const char *name);
	std::shared_ptr<HD_UI_ButtonMenu> getButtonMenuByName(const char *name);
	std::shared_ptr<HD_UI_PopUp> getPopUpByName(const char *name);
	std::shared_ptr<HD_UI_TextObject> getTextObjectByName(const char *name);

	void removeChild(std::shared_ptr<HD_UI_Object> child);
	void removeChildByName(char *childName);
	void removeChildFromIndex(unsigned int index);
	void removeAllChildren();
};

#endif