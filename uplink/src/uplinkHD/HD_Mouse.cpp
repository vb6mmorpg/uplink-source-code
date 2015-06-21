// Implementation of Mouse
//========================

#include "HD_Mouse.h"
#include "HD_Screen.h"
#include "HD_Resources.h"

bool HD_Mouse::SetTarget(std::shared_ptr<HD_UI_Container> newTarget)
{
	//keep the current target
	if (focusTaken)
		return newTarget == target ? true : false;

	if (!newTarget)
	{
		target = nullptr;
		return false;
	}

	//try to assign a new target
	if (target == nullptr)
	{
		target = newTarget;
		return true;
	}
	else
	{
		//verify if the newTarget is above the current one
		if (newTarget->gIndex >= target->gIndex)
		{
			target = newTarget;
			return true;
		}
		else
		{
			return false;
		}
	}
}

void HD_Mouse::Update()
{
	al_get_mouse_state(&state);
}

void HD_Mouse::Draw()
{
	al_draw_bitmap(gfxImage, state.x, state.y, 0);

	if (HDScreen->DebugInfo && target)
	{
		al_draw_text(HDResources->debugFont, al_map_rgb_f(1.0f, 0.0f, 1.0f), state.x, state.y + 26.0f, 0, target->name.c_str());
		std::string gID = "gI = " + std::to_string(target->gIndex);
		al_draw_text(HDResources->debugFont, al_map_rgb_f(1.0f, 0.0f, 1.0f), state.x, state.y + 38.0f, 0, gID.c_str());
		std::string mZ = "mZ = " + std::to_string(state.z);
		al_draw_text(HDResources->debugFont, al_map_rgb_f(0.0f, 1.0f, 0.0f), state.x, state.y + 50.0f, 0, mZ.c_str());
	}
}