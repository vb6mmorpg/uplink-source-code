// Implementation of Mouse
//========================

#include "HD_Mouse.h"

bool HD_Mouse::SetTarget(HD_UI_Object *newTarget)
{
	if (newTarget == NULL)
	{
		target = NULL;
		return false;
	}

	if (target == NULL)
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