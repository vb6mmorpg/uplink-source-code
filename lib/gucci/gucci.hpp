#ifndef GUCCI_HPP
#define GUCCI_HPP

#include <string>

#include "SDL_ttf.h"

#include "image.hpp"

namespace Gucci {
    typedef void (*DisplayCallback)();
    typedef void (*KeyboardCallback)(const SDL_KeyboardEvent &event_data);
    typedef void (*TextInputCallback)(const SDL_TextInputEvent &event_data);
    typedef void (*TextEditingCallback)(const SDL_TextEditingEvent &event_data);
    typedef void (*MouseMotionCallback)(const SDL_MouseMotionEvent &event_data);
    typedef void (*MouseWheelCallback)(const SDL_MouseWheelEvent &event_data);
    typedef void (*MouseButtonCallback)(const SDL_MouseButtonEvent &event_data);

    void Init(const std::string& window_title, int width, int height, bool fullscreen = true, bool debug = false);
    void Quit();
    void MainLoop();

    void RegisterDisplayCallback(const DisplayCallback cb);
    void RegisterKeyboardCallback(const KeyboardCallback cb);
    void RegisterTextInputCallback(const TextInputCallback cb);
    void RegisterTextEditingCallback(const TextEditingCallback cb);
    void RegisterMouseMotionCallback(const MouseMotionCallback cb);
    void RegisterMouseWheelCallback(const MouseWheelCallback cb);
    void RegisterMouseButtonCallback(const MouseButtonCallback cb);

    void DeregisterDisplayCallback(const DisplayCallback cb);
    void DeregisterKeyboardCallback(const KeyboardCallback cb);
    void DeregisterTextInputCallback(const TextInputCallback cb);
    void DeregisterTextEditingCallback(const TextEditingCallback cb);
    void DeregisterMouseMotionCallback(const MouseMotionCallback cb);
    void DeregisterMouseWheelCallback(const MouseWheelCallback cb);
    void DeregisterMouseButtonCallback(const MouseButtonCallback cb);

    bool LoadTrueTypeFont(const std::string &file, const std::string &font_name, int ptsize, int index = 0);
    void UnloadTrueTypeFont(const std::string &font_name);
    bool SetDefaultFont(const std::string &font_name);
    
    SDL_Texture *CreateTextureFromSurface(const SDL_Surface *surface);

    bool DrawText(int x, int y, const std::string &text, const SDL_Color *fg = nullptr, int style = TTF_STYLE_NORMAL);
    bool DrawText(int x, int y, const std::string &text, const std::string &font_name, const SDL_Color *fg = nullptr, int style = TTF_STYLE_NORMAL);
    bool DrawImage(const Image &img);
}

#endif
