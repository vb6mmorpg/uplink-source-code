//====================================
// The Top Bar in the HUD.
// Keeps the Quit, options, game speed
// buttons. The date, cpu usage and 
// location adress.
//====================================

#ifndef HD_HUD_TOPBAR_H
#define HD_HUD_TOPBAR_H

#include "../../../UI_Objects/HD_UI_Container.h"
#include "../../../UI_Objects/HD_UI_TextObject.h"

class HD_HUD_TopBar : public HD_UI_Container
{
private:
	std::shared_ptr<HD_UI_TextObject> timeTxt = nullptr;
	std::shared_ptr<HD_UI_TextObject> dateTxt = nullptr;
	std::shared_ptr<HD_UI_TextObject> locNameTxt = nullptr;
	std::shared_ptr<HD_UI_TextObject> locIPTxt = nullptr;

protected:
	//Clicked callbacks
	void exitClicked();
	void exitMenuClicked(unsigned int id);
	void speedButtonClicked(unsigned int id);

public:
	HD_HUD_TopBar();
	~HD_HUD_TopBar() { }

	void Create();
	void Update();
};

#endif