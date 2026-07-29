#include "application.hpp"
#include "paths.hpp"
#include "point_cloud.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <string>
#include <vector>

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
/*     constexpr float kInitialAspect = kInitialWindowWidth / kInitialWindowHeight;
    constexpr float kLogoNativeWidth = 842.0f;
    constexpr float kLogoNativeHeight = 596.0f;
    constexpr float kLogoAspect = kLogoNativeWidth / kLogoNativeHeight; */

    // Camera parameters
    constexpr float kFieldOfView = 90.0f;
    constexpr float kNearPlane = 0.1f;
    constexpr float kFarPlane = 300.0f;

    std::vector<glm::vec3> build_unit_quad(){
        return {
            {-0.5f, -0.5f, 0.0f}, { 0.5f, -0.5f, 0.0f}, { 0.5f,  0.5f, 0.0f},
            {-0.5f, -0.5f, 0.0f}, { 0.5f,  0.5f, 0.0f}, {-0.5f,  0.5f, 0.0f},
        };
    }

    std::vector<TexturedVertex> build_textured_unit_quad(){
        return {
            {{-0.5f,-0.5f,0.0f},{0.0f,0.0f}}, {{0.5f,-0.5f,0.0f},{1.0f,0.0f}}, 
            {{0.5f,0.5f,0.0f},{1.0f,1.0f}}, {{-0.5f,-0.5f,0.0f},{0.0f,0.0f}}, 
            {{0.5f,0.5f,0.0f},{1.0f,1.0f}}, {{-0.5f,0.5f,0.0f},{0.0f,1.0f}}
        };
    }

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

Application::Application() : 
    glfw_context_(),
    window_(window_width_, window_height_, "Cathode Visualization", &glfw_context_), 
    camera_(
        Pose(Position(0.0f, 70.0f, 105.0f), Rotation(-60.0f, 90.0f, 0.0f)), // Looks down on grid defined in x-z plane
        kFieldOfView,
        static_cast<float>(window_width_) / static_cast<float>(window_height_),
        kNearPlane,
        kFarPlane),
    shader_("shaders/basic.vert", "shaders/basic.frag"),
    textured_shader_("shaders/textured.vert", "shaders/textured.frag"),
    logo_texture_("textures/hydro_logo.bmp"),
    close_btn_texture_("textures/close_btn.png"),
    grid_(kCellWidth / kResolution, kCellLength / kResolution),
    point_cloud_renderer_(grid_.generate_random_point_cloud(), GL_POINTS),
    quad_renderer_(build_unit_quad(), GL_TRIANGLES),
    textured_quad_renderer_(build_textured_unit_quad(), GL_TRIANGLES),
    display_text_renderer_(
        "fonts/IvarDisplayHydro-Regular.ttf", 
        static_cast<int>(192), window_width_, window_height_
    ),
    regular_text_renderer_(
        "fonts/IvarTextHydro-Regular.ttf", 
        static_cast<int>(96), window_width_, window_height_
    ),
    input_manager_(window_, camera_, &glfw_context_),
    ui_ctx_{
        shader_, textured_shader_, 
        quad_renderer_, textured_quad_renderer_, 
        regular_text_renderer_, 
        input_manager_, 
        ui_projection_
    },
    startup_screen_(
        ui_ctx_,
        display_text_renderer_,
        logo_texture_,
        close_btn_texture_,
        window_width_,
        window_height_
    )
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
}

// Core behavior
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

                needs_redraw_ |= startup_screen_.update();

                if (needs_redraw_){
                    render_startup();
                    window_.swap_buffers();
                    needs_redraw_ = false;
                }

                if (startup_screen_.finished()){
                    if(startup_screen_.should_load_point_cloud()){
                        point_cloud_renderer_.update_vertices(
                            PointCloud::load_CSV(startup_screen_.file_path()).points()
                        );
                    }
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

// Running
void Application::update_running(float delta_time) {
    input_manager_.process_input(delta_time);
}

void Application::render_running() {
    clear_screen();
    draw_scene();
    draw_hud();
}

// Paused
void Application::update_paused(){
    if (auto action = input_manager_.consume_triggered_action()){
        switch (*action){
        case UIAction::GoToMainMenu:
            input_manager_.set_paused(false);
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

// Startup
void Application::render_startup(){
    clear_screen();
    startup_screen_.draw();
}

// Shared drawing helpers
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

    draw_button(ui_ctx_, UIPage::PauseMenu, main_menu_btn);

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