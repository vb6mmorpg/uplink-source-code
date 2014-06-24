// Image.h: interface for the Image class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_IMAGE_H
#define _INCLUDED_IMAGE_H

/*
#ifdef _DEBUG
#include "slasher.h"
#endif
*/


class Image  
{

protected:

	int width, height;
	char alpha;
        unsigned char *rgb_pixels;

	void SetAlphaBorderRec ( int x, int y, unsigned char a, unsigned char r, unsigned char g, unsigned char b );

public:
	
	unsigned char *pixels;	

	Image();
	Image( const Image& img );

	virtual ~Image();

	void LoadRAW ( char *filename, int sizex, int sizey );
	void LoadTIF ( char *filename );
	
        unsigned char *GetRGBPixels();
	
	void  SetAlpha ( float newalpha );
	void  SetAlphaBorder ( float newalpha, float r, float g, float b );
	float GetAlpha ();

	int Width ();
	int Height ();

	void FlipAroundH ();
	void Scale ( int newwidth, int newheight );
	void ScaleToOpenGL ();							// Resizes to valid OGL sizes (powers of 2)

	void Draw ( int x, int y );
	void DrawBlend ( int x, int y );

	void CreateErrorBitmap ();						// Makes the image into an error cross

	char GetPixelR ( int x, int y );
	char GetPixelG ( int x, int y );
	char GetPixelB ( int x, int y );

};

#endif
