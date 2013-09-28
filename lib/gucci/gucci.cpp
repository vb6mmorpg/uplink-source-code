#include <map>
#include <stdexcept>
#include <vector>

#include "SDL_image.h"
#include "SDL_ttf.h"

#include "gucci.hpp"

namespace Gucci {
    static SDL_Window   *window;
    static SDL_Renderer *renderer;
    
    static std::vector<DisplayCallback>     disp_callbacks;
    static std::vector<KeyboardCallback>    kb_callbacks;
    static std::vector<TextInputCallback>   ti_callbacks;
    static std::vector<TextEditingCallback> te_callbacks;
    static std::vector<MouseMotionCallback> mm_callbacks;
    static std::vector<MouseWheelCallback>  mw_callbacks;
    static std::vector<MouseButtonCallback> mb_callbacks;
    
    static std::map<std::string, TTF_Font *> fonts;
    static std::map<std::string, TTF_Font *>::iterator default_font = fonts.end();
    
    void Init(const std::string& window_title, int width, int height, bool fullscreen, bool debug) {
        if (IMG_Init(IMG_INIT_TIF) != 0)
            throw std::runtime_error(std::string("SDL2_image initialisation failed: ") + std::string(SDL_GetError()));
        
        if (TTF_Init() != 0)
            throw std::runtime_error(std::string("SDL2_ttf initialisation failed: ") + std::string(SDL_GetError()));
        
        Uint32 fs = fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;
        if ((window = SDL_CreateWindow(window_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_RESIZABLE | fs)) == nullptr)
            throw std::runtime_error(std::string("Window creation failed: ") + std::string(SDL_GetError()));
        if ((renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE)) == nullptr)
            throw std::runtime_error(std::string("Renderer creation failed: ") + std::string(SDL_GetError()));
    }
    
    void Quit() {
        for (std::map<std::string, TTF_Font *>::iterator it = fonts.begin(); it != fonts.end(); it++)
            TTF_CloseFont(it->second);
        IMG_Quit();
        TTF_Quit();
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
    }
    
    void MainLoop() {
        while (true) {
            for (std::vector<DisplayCallback>::iterator it = disp_callbacks.begin(); it != disp_callbacks.end(); it++)
                (*it)();
            
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                switch(event.type) {
                    case SDL_QUIT:
                        Quit();
                        return;
                    case SDL_KEYDOWN:
                    case SDL_KEYUP:
                        for (std::vector<KeyboardCallback>::iterator it = kb_callbacks.begin(); it != kb_callbacks.end(); it++)
                            (*it)(event.key);
                        break;
                    case SDL_TEXTINPUT:
                        for (std::vector<TextInputCallback>::iterator it = ti_callbacks.begin(); it != ti_callbacks.end(); it++)
                            (*it)(event.text);
                        break;
                    case SDL_TEXTEDITING:
                        for (std::vector<TextEditingCallback>::iterator it = te_callbacks.begin(); it != te_callbacks.end(); it++)
                            (*it)(event.edit);
                        break;
                    case SDL_MOUSEMOTION:
                        for (std::vector<MouseMotionCallback>::iterator it = mm_callbacks.begin(); it != mm_callbacks.end(); it++)
                            (*it)(event.motion);
                        break;
                    case SDL_MOUSEWHEEL:
                        for (std::vector<MouseWheelCallback>::iterator it = mw_callbacks.begin(); it != mw_callbacks.end(); it++)
                            (*it)(event.wheel);
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                    case SDL_MOUSEBUTTONUP:
                        for (std::vector<MouseButtonCallback>::iterator it = mb_callbacks.begin(); it != mb_callbacks.end(); it++)
                            (*it)(event.button);
                        break;
                }
            }
        }
    }
    
    void RegisterDisplayCallback(const DisplayCallback cb) {
        disp_callbacks.push_back(cb);
    }
    void RegisterKeyboardCallback(const KeyboardCallback cb) {
        kb_callbacks.push_back(cb);
    }
    void RegisterTextInputCallback(const TextInputCallback cb) {
        ti_callbacks.push_back(cb);
    }
    void RegisterTextEditingCallback(const TextEditingCallback cb) {
        te_callbacks.push_back(cb);
    }
    void RegisterMouseMotionCallback(const MouseMotionCallback cb) {
        mm_callbacks.push_back(cb);
    }
    void RegisterMouseWheelCallback(const MouseWheelCallback cb) {
        mw_callbacks.push_back(cb);
    }
    void RegisterMouseButtonCallback(const MouseButtonCallback cb) {
        mb_callbacks.push_back(cb);
    }
    
    void DeregisterDisplayCallback(const DisplayCallback cb) {
        for (std::vector<DisplayCallback>::iterator it = disp_callbacks.begin(); it != disp_callbacks.end(); it++) {
            if (*it == cb) {
                disp_callbacks.erase(it);
                return;
            }
        }
    }
    void DeregisterKeyboardCallback(const KeyboardCallback cb) {
        for (std::vector<KeyboardCallback>::iterator it = kb_callbacks.begin(); it != kb_callbacks.end(); it++) {
            if (*it == cb) {
                kb_callbacks.erase(it);
                return;
            }
        }
    }
    void DeregisterTextInputCallback(const TextInputCallback cb) {
        for (std::vector<TextInputCallback>::iterator it = ti_callbacks.begin(); it != ti_callbacks.end(); it++) {
            if (*it == cb) {
                ti_callbacks.erase(it);
                return;
            }
        }
    }
    void DeregisterTextEditingCallback(const TextEditingCallback cb) {
        for (std::vector<TextEditingCallback>::iterator it = te_callbacks.begin(); it != te_callbacks.end(); it++) {
            if (*it == cb) {
                te_callbacks.erase(it);
                return;
            }
        }
    }
    void DeregisterMouseMotionCallback(const MouseMotionCallback cb) {
        for (std::vector<MouseMotionCallback>::iterator it = mm_callbacks.begin(); it != mm_callbacks.end(); it++) {
            if (*it == cb) {
                mm_callbacks.erase(it);
                return;
            }
        }
    }
    void DeregisterMouseWheelCallback(const MouseWheelCallback cb) {
        for (std::vector<MouseWheelCallback>::iterator it = mw_callbacks.begin(); it != mw_callbacks.end(); it++) {
            if (*it == cb) {
                mw_callbacks.erase(it);
                return;
            }
        }
    }
    void DeregisterMouseButtonCallback(const MouseButtonCallback cb) {
        for (std::vector<MouseButtonCallback>::iterator it = mb_callbacks.begin(); it != mb_callbacks.end(); it++) {
            if (*it == cb) {
                mb_callbacks.erase(it);
                return;
            }
        }
    }
    
    bool LoadTrueTypeFont(const std::string &file, const std::string &font_name, int ptsize, int index) {
        TTF_Font *f = TTF_OpenFontIndex(file.c_str(), ptsize, index);
        if (f == nullptr)
            return false;
        if (fonts.insert(std::pair<std::string, TTF_Font *>(font_name, f)).second == false)
            TTF_CloseFont(f);
        return true;
    }
    
    void UnloadTrueTypeFont(const std::string &font_name) {
        for (std::map<std::string, TTF_Font *>::iterator it = fonts.begin(); it != fonts.end(); it++) {
            if (font_name == it->first) {
                TTF_CloseFont(it->second);
                fonts.erase(it);
                return;
            }
        }
    }
    
    bool FontLoaded(const std::string &font_name) {
        return fonts.find(font_name) != fonts.end();
    }
    
    const std::string &GetDefaultFont() {
        return default_font->first;
    }
    
    bool SetDefaultFont(const std::string &font_name) {
        std::map<std::string, TTF_Font *>::iterator it = fonts.find(font_name);
        if (it == fonts.end())
            return false;
        default_font = it;
        return true;
    }
    
    static TTF_Font *GetFont(const std::string &font_name) {
        std::map<std::string, TTF_Font *>::iterator it = fonts.find(font_name);
        if (it == fonts.end())
            return nullptr;
        return it->second;
    }
    
    SDL_Texture *CreateTextureFromSurface(SDL_Surface *surface) {
        return SDL_CreateTextureFromSurface(renderer, surface);
    }
    
    bool RenderText(Text &t) {
        if (t.text.empty() || (!FontLoaded(t.font_name) && GetDefaultFont().empty()))
            return false;
        
        if (t.base)
            SDL_FreeSurface(t.base);
        if (t.texture)
            SDL_DestroyTexture(t.texture);
        
        if ((t.base = TTF_RenderUTF8_Blended(GetFont(t.font_name), t.text.c_str(), t.color)) == nullptr)
            throw std::runtime_error(std::string("Text rendering failed: ") + std::string(SDL_GetError()));
        
        if ((t.texture = CreateTextureFromSurface(t.base)) == nullptr)
            throw std::runtime_error(std::string("Text texture creation failed: ") + std::string(SDL_GetError()));
        
        return true;
    }
    
    static bool DrawText(int x, int y, const std::string &text, const std::map<std::string, TTF_Font *>::iterator font, const SDL_Color *fg, int style) {
        if (font == fonts.end())
            return false;
        
        TTF_SetFontStyle(font->second, style);
        
        SDL_Color c;
        if (fg == nullptr) {
            c.r = 255;
            c.g = 255;
            c.b = 255;
            c.a = 255;
        } else {
            c.r = fg->r;
            c.g = fg->b;
            c.b = fg->g;
            c.a = fg->a;
        }
        
        SDL_Surface *rendered_text = TTF_RenderUTF8_Blended(font->second, text.c_str(), c);
        
        SDL_Rect dest;
        dest.x = x;
        dest.y = y;
        dest.w = rendered_text->w;
        dest.h = rendered_text->h;
        
        SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, rendered_text);
        
        SDL_RenderCopy(renderer, text_texture, nullptr, &dest);
        
        SDL_DestroyTexture(text_texture);
        SDL_FreeSurface(rendered_text);
        
        return true;
    }
    
    bool DrawText(int x, int y, const std::string &text, const SDL_Color *fg, int style) {
        return DrawText(x, y, text, default_font, fg, style);
    }
    
    bool DrawText(int x, int y, const std::string &text, const std::string &font_name, const SDL_Color *fg, int style) {
        std::map<std::string, TTF_Font *>::iterator it = fonts.find(font_name);
        if (it == fonts.end())
            return false;
        return DrawText(x, y, text, it, fg, style);
    }
    
    bool DrawImage(const Image &img) {
        SDL_RendererFlip flip_flags = SDL_FLIP_NONE;
        double flip_correct = 0.0;
        
        if (img.flip_h && img.flip_v)
            flip_correct = 180.0;
        else if (img.flip_h)
            flip_flags = SDL_FLIP_HORIZONTAL;
        else if (img.flip_v)
                flip_flags = SDL_FLIP_VERTICAL;
        
        return SDL_RenderCopyEx(renderer, img.texture, nullptr, &(img.geometry), img.rotation + flip_correct, &(img.rot_origin), flip_flags) == 0;
    }
}
