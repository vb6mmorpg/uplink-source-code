

#ifndef _included_passwordscreeninterface_h
#define _included_passwordscreeninterface_h


// ============================================================================

#include "interface/remoteinterface/remoteinterfacescreen.h"

class ComputerScreen;
class PasswordScreen;

// ============================================================================


class PasswordScreenInterface : public RemoteInterfaceScreen
{

protected:

	static void CodeButtonDraw ( Button *button, bool highlighted, bool clicked );

	static void CursorFlash ();
	static void PasswordClick ( Button *button );

	static void AccessCodeClick ( Button *button );

	static void BypassClick ( Button *button );

public:

	void NextPage ();

	bool ReturnKeyPressed ();

	void Create ( ComputerScreen *cs );
	void Remove ();
	void Update ();
	bool IsVisible ();

	int ScreenID ();

	PasswordScreen *GetComputerScreen ();

};


#endif
