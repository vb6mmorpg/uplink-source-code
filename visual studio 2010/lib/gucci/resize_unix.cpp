
#ifndef WIN32

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/xf86vmode.h>
#include <X11/cursorfont.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/glx.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "gucci.h"

#include "mmgr.h"

// From ClanLib, LGPL (x11_resolution.cpp)
// needed by qsort
static int cmpmodes(const void *va, const void *vb)
{
	XF86VidModeModeInfo *a = *(XF86VidModeModeInfo**)va;
	XF86VidModeModeInfo *b = *(XF86VidModeModeInfo**)vb;
	if(a->hdisplay > b->hdisplay)
		return -1;
	return b->vdisplay - a->vdisplay;
}

// From ClanLib, LGPL (x11_resolution.cpp)
static XF86VidModeModeInfo GetXF86Mode(Display *dpy, int scr, int width, int height)
{
	XF86VidModeModeInfo **modes;
	XF86VidModeModeLine mode;
	int nmodes;
	int i;

	if ( XF86VidModeGetModeLine(dpy, scr, &i, &mode) &&
		XF86VidModeGetAllModeLines(dpy, scr, &nmodes, &modes))
	{
		qsort(modes, nmodes, sizeof *modes, cmpmodes);
		for (i=nmodes-1; i>= 0; --i) 
		{
			if ( (modes[i]->hdisplay >= width) &&
		             (modes[i]->vdisplay >= height) )
				break;
		}
		if (i>=0)
		{
			if ( (modes[i]->hdisplay != mode.hdisplay) ||
		             (modes[i]->vdisplay != mode.vdisplay) ) 
			{
				return *modes[i];
        		}
    	    		XFree(modes);
		}
	}
	
	XF86VidModeModeInfo dummy_mode;
	memset(&dummy_mode, 0, sizeof(dummy_mode));
	return dummy_mode;
}

static int ignoreXErrors(Display *dpy, XErrorEvent *event)
{
    return 0;
}

extern Display *__glutDisplay;
extern int __glutScreenWidth, __glutScreenHeight;
extern Window __glutRoot;

static XF86VidModeModeInfo origMode;
static bool savedMode = false;


void GciSetCursor()
{
    Window rootWindow = __glutRoot;
    Display *dpy = __glutDisplay;
    
    Cursor arrowCursor = XCreateFontCursor(dpy, XC_left_ptr);
    
#ifdef RECOLOR_CURSOR
    XWindowAttributes a;
    XColor fg, bg, exact;
    
    XGetWindowAttributes(dpy, rootWindow, &a);
    XAllocNamedColor(dpy, a.colormap, "#08184A", &fg, &exact);
    XAllocNamedColor(dpy, a.colormap, "#628DF6", &bg, &exact);
    XRecolorCursor(dpy, arrowCursor, &fg, &bg);
#endif
    
    XDefineCursor(dpy, rootWindow, arrowCursor);
    XFlush(dpy);
}

// Sections From ClanLib (LGPL) (x11_resolution.cpp)
void GciResizeGlut ( int width, int height )
{
    Display *dpy = __glutDisplay; 
    int scr = DefaultScreen(dpy);
    
    // Set the error handler
    
    XSetErrorHandler(ignoreXErrors);
    
    // Save the video mode

    memset(&origMode, 0, sizeof(origMode));
    XF86VidModeModeLine *l = (XF86VidModeModeLine*)((char*) &origMode + sizeof(origMode.dotclock));
    XF86VidModeGetModeLine(dpy, scr, (int*) &origMode.dotclock, l);
    savedMode = true;
    
    Window realRoot = RootWindow(dpy, scr);

    // Set the new video mode

    XF86VidModeModeInfo mode = GetXF86Mode(dpy, scr, width, height);
    XWarpPointer(dpy, None, realRoot, 0, 0, 0, 0, 0, 0);
    XF86VidModeSwitchToMode(dpy, scr, &mode);
    XWarpPointer(dpy, None, realRoot, 0, 0, 0, 0, 0, 0);	
    XSync(dpy, True);

    __glutScreenWidth = width;
    __glutScreenHeight = height;

    XSetWindowAttributes attr;
    attr.override_redirect = True;
    attr.background_pixel = BlackPixel(dpy, scr);
    attr.backing_store = Always;
    int attr_flags = CWOverrideRedirect | CWBackPixel | CWBackingStore;

    Window rootWindow =  XCreateWindow(__glutDisplay, realRoot,
				0, 0, width, height, 0,
				CopyFromParent,
				CopyFromParent,
				CopyFromParent,
				attr_flags,
				&attr);
    XSizeHints size_hints;
    size_hints.x = 0;
    size_hints.y = 0;
    size_hints.width = width;
    size_hints.height = height;
    size_hints.flags = PSize | PMinSize | PMaxSize;
    size_hints.min_width = width;
    size_hints.min_height = height;
    size_hints.max_width = width;
    size_hints.max_height = height;

    XSetWMProperties(dpy, rootWindow, 
		     NULL,
		     NULL,
		     0,
		     0,
		     &size_hints,
		     0,
		     0);

    XMapRaised(dpy, rootWindow);

    XSetInputFocus(dpy, rootWindow, RevertToParent, CurrentTime);
    XGrabPointer(dpy, rootWindow, true, 0, 
		 GrabModeAsync, GrabModeAsync, rootWindow, None, CurrentTime);

    XSelectInput(dpy, rootWindow, KeyPressMask|KeyReleaseMask);
    
    __glutRoot = rootWindow;
    GciSetCursor();
}

void GciRestoreGlut()
{
    if (savedMode) {
        savedMode = false;
        Display *dpy = __glutDisplay;
        int scr = DefaultScreen(dpy);
        XF86VidModeSwitchToMode(dpy, scr, &origMode);
        XSync(dpy,False);
    }
}

void GciStoreScreenSize ()
{
}

bool GciSetScreenSize ( int, int, int, int )
{
    return true;
}

void GciRestoreScreenSize()
{
    GciRestoreGlut();
}

#endif
