
/*

	Definition of Hardware Sales interface

  */

#ifndef _included_hwsalesscreeninterface_h
#define _included_hwsalesscreeninterface_h

// ============================================================================

#include "interface/remoteinterface/remoteinterfacescreen.h"

class ComputerScreen;
class GenericScreen;
class Sale;

// ============================================================================


class HWSalesScreenInterface : public RemoteInterfaceScreen
{

protected:

	int HWType;												// What type of hardware upgrade are we buying.  
	LList <Sale *> items;									// (-1 = Menu of all types)

protected:

	static int baseoffset;
	static int currentselect;

protected:

	// Specific to Menu

	static void ShowSalesMenuClick ( Button *button );

	// Specific to Sales Menu

	static void DrawHWButton       ( Button *button, bool highlighted, bool clicked );
	static void ClickHWButton      ( Button *button );
	static void MousedownHWButton  ( Button *button );
	static void HighlightHWButton  ( Button *button );

	static void DrawDetails     ( Button *button, bool highlighted, bool clicked );

	static void AcceptClick		( Button *button );
	static void ExitClick		( Button *button );
	static void ScrollUpClick   ( Button *button );
	static void ScrollDownClick ( Button *button );

public:

	HWSalesScreenInterface ();
	~HWSalesScreenInterface ();

    bool EscapeKeyPressed ();

	void SetHWType ( int newType );							// Populates "items" with all valid upgrades

	void Create ( ComputerScreen *cs );						// Creates the menu (HWType == -1)
	void CreateMenu ( ComputerScreen *cs );
	void CreateSalesMenu ( ComputerScreen *cs );

	void Remove ();
	void RemoveMenu ();
	void RemoveSalesMenu ();

	bool IsVisible ();
	bool IsVisibleMenu ();
	bool IsVisibleSalesMenu ();

	int ScreenID ();

	GenericScreen *GetComputerScreen ();

};

#endif

