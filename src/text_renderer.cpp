#include "text_renderer.hpp"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

TextBlockLayout build_text_block(
    const std::vector<TextLine>& lines,
    float center_x, float center_y,
    float line_spacing, float padding
){
    std::vector<glm::vec2> dims;
    dims.reserve(lines.size());
    float total_height = 0.0f;
    float max_width = 0.0f;
    for (const auto& line : lines){
        glm::vec2 d  = line.renderer->measure_text(line);
        dims.push_back(d);
        total_height += d.y;
        max_width = std::max(max_width, d.x);
    }
    total_height += line_spacing * static_cast<float>(lines.size()-1);
    float cursor_top = center_y + total_height * 0.5f;

    TextBlockLayout layout;
    layout.texts.reserve(lines.size());

    for (size_t i=0; i < lines.size(); i++){
        float baseline_y = cursor_top - dims[i].y;
        float x = center_x - dims[i].x * 0.5f;

        TextLine positioned = lines[i];
        positioned.x = x;
        positioned.y = baseline_y;
        layout.texts.push_back(positioned);

        cursor_top = baseline_y - line_spacing;
    }

    layout.box_x = center_x - (max_width * 0.5f) - padding;
    layout.box_y = center_y - (total_height * 0.5f) - padding;
    layout.box_width = max_width + 2.0f * padding;
    layout.box_height = total_height + 2.0f * padding;

    return layout;
}

TextRenderer::TextRenderer(const std::string& font_path, unsigned int pixel_height,
                            int screen_width, int screen_height)
    : shader_("shaders/text.vert", "shaders/text.frag")
{
    set_screen_size(screen_width, screen_height);

    FT_Library ft;
    FT_Init_FreeType(&ft);
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "FreeType: could not init library\n";
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, font_path.c_str(), 0, &face)) {   // <- this call must exist and use font_path + face
        std::cerr << "FreeType: failed to load font: " << font_path << "\n";
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, pixel_height);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 32; c < 128; ++c) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "FreeType: failed to load glyph '" << c << "'\n";
            continue;
        }

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RED,
            face->glyph->bitmap.width, face->glyph->bitmap.rows,
            0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer
        );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glyphs_[c] = Glyph{
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

TextRenderer::~TextRenderer() {
    glDeleteVertexArrays(1, &vao_);
    glDeleteBuffers(1, &vbo_);
    for (auto& [ch, glyph] : glyphs_) {
        glDeleteTextures(1, &glyph.texture_id);
    }
}

void TextRenderer::set_screen_size(int width, int height) {
    projection_ = glm::ortho(0.0f, static_cast<float>(width),
                              0.0f, static_cast<float>(height));
}

glm::vec2 TextRenderer::measure_text(const std::string& text, float scale) const{
    float width = 0.0f, max_top = 0.0f, max_bottom = 0.0f;
    for (char c: text){
        auto it = glyphs_.find(c);
        if (it == glyphs_.end()) continue;
        const Glyph& g = it->second;
        width += (g.advance >> 6)*scale;
        float top = g.bearing.y*scale;
        float bottom = (g.size.y - g.bearing.y)*scale;
        max_top = std::max(max_top, top);
        max_bottom = std::max(max_bottom, bottom);
    }
    return glm::vec2(width, max_top + max_bottom);
}

glm::vec2 TextRenderer::measure_text(const TextLine& text) const{
    return measure_text(text.text, text.scale);
}

void TextRenderer::render_text(TextLine& text){
    shader_.use();
    shader_.setMat4("projection", projection_);
    shader_.setVec4("textColor", text.color);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vao_);

    for (char c : text.text) {
        auto it = glyphs_.find(c);
        if (it == glyphs_.end()) continue;
        const Glyph& g = it->second;

        float xpos = text.x + g.bearing.x * text.scale;
        float ypos = text.y - (g.size.y - g.bearing.y) * text.scale;
        float w = g.size.x * text.scale;
        float h = g.size.y * text.scale;

        float vertices[6][4] = {
            { xpos     , ypos + h ,   0.0f, 0.0f },
            { xpos     , ypos     ,   0.0f, 1.0f },
            { xpos + w , ypos     ,   1.0f, 1.0f },

            { xpos     , ypos + h ,   0.0f, 0.0f },
            { xpos + w , ypos     ,   1.0f, 1.0f },
            { xpos + w , ypos + h ,   1.0f, 0.0f },
        };

        glBindTexture(GL_TEXTURE_2D, g.texture_id);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        text.x += (g.advance >> 6) * text.scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}