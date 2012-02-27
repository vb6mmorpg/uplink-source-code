

#ifndef _included_logscreeninterface_h
#define _included_logscreeninterface_h


// ============================================================================

#include "interface/remoteinterface/remoteinterfacescreen.h"

class ComputerScreen;
class LogScreen;

// ============================================================================


class LogScreenInterface : public RemoteInterfaceScreen
{

protected:

	static int baseoffset;
	static int previousnumlogs;

protected:

	static void LogDraw			( Button *button, bool highlighted, bool clicked );

/*
	static void ScrollUpClick	( Button *button );
	static void ScrollDownClick ( Button *button );
*/

	static int GetNbItems ( DArray<class AccessLog *> *logs );
	static int GetFirstItem ( DArray<class AccessLog *> *logs );
	static int GetLastItem ( DArray<class AccessLog *> *logs );

    static void CreateScrollBar ( int nbItems );
    static void ScrollChange ( char *scrollname, int newValue );

	static void LogClick		( Button *button );
	static void CloseClick		( Button *button );

public:

    bool EscapeKeyPressed ();

	void Create ();
	void Create ( ComputerScreen *newcs );
	void Remove ();
	void Update ();
	bool IsVisible ();

	int ScreenID ();

	LogScreen *GetComputerScreen ();

};


#endif
