//implementation of HD_Screen

#include "HD_Screen.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>

#include "options/options.h"
#include "app/app.h"

#include "HD_Resources.h"
#include "HD_Mouse.h"
#include "UI_Layouts\HD_MainMenu.h"

//=======================================================
//Private:
//=======================================================

void HD_Screen:: HD_Init_Allegro()
{
	hdDisplay = NULL;
	al_get_display_mode(al_get_num_display_modes() - 1, &display_modes);

	nScreenW = 0;
	nScreenH = 0;
	bFrameless = false;
	refreshRate = 0;

	//For the first run, set the maximum available resolution and fullscreen windowed
	//if not, get the settings in options
	if (app->GetOptions()->IsOptionEqualTo("game_firsttime", 1))
	{
		nScreenW = display_modes.width;
		nScreenH = display_modes.height;
		bFrameless = true;
		refreshRate = display_modes.refresh_rate;

		app->GetOptions()->SetOptionValue("graphics_screenwidth", nScreenW);
		app->GetOptions()->SetOptionValue("graphics_screenheight", nScreenH);
		app->GetOptions()->SetOptionValue("graphics_fullscreen", bFrameless);
		app->GetOptions()->SetOptionValue("graphics_screenrefresh", refreshRate);
		app->GetOptions()->SetOptionValue("game_firsttime", 0);

		app->GetOptions()->Save(NULL);
	}
	else
	{
		nScreenW = app->GetOptions()->GetOptionValue("graphics_screenwidth");
		nScreenH = app->GetOptions()->GetOptionValue("graphics_screenheight");
		bFrameless = app->GetOptions()->GetOptionValue("graphics_fullscreen");
		refreshRate = app->GetOptions()->GetOptionValue("graphics_screenrefresh");
		if (refreshRate == -1) refreshRate = display_modes.refresh_rate;
	}

	//set fullscreen/windowed mode 
	if (bFrameless)
	{
		al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
		al_set_new_display_flags(ALLEGRO_FRAMELESS);
	}
	else al_set_new_display_flags(ALLEGRO_WINDOWED);

	al_set_new_display_refresh_rate(refreshRate);

	//multisampling
	al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
	al_set_new_display_option(ALLEGRO_SAMPLES, 4, ALLEGRO_SUGGEST);
	al_set_new_display_option(ALLEGRO_COLOR_SIZE, 32, ALLEGRO_REQUIRE);
	//al_set_new_bitmap_flags(ALLEGRO_MAG_LINEAR | ALLEGRO_MIN_LINEAR);
	al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);

	//creating the display
	hdDisplay = al_create_display(nScreenW, nScreenH);
	if (!hdDisplay)
	{
		al_show_native_message_box(hdDisplay, "Warning!", "",
			"Could not create the Display!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
	}

	al_set_window_position(hdDisplay, 0, 0);

	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_flip_display();

	fps = 60;

	hd_initAnchors();	//reset anchors
	currentLayout = NULL;
}

void HD_Screen:: HD_Init_Allegro_Modules()
{
	//Initialize Allegro Modules
	al_init_font_addon();
	al_init_ttf_addon();
	al_init_image_addon();
	al_init_primitives_addon();

	al_install_mouse();
	al_install_keyboard();
}

//====================================
//The Main Loop for updating/rendering
//====================================
int HD_Screen:: HD_Main_Loop()
{
	isRunning = true;
	bool redraw = false;

	ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
	ALLEGRO_TIMER *timer = al_create_timer(1.0 / fps);

	al_register_event_source(event_queue, al_get_display_event_source(hdDisplay));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());

	al_start_timer(timer);

	//Last call to arms before loop
	al_hide_mouse_cursor(hdDisplay);
	mouse = new HD_Mouse();

	//First Screen:
	HD_MainMenu *mainMenu = new HD_MainMenu();
	hd_setNewLayout(mainMenu);

	while (isRunning)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		//Close if closed from the window button!
		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			isRunning = false;
		}

		//Update everything!
		if (ev.type == ALLEGRO_EVENT_TIMER)
		{
			redraw = true;

			//update members
			mouse->Update();
			deltaTime = al_get_timer_speed(timer);

			//update current layout
			if (currentLayout)
				currentLayout->Update();

		}

		//Keyboard functionality
		if (ev.type == ALLEGRO_EVENT_KEY_CHAR)
		{
			keyboard.key = ev.keyboard.keycode;
			keyboard.uniKey = ev.keyboard.unichar;
			keyboard.modifiers = ev.keyboard.modifiers;
		}
		else
		{
			keyboard.key = 0;
			keyboard.uniKey = 0;
			keyboard.modifiers = 0;
		}

		//Rendering
		if (redraw && al_is_event_queue_empty(event_queue))
		{
			redraw = false;
			//redraw current layout
			if (currentLayout)
				currentLayout->Draw();

			mouse->Draw();

			al_flip_display();
			al_clear_to_color(al_map_rgb(0, 0, 0));
		}
	}

	return 0;
}

//=======================================================
//Public:
//=======================================================

//Construction/Destruction
HD_Screen::HD_Screen()
{
	HD_Init_Allegro();
	HD_Init_Allegro_Modules();
}

HD_Screen::~HD_Screen()
{
	HDScreen = NULL;
	al_destroy_display(hdDisplay);
}

bool HD_Screen::bHD_Started = false;
void HD_Screen::HD_StartMainLoop()
{
	if (!bHD_Started)
	{
		bHD_Started = true;
		HD_Main_Loop();
	}
	else
	{
		al_show_native_message_box(hdDisplay, "Warning!", "",
			"You're trying to start the main loop again! Won't do that.", NULL, ALLEGRO_MESSAGEBOX_ERROR);
	}
}

void HD_Screen::HD_Dispose()
{
	delete HDResources;

	al_shutdown_font_addon();
	al_shutdown_ttf_addon();
	al_shutdown_image_addon();
	al_shutdown_primitives_addon();

	al_uninstall_mouse();
	al_uninstall_keyboard();

	delete this;
}

//=================
// Common functions
//=================

void HD_Screen:: hd_setResolution(int width, int height)
{
	nScreenW = width;
	nScreenH = height;
	app->GetOptions()->SetOptionValue("graphics_screenwidth", nScreenW);
	app->GetOptions()->SetOptionValue("graphics_screenheight", nScreenH);
	app->GetOptions()->Save(NULL);

	al_resize_display(hdDisplay, nScreenW, nScreenH);
}

void HD_Screen::hd_setFrameless(bool bIsFrameless)
{
	bFrameless = bIsFrameless;
	app->GetOptions()->SetOptionValue("graphics_fullscreen", bIsFrameless);
	app->GetOptions()->Save(NULL);

	al_set_display_flag(hdDisplay, ALLEGRO_FRAMELESS, bIsFrameless);
	al_set_display_flag(hdDisplay, ALLEGRO_FULLSCREEN_WINDOW, bIsFrameless);

	al_set_window_position(hdDisplay, 0, 0);

	hd_initAnchors();	//reset anchors
}

//scaling functions
float HD_Screen::hd_scaleByWidth(float value)
{
	float origW = 1920; //the base W resolution of the UI
	value = (value / origW) * nScreenW;
	return value;
}

float HD_Screen::hd_scaleByHeight(float value)
{
	float origH = 1080; //the base H resolution of the UI
	value = (value / origH) * nScreenH;
	return value;
}

//layout functions
void HD_Screen::hd_setNewLayout(HD_UI_Container *newLayout)
{
	if (currentLayout)
		currentLayout->Clear();

	currentLayout = newLayout;
	currentLayout->Create();
}

HD_UI_Container* HD_Screen::hd_getCurrentLayout()
{
	return currentLayout;
}


//=============================================
// Sets up the screen anchors
//=============================================
void HD_Screen::hd_initAnchors()
{
	anchors.tl.x = 0;	//x
	anchors.tl.y = 0;	//y

	anchors.tc.x = nScreenW / 2;
	anchors.tc.y = 0;

	anchors.tr.x = nScreenW;
	anchors.tr.y = 0;

	anchors.bl.x = 0;
	anchors.bl.y = nScreenH;

	anchors.bc.x = nScreenW / 2;
	anchors.bc.y = nScreenH;

	anchors.br.x = nScreenW;
	anchors.br.y = nScreenH;

	anchors.lc.x = 0;
	anchors.lc.y = nScreenH / 2;

	anchors.c.x = nScreenW / 2;
	anchors.c.y = nScreenH / 2;

	anchors.rc.x = nScreenW;
	anchors.rc.y = nScreenH / 2;

	anchors.w13 = nScreenW / 3;
	anchors.w23 = (nScreenW * 2) / 3;

	anchors.h13 = nScreenH / 3;
	anchors.h23 = (nScreenH * 2) / 3;
}

HD_Screen *HDScreen;