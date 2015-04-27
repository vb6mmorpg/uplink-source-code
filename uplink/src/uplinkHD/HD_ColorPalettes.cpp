#include <allegro5/allegro.h>
#include "HD_ColorPalettes.h"

HD_ColorPalettes::HD_ColorPalettes(){
	//Color Sets - Used in Buttons mostly
	btnColors_white[0] = addAlphaToColor(warmGreys.cGrey3, 0.0f);		//base standard
	btnColors_white[1] = addAlphaToColor(warmGreys.cGrey3, 0.5f);			//base hover
	btnColors_white[2] = warmGreys.cGrey3;								//base clicked
	btnColors_white[3] = warmGreys.cGrey3;								//text standard
	btnColors_white[4] = warmGreys.cGrey0;								//text clicked
	btnColors_white[5] = warmGreys.cGrey3;								//stroke
	
	btnColors_red[0] = addAlphaToColor(cRed, 0);						//base standard
	btnColors_red[1] = addAlphaToColor(cRed, 0.5f);					//base hover
	btnColors_red[2] = cRed;											//base clicked
	btnColors_red[3] = cRed;											//text standard
	btnColors_red[4] = warmGreys.cGrey3;								//text clicked
	btnColors_red[5] = cRed;											//stroke

	btnColors_blue[0] = addAlphaToColor(blues.cBlue3, 0);				//base standard
	btnColors_blue[1] = addAlphaToColor(blues.cBlue3, 0.5f);			//base hover
	btnColors_blue[2] = blues.cBlue3;									//base clicked
	btnColors_blue[3] = blues.cBlue3;									//text standard
	btnColors_blue[4] = warmGreys.cGrey3;								//text clicked
	btnColors_blue[5] = blues.cBlue3;									//stroke

	btnColors_blueSat[0] = addAlphaToColor(bluesSat.cBlue2, 0);			//base standard
	btnColors_blueSat[1] = addAlphaToColor(bluesSat.cBlue2, 0.5f);		//base hover
	btnColors_blueSat[2] = bluesSat.cBlue2;								//base clicked
	btnColors_blueSat[3] = bluesSat.cBlue2;								//text standard
	btnColors_blueSat[4] = warmGreys.cGrey3;							//text clicked
	btnColors_blueSat[5] = bluesSat.cBlue2;								//stroke

	btnInputColors_blueSat[0] = addAlphaToColor(bluesSat.cBlue2, 0);	//base standard
	btnInputColors_blueSat[1] = addAlphaToColor(bluesSat.cBlue2, 0.5f);//base hover
	btnInputColors_blueSat[2] = bluesSat.cBlue2;						//base clicked
	btnInputColors_blueSat[3] = blues.cBlue3;							//text standard
	btnInputColors_blueSat[4] = warmGreys.cGrey3;						//text clicked
	btnInputColors_blueSat[5] = bluesSat.cBlue2;						//stroke & text input

	btnColors_orange[0] = addAlphaToColor(oranges.cOrange1, 0);			//base standard
	btnColors_orange[1] = addAlphaToColor(oranges.cOrange1, 0.5f);		//base hover
	btnColors_orange[2] = oranges.cOrange1;								//base clicked
	btnColors_orange[3] = oranges.cOrange1;								//text standard
	btnColors_orange[4] = warmGreys.cGrey3;								//text clicked
	btnColors_orange[5] = oranges.cOrange1;								//stroke

	btnSwitchColors_blueSat[0] = addAlphaToColor(bluesSat.cBlue2, 0);	//base standard
	btnSwitchColors_blueSat[1] = addAlphaToColor(bluesSat.cBlue2, 0.5f);//base hover
	btnSwitchColors_blueSat[2] = bluesSat.cBlue2;						//base clicked
	btnSwitchColors_blueSat[3] = blues.cBlue3;							//text standard
	btnSwitchColors_blueSat[4] = warmGreys.cGrey3;						//text clicked
	btnSwitchColors_blueSat[5] = bluesSat.cBlue2;						//stroke & text input
	btnSwitchColors_blueSat[6] = bluesSat.cBlue2;						//stroke & text input
}

// Helpful Functions
ALLEGRO_COLOR HD_ColorPalettes::getColorWithAlpha(ALLEGRO_COLOR c)
{
	ALLEGRO_COLOR cB = al_map_rgba(0, 0, 0, 0);

	cB.r = c.r * c.a / 255;
	cB.g = c.g * c.a / 255;
	cB.b = c.b * c.a / 255;
	cB.a = c.a / 255;

	return cB;
}

ALLEGRO_COLOR HD_ColorPalettes::addAlphaToColor(ALLEGRO_COLOR c, float a)
{
	ALLEGRO_COLOR cB = al_map_rgba_f(1.0f, 1.0f, 1.0f, 1.0f);

	cB.r = c.r * a;
	cB.g = c.g * a;
	cB.b = c.b * a;
	cB.a = a;

	return cB;
}

ALLEGRO_COLOR HD_ColorPalettes::getColorFromRange(ALLEGRO_COLOR c1, ALLEGRO_COLOR c2, float position)
{
	ALLEGRO_COLOR c = al_map_rgba_f(1.0f, 0.0f, 1.0f, 1.0f);

	if (position > 1.0f) return c; //position should be in the range of 0-1; return TEH HORROR

	c.r = c1.r > c2.r ? c2.r + ((c1.r - c2.r) * position) : c1.r + ((c2.r - c1.r) * position);

	c.g = c1.g > c2.g ? c2.g + ((c1.g - c2.g) * position) : c1.g + ((c2.g - c1.g) * position);

	c.b = c1.b > c2.b ? c2.b + ((c1.b - c2.b) * position) : c1.b + ((c2.b - c1.b) * position);

	c.a = c1.a > c2.a ? c2.a + ((c1.a - c2.a) * position) : c1.a + ((c2.a - c1.a) * position);

	return c;
}

HD_ColorPalettes *palette = NULL;