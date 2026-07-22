#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "window.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "renderer.hpp"
#include "input_manager.hpp"
#include "grid.hpp"
#include "text_renderer.hpp"
#include "geometry.hpp"
#include "texture.hpp"

enum class AppState {
    Startup,
    Running,
    Paused
};

class Application {
public:
    Application();
    void run();

private:
    Window window_;
    Camera camera_;
    Shader shader_;
    Shader textured_shader_;
    Texture logo_texture_;
    Grid grid_;
    Renderer point_cloud_renderer_;
    Renderer quad_renderer_;
    Renderer textured_quad_renderer_;
    TextRenderer display_text_renderer_;
    TextRenderer regular_text_renderer_;    
    InputManager input_manager_;
    Pose last_camera_pose_{};
    glm::mat4 ui_projection_;
    float last_frame_time_ = 0.0f;
    bool needs_redraw_ = true;
    bool startup_finished();
    
    void clear_screen();
    void draw_scene();
    void draw_hud();
    void draw_pause_overlay();
    void draw_startup_menu();
    bool check_resize(int& last_fb_width, int& last_fb_height);

    void update_startup();
    void render_startup();

    void update_running(float delta_time);
    void render_running();

    void render_paused();
};