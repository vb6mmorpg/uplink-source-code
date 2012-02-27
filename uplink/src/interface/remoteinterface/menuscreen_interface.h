

#ifndef _included_Menuscreeninterface_h
#define _included_Menuscreeninterface_h

#include "eclipse.h"

// ============================================================================

#include "interface/remoteinterface/remoteinterfacescreen.h"

class ComputerScreen;
class MenuScreen;

// ============================================================================


class MenuScreenInterface : public RemoteInterfaceScreen
{

protected:

	static void DrawMenuOption			( Button *button, bool highlighted, bool clicked );
	static void DrawMenuOptionDimmed	( Button *button, bool highlighted, bool clicked );
	static void ClickMenuScreenOption	( Button *button );

public:

	void Create ( ComputerScreen *cs );
	void Remove ();
	bool IsVisible ();

	int ScreenID ();

	MenuScreen *GetComputerScreen ();

};


#endif
