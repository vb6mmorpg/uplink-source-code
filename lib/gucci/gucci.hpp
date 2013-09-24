#ifndef GUCCI_HPP
#define GUCCI_HPP

#include <string>

#include "SDL.h"

typedef void (*GciKeyboardCallback)(const SDL_KeyboardEvent *event_data);
typedef void (*GciTextInputCallback)(const SDL_TextInputEvent *event_data);
typedef void (*GciTextEditingCallback)(const SDL_TextEditingEvent *event_data);
typedef void (*GciMouseMotionCallback)(const SDL_MouseMotionEvent *event_data);
typedef void (*GciMouseWheelCallback)(const SDL_MouseWheelEvent *event_data);
typedef void (*GciMouseButtonCallback)(const SDL_MouseButtonEvent *event_data);

void GciInit(const std::string& window_title, int width, int height, bool fullscreen = true, bool debug = false);
void GciQuit();
void GciMainLoop();

void GciRegisterKeyboardCallback(const GciKeyboardCallback cb);
void GciRegisterTextInputCallback(const GciTextInputCallback cb);
void GciRegisterTextEditingCallback(const GciTextEditingCallback cb);
void GciRegisterMouseMotionCallback(const GciMouseMotionCallback cb);
void GciRegisterMouseWheelCallback(const GciMouseWheelCallback cb);
void GciRegisterMouseButtonCallback(const GciMouseButtonCallback cb);

void GciDeregisterKeyboardCallback(const GciKeyboardCallback cb);
void GciDeregisterTextInputCallback(const GciTextInputCallback cb);
void GciDeregisterTextEditingCallback(const GciTextEditingCallback cb);
void GciDeregisterMouseMotionCallback(const GciMouseMotionCallback cb);
void GciDeregisterMouseWheelCallback(const GciMouseWheelCallback cb);
void GciDeregisterMouseButtonCallback(const GciMouseButtonCallback cb);

bool GciLoadTrueTypeFont(const std::string &file, int ptsize, int index = 0);
void GciUnloadTrueTypeFont(const std::string &family_name);

#endif
