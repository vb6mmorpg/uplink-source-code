//========================================
// This is where all palettes are kept.
// Singleton.
//========================================

#ifndef HD_COLORPALETTES_H
#define HD_COLORPALETTES_H

#include <allegro5/allegro.h>

class HD_ColorPalettes
{
private:
	// COLOR PALETTES
	struct Grey01
	{
		ALLEGRO_COLOR cGrey0 = al_map_rgba(11, 11, 11, 255);
		ALLEGRO_COLOR cGrey1 = al_map_rgba(60, 60, 60, 255);
		ALLEGRO_COLOR cGrey2 = al_map_rgba(157, 157, 157, 255);
		ALLEGRO_COLOR cGrey3 = al_map_rgba(250, 250, 250, 255);
	};

	struct Grey02
	{
		ALLEGRO_COLOR cGrey0 = al_map_rgba(123, 129, 132, 255);
		ALLEGRO_COLOR cGrey1 = al_map_rgba(161, 168, 172, 255);
		ALLEGRO_COLOR cGrey2 = al_map_rgba(198, 207, 212, 255);
	};

	struct Blues01
	{
		ALLEGRO_COLOR cBlue0 = al_map_rgba(1, 23, 46, 255);
		ALLEGRO_COLOR cBlue1 = al_map_rgba(17, 43, 60, 255);
		ALLEGRO_COLOR cBlue2 = al_map_rgba(8, 62, 118, 255);
		ALLEGRO_COLOR cBlue3 = al_map_rgba(30, 98, 168, 255);
	};

	struct Blues02
	{
		ALLEGRO_COLOR cBlue0 = al_map_rgba(14, 43, 90, 255);
		ALLEGRO_COLOR cBlue1 = al_map_rgba(6, 67, 171, 255);
		ALLEGRO_COLOR cBlue2 = al_map_rgba(43, 170, 255, 255);
		ALLEGRO_COLOR cBlue3 = al_map_rgba(191, 230, 255, 255);
	};

	struct Cyans
	{
		ALLEGRO_COLOR cCyan0 = al_map_rgba(14, 89, 90, 255);
		ALLEGRO_COLOR cCyan1 = al_map_rgba(6, 168, 171, 255);
		ALLEGRO_COLOR cCyan2 = al_map_rgba(43, 255, 209, 255);
		ALLEGRO_COLOR cCyan3 = al_map_rgba(191, 255, 240, 255);
	};

	struct Oranges
	{
		ALLEGRO_COLOR cOrange0 = al_map_rgba(181, 108, 0, 255);
		ALLEGRO_COLOR cOrange1 = al_map_rgba(255, 164, 28, 255);
		ALLEGRO_COLOR cOrange2 = al_map_rgba(255, 234, 203, 255);
	};
	//END



public:
	Grey01 warmGreys;
	Grey02 coldGreys;
	Blues01 blues;
	Blues02 bluesSat;
	Cyans cyans;
	Oranges oranges; //the colors not the fruits
	ALLEGRO_COLOR cRed = al_map_rgba(211, 26, 26, 255);

	//Color Sets - Used in Buttons mostly
	/*ALLEGRO_COLOR btnColors_white[] =
	{
		addAlphaToColor(warmGreys.cGrey3, 0),		//base standard
		addAlphaToColor(warmGreys.cGrey3, 125),		//base hover
		warmGreys.cGrey3,							//base clicked
		warmGreys.cGrey3,							//text standard
		warmGreys.cGrey0,							//text clicked
		warmGreys.cGrey3							//stroke
	};*/

	// Helpful Functions
	ALLEGRO_COLOR getColorWithAlpha(ALLEGRO_COLOR c)
	{
		c.r = c.r * c.a / 255;
		c.g = c.g * c.a / 255;
		c.b = c.b * c.a / 255;

		return c;
	}

	ALLEGRO_COLOR addAlphaToColor(ALLEGRO_COLOR c, unsigned char a)
	{
		c.r = c.r * a / 255;
		c.g = c.g * a / 255;
		c.b = c.b * a / 255;
		c.a = a;

		return c;
	}
};

extern HD_ColorPalettes *HDColors;

#endif