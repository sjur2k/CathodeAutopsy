#include "startup_screen.hpp"
#include "paths.hpp"

#include "tinyfiledialogs/tinyfiledialogs.h"

#include <algorithm>
#include <string>

namespace {
    constexpr float kInitialWindowWidth = 1920.0f;
    constexpr float kInitialWindowHeight = 1080.0f;
    constexpr float kLogoNativeWidth = 842.0f;
    constexpr float kLogoNativeHeight = 596.0f;
    constexpr float kLogoAspect = kLogoNativeWidth / kLogoNativeHeight;
}

StartupScreen::StartupScreen(
    UIContext ui_ctx,
    TextRenderer& display_text_renderer,
    Texture& logo_texture,
    Texture& close_btn_texture,
    int& window_width,
    int& window_height
) : ui_ctx_(ui_ctx),
    display_text_renderer_(display_text_renderer),
    logo_texture_(logo_texture),
    close_btn_texture_(close_btn_texture),
    window_width_(window_width),
    window_height_(window_height)
{}

// Core behavior
bool StartupScreen::update(){
    auto action = ui_ctx_.input_manager.consume_triggered_action();
    if(!action) return false;
    
    switch (*action){
    case UIAction::OpenFile: {
        const char* filter[] = {"*.csv"};
        const char* path = tinyfd_openFileDialog(
            "Select a point cloud file","", 1, filter, "CSV files", 0
        );
        if (path){
            loaded_file_path_ = path;
            file_name_ = paths::extract_name(loaded_file_path_);
            file_loaded_ = true;
        }
        return true;
    }
    case UIAction::StartSimulation:
        startup_finished_ = true;
        return true;

    case UIAction::ShowInfo:
        display_info_ = true;
        return true;

    case UIAction::HideInfo:
        display_info_ = false;
        return true;

    case UIAction::ToggleUseFileData:
        use_file_data_ = !use_file_data_;
        return true;

    default:
        return false;
    }
}

void StartupScreen::draw(){
    Shader& shader = ui_ctx_.shader;
    Shader& textured_shader = ui_ctx_.textured_shader;
    Renderer& quad_renderer = ui_ctx_.quad_renderer;
    Renderer& textured_quad_renderer = ui_ctx_.textured_quad_renderer;
    TextRenderer& text_renderer = ui_ctx_.regular_text_renderer;
    const glm::mat4& projection = ui_ctx_.ui_projection;

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float x_scale = window_width_ / kInitialWindowWidth;
    float y_scale = window_height_ / kInitialWindowHeight;
    float ui_scale = std::min(x_scale, y_scale);
    
    // Logo
    logo_texture_.bind(0);
    textured_shader.setInt("Texture", 0);
    textured_shader.setVec4("tintColor", Colors::White);
    float logo_w = std::min(window_width_ * ui_scale * 0.2f, kInitialWindowWidth * 0.4f);
    float logo_h = logo_w / kLogoAspect;
    float logo_x = - logo_w * 0.1f;
    float logo_y = window_height_ - logo_h;
    glm::mat4 logo_model = build_ui_quad_model(logo_x, logo_y, logo_w, logo_h);
    textured_quad_renderer.draw(
        textured_shader, projection, glm::mat4(1.0f), logo_model
    );

    // Title text
    Color text_color = Colors::White;
    float textbox_padding = 20.0f * ui_scale;
    float lineshift = 40.0f * ui_scale;
    float textbox_x = window_width_ * 0.5f;
    float textbox_y = window_height_ * 0.6f;
    float line1_scale = 0.4f * ui_scale;
    float line2_scale = 0.3f * ui_scale;

    TextBlockLayout title_block = build_text_block(
        { // Make lines without defining x and y. This is done below
            TextLine("REMOTE CATHODE AUTOPSY PROJECT", line1_scale, text_color, &display_text_renderer_),
            TextLine("An interactive 3D point cloud visualization tool", line2_scale, text_color, &display_text_renderer_)
        },
        textbox_x, textbox_y,
        lineshift, textbox_padding
    );

    for (auto& text : title_block.texts){
        text.renderer->render_text(text);
    }

    // File upload target
    float upload_w = 400.0f * x_scale;
    float upload_h = 70.0f * y_scale;
    float upload_x = window_width_ * 0.5f - upload_w * 0.5f;
    float upload_y = window_height_ * 0.35f;
    float upload_scale = 0.25f * ui_scale;
    std::string upload_label = file_loaded_ ? file_name_ : "Select CSV file";

    if (file_loaded_){
        float file_name_width = std::get<0>(
            text_renderer.measure_text(file_name_, upload_scale)
        );
        if (file_name_width > upload_w){
            upload_x += upload_w * 0.5f;
            upload_w = file_name_width + 30.0f;
            upload_x -= upload_w * 0.5f;
        }
    }
    ButtonSpec upload_btn_spec{
        .box = UIBox{
            .x = upload_x, 
            .y = upload_y, 
            .width = upload_w, 
            .height = upload_h
        }, 
        .scale = upload_scale,
        .box_color = std::nullopt,
        .label_color = std::nullopt,
        .label_texture = nullptr,
        .label = file_loaded_ ? file_name_ : "Select CSV file",
        .action = UIAction::OpenFile,
        .enabled = !display_info_
    };
    draw_button(ui_ctx_, UIPage::StartMenu, upload_btn_spec);

    // Simulation button
    float button_w = 400.0f * x_scale;
    float button_h = 50.0f * y_scale;

    ButtonSpec sim_btn_spec{
        .box = UIBox{
            .x = window_width_ * 0.5f - button_w * 0.5f,
            .y = upload_y - button_h - 20.0f,
            .width = button_w,
            .height = button_h
        },
        .scale = 0.25f * ui_scale,
        .box_color = std::nullopt,
        .label_color = std::nullopt,
        .label_texture = nullptr,
        .label = "Start simulation",
        .action = UIAction::StartSimulation,
        .enabled = (file_loaded_ || !use_file_data_) && !display_info_
    };
    draw_button(ui_ctx_, UIPage::StartMenu, sim_btn_spec);
    
    // Checkbox text (no button functionality)
    ButtonSpec sim_type_btn_1{
        .box = UIBox{
            .x = window_width_ * 0.5f - button_w * 0.5f,
            .y = upload_y - 2.0f*(button_h + 20.0f),
            .width = button_w,
            .height = button_h
        },
        .scale = 0.25f * ui_scale,
        .box_color = std::nullopt,
        .label_color = std::nullopt,
        .label_texture = nullptr,
        .label = "Use pseudo-random example data:",
        .action = UIAction::ToggleUseFileData,
        .enabled = !display_info_ // just to get the same color
    };
    draw_button(ui_ctx_, UIPage::StartMenu, sim_type_btn_1);

    // Checkbox (should use generated data or not)
    ButtonSpec sim_type_btn_2{
        .box = UIBox{
            .x = window_width_ * 0.5f + button_w * 0.5f + 10.0f,
            .y = upload_y - 2.0f*(button_h + 20.0f),
            .width = button_h,
            .height = button_h
        },
        .scale = 0.4f * ui_scale,
        .box_color = std::nullopt,
        .label_color = Colors::Background,
        .label_texture = nullptr,
        .label = use_file_data_ ? "" : "X",
        .action = UIAction::ToggleUseFileData,
        .enabled = !display_info_
    };
    draw_button(ui_ctx_, UIPage::StartMenu, sim_type_btn_2);
    
    // Info tab button
    button_w = 200.0f * x_scale;
    ButtonSpec info_btn_spec{
        .box = UIBox{
            .x = window_width_ * 0.5f - button_w * 0.5f,
            .y = upload_y - 3.0f * (button_h + 20.0f),
            .width = button_w,
            .height = button_h
        },
        .scale = 0.25f * ui_scale,
        .box_color = std::nullopt,
        .label_color = std::nullopt,
        .label_texture = nullptr,
        .label = "Show info",
        .action = UIAction::ShowInfo,
        .enabled = !display_info_
    };
    draw_button(ui_ctx_, UIPage::StartMenu, info_btn_spec);

    // Info tab
    float info_w = window_width_ * 0.4f;
    float info_h = window_height_ * 0.4f;
    float info_x = window_width_ * 0.5f;
    float info_y = window_height_ * 0.5f;
    float info_title_scale = 0.4f * ui_scale;
    float info_text_scale = 0.3f * ui_scale;

    text_color = Colors::White;
    TextBlockLayout info_block = build_text_block(
        {
            TextLine("Expected CSV format:", info_title_scale, text_color, &text_renderer),
            TextLine("Header: x, y, z", info_text_scale, text_color, &text_renderer),
            TextLine("With corresponding data lines", info_text_scale, text_color, &text_renderer),
            TextLine("(Where z denotes the \"up\"-direction)", info_text_scale, text_color, &text_renderer)
        },
        info_x, info_y,
        lineshift, textbox_padding
    );

    if (display_info_){
        shader.setVec4("color", Colors::WhiteHalfOpaque);
        quad_renderer.draw(shader, projection, glm::mat4(1.0f), 
            build_ui_quad_model(0.0f, 0.0f, window_width_, window_height_)
        );
        shader.setVec4("color", Colors::Background);
        quad_renderer.draw(shader, projection, glm::mat4(1.0f), 
            build_ui_quad_model(
                info_x - info_w * 0.5f,
                info_y - info_h * 0.5f,
                info_w, info_h)
        );
        for (auto& text : info_block.texts){
            text_renderer.render_text(text);
        }

        float button_h = 50.0f * ui_scale; //Square
        ButtonSpec close_info_btn_spec{
            .box = UIBox{
                .x = info_x + 0.5f * info_w - button_h - 10.0f,
                .y = info_y + 0.5f * info_h - button_h - 10.0f,
                .width = button_h,
                .height = button_h
            },
            .scale = 0.4f * ui_scale,
            .box_color = Colors::WhiteHalfOpaque, 
            .label_color = Colors::Background,
            .label_texture = &close_btn_texture_,
            .label = "",
            .action = UIAction::HideInfo,
            .enabled = true
        };
        draw_button(ui_ctx_, UIPage::StartMenu, close_info_btn_spec);
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}