

#ifndef _included_disconnectedscreeninterface_h
#define _included_disconnectedscreeninterface_h


// ============================================================================

#include "interface/remoteinterface/remoteinterfacescreen.h"

class ComputerScreen;
class DisconnectedScreen;

// ============================================================================


class DisconnectedScreenInterface : public RemoteInterfaceScreen
{

protected:

	static void Click ( Button *button );

public:

	bool ReturnKeyPressed ();

	void Create ();
	void Create ( ComputerScreen *newcs );
	void Remove ();
	bool IsVisible ();

	int ScreenID ();

	DisconnectedScreen *GetComputerScreen ();

};


#endif
