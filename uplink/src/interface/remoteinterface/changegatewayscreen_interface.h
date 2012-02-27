
/*

  Change Gateway screen interface

	Allows the user to exchange his gateway
	for a new one

  */


#ifndef _included_changegatewayscreeninterface_h
#define _included_changegatewayscreeninterface_h

#include "interface/remoteinterface/remoteinterfacescreen.h"

class GenericScreen;



class ChangeGatewayScreenInterface : public RemoteInterfaceScreen
{

protected:

	static void GatewayButtonDraw ( Button *button, bool highlighted, bool clicked );
	static void InfoTextDraw ( Button *button, bool highlighted, bool clicked );

	static void GatewayButtonClick ( Button *button );
	static void CloseClick ( Button *button );
	static void BuyClick ( Button *button );
	static void BuyConfirmClick ( Button *button );
	static void BuyCancelClick ( Button *button );

    static void GatewayPictureDraw ( Button *button, bool highlighted, bool clicked );

    static void ScrollChange ( char *scrollname, int newIndex );

protected:

	static int baseOffset;
    int currentselect;

public:

	ChangeGatewayScreenInterface ();
	~ChangeGatewayScreenInterface ();

	void ShowGateway ( int index );
	int GetGatewayPrice ( int index );							// Takes into account Part exchange value of your gateway

	void Create ();
	void Create ( ComputerScreen *cs );
	void CreateAreYouSure ();

	void Remove ();
	void Update ();
	bool IsVisible ();

	bool ReturnKeyPressed ();
    bool EscapeKeyPressed ();

	int ScreenID ();

	GenericScreen *GetComputerScreen ();

};


#endif

