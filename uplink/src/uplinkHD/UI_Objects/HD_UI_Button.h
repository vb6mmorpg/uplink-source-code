//========================================
// Finally some input!
// This creates clickable buttons including
// normal, radio, switch, etc.
// TO-DO: Even text input??
//========================================

#ifndef HD_UI_BUTTON_H
#define HD_UI_BUTTON_H

#include <string>
#include <functional>
#include <allegro5/allegro.h>

#include "HD_UI_Object.h"
#include "HD_UI_Container.h"
#include "HD_UI_GraphicsObject.h"
#include "HD_UI_TextObject.h"

class HD_UI_Button : public HD_UI_Container
{
	//the hitzone is in % of objects w/h
	//and is calculated from its center
	struct Hitzone
	{
		float width;
		float height;
	};

	enum ButtonState
	{
		standard,
		over,
		clicked,
		released
	};

private:
	ALLEGRO_BITMAP *standardImage = NULL;
	ALLEGRO_BITMAP *hoverImage = NULL;
	ALLEGRO_BITMAP *clickImage = NULL;

	//Colors for primitive based buttons
	//0 - standard; 1 - hover; 2 - clicked;
	//3 - standardText; 4 - clickedText; 5 - stroke;
	ALLEGRO_COLOR stateColors[6];

	HD_UI_GraphicsObject *gfxObject = NULL;
	HD_UI_TextObject *textObject = NULL;
	//To-Do: tooltip object
	//To-Do: Right-Click Context menu object

	Hitzone hitzone;
	ButtonState btnState = standard;

protected:
	std::string tooltipText;
	std::string captionText;

	std::function<void()>buttonCallback;

	

	//Button functions
	void mouseOver();
	void mouseOut();
	void mouseClick();
	void mouseRelease();

	bool checkMouseOver(int mX, int mY);

public:
	//Creation functions
	//Construction
	HD_UI_Button();

	//Image button
	//based on the standard image, the hover and clicked ones are found auto-magically
	HD_UI_Button(char *objectName, int index, char *standardImageName, char *atlasName, void(*callback)(), char *tooltip,
		float fX, float fY, float hzWidth, float hzHeight, HD_UI_Container *newParent);

	//Filled Rect or Filled & Stroked Rect button; has a text caption
	//the first 3 colors are for standard, hover and clicked states of the bg
	//the next 2 colors are for the text's standard and clicked color; and the last one is for the stroke
	HD_UI_Button(char *objectName, int index, char *caption, void(*callback)(), char *tooltip,
		float fX, float fY, float fWidth, float fHeight, ALLEGRO_COLOR colors[6], bool isFilled, ALLEGRO_FONT *captionFont,
		float hzWidth, float hzHeight, HD_UI_Container *newParent);

	//Destruction
	~HD_UI_Button();

	//General functions
	void Update(ALLEGRO_MOUSE_STATE *mouseState, double timeSpeed);
	void Draw();
	void Clear();

};

#endif