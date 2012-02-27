
/*

	Definition of Bulletin Board interface

  */

#ifndef _included_bbsscreeninterface_h
#define _included_bbsscreeninterface_h

// ============================================================================

#include "interface/remoteinterface/remoteinterfacescreen.h"

class ComputerScreen;
class BBSScreen;

// ============================================================================


class BBSScreenInterface : public RemoteInterfaceScreen
{

protected:

	static int baseoffset;
	static int currentselect;

	static int previousnummessages;
	static int previousupdate;

protected:

	static void DrawBBSButton      ( Button *button, bool highlighted, bool clicked );
	static void ClickBBSButton     ( Button *button );
	static void MousedownBBSButton ( Button *button );
	static void HighlightBBSButton ( Button *button );
	
	static void DrawDetails     ( Button *button, bool highlighted, bool clicked );

	static void AcceptClick		( Button *button );
	static void ContactClick	( Button *button );
	static void ExitClick		( Button *button );
	static void ScrollChange    ( char *scrollname, int newValue );

    static int NumItemsOnScreen ();

public:

    bool EscapeKeyPressed ();

	void Create ( ComputerScreen *cs );
	void Remove ();
	void Update ();
	bool IsVisible ();

	int ScreenID ();

	BBSScreen *GetComputerScreen ();				// Should override this with correct data type

};

#endif

