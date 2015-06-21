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
	hd_loadAtlasImage("misc_atlas.png");

	palette = std::make_unique<HD_ColorPalettes>();
}

HD_Resources::~HD_Resources()
{
	hd_destroyFonts();
	hd_removeAllImages();

	al_destroy_path(appPath);
}

// Image functions
//================

bool HD_Resources::hd_loadImage(const char *imageName)
{
	std::string imagePath = "graphics/";
	imagePath.append(imageName);
	ALLEGRO_BITMAP *image = NULL;
	std::unique_ptr<SingleImage> bitmap = std::make_unique<SingleImage>();

	if (singleImages.size() < 8)
		image = al_load_bitmap(imagePath.c_str());
	else
		al_show_native_message_box(NULL, "Warning!", "", "You reached the maximum images allowed!", NULL, ALLEGRO_MESSAGEBOX_WARN);

	if (image)
	{
		bitmap->image = image;
		bitmap->name = imageName;
		singleImages.push_back(nullptr);
		singleImages.back() = std::move(bitmap);
		return true;
	}
	else
	{
		al_show_native_message_box(NULL, "Warning!", "", "Couldn't find the image.", NULL, ALLEGRO_MESSAGEBOX_WARN);
		return false;
	}
}

bool HD_Resources::hd_loadAtlasImage(const char *atlasName)
{
	std::string imagePath = "graphics/";
	imagePath.append(atlasName);
	ALLEGRO_BITMAP *image = NULL;
	std::unique_ptr<AtlasImage> atlasImage = std::make_unique<AtlasImage>();

	if (atlasImages.size() < 8)
		image = al_load_bitmap(imagePath.c_str());
	else
		al_show_native_message_box(NULL, "Warning!", "", "You reached the maximum images allowed!", NULL, ALLEGRO_MESSAGEBOX_WARN);

	if (image)
	{
		atlasImage->image = image;
		atlasImage->name = atlasName;

		ALLEGRO_BITMAP *subImage = NULL;

		std::string xmlPath = imagePath.substr(0, imagePath.size() - 3) + "xml";
		tinyxml2::XMLDocument atlasXML;
		atlasXML.LoadFile(xmlPath.c_str());

		int ix, iy, iw, ih = 0;
		std::string sName;
		
		tinyxml2::XMLElement *textureElement = atlasXML.FirstChildElement("TextureAtlas")->FirstChildElement("SubTexture");

		do {
			sName = textureElement->Attribute("name");
			textureElement->QueryIntAttribute("x", &ix);
			textureElement->QueryIntAttribute("y", &iy);
			textureElement->QueryIntAttribute("width", &iw);
			textureElement->QueryIntAttribute("height", &ih);

			subImage = al_create_sub_bitmap(atlasImage->image, ix, iy, iw, ih);

			atlasImage->AddSubImage(subImage, sName);

			textureElement = textureElement->NextSiblingElement("SubTexture");

		} while (textureElement);

		atlasImages.push_back(nullptr);
		atlasImages.back() = std::move(atlasImage);
		return true;
	}
	else
	{
		al_show_native_message_box(NULL, "Warning!", "", "Couldn't find the image.", NULL, ALLEGRO_MESSAGEBOX_WARN);
		return false;
	}
}

// Get Images
//===============

ALLEGRO_BITMAP* HD_Resources::hd_getImage(const char *imageName)
{
	ALLEGRO_BITMAP *image = NULL;

	//Search only if there are already some images loaded
	if (!singleImages.empty())
		for (unsigned int ii = 0; ii < singleImages.size(); ii++)
		{
			if (singleImages[ii]->image)
			{
				if (strcmp(singleImages[ii]->name.c_str(), imageName) == 0)
				{
					image = singleImages[ii]->image;
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

ALLEGRO_BITMAP* HD_Resources::hd_getSubImage(const char *subImageName, const char *atlasName)
{
	ALLEGRO_BITMAP *image = NULL;
	
	if (!atlasImages.empty())
		for (unsigned int ii = 0; ii < atlasImages.size(); ii++)
		{
			if (atlasImages[ii]->image)
			{
				if (strcmp(atlasImages[ii]->name.c_str(), atlasName) == 0)
				{
					image = atlasImages[ii]->GetSubImage(subImageName);
					break;
				}
			}
			else
				break;
		}

	return image;
}

void HD_Resources::hd_removeImage(const char *imageName)
{
	bool bDestroyed = false;

	for (unsigned int ii = 0; ii < singleImages.size(); ii++)
	{
		if (strcmp(singleImages[ii]->name.c_str(), imageName) == 0)
		{
			singleImages.erase(singleImages.begin() + ii);
			bDestroyed = true;
			break;
		}
	}

	for (unsigned int ii = 0; ii < atlasImages.size(); ii++)
	{
		if (strcmp(atlasImages[ii]->name.c_str(), imageName) == 0)
		{
			atlasImages.erase(atlasImages.begin() + ii);
			bDestroyed = true;
			break;
		}
	}

	if (!bDestroyed)
		al_show_native_message_box(NULL, "Warning!", "", "Could not delete image!", NULL, ALLEGRO_MESSAGEBOX_WARN);
}

void HD_Resources::hd_removeAllImages()
{
	/*for (unsigned int ii = 0; ii < singleImages.size(); ii++)
		if (singleImages[ii]->image)
			singleImages.erase(singleImages.end() - ii - 1);
	
	for (unsigned int ii = 0; ii < atlasImages.size(); ii++)
		if (atlasImages[ii]->image)
			atlasImages.erase(atlasImages.end() - ii - 1);*/

	singleImages.clear();
	atlasImages.clear();
}

// Font functions
//===============

void HD_Resources::hd_loadFonts()
{
	//loads the text fonts
	//and resizes them according to the current resolution
	float s = 18.0f;
	float m = 24.0f;
	float l = 30.0f;
	float xl = 48.0f;

	float size = (s / 1920) * HDScreen->nScreenW;
	if (size < 16.0f) size = 16.0f;
	font18 = al_load_font("fonts/AeroMaticsLight.ttf", size, 0);

	size = (m / 1920) * HDScreen->nScreenW;
	if (size < 20.0f) size = 20.0f;
	font24 = al_load_font("fonts/AeroMaticsLight.ttf", size, 0);
	
	size = (l / 1920) * HDScreen->nScreenW;
	if (size < 24.0f) size = 24.0f;
	font30 = al_load_font("fonts/AeroMaticsLight.ttf", size, 0);

	size = (xl / 1920) * HDScreen->nScreenW;
	if (size < 32.0f) size = 32.0f;
	font48 = al_load_font("fonts/AeroMaticsLight.ttf", size, 0);

	debugFont = al_load_font("fonts/dungeon.ttf", 10, 0);

	if (!font18 || !font24 || !font30 || !font48)
		al_show_native_message_box(NULL, "Warning!", "", "Could not load fonts!", NULL, ALLEGRO_MESSAGEBOX_WARN);

	//font pre-cache
	al_draw_text(font18, al_map_rgba(0, 0, 0, 0), 0, 0, 0, " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
	al_draw_text(font24, al_map_rgba(0, 0, 0, 0), 0, 0, 0, " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
	al_draw_text(font30, al_map_rgba(0, 0, 0, 0), 0, 0, 0, " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
	al_draw_text(font48, al_map_rgba(0, 0, 0, 0), 0, 0, 0, " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
	al_draw_text(debugFont, al_map_rgba(0, 0, 0, 0), 0, 0, 0, " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~");
}

void HD_Resources::hd_destroyFonts()
{
	al_destroy_font(font18);
	al_destroy_font(font24);
	al_destroy_font(font30);
	al_destroy_font(font48);
	al_destroy_font(debugFont);

	font18 = font24 = font30 = font48 = debugFont = NULL;
}

// Atlas Image struct methods
//===========================

void HD_Resources::AtlasImage::AddSubImage(ALLEGRO_BITMAP *subImage, std::string name)
{
	subImages.push_back(subImage);
	subImageNames.push_back(name);
}

ALLEGRO_BITMAP *HD_Resources::AtlasImage::GetSubImage(const char* name)
{
	ALLEGRO_BITMAP *image = NULL;

	//Search only if there are already some images loaded
	if (!subImages.empty())
		for (unsigned int ii = 0; ii < subImages.size(); ii++)
		{
			if (subImages[ii])
			{
				if (strcmp(subImageNames[ii].c_str(), name) == 0)
				{
					image = subImages[ii];
					break;
				}
			}
			else
				break;
		}

	return image;
}

void HD_Resources::AtlasImage::RemoveAllSubImages()
{
	for (unsigned int ii = 0; ii < subImages.size(); ii++)
		al_destroy_bitmap(subImages[ii]);

	subImages.clear();
}

std::unique_ptr<HD_Resources> HDResources = nullptr;