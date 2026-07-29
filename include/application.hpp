#pragma once

#include "camera.hpp"
#include "geometry.hpp"
#include "glfw_context.hpp"
#include "grid.hpp"
#include "input_manager.hpp"
#include "renderer.hpp"
#include "shader.hpp"
#include "startup_screen.hpp"
#include "text_renderer.hpp"
#include "texture.hpp"
#include "ui_geometry.hpp"
#include "window.hpp"

#include <glm/glm.hpp>

#include <vector>

enum class AppState {
    Startup,
    Running,
    Paused
};

class Application {
public:
    Application();

    // Core Behavior
    void run();

private:
    // Update/render per state
    void update_running(float delta_time);
    void render_running();

    void update_paused();
    void render_paused(); 
    
    void render_startup();
    
    // Shared drawing helpers
    void clear_screen();
    void draw_scene();
    void draw_hud();
    void draw_pause_overlay();
    bool check_resize(int& last_fb_width, int& last_fb_height);  


    int window_width_ = 1920;
    int window_height_ = 1080;
    
    GLFWUserContext glfw_context_; // Must be declared before Window and InputManager
    Window window_;
    Camera camera_;
    Shader shader_;
    Shader textured_shader_;
    Texture logo_texture_;
    Texture close_btn_texture_;
    Grid grid_;
    Renderer point_cloud_renderer_;
    Renderer quad_renderer_;
    Renderer textured_quad_renderer_;
    TextRenderer display_text_renderer_;
    TextRenderer regular_text_renderer_;    
    InputManager input_manager_;
    glm::mat4 ui_projection_;
    UIContext ui_ctx_;

    // Screens
    StartupScreen startup_screen_;

    // App state
    AppState state_ = AppState::Startup;
    Pose last_camera_pose_{};
    float last_frame_time_ = 0.0f;
    bool needs_redraw_ = true;    
};