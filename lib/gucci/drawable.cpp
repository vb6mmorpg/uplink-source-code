#include "drawable.hpp"

namespace Gucci {
    Drawable::Drawable() {
        this->pixels = nullptr;
        this->base = nullptr;
        this->texture = nullptr;
        this->geometry.x = 0;
        this->geometry.y = 0;
        this->geometry.w = 0;
        this->geometry.h = 0;
        this->rotation = 0.0;
        this->rot_origin.x = 0;
        this->rot_origin.y = 0;
        this->flip_h = false;
        this->flip_v = false;
    }
    
    Drawable::~Drawable() {
        if (this->base)
            SDL_FreeSurface(this->base);
        if (this->texture)
            SDL_DestroyTexture(this->texture);
    }
    
    int Drawable::GetOriginalWidth() {
        return this->base->w;
    }
    
    int Drawable::GetOriginalHeight() {
        return this->base->h;
    }
    
    const SDL_Rect &Drawable::GetGeometry() {
        return this->geometry;
    }
    
    double Drawable::GetRotation() {
        return this->rotation;
    }
    
    const SDL_Point &Drawable::GetRotationOrigin() {
        return this->rot_origin;
    }
    
    bool Drawable::IsFlippedHorizontal() {
        return this->flip_h;
    }
    
    bool Drawable::IsFlippedVertical() {
        return this->flip_v;
    }
    
    void Drawable::SetGeometry(int x, int y, int w, int h) {
        this->Move(x, y);
        this->Resize(w, h);
    }

    void Drawable::Move(int x, int y) {
        this->geometry.x = x;
        this->geometry.y = y;
    }
    
    void Drawable::Resize(int w, int h) {
        this->geometry.w = w;
        this->geometry.h = h;
    }
    
    void Drawable::SetRotation(double rot) {
        this->rotation = rot;
    }
    
    void Drawable::SetRotationOrigin(int x, int y) {
        this->rot_origin.x = x;
        this->rot_origin.y = y;
    }
    
    void Drawable::FlipHorizontal() {
        this->flip_h = !(this->flip_h);
    }
    
    void Drawable::FlipVertical() {
        this->flip_v = !(this->flip_v);
    }
    
    void Drawable::FlipDiagonal() {
        this->FlipHorizontal();
        this->FlipVertical();
    }
}
