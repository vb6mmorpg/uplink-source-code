
/*

  LAN Interface

    Local Interface screen
    Allows you to view a Local Area Network

  */


#ifndef _included_laninterface_h
#define _included_laninterface_h

// ============================================================================

#include "world/computer/lancomputer.h"
#include "interface/localinterface/localinterfacescreen.h"


struct LanInterfaceObject
{
    char    *name;
    char    *filename;
    int     width;
    int     height;
    char    *description;
};

// ============================================================================


#define LANHIGHLIGHT_NONE               0
#define LANHIGHLIGHT_VALIDSUBNET        1
#define LANHIGHLIGHT_CONTROLS           2

class LanInterfaceHighlight
{
public:
    LanInterfaceHighlight ( int _systemIndex, char *_text ) 
        : systemIndex(_systemIndex) 
        {
            UplinkStrncpy ( text, _text, sizeof ( text ) );
        }
    
    int systemIndex;
    char text[64];
};


// ============================================================================


class LanInterface : public LocalInterfaceScreen
{

protected:
    
    static LanInterfaceObject lanInterfaceObjects[LANSYSTEM_NUMTYPES];

    static void TitleDraw           ( Button *button, bool highlighted, bool clicked );
    static void PanelBackgroundDraw ( Button *button, bool highlighted, bool clicked );

    static void LanSystemDraw		( Button *button, bool highlighted, bool clicked );
    static void LanSystemClick		( Button *button );
    static void LanSystemMouseMove	( Button *button );

    static void TitleClick          ( Button *button );
    static void LanBackgroundDraw	( Button *button, bool highlighted, bool clicked );
    static void LanBackgroundMouseMove ( Button *button );

	static void ConnectDraw			( Button *button, bool highlighted, bool clicked );
	static void ConnectClick		( Button *button );
	static void ConnectMouseMove	( Button *button );

	static void CancelClick			( Button *button );

    static void BackDraw            ( Button *button, bool highlighted, bool clicked );
    static void BackMouseMove       ( Button *button );
    static void BackClick           ( Button *button );

    static void GenerateClick       ( Button *button );

	static void DrawLink ( LanComputerLink *link, 
							float fromX, float fromY,
							float toX, float toY );

    static void ScrollClick ( Button *button );

protected:

    char ip[SIZE_VLOCATION_IP];
    LList <LanInterfaceHighlight *> highlights;

    static int offsetX;
    static int offsetY;

public:

    LanInterface ();
    ~LanInterface ();

    void CreateLayout ();
    void RemoveLayout ();
    void PositionLayout ();                         // Moves all buttons

    void SelectSystem ( int systemIndex );

    static LanInterfaceObject *GetLanInterfaceObject( int TYPE );

	virtual void Create ();
	virtual void Remove ();
	virtual void Update ();
	virtual bool IsVisible ();

	virtual int  ScreenID ();			

};


#endif

