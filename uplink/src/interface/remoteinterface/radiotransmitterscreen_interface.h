
/*

    Radio Transmitter Interface

  */

#ifndef _included_radiotransmitterscreeninterface_h
#define _included_radiotransmitterscreeninterface_h

#include "world/computer/computerscreen/genericscreen.h"
#include "interface/remoteinterface/remoteinterfacescreen.h"


class RadioTransmitterScreenInterface : public RemoteInterfaceScreen
{

protected:

    static void BackgroundDraw  ( Button *button, bool highlighted, bool clicked );
    static void FrequencyDraw   ( Button *button, bool highlighted, bool clicked );

    static void DownClick       ( Button *button );
    static void BigDownClick    ( Button *button );

    static void UpClick         ( Button *button );
    static void BigUpClick      ( Button *button );

    static void ConnectClick    ( Button *button );

protected:

    static int frequencyGhz;
    static int frequencyMhz;

public:

    void Connect ();

	bool ReturnKeyPressed ();

    void Create ();
	void Create ( ComputerScreen *cs );
	void Remove ();
	void Update ();
	bool IsVisible ();

	int ScreenID ();

	GenericScreen *GetComputerScreen ();

};


#endif
