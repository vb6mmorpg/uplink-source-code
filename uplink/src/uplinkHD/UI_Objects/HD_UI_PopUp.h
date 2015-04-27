//=================================
// PopUp class.
// Creates various types of PopUps.
//=================================

#ifndef HD_UI_POPUP_H
#define HD_UI_POPUP_H

#include "HD_UI_Container.h"

#include <functional>

enum PopUpType
{
	OK,
	OKCANCEL,
	YESNO
};

class HD_UI_PopUp : public HD_UI_Container
{
protected:
	HD_UI_PopUp() {}

	void defaultCallback();

public:
	~HD_UI_PopUp();

	HD_UI_PopUp(char* objectName, PopUpType type, char *title, char *contents, std::function<void()> yesCallback);

};

#endif