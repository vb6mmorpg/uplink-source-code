//==================================
// PopUp class.
// Creates various types of PopUps.
// Only title, text and # of buttons
// can be customized.
//==================================

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
	void defaultCallback();

public:
	HD_UI_PopUp() {}
	~HD_UI_PopUp() {}

	void Create(char* objectName, PopUpType type, char *title, char *contents, std::function<void()> yesCallback);

	void ShowPopUp(bool show);
};

#endif