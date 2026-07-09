#pragma once

#include <GLFW/glfw3.h>
#include "camera.hpp"

class InputManager {
    public:
        InputManager(GLFWwindow* window, Camera& camera);
        void process_input(float delta_time);
        bool is_paused();
        void set_paused(bool value);
    private:
        GLFWwindow* window_;
        Camera& camera_;
        double last_x_ = 0.0;
        double last_y_ = 0.0;
        bool orbiting_ = false;
        bool paused_ = false;
        glm::vec3 orbit_target_ = glm::vec3(0.0f);
        float orbit_distance_ = 150.0f;
        float orbit_yaw_ = 0.0f;
        float orbit_pitch_ = 45.0f;
        bool first_mouse_ = true;
        float mouse_sensitivity_ = 0.1f;
        float movement_speed_multiplier_ = 50.0f;

        void orbit(double xpos, double ypos);

        static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        //static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};