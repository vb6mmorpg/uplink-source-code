//========================================
// This is where all resources are loaded
// and kept.
// Singleton.
//========================================

#ifndef HD_RESOURCES_H
#define HD_RESOURCES_H

#define MAX_IMAGES 8
//#define DEBUGHD

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

class HD_Resources
{
private:
	ALLEGRO_PATH *appPath;

	// Image files
	//================
	ALLEGRO_BITMAP *imageFiles[MAX_IMAGES];	//loaded image files
	int currentImageFilesIndex = 0;
	const char *imageFilesNames[MAX_IMAGES];

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
	void hd_loadImage(const char *imageName);

	ALLEGRO_BITMAP* hd_getImage(const char *imageName);
	ALLEGRO_BITMAP* hd_getSubImage(const char *subImageName, const char *atlasName);

	void hd_removeImage(const char *imageName);
	void hd_removeAllImages();

	void hd_loadFonts();
	void hd_destroyFonts();

};

extern HD_Resources *HDResources;

#endif