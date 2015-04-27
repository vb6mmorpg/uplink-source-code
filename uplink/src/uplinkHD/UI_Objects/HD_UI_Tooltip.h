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
private:
	HD_UI_GraphicsObject *bgObject[3];
	HD_UI_TextObject *txtObject = NULL;

	HD_UI_Tooltip() {}

public:
	HD_UI_Tooltip(char *tooltip, HD_UI_Container *newParent);
	~HD_UI_Tooltip() {}

	void ShowTooltip(bool show);
};

#endif