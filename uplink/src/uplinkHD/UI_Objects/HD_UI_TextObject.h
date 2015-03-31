//========================================
// This creates single or multiline text
// objects.
//========================================

#ifndef HD_UI_TEXTOBJECT_H
#define HD_UI_TEXTOBJECT_H

#include <functional>
#include <string>
#include <vector>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include "HD_UI_Object.h"

class HD_UI_TextObject : public HD_UI_Object
{
private:

	std::string textString;
	std::vector<std::string> multilineStrings;
	float lineHeight;
	float lineOffset;	//offset between multilines

	ALLEGRO_COLOR textColor = al_map_rgba(255, 0, 255, 255);
	ALLEGRO_FONT *textFont = NULL;
	int alignment = 0;

protected:

	//Drawing functions
	std::function<void()>drawTextObject;

	void DrawTextGfx();
	void DrawMultilineTextGfx();

public:

	//Creation functions
	HD_UI_TextObject();

	//Singleline text object
	HD_UI_TextObject(char *objectName, int index, float fX, float fY, char *text,
		ALLEGRO_FONT *font, ALLEGRO_COLOR color, int align, HD_UI_Container *newParent);

	//Multiline text with MAX width object
	HD_UI_TextObject(char *objectName, int index, float fX, float fY, char *text,
		ALLEGRO_FONT *font, ALLEGRO_COLOR color, int align, float maxWidth, float lineHeight, HD_UI_Container *newParent);

	//Breaks a given string into multiple lines if its width > maxWidth
	//Returns NumLines
	int breakString(std::string text, ALLEGRO_FONT *font, float maxWidth);

	//Destruction
	~HD_UI_TextObject() { }

	void setTextColor(ALLEGRO_COLOR newColor);

	//general functions
	void Draw();
	void Clear();
};

#endif