//================================
// The central window of the HUD.
// Keeps track of the current "view" and reflects
// the remote interface.
// It's a big'un.
//================================

#ifndef HD_HUD_MAINWINDOW_H
#define HD_HUD_MAINWINDOW_H

#include "../../../UI_Objects/HD_UI_Container.h"

#include "../../../../world/computer/computerscreen/computerscreen.h"

class HD_HUD_MainWindow : public HD_UI_Container
{
protected:
	std::vector<std::shared_ptr<HD_UI_Container>> previousScreens;	//keep them here, untill you delete them
	std::vector<std::shared_ptr<HD_UI_Container>> currentScreens;	//keep them handy; reflects the comp screen id
	int previousScreenID = 0;
	int currentScreenID = -1;
	void createScreen(ComputerScreen *cScreen, int screenIndex);
	void removeScreens();

public:
	HD_HUD_MainWindow();
	~HD_HUD_MainWindow() {}

	void Create();
	void Update();

	void ConnectTo(char* ip, int nextScreen = 0);
	void Disconnect();
	void GoToScreen(int nextScreen = 0);

	int CurrentScreenID() { return currentScreenID; }
	int PreviousScreenID() { return previousScreenID; }

	//Called from the connection listener
	void RemovePreviousScreens();

	//connection animation listener. USED ONLY HERE!
	class ConnectionComplete : public CDBTweener::IListener
	{
	public:
		void onTweenFinished(CDBTweener::CTween *pTween);
	} connectionComplete;
};

#endif