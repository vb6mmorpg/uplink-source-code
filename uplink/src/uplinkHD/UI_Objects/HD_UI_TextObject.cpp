// Implementation of the Text UI Object
//=====================================

#include <functional>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#include "../HD_Screen.h"
#include "../HD_Resources.h"
#include "../HD_ColorPalettes.h"

#include "HD_UI_TextObject.h"
#include "HD_UI_Container.h"


//============================
// Protected Drawing Functions
//============================

//Drawing Functions
void HD_UI_TextObject::DrawTextGfx()
{
	/*float imageW = al_get_bitmap_width(glyphTexture);
	float imageH = al_get_bitmap_height(glyphTexture);

	ALLEGRO_COLOR c = palette->addAlphaToColor(al_map_rgb_f(1.0f, 1.0f, 1.0f), drawAlpha);
	float r = (rotation * ALLEGRO_PI) / 180.0f;

	//Images get scaled by the X scale!
	al_draw_tinted_scaled_rotated_bitmap(glyphTexture, c, imageW / 2.0f, imageH / 2.0f, globalX + drawWidth / 2.0f, globalY + drawHeight / 2.0f, globalScaleX, globalScaleX, r, 0);*/

	//Just a single line
	ALLEGRO_COLOR c = palette->addAlphaToColor(textColor, drawAlpha);
	al_draw_text(textFont, c, globalX, globalY, alignment, textString.c_str());
}

void HD_UI_TextObject::DrawMultilineTextGfx()
{
	//Multiline text
	ALLEGRO_COLOR c = palette->addAlphaToColor(textColor, drawAlpha);
	for (unsigned int ii = 0; ii < multilineStrings.size(); ii++)
	{
		al_draw_text(textFont, c, globalX, globalY + ii * (lineHeight + lineOffset), alignment, multilineStrings[ii].c_str());
	}
}

//============================
// Public Creation Functions
//============================

//Singleline text object
//HD_UI_TextObject::HD_UI_TextObject(const char *objectName, int index, float fX, float fY, const char *text,
//	ALLEGRO_FONT *font, ALLEGRO_COLOR color, int align, HD_UI_Container *newParent)
void HD_UI_TextObject::CreateSinglelineText(const char *objectName, float fX, float fY, const char *text,
	ALLEGRO_COLOR color, ALLEGRO_FONT *font, int align)
{
	float fWidth = al_get_text_width(font, text);
	float fHeight = al_get_font_line_height(font);

	//create the texture
	/*glyphTexture = al_create_bitmap(fWidth, fHeight);
	al_set_target_bitmap(glyphTexture);
	al_draw_text(font, color, 0.0f, 0.0f, 0, text);
	al_set_target_backbuffer(al_get_current_display());

	float alignX = 0.0f;

	switch (align)
	{
	case 0:
		alignX = fX;	//left align
		break;

	case 1:
		alignX = fX - fWidth / 2.0f;	//center align
		break;
	
	case 2:
		alignX = fX - fWidth;	//right align
		break;
	}*/

	setObjectProperties(objectName, fX, fY, fWidth, fHeight);

	textString = text;

	textFont = font;
	textColor = color;
	alignment = align;

	drawTextObject = std::bind(&HD_UI_TextObject::DrawTextGfx, this);
}

//Multiline text with MAX width object
//HD_UI_TextObject::HD_UI_TextObject(const char *objectName, int index, float fX, float fY, const char *text,
//	ALLEGRO_FONT *font, ALLEGRO_COLOR color, int align, float maxWidth, float lineOffset, HD_UI_Container *newParent)
void HD_UI_TextObject::CreateMultilineText(const char *objectName, float fX, float fY, const char *text,
	ALLEGRO_COLOR color, float maxWidth, int maxLines, float lineOffset, ALLEGRO_FONT *font, int align)
{
	textString = text;
	HD_UI_TextObject::lineOffset = lineOffset;
	lineHeight = al_get_font_line_height(font);
	breakString(textString, font, maxWidth, maxLines);

	float fWidth = maxWidth;
	float fHeight = multilineStrings.size() * lineHeight + (lineOffset * (multilineStrings.size() - 1));

	setObjectProperties(objectName, fX, fY, fWidth, fHeight);

	textFont = font;
	textColor = color;
	alignment = align;

	drawTextObject = std::bind(&HD_UI_TextObject::DrawMultilineTextGfx, this);
}

//Breaks a given string into multiple lines if its width > maxWidth
int HD_UI_TextObject::breakString(std::string text, ALLEGRO_FONT *font, float maxWidth, int maxLines)
{
	unsigned int numLines = 0;
	unsigned int textLength = text.length();
	unsigned int textSize = 0;
	unsigned int lastSpaceIndex = 0;
	unsigned int prevBreakIndex = 0;
	std::string stringLine;

	for (unsigned int ii = 0; ii < textLength; ii++)
	{
		const char character = text[ii];
		textSize += al_get_text_width(font, &character);

		if (text[ii] == ' ' || text[ii] == '\n')
			lastSpaceIndex = ii;
		
		if (textSize > maxWidth || text[ii] == '\n')
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

		//cap it at maxLines if it's != 0
		if (maxLines > 0 && numLines == maxLines)
		{
			multilineStrings[numLines - 1].append("...");
			break;
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

void HD_UI_TextObject::setText(const char *newText)
{
	textString = newText;
}

void HD_UI_TextObject::setText(const char *newText, float maxWidth, int maxLines)
{
	textString = newText;
	multilineStrings.clear();
	breakString(textString, textFont, maxWidth, maxLines);
}

float HD_UI_TextObject::getTextWidth()
{
	float width = 0.0f;

	//Singleline
	if (!textString.empty())
		width = al_get_text_width(textFont, textString.c_str());

	//Multiline
	if (!multilineStrings.empty())
		for (unsigned int ii = 0; ii < multilineStrings.size(); ii++)
		{
			float w = al_get_text_width(textFont, multilineStrings[ii].c_str());
			width < w ? w : width;
		}

	return width;
}

float HD_UI_TextObject::getTextWidth(int charsNo)
{
	float width = 0.0f;

	//Singleline
	if (!textString.empty())
	{
		std::string part = textString.substr(0, charsNo);
		width = al_get_text_width(textFont, part.c_str());
	}

	//Multiline To-Do

	return width;
}

void HD_UI_TextObject::Draw()
{
	if (isVisible && redraw && isInClippingMask())
	{
		drawTextObject();
		redraw = false;
	}

	if (HDScreen->DebugInfo && isVisible)
	{
		al_draw_rectangle(globalX, globalY, globalX + width, globalY + height, al_map_rgb_f(1.0f, 0.0f, 1.0f), 1.0f);
		std::string sName = "name:" + name;
		al_draw_text(HDResources->debugFont, al_map_rgb_f(1.0f, 0.0f, 1.0f), globalX, globalY + height + 2.0f, 0, sName.c_str());
	}
}

void HD_UI_TextObject::Clear()
{
	//if (glyphTexture)
	//	al_destroy_bitmap(glyphTexture);

	HD_UI_Object::Clear();
}