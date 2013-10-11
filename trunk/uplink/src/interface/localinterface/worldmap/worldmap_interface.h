/*

  Worldmap
  interface definition

  */

#ifndef _included_worldmapinterface_h
#define _included_worldmapinterface_h


#include "eclipse.h"

#include "interface/localinterface/localinterfacescreen.h"
#include "rectangle.h"

#define WORLDMAP_NONE     0
#define WORLDMAP_SMALL    1
#define WORLDMAP_LARGE    2


#define TEXTSIZEX 5						// Size of text grid
#define TEXTSIZEY 20					//

class WorldMapLayout;

class WorldMapInterface : public LocalInterfaceScreen
{

protected:

    static int  stipplepattern;
    static void CycleStipplePattern ();
   
    static void RemoveWorldMapInterface ();

    static void FullScreenClick ( Button *button );	
    static void LocationClick   ( Button *button );
	
    static void ConnectClick     ( Button *button );
    static void ConnectMouseMove ( Button *button );
    static void ConnectMouseDown ( Button *button );
    static void ConnectDraw      ( Button *button, bool highlighted, bool clicked );

    static void CancelClick       ( Button *button );
    static void TraceAdvanceClick ( Button *button );
    static void CloseClick        ( Button *button );

    static void SaveConnectionClick ( Button *button );
    static void LoadConnectionClick ( Button *button );

    static void ZoomInClick ( Button *button );
    static void ZoomOutClick ( Button *button );    
    static void ZoomButtonClick ( Button *button );
    static void ZoomButtonDraw ( Button *button, bool highlighted, bool clicked );

    static void DrawWorldMapSmall ( Button *button, bool highlighted, bool clicked );
    static void DrawWorldMapLarge ( Button *button, bool highlighted, bool clicked );
    static void DrawLocation ( Button *button, bool highlighted, bool clicked );

    static int GetScaledX ( int x, int SIZE );
    static int GetScaledY ( int y, int SIZE );
    
    static MapRectangle GetLargeMapRect();
    static int GetLargeMapX1();
    static int GetLargeMapY1();
    static int GetLargeMapWidth();
    static int GetLargeMapHeight();

	static void UpdateAccessLevel();

protected:

    // This stuff handles the pre-cached text and lines
    
    WorldMapLayout *layout;

    void ProgramLayoutEngine();
	void ProgramLayoutEnginePartial();
    void DrawAllObjects ();
    void CheckLinksChanged();
	static void RemoveTempConnectionButton();

protected:

    // This stuff handles loading/saving of connections
    LList <char *> savedconnection;
    void SaveCurrentConnection ();
    void LoadConnection ();

protected:

    // This stuff handles map zooming and scrolling

    static float scrollX;
    static float scrollY;
    static float zoom;

    void ScrollX ( float x );
    void ScrollY ( float y );
    void SetZoom ( float z );
    void ChangeZoom ( float z );

public:

    static void CreateWorldMapInterface ( int SIZE );
    static void CreateWorldMapInterface_Small ();
    static void CreateWorldMapInterface_Large ();
    static void CloseWorldMapInterface_Large ();
    static int IsVisibleWorldMapInterface ();					// returns size of map visible

	static void AfterPhoneDialler ( char *ip, char *info );
    
    WorldMapInterface ();
    ~WorldMapInterface ();

    void Create ();
    void Create ( int SIZE );
    void Update ();
    void Remove ();
    bool IsVisible ();					

    int ScreenID ();

	// Common functions

	virtual bool Load  ( FILE *file );
	virtual void Save  ( FILE *file );
	virtual void Print ();
	
	virtual char *GetID ();

protected:

	// Used for the coloring of the servers

	int nbmissions;
	int nbmessages;
	int nbcolored;

};



#endif
