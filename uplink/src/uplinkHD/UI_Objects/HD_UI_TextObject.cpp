// Implementation of the Text UI Object
//=====================================

#include <functional>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#include "../HD_Resources.h"

#include "HD_UI_TextObject.h"


//============================
// Protected Drawing Functions
//============================

//Drawing Functions
void HD_UI_TextObject::DrawTextGfx()
{
	//Just a single line
	al_draw_text(textFont, textColor, x, y, alignment, textString.c_str());
}

void HD_UI_TextObject::DrawMultilineTextGfx()
{
	//Multiline text
	for (unsigned int ii = 0; ii < multilineStrings.size(); ii++)
	{
		al_draw_text(textFont, textColor, x, y + ii * (lineHeight + lineOffset), alignment, multilineStrings[ii].c_str());
	}

#ifdef DEBUGHD
	al_draw_rectangle(x, y, x + width, y + multilineStrings.size() * (lineHeight + lineOffset), al_map_rgb(120, 120, 120), 0.0f);
#endif
}

//============================
// Public Creation Functions
//============================

//Singleline text object
HD_UI_TextObject::HD_UI_TextObject(char *objectName, int index, float fX, float fY, char *text,
	ALLEGRO_FONT *font, ALLEGRO_COLOR color, int align, HD_UI_Container *newParent)
{
	float fWidth = al_get_text_width(font, text);
	float fHeight = al_get_font_line_height(font);

	setObjectProperties(objectName, fX, fY, fWidth, fHeight, newParent, index);

	textString = text;

	textFont = font;
	textColor = color;
	alignment = align;

	drawTextObject = std::bind(&HD_UI_TextObject::DrawTextGfx, this);
}

//Multiline text with MAX width object
HD_UI_TextObject::HD_UI_TextObject(char *objectName, int index, float fX, float fY, char *text,
	ALLEGRO_FONT *font, ALLEGRO_COLOR color, int align, float maxWidth, float lineOffset, HD_UI_Container *newParent)
{
	textString = text;
	HD_UI_TextObject::lineOffset = lineOffset;
	lineHeight = al_get_font_line_height(font);
	breakString(textString, font, maxWidth);

	float fWidth = maxWidth;
	float fHeight = multilineStrings.size() * lineHeight + (lineOffset * (multilineStrings.size() - 1));

	setObjectProperties(objectName, fX, fY, fWidth, fHeight, newParent, index);

	textFont = font;
	textColor = color;
	alignment = align;

	textString.clear();

	drawTextObject = std::bind(&HD_UI_TextObject::DrawMultilineTextGfx, this);
}

//Breaks a given string into multiple lines if its width > maxWidth
int HD_UI_TextObject::breakString(std::string text, ALLEGRO_FONT *font, float maxWidth)
{
	unsigned int numLines = 0;
	unsigned int textLength = text.length();
	unsigned int textSize = 0;
	unsigned int lastSpaceIndex = 0;
	unsigned int prevBreakIndex = 0;
	std::string stringLine;

	//TO-DO: This isn't working! :(
	for (unsigned int ii = 0; ii < textLength; ii++)
	{
		const char character = text[ii];
		textSize += al_get_text_width(font, &character);

		if (text[ii] == ' ')
			lastSpaceIndex = ii;
		
		if (textSize > maxWidth)
		{
			//if maxWidth is too small for the whole word, break in the middle of the word
			//if not, break it at the lastSpaceIndex
			if (lastSpaceIndex != 0)
			{
				stringLine = text.substr(prevBreakIndex, lastSpaceIndex - prevBreakIndex);
				prevBreakIndex = lastSpaceIndex + 1;
				ii = lastSpaceIndex;
				lastSpaceIndex = 0;
			}
			else
			{
				stringLine = text.substr(prevBreakIndex, ii- prevBreakIndex);
				prevBreakIndex = ii;
			}

			//reset the size so we can calculate the next line
			textSize = 0;
		}

		//if there are any more characters, add them
		if (ii == textLength - 1)
		{
			stringLine = text.substr(prevBreakIndex, ii - prevBreakIndex + 1);
		}

		if (!stringLine.empty())
		{
			multilineStrings.push_back(stringLine);
			stringLine.clear();
			numLines++;
		}
	}

	return numLines;
}

//============================
// General Functions
//============================

void HD_UI_TextObject::setTextColor(ALLEGRO_COLOR newColor)
{
	textColor = newColor;
}

void HD_UI_TextObject::Draw()
{
	if (isVisible)
		drawTextObject();
}

void HD_UI_TextObject::Clear()
{
	multilineStrings.clear();
	textString.clear();
	delete this;
}