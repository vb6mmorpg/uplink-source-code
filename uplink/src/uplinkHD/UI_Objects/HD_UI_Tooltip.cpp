// Implementation of the Tooltip Object
//=====================================

#include "HD_UI_Tooltip.h"
#include "../HD_Resources.h"

#include <math.h>

HD_UI_Tooltip::HD_UI_Tooltip(const char *tooltip, HD_UI_Container *newParent)
{
	setObjectProperties("tooltip", 0.0f, 0.0f, 0.0f, 0.0f, newParent, -1);

	bgObject[0] = new HD_UI_GraphicsObject("bgL", -1, "tooltip_L", "misc_atlas.xml", 0.0f, 0.0f, this);

	txtObject = new HD_UI_TextObject("txt", -1, bgObject[0]->width, 0.0f, tooltip, HDResources->font24, al_map_rgb_f(1.0f, 1.0f, 1.0f), ALLEGRO_ALIGN_LEFT, this);
	txtObject->y = bgObject[0]->height / 2.0f - txtObject->height / 2.0f;

	bgObject[1] = new HD_UI_GraphicsObject("bgM", 0, "tooltip_M", "misc_atlas.xml", bgObject[0]->width, 0.0f, this);
	bgObject[1]->width = ceilf(txtObject->getTextWidth());
	bgObject[2] = new HD_UI_GraphicsObject("bgR", 0, "tooltip_R", "misc_atlas.xml", bgObject[1]->x + bgObject[1]->width, 0.0f, this);

	x = ceilf(parent->width + 6.0f);
	y = ceilf(parent->height * 0.75f);
	width = bgObject[0]->width + bgObject[1]->width + bgObject[2]->width;
	height = bgObject[0]->height;
}

void HD_UI_Tooltip::ShowTooltip(bool show)
{
	visible = show;
	alpha = 0.0f;

	if (show)
	{
		alpha = 0.0f;
		CDBTweener::CTween *tween = new CDBTweener::CTween(&CDBTweener::TWEQ_LINEAR, CDBTweener::TWEA_OUT, 0.5f, &alpha, 1.0f);
		addAnimation(tween, true);
	}
	else alpha = 0.0f;
}