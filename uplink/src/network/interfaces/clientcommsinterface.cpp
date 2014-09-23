// ClientCommsInterface.cpp: implementation of the ClientCommsInterface class.
//
//////////////////////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include <GL/glu.h> /*_glu_extention_library_*/

#include "eclipse.h"
#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/opengl_interface.h"

#include "options/options.h"

#include "mainmenu/mainmenu.h"

#include "network/network.h"
#include "network/networkclient.h"
#include "network/interfaces/clientcommsinterface.h"

#include "interface/localinterface/worldmap/worldmap_layout.h"

#include "world/vlocation.h"

#include "mmgr.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BTree <VLocation *> ClientCommsInterface::locations;
LList <char *> ClientCommsInterface::connection;
WorldMapLayout *ClientCommsInterface::layout = NULL;

static int stippleindex = 0;
static int STIPPLE []  = { 0x1111,		//		0001000100010001
			       0x1111,		//		0001000100010001
			       0x2222,		//		0010001000100010
			       0x2222,		//		0010001000100010
			       0x4444,		//		0100010001000100
			       0x4444,		//		0100010001000100
			       0x8888,		//		1000100010001000
			       0x8888 };	//		1000100010001000

static int lastupdate = 0;

int ClientCommsInterface::stipplepattern = STIPPLE [0];
int ClientCommsInterface::traceprogress = 0;


void ClientCommsInterface::CycleStipplePattern ()
{

    ++stippleindex;
    if ( stippleindex > 7 ) stippleindex = 0;

    stipplepattern = STIPPLE [stippleindex];

}


ClientCommsInterface::ClientCommsInterface()
{
}

ClientCommsInterface::~ClientCommsInterface()
{
    
    if (layout)	delete layout;
    
}

void ClientCommsInterface::MainMenuClick ( Button *button )
{

    UplinkAssert ( app->GetNetwork ()->STATUS == NETWORK_CLIENT );

    app->GetNetwork ()->GetClient ()->SetClientType ( CLIENT_NONE );
    app->GetMainMenu ()->RunScreen ( MAINMENU_LOGIN );

}

void ClientCommsInterface::LargeMapDraw ( Button *button, bool highlighted, bool clicked )
{
    // Draw the background bitmap

	imagebutton_drawtextured ( button, highlighted, clicked );

    int screenheight = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );
    glScissor ( button->x, screenheight - (button->y + button->height), button->width, button->height );	
    glEnable ( GL_SCISSOR_TEST );


    glLineWidth ( 2.0 );
    glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );

    glLineStipple ( 2, stipplepattern );
    CycleStipplePattern ();
    glEnable ( GL_LINE_STIPPLE );

    // Draw the dotted lines

    glBegin ( GL_LINE_STRIP );

    for ( int li = 0; li < connection.Size (); ++li ) {

	char *ip = connection.GetData (li);
	UplinkAssert (ip);
	VLocation *vl = locations.GetData (ip);
		
	if ( vl ) {

	    glVertex2i ( button->x + vl->x, button->y + vl->y );

	    if ( traceprogress == (locations.Size () - li - 1) ) {
		glColor4f ( 1.0f, 0.0f, 0.0f, 1.0f );
		glVertex2i ( button->x + vl->x, button->y + vl->y );				
	    }

	}

    }

    glEnd ();

    glDisable ( GL_LINE_STIPPLE );

//     // Draw the dots for each location

//     glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );

//     glBegin ( GL_QUADS );

//     for ( int di = 0; di < connection.Size (); ++di ) {

// 	char *ip = connection.GetData (di);
// 	UplinkAssert (ip);
// 	VLocation *vl = locations.GetData (ip);
			
// 	if ( vl ) {

// 	    int x = button->x + vl->x;
// 	    int y = button->y + vl->y;

// 	    glVertex2f ( x - 3, y - 3 );
// 	    glVertex2f ( x + 3, y - 3 );
// 	    glVertex2f ( x + 3, y + 3 );
// 	    glVertex2f ( x - 3, y + 3 );

// 	}

//     }

//     glEnd ();

//     // Write any textual information

//     glColor4f ( 1.0f, 1.0f, 1.0f, 1.0f );

//     for ( int ti = 0; ti < connection.Size (); ++ti ) {

// 	char *ip = connection.GetData (ti);
// 	UplinkAssert (ip);
// 	VLocation *vl = locations.GetData (ip);
		
// 	if ( vl ) {

// 	    int x = button->x + vl->x;
// 	    int y = button->y + vl->y;

// 	    GciDrawText ( x - 20, y - 15, vl->ip );
// 	    GciDrawText ( x - 20, y - 7, vl->computer );

// 	}

//     }
    
    DrawAllObjects();

    glDisable ( GL_SCISSOR_TEST );

}

void ClientCommsInterface::DrawAllObjects ()
{
    layout->FullLayoutLabels();

    LList <WorldMapInterfaceObject *> &locations = layout->GetLocations();
    LList <WorldMapInterfaceLabel *> &labels = layout->GetLabels();

    for ( int i = 0; i < labels.Size (); ++i ) {

	WorldMapInterfaceLabel *label = labels.GetData (i);
	WorldMapInterfaceObject *point = locations.GetData (i);
	    
	UplinkAssert (label);
	UplinkAssert (point);
	    
	if (InConnection(label->GetCaption())) {
		
	    label->Draw ();
	    point->Draw ();

	}
	    
    }
}

bool ClientCommsInterface::InConnection( const char *computerName )
{
    for (int i = 0; i < connection.Size(); i++) {
	
	VLocation *vl = locations.GetData ( connection.GetData (i) );
	UplinkAssert (vl);
	
	if ( strcmp ( vl->computer, computerName ) == 0 )
	    return true;
	
    }
    return false;
}


void ClientCommsInterface::Create ()
{
    if ( !IsVisible () ) {
	
	int screenw = app->GetOptions ()->GetOptionValue ( "graphics_screenwidth" );
	int screenh = app->GetOptions ()->GetOptionValue ( "graphics_screenheight" );

	// Create a close button

	EclRegisterButton ( 3, 4, 13, 13, "", "Go to the Main Menu", "comms_mainmenu" );
	button_assignbitmaps ( "comms_mainmenu","close.tif", "close_h.tif", "close_c.tif" );
	EclRegisterButtonCallback ( "comms_mainmenu", MainMenuClick );

	// Create the worldmap graphic

	int fullsizeX = screenw - 40;
	int fullsizeY = (int)(316.0 * ( fullsizeX / 595.0 ));
		
	// Create the background bitmap
	if ( layout )
		delete layout;
	layout = new WorldMapLayout( MapRectangle( 20, 50, fullsizeX, fullsizeY ) );
	
	EclRegisterButton ( 20, 50, fullsizeX, fullsizeY, "", "", "comms_largemap" );												
	
	// TODO: fix to show the right world map
	button_assignbitmap ( "comms_largemap", "worldmaplarge.tif" );
    EclGetButton ( "comms_largemap" )->image_standard->ScaleToOpenGL ();

	//EclGetButton ( "comms_largemap" )->image_standard->SetAlpha ( 0.99 );
	EclRegisterButtonCallbacks ( "comms_largemap", LargeMapDraw, NULL, NULL, NULL );				

//		EclRegisterButton ( 22, 50, 595, 316, "", "Global communications", "comms_largemap" );												
//		button_assignbitmap ( "comms_largemap", "worldmaplarge.tif" );
//		EclRegisterButtonCallbacks ( "comms_largemap", LargeMapDraw, NULL, NULL, NULL );
//		EclGetButton ( "comms_largemap" )->image_standard->FlipAroundH ();		

    }

}

void ClientCommsInterface::Remove ()
{

    if ( IsVisible () ) {

	EclRemoveButton ( "comms_mainmenu" );
	EclRemoveButton ( "comms_largemap" );

    }

}

void ClientCommsInterface::LayoutLabels()
{
    
//    cout << "ClientCommsInterface::LayoutLabels\n";
    layout->Reset();
    
    DArray <VLocation *> *alllinks = locations.ConvertToDArray ();
	
    for (int i = 0; i < alllinks->Size(); i++) {

		if ( alllinks->ValidIndex ( i ) ) {

			VLocation *vl = alllinks->GetData (i);
			    
			if ( vl ) {

		//	    cout << "  : AddLocation " << 20 + vl->x << ", " << 50 + vl->y << ", " << vl->computer << "\n";
				layout->AddLocation( 20 + vl->x, 50 + vl->y, vl->computer, vl->ip );
		        
			}

		}
	
    }
    
    delete alllinks;
    
}

void ClientCommsInterface::Update ()
{
    int timesincelastupdate = (int)EclGetAccurateTime () - lastupdate;

    if ( timesincelastupdate > 200 ) {
		
	EclDirtyButton ( "comms_largemap" );

	lastupdate = (int)EclGetAccurateTime ();

    }
    
}

bool ClientCommsInterface::IsVisible ()
{

    return ( EclGetButton ( "comms_mainmenu" ) != NULL );

}

int ClientCommsInterface::ScreenID ()
{

    return CLIENT_COMMS;

}
