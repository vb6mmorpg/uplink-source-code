//========================================
// This is where all palettes are kept.
// Refer to the UHD color sheet for more info.
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
		ALLEGRO_COLOR cGrey0;
		ALLEGRO_COLOR cGrey1;
		ALLEGRO_COLOR cGrey2;
		ALLEGRO_COLOR cGrey3;

		Grey01()
		{
			cGrey0 = al_map_rgba_f(0.04f, 0.04f, 0.04f, 1.0f); //al_map_rgba(11, 11, 11, 255);
			cGrey1 = al_map_rgba_f(0.23f, 0.23f, 0.23f, 1.0f); //al_map_rgba(60, 60, 60, 255);
			cGrey2 = al_map_rgba_f(0.61f, 0.61f, 0.61f, 1.0f); //al_map_rgba(157, 157, 157, 255);
			cGrey3 = al_map_rgba_f(1.0f, 1.0f, 1.0f, 1.0f); //al_map_rgba(255, 255, 255, 255);
		}
	};

	struct Grey02
	{
		ALLEGRO_COLOR cGrey0;
		ALLEGRO_COLOR cGrey1;
		ALLEGRO_COLOR cGrey2;

		Grey02()
		{
			cGrey0 = al_map_rgba_f(0.48f, 0.5f, 0.51f, 1.0f); //al_map_rgba(123, 129, 132, 255);
			cGrey1 = al_map_rgba_f(0.63f, 0.65f, 0.67f, 1.0f); //al_map_rgba(161, 168, 172, 255);
			cGrey2 = al_map_rgba_f(0.77f, 0.81f, 0.83f, 1.0f); //al_map_rgba(198, 207, 212, 255);
		}
	};

	struct Blues01
	{
		ALLEGRO_COLOR cBlue0;
		ALLEGRO_COLOR cBlue1;
		ALLEGRO_COLOR cBlue2;
		ALLEGRO_COLOR cBlue3;

		Blues01()
		{
			cBlue0 = al_map_rgba_f(0.0f, 0.09f, 0.18f, 1.0f); //al_map_rgba(1, 23, 46, 255);
			cBlue1 = al_map_rgba_f(0.06f, 0.16f, 0.23f, 1.0f); //al_map_rgba(17, 43, 60, 255);
			cBlue2 = al_map_rgba_f(0.03f, 0.24f, 0.46f, 1.0f); //al_map_rgba(8, 62, 118, 255);
			cBlue3 = al_map_rgba_f(0.11f, 0.38f, 0.65f, 1.0f); //al_map_rgba(30, 98, 168, 255);
		}
	};

	struct Blues02
	{
		ALLEGRO_COLOR cBlue0;
		ALLEGRO_COLOR cBlue1;
		ALLEGRO_COLOR cBlue2;
		ALLEGRO_COLOR cBlue3;

		Blues02()
		{
			cBlue0 = al_map_rgba_f(0.05f, 0.16f, 0.35f, 1.0f); //al_map_rgba(14, 43, 90, 255);
			cBlue1 = al_map_rgba_f(0.02f, 0.26f, 0.67f, 1.0f); //al_map_rgba(6, 67, 171, 255);
			cBlue2 = al_map_rgba_f(0.16f, 0.66f, 1.0f, 1.0f); //al_map_rgba(43, 170, 255, 255);
			cBlue3 = al_map_rgba_f(0.74f, 0.90f, 1.0f, 1.0f); //al_map_rgba(191, 230, 255, 255);
		}
	};

	struct Cyans
	{
		ALLEGRO_COLOR cCyan0;
		ALLEGRO_COLOR cCyan1;
		ALLEGRO_COLOR cCyan2;
		ALLEGRO_COLOR cCyan3;

		Cyans()
		{
			cCyan0 = al_map_rgba_f(0.05f, 0.34f, 0.35f, 1.0f); //al_map_rgba(14, 89, 90, 255);
			cCyan1 = al_map_rgba_f(0.02f, 0.65f, 0.67f, 1.0f); //al_map_rgba(6, 168, 171, 255);
			cCyan2 = al_map_rgba_f(0.16f, 1.0f, 0.82f, 1.0f); //al_map_rgba(43, 255, 209, 255);
			cCyan3 = al_map_rgba_f(0.74f, 1.0f, 0.94f, 1.0f); //al_map_rgba(191, 255, 240, 255);
		}
	};

	struct Oranges
	{
		ALLEGRO_COLOR cOrange0;
		ALLEGRO_COLOR cOrange1;
		ALLEGRO_COLOR cOrange2;

		Oranges()
		{
			cOrange0 = al_map_rgba_f(0.70f, 0.42f, 0.0f, 1.0f); //al_map_rgba(181, 108, 0, 255);
			cOrange1 = al_map_rgba_f(1.0f, 0.64f, 0.1f, 1.0f); //al_map_rgba(255, 164, 28, 255);
			cOrange2 = al_map_rgba_f(1.0f, 0.91f, 0.79f, 1.0f); //al_map_rgba(255, 234, 203, 255);
		}
	};
	//END



public:

	HD_ColorPalettes();
	~HD_ColorPalettes(){};

	const Grey01 warmGreys = Grey01();
	const Grey02 coldGreys = Grey02();
	const Blues01 blues = Blues01();
	const Blues02 bluesSat = Blues02();
	const Cyans cyans = Cyans();
	const Oranges oranges = Oranges(); //the colors not the fruits
	const ALLEGRO_COLOR cRed = al_map_rgba_f(0.82f, 0.1f, 0.1f, 1.0f); //al_map_rgba(211, 26, 26, 255);

	const ALLEGRO_COLOR cBg1 = warmGreys.cGrey0;
	const ALLEGRO_COLOR cBg2 = blues.cBlue1;

	//Color Sets - Used in Buttons mostly
	ALLEGRO_COLOR btnColors_white[6];
	ALLEGRO_COLOR btnColors_red[6];
	ALLEGRO_COLOR btnColors_blue[6];
	ALLEGRO_COLOR btnColors_blueSat[6];
	ALLEGRO_COLOR btnInputColors_blueSat[6];
	ALLEGRO_COLOR btnColors_orange[6];
	ALLEGRO_COLOR btnSwitchColors_blueSat[7];

	// Helpful Functions
	ALLEGRO_COLOR getColorWithAlpha(ALLEGRO_COLOR c);										//pre-multiplies the alpha of the given color
	ALLEGRO_COLOR addAlphaToColor(ALLEGRO_COLOR c, float a);								//adds alpha to the color and pre-multiplies it
	ALLEGRO_COLOR getColorFromRange(ALLEGRO_COLOR c1, ALLEGRO_COLOR c2, float position);	//gets the color at position from the range specified by the two colors

};

extern HD_ColorPalettes *palette;

#endif