#pragma once

#include "glfw_context.hpp"

#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <optional>
#include <unordered_map>

class Camera;

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
    GoToMainMenu,
    // Add more actions if needed.
};

class InputManager {
    public:
        InputManager(Window& window, Camera& camera, GLFWUserContext* context);
        ~InputManager();

        // Core behavior
        void process_input(float delta_time);
        
        // State queries
        bool is_paused() const {return paused_;}
        bool has_active_input() const;
        UIPage get_active_page() const {return active_page_;}
        
        // State mutators
        void set_paused(bool value) {paused_ = value;}
        void set_active_page(UIPage page) {active_page_ = page;}
        void set_mode(InputMode mode);
        void set_button_box(UIPage page, UIAction action, UIBox box);
        void remove_button_box(UIPage page, UIAction action);
        void set_movement_speed(float speed){ movement_speed_multiplier_ = speed; }
        void set_min_height(float min_height){ min_height_ = min_height;}
        
        // Actions
        std::optional<UIAction> consume_triggered_action();

    private:
        // Helpers
        bool hit_test_active_page(double x, double y, UIAction& out_action) const;        
        void orbit(double xpos, double ypos);

        // Static callbacks (GLFW C API)
        static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
        static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
        static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

        // References
        Window& window_;
        Camera& camera_;

        // Mode and page state
        InputMode mode_ = InputMode::Locked;
        UIPage active_page_ = UIPage::StartMenu;
        std::unordered_map<UIPage, std::unordered_map<UIAction, UIBox>> page_buttons_;
        std::optional<UIAction> triggered_action_;
        bool paused_ = false;

        // Cursor state
        GLFWcursor* arrow_cursor_ = nullptr;
        GLFWcursor* hand_cursor_ = nullptr;
        bool hovering_button_ = false;

        // Movement and orbit state
        double last_x_ = 0.0;
        double last_y_ = 0.0;
        bool orbiting_ = false; 
        bool first_mouse_ = true;
        float mouse_sensitivity_ = 0.1f;
        float movement_speed_multiplier_ = 50.0f;
        float min_height_ = 1.0f;
};