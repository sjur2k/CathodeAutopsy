#include "ui/running_screen.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <string>

namespace {
    // Assumed constants (SI-units)
    constexpr float kCellWidth = 10.0f;
    constexpr float kCellLength = 4.0f;
    constexpr float kResolution = 0.05f;

    // Constants for rendering
    constexpr float kGridWidth = kCellWidth / kResolution;
    constexpr float kGridLength = kCellLength / kResolution;
    constexpr float kPadding = 10.0f;

    glm::mat4 build_floor_model(){
        glm::mat4 model = glm::translate(
            glm::mat4(1.0f),
            glm::vec3(kGridLength/2.0f, 0.0f, kGridWidth/2.0f)
        );
        model = glm::rotate(
            model,
            glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f)
        );
        model = glm::scale(
            model,
            glm::vec3(kGridLength + 2*kPadding, kGridWidth + 2*kPadding, 1.0f)
        );
        return model;
    }
}

RunningScreen::RunningScreen(
    UIContext ui_ctx,
    Camera& camera,
    Renderer& point_cloud_renderer
) : ui_ctx_(ui_ctx),
    camera_(camera),
    point_cloud_renderer_(point_cloud_renderer)
{}

// Core behavior
void RunningScreen::update(float delta_time){
    ui_ctx_.input_manager.process_input(delta_time);
}

void RunningScreen::draw(){
    Shader& shader = ui_ctx_.shader;
    Renderer& quad_renderer = ui_ctx_.quad_renderer;
    TextRenderer& text_renderer = ui_ctx_.regular_text_renderer;
    const glm::mat4& ui_proj = ui_ctx_.ui_projection;
    int& window_height = ui_ctx_.window_height;
    
    // Scene
    glm::mat4 proj = camera_.get_projection_matrix();
    glm::mat4 view = camera_.get_view_matrix();
    
    shader.setVec4("color", Colors::Cyan);
    point_cloud_renderer_.draw(shader, proj, view, glm::mat4(1.0f));

    shader.setVec4("color", Colors::Floor);
    quad_renderer.draw(shader, proj, view, build_floor_model());

    // HUD
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Color text_color = Colors::Background;
    float textbox_padding = 10.0f;

    std::string msg = "For information, press ESC";
    float text_scale = 0.2f;
    auto [width, ascent, descent] = text_renderer.measure_text(msg, text_scale);
    float height = ascent - descent;
    glm::vec2 text_coords(textbox_padding, window_height - height - textbox_padding);
    TextLine text(msg, text_coords, text_scale, text_color);

    glm::mat4 textbox_model = build_ui_quad_model(
        0.0f,
        window_height - height - 2.0f * textbox_padding,
        width + 2 * textbox_padding,
        height + 2 * textbox_padding
    );

    shader.setVec4("color", Colors::Background);
    quad_renderer.draw(shader, ui_proj, glm::mat4(1.0f), textbox_model);
    shader.setVec4("color", Colors::WhiteHalfOpaque);
    quad_renderer.draw(shader, ui_proj, glm::mat4(1.0f), textbox_model);
    text_renderer.render_text(text);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}