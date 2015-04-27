//========================================
// This creates an on/off switch button.
//========================================

#ifndef HD_UI_BUTTONSWITCH_H
#define HD_UI_BUTTONSWITCH_H

#include "HD_UI_Button.h"

class HD_UI_ButtonSwitch : public HD_UI_Button
{
protected:
	HD_UI_GraphicsObject *switchObject = NULL;
	HD_UI_TextObject *onText = NULL;
	HD_UI_TextObject *offText = NULL;
	bool isOn;

	std::function<void()>onCallback;
	std::function<void()>offCallback;

	//Button functions
	void mouseRelease();

	HD_UI_ButtonSwitch();
	~HD_UI_ButtonSwitch();

public:

	//Filled Rect or Filled & Stroked Rect switch button
	//colors[7] - the first 3 colors are for standard, hover and clicked states of the bg
	//the next 2 colors are for the text's standard and clicked color;
	// and the last 2 are for the stroke & on/off states of the switch
	HD_UI_ButtonSwitch(char *objectName, int index, float fX, float fY, float fWidth, float fHeight, ALLEGRO_COLOR colors[7],
		char *caption, char *tooltip, ALLEGRO_FONT *captionFont, bool isFilled, bool initState, HD_UI_Container *newParent);

	bool getSwitchState() { return isOn; }
	void setCallback(std::function<void()>newOnCallback, std::function<void()>newOffCallback);

};

#endif