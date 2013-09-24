#include <stdexcept>
#include <vector>

#include "SDL_image.h"
#include "SDL_ttf.h"

#include "gucci.hpp"

static SDL_Window   *window;
static SDL_Renderer *renderer;

static std::vector<GciKeyboardCallback>    kb_callbacks;
static std::vector<GciTextInputCallback>   ti_callbacks;
static std::vector<GciTextEditingCallback> te_callbacks;
static std::vector<GciMouseMotionCallback> mm_callbacks;
static std::vector<GciMouseWheelCallback>  mw_callbacks;
static std::vector<GciMouseButtonCallback> mb_callbacks;

static std::vector<TTF_Font *> fonts;

void GciInit(const std::string& window_title, int width, int height, bool fullscreen, bool debug) {
    if (IMG_Init(IMG_INIT_TIF) != 0)
        throw std::runtime_error(std::string("SDL2_image initialisation failed: ") + std::string(SDL_GetError()));
    
    if (TTF_Init() != 0)
        throw std::runtime_error(std::string("SDL2_ttf initialisation failed: ") + std::string(SDL_GetError()));
    
    Uint32 fs = fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;
    if ((window = SDL_CreateWindow(window_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, fs)) == nullptr)
        throw std::runtime_error(std::string("Window creation failed: ") + std::string(SDL_GetError()));
    if ((renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE)) == nullptr)
        throw std::runtime_error(std::string("Renderer creation failed: ") + std::string(SDL_GetError()));
}

void GciQuit() {
    for (std::vector<TTF_Font *>::iterator it = fonts.begin(); it != fonts.end(); it++)
        TTF_CloseFont(*it);
    IMG_Quit();
    if (TTF_WasInit())
        TTF_Quit();
    if (window)
        SDL_DestroyWindow(window);
    if (renderer)
        SDL_DestroyRenderer(renderer);
}

void GciMainLoop() {
    while (true) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    GciQuit();
                    return;
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                    for (std::vector<GciKeyboardCallback>::iterator it = kb_callbacks.begin(); it != kb_callbacks.end(); it++)
                        (*it)(&(event.key));
                    break;
                case SDL_TEXTINPUT:
                    for (std::vector<GciTextInputCallback>::iterator it = ti_callbacks.begin(); it != ti_callbacks.end(); it++)
                        (*it)(&(event.text));
                    break;
                case SDL_TEXTEDITING:
                    for (std::vector<GciTextEditingCallback>::iterator it = te_callbacks.begin(); it != te_callbacks.end(); it++)
                        (*it)(&(event.edit));
                    break;
                case SDL_MOUSEMOTION:
                    for (std::vector<GciMouseMotionCallback>::iterator it = mm_callbacks.begin(); it != mm_callbacks.end(); it++)
                        (*it)(&(event.motion));
                    break;
                case SDL_MOUSEWHEEL:
                    for (std::vector<GciMouseWheelCallback>::iterator it = mw_callbacks.begin(); it != mw_callbacks.end(); it++)
                        (*it)(&(event.wheel));
                    break;
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                    for (std::vector<GciMouseButtonCallback>::iterator it = mb_callbacks.begin(); it != mb_callbacks.end(); it++)
                        (*it)(&(event.button));
                    break;
            }
        }
    }
}

void GciRegisterKeyboardCallback(const GciKeyboardCallback cb) {
    kb_callbacks.push_back(cb);
}
void GciRegisterTextInputCallback(const GciTextInputCallback cb) {
    ti_callbacks.push_back(cb);
}
void GciRegisterTextEditingCallback(const GciTextEditingCallback cb) {
    te_callbacks.push_back(cb);
}
void GciRegisterMouseMotionCallback(const GciMouseMotionCallback cb) {
    mm_callbacks.push_back(cb);
}
void GciRegisterMouseWheelCallback(const GciMouseWheelCallback cb) {
    mw_callbacks.push_back(cb);
}
void GciRegisterMouseButtonCallback(const GciMouseButtonCallback cb) {
    mb_callbacks.push_back(cb);
}

void GciDeregisterKeyboardCallback(const GciKeyboardCallback cb) {
    for (std::vector<GciKeyboardCallback>::iterator it = kb_callbacks.begin(); it != kb_callbacks.end(); it++) {
        if (*it == cb) {
            kb_callbacks.erase(it);
            return;
        }
    }
}
void GciDeregisterTextInputCallback(const GciTextInputCallback cb) {
    for (std::vector<GciTextInputCallback>::iterator it = ti_callbacks.begin(); it != ti_callbacks.end(); it++) {
        if (*it == cb) {
            ti_callbacks.erase(it);
            return;
        }
    }
}
void GciDeregisterTextEditingCallback(const GciTextEditingCallback cb) {
    for (std::vector<GciTextEditingCallback>::iterator it = te_callbacks.begin(); it != te_callbacks.end(); it++) {
        if (*it == cb) {
            te_callbacks.erase(it);
            return;
        }
    }
}
void GciDeregisterMouseMotionCallback(const GciMouseMotionCallback cb) {
    for (std::vector<GciMouseMotionCallback>::iterator it = mm_callbacks.begin(); it != mm_callbacks.end(); it++) {
        if (*it == cb) {
            mm_callbacks.erase(it);
            return;
        }
    }
}
void GciDeregisterMouseWheelCallback(const GciMouseWheelCallback cb) {
    for (std::vector<GciMouseWheelCallback>::iterator it = mw_callbacks.begin(); it != mw_callbacks.end(); it++) {
        if (*it == cb) {
            mw_callbacks.erase(it);
            return;
        }
    }
}
void GciDeregisterMouseButtonCallback(const GciMouseButtonCallback cb) {
    for (std::vector<GciMouseButtonCallback>::iterator it = mb_callbacks.begin(); it != mb_callbacks.end(); it++) {
        if (*it == cb) {
            mb_callbacks.erase(it);
            return;
        }
    }
}

bool GciLoadTrueTypeFont(const std::string &file, int ptsize, int index) {
    TTF_Font *f = TTF_OpenFontIndex(file.c_str(), ptsize, index);
    if (f == nullptr)
        return false;
    fonts.push_back(f);
    return true;
}

void GciUnloadTrueTypeFont(const std::string &family_name) {
    for (std::vector<TTF_Font *>::iterator it = fonts.begin(); it != fonts.end(); it++) {
        if (family_name == TTF_FontFaceFamilyName(*it)) {
            TTF_CloseFont(*it);
            fonts.erase(it);
            return;
        }
    }
}
