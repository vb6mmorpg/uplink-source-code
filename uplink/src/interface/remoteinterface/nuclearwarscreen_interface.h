
/*

  Nuclear War screen interface

	Run from the Protovision system
    Plays as a basic version of Asteroids

  */


#ifndef _included_nuclearwarscreeninterface_h
#define _included_nuclearwarscreeninterface_h

#include "interface/remoteinterface/remoteinterfacescreen.h"

class GenericScreen;
struct NuclearWar_Nuke;


class NuclearWarScreenInterface : public RemoteInterfaceScreen
{

protected:

	static void DrawBlack ( Button *button, bool highlighted, bool clicked );

	static void DrawLocation ( Button *button, bool highlighted, bool clicked );
	static void ClickLocation ( Button *button );

    static void DrawMainMap ( Button *button, bool highlighted, bool clicked );

    static void CloseClick ( Button *button );

protected:

    LList <NuclearWar_Nuke *> nukes;

public:

	NuclearWarScreenInterface ();
	~NuclearWarScreenInterface ();

	bool ReturnKeyPressed ();

	void Create ( ComputerScreen *newcs );
	void Remove ();
    void Update ();
	bool IsVisible ();

	int ScreenID ();

	GenericScreen *GetComputerScreen ();

};


struct NuclearWar_Nuke
{

    int sx;
    int sy;

    int x;
    int y;
    int time;

    bool sound;
    
};

#endif

