// Image.cpp: implementation of the Image class.


//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
#include <windows.h>
#endif

#ifndef HAVE_GLES
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <GLES/gl.h>
#include <GLES/glues.h>
#endif

#include "tiff.h"
#include "tiffio.h"

#include "image.h"

#include "mmgr.h"

int pow2(int n) {
	int x = 1;

	while(x < n) {
		x <<= 1;
	}

	return x;
}

void etest(int pos) {
	return;
	int err = glGetError();
	if (err) {
		printf("GL error @%d: %d\n", pos, err);
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Image::Image()
{

	pixels = NULL;
	rgb_pixels = NULL;
#ifdef HAVE_GLES
	texture = 0;
	hscale = 1.0f;
	wscale = 1.0f;
	glGenTextures(1, &texture);
	// printf("Created: %d\n", texture);
#endif
}

Image::Image( const Image& img )
{
	width = img.width;
	height = img.height;
	alpha = img.alpha;

	pixels = NULL;
	if ( img.pixels ) {
		pixels = new unsigned char [ width * height * 4 ];
		memcpy ( pixels, img.pixels, width * height * 4 );
	}

	rgb_pixels = NULL;
	if ( img.rgb_pixels ) {
	    rgb_pixels = new unsigned char [ width * height * 3 ];
		memcpy ( rgb_pixels, img.rgb_pixels, width * height * 3 );
	}

#ifdef HAVE_GLES
	GenTexture();
#endif
}

Image::~Image()
{

	if ( pixels )
		delete [] pixels;
	if ( rgb_pixels )
		delete [] rgb_pixels;
#ifdef HAVE_GLES
	if ( texture ) {
		// printf("Deleting: %d\n", texture);
		glDeleteTextures(1, &texture);
	}
#endif
}

#ifdef HAVE_GLES
void Image::GenTexture() {
	wscale = (float)width / (float)pow2(width);
	hscale = (float)height / (float)pow2(height);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	etest(1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pow2(width), pow2(height), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	etest(2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	etest(3);
	//printf("Gen texture: (%d x %d) (%d, %d) (%.02f, %.02f)\n", width, height, pow2(width), pow2(height), wscale, hscale);
	UpdateTexture();
}

void Image::UpdateTexture() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	etest(4);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	etest(5);
	//printf("Update texture: (%d x %d) (%d, %d)\n", width, height, pow2(width), pow2(height));
	glDisable(GL_TEXTURE_2D);
}
#endif

void Image::LoadRAW ( char *filename, int sizex, int sizey )
{
	width = sizex;
	height = sizey;

	if ( rgb_pixels ) {
		delete [] rgb_pixels;
		rgb_pixels = NULL;
	}

	if ( pixels )
		delete [] pixels;
	pixels = new unsigned char [sizex * sizey * 4];

	FILE *file = fopen ( filename, "rb" );

	if ( !file ) {
		printf ( "GUCCI Error - failed to load RAW image %s\n", filename );
		//exit(255);
		CreateErrorBitmap ();
		return;
	}

	for ( int y = 0; y < sizey; ++y ) {
		for ( int x = 0; x < sizex; ++x ) {

			for ( int i = 0; i < 3; ++i ) {
				
				int thechar = fgetc ( file );
				pixels [((height-y-1) * width + x) * 4 + i] = (unsigned char) thechar;

			}

			pixels [((height-y-1) * width + x) * 4 + 3] = alpha;

		}
	}

	fclose ( file );

#ifdef HAVE_GLES
	GenTexture();
#endif
}

void Image::LoadTIF ( char *filename )
{

    char emsg[1024];		
	TIFFRGBAImage img;

    TIFF *tif = TIFFOpen(filename, "r");
    if ( !tif ) {
        printf ( "GUCCI Error - failed to load TIF %s\n", filename );
		CreateErrorBitmap ();
		return;
	}

    TIFFRGBAImageBegin (&img, tif, 0, emsg);
    int npixels = img.width * img.height;    
	uint32 *raster = new uint32 [npixels * sizeof(uint32)];
    
	TIFFRGBAImageGet(&img, raster, img.width, img.height);

	// Now convert the TIFF data into RAW data

	width = img.width;
	height = img.height;

	if ( rgb_pixels ) {
		delete [] rgb_pixels;
		rgb_pixels = NULL;
	}

	if ( pixels )
		delete [] pixels;
	pixels = (unsigned char *) raster;

	// Close down all those horrible TIF structures

    TIFFRGBAImageEnd(&img);
	TIFFClose ( tif );

#ifdef HAVE_GLES
	GenTexture();
#endif
}

void Image::SetAlpha ( float newalpha )
{

	alpha = (unsigned char) ( newalpha * 256.0 );
	unsigned char a = (unsigned char) ( newalpha * 255.0 );
	
	if ( pixels ) {

		for ( int x = 0; x < width; ++x ) 
			for ( int y = 0; y < height; ++y )
				pixels [( y * width + x) * 4 + 3] = a;

	}

#ifdef HAVE_GLES
	UpdateTexture();
#endif
}

void Image::SetAlphaBorderRec ( int x, int y, unsigned char a, unsigned char r, unsigned char g, unsigned char b )
{

	if ( 0 <= x && x < width && 0 <= y && y < height ) {
		unsigned char *curpixel = &pixels [( y * width + x) * 4];
		if ( curpixel[3] != a && curpixel[0] == r && curpixel[1] == g && curpixel[2] == b ) {
			curpixel[3] = a;
			SetAlphaBorderRec ( x - 1, y, a, r, g, b );
			SetAlphaBorderRec ( x + 1, y, a, r, g, b );
			SetAlphaBorderRec ( x, y - 1, a, r, g, b );
			SetAlphaBorderRec ( x, y + 1, a, r, g, b );
		}
	}

#ifdef HAVE_GLES
	UpdateTexture();
#endif
}

void Image::SetAlphaBorder ( float newalpha, float testred, float testgreen, float testblue )
{

	unsigned char a = (unsigned char) ( newalpha * 255.0 );
	unsigned char r = (unsigned char) ( testred * 255.0 );
	unsigned char g = (unsigned char) ( testgreen * 255.0 );
	unsigned char b = (unsigned char) ( testblue * 255.0 );
	
	if ( pixels ) {

		for ( int x = 0; x < width; ++x ) {
			SetAlphaBorderRec ( x, 0, a, r, g, b );
			SetAlphaBorderRec ( x, height - 1, a, r, g, b );
		}

		for ( int y = 0; y < height; ++y ) {
			SetAlphaBorderRec ( 0, y, a, r, g, b );
			SetAlphaBorderRec ( width - 1, y, a, r, g, b );
		}

	}

#ifdef HAVE_GLES
	UpdateTexture();
#endif
}

float Image::GetAlpha ()
{

	return float ( alpha / 256.0 );

}

int Image::Width ()
{
	
	return width;

}

int Image::Height ()
{

	return height;

}

void Image::FlipAroundH ()
{
	if ( pixels ) {

		unsigned char *newpixels = new unsigned char [width * height * 4];
		
		for ( int y = 0; y < height; ++y ) {

			unsigned char *source = pixels + y * width * 4;
			unsigned char *dest = newpixels + (width * (height-1) * 4) - (y * width * 4);

			memcpy ( (void *) dest, (void *) source, width * 4 );

		}

		if ( rgb_pixels ) {
			delete [] rgb_pixels;
			rgb_pixels = NULL;
		}

		delete [] pixels;
		pixels = newpixels;

	}

#ifdef HAVE_GLES
	UpdateTexture();
#endif
}

void Image::Scale ( int newwidth, int newheight )
{
#ifdef HAVE_GLES
	width = newwidth;
	height = newheight;
	return;
#endif

	if ( pixels ) {

		unsigned char *newpixels = new unsigned char [newwidth * newheight * 4];

/*
		for ( int x = 0; x < newwidth; ++x ) {
			for ( int y = 0; y < newheight; ++y ) {

				int scaleX = ((float) x / (float) newwidth) * width;
				int scaleY = ((float) y / (float) newheight) * height;

				for ( int i = 0; i < 4; ++i )
					newpixels [ (y * newwidth + x) * 4 + i ] = pixels [ (scaleY * width + scaleX) * 4 + i ];

			}
		}
*/
		int result = gluScaleImage ( GL_RGBA, width, height, GL_UNSIGNED_BYTE, pixels, newwidth, newheight, GL_UNSIGNED_BYTE, newpixels );
		char *resultc = (char *) gluErrorString ( (GLenum) result );

		if ( rgb_pixels ) {
			delete [] rgb_pixels;
			rgb_pixels = NULL;
		}

		delete [] pixels;
		pixels = newpixels;

		width = newwidth;
		height = newheight;

	}
		
}

void Image::ScaleToOpenGL ()
{

	int twidth, theight;

	if		( width <= 32 )  twidth = 64;
	else if ( width <= 128 ) twidth = 128;
	else					 twidth = 256;

	if		( height <= 32 )  theight = 64;
	else if ( height <= 128 ) theight = 128;
	else					  theight = 256;

	Scale ( twidth, theight );

}

void Image::Draw ( int x, int y )
{
#ifndef HAVE_GLES
	if ( pixels ) {

		glPushAttrib ( GL_ALL_ATTRIB_BITS );
		glDisable ( GL_BLEND );

		glRasterPos2i ( x, y + height );
		glDrawPixels ( width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels );

		glPopAttrib ();
#else
	if (texture) {
		GLfloat verts[] = {
			x, y + height,
			x + width, y + height,
			x + width, y,
			x, y
		};

		GLfloat tex[] = {
			0.0f, 0.0f,
			wscale, 0.0f,
			wscale, hscale,
			0.0f, hscale
		};

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture);
		etest(6);

		glDisableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		etest(7);

		glVertexPointer(2, GL_FLOAT, 0, verts);
		glTexCoordPointer(2, GL_FLOAT, 0, tex);
		etest(8);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		etest(9);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisable(GL_TEXTURE_2D);
		etest(10);
/*
		GLenum err = glGetError();
		if (err)
			printf("GL error: %d\n", err);
*/
#endif
	}

}

unsigned char *Image::GetRGBPixels()
{

	if ( pixels ) {

		if ( rgb_pixels == NULL ) {
			rgb_pixels = new unsigned char [ width * height * 3 ];

			for ( int x = 0; x < width; ++x ) 
				for ( int y = 0; y < height; ++y ) 
					for ( int i = 0; i < 3; ++i)
						rgb_pixels[3 * (y * width + x) + i] = pixels[4 * (y * width + x) + i];
		}

	}

	return rgb_pixels;

}

void Image::DrawBlend ( int x, int y )
{
	if ( pixels ) {
#ifndef HAVE_GLES
		glPushAttrib ( GL_ALL_ATTRIB_BITS );

		glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

		glRasterPos2i ( x, y + height );
		glDrawPixels ( width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels );

		glPopAttrib ();
#else
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Draw(x, y);
		glDisable(GL_BLEND);
		return;
		GLfloat verts[] = {
			x, y,
			x + width, y,
			x + width, y + height,
			x, y + height
		};

		GLfloat tex[] = {
			0, 0,
			width, 0,
			width, height,
			0, height
		};

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(2, GL_FLOAT, 0, verts);
		glTexCoordPointer(2, GL_FLOAT, 0, tex);

		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		GLenum err = glGetError();
		if (err)
			printf("GL error: %d\n", err);
#endif

	}

}

void Image::CreateErrorBitmap ()
{

	width = 32;
	height = 32;
	uint32 *newimage = new uint32 [ width * height * sizeof(uint32) ];

	uint32 WHITE = 0xFFFFFFFF;
	uint32 BLACK = 0xFF000000;

	for ( int x = 0; x < width; ++x ) {
		for ( int y = 0; y < height; ++y ) {

			if ( x == 0 || y == 0 || x == width - 1 || y == height - 1 || x == y || x + y == width )
				newimage [y * width + x] = WHITE;

			else
				newimage [y * width + x] = BLACK;

		}
	}
	
	if ( rgb_pixels ) {
		delete [] rgb_pixels;
		rgb_pixels = NULL;
	}

	if ( pixels )
		delete [] pixels;
	pixels = (unsigned char *) newimage;

}

char Image::GetPixelR ( int x, int y )
{

	if ( pixels ) {

		if ( x < 0 || x >= width ||
	  		 y < 0 || y >= height )

			 return -1;

		else

			return ( pixels [(y * width + x) * 4] );

	}

	 return -1;

}

char Image::GetPixelG ( int x, int y )
{

	if ( pixels ) {

		if ( x < 0 || x >= width ||
	  		 y < 0 || y >= height )

			 return -1;

		else

			return ( pixels [(y * width + x) * 4 + 1] );

	}

	 return -1;

}

char Image::GetPixelB ( int x, int y )
{

	if ( pixels ) {

		if ( x < 0 || x >= width ||
	  		 y < 0 || y >= height )

			return -1;

		else

			return ( pixels [(y * width + x) * 4 + 2] );

	}

	 return -1;

}
