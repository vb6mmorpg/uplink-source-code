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
#include "../HD_Resources.h"

class HD_UI_TextObject : public HD_UI_Object
{
private:

	std::string textString;
	std::vector<std::string> multilineStrings;
	float lineHeight;
	float lineOffset;	//offset between multilines

	//ALLEGRO_BITMAP* glyphTexture = NULL;
	ALLEGRO_COLOR textColor = al_map_rgba(255, 0, 255, 255);
	ALLEGRO_FONT *textFont = NULL;
	int alignment = 0;

protected:

	//Drawing functions
	std::function<void()>drawTextObject;

	void DrawTextGfx();
	void DrawMultilineTextGfx();

	//Breaks a given string into multiple lines if its width > maxWidth
	//Returns NumLines
	int breakString(std::string text, ALLEGRO_FONT *font, float maxWidth, int maxLines);

public:

	//Creation/Destruction
	HD_UI_TextObject() {}
	~HD_UI_TextObject() {}

	//Singleline text object
	void CreateSinglelineText(const char *objectName, float fX, float fY, const char *text,
		ALLEGRO_COLOR color, ALLEGRO_FONT *font = HDResources->font24, int align = ALLEGRO_ALIGN_LEFT);

	//Multiline text with MAX width object
	void CreateMultilineText(const char *objectName, float fX, float fY, const char *text,
		ALLEGRO_COLOR color, float maxWidth, int maxLines, float lineOffset,
		ALLEGRO_FONT *font = HDResources->font24, int align = ALLEGRO_ALIGN_LEFT);

	//Text object helper functions

	//Gets the full text
	const char* getText() { return textString.c_str(); }
	//Sets the text color
	void setTextColor(ALLEGRO_COLOR newColor);
	//Sets the alignment
	void setTextAlign(int align) { alignment = align; }
	//Sets a new singleline text
	void setText(const char *newText);
	//Sets a new multiline text
	void setText(const char *newText, float maxWidth, int maxLines);
	//Gets the whole width of the text
	float getTextWidth();
	//Gets the width of the first charsNo characters. Starts from 0. Used by TextInput obj mainly.
	float getTextWidth(int charsNo);
	//Gets the number of lines
	int getTextNumLines() { return multilineStrings.size(); }

	//general functions
	void Draw();
	void Clear();
};

#endif