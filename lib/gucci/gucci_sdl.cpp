#ifdef USE_SDL

#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#include <sys/resource.h>
#endif

#include <list>
#include <iostream>

#include <SDL/SDL.h>
#include <GL/gl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gucci.h"
#include "gucci_internal.h"

using namespace std;

#include "mmgr.h"

#define GciAbs(num) (((num)>0)?(num):-(num))


static SDL_Surface *screen = NULL;

static unsigned _GciGetAccurateTime ();

static bool _GciIsInitGraphicsLibrary = false;

static int sdlKeyToGucci(int key)
{
  switch (key) {
  case SDLK_F1: return GCI_KEY_F1;
  case SDLK_F2: return GCI_KEY_F2;
  case SDLK_F3: return GCI_KEY_F3;
  case SDLK_F4: return GCI_KEY_F4;
  case SDLK_F5: return GCI_KEY_F5;
  case SDLK_F6: return GCI_KEY_F6;
  case SDLK_F7: return GCI_KEY_F7;
  case SDLK_F8: return GCI_KEY_F8;
  case SDLK_F9: return GCI_KEY_F9;
  case SDLK_F10: return GCI_KEY_F10;
  case SDLK_F11: return GCI_KEY_F11;
  case SDLK_F12: return GCI_KEY_F12;
  default: return 1000 + key;
  }
}

static int sdlButtonToGucci(int button)
{
  switch (button) {
  case SDL_BUTTON_LEFT: return GCI_LEFT_BUTTON;
  case SDL_BUTTON_MIDDLE: return GCI_MIDDLE_BUTTON;
  case SDL_BUTTON_RIGHT: return GCI_RIGHT_BUTTON;
  default: return GCI_UNKNOWN;
  }
}

static int sdlMouseEventToGucci(int eventType)
{
  switch (eventType) {
  case SDL_MOUSEBUTTONDOWN: return GCI_DOWN;
  case SDL_MOUSEBUTTONUP: return GCI_UP;
  default: return GCI_UNKNOWN;
  }
}

char *GciInitGraphicsLibrary ( int graphics_flags )
{
  bool debugging = (graphics_flags & GCI_DEBUGSTART) != 0;

  if ( debugging ) printf ( "Initialising SDL..." );
  //if ((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER)==-1)) { 
#ifdef _DEBUG
  if ((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_NOPARACHUTE)==-1)) { 
#else
  if ((SDL_Init(SDL_INIT_VIDEO)==-1)) { 
#endif
    //printf("Could not initialize SDL: %s.\n", SDL_GetError());
    //exit(-1);
    char message[] = "Could not initialize SDL: %s.";
    char *errorMessage = NULL;
    char *sdlErrorMessage = SDL_GetError ();
	size_t sizeErrorMessage = sizeof ( message ) + strlen ( sdlErrorMessage ) + 1;
    errorMessage = new char [ sizeErrorMessage ];
#ifdef WIN32
	_snprintf ( errorMessage, sizeErrorMessage, message, sdlErrorMessage );
#else
	snprintf ( errorMessage, sizeErrorMessage, message, sdlErrorMessage );
#endif
	errorMessage [ sizeErrorMessage - 1 ] = '\0';
	return errorMessage;
  }
  if ( debugging ) printf ( "done\n ");

  _GciIsInitGraphicsLibrary = true;
  return NULL;
}

char *GciInitGraphics( const char *caption, int graphics_flags, int screenWidth, int screenHeight, 
		      int screenDepth, int screenRefresh, int argc, char *argv[] )
{
  bool debugging = (graphics_flags & GCI_DEBUGSTART) != 0;
  bool runFullScreen = (graphics_flags & GCI_FULLSCREEN) != 0;

  int sdlFlags = 0;
  if (graphics_flags & GCI_DOUBLE) {
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //sdlFlags |= SDL_DOUBLEBUF;
  }

  const SDL_VideoInfo* info = NULL;
  info=SDL_GetVideoInfo();

  if (screenDepth == -1)
    screenDepth = info->vfmt->BitsPerPixel;

  if (graphics_flags & GCI_RGB) {
    switch(screenDepth) {
    case 24:
    case 32:
      SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
      SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
      SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
      break;
	
    case 16:
    default:
      SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
      SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
      SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
	  screenDepth = 16;
      break;
    }
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
  }
  if (graphics_flags & GCI_FULLSCREEN) sdlFlags |= SDL_FULLSCREEN;

  int closestbpp = SDL_VideoModeOK(screenWidth, screenHeight, screenDepth, sdlFlags | SDL_OPENGL);
  if (closestbpp == 0) {
    printf("Warning, no available video mode for width: %d, height:%d, flags:%d\n", screenWidth, screenHeight, sdlFlags | SDL_OPENGL);
  }
  else if (screenDepth != closestbpp) {
    printf("Warning, difference in depth between the video mode requested %d and the closest available %d for width: %d, height:%d, flags:%d\n", screenDepth, closestbpp, screenWidth, screenHeight, sdlFlags | SDL_OPENGL);

    switch(closestbpp) {
    case 24:
    case 32:
      SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
      SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
      SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
      break;

    case 16:
      SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
      SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
      SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
      break;

    default:
      SDL_GL_SetAttribute( SDL_GL_RED_SIZE, closestbpp / 3 );
      SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, closestbpp / 3 );
      SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, closestbpp / 3 );
      break;
    }

    screenDepth = closestbpp;
  }

  if ( debugging ) printf ( "SDL is now opening a %dx%d window in %d depth ...", screenWidth, screenHeight, screenDepth );
  screen = SDL_SetVideoMode(screenWidth, screenHeight, screenDepth, sdlFlags | SDL_OPENGL);
  if (screen == NULL) {
    //printf("Could not initialize SDL Video: %s.\n", SDL_GetError());
    //exit(-1);
    char message[] = "Could not initialize SDL Video: %s.";
    char *errorMessage = NULL;
    char *sdlErrorMessage = SDL_GetError ();
	size_t sizeErrorMessage = sizeof ( message ) + strlen ( sdlErrorMessage ) + 1;
    errorMessage = new char [ sizeErrorMessage ];
#ifdef WIN32
	_snprintf ( errorMessage, sizeErrorMessage, message, sdlErrorMessage );
#else
	snprintf ( errorMessage, sizeErrorMessage, message, sdlErrorMessage );
#endif
	errorMessage [ sizeErrorMessage - 1 ] = '\0';
	return errorMessage;
  }
  if ( debugging ) printf ( "done\n ");  

  if ( debugging ) printf ( "SDL is now changing the window caption and diverse settings ...");  

  SDL_WM_SetCaption(caption, 0);
  
  SDL_EnableUNICODE(1);
  SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);

  if ( debugging ) printf ( "done\n ");

  return NULL;
}


extern int gci_defaultfont; // The default Gucci style

void GciFallbackDrawText ( int x, int y, char *text, int STYLE )
{
  static bool inside = false;

  if (!inside) {
    inside = true;
    GciDrawText(x, y, text, gci_defaultfont);
    inside = false;
  }
  else {
    // We recursed back here, not even the
    // default font is supported!
    abort();
  }
}

int GciFallbackTextWidth ( char *text, int STYLE )
{
  static bool inside = false;

  if (!inside) {
    inside = true;
    int result = GciTextWidth(text, gci_defaultfont);
    inside = false;
	return inside;
  }
  else {
    // We recursed back here, not even the 
    // default font is supported!
    abort();
    return 0;
  }
}

#define GUCCI_FUNC(Function) \
static Gci##Function##FuncT *gci##Function##HandlerP = 0; \
void Gci##Function##Func( Gci##Function##FuncT * f) \
{ \
    gci##Function##HandlerP = f; \
}

GUCCI_FUNC(Display);
GUCCI_FUNC(Motion);
GUCCI_FUNC(PassiveMotion);
GUCCI_FUNC(Keyboard);
GUCCI_FUNC(Idle);
GUCCI_FUNC(Reshape);
GUCCI_FUNC(Mouse);
GUCCI_FUNC(Special);

static bool gciRedisplay = true;
static bool displayDamaged = false;
static bool finished = false;

bool GciLayerDamaged()
{
  return displayDamaged;
}

void GciSwapBuffers()
{
  SDL_GL_SwapBuffers();
  displayDamaged = false;
}

void GciPostRedisplay()
{
  gciRedisplay = displayDamaged = true;
}

class Callback {
public:
  Callback(unsigned duration, GciCallbackT *callback, int value)
    : callback(callback), value(value)
  {
    expiryTime = SDL_GetTicks() + duration;
  };

  bool expired() {
    return SDL_GetTicks() > expiryTime;
  };

  void invoke();

private:
  GciCallbackT *callback;
  int value;
  unsigned expiryTime;
};

typedef list<Callback *> TimerList;
static TimerList timerEvents;

static void GciProcessTimerEvents()
{
  for (TimerList::iterator i = timerEvents.begin(); i != timerEvents.end(); ) {
    Callback *c = *i;
    if (c->expired()) {
      i = timerEvents.erase(i);
      c->invoke();
      delete c;
    }
    else
      i++;
  }
}

void Callback::invoke()
{
  (*callback)(value);
};

void GciTimerFunc(unsigned int millis, GciCallbackT *callback, int value)
{
  timerEvents.push_back(new Callback(millis, callback, value));
}

void GciMainLoop()
{
  finished = false;

  unsigned lastFrameTime = _GciGetAccurateTime();
  unsigned frameRate = 35;
  unsigned framePeriod = 1000 / frameRate;

  while (!finished) {
//     unsigned now = _GciGetAccurateTime();
//     unsigned frameDuration = now - lastFrameTime;

//     if (frameDuration > framePeriod) {
//       lastFrameTime = now;
    if (gciDisplayHandlerP)
	(*gciDisplayHandlerP)();

    SDL_Event event;

    /* Check for events */
    while(SDL_PollEvent(&event) && !finished){  /* Loop until there are no events left on the queue */
      switch(event.type){  /* Process the appropiate event type */
      case SDL_KEYDOWN: {
	int x, y;
	SDL_GetMouseState(&x, &y);

	Uint16 unicode = event.key.keysym.unicode;
	if ( ( event.key.keysym.mod & KMOD_NUM ) == KMOD_NUM ) {
		if ( SDLK_KP0 <= event.key.keysym.sym && event.key.keysym.sym <= SDLK_KP9 )
			unicode = SDLK_0 + ( event.key.keysym.sym - SDLK_KP0 );
		else if ( event.key.keysym.sym == SDLK_KP_PERIOD )
			unicode = SDLK_PERIOD;
		else if ( event.key.keysym.sym == SDLK_KP_ENTER )
			unicode = SDLK_RETURN;
		else if ( event.key.keysym.sym == SDLK_KP_PLUS )
			unicode = SDLK_PLUS;
		else if ( event.key.keysym.sym == SDLK_KP_MINUS )
			unicode = SDLK_MINUS;
		else if ( event.key.keysym.sym == SDLK_KP_MULTIPLY )
			unicode = SDLK_ASTERISK;
		else if ( event.key.keysym.sym == SDLK_KP_DIVIDE )
			unicode = SDLK_SLASH;
		else if ( event.key.keysym.sym == SDLK_KP_EQUALS )
			unicode = SDLK_EQUALS;
	}

	if (unicode == 0) {
	  if (gciSpecialHandlerP) 
	    (*gciSpecialHandlerP)(sdlKeyToGucci(event.key.keysym.sym), x, y);
	}
	else {
	  if (gciKeyboardHandlerP) 
	    (*gciKeyboardHandlerP)((unsigned char) unicode, x, y);
	}
      }
	break;
      case SDL_MOUSEMOTION: 
	if (gciMotionHandlerP) {
	  /* I have a feeling we should only call this if 
	     a mouse button is depressed */
	  	  (*gciMotionHandlerP)(event.motion.x, event.motion.y);
	}
	if (gciPassiveMotionHandlerP) {
	  (*gciPassiveMotionHandlerP)(event.motion.x, event.motion.y);
	}
	break;
      case SDL_MOUSEBUTTONDOWN:
      case SDL_MOUSEBUTTONUP:
	if (gciMouseHandlerP) {
	  (*gciMouseHandlerP)(sdlButtonToGucci(event.button.button),
			      sdlMouseEventToGucci(event.button.type),
			      event.button.x,
			      event.button.y);
	}
	break;
      case SDL_QUIT:
	finished = true;
	break;
      case SDL_VIDEOEXPOSE:
      case SDL_VIDEORESIZE:
	displayDamaged = true;
	break;
      default: /* Report an unhandled event */
	break;
      }
    }

	if ( !finished ) {
		/* Let other processes run */
		// SDL_Delay(1);

		GciProcessTimerEvents();

		if (gciIdleHandlerP)
		  (*gciIdleHandlerP)();

	//     if (!gciRedisplay) {
	// 	    SDL_WaitEvent(&event);
	// 	    SDL_PushEvent(&event);
	//     }
	}
  }
}

void GciRestoreScreenSize()
{
  if ( _GciIsInitGraphicsLibrary && SDL_WasInit(SDL_INIT_VIDEO) ) {
    // A try to remove some exit crashes
    int value = 0;
    if ( SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER,&value) == 0 )
		if ( value != 0 )
			SDL_GL_SwapBuffers();

    SDL_QuitSubSystem(SDL_INIT_VIDEO);
  }
  _GciIsInitGraphicsLibrary = false;
  finished = true;
}

void GciStoreScreenSize ()
{
}

bool GciSetScreenSize ( int width, int height,
                        int bpp, int refresh )
{
  return true;
}

void GciResizeGlut ( int width, int height )
{
}

void GciRestoreGlut()
{
}

static unsigned _GciGetAccurateTime ()
{

#ifdef WIN32

	static bool supportsHighResTime = false;
	static bool initted = false;

	static LARGE_INTEGER frequency;
	static LARGE_INTEGER firstPerformanceCount;
	static DWORD firstTickCount = 0;

	if ( !initted ) {
		initted = true;

		if ( QueryPerformanceFrequency ( &frequency ) && 
		     QueryPerformanceCounter ( &firstPerformanceCount ) ) {

			supportsHighResTime = true;
		}
		else {
			firstTickCount = GetTickCount();
		}
	}

	if ( supportsHighResTime ) {
		LARGE_INTEGER performanceCount;
		QueryPerformanceCounter ( &performanceCount );

		return (unsigned) ( 1000 * ( performanceCount.QuadPart - firstPerformanceCount.QuadPart ) / frequency.QuadPart );
	}
	else {
		return GetTickCount() - firstTickCount;
	}

	/*
	//return 1000 * ( (float) clock () / (float) CLOCKS_PER_SEC );

	LARGE_INTEGER lpFrequency;
	LARGE_INTEGER lpPerformanceCount;
	if ( QueryPerformanceFrequency ( &lpFrequency ) != 0 && QueryPerformanceCounter ( &lpPerformanceCount ) != 0 ) {

		return (unsigned) ( ( lpPerformanceCount.QuadPart * 1000 ) / lpFrequency.QuadPart );

	}

	return GetTickCount ();
	*/

#else

	// Linux version

	static bool initted = false; 
	static struct timeval startTime; 
	struct timeval tv; 
  
	if (!initted) { 
		initted = true; 
		gettimeofday(&startTime, NULL); 
		return 0; 
	} 

	gettimeofday(&tv, NULL); 
  
	long diff_usec = tv.tv_usec - startTime.tv_usec; 
	if (diff_usec < 0) { 
		diff_usec += 1000000; 
		tv.tv_sec --; 
	} 
	long diff_sec = tv.tv_sec - startTime.tv_sec; 

	return 1000 * diff_sec + diff_usec / 1000; 

#endif

}


GciScreenModeList *GciListScreenModes() {

	GciScreenModeList *resolutions = new DArray<GciScreenMode *>;

	SDL_Rect **modes;
	/* Get available fullscreen/hardware modes */
	modes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE|SDL_OPENGL);

	/* Check is there are any modes available
	   and try non-hardware modes if not. */
	if(modes == (SDL_Rect **)0){
		printf( "SDL Resolutions: not using HW flag.\n" );
		modes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_OPENGL);
	}

	for( unsigned i = 0; modes[i]; ++i ) {
		// We don't need the really small resolutions
		if (modes[i]->w >= 640 && modes[i]->h >= 480) {
			GciScreenMode *newMode = new GciScreenMode;
			newMode->w = modes[i]->w;
			newMode->h = modes[i]->h;
			resolutions->PutData( newMode );
		}
	}

	return resolutions;
}

GciScreenMode *GciGetClosestScreenMode ( int width, int height ) {

	GciScreenMode *newMode = new GciScreenMode;
	newMode->w = width;
	newMode->h = height;

	SDL_Rect **modes;
	/* Get available fullscreen/hardware modes */
	modes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE|SDL_OPENGL);

	/* Check is there are any modes available
	   and try non-hardware modes if not. */
	if(modes == (SDL_Rect **)0){
		printf( "SDL Resolutions: not using HW flag.\n" );
		modes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_OPENGL);
	}

	int curdeltaW = -1, curdeltaH = -1;
	for( unsigned i = 0; modes[i]; ++i ) {
		// We don't need the really small resolutions
		
		int deltaW = GciAbs ( width - modes[i]->w );
		int deltaH = GciAbs ( height - modes[i]->h );
		if ( curdeltaW == -1 || curdeltaH == -1 || ( deltaW + deltaH ) < ( curdeltaW + curdeltaH ) ) {
			curdeltaW = deltaW;
			curdeltaH = deltaH;
			newMode->w = modes[i]->w;
			newMode->h = modes[i]->h;
		}
	}

	return newMode;
}

void GciDeleteScreenModeArrayData(GciScreenModeList *modes) {

	if (modes == NULL) return;

	for ( int i = 0; i < modes->Size(); ++i )
		if ( modes->ValidIndex( i ) )
			if ( modes->GetData( i ) )
				delete modes->GetData( i );
	modes->Empty();
}

void GciSaveScreenshot( const char * file ) {

	SDL_Surface *temp;
	unsigned char *pixels;
	int i;

	int w = screen->w;
	int h = screen->h;
	
	if (!(screen->flags & SDL_OPENGL))
	{
		//SDL_SaveBMP(temp, file);
		return;
	}
		
	temp = SDL_CreateRGBSurface(SDL_SWSURFACE, screen->w, screen->h, 24,
#if SDL_BYTEORDER == SDL_LIL_ENDIAN
	0x000000FF, 0x0000FF00, 0x00FF0000, 0
#else
	0x00FF0000, 0x0000FF00, 0x000000FF, 0
#endif
	);
	if (temp == NULL)
		return;

	pixels = (unsigned char *)malloc(3 * w * h);
	if (pixels == NULL)
	{
		SDL_FreeSurface(temp);
		return;
	}

	glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	for (i=0; i<h; i++)
		memcpy(((char *) temp->pixels) + temp->pitch * i, pixels + 3*w * (h-i-1), w*3);
	free(pixels);

	SDL_SaveBMP(temp, file);
	SDL_FreeSurface(temp);
	return;
}

bool GciAppVisible ()
{

	if ( ( SDL_GetAppState () & SDL_APPACTIVE ) == SDL_APPACTIVE )
		return true;

	return false;

}

#endif // USE_SDL
