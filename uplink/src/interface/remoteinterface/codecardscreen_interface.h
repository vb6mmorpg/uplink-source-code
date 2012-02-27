

#ifndef _included_codecardscreeninterface_h
#define _included_codecardscreeninterface_h


#include "interface/remoteinterface/remoteinterfacescreen.h"


class CodeCardScreenInterface : public RemoteInterfaceScreen
{

protected:

    static char carddataTOM[26][16][3];
    static char carddataCHRIS[26][16][3];
    static char carddataMARK[26][16][3];

    char row;
    int col;

protected:

    static void ProceedClick ( Button *button );
    static void CodeButtonDraw ( Button *button, bool highlighted, bool clicked );

public:

    CodeCardScreenInterface ();
    ~CodeCardScreenInterface ();

    static char *GetCodeTOM     ( char row, int column );                          // Lower case char only please
    static char *GetCodeCHRIS   ( char row, int column );
    static char *GetCodeMARK    ( char row, int column );

	void Create ( ComputerScreen *cs );
	void Remove ();
	void Update ();
	bool IsVisible ();

    bool ReturnKeyPressed ();

	int  ScreenID ();	
    GenericScreen *GetComputerScreen ();

};



#endif

