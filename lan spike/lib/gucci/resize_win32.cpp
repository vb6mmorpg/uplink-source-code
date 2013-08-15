#ifndef USE_SDL
#ifdef WIN32

#include <windows.h>

#include <stdio.h>

#include "gucci.h"

#include "mmgr.h"

static int old_screenwidth;    
static int old_screenheight;
static int old_screendepth;
static int old_screenrefresh;

static bool gci_displaychanged = false;                            // True if screen has been resized

void GciResizeGlut ( int, int )
{
    // The screen resizing has nothing to do with GLUT
}

void GciRestoreGlut ()
{
    // The screen resizing has nothing to do with GLUT
}

void GciStoreScreenSize ()
{
    
    DEVMODE mode;
    ZeroMemory(&mode, sizeof(mode));
    mode.dmSize = sizeof(mode);

    BOOL success = EnumDisplaySettings ( NULL, ENUM_CURRENT_SETTINGS, &mode );

    if ( success != 0 ) {
        
        old_screenwidth = mode.dmPelsWidth;
        old_screenheight = mode.dmPelsHeight;
        old_screendepth = mode.dmBitsPerPel;
        old_screenrefresh = mode.dmDisplayFrequency;

    }
    else {
#ifdef _DEBUG
        printf ( "Gci Warning : GciStoreScreenSize failed - cannot determine display properties\n" );
#endif
    }

}


bool GciSetScreenSize ( int width, int height, int bpp, int refresh )
{

    DEVMODE mode;
    ZeroMemory(&mode, sizeof(mode));
    mode.dmSize = sizeof(mode);
    mode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
    mode.dmPelsWidth = width;
    mode.dmPelsHeight = height;

    if ( refresh != -1 ) {
        mode.dmFields = mode.dmFields | DM_DISPLAYFREQUENCY;
        mode.dmDisplayFrequency = refresh;
    }

    if ( bpp != -1 ) {
        mode.dmFields = mode.dmFields | DM_BITSPERPEL;
        mode.dmBitsPerPel = bpp;
    }

    int result = ChangeDisplaySettings(&mode, 0);

    if ( result == DISP_CHANGE_SUCCESSFUL ) {
        gci_displaychanged = true;
        return true;
    }
    else 
        return false; 

}

void GciRestoreScreenSize ()
{

    if ( gci_displaychanged ) {

        DEVMODE mode;
        ZeroMemory(&mode, sizeof(mode));
        mode.dmSize = sizeof(mode);
        mode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;
        if ( old_screenrefresh > 0 ) mode.dmFields = mode.dmFields | DM_DISPLAYFREQUENCY;
        mode.dmPelsWidth = old_screenwidth;
        mode.dmPelsHeight = old_screenheight;
        mode.dmBitsPerPel = old_screendepth;
        mode.dmDisplayFrequency = old_screenrefresh;
        int result = ChangeDisplaySettings(&mode, 0);

    }
}

#endif // WIN32
#endif // USE_SDL