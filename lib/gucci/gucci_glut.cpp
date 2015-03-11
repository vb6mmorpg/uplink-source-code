#ifndef USE_SDL
#include <GL/glut.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gucci.h"
#include "gucci_internal.h"

#include "mmgr.h"

static void * gucciStyleToGlut(int gucciStyle)
{
  switch (gucciStyle) {
  case BITMAP_13: return GLUT_BITMAP_8_BY_13;
  case BITMAP_15: return GLUT_BITMAP_9_BY_15;
  case TIMESROMAN_10: return GLUT_BITMAP_TIMES_ROMAN_10;
  case TIMESROMAN_24: return GLUT_BITMAP_TIMES_ROMAN_24;
  case HELVETICA_10:  return GLUT_BITMAP_HELVETICA_10;
  case HELVETICA_12:  return GLUT_BITMAP_HELVETICA_12;
  case HELVETICA_18:  return GLUT_BITMAP_HELVETICA_18;
  default: return GLUT_BITMAP_8_BY_13;
  }
}

static int glutKeyToGucci(int key)
{
  switch (key) {
  case GLUT_KEY_F1: return GCI_KEY_F1;
  case GLUT_KEY_F2: return GCI_KEY_F2;
  case GLUT_KEY_F3: return GCI_KEY_F3;
  case GLUT_KEY_F4: return GCI_KEY_F4;
  case GLUT_KEY_F5: return GCI_KEY_F5;
  case GLUT_KEY_F6: return GCI_KEY_F6;
  case GLUT_KEY_F7: return GCI_KEY_F7;
  case GLUT_KEY_F8: return GCI_KEY_F8;
  case GLUT_KEY_F9: return GCI_KEY_F9;
  case GLUT_KEY_F10: return GCI_KEY_F10;
  case GLUT_KEY_F11: return GCI_KEY_F11;
  case GLUT_KEY_F12: return GCI_KEY_F12;
  default: return 1000 + key;
  }
}

static int glutButtonToGucci(int button)
{
  switch (button) {
  case GLUT_LEFT_BUTTON: return GCI_LEFT_BUTTON;
  case GLUT_RIGHT_BUTTON: return GCI_RIGHT_BUTTON;
  case GLUT_MIDDLE_BUTTON: return GCI_MIDDLE_BUTTON;
  default: abort();
  }
  return 0;
}

static int glutStateToGucci(int state)
{
  switch (state) {
  case GLUT_UP: return GCI_UP;
  case GLUT_DOWN: return GCI_DOWN;
  default: abort();
  }
  return 0;
}

void GciInitGraphics( const char *caption,
		      int graphics_flags, int screenWidth, int screenHeight, 
		      int screenDepth, int screenRefresh, int argc, char *argv[] )
{
  bool debugging = (graphics_flags & GCI_DEBUGSTART) != 0;
  bool runFullScreen = (graphics_flags & GCI_FULLSCREEN) != 0;

  if ( debugging ) printf ( "Gucci is storing the current screen settings..." );
  GciStoreScreenSize ();
  if ( debugging ) printf ( "done\n" );

  if ( runFullScreen ) {

    if ( debugging ) printf ( "GUCCI is now setting the screen size..." );
    GciSetScreenSize ( screenWidth, screenHeight, screenDepth, screenRefresh );

    if ( debugging ) printf ( "done\n" );

  }

  if ( debugging ) printf ( "Initialising GLUT..." );
  glutInit(&argc, argv);
  
  int glutFlags = 0;
  if (graphics_flags & GCI_DOUBLE) glutFlags |= GLUT_DOUBLE;
  if (graphics_flags & GCI_RGB) glutFlags |= GLUT_RGB;

  glutInitDisplayMode(glutFlags);
  if ( debugging ) printf ( "done\n ");

  /*
  if ( runFullScreen )
    GciResizeGlut( screenWidth, screenHeight );
  */

  if ( debugging ) printf ( "GLUT is now opening a %dx%d window...", screenWidth, screenHeight );
  glutInitWindowSize( screenWidth, screenHeight );
  glutInitWindowPosition(0, 0);
  glutCreateWindow( caption );
  if ( debugging ) printf ( "done\n ");

  if ( runFullScreen ) {
    if ( debugging ) printf ( "GLUT is now changing into fullscreen mode..." );
    glutFullScreen ();
    if ( debugging ) printf ( "done\n" );

  }
}

void GciFallbackDrawText ( int x, int y, char *text, int STYLE )
{
  void *glutStyle = gucciStyleToGlut(STYLE);

  // Use ordinary fonts

  glRasterPos2f(x, y);

  unsigned len = strlen(text);

  for ( unsigned i = 0; i < len; i++) {

    glutBitmapCharacter( glutStyle, text[i]);

    if ( text[i] == ' ' ) {

      float rasterpos [4];
      glGetFloatv ( GL_CURRENT_RASTER_POSITION, rasterpos );

      glRasterPos2f( rasterpos [0] + GciFallbackTextWidth (" ", STYLE),
		     glutGet((GLenum) GLUT_WINDOW_HEIGHT) - rasterpos [1] );

    }

  }
}

int GciFallbackTextWidth ( char *text, int STYLE )
{
  return glutBitmapLength ( (void *) STYLE, (const unsigned char *) text );
}

#define GUCCI_GLUT_FUNC(Function) \
void Gci##Function##Func( Gci##Function##FuncT * f) \
{ \
  glut##Function##Func(f); \
}

GUCCI_GLUT_FUNC(Display);
GUCCI_GLUT_FUNC(Motion);
GUCCI_GLUT_FUNC(PassiveMotion);
GUCCI_GLUT_FUNC(Keyboard);
GUCCI_GLUT_FUNC(Idle);
GUCCI_GLUT_FUNC(Reshape);

static GciSpecialFuncT *gciSpecialHandlerP = 0;
static GciMouseFuncT *gciMouseHandlerP = 0;

static void GciSpecialWrapper(int key, int x, int y)
{
  if (gciSpecialHandlerP)
    (*gciSpecialHandlerP)(glutKeyToGucci(key), x, y);
}

static void GciMouseWrapper(int button, int state, int x, int y)
{
  if (gciMouseHandlerP)
    (*gciMouseHandlerP)(glutButtonToGucci(button),
			glutStateToGucci(state), x, y);
}

void GciMouseFunc(GciMouseFuncT *f)
{
  gciMouseHandlerP = f;
  glutMouseFunc(GciMouseWrapper);
}

void GciSpecialFunc(GciSpecialFuncT *f)
{
  gciSpecialHandlerP = f;
  glutSpecialFunc(GciSpecialWrapper);
}

bool GciLayerDamaged()
{
  return glutLayerGet(GLUT_NORMAL_DAMAGED) != 0;
}

void GciSwapBuffers()
{
  glutSwapBuffers();  
}

void GciPostRedisplay()
{
  glutPostRedisplay ();
}

void GciTimerFunc(unsigned int millis, GciCallbackT *callback, int value)
{
  glutTimerFunc(millis, callback, value);
}

void GciMainLoop()
{
  glutMainLoop ();
};
#endif // USE_SDL