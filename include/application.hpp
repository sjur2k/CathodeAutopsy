#pragma once

#include <vector>
#include <glm/glm.hpp>
#include "window.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "renderer.hpp"
#include "input_manager.hpp"
#include "grid.hpp"

class Application {
public:
    Application();
    void run();

private:
    Window window_;
    Camera camera_;
    Shader shader_;
    Grid grid_;
    Renderer renderer_;
    Renderer floor_renderer_;
    InputManager input_manager_;

    float last_frame_time_ = 0.0f;

    void update(float delta_time);
    void render();
};