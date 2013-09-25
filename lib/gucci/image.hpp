// Image.h: interface for the Image class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _INCLUDED_IMAGE_H
#define _INCLUDED_IMAGE_H

#include <string>

#include "drawable.hpp"

namespace Gucci {
    class Image : public Drawable {
        protected:
            void SetAlphaBorderRec(int x, int y, Uint8 a, Uint8 r, Uint8 g, Uint8 b);
        
        public:
            Image();
            Image(const std::string &filename);
            
            ~Image();
            
            void Load(const std::string &filename);
            
            Uint8 GetAlphaMod();
            void SetAlphaMod(Uint8 alpha);
            void SetAlphaBorder(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
            
            void CreateErrorBitmap(); // Makes the image into an error cross
            
            friend bool DrawImage(const Image &img);
    };
}

#endif
