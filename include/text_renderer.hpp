#pragma once
#include <map>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.hpp"
#include "color.hpp"

class TextRenderer;

struct TextLine{
    std::string text;
    float x,y;
    float scale;
    Color color;
    TextRenderer* renderer = nullptr;

    TextLine(std::string t, glm::vec2 xy, float s, Color c, TextRenderer* r = nullptr):
        text(t), x(xy.x), y(xy.y), scale(s), color(c), renderer(r){}
    TextLine(std::string t, float x, float y, float s, Color c, TextRenderer* r = nullptr):
        text(t), x(x), y(y), scale(s), color(c), renderer(r){}
};

struct TextBlockLayout {
    std::vector<TextLine> texts;
    float box_x, box_y;
    float box_width, box_height;
};

TextBlockLayout build_text_block(
    const std::vector<TextLine>& lines,
    float center_x, float center_y,
    float line_spacing, float padding
);

class TextRenderer{
    public:
        TextRenderer(
            const std::string& font_path, 
            unsigned int pixel_height, 
            int screen_width, 
            int screen_height
        );
        ~TextRenderer();

        void render_text(TextLine& text);
        void set_screen_size(int width, int height);
        glm::vec2 measure_text(const std::string& text, float scale) const;
        glm::vec2 measure_text(const TextLine& text) const;
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