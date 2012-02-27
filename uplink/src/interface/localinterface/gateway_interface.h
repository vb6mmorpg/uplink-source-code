
#ifndef _included_gatewayinterface_h
#define _included_gatewayinterface_h


#include "localinterfacescreen.h"


class GatewayInterface : public LocalInterfaceScreen  
{

protected:

	static void CloseClick ( Button *button );

	static void DrawGatewayBackground ( Button *button, bool highlighted, bool clicked );
	static void DrawGatewayPicture    ( Button *button, bool highlighted, bool clicked );
	static void DrawMainTitle ( Button *button, bool highlighted, bool clicked );
	
	static void DrawPopulatedItem ( Button *button, bool highlighted, bool clicked );
	static void DrawUnPopulatedItem ( Button *button, bool highlighted, bool clicked );

public:

	void Create ();
	void Remove ();
	void Update ();
	bool IsVisible ();

	int ScreenID ();

};

#endif 
