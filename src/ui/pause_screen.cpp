#include "ui/pause_screen.hpp"

#include <algorithm>
#include <string>

namespace {
    constexpr float kInitialWindowWidth = 1920.0f;
    constexpr float kInitialWindowHeight = 1080.0f;
}

// Core behavior
bool PauseScreen::update(){
    auto action = ui_ctx_.input_manager.consume_triggered_action();
    if(!action) return false;

    switch (*action){
    case UIAction::GoToMainMenu:
        should_exit_ = true;
        return true;

    default:
        return false;
    }
}

void PauseScreen::draw(){
    Shader& shader = ui_ctx_.shader;
    Renderer& quad_renderer = ui_ctx_.quad_renderer;
    TextRenderer& reg_text_renderer = ui_ctx_.regular_text_renderer;
    TextRenderer& dis_text_renderer = ui_ctx_.display_text_renderer;
    const glm::mat4& projection = ui_ctx_.ui_projection;
    int& window_width = ui_ctx_.window_width;
    int& window_height = ui_ctx_.window_height;

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Color text_color = Colors::White;
    float textbox_padding = 20.0f;
    float lineshift = 30.0f;

    float x_scale = window_width / kInitialWindowWidth;
    float y_scale = window_height / kInitialWindowHeight;
    float ui_scale = std::min(x_scale, y_scale);
    
    TextBlockLayout top_block = build_text_block(
        {
            TextLine("PAUSED", 0.0f, 0.0f, 0.5f, text_color, &dis_text_renderer),
            TextLine("Press ESC to resume", 0.0f, 0.0f, 0.4f, text_color, &reg_text_renderer),
        },
        window_width * 0.5f, window_height * 0.85f,
        lineshift, textbox_padding
    );

    TextBlockLayout bottom_block = build_text_block(
        {
            TextLine("Controls", 0.0f, 0.0f, 0.4f, text_color, &dis_text_renderer),
            TextLine("W/A/S/D: Horizontal Movement", 0.0f, 0.0f, 0.3f, text_color, &reg_text_renderer),
            TextLine("LSHIFT/SPACE: Vertical Movement", 0.0f, 0.0f, 0.3f, text_color, &reg_text_renderer),
            TextLine("MOUSE1 (HOLD): Pan", 0.0f, 0.0f, 0.3f, text_color, &reg_text_renderer)
        },
        window_width * 0.5f, window_height * 0.5f,
        lineshift, textbox_padding
    );

    std::string main_menu_btn_text = "Return to main menu";
    float main_menu_btn_scale = 0.3f * ui_scale;
    auto [width, ascent, descent] = dis_text_renderer.measure_text(
        main_menu_btn_text,
        main_menu_btn_scale
    );
    width += 2.0f * textbox_padding;
    float height = ascent - descent + 2.0f * textbox_padding;
    
    ButtonSpec main_menu_btn{
        .box = UIBox{
            .x = window_width * 0.5f - width * 0.5f,
            .y = window_height * 0.5f - bottom_block.box_height * 0.5f - height - textbox_padding,
            .width = width,
            .height = height
        },
        .scale = main_menu_btn_scale,
        .box_color = Colors::Background,
        .label_color = Colors::White,
        .label_texture = nullptr,
        .label = main_menu_btn_text,
        .action = UIAction::GoToMainMenu,
        .enabled = true        
    };
    
    // Rendering
    shader.setVec4("color", Colors::WhiteHalfOpaque);
    quad_renderer.draw(shader, projection, glm::mat4(1.0f), 
        build_ui_quad_model(0.0f,0.0f,window_width, window_height)
    );
    
    shader.setVec4("color", Colors::Background);
    quad_renderer.draw(shader, projection, glm::mat4(1.0f), 
        build_ui_quad_model(top_block)
    );
    quad_renderer.draw(shader, projection, glm::mat4(1.0f), 
        build_ui_quad_model(bottom_block)
    );

    for (auto& text : top_block.texts){
        text.renderer->render_text(text);
    }
    for (auto& text : bottom_block.texts){
        text.renderer->render_text(text);
    }

    draw_button(ui_ctx_, UIPage::PauseMenu, main_menu_btn);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

bool PauseScreen::should_exit(){
    if (should_exit_){
        should_exit_ = false;
        return true;
    }
    return false;
}