//==============================
// The Mouse object.
// Keeps track of the MouseState
// and it's target underneath.
// Tooltips also.
//==============================

#ifndef HD_MOUSE_H
#define HD_MOUSE_H

#include <allegro5/allegro.h>
#include <memory>

#include "UI_Objects/HD_UI_Container.h"
#include "HD_Resources.h"

class HD_Mouse
{
private:
	ALLEGRO_BITMAP *gfxImage;

	ALLEGRO_MOUSE_STATE state;
	std::shared_ptr<HD_UI_Container> target = nullptr;
	bool focusTaken = false;

public:
	HD_Mouse()
	{
		HDResources->hd_loadImage("mousePointer.png");
		gfxImage = HDResources->hd_getImage("mousePointer.png");
	}
	~HD_Mouse()
	{
		target = nullptr;
	}

	void Update();
	void Draw();

	bool SetTarget(std::shared_ptr<HD_UI_Container> newTarget);
	std::shared_ptr<HD_UI_Container> GetTarget() { return target; }
	ALLEGRO_MOUSE_STATE* GetState() { return &state; }
	void SetFocused(bool focused) { focusTaken = focused; }
	bool IsFocused() { return focusTaken; }
};

#endif