//==============================
// Root object of the interface.
// This holds/updates/resizes
// the current interface.
// Gets updated by HDScreen.
//==============================

#ifndef HD_ROOT_H
#define HD_ROOT_H

#include "../UI_Objects/HD_UI_Container.h"
#include "HUD/Interfaces/HD_HUD_MainWindow.h"

class HD_Root : public HD_UI_Container
{
private:
	void createLayout(const char *layoutName);
	unsigned int layoutIndex = 0;

public:
	HD_Root();
	~HD_Root() {}

	void Create();

	//scaling functions
	float ScaleByWidth(float value);	//scales a value by the Width difference
	float ScaleByHeight(float value);	//scales a value by the Height difference
	void Rescale();						//rescales the root after resolution change

	//Tooltip
	void ShowTooltip(bool show);

	//layout functions
	//set a layout for the screen
	void SetNewLayout(const char *layoutName, bool useLoading, const char* loadingText);

	std::shared_ptr<HD_UI_Container> GetLayout()
	{
		return std::static_pointer_cast<HD_HUD_MainWindow>(children[layoutIndex]);
	}

	std::shared_ptr<HD_HUD_MainWindow> GetHUDMainWindow()
	{
		return std::static_pointer_cast<HD_HUD_MainWindow>(GetLayout()->getChildByName("mainWindow"));
	}
};

extern std::shared_ptr<HD_Root> root;

#endif