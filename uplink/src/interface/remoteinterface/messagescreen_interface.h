

#ifndef _included_messagescreeninterface_h
#define _included_messagescreeninterface_h


// ============================================================================

#include "interface/remoteinterface/remoteinterfacescreen.h"

class ComputerScreen;
class MessageScreen;

// ============================================================================


class MessageScreenInterface : public RemoteInterfaceScreen
{

protected:

	static void Click ( Button *button );
	static void MailMeClick ( Button *button );

public:

	bool ReturnKeyPressed ();

	void Create ();
	void Create ( ComputerScreen *newcs );
	void Remove ();
	bool IsVisible ();

	int ScreenID ();

	MessageScreen *GetComputerScreen ();

};


#endif
