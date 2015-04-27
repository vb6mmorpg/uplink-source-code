//==============================
// The Mouse object.
// Keeps track of the MouseState
// and it's target underneath.
// Tooltips also.
//==============================

#ifndef HD_MOUSE_H
#define HD_MOUSE_H

#include <allegro5/allegro.h>

#include "UI_Objects/HD_UI_Object.h"
#include "HD_Resources.h"

class HD_Mouse
{
private:
	ALLEGRO_BITMAP *gfxImage;

	ALLEGRO_MOUSE_STATE state;
	HD_UI_Object *target;

public:
	HD_Mouse()
	{
		gfxImage = HDResources->hd_getImage("mousePointer.png");
		target = NULL;
	}
	~HD_Mouse()
	{
		al_destroy_bitmap(gfxImage);
		target = NULL;
	}

	void Update();
	void Draw() { al_draw_bitmap(gfxImage, state.x, state.y, 0); }

	bool SetTarget(HD_UI_Object *newTarget);
	HD_UI_Object* GetTarget() { return target; }
	ALLEGRO_MOUSE_STATE* GetState() { return &state; }
};

#endif