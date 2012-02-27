
/*

	Definition of Sofware Sales interface

  */

#ifndef _included_swsalesscreeninterface_h
#define _included_swsalesscreeninterface_h

// ============================================================================

#include "interface/remoteinterface/remoteinterfacescreen.h"

class ComputerScreen;
class GenericScreen;

// ============================================================================


class SWSalesScreenInterface : public RemoteInterfaceScreen
{

protected:

	static int baseoffset;
	static int currentselect;
	static int currentversion;

protected:

	static void DrawSWButton       ( Button *button, bool highlighted, bool clicked );
	static void ClickSWButton      ( Button *button );
	static void MousedownSWButton  ( Button *button );
	static void HighlightSWButton  ( Button *button );

	static void DrawDetails     ( Button *button, bool highlighted, bool clicked );

	static void AcceptClick		 ( Button *button );
	static void NextVersionClick ( Button *button );
	static void PrevVersionClick ( Button *button );
	static void ExitClick		 ( Button *button );
    static void ScrollChange     ( char *scrollname, int newValue );

    static int NumItemsOnScreen ();

public:

    bool EscapeKeyPressed ();

	void Create ( ComputerScreen *cs );
	void Remove ();
	bool IsVisible ();

	int ScreenID ();

	GenericScreen *GetComputerScreen ();

};

#endif

