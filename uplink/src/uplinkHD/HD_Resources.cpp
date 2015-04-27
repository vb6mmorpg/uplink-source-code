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

#include "HD_ColorPalettes.h"

HD_Resources::HD_Resources()
{
	appPath = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
	al_append_path_component(appPath, "uplinkHD");
	al_change_directory(al_path_cstr(appPath, '/'));

	hd_loadFonts();

	palette = new HD_ColorPalettes();
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

bool HD_Resources::hd_loadImage(const char *imageName)
{
	std::string imagePath = "graphics/";
	imagePath.append(imageName);
	ALLEGRO_BITMAP *image = NULL;
	Bitmap *bitmap = new Bitmap();

	if (imageFiles.size() < MAX_IMAGES)
		image = al_load_bitmap(imagePath.c_str());
	else
		al_show_native_message_box(NULL, "Warning!", "", "You reached the maximum images allowed!", NULL, ALLEGRO_MESSAGEBOX_WARN);

	if (image)
	{
		bitmap->image = image;
		bitmap->name = imageName;
		imageFiles.push_back(bitmap);
		return true;
	}
	else
	{
		al_show_native_message_box(NULL, "Warning!", "", "Couldn't find the image.", NULL, ALLEGRO_MESSAGEBOX_WARN);
		return false;
	}
}

ALLEGRO_BITMAP* HD_Resources::hd_getImage(const char *imageName)
{
	ALLEGRO_BITMAP *image = NULL;

	//Search only if there are already some images loaded
	if (!imageFiles.empty())
		for (unsigned int ii = 0; ii < imageFiles.size(); ii++)
		{
			if (imageFiles[ii]->image)
			{
				if (strcmp(imageFiles[ii]->name.c_str(), imageName) == 0)
				{
					image = imageFiles[ii]->image;
					break;
				}
			}
			else
				break;
		}

	//If it didn't find it, load it
	if (!image)
	{
		hd_loadImage(imageName);
		image = hd_getImage(imageName);
	}

	return image;
}

// atlasName = atlas xml document
ALLEGRO_BITMAP* HD_Resources::hd_getSubImage(const char *subImageName, const char *atlasName)
{
	ALLEGRO_BITMAP *image = NULL;
	ALLEGRO_BITMAP *atlas = NULL;
	std::string xmlPath = "graphics/";
	xmlPath.append(atlasName);

	tinyxml2::XMLDocument atlasXML;
	atlasXML.LoadFile(xmlPath.c_str());

	const char *atlasImageName = atlasXML.FirstChildElement("TextureAtlas")->Attribute("imagePath");
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

	for (unsigned int ii = 0; ii < imageFiles.size(); ii++)
	{
		if (strcmp(imageFiles[ii]->name.c_str(), imageName))
		{
			imageFiles.erase(imageFiles.begin() + ii);
			bDestroyed = true;
			break;
		}
	}

	if (!bDestroyed)
		al_show_native_message_box(NULL, "Warning!", "", "Could not delete image!", NULL, ALLEGRO_MESSAGEBOX_WARN);
}

void HD_Resources::hd_removeAllImages()
{
	for (unsigned int ii = 0; ii < imageFiles.size(); ii++)
	{
		if (imageFiles[ii]->image)
			imageFiles.erase(imageFiles.begin() + ii);
	}
}

// Font functions
//===============

void HD_Resources::hd_loadFonts()
{
	//loads the text fonts
	//and resizes them according to the current resolution
	float size = HDScreen->hd_scaleByWidth(18.0f);
	if (size < 16.0f) size = 16.0f;
	textFont18 = al_load_font("fonts/AeroMaticsLight.ttf", size, 0);
	font18 = textFont18;

	size = HDScreen->hd_scaleByWidth(24.0f);
	if (size < 20.0f) size = 20.0f;
	textFont24 = al_load_font("fonts/AeroMaticsLight.ttf", size, 0);
	font24 = textFont24;
	
	size = HDScreen->hd_scaleByWidth(30.0f);
	if (size < 24.0f) size = 24.0f;
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