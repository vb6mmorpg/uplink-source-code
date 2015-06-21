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
#include "UI_Layouts\HD_Root.h"

//=======================================================
//Private:
//=======================================================

void HD_Screen:: HD_Init_Allegro()
{
	hdDisplay = NULL;
	ALLEGRO_DISPLAY_MODE displayMode;
	al_get_display_mode(al_get_num_display_modes() - 1, &displayMode);

	nScreenW = 0;
	nScreenH = 0;
	bFrameless = false;
	refreshRate = 0;

	//For the first run, set the maximum available resolution and fullscreen windowed
	//if not, get the settings in options
	if (app->GetOptions()->IsOptionEqualTo("game_firsttime", 1))
	{
		nScreenW = displayMode.width;
		nScreenH = displayMode.height;
		bFrameless = true;
		refreshRate = displayMode.refresh_rate;

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
		if (refreshRate == -1) refreshRate = displayMode.refresh_rate;
	}

	//set fullscreen/windowed mode 
	if (bFrameless)
	{
		al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
		al_set_new_display_flags(ALLEGRO_FRAMELESS);
	}
	else al_set_new_display_flags(ALLEGRO_WINDOWED);

	al_set_new_display_refresh_rate(refreshRate);

	//multisampling & other screen options
	al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
	al_set_new_display_option(ALLEGRO_SAMPLES, 4, ALLEGRO_SUGGEST);
	al_set_new_display_option(ALLEGRO_COLOR_SIZE, 32, ALLEGRO_REQUIRE);
	al_set_new_bitmap_flags(ALLEGRO_MAG_LINEAR | ALLEGRO_MIN_LINEAR);
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
	mouse = std::make_shared<HD_Mouse>();

	//Root object and first screen
	root = std::make_shared<HD_Root>();
	root->Create();

	//debug FPS
	bool debugBFPS = false;
	int debugFPSCount = 0;
	int debugFPS = 0;
	double debugFPSTime = 0;

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

			//update the Uplink stuff
			app->Update();
			//SgUpdate();

			//update members
			mouse->Update();
			deltaTime = al_get_timer_speed(timer);
			debugFPSTime += deltaTime;

			//update current layout
			if (root)
				root->Update();
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

		//DEBUG INFO DISPLAY
		if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
		{
			if (ev.keyboard.keycode == ALLEGRO_KEY_F1)
				DebugInfo = DebugInfo == 4 ? 0 : DebugInfo + 1;

			if (ev.keyboard.keycode == ALLEGRO_KEY_F2)
				debugBFPS = !debugBFPS;
		}

		//Rendering
		if (redraw && al_is_event_queue_empty(event_queue))
		{
			redraw = false;
			//redraw current layout
			if (root)
				root->Draw();

			mouse->Draw();

			//debug FPS
			debugFPSCount++;
			if (debugFPSTime > 1)
			{
				debugFPS = debugFPSCount;
				debugFPSCount = 0;
				debugFPSTime = 0;
			}
			if (debugBFPS)
				al_draw_textf(HDResources->debugFont, al_map_rgb(255, 255, 0), 10.0f, 10.0f, 0, "FPS: %i", debugFPS);

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
	root->Clear();
	HDResources.reset();

	al_shutdown_font_addon();
	al_shutdown_ttf_addon();
	al_shutdown_image_addon();
	al_shutdown_primitives_addon();

	al_uninstall_mouse();
	al_uninstall_keyboard();

	HDScreen.reset();
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

	root->Rescale();
}

void HD_Screen::hd_setFrameless(bool bIsFrameless)
{
	bFrameless = bIsFrameless;
	app->GetOptions()->SetOptionValue("graphics_fullscreen", bIsFrameless);
	app->GetOptions()->Save(NULL);

	al_set_display_flag(hdDisplay, ALLEGRO_FRAMELESS, bIsFrameless);
	al_set_display_flag(hdDisplay, ALLEGRO_FULLSCREEN_WINDOW, bIsFrameless);

	al_set_window_position(hdDisplay, 0, 0);
}

std::unique_ptr<HD_Screen> HDScreen = nullptr;