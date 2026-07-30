#pragma once

#include "color.hpp"
#include "platform/input_manager.hpp"
#include "rendering/renderer.hpp"
#include "rendering/shader.hpp"
#include "rendering/text_renderer.hpp"
#include "rendering/texture.hpp"

#include <glm/glm.hpp>

#include <optional>
#include <string>

struct ButtonSpec {
    UIBox box;
    float scale;
    std::optional<Color> box_color;
    std::optional<Color> label_color;
    Texture* label_texture = nullptr;
    std::string label;
    UIAction action;
    bool enabled = true;
};

struct UIContext {
    Shader& shader;
    Shader& textured_shader;
    Renderer& quad_renderer;
    Renderer& textured_quad_renderer;
    TextRenderer& regular_text_renderer;
    TextRenderer& display_text_renderer;
    InputManager& input_manager;
    const glm::mat4& ui_projection;
    int& window_width;
    int& window_height;
};

glm::mat4 build_ui_quad_model(float x, float y, float width, float height);
glm::mat4 build_ui_quad_model(const TextBlockLayout& block);
glm::mat4 build_ui_quad_model(const UIBox& box);

void draw_button(UIContext& ctx, UIPage page, const ButtonSpec& spec);