#pragma once

#include <GLFW/glfw3.h>
#include "camera.hpp"

class InputManager {
    public:
        InputManager(GLFWwindow* window, Camera& camera);
        void process_input(float delta_time);
    private:
        GLFWwindow* window_;
        Camera& camera_;
        double last_x_ = 0.0;
        double last_y_ = 0.0;
        bool orbiting_ = false;
        bool first_mouse_ = true;
        float mouse_sensitivity_ = 0.1f;
        float movement_speed_multiplier_ = 50.0f;

        void process_keyboard(float delta_time);
        void orbit(double xpos, double ypos);

        static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
};