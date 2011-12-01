#ifdef WIN32
#include <windows.h>
#endif

#include <stdio.h>

#include <GL/gl.h>

#include <map>

#include "gucci.h"
#include "gucci_internal.h"
#include "tosser.h"

#include <FTGL/ftgl.h>

static std::map<int, FTGLBitmapFont *> fonts;

#include <math.h>

using namespace std;

#include "mmgr.h"

int  gci_defaultfont = HELVETICA_12;
bool gci_truetypeenabled = false;

void GciSetDefaultFont ( int STYLE )
{

	gci_defaultfont = STYLE;

}

void GciDrawText ( int x, int y, char *text )
{

	GciDrawText ( x, y, text, gci_defaultfont );

}

int GciTextWidth ( char *text )
{
	
	return GciTextWidth ( text, gci_defaultfont );

}

void GciEnableTrueTypeSupport ()
{

	gci_truetypeenabled = true;

}

void GciDisableTrueTypeSupport ()
{

	gci_truetypeenabled = false;

}

bool GciRegisterTrueTypeFont( const char *filename )
{
    return true;
}

bool GciUnregisterTrueTypeFont( const char *filename )
{
    return true;
}

void GciDrawText ( int x, int y, char *text, int STYLE )
{

    if ( gci_truetypeenabled && fonts[STYLE] ) {
            
        // Use true type fonts
        FTGLBitmapFont *font = fonts[STYLE];
        //FTGLPixmapFont *font = fonts[STYLE];
        glRasterPos2i(x, y);
        font->Render(text);
    }
    else {
        GciFallbackDrawText( x, y, text, STYLE );
    }
}

int GciTextWidth ( char *text, int STYLE )
{
  if (fonts[STYLE]) {
    float llx, lly, llz, urx, ury, urz;
    fonts[STYLE]->BBox( text, llx, lly, llz, urx, ury, urz );
    return (int)(fabs(llx - urx) + 0.5);
  }
    else 
    return GciFallbackTextWidth( text, STYLE );
}

bool GciLoadTrueTypeFont ( int index, char *fontname, char *filename, int size )
{

    if (gci_truetypeenabled) {
        int pointSize = int (size * 72.0 / 96.0 + 0.5);
        
        FTGLBitmapFont *font = new FTGLBitmapFont(filename);
        //FTGLPixmapFont *font = new FTGLPixmapFont(filename);
        if (font->Error() != 0 || !font->FaceSize(pointSize, 96)) {
            delete font;
            return false;
        }
            
        GciDeleteTrueTypeFont(index);
        fonts[index] = font;

        return true;
    }
    else {
        printf ( "GciLoadTrueTypeFont called, but truetypes are not enabled\n" );
        return false;
    }
}

void GciDeleteTrueTypeFont ( int index )
{
    if (fonts[index]) 
        delete fonts[index];
}

void GciDeleteAllTrueTypeFonts ()
{
    // Delete all the frickin' fonts
    for (map<int, FTGLBitmapFont *>::iterator x = fonts.begin(); x != fonts.end(); x++)
        GciDeleteTrueTypeFont(x->first);
    fonts.clear ();

}
