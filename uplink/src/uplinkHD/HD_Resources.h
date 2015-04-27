//========================================
// This is where all resources are loaded
// and kept.
// Singleton.
//========================================

#ifndef HD_RESOURCES_H
#define HD_RESOURCES_H

#define MAX_IMAGES 8

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include <vector>
#include <string>

class HD_Resources
{
private:
	ALLEGRO_PATH *appPath;

	// Image files
	//================
	struct Bitmap
	{
		ALLEGRO_BITMAP *image;
		std::string name;

		Bitmap()
		{
			image = NULL;
		}

		~Bitmap()
		{
			al_destroy_bitmap(image);
			image = NULL;
		}
	};

	std::vector<Bitmap*> imageFiles;

	// Font files
	//=================
	ALLEGRO_FONT *textFont18;
	ALLEGRO_FONT *textFont24;
	ALLEGRO_FONT *textFont30;

	// Sound files
	//=================
	//TO-DO if needed.

public:

	//constructor
	HD_Resources();
	~HD_Resources();

	//font pointers
	ALLEGRO_FONT *font18;
	ALLEGRO_FONT *font24;
	ALLEGRO_FONT *font30;

	//resource loading functions
	bool hd_loadImage(const char *imageName);

	ALLEGRO_BITMAP* hd_getImage(const char *imageName);
	ALLEGRO_BITMAP* hd_getSubImage(const char *subImageName, const char *atlasName);

	void hd_removeImage(const char *imageName);
	void hd_removeAllImages();

	void hd_loadFonts();
	void hd_destroyFonts();

};

extern HD_Resources *HDResources;

#endif