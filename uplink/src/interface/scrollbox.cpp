
#include "stdafx.h"

#include <GL/gl.h>

#include <GL/glu.h>


#include <stdio.h>

#include "eclipse.h"
#include "gucci.h"

#include "app/app.h"
#include "app/globals.h"
#include "app/miscutils.h"
#include "app/opengl_interface.h"
#include "app/opengl.h"

#include "interface/scrollbox.h"

#include "mmgr.h"


LList <ScrollBox *> ScrollBox::scrollBoxes;
char ScrollBox::currentGrab[] = "None";

ScrollBox::ScrollBox()
{

	interfaceCreated = false;
    UplinkStrncpy ( name, "NewScrollBox", sizeof ( name ) );
    x = y = w = h = 0;
    numItems = windowSize = currentIndex = 0;
    callback = NULL;

}

ScrollBox::~ScrollBox()
{
}

void ScrollBox::SetName ( char *newname )
{
    
    UplinkAssert (strlen(newname) < SIZE_SCROLLBOX_NAME );
    UplinkStrncpy ( name, newname, sizeof ( name ) );

}

void ScrollBox::SetDimensions ( int newx, int newy, int neww, int newh )
{
   
    x = newx;
    y = newy;
    w = neww;
    h = newh;

}

void ScrollBox::SetNumItems ( int newNumItems )
{
    numItems = newNumItems;

    if ( currentIndex > (numItems - windowSize) ) {
        currentIndex = numItems - windowSize;
        if ( currentIndex < 0 ) currentIndex = 0;
        if ( callback ) callback( name, currentIndex );
    }

    UpdateInterface ();
}

void ScrollBox::SetWindowSize ( int newWindowSize )
{
    windowSize = newWindowSize;
    UpdateInterface ();
}

void ScrollBox::SetCurrentIndex ( int newCurrentIndex )
{
    currentIndex = newCurrentIndex;
    UpdateInterface ();
}

void ScrollBox::SetCallback ( void (*newcallback) (char *, int) )
{
    callback = newcallback;
}

void ScrollBox::DrawScrollBar ( Button *button, bool highlighted, bool clicked )
{

    //
    // Draw the background

	glBegin ( GL_QUADS );		
		SetColour ( "PanelBackgroundA" );       glVertex2i ( button->x, button->y + button->height );
		SetColour ( "PanelBackgroundB" );       glVertex2i ( button->x, button->y );
		SetColour ( "PanelBackgroundA" );       glVertex2i ( button->x + button->width, button->y );
		SetColour ( "PanelBackgroundB" );       glVertex2i ( button->x + button->width, button->y + button->height );
	glEnd ();

    //
    // Draw the selecter, when the crowd say bo

    char unused[64];
    char name[128];
    sscanf ( button->name, "%s %s", unused, name );
    ScrollBox *thisBox = ScrollBox::GetScrollBox( name );

    if ( thisBox ) {

        int x = thisBox->x;
        int w = thisBox->w;
        int y = (int) ( thisBox->y + 15 + ((float) thisBox->currentIndex / (float) thisBox->numItems) * (thisBox->h - 30) );
        int h = (int) ( ((float) thisBox->windowSize / (float) thisBox->numItems) * (thisBox->h - 30) );
        if ( h > button->height ) h = button->height;

	    glBegin ( GL_QUADS );
		    SetColour ( "ButtonNormalA" );          glVertex2i ( x, y + h );
		    SetColour ( "ButtonNormalB" );          glVertex2i ( x, y );
		    SetColour ( "ButtonNormalA" );          glVertex2i ( x + w, y );
            SetColour ( "ButtonNormalB" );          glVertex2i ( x + w, y + h );
	    glEnd ();

    }

    //
    // Draw the border

	SetColour ( "PanelBorder" );
	border_draw ( button );
    
}

void ScrollBox::ScrollUp ()
{

    if (currentIndex > 0) {
        currentIndex--;
        UpdateInterface ();
        if ( callback ) callback(name, currentIndex);
    }
        
}

void ScrollBox::ScrollDown ()
{
    
    if (currentIndex < (numItems - windowSize) ) {
        ++currentIndex;
        UpdateInterface ();
        if ( callback ) callback(name, currentIndex);
    }

}

void ScrollBox::ClickScrollUp   ( Button *button )
{

    //
    // Get this scrollbar

    char unused[64];
    char name[128];
    sscanf ( button->name, "%s %s", unused, name );
    ScrollBox *thisBox = ScrollBox::GetScrollBox( name );

    if ( thisBox ) thisBox->ScrollUp ();

}

void ScrollBox::ClickScrollDown ( Button *button )
{

    //
    // Get this scrollbar

    char unused[64];
    char name[128];
    sscanf ( button->name, "%s %s", unused, name );
    ScrollBox *thisBox = ScrollBox::GetScrollBox( name );

    if ( thisBox ) thisBox->ScrollDown ();

}

void ScrollBox::ClickScrollBar ( Button *button )
{

    //
    // Get this scrollbar

    char unused[64];
    char name[128];
    sscanf ( button->name, "%s %s", unused, name );
    ScrollBox *thisBox = ScrollBox::GetScrollBox( name );

    if ( thisBox ) {
    
        int mouseY = get_mouseY ();

        int y = (int) ( thisBox->y + 15 + ((float) thisBox->currentIndex / (float) thisBox->numItems) * (thisBox->h - 30) );
        int h = (int) ( ((float) thisBox->windowSize / (float) thisBox->numItems) * (thisBox->h - 30) );

        if ( mouseY < y ) {

            thisBox->currentIndex -= thisBox->windowSize;
            if ( thisBox->currentIndex < 0 ) thisBox->currentIndex = 0;
            if ( thisBox->callback ) thisBox->callback ( name, thisBox->currentIndex );
            thisBox->UpdateInterface ();

        }
        else if ( mouseY > y + h ) {

            thisBox->currentIndex += thisBox->windowSize;
            if ( thisBox->currentIndex > (thisBox->numItems - thisBox->windowSize) ) {
                thisBox->currentIndex = thisBox->numItems - thisBox->windowSize;
                if ( thisBox->currentIndex < 0 )
                    thisBox->currentIndex = 0;
            }
            if ( thisBox->callback ) thisBox->callback ( name, thisBox->currentIndex );
            thisBox->UpdateInterface ();

        }

    }

}

void ScrollBox::MouseDownScrollBar ( Button *button )
{
    button_click(button);

    char unused[64];
    char name[128];
    sscanf ( button->name, "%s %s", unused, name );
    GrabScrollBar ( name );
    
}

void ScrollBox::MouseMoveScrollBar ( Button *button )
{
    button_highlight(button);
}

void ScrollBox::GrabScrollBar ( char *name )
{
    int mouseY = get_mouseY ();
    UplinkStrncpy ( currentGrab, name, sizeof ( currentGrab ) );
}

void ScrollBox::UnGrabScrollBar ()
{
    if ( strcmp ( currentGrab, "None" ) != 0 ) {
        UplinkStrncpy ( currentGrab, "None", sizeof ( currentGrab ) );
    }       
}
 
bool ScrollBox::IsGrabInProgress ()
{
    if ( strcmp ( currentGrab, "None" ) == 0 )
        return false;

    else {

        return true;

    }
       
}
   
void ScrollBox::UpdateGrabScroll ()
{

    ScrollBox *thisBox = ScrollBox::GetScrollBox( currentGrab );
    if ( thisBox ) {

        if ( thisBox->windowSize >= thisBox->numItems ) return;

        int mouseY = get_mouseY ();

        float fractionThrough = (float) (mouseY - thisBox->y) / (float) (thisBox->h);
        int barH = (int) ( ((float) thisBox->windowSize / (float) thisBox->numItems) * (thisBox->h - 30) );
        if ( barH > thisBox->h) barH = thisBox->h;
        float barFraction = (float) barH / (float) thisBox->h;
        fractionThrough -= barFraction / 2.0f;

        if ( fractionThrough < 0.0f ) fractionThrough = 0.0f;
        if ( fractionThrough > 1.0f ) fractionThrough = 1.0f;

        if (thisBox->numItems * fractionThrough <= (thisBox->numItems - thisBox->windowSize) ) {
            thisBox->SetCurrentIndex( (int) ( thisBox->numItems * fractionThrough ) );
        }
        else {
            thisBox->SetCurrentIndex( thisBox->numItems - thisBox->windowSize );
        }

        if ( thisBox->callback ) thisBox->callback(thisBox->name, thisBox->currentIndex);

    }
    else {
        UnGrabScrollBar ();
    }

}

void ScrollBox::CreateInterface ()
{

    char scrollUpName[256];
    char scrollBarName[256];
    char scrollDownName[256];

    UplinkSnprintf ( scrollUpName, sizeof ( scrollUpName ), "scrollup %s", name );
    UplinkSnprintf ( scrollDownName, sizeof ( scrollDownName ), "scrolldown %s", name );
    UplinkSnprintf ( scrollBarName, sizeof ( scrollBarName ), "scrollbar %s", name );

    EclRegisterButton ( x, y, 15, 15, "^", scrollUpName );
    EclRegisterButton ( x, y + 15, 15, h - 30, " ", scrollBarName );
    EclRegisterButton ( x, (y + h) - 15, 15, 15, "v", scrollDownName );

    button_assignbitmaps ( scrollUpName, "up.tif", "up_h.tif", "up_c.tif" );
    button_assignbitmaps ( scrollDownName, "down.tif", "down_h.tif", "down_c.tif" );
    
    EclRegisterButtonCallback ( scrollUpName, ClickScrollUp );
    EclRegisterButtonCallback ( scrollDownName, ClickScrollDown );
    EclRegisterButtonCallbacks ( scrollBarName, DrawScrollBar, ClickScrollBar, MouseDownScrollBar, MouseMoveScrollBar);

	interfaceCreated = true;
}

void ScrollBox::RemoveInterface ()
{

	if (!interfaceCreated) return;

    char scrollUpName[256];
    char scrollBarName[256];
    char scrollDownName[256];

    UplinkSnprintf ( scrollUpName, sizeof ( scrollUpName ), "scrollup %s", name );
    UplinkSnprintf ( scrollDownName, sizeof ( scrollDownName ), "scrolldown %s", name );
    UplinkSnprintf ( scrollBarName, sizeof ( scrollBarName ), "scrollbar %s", name );

    EclRemoveButton ( scrollUpName );
    EclRemoveButton ( scrollBarName );
    EclRemoveButton ( scrollDownName );

    if ( strcmp ( currentGrab, scrollBarName ) == 0 )
		ScrollBox::UnGrabScrollBar ();

	interfaceCreated = false;
}

void ScrollBox::UpdateInterface ()
{

	if (interfaceCreated) {
	    char scrollBarName [256];
		UplinkSnprintf ( scrollBarName, sizeof ( scrollBarName ), "scrollbar %s", name );
		EclDirtyButton ( scrollBarName );
	}

}

bool ScrollBox::IsInterfaceVisible ()
{
    return true;
}

void ScrollBox::CreateScrollBox ( char *name, 
                       int x, int y, int w, int h,
                       int numItems, int windowSize, int currentIndex,
                       void (*callback)(char *, int) )
{

    ScrollBox *sb = new ScrollBox ();
    sb->SetName ( name );
    sb->SetDimensions ( x, y, w, h );
    sb->SetNumItems ( numItems );
    sb->SetWindowSize ( windowSize );
    sb->SetCurrentIndex ( currentIndex );
    sb->SetCallback ( callback );
    scrollBoxes.PutData( sb );

    sb->CreateInterface();

}

void ScrollBox::RemoveScrollBox ( char *name )
{

    for ( int i = 0; i < scrollBoxes.Size(); ++i ) {
        
        ScrollBox *sb = scrollBoxes.GetData(i);

        if ( strcmp ( sb->name, name ) == 0 ) {
            sb->RemoveInterface ();
            delete sb;
            scrollBoxes.RemoveData(i);
            return;
        }

    }
            
}

ScrollBox *ScrollBox::GetScrollBox ( char *name )
{

    for ( int i = 0; i < scrollBoxes.Size(); ++i )
        if ( strcmp ( scrollBoxes.GetData(i)->name, name ) == 0 )
            return scrollBoxes.GetData(i);

    return NULL;

}



