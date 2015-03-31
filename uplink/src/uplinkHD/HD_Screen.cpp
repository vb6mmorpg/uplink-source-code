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
#include "UI_Layouts/HD_UI_TestLayout.h"

//=======================================================
//Private:
//=======================================================

void HD_Screen:: HD_Init_Allegro()
{
	hdDisplay = NULL;
	al_get_display_mode(al_get_num_display_modes() - 1, &display_modes);

	nScreenW = 0;
	nScreenH = 0;
	bFullscreen = false;

	//For the first run, set the maximum available resolution and fullscreen windowed
	//if not, get the settings in options
	if (app->GetOptions()->IsOptionEqualTo("game_firsttime", 0)) //DON'T FORGET TO REPLACE WITH 1!!!
	{
		nScreenW = display_modes.width;
		nScreenH = display_modes.height;
		bFullscreen = true;

		app->GetOptions()->SetOptionValue("graphics_screenwidth", nScreenW);
		app->GetOptions()->SetOptionValue("graphics_screenheight", nScreenH);
		app->GetOptions()->SetOptionValue("graphics_fullscreen", bFullscreen);
		app->GetOptions()->SetOptionValue("game_firsttime", 0);
	}
	else
	{
		nScreenW = app->GetOptions()->GetOptionValue("graphics_screenwidth");
		nScreenH = app->GetOptions()->GetOptionValue("graphics_screenheight");
		bFullscreen = app->GetOptions()->GetOptionValue("graphics_fullscreen");
	}

	//set fullscreen/windowed mode 
	if (bFullscreen)
	{
		al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
		al_set_new_display_flags(ALLEGRO_FRAMELESS);
	}
	else al_set_new_display_flags(ALLEGRO_WINDOWED);

	//multisampling
	al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
	al_set_new_display_option(ALLEGRO_SAMPLES, 4, ALLEGRO_SUGGEST);
	//al_set_new_display_option(ALLEGRO_RENDER_METHOD, 2, ALLEGRO_REQUIRE);

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

	fps = 35;

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
	bool bFinished = false;
	bool redraw = false;

	//SetUp the mouse!
	HDResources->hd_loadImage("graphics/mousePointer.png");
	mouseCursor = al_create_mouse_cursor(HDResources->hd_getImage("graphics/mousePointer.png"), 0, 0);
	al_set_mouse_cursor(hdDisplay, mouseCursor);

	ALLEGRO_EVENT_QUEUE *event_queue = al_create_event_queue();
	ALLEGRO_TIMER *timer = al_create_timer(1.0 / fps);
	ALLEGRO_MOUSE_STATE mouseState;

	al_register_event_source(event_queue, al_get_display_event_source(hdDisplay));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	//al_register_event_source(event_queue, al_get_mouse_event_source());

	al_start_timer(timer);

	HD_UI_TestLayout *testLayout = new HD_UI_TestLayout();
	hd_setNewLayout(testLayout);

	while (!bFinished)
	{
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if (ev.type == ALLEGRO_EVENT_TIMER)
		{
			redraw = true;

			al_get_mouse_state(&mouseState);
			//update current layout
			if (currentLayout)
				currentLayout->Update(&mouseState, al_get_timer_speed(timer));
			
		}
		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
		{
			bFinished = true;
		}

		if (redraw && al_is_event_queue_empty(event_queue))
		{
			redraw = false;
			//redraw current layout
			if (currentLayout)
				currentLayout->Draw();
			else {
				al_draw_line(0, 0, hd_screenAnchor.br[0], hd_screenAnchor.br[1], al_map_rgb(255, 255, 255), 1);
				al_draw_text(HDResources->font18, al_map_rgb(255, 255, 255), nScreenW / 2, nScreenH / 2, ALLEGRO_ALIGN_CENTER, "GREAT SUCCESS! But there's no layout...");
			}

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

	al_resize_display(hdDisplay, nScreenW, nScreenH);
}

void HD_Screen:: hd_setFullscreen(bool bIsFullscreen)
{
	bFullscreen = bIsFullscreen;
	app->GetOptions()->SetOptionValue("graphics_fullscreen", bFullscreen);

	al_set_display_flag(hdDisplay, ALLEGRO_FRAMELESS, bIsFullscreen);
	al_set_display_flag(hdDisplay, ALLEGRO_FULLSCREEN_WINDOW, bIsFullscreen);

	if (!bFullscreen)
	{
		hd_setResolution(1920, 1080);
	}

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
	hd_screenAnchor.tl[0] = 0;	//x
	hd_screenAnchor.tl[1] = 0;	//y

	hd_screenAnchor.tc[0] = nScreenW / 2;
	hd_screenAnchor.tc[1] = 0;

	hd_screenAnchor.tr[0] = nScreenW;
	hd_screenAnchor.tr[1] = 0;

	hd_screenAnchor.bl[0] = 0;
	hd_screenAnchor.bl[1] = nScreenH;

	hd_screenAnchor.bc[0] = nScreenW / 2;
	hd_screenAnchor.bc[1] = nScreenH;

	hd_screenAnchor.br[0] = nScreenW;
	hd_screenAnchor.br[1] = nScreenH;

	hd_screenAnchor.lc[0] = 0;
	hd_screenAnchor.lc[1] = nScreenH / 2;

	hd_screenAnchor.c[0] = nScreenW / 2;
	hd_screenAnchor.c[1] = nScreenH / 2;

	hd_screenAnchor.rc[0] = nScreenW;
	hd_screenAnchor.rc[1] = nScreenH / 2;

	hd_screenAnchor.w13 = nScreenW / 3;
	hd_screenAnchor.w23 = (nScreenW * 2) / 3;

	hd_screenAnchor.h13 = nScreenH / 3;
	hd_screenAnchor.h23 = (nScreenH * 2) / 3;
}

HD_Screen *HDScreen;