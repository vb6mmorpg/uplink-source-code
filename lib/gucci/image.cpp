// Image.cpp: implementation of the Image class.
//
//////////////////////////////////////////////////////////////////////

#include <stdexcept>

#include "SDL_image.h"

#include "image.hpp"

#include "mmgr.h"

namespace Gucci {
    extern SDL_Texture *CreateTextureFromSurface(const SDL_Surface *surface);
    
    void Image::SetAlphaBorderRec(int x, int y, Uint8 a, Uint8 r, Uint8 g, Uint8 b) {
        if (x >= 0 && x < this->base->w && y >= 0 && y < this->base->h) {
            Uint32 *pixel = reinterpret_cast<Uint32 *>(this->base->pixels) + (y * this->base->w + x);
            Uint8 pix_r, pix_g, pix_b, pix_a;
            
            SDL_GetRGBA(*pixel, this->base->format, &pix_r, &pix_g, &pix_b, &pix_a);
            
            if (pix_a != a && pix_r == r && pix_g == g && pix_b == b)
                *pixel = SDL_MapRGBA(this->base->format, r, g, b, a);
        }
    }

    Image::Image(const std::string &filename) {
        this->geometry.x = 0;
        this->geometry.y = 0;
        this->rotation = 0.0;
        this->flip_h = false;
        this->flip_v = false;
        this->Load(filename);
    }

    void Image::Load(const std::string &filename) {
        // You shouldn't be re-using the same object over and over. It's *SLOW*.
        // Just load each image separately and apply transformations accordingly.
        if (!(this->base || this->texture)) {
            // Forcing a conversion to a 32-bit ARGB format simplifies SetAlphaBorderRec(),
            // because now we don't have to worry about handling esoteric formats.
            if ((this->base = SDL_ConvertSurfaceFormat(IMG_Load(filename.c_str()), SDL_PIXELFORMAT_ARGB8888, 0)) == nullptr) {
                this->CreateErrorBitmap();
                return;
            }
            this->geometry.w = this->base->w;
            this->geometry.h = this->base->h;
            this->rot_origin.x = (int)(this->geometry.w / 2);
            this->rot_origin.y = (int)(this->geometry.h / 2);
            if ((this->texture = CreateTextureFromSurface(this->base)) == nullptr)
                this->CreateErrorBitmap();
        }
    }

    Uint8 Image::GetAlphaMod() const {
        Uint8 alpha;
        SDL_GetTextureAlphaMod(this->texture, &alpha);
        return alpha;
    }

    void Image::SetAlphaMod(Uint8 alpha) {
        SDL_SetTextureAlphaMod(this->texture, alpha);
    }

    void Image::SetAlphaBorder(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
        for (int x = 0; x < this->base->w; ++x) {
            this->SetAlphaBorderRec(x, 0, a, r, g, b);
            this->SetAlphaBorderRec(x, this->base->h - 1, a, r, g, b);
        }
        for (int y = 0; y < this->base->h; ++y) {
            this->SetAlphaBorderRec(0, y, a, r, g, b );
            this->SetAlphaBorderRec(this->base->w - 1, y, a, r, g, b);
        }
        SDL_DestroyTexture(texture);
        if ((this->texture = CreateTextureFromSurface(this->base)) == nullptr)
            this->CreateErrorBitmap();
    }

    void Image::CreateErrorBitmap() {
        if (this->base)
            SDL_FreeSurface(this->base);
        if (this->texture)
            SDL_DestroyTexture(this->texture);
        
        int bpp;
        Uint32 Rmask, Gmask, Bmask, Amask;
        SDL_PixelFormatEnumToMasks(SDL_PIXELFORMAT_ARGB8888, &bpp, &Rmask, &Gmask, &Bmask, &Amask);
        if ((this->base = SDL_CreateRGBSurface(0, 32, 32, bpp, Rmask, Gmask, Bmask, Amask)) == nullptr)
            throw std::runtime_error(std::string("\"Error\" bitmap generation failed: ") + std::string(SDL_GetError()));

        for (int x = 0; x < this->base->w; ++x) {
            for (int y = 0; y < this->base->h; ++y) {
                if (x == 0 || y == 0 || x == this->base->w - 1 || y == this->base->h - 1 || x == y || x + y == this->base->w)
                    reinterpret_cast<Uint32 *>(this->base->pixels)[y * this->base->w + x] = SDL_MapRGBA(this->base->format, 255, 255, 255, 255);
                else
                    reinterpret_cast<Uint32 *>(this->base->pixels)[y * this->base->w + x] = SDL_MapRGBA(this->base->format, 0, 0, 0, 255);
            }
        }
        
        if ((this->texture = CreateTextureFromSurface(this->base)) == nullptr)
            throw std::runtime_error(std::string("\"Error\" bitmap generation failed: ") + std::string(SDL_GetError()));
    }
}
