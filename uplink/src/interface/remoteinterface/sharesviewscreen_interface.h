
/*

  Shares View Screen Interface

	Allows the player to view the details of one
	specific share, including its past share price history.

	Also allows the player to buy and sell this share type.

  */


#ifndef _included_sharesviewscreeninterface_h
#define _included_sharesviewscreeninterface_h

#include "world/company/company.h"
#include "world/computer/computerscreen/genericscreen.h"

#include "interface/remoteinterface/remoteinterfacescreen.h"


class SharesViewScreenInterface : public RemoteInterfaceScreen 
{

protected:

	char companyname [SIZE_COMPANY_NAME];
	int sharehistory [12];
	int lastmonthset;

protected:
	
	static void DrawPriceGraph ( Button *button, bool highlighted, bool clicked );
    static void DrawProfit ( Button *button, bool highlighted, bool clicked );

	static void BuyClick ( Button *button );
	static void SellClick ( Button *button );
	static void CloseClick ( Button *button );

	static void UpdateStatus ();							// Updates caption on number owned / profit etc

public:

	SharesViewScreenInterface ();
	~SharesViewScreenInterface ();

    bool EscapeKeyPressed ();

	void SetCompanyName ( char *newcompanyname );

	void Create ();
	void Create ( ComputerScreen *newcs );
	void Remove ();
	void Update ();
	bool IsVisible ();

	int ScreenID ();

	GenericScreen *GetComputerScreen ();

};


#endif

