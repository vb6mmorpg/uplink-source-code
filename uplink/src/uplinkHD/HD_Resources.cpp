// Implementation of Resources
//============================

#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_native_dialog.h>
#include "tinyxml2.h"

#include "HD_Resources.h"
#include "HD_Screen.h"

HD_Resources::HD_Resources()
{
	hd_loadFonts();

	for (unsigned ii = 0; ii < MAX_IMAGES; ii++)
	{
		imageFiles[ii] = NULL;
	}
}

HD_Resources* HD_Resources::resources = NULL;
HD_Resources* HD_Resources::HD_GetResources()
{
	if (!resources)
	{
		resources = new HD_Resources();
		return resources;
	}
	else
	{
		return resources;
	}
}

HD_Resources::~HD_Resources()
{
	hd_destroyFonts();
	hd_removeAllImages();

	resources = NULL;
}

// Image functions
//================

void HD_Resources::hd_loadImage(const char *imageName)
{
	if (currentImageFilesIndex < MAX_IMAGES)
	{
		ALLEGRO_BITMAP *image = al_load_bitmap(imageName);
		imageFiles[currentImageFilesIndex] = image;
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

	for (unsigned ii = 0; ii < MAX_IMAGES; ii++)
	{
		if (strcmp(imageFilesNames[ii], imageName))
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
ALLEGRO_BITMAP* HD_Resources::hd_getSubImage(char *subImageName, char *atlasName)
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
	float size = 18.0f;
	//HD_Screen::HD_GetScreen()->hd_scaleByWidth(size);
	textFont18 = al_load_font("uplinkHD/fonts/Aero Matics Light.ttf", size, 0);

	size = 24.0f;
	//HD_Screen::HD_GetScreen()->hd_scaleByWidth(size);
	textFont24 = al_load_font("uplinkHD/fonts/Aero Matics Light.ttf", size, 0);
	
	size = 30.0f;
	//HD_Screen::HD_GetScreen()->hd_scaleByWidth(size);
	textFont30 = al_load_font("uplinkHD/fonts/Aero Matics Light.ttf", size, 0);
}

void HD_Resources::hd_destroyFonts()
{
	al_destroy_font(textFont18);
	al_destroy_font(textFont24);
	al_destroy_font(textFont30);
}

//HD_Resources *UplinkHDResources = NULL;