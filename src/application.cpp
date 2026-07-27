#include <iostream>
#include <string>
#include "application.hpp"
#include "mesh.hpp"
#include "color.hpp"
#include "renderer.hpp"
#include "geometry.hpp"
#include "paths.hpp"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "tinyfiledialogs/tinyfiledialogs.h"

namespace {
    // Assumed constants (SI-units)
    constexpr float kCellWidth = 10.0f;
    constexpr float kCellLength = 4.0f;
    constexpr float kResolution = 0.05f;

    // Constants for rendering
    constexpr float kGridWidth = kCellWidth / kResolution;
    constexpr float kGridLength = kCellLength / kResolution;
    constexpr float kPadding = 10.0f;
    constexpr float kInitialWindowWidth = 1920.0f;
    constexpr float kInitialWindowHeight = 1080.0f;
    constexpr float kInitialAspect = kInitialWindowWidth / kInitialWindowHeight;
    constexpr float kLogoNativeWidth = 842.0f;
    constexpr float kLogoNativeHeight = 596.0f;
    constexpr float kLogoAspect = kLogoNativeWidth / kLogoNativeHeight;

    // Camera parameters
    constexpr float kFieldOfView = 90.0f;
    constexpr float kNearPlane = 0.1f;
    constexpr float kFarPlane = 300.0f;
}

static std::vector<glm::vec3> build_point_cloud(Grid& grid){
    grid.fill_random_smooth();
    return grid.get_point_cloud_vec3();
}

static std::vector<glm::vec3> build_unit_quad(){
    return {
        {-0.5f, -0.5f, 0.0f}, { 0.5f, -0.5f, 0.0f}, { 0.5f,  0.5f, 0.0f},
        {-0.5f, -0.5f, 0.0f}, { 0.5f,  0.5f, 0.0f}, {-0.5f,  0.5f, 0.0f},
    };
}

static std::vector<TexturedVertex> build_textured_unit_quad(){
    return {
        {{-0.5f,-0.5f,0.0f},{0.0f,0.0f}}, {{0.5f,-0.5f,0.0f},{1.0f,0.0f}}, 
        {{0.5f,0.5f,0.0f},{1.0f,1.0f}}, {{-0.5f,-0.5f,0.0f},{0.0f,0.0f}}, 
        {{0.5f,0.5f,0.0f},{1.0f,1.0f}}, {{-0.5f,0.5f,0.0f},{0.0f,1.0f}}
    };
}

static glm::mat4 build_floor_model(){
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

static glm::mat4 build_ui_quad_model(float x, float y, float width, float height){
    glm::mat4 model = glm::translate(glm::mat4(1.0f),
        glm::vec3(x + width / 2.0f, y + height / 2.0f, 0.0f)
    );
    model = glm::scale(model, glm::vec3(width, height, 1.0f));
    return model;
}

static glm::mat4 build_ui_quad_model(TextBlockLayout block){
    return build_ui_quad_model(block.box_x, block.box_y, block.box_width, block.box_height);
}

static glm::mat4 build_ui_quad_model(UIBox box){
    return build_ui_quad_model(box.x, box.y, box.width, box.height);
}

Application::Application() : 
    glfw_context_(),
    window_(window_width_, window_height_, "Cathode Visualization", &glfw_context_), 
    camera_(
        Pose(Position(0.0f, 70.0f, 105.0f), Rotation(-60.0f, 90.0f, 0.0f)), // Looks down on grid defined in x-z plane
        kFieldOfView,
        static_cast<float>(window_width_) / static_cast<float>(window_height_),
        kNearPlane,
        kFarPlane),
    shader_(
        paths::asset("shaders/basic.vert").string().c_str(),
        paths::asset("shaders/basic.frag").string().c_str()
    ),
    textured_shader_(
        paths::asset("shaders/textured.vert").string().c_str(),
        paths::asset("shaders/textured.frag").string().c_str()
    ),
    logo_texture_(
        paths::asset("textures/hydro_logo.bmp").string().c_str()
    ),
    close_btn_texture_(
        paths::asset("textures/close_btn.png").string().c_str()
    ),
    grid_(kCellWidth / kResolution, kCellLength / kResolution),
    point_cloud_renderer_(build_point_cloud(grid_), GL_POINTS),
    quad_renderer_(build_unit_quad(), GL_TRIANGLES),
    textured_quad_renderer_(build_textured_unit_quad(), GL_TRIANGLES),
    display_text_renderer_(
        paths::asset("fonts/IvarDisplayHydro-Regular.ttf").string().c_str(), 
        static_cast<int>(192), window_width_, window_height_
    ),
    regular_text_renderer_(
        paths::asset("fonts/IvarTextHydro-Regular.ttf").string().c_str(), 
        static_cast<int>(96), window_width_, window_height_
    ),
    input_manager_(window_, camera_, &glfw_context_)
{
    glEnable(GL_DEPTH_TEST);
    
    window_.set_resize_callback([this](int width, int height){
        window_width_ = width;
        window_height_ = height;
        camera_.set_aspect_ratio(static_cast<float>(width) / static_cast<float>(height));
        ui_projection_ = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
        display_text_renderer_.set_screen_size(width, height);
        regular_text_renderer_.set_screen_size(width, height);
        needs_redraw_ = true;
    });
    window_.set_refresh_callback([this](){
        switch (state_){
            case AppState::Startup: render_startup(); break;
            case AppState::Running: render_running(); break;
            case AppState::Paused: render_paused(); break;
        }
        window_.swap_buffers();
    });
    window_width_ = window_.width();
    window_height_ = window_.height();
    ui_projection_ = glm::ortho(0.0f, static_cast<float>(window_width_), 
                                0.0f, static_cast<float>(window_height_));
    display_text_renderer_.set_screen_size(window_width_, window_height_);
    regular_text_renderer_.set_screen_size(window_width_, window_height_);
    if (window_.needs_initial_fullscreen()){
        window_.toggle_fullscreen();
    }
}

void Application::run() {
    input_manager_.set_active_page(UIPage::StartMenu);
    input_manager_.set_mode(InputMode::Interactive);
    int last_fb_width = 0, last_fb_height = 0;
    glfwGetFramebufferSize(window_.get_handle(), &last_fb_width, &last_fb_height);
    last_camera_pose_ = camera_.get_pose();

    while (!window_.should_close()) {
        switch(state_){
            case AppState::Startup: {
                if (!needs_redraw_){
                    glfwWaitEvents();
                }

                update_startup();

                if (needs_redraw_){
                    render_startup();
                    window_.swap_buffers();
                    needs_redraw_ = false;
                }

                if (startup_finished_){
                    state_ = AppState::Running;
                    input_manager_.set_active_page(UIPage::None);
                    input_manager_.set_mode(InputMode::Locked);
                    last_frame_time_ = static_cast<float>(glfwGetTime());
                    needs_redraw_ = true;
                }
                break;
            }
            case AppState::Running: {
                if (input_manager_.has_active_input()){ // Prevents busy idling
                    glfwPollEvents();
                } else {
                    glfwWaitEvents();
                    last_frame_time_ = static_cast<float>(glfwGetTime());
                }

                if (input_manager_.is_paused()){
                    state_ = AppState::Paused;
                    input_manager_.set_active_page(UIPage::PauseMenu);
                    input_manager_.set_mode(InputMode::Interactive);
                    needs_redraw_ = true;
                    break;
                }
                
                needs_redraw_ |= check_resize(last_fb_width, last_fb_height);

                float current_time = static_cast<float>(glfwGetTime());
                float delta_time = current_time - last_frame_time_;
                last_frame_time_ = current_time;

                update_running(delta_time);

                Pose current_pose = camera_.get_pose();
                if (current_pose != last_camera_pose_){
                    needs_redraw_ = true,
                    last_camera_pose_ = current_pose;
                }
                
                if(needs_redraw_){ // Assumes constant grid, no moving objects etc..
                    render_running();
                    window_.swap_buffers();
                    needs_redraw_ = false;
                }
                break;
            } 
            case AppState::Paused: {
                glfwWaitEvents();

                if (!input_manager_.is_paused()){
                    state_ = AppState::Running;
                    input_manager_.set_active_page(UIPage::None);
                    input_manager_.set_mode(InputMode::Locked);
                    last_frame_time_ = static_cast<float>(glfwGetTime());
                    needs_redraw_ = true;
                    break;
                }
                
                needs_redraw_ |= check_resize(last_fb_width, last_fb_height); 

                if (needs_redraw_){
                    render_paused();
                    window_.swap_buffers();
                    needs_redraw_ = false;
                }
                
                update_paused(); // Might switch state_ to Startup
                break;
            }
        }
    }
}

void Application::draw_button(UIPage page, const ButtonSpec& spec){
    auto [x,y,w,h] = spec.box;
    glm::mat4 button_model = build_ui_quad_model(spec.box);

    Color box_color = spec.box_color.value_or(
        spec.enabled ? Colors::WhiteHalfOpaque : Colors::WhiteQuarterOpaque
    );
    Color label_color = spec.label_color.value_or(
        spec.enabled ? Colors::White : Colors::WhiteHalfOpaque
    );
    
    shader_.setVec4("color", box_color);
    quad_renderer_.draw(shader_, ui_projection_, glm::mat4(1.0f), button_model);

    if(spec.label_texture){
        spec.label_texture->bind(0);
        textured_shader_.setInt("Texture", 0);
        textured_shader_.setVec4("tintColor", label_color);
        textured_quad_renderer_.draw(
            textured_shader_, ui_projection_, glm::mat4(1.0f), button_model
        );    
    } else {
        glm::vec2 label_pos = regular_text_renderer_.center_text_in_box(
            spec.label, spec.scale,
            x + w * 0.5f,
            y + h * 0.5f
        );
        TextLine label(spec.label, label_pos, spec.scale, label_color);
        regular_text_renderer_.render_text(label);
    }

    if (spec.enabled){
        input_manager_.set_button_box(page, spec.action, spec.box);
    } else {
        input_manager_.remove_button_box(page, spec.action);
    }
}

void Application::clear_screen(){
    auto [r,g,b,opacity] = Colors::Background;
    glClearColor(r,g,b,opacity);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Application::draw_scene(){
    glm::mat4 proj = camera_.get_projection_matrix();
    glm::mat4 view = camera_.get_view_matrix();

    shader_.setVec4("color", Colors::Cyan);
    point_cloud_renderer_.draw(shader_, proj, view, glm::mat4(1.0f));
    
    shader_.setVec4("color", Colors::Floor);
    quad_renderer_.draw(shader_, proj, view, build_floor_model());
}

void Application::draw_hud(){
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Color text_color = Colors::Background;
    float textbox_padding = 10.0f;

    std::string msg = "For information, press ESC";
    float text_scale = 0.2f;
    auto [width, ascent, descent] = regular_text_renderer_.measure_text(msg, text_scale);
    float height = ascent - descent;
    glm::vec2 text_coords(textbox_padding, window_height_ - height - textbox_padding);
    TextLine text(msg, text_coords, text_scale, text_color);

    glm::mat4 textbox_model = build_ui_quad_model(
        0.0f,
        window_height_ - height - 2.0f * textbox_padding,
        width + 2 * textbox_padding,
        height + 2 * textbox_padding
    );

    shader_.setVec4("color", Colors::Background);
    quad_renderer_.draw(shader_, ui_projection_, glm::mat4(1.0f), textbox_model);
    shader_.setVec4("color", Colors::WhiteHalfOpaque);
    quad_renderer_.draw(shader_, ui_projection_, glm::mat4(1.0f), textbox_model);
    regular_text_renderer_.render_text(text);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void Application::draw_pause_overlay(){
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Color text_color = Colors::White;
    float textbox_padding = 20.0f;
    float lineshift = 30.0f;

    float x_scale = window_width_ / kInitialWindowWidth;
    float y_scale = window_height_ / kInitialWindowHeight;
    float ui_scale = std::min(x_scale, y_scale);
    
    TextBlockLayout top_block = build_text_block(
        {
            TextLine("PAUSED", 0.0f, 0.0f, 0.5f, text_color, &display_text_renderer_),
            TextLine("Press ESC to resume", 0.0f, 0.0f, 0.4f, text_color, &regular_text_renderer_),
        },
        window_width_ * 0.5f, window_height_ * 0.85f,
        lineshift, textbox_padding
    );

    TextBlockLayout bottom_block = build_text_block(
        {
            TextLine("Controls", 0.0f, 0.0f, 0.4f, text_color, &display_text_renderer_),
            TextLine("W/A/S/D: Horizontal Movement", 0.0f, 0.0f, 0.3f, text_color, &regular_text_renderer_),
            TextLine("LSHIFT/SPACE: Vertical Movement", 0.0f, 0.0f, 0.3f, text_color, &regular_text_renderer_),
            TextLine("MOUSE1 (HOLD): Pan", 0.0f, 0.0f, 0.3f, text_color, &regular_text_renderer_)
        },
        window_width_ * 0.5f, window_height_ * 0.5f,
        lineshift, textbox_padding
    );

    std::string main_menu_btn_text = "Return to main menu";
    float main_menu_btn_scale = 0.3f * ui_scale;
    auto [width, ascent, descent] = display_text_renderer_.measure_text(
        main_menu_btn_text,
        main_menu_btn_scale
    );
    width += 2.0f * textbox_padding;
    float height = ascent - descent + 2.0f * textbox_padding;
    
    ButtonSpec main_menu_btn{
        .box = UIBox{
            .x = window_width_ * 0.5f - width * 0.5f,
            .y = window_height_ * 0.5f - bottom_block.box_height * 0.5f - height - textbox_padding,
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
    shader_.setVec4("color", Colors::WhiteHalfOpaque);
    quad_renderer_.draw(shader_, ui_projection_, glm::mat4(1.0f), 
        build_ui_quad_model(0.0f,0.0f,window_width_, window_height_)
    );
    
    shader_.setVec4("color", Colors::Background);
    quad_renderer_.draw(shader_, ui_projection_, glm::mat4(1.0f), 
        build_ui_quad_model(top_block)
    );
    quad_renderer_.draw(shader_, ui_projection_, glm::mat4(1.0f), 
        build_ui_quad_model(bottom_block)
    );

    for (auto& text : top_block.texts){
        text.renderer->render_text(text);
    }
    for (auto& text : bottom_block.texts){
        text.renderer->render_text(text);
    }

    draw_button(UIPage::PauseMenu, main_menu_btn);

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void Application::draw_startup_menu(){
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float x_scale = window_width_ / kInitialWindowWidth;
    float y_scale = window_height_ / kInitialWindowHeight;
    float ui_scale = std::min(x_scale, y_scale);
    
    // Logo
    logo_texture_.bind(0);
    textured_shader_.setInt("Texture", 0);
    textured_shader_.setVec4("tintColor", Colors::White);
    float logo_w = std::min(window_width_ * ui_scale * 0.2f, kInitialWindowWidth * 0.4f);
    float logo_h = logo_w / kLogoAspect;
    float logo_x = - logo_w * 0.1f;
    float logo_y = window_height_ - logo_h;
    glm::mat4 logo_model = build_ui_quad_model(logo_x, logo_y, logo_w, logo_h);
    textured_quad_renderer_.draw(
        textured_shader_, ui_projection_, glm::mat4(1.0f), logo_model
    );

    // Text
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
    float upload_scale = 0.3f * ui_scale;
    std::string upload_label = file_loaded_ ? file_name_ : "Select CSV file";

    if (file_loaded_){
        float file_name_width = std::get<0>(
            regular_text_renderer_.measure_text(file_name_, upload_scale)
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
    draw_button(UIPage::StartMenu, upload_btn_spec);

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
    draw_button(UIPage::StartMenu, sim_btn_spec);
    
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
    draw_button(UIPage::StartMenu, sim_type_btn_1);

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
    draw_button(UIPage::StartMenu, sim_type_btn_2);
    
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
    draw_button(UIPage::StartMenu, info_btn_spec);

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
            TextLine("Expected CSV format:", info_title_scale, text_color, &regular_text_renderer_),
            TextLine("Header: x, y, z", info_text_scale, text_color, &regular_text_renderer_),
            TextLine("With corresponding data lines", info_text_scale, text_color, &regular_text_renderer_),
            TextLine("(Where z denotes the \"up\"-direction)", info_text_scale, text_color, &regular_text_renderer_)
        },
        info_x, info_y,
        lineshift, textbox_padding
    );

    if (display_info_){
        shader_.setVec4("color", Colors::WhiteHalfOpaque);
        quad_renderer_.draw(shader_, ui_projection_, glm::mat4(1.0f), 
            build_ui_quad_model(0.0f,0.0f,window_width_, window_height_)
        );
        shader_.setVec4("color", Colors::Background);
        quad_renderer_.draw(shader_, ui_projection_, glm::mat4(1.0f), 
            build_ui_quad_model(
                info_x - info_w * 0.5f,
                info_y - info_h * 0.5f,
                info_w, info_h)
        );
        for (auto& text : info_block.texts){
            regular_text_renderer_.render_text(text);
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
        draw_button(UIPage::StartMenu, close_info_btn_spec);
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

bool Application::check_resize(int& last_fb_width, int& last_fb_height){
    int fb_width, fb_height;
    glfwGetFramebufferSize(window_.get_handle(), &fb_width, &fb_height);
    if (fb_width != last_fb_width || fb_height != last_fb_height){
        last_fb_width = fb_width;
        last_fb_height = fb_height;
        return true;
    }
    return false;
}

void Application::update_startup(){
    if (auto action = input_manager_.consume_triggered_action()){
        switch (*action)
        {
        case UIAction::OpenFile: {
            const char* filter[] = {"*.csv"};
            const char* path = tinyfd_openFileDialog("Select a point cloud file","", 1, filter, "CSV files", 0);
            if (path){
                loaded_file_path_ = path;
                file_name_ = paths::extract_name(loaded_file_path_);
                file_loaded_ = true;
            }
            last_frame_time_ = static_cast<float>(glfwGetTime());
            needs_redraw_ = true;
            break;
        }
        case UIAction::StartSimulation:
            startup_finished_ = true;
            if (file_loaded_ && use_file_data_){
                point_cloud_renderer_.update_vertices(
                    point_cloud_renderer_.load_CSV(loaded_file_path_)
                );
            }
            break;

        case UIAction::ShowInfo:
            display_info_ = true;
            needs_redraw_ = true;
            break;

        case UIAction::HideInfo:
            display_info_ = false;
            needs_redraw_ = true;
            break;

        case UIAction::ToggleFullscreen:
            input_manager_.toggle_fullscreen();
            needs_redraw_ = true;
            break;

        case UIAction::ToggleUseFileData:
            use_file_data_ = !use_file_data_;
            needs_redraw_ = true;
            break;

        default:
            break;
        }
    }
}

void Application::render_startup(){
    clear_screen();
    draw_startup_menu();
}

void Application::update_running(float delta_time) {
    input_manager_.process_input(delta_time);
}

void Application::render_running() {
    clear_screen();
    draw_scene();
    draw_hud();
}

void Application::update_paused(){
    if (auto action = input_manager_.consume_triggered_action()){
        switch (*action){
        case UIAction::GoToMainMenu:
            input_manager_.set_paused(false);
            startup_finished_ = false;
            state_ = AppState::Startup;
            input_manager_.set_active_page(UIPage::StartMenu);
            needs_redraw_ = true;
            break;
        
        default:
            break;
        }
    }
}

void Application::render_paused(){
    clear_screen();
    draw_scene();
    draw_pause_overlay();
}

