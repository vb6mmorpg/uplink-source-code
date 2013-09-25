#include <stdexcept>

#include "text.hpp"

namespace Gucci {
    Text::Text(int style) {
        this->font_name = std::string(GetDefaultFont());
        
        this->color.r = 255;
        this->color.g = 255;
        this->color.b = 255;
        this->color.a = 255;
        
        this->style = style;
    }
    
    Text::Text(const std::string &text, int style) {
        this->text = std::string(text);
        
        this->font_name = std::string(GetDefaultFont()->first);
        
        this->color.r = 255;
        this->color.g = 255;
        this->color.b = 255;
        this->color.a = 255;
        
        this->style = style;
    }
    
    Text::Text(const std::string &text, const std::string &font_name, int style) {
        this->text = std::string(text);
        
        this->font_name = std::string(font_name);
        
        this->color.r = 255;
        this->color.g = 255;
        this->color.b = 255;
        this->color.a = 255;
        
        this->style = style;
    }
    
    Text::Text(const std::string &text, Uint8 r, Uint8 g, Uint8 b, Uint8 a, int style) {
        this->text = std::string(text);
        
        this->font_name = std::string(GetDefaultFont());
        
        this->color.r = r;
        this->color.g = g;
        this->color.b = b;
        this->color.a = a;
        
        this->style = style;
    }
    
    Text::Text(const std::string &text, const std::string &font_name, Uint8 r, Uint8 g, Uint8 b, Uint8 a, int style) {
        this->text = std::string(text);
        
        this->font_name = std::string(font_name);
        
        this->color.r = r;
        this->color.g = g;
        this->color.b = b;
        this->color.a = a;
        
        this->style = style;
    }
    
    void Text::SetText(const std::string &text) {
        this->text = std::string(text);
    }
    
    bool Text::SetFont(const std::string &font_name) {
        if (GetFont(this->font_name) == nullptr && GetDefaultFont() == nullptr)
            return false;
        this->font_name = std::string(font_name);
    }
    
    void Text::SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
        this->color.r = r;
        this->color.g = g;
        this->color.b = b;
        this->color.a = a;
    }
    
    void SetStyle(int style) {
        this->style = style;
    }
}
