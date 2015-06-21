//========================================
// This creates an on/off switch button.
//========================================

#ifndef HD_UI_BUTTONSWITCH_H
#define HD_UI_BUTTONSWITCH_H

#include "HD_UI_Button.h"
#include "../HD_Resources.h"

class HD_UI_ButtonSwitch : public HD_UI_Button
{
protected:
	std::shared_ptr<HD_UI_GraphicsObject> switchObject;
	bool isOn = false;

	std::function<void()>onCallback;
	std::function<void()>offCallback;

	//Button functions
	void mouseRelease();

public:
	HD_UI_ButtonSwitch() {}
	~HD_UI_ButtonSwitch() {}

	//Filled Rect or Filled & Stroked Rect switch button
	//colors[7] - the first 3 colors are for standard, hover and clicked states of the bg
	//the next 2 colors are for the text's standard and clicked color;
	// and the last 2 are for the stroke & on/off states of the switch
	void CreateSwitchButton(const char *objectName, float fX, float fY, float fWidth, float fHeight, ALLEGRO_COLOR colors[7],
		const char *caption, const char *tooltip, bool isFilled, ALLEGRO_FONT *captionFont = HDResources->font24,
		bool initState = false);

	bool getSwitchState() { return isOn; }
	void setCallback(std::function<void()>newOnCallback, std::function<void()>newOffCallback);

};

#endif