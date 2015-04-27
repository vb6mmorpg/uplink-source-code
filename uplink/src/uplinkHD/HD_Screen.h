//=========================================
// The meat of the MOD.
// Contains the Init and main loop,
// the screen, its properties and functions
// for manipulating its layout.
//
// Started on: 22:02.2015
// By: Tudor Stamate
//=========================================

#ifndef HD_SCREEN_H
#define HD_SCREEN_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include "UI_Objects/HD_UI_Container.h"
#include "HD_Mouse.h"

class HD_Screen
{
	struct Keyboard
	{
		int uniKey = 0;							//unicode char key; used for typing
		//bool key[ALLEGRO_KEY_MAX];				//normal AL keycode; used by various objects for input
		int key = 0;
		unsigned int modifiers = 0;				//AL bitfields; maybe it's gonna be used
	};

	//screen anchors
	struct AnchorPoint
	{
		int x;
		int y;
	};

	struct HD_ANCHORS
	{
		AnchorPoint tl;		//top-left
		AnchorPoint tc;		//top-center
		AnchorPoint tr;		//top-right
		AnchorPoint bl;		//bottom-left
		AnchorPoint bc;		//bottom-center
		AnchorPoint br;		//bottom-right

		AnchorPoint lc;		//left-center
		AnchorPoint c;		//center
		AnchorPoint rc;		//right-center

		int w13;		//width 1/3
		int w23;		//width 2/3

		int h13;		//height 1/3
		int h23;		//height 2/3
	};

private:

	//screen
	ALLEGRO_DISPLAY *hdDisplay;

	//update/render fps
	int fps;

	//current layout to update
	HD_UI_Container *currentLayout;		//the current layout to update

	//Allegro Inits. Called on start of game
	void HD_Init_Allegro();
	void HD_Init_Allegro_Modules();

	//The Main Loop
	bool isRunning;
	int  HD_Main_Loop();
	static bool bHD_Started;

public:

	//screen info
	int  nScreenW;
	int  nScreenH;
	int  refreshRate;
	bool bFullscreen;
	ALLEGRO_DISPLAY_MODE display_modes;

	HD_ANCHORS anchors;

	//updated members
	HD_Mouse *mouse;
	Keyboard keyboard;
	double deltaTime;

	//construction/destruction
	HD_Screen();
	~HD_Screen();

	void HD_StartMainLoop();

	void HD_Quit() { isRunning = false; }
	void HD_Dispose();	//used to uninstall Allegro and destroy the display

	//screen functions
	void hd_setResolution(int width, int height);
	void hd_setFullscreen(bool bIsFullscreen);
	void hd_initAnchors();

	//scaling functions
	float hd_scaleByWidth(float value);		//scales a value by the Width difference
	float hd_scaleByHeight(float value);	//scales a value by the Height difference

	//layout functions
	void hd_setNewLayout(HD_UI_Container *newLayout);		//set a layout for the screen
	HD_UI_Container* hd_getCurrentLayout();
};

extern HD_Screen *HDScreen;

#endif