

#ifndef _included_protovisionscreeninterface_h
#define _included_protovisionsscreeninterface_h


// ============================================================================

#include "interface/remoteinterface/remoteinterfacescreen.h"

class ComputerScreen;
class GenericScreen;

// ============================================================================


class ProtovisionScreenInterface : public RemoteInterfaceScreen
{

protected:

	static void Close ( Button *button );
    
    static void NuclearWarDraw ( Button *button, bool highlighted, bool clicked );
    static void NuclearWar ( Button *button );
    
public:

	bool ReturnKeyPressed ();

	void Create ();
	void Create ( ComputerScreen *newcs );
	void Remove ();
    void Update ();
    bool IsVisible ();

	int ScreenID ();

	GenericScreen *GetComputerScreen ();

};


#endif
