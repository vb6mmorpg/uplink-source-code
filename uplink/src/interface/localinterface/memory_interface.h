// MemoryInterface.h: interface for the MemoryInterface class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _included_memoryinterface_h
#define _included_memoryinterface_h


#include "interface/localinterface/localinterfacescreen.h"


class MemoryInterface : public LocalInterfaceScreen  
{

protected:

	static int previousnumfiles;

	static int baseoffset;
	static int currentprogramindex;					// Currently highlighted program

    static int specialHighlight;

protected:

	static void MemoryBlockDraw      ( Button *button, bool highlighted, bool clicked );
	static void MemoryBlockHighlight ( Button *button );
	static void MemoryBlockClick	 ( Button *button );
	
	static void TitleClick ( Button *button );
    static void ScrollChange ( char *scrollname, int newValue );

public:

    void ForceUpdateAll ();
    void SpecialHighlight ( int memoryIndex );

	void Create ();
	void Remove ();
	void Update ();
	bool IsVisible ();

	int ScreenID ();

};

#endif 
