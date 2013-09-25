#ifndef GUCCI_TEXT_HPP
#define GUCCI_TEXT_HPP

#include <string>

#include "SDL_ttf.h"

#include "drawable.hpp"

namespace Gucci {
    class Text : public Drawable {
        protected:
            std::string text;
            std::string font_name;
            SDL_Color color;
            int style;
        
        public:
            Text(int style = TTF_STYLE_NORMAL);
            Text(const std::string &text, int style = TTF_STYLE_NORMAL);
            Text(const std::string &text, std::string font_name, int style = TTF_STYLE_NORMAL);
            Text(const std::string &text, Uint8 r, Uint8 g, Uint8 b, Uint8 a, int style = TTF_STYLE_NORMAL);
            Text(const std::string &text, std::string font_name, Uint8 r, Uint8 g, Uint8 b, Uint8 a, int style = TTF_STYLE_NORMAL);
            
            ~Text();
            
            void SetText(const std::string &text);
            bool SetFont(const std::string &font_name);
            void SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
            void SetStyle(int style);
            
            friend bool RenderText(Text &text);
            friend bool DrawText(const Text &text);
    };
}

#endif
