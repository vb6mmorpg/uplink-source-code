
/*

	Definition of News screen interface

  */

#ifndef _included_newsscreeninterface_h
#define _included_newsscreeninterface_h

// ============================================================================

#include "interface/remoteinterface/remoteinterfacescreen.h"

class ComputerScreen;
class GenericScreen;

// ============================================================================


class NewsScreenInterface : public RemoteInterfaceScreen
{

protected:

	static int baseoffset;
	static int currentselect;
	static int previousnummessages;

protected:

	static void DrawNewsButton      ( Button *button, bool highlighted, bool clicked );
	static void ClickNewsButton     ( Button *button );
	static void MousedownNewsButton ( Button *button );
	static void HighlightNewsButton ( Button *button );
	
	static void DrawDetails     ( Button *button, bool highlighted, bool clicked );

	static void ExitClick		( Button *button );
    static void ScrollChange    ( char *scrollname, int newValue );

public:

    bool EscapeKeyPressed ();

	void Create ( ComputerScreen *cs );
	void Remove ();
	void Update ();
	bool IsVisible ();

	int ScreenID ();

	GenericScreen *GetComputerScreen ();				// Should override this with correct data type

};

#endif

