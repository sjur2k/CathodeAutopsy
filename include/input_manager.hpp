#pragma once

#include <GLFW/glfw3.h>
#include <optional>
#include "camera.hpp"

enum class InputMode {
    Locked, // Flying camera mode
    Interactive // Menu mode
};

struct UIBox {
    float x = 0, y = 0, width = 0, height = 0;
    bool contains(double px, double py) const {
        return px >= x && px <= x + width && py >= y && py <= y + height;
    }
};

enum class UIPage {
    StartMenu,
    None, // While running, only HUD is shown.
    PauseMenu,
};

enum class UIAction {
    OpenFile,
    Resume,
    // Add more actions if needed.
};

class InputManager {
    public:
        InputManager(GLFWwindow* window, Camera& camera);
        void process_input(float delta_time);
        bool is_paused() {return paused_;}
        bool has_active_input() const;
        void set_paused(bool value) {paused_ = value;}
        void set_mode(InputMode mode) {mode_ = mode;}
        void set_active_page(UIPage page) {active_page_ = page;}
        void set_button_box(UIPage page, UIAction action, UIBox box){
            page_buttons_[page][action] = box;
        }
    private:
        GLFWwindow* window_;
        Camera& camera_;
        InputMode mode_ = InputMode::Locked;
        std::unordered_map<UIPage, std::unordered_map<UIAction, UIBox>> page_buttons_;
        std::optional<UIAction> triggered_action_;
        UIPage active_page_ = UIPage::StartMenu;
        double last_x_ = 0.0;
        double last_y_ = 0.0;
        bool orbiting_ = false;
        bool paused_ = false;
        bool open_file_requested_ = false;
        glm::vec3 orbit_target_ = glm::vec3(0.0f);
        float orbit_distance_ = 150.0f;
        float orbit_yaw_ = 0.0f;
        float orbit_pitch_ = 45.0f;
        bool first_mouse_ = true;
        float mouse_sensitivity_ = 0.1f;
        float movement_speed_multiplier_ = 50.0f;
        
        std::optional<UIAction> consume_triggered_action();

        void orbit(double xpos, double ypos);

        static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        //static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};