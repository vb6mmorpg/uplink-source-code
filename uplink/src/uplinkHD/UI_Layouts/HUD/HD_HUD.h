// The HUD interface. This is where
// the meat of the game takes place.
// All other elements of the HUD are
// controlled by this.
//==================================

#ifndef HD_HUD_H
#define HD_HUD_H

#include "../../UI_Objects/HD_UI_Container.h"

#include "Elements/HD_HUD_TopBar.h"
#include "Elements/HD_HUD_BottomBar.h"
#include "Interfaces/HD_HUD_MainWindow.h"

class HD_HUD : public HD_UI_Container
{
private:
	std::shared_ptr<HD_HUD_TopBar> topBar = nullptr;
	std::shared_ptr<HD_HUD_BottomBar> bottomBar = nullptr;
	std::shared_ptr<HD_HUD_MainWindow> mainWindow = nullptr;

protected:
	//Callbacks

public:
	HD_HUD();
	~HD_HUD();

	void Create();
	void Update();
	void Clear();
};

#endif