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

class HD_Resources
{
private:
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

	//constructor
	static HD_Resources *resources;
	HD_Resources();

public:

	static HD_Resources* HD_GetResources();
	~HD_Resources();

	void hd_loadImage(const char *imageName);

	ALLEGRO_BITMAP* hd_getImage(const char *imageName);
	ALLEGRO_BITMAP* hd_getSubImage(char *subImageName, char *atlasName);

	void hd_removeImage(const char *imageName);
	void hd_removeAllImages();

	void hd_loadFonts();	//this gets called at startup
	void hd_destroyFonts();

};

//extern HD_Resources *UplinkHDResources;

#endif