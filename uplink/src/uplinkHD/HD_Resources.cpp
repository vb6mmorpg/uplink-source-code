// Implementation of Resources
//============================

#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>
#include "tinyxml2.h"

#include "HD_Resources.h"
#include "HD_Screen.h"

HD_Resources::HD_Resources()
{
	appPath = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
	al_append_path_component(appPath, "uplinkHD");
	al_change_directory(al_path_cstr(appPath, '/'));

	//al_show_native_message_box(NULL, "Info.", "App path.", al_path_cstr(appPath, '/'), NULL, ALLEGRO_MESSAGEBOX_WARN);

	hd_loadFonts();

	for (unsigned ii = 0; ii < MAX_IMAGES; ii++)
	{
		imageFiles[ii] = NULL;
		imageFilesNames[ii] = NULL;
	}
}

HD_Resources::~HD_Resources()
{
	hd_destroyFonts();
	hd_removeAllImages();

	al_destroy_path(appPath);

	HDResources = NULL;
}

// Image functions
//================

void HD_Resources::hd_loadImage(const char *imageName)
{
	if (currentImageFilesIndex < MAX_IMAGES)
	{
		ALLEGRO_BITMAP *image = al_load_bitmap(imageName);
		imageFiles[currentImageFilesIndex] = image;
		//To-Do: change the imageFilesNames index to the actual file name, not the full path!
		imageFilesNames[currentImageFilesIndex] = imageName;
		currentImageFilesIndex++;
	}
	else
	{
		al_show_native_message_box(NULL, "Warning!", "", "Could not find image!", NULL, ALLEGRO_MESSAGEBOX_WARN);
	}
}

ALLEGRO_BITMAP* HD_Resources::hd_getImage(const char *imageName)
{
	ALLEGRO_BITMAP *image = NULL;

	for (unsigned int ii = 0; ii < MAX_IMAGES; ii++)
	{
		if (strcmp(imageFilesNames[ii], imageName) == 0)
		{
			image = imageFiles[ii];
			break;
		}
	}

	if (!image)
		al_show_native_message_box(NULL, "Warning!", "", "Could not find image!", NULL, ALLEGRO_MESSAGEBOX_WARN);

	return image;
}

// atlasName = atlas xml document
ALLEGRO_BITMAP* HD_Resources::hd_getSubImage(const char *subImageName, const char *atlasName)
{
	ALLEGRO_BITMAP *image = NULL;
	ALLEGRO_BITMAP *atlas = NULL;

	tinyxml2::XMLDocument atlasXML;
	atlasXML.LoadFile(atlasName);

	const char *atlasImageName = atlasXML.FirstChildElement("TextureAtlas")->Attribute("imagePath");
	hd_loadImage(atlasImageName);
	atlas = hd_getImage(atlasImageName);

	int ix = 0;
	int iy = 0;
	int iw = 10;
	int ih = 10;
	bool found = false;

	tinyxml2::XMLElement *textureElement = atlasXML.FirstChildElement("TextureAtlas")->FirstChildElement("SubTexture");

	do {

		if (strcmp(textureElement->Attribute("name"), subImageName) == 0)
		{
			textureElement->QueryIntAttribute("x", &ix);
			textureElement->QueryIntAttribute("y", &iy);
			textureElement->QueryIntAttribute("width", &iw);
			textureElement->QueryIntAttribute("height", &ih);

			found = true;
		}
		else
		{
			textureElement = textureElement->NextSiblingElement("SubTexture");

			if (!textureElement){
				break;
			}

		}

	} while (!found);

	if (found)
		image = al_create_sub_bitmap(atlas, ix, iy, iw, ih);
	else
		al_show_native_message_box(NULL, "Warning!", "", "Could not find image in atlas!", NULL, ALLEGRO_MESSAGEBOX_WARN);

	return image;
}

void HD_Resources::hd_removeImage(const char *imageName)
{
	bool bDestroyed = false;

	for (unsigned ii = 0; ii < MAX_IMAGES; ii++)
	{
		if (strcmp(imageFilesNames[ii], imageName))
		{
			al_destroy_bitmap(imageFiles[ii]);
			imageFiles[ii] = NULL;
			imageFilesNames[ii] = NULL;
			bDestroyed = true;
			break;
		}
	}

	if (!bDestroyed)
		al_show_native_message_box(NULL, "Warning!", "", "Could not delete image!", NULL, ALLEGRO_MESSAGEBOX_WARN);
}

void HD_Resources::hd_removeAllImages()
{
	for (unsigned ii = 0; ii < MAX_IMAGES; ii++)
	{
		if (imageFiles[ii]) {
			al_destroy_bitmap(imageFiles[ii]);
			imageFiles[ii] = NULL;
		}
	}
}

// Font functions
//===============

void HD_Resources::hd_loadFonts()
{
	//loads the text fonts
	//and resizes them according to the current resolution
	float size = HDScreen->hd_scaleByWidth(18.0f);
	textFont18 = al_load_font("fonts/AeroMaticsLight.ttf", size, 0);
	font18 = textFont18;

	size = HDScreen->hd_scaleByWidth(24.0f);
	textFont24 = al_load_font("fonts/AeroMaticsLight.ttf", size, 0);
	font24 = textFont24;
	
	size = HDScreen->hd_scaleByWidth(30.0f);
	textFont30 = al_load_font("fonts/AeroMaticsLight.ttf", size, 0);
	font30 = textFont30;

	if (!textFont18 || !textFont24 || !textFont30)
		al_show_native_message_box(NULL, "Warning!", "", "Could not load fonts!", NULL, ALLEGRO_MESSAGEBOX_WARN);
}

void HD_Resources::hd_destroyFonts()
{
	font18 = font24 = font30 = NULL;

	al_destroy_font(textFont18);
	al_destroy_font(textFont24);
	al_destroy_font(textFont30);
}

HD_Resources *HDResources = NULL;