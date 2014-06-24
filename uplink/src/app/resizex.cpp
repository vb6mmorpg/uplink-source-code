#ifdef WIN32
void ResizeGlut ( int, int )
{
}
void RestoreGlut ( int, int )
{
}
#else
#include <iostream.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/xf86vmode.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/glx.h>

#include <stdlib.h>
#include <string.h>

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


extern Display *__glutDisplay;
extern int __glutScreenWidth, __glutScreenHeight;
extern Window __glutRoot;

static XF86VidModeModeInfo origMode;

// Sections From ClanLib (LGPL) (x11_resolution.cpp)
void ResizeGlut ( int width, int height )
{
    Display *dpy = __glutDisplay; 
    int scr = DefaultScreen(dpy);

    // Save the video mode

    memset(&origMode, 0, sizeof(origMode));
    XF86VidModeModeLine *l = (XF86VidModeModeLine*)((char*) &origMode + sizeof(origMode.dotclock));
    XF86VidModeGetModeLine(dpy, scr, (int*) &origMode.dotclock, l);

    // Set the new video mode

    XF86VidModeModeInfo mode = GetXF86Mode(dpy, scr, width, height);
    XWarpPointer(dpy, None, RootWindow(dpy, scr), 0, 0, 0, 0, 0, 0);
    XF86VidModeSwitchToMode(dpy, scr, &mode);
    XWarpPointer(dpy, None, RootWindow(dpy, scr), 0, 0, 0, 0, 0, 0);	
    XSync(dpy, False);

    __glutScreenWidth = width;
    __glutScreenHeight = height;

    Window realRoot = RootWindow(dpy, scr);

    XSetWindowAttributes attr;
    attr.override_redirect = False;
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
    XSync(dpy,False);

//    XSetInputFocus(dpy, rootWindow, RevertToParent, CurrentTime);
    XGrabPointer(dpy, rootWindow, true, 0, 
		 GrabModeAsync, GrabModeAsync, rootWindow, None, CurrentTime);

    XWarpPointer(dpy, None, rootWindow, 0, 0, 0, 0, width, height);	
    XSync(dpy,False);

    __glutRoot = rootWindow;
}

void RestoreGlut()
{
    cout << "Restoring X11 screen\n";
    Display *dpy = __glutDisplay;
    int scr = DefaultScreen(dpy);
    XF86VidModeSwitchToMode(dpy, scr, &origMode);
    XSync(dpy,False);
}

static void display(void)
{
	for ( int i = 0; i < 2; ++i ) {

		//  Draw the world

		glPushMatrix ();
		glLoadIdentity ();
		glMatrixMode ( GL_PROJECTION );
		glPushMatrix ();
		glLoadIdentity ();
		glPushAttrib ( GL_ALL_ATTRIB_BITS );
		
		glOrtho ( 0.0, 640, 480, 0.0, -1.0, 1.0 );

		glTranslatef ( 0.375, 0.375, 0.0 );

		
		glColor4f ( 1.0f, 0.0f, 0.0f, 1.0f );
		glBegin ( GL_QUADS ); 
		{
		    glVertex2i ( 0, 0 );
		    glVertex2i ( 639, 0 );
		    glVertex2i ( 639, 479 );
		    glVertex2i ( 0, 479 );
		} 
		glEnd ();

		glPopAttrib ();
		glPopMatrix ();
		glMatrixMode ( GL_MODELVIEW );
		glPopMatrix ();

		//  Swap the buffers, do it all again to the new back-buffer
		glutSwapBuffers();
	}
}

#ifdef TEST

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    ResizeGlut(640, 480);
    glutInitWindowSize(640, 480);
    glutInitWindowPosition(0, 0);
    glutCreateWindow( "Uplink" );

    glutFullScreen();
    glutDisplayFunc(display);
    glutMainLoop();
}

#endif
#endif // !WIN32
