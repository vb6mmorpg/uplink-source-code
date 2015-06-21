//======================================
// The Options Layout opened from MM.
//======================================

#ifndef HD_MMOPTIONS_H
#define HD_MMOPTIONS_H

#include "../UI_Objects/HD_UI_Container.h"

#include "../UI_Objects/HD_UI_ButtonDropdown.h"
#include "../UI_Objects/HD_UI_ButtonSwitch.h"

class HD_MMOptions : public HD_UI_Container
{
private:
	std::vector<std::string> getScreenModesStrings();

	std::shared_ptr<HD_UI_ButtonSwitch> btnFrameless = nullptr;
	std::shared_ptr<HD_UI_ButtonDropdown> btnResolution = nullptr;
	std::shared_ptr<HD_UI_ButtonSwitch> btnMusic = nullptr;

protected:
	//callback functions
	void applyClick();

public:
	//creation
	HD_MMOptions() {}
	~HD_MMOptions() {}

	void Create();
	void Update();

	void ShowScreen(bool show);

};

#endif