#include <iostream>
#include "application.hpp"
#include <GLFW/glfw3.h>

namespace {
    constexpr int kWindowWidth = 1920;
    constexpr int kWindowHeight = 1080;

    // Assumed constants (SI-units)
    constexpr float kCellWidth = 10.0f;
    constexpr float kCellLength = 4.0f;
    constexpr float kResolution = 0.05f;

    // Camera parameters
    constexpr float kFieldOfView = 90.0f;
    constexpr float kNearPlane = 0.1f;
    constexpr float kFarPlane = 300.0f;

    constexpr float padding = 10.0f;
    constexpr float width = kCellWidth / kResolution;
    constexpr float length = kCellLength / kResolution;
}

static std::vector<glm::vec3> build_point_cloud(Grid& grid){
    grid.fill_random_smooth();
    return grid.get_point_cloud_vec3();
}

static std::vector<glm::vec3> build_floor(){
    
    return {
        {   0.0f - padding,  0.0f,   0.0f - padding},
        {   0.0f - padding,  0.0f,  width + padding},
        { length + padding,  0.0f,  width + padding},
        {   0.0f - padding,  0.0f,   0.0f - padding},
        { length + padding,  0.0f,  width + padding},
        { length + padding,  0.0f,   0.0f - padding}
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
    input_manager_(window_.get_handle(), camera_)
{
    float padding = 10.0f;
    float width = kCellWidth / kResolution;
    float length = kCellLength / kResolution;
    floor_renderer_ = Renderer(Mesh::floor_plane(0.0f - padding, length + padding, 0.0f - padding, width + padding, 0.0f ), GL_TRIANGLES)
    glEnable(GL_DEPTH_TEST);
}

void Application::run() {
    while (!window_.should_close()) {
        float current_time = static_cast<float>(glfwGetTime());
        float delta_time = current_time - last_frame_time_;
        last_frame_time_ = current_time;

        update(delta_time);
        render();

        window_.swap_buffers();
        glfwPollEvents();
    }
}

void Application::update(float delta_time) {
    input_manager_.process_input(delta_time);
}

void Application::render() {
    glClearColor(0.1f, 0.15f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 proj = camera_.get_projection_matrix();
    glm::mat4 view = camera_.get_view_matrix();
    
    shader_.setVec3("color", glm::vec3(0.0f, 0.0f, 1.0f));
    renderer_.draw(shader_, proj, view, glm::mat4(1.0f));
    shader_.setVec3("color", glm::vec3(1.0f, 1.0f, 1.0f));
    floor_renderer_.draw(shader_, proj, view, glm::mat4(1.0f));
}