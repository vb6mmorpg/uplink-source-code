
/*

  Security Screen Interface

	Gives the player access to all installed
	security systems on the computer

  */


#ifndef _included_securityscreeninterface_h
#define _included_securityscreeninterface_h

// ============================================================================

#include "interface/remoteinterface/remoteinterfacescreen.h"

class GenericScreen;

// ============================================================================


class SecurityScreenInterface : public RemoteInterfaceScreen
{

protected:

	static time_t lastupdate;

	static void SystemTitleDraw ( Button *button, bool highlighted, bool clicked );
	static void StatusClick ( Button *button );

	static void CloseClick ( Button *button );

public:

    bool EscapeKeyPressed ();

	void Create ( ComputerScreen *newcs );
	void Remove ();
	void Update ();
	bool IsVisible ();

	int ScreenID ();

	GenericScreen *GetComputerScreen ();

};


#endif

