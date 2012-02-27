

#ifndef _included_fileserverscreeninterface_h
#define _included_fileserverscreeninterface_h

// ============================================================================

#include "interface/remoteinterface/remoteinterfacescreen.h"

class ComputerScreen;
class GenericScreen;
class DataBank;
class Data;

// ============================================================================


class FileServerScreenInterface : public RemoteInterfaceScreen
{

protected:

	static int baseoffset;
	static int previousnumfiles;

protected:

	static void FileDraw		( Button *button, bool highlighted, bool clicked );

	/*
	static void ScrollUpClick	( Button *button );
	static void ScrollDownClick ( Button *button );
	*/

	static int GetNbRowsDisplayDataBank ( DataBank *db );
	static int GetInfoRowDisplayDataBank ( DataBank *db, int fileindex, 
	                                       Data **data, int *size, int* memoryindex );

    static void CreateScrollBar ( int nbItems );
    static void ScrollChange ( char *scrollname, int newValue );

	static void FileClick		( Button *button );
	static void CloseClick		( Button *button );
	
public:

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
