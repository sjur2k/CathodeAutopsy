#pragma once

#include "rendering/camera.hpp"
#include "transform.hpp"
#include "glfw_context.hpp"
#include "data/grid.hpp"
#include "platform/input_manager.hpp"
#include "rendering/renderer.hpp"
#include "rendering/shader.hpp"
#include "ui/startup_screen.hpp"
#include "ui/pause_screen.hpp"
#include "ui/running_screen.hpp"
#include "rendering/text_renderer.hpp"
#include "rendering/texture.hpp"
#include "ui/ui_geometry.hpp"
#include "platform/window.hpp"

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
    // Rendering
    void render_running();
    void render_paused(); 
    void render_startup();
    
    // Shared drawing helpers
    void clear_screen();
    bool check_resize(int& last_fb_width, int& last_fb_height);  

    int window_width_ = 1920;
    int window_height_ = 1080;
    
    GLFWUserContext glfw_context_;
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
    PauseScreen pause_screen_;
    RunningScreen running_screen_;

    // App state
    AppState state_ = AppState::Startup;
    Pose last_camera_pose_{};
    float last_frame_time_ = 0.0f;
    bool needs_redraw_ = true;    
};