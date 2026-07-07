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
}

std::vector<glm::vec3> Application::build_point_cloud(Grid& grid){
    grid.fill_random_smooth();
    return grid.get_point_cloud_vec3();
}

Application::Application(bool verbose) : window_(kWindowWidth, kWindowHeight, "Cathode Visualization"), 
    camera_(
        Pose(Position(50.0f, 100.0f, 100.0f), Rotation(0.0f, 0.0f, 0.0f)),
        kFieldOfView,
        static_cast<float>(kWindowWidth) / static_cast<float>(kWindowHeight),
        kNearPlane,
        kFarPlane
    ),
    shader_("shaders/basic.vert", "shaders/basic.frag"),
    grid_(kCellWidth / kResolution, kCellLength / kResolution),
    renderer_(build_point_cloud(grid_)),
    input_manager_(window_.get_handle(), camera_),
    verbose_(verbose)
{}

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
    glClear(GL_COLOR_BUFFER_BIT);
    bool printed = false;
    if (verbose_ && !printed){
        Position current_pos = camera_.get_pose().position;
        std::cout<<"Cam position: ("<<current_pos.x<<","<<current_pos.y<<","<<current_pos.z<<")"<<std::endl;
        printed = true;
    }
    renderer_.draw(shader_, camera_.get_projection_matrix(), camera_.get_view_matrix(), glm::mat4(1.0f));
}