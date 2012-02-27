
/*

  Faith Screen Interface

	Shows the progress of Faith V Revelation

  */


// ============================================================================

#ifndef _included_faithscreeninterface_h
#define _included_faithscreeninterface_h

#include "interface/remoteinterface/remoteinterfacescreen.h"

class GenericScreen;

// ============================================================================



class FaithScreenInterface : public RemoteInterfaceScreen
{

protected:

	static void CloseClick ( Button *button );

public:

	FaithScreenInterface ();
	~FaithScreenInterface ();

    bool EscapeKeyPressed ();

	void Create ();
	void Create ( ComputerScreen *newcs );
	void Remove ();
	void Update ();
	bool IsVisible ();

	int ScreenID ();

	GenericScreen *GetComputerScreen ();

};



#endif

