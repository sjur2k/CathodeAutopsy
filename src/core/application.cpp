#include "core/application.hpp"
#include "core/paths.hpp"
#include "data/point_cloud.hpp"
#include "core/scene_config.hpp"
#include "data/primitives.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <string>
#include <vector>

Application::Application() : 
    glfw_context_(),
    window_(window_width_, window_height_, "Cathode Visualization", &glfw_context_), 
    camera_(
        SceneConfig::kInitialCameraPose,
        SceneConfig::kFieldOfView,
        static_cast<float>(window_width_) / static_cast<float>(window_height_),
        SceneConfig::kNearPlane,
        SceneConfig::kFarPlane),
    shader_("shaders/basic.vert", "shaders/basic.frag"),
    textured_shader_("shaders/textured.vert", "shaders/textured.frag"),
    logo_texture_("textures/hydro_logo.bmp"),
    close_btn_texture_("textures/close_btn.png"),
    grid_(
        SceneConfig::kCellWidth  / SceneConfig::kResolution, 
        SceneConfig::kCellLength / SceneConfig::kResolution
    ),
    point_cloud_renderer_(grid_.generate_random_point_cloud(), GL_POINTS),
    quad_renderer_(Primitives::unit_quad(), GL_TRIANGLES),
    textured_quad_renderer_(Primitives::textured_unit_quad(), GL_TRIANGLES),
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
        regular_text_renderer_, display_text_renderer_,
        input_manager_, 
        ui_projection_,
        window_width_, window_height_
    },
    startup_screen_(ui_ctx_, logo_texture_, close_btn_texture_),
    pause_screen_(ui_ctx_),
    running_screen_(ui_ctx_, camera_, point_cloud_renderer_)
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
                        PointCloud cloud;
                        cloud.load(startup_screen_.file_path());
                        point_cloud_renderer_.update_vertices(cloud.points());
                        
                        auto bounds = cloud.compute_bounds();
                        camera_.frame_bounds(bounds.center, bounds.radius);
                        input_manager_.set_movement_speed(bounds.radius * 0.5f);

                        Position floor_pos(
                            bounds.center.x, 
                            bounds.min_y - 0.1f * abs(bounds.min_y), 
                            bounds.center.z
                        );
                        input_manager_.set_min_height(floor_pos.y + bounds.radius * 0.1f);
                        
                        running_screen_.set_floor_extent(
                            floor_pos,
                            bounds.half_extent_x,
                            bounds.half_extent_z
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

                running_screen_.update(delta_time);

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

                needs_redraw_ |= pause_screen_.update();

                if (pause_screen_.should_exit()){
                    input_manager_.set_paused(false);
                    state_ = AppState::Startup;
                    input_manager_.set_active_page(UIPage::StartMenu);
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
                break;
            }
        }
    }
}

// Rendering helpers
void Application::render_running() {
    clear_screen();
    running_screen_.draw();
}

void Application::render_paused(){
    clear_screen();
    running_screen_.draw();
    pause_screen_.draw();
}

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