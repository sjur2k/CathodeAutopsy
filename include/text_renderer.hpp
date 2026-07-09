#pragma once
#include <map>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.hpp"
#include "color.hpp"


struct Text{
    std::string text;
    float x,y;
    float scale;
    Color color;

    Text(std::string t, glm::vec2 xy, float s, Color c):
        text(t), x(xy.x), y(xy.y), scale(s), color(c){}
    Text(std::string t, float x, float y, float s, Color c):
        text(t), x(x), y(y), scale(s), color(c){}
};

class TextRenderer{
    public:
        TextRenderer(
            const std::string& font_path, 
            unsigned int pixel_height, 
            int screen_width, 
            int screen_height
        );
        ~TextRenderer();

        void render_text(Text& text);
        void set_screen_size(int width, int height);
        glm::vec2 measure_text(const std::string& text, float scale) const;
        glm::vec2 measure_text(const Text& text) const;
    private:
        struct Glyph{
            unsigned int texture_id;
            glm::ivec2 size;
            glm::ivec2 bearing;
            unsigned int advance;
        };
        
        std::map<char, Glyph> glyphs_;
        unsigned int vao_ = 0, vbo_ = 0;
        Shader shader_;
        glm::mat4 projection_;
};