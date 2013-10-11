
/*

  Client Status Interface

	Shows info on the world, the player and the gateway

  */

#ifndef _included_statusinterface_h
#define _included_statusinterface_h


#include "network/interfaces/networkscreen.h"



class ClientStatusInterface : public NetworkScreen
{

protected:

	static void BackgroundDraw ( Button *button, bool highlighted, bool clicked );

	static void GatewayPanelDraw ( Button *button, bool highlighted, bool clicked );
	static void PersonalPanelDraw ( Button *button, bool highlighted, bool clicked );
	static void WorldPanelDraw ( Button *button, bool highlighted, bool clicked );

	static void MainMenuClick ( Button *button );

protected:

	char hardware	 [256];					// 
	char hudupgrades [256];					// Gateway data
	char connection  [128];					//

	char ratings	 [256];					//
	char criminal	 [256];					// Personal data
	char financial	 [256];					//

	LList <char *> news;					// World data

public:

	ClientStatusInterface();
	virtual ~ClientStatusInterface();

	void SetHardware	( char *newhw );
	void SetHUDUpgrades ( char *newhud );
	void SetConnection	( char *newconnection );

	void SetRating      ( char *newrating );
	void SetFinancial   ( char *newfinancial );
	void SetCriminal    ( char *newcriminal );
	
	void AddNewsStory   ( char *news );

	void Create ();
	void Remove ();
	void Update ();
	bool IsVisible ();

	int  ScreenID ();	

};



#endif


