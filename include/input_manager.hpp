#pragma once

#include <GLFW/glfw3.h>
#include <optional>
#include "camera.hpp"
#include "glfw_context.hpp"

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
    NoAction,
    OpenFile,
    StartSimulation,
    ShowInfo,
    HideInfo,
    ToggleUseFileData,
    ToggleFullscreen,
    GoToMainMenu,
    // Add more actions if needed.
};

class InputManager {
    public:
        InputManager(Window& window, Camera& camera, GLFWUserContext* context);
        ~InputManager();
        void process_input(float delta_time);
        bool is_paused() {return paused_;}
        bool has_active_input() const;
        void set_paused(bool value) {paused_ = value;}
        void toggle_fullscreen();
        void set_mode(InputMode mode);
        UIPage get_active_page() {return active_page_;}
        void set_active_page(UIPage page) {active_page_ = page;}
        void set_button_box(UIPage page, UIAction action, UIBox box){
            page_buttons_[page][action] = box;
        }
        void remove_button_box(UIPage page, UIAction action);
        std::optional<UIAction> consume_triggered_action();

    private:
        Window& window_;
        Camera& camera_;
        InputMode mode_ = InputMode::Locked;
        std::unordered_map<UIPage, std::unordered_map<UIAction, UIBox>> page_buttons_;
        std::optional<UIAction> triggered_action_;
        UIPage active_page_ = UIPage::StartMenu;
        GLFWcursor* arrow_cursor_ = nullptr;
        GLFWcursor* hand_cursor_ = nullptr;
        bool hovering_button_ = false;

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
        
        bool hit_test_active_page(double x, double y, UIAction& out_action) const;        

        void orbit(double xpos, double ypos);

        static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
        //static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
};