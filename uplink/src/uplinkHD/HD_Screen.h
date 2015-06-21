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
		int key = 0;
		unsigned int modifiers = 0;				//AL bitfields; maybe it's gonna be used
	};

private:

	//screen
	ALLEGRO_DISPLAY *hdDisplay;

	//update/render fps
	int fps;

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
	bool bFrameless;

	//updated members
	std::shared_ptr<HD_Mouse> mouse;
	Keyboard keyboard;
	double deltaTime;
	unsigned int DebugInfo = 0;

	//construction/destruction
	HD_Screen();
	~HD_Screen();

	void HD_StartMainLoop();

	void HD_Quit() { isRunning = false; }
	void HD_Dispose();	//used to uninstall Allegro and destroy the display

	//screen functions
	void hd_setResolution(int width, int height);
	void hd_setFrameless(bool bIsFrameless);
};

extern std::unique_ptr<HD_Screen> HDScreen;

#endif