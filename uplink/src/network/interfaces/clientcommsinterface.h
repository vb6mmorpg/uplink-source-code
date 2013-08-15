// ClientCommsInterface.h: interface for the ClientCommsInterface class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _included_clientcommsinterface_h
#define _included_clientcommsinterface_h

// ============================================================================

#include "eclipse.h"

#include "network/interfaces/networkscreen.h"

class VLocation;
class WorldMapLayout;

// ============================================================================


class ClientCommsInterface : public NetworkScreen  
{

protected:

    static int stipplepattern;							// Used for drawing connection lines
    static void CycleStipplePattern ();					//

    static void MainMenuClick ( Button *button );
    static void LargeMapDraw ( Button *button, bool highlighted, bool clicked );

public:

    static BTree <VLocation *> locations;
    static LList <char *> connection;
    static int traceprogress;
    
protected:

    static WorldMapLayout *layout;
    static void DrawAllObjects();
    static bool InConnection( const char *computerName );

public:

    ClientCommsInterface();
    virtual ~ClientCommsInterface();
    
    void LayoutLabels();
    
    void Create ();
    void Remove ();
    void Update ();
    bool IsVisible ();

    int  ScreenID ();			

};

#endif 
