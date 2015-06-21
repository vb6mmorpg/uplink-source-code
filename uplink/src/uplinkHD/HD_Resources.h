//========================================
// This is where all resources are loaded
// and kept.
// Singleton.
//========================================

#ifndef HD_RESOURCES_H
#define HD_RESOURCES_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#include <vector>
#include <string>
#include <memory>

class HD_Resources
{
public:

	//constructor
	HD_Resources();
	~HD_Resources();

	//font pointers
	ALLEGRO_FONT *debugFont;
	ALLEGRO_FONT *font18;
	ALLEGRO_FONT *font24;
	ALLEGRO_FONT *font30;
	ALLEGRO_FONT *font48;

	//resource loading/removal functions
	bool hd_loadImage(const char *imageName);
	bool hd_loadAtlasImage(const char *atlasName);

	ALLEGRO_BITMAP* hd_getImage(const char *imageName);
	ALLEGRO_BITMAP* hd_getSubImage(const char *subImageName, const char *atlasName);

	void hd_removeImage(const char *imageName);
	void hd_removeAllImages();

	void hd_loadFonts();
	void hd_destroyFonts();

	//GLOBALS
	bool USECHEATS = true;

private:
	ALLEGRO_PATH *appPath;

	// Image files
	//================

	//A single image file
	struct SingleImage
	{
		ALLEGRO_BITMAP *image;
		std::string name;

		SingleImage()
		{
			image = NULL;
		}

		~SingleImage()
		{
			al_destroy_bitmap(image);
			image = NULL;
		}
	};

	//An atlas image file
	struct AtlasImage
	{
		ALLEGRO_BITMAP *image;
		std::string name;

		std::vector<ALLEGRO_BITMAP *> subImages;
		std::vector<std::string> subImageNames;

		AtlasImage()
		{
			image = NULL;
		}

		~AtlasImage()
		{
			RemoveAllSubImages();
			subImageNames.clear();
			al_destroy_bitmap(image);
			image = NULL;
		}

		void AddSubImage(ALLEGRO_BITMAP *subImage, std::string name);
		ALLEGRO_BITMAP *GetSubImage(const char* name);
		void RemoveAllSubImages();
	};

	std::vector<std::unique_ptr<SingleImage>> singleImages;
	std::vector<std::unique_ptr<AtlasImage>> atlasImages;

	// Sound files
	//=================
	//TO-DO if needed.

};

extern std::unique_ptr<HD_Resources> HDResources;

#endif