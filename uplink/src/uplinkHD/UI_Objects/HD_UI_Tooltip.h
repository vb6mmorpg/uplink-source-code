// Tooltip object.
// Used by buttons, mostly.
//===========================

#ifndef HD_UI_TOOLTIP_H
#define HD_UI_TOOLTIP_H

#include "HD_UI_Container.h"
#include "HD_UI_GraphicsObject.h"
#include "HD_UI_TextObject.h"

class HD_UI_Tooltip : public HD_UI_Container
{
protected:
	void resize();

public:
	HD_UI_Tooltip() {}
	~HD_UI_Tooltip() {}

	void Create(const char *tooltip, float fX, float fY);

	void ShowTooltip(bool show);
};

#endif