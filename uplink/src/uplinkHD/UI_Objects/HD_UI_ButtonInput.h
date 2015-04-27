//============================
// A button that receives text
// input from user.
// (ex. password box)
// It does not use images.
//============================

#ifndef HD_UI_BUTTONINPUT_H
#define HD_UI_BUTTONINPUT_H

#include <allegro5/allegro.h>
#include <string>

#include "HD_UI_Button.h"

class HD_UI_ButtonInput : public HD_UI_Button
{
protected:
	HD_UI_GraphicsObject *shavedStrokeObject = NULL;
	HD_UI_GraphicsObject *textCaret = NULL;
	unsigned int caretIndex = 0;
	std::string caption;
	std::string inputString;

	//Button functions
	void mouseOut();
	void mouseClick();
	void mouseRelease();

	//Text input
	void getInput();

	//Construction/destruction
	HD_UI_ButtonInput();
	~HD_UI_ButtonInput();

public:
	bool isPassword = false; //hides the text with * symbols

	//Singleline input
	//Creates a normal or bottom-right shaved box as GFX
	HD_UI_ButtonInput(char *objectName, int index, char *caption, char *tooltip, float fX, float fY,
		float fWidth, float fHeight, bool isShaved, ALLEGRO_COLOR colors[6], ALLEGRO_FONT *captionFont, HD_UI_Container *newParent);

	//Multiline input
	HD_UI_ButtonInput(char *objectName, int index, char *caption, char *tooltip, float fX, float fY,
		float fWidth, float fHeight, ALLEGRO_COLOR colors[6], ALLEGRO_FONT *captionFont, HD_UI_Container *newParent);

	//general functions
	void Update();
	void Clear();

	const char* getInputText() { return inputString.c_str(); }

};

#endif