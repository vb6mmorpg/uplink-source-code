#ifndef GUCCI_DRAWABLE_HPP
#define GUCCI_DRAWABLE_HPP

#include "SDL.h"

namespace Gucci {
    class Drawable {
        protected:
            Uint8 *pixels;
            SDL_Surface *base;
            SDL_Texture *texture;
            SDL_Rect geometry;
            double rotation;
            SDL_Point rot_origin;
            bool flip_h, flip_v;
            
        public:
            Drawable();
            virtual ~Drawable() = 0;
            
            int GetOriginalWidth() const;
            int GetOriginalHeight() const;
            const SDL_Rect &GetGeometry() const;
            double GetRotation() const;
            const SDL_Point &GetRotationOrigin() const;
            bool IsFlippedHorizontal() const;
            bool IsFlippedVertical() const;
            
            void SetGeometry(int x, int y, int w, int h);
            void Move(int x, int y);
            void Resize(int w, int h);
            void SetRotation(double rot);
            void SetRotationOrigin(int x, int y);
            void FlipHorizontal();
            void FlipVertical();
            void FlipDiagonal();
    };
}

#endif
