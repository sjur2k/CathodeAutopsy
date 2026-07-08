#include <iostream>
#include "application.hpp"
#include "mesh.hpp"
#include "color.hpp"
#include "renderer.hpp"
#include <GLFW/glfw3.h>

namespace {
    constexpr int kWindowWidth = 1920;
    constexpr int kWindowHeight = 1080;

    // Assumed constants (SI-units)
    constexpr float kCellWidth = 10.0f;
    constexpr float kCellLength = 4.0f;
    constexpr float kResolution = 0.05f;

    // Constants for rendering
    constexpr float kGridWidth = kCellWidth / kResolution;
    constexpr float kGridLength = kCellLength / kResolution;
    constexpr float kPadding = 10.0f;

    // Camera parameters
    constexpr float kFieldOfView = 90.0f;
    constexpr float kNearPlane = 0.1f;
    constexpr float kFarPlane = 300.0f;
}

static std::vector<glm::vec3> build_point_cloud(Grid& grid){
    grid.fill_random_smooth();
    return grid.get_point_cloud_vec3();
}

static std::vector<glm::vec3> build_floor(){
    return Mesh::floor(
            0.0f - kPadding, kGridLength + kPadding, // X-dims
            0.0f - kPadding, kGridWidth + kPadding,  // Z-dims
            0.0f); // Y (height above zero)
}

static std::vector<glm::vec3> build_pause_quad(){
    return {
        {-1.0f, -1.0f, 0.0f}, { 1.0f, -1.0f, 0.0f}, { 1.0f,  1.0f, 0.0f},
        {-1.0f, -1.0f, 0.0f}, { 1.0f,  1.0f, 0.0f}, {-1.0f,  1.0f, 0.0f},
    };
}

Application::Application() : window_(kWindowWidth, kWindowHeight, "Cathode Visualization"), 
    camera_(
        Pose(Position(0.0f, 70.0f, 105.0f), Rotation(-60.0f, 90.0f, 0.0f)), // Looks down on grid defined in x-z plane
        kFieldOfView,
        static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight),
        kNearPlane,
        kFarPlane
    ),
    shader_("shaders/basic.vert", "shaders/basic.frag"),
    grid_(kCellWidth / kResolution, kCellLength / kResolution),
    renderer_(build_point_cloud(grid_), GL_POINTS),
    floor_renderer_(build_floor(), GL_TRIANGLES),
    pause_overlay_renderer_(build_pause_quad(), GL_TRIANGLES),
    input_manager_(window_.get_handle(), camera_)
{
    glEnable(GL_DEPTH_TEST);
}

void Application::run() {
    while (!window_.should_close()) {
        float current_time = static_cast<float>(glfwGetTime());
        float delta_time = current_time - last_frame_time_;
        last_frame_time_ = current_time;
        if(!input_manager_.is_paused()){
            update(delta_time);
        }
        render();
        window_.swap_buffers();
        glfwPollEvents();
    }
}

void Application::update(float delta_time) {
    input_manager_.process_input(delta_time);
}

void Application::render() {
    auto [r,g,b,opacity] = Colors::Background;
    glClearColor(r,g,b,opacity);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 proj = camera_.get_projection_matrix();
    glm::mat4 view = camera_.get_view_matrix();
    
    shader_.setVec4("color", Colors::Cyan);
    renderer_.draw(shader_, proj, view, glm::mat4(1.0f));
    shader_.setVec4("color", Colors::Floor);
    floor_renderer_.draw(shader_, proj, view, glm::mat4(1.0f));
    if(input_manager_.is_paused()){
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        shader_.setVec4("color", glm::vec4(1.0f, 1.0f, 1.0f, 0.5f));
        pause_overlay_renderer_.draw(shader_, glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f));
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }
}