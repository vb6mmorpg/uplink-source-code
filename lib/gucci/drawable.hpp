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
            
            int GetOriginalWidth();
            int GetOriginalHeight();
            const SDL_Rect &GetGeometry();
            double GetRotation();
            const SDL_Point &GetRotationOrigin();
            bool IsFlippedHorizontal();
            bool IsFlippedVertical();
            
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
