#include <iostream>
#include "glfw_context.hpp"
#include "camera.hpp"
#include "window.hpp"
#include "input_manager.hpp"

InputManager::InputManager(Window& window, Camera& camera, GLFWUserContext* context) 
    : window_(window), camera_(camera) {
    context->input_manager = this;
    GLFWwindow* window_handle = window_.get_handle();
    glfwSetInputMode(window_handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window_handle, mouse_callback);
    glfwSetMouseButtonCallback(window_handle, mouse_button_callback);
    glfwSetKeyCallback(window_handle, key_callback);
    
    arrow_cursor_ = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    hand_cursor_ = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
}

InputManager::~InputManager(){
    glfwDestroyCursor(arrow_cursor_);
    glfwDestroyCursor(hand_cursor_);
}

void InputManager::process_input(float delta_time) {

    float velocity = movement_speed_multiplier_ * delta_time;
    Pose current_pose = camera_.get_pose();
    
    float yaw_rad = glm::radians(current_pose.rotation.yaw);
    
    Position unit_forward(
        sin(yaw_rad),
        0.0f,
        -cos(yaw_rad)
    );

    Position unit_right(
        cos(yaw_rad),
        0.0f,
        sin(yaw_rad)
    );

    Position unit_up(0.0f,1.0f,0.0f);

    GLFWwindow* window_handle = window_.get_handle();
    if (glfwGetKey(window_handle, GLFW_KEY_W) == GLFW_PRESS){
        current_pose.position += unit_forward*velocity;
    }
    if (glfwGetKey(window_handle, GLFW_KEY_A) == GLFW_PRESS){
        current_pose.position -= unit_right*velocity;
    }
    if (glfwGetKey(window_handle, GLFW_KEY_S) == GLFW_PRESS){
        current_pose.position -= unit_forward*velocity;
    }
    if (glfwGetKey(window_handle, GLFW_KEY_D) == GLFW_PRESS){
        current_pose.position += unit_right*velocity;
    }
    if (glfwGetKey(window_handle, GLFW_KEY_SPACE) == GLFW_PRESS){
        current_pose.position += unit_up*velocity;
    }
    if (glfwGetKey(window_handle, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
        if (current_pose.position.y > 1){
            current_pose.position -= unit_up*velocity;
        }
    }
    camera_.set_pose(current_pose);
}

bool InputManager::has_active_input() const {
    if (orbiting_) return true;
    GLFWwindow* window_handle = window_.get_handle();
    return glfwGetKey(window_handle, GLFW_KEY_W) == GLFW_PRESS
        || glfwGetKey(window_handle, GLFW_KEY_A) == GLFW_PRESS
        || glfwGetKey(window_handle, GLFW_KEY_S) == GLFW_PRESS
        || glfwGetKey(window_handle, GLFW_KEY_D) == GLFW_PRESS
        || glfwGetKey(window_handle, GLFW_KEY_SPACE) == GLFW_PRESS
        || glfwGetKey(window_handle, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
}

void InputManager::set_mode(InputMode mode){
    GLFWwindow* window_handle = window_.get_handle();
    mode_ = mode;
    if (mode_ == InputMode::Interactive){
        glfwSetInputMode(window_handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        hovering_button_ = false;
        glfwSetCursor(window_handle, arrow_cursor_);
    } else if (mode_ == InputMode::Locked){
        glfwSetInputMode(window_handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

void InputManager::remove_button_box(UIPage page, UIAction action){
    auto it = page_buttons_.find(page);
    if (it == page_buttons_.end()) return;
    it->second.erase(action);
}

bool InputManager::hit_test_active_page(double x, double y, UIAction& out_action) const{
    auto it = page_buttons_.find(active_page_);
    if (it == page_buttons_.end()) return false;
    for (auto& [action_id, box] : it->second){
        if (box.contains(x, y)){
            out_action = action_id;
            return true;
        }
    }
    return false;
}

void InputManager::orbit(double xpos, double ypos) {
    if (first_mouse_) {
        last_x_ = xpos;
        last_y_ = ypos;
        first_mouse_ = false;
        return;
    }

    float xoffset = static_cast<float>(xpos - last_x_)*mouse_sensitivity_;
    float yoffset = static_cast<float>(last_y_ - ypos)*mouse_sensitivity_; // Reversed due to convention
    last_x_ = xpos;
    last_y_ = ypos;

    Pose current_pose = camera_.get_pose();
    current_pose.rotation.yaw += xoffset;
    current_pose.rotation.pitch += yoffset;

    // Constrain the pitch
    if (current_pose.rotation.pitch > 89.0f)
        current_pose.rotation.pitch = 89.0f;
    if (current_pose.rotation.pitch < -89.0f)
        current_pose.rotation.pitch = -89.0f;

    camera_.set_pose(current_pose);
}

std::optional<UIAction> InputManager::consume_triggered_action(){
    auto result = triggered_action_;
    triggered_action_.reset();
    return result;
}

void InputManager::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    auto* ctx = static_cast<GLFWUserContext*>(glfwGetWindowUserPointer(window));
    if (!ctx || !ctx->input_manager) return;
    InputManager* input_manager = ctx->input_manager;
    if (input_manager->orbiting_){
        input_manager->orbit(xpos, ypos);
        return;
    }

    if (input_manager->mode_ == InputMode::Interactive){
        int win_height;
        glfwGetWindowSize(window, nullptr, &win_height);
        double flipped_y = win_height - ypos;

        UIAction hit;
        bool now_hovering = input_manager->hit_test_active_page(xpos, flipped_y, hit);

        if (now_hovering != input_manager->hovering_button_){
            input_manager->hovering_button_ = now_hovering;
            glfwSetCursor(
                window, 
                now_hovering ? input_manager->hand_cursor_ : input_manager->arrow_cursor_
            );
        }
    }
}

void InputManager::mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    (void)mods; // Suppress unused-variable-warnings
    auto* ctx = static_cast<GLFWUserContext*>(glfwGetWindowUserPointer(window));
    if (!ctx || !ctx->input_manager) return;
    InputManager* input_manager = ctx->input_manager;

    // check different button actions
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        if (input_manager->mode_ == InputMode::Interactive){
            double x,y;
            glfwGetCursorPos(window,&x,&y);

            int win_height;
            glfwGetWindowSize(window, nullptr, &win_height);
            double flipped_y = win_height - y;

            UIAction hit;
            if (input_manager->hit_test_active_page(x, flipped_y, hit)){
                input_manager->triggered_action_ = hit;
            }
        } else if (input_manager->mode_ == InputMode::Locked){
            input_manager->orbiting_ = true;
            input_manager->first_mouse_ = true;
        }
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
        if(input_manager->mode_ == InputMode::Locked){
            input_manager->orbiting_ = false;
            input_manager->first_mouse_ = false;
        }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
        if (input_manager->mode_ == InputMode::Locked){
            input_manager->camera_.reset_pose();
        }
    }    
}

void InputManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    (void)scancode; (void)mods; // Suppress unused-variable-warnings
    if(action != GLFW_PRESS) return;
    auto* ctx = static_cast<GLFWUserContext*>(glfwGetWindowUserPointer(window));
    if (!ctx || !ctx->input_manager || !ctx->window) return;
    InputManager* input_manager = ctx->input_manager;
    // Window* active_window = ctx->window;

    switch(key)
    {
    case GLFW_KEY_T:
        input_manager->camera_.print_pose();
        break;
    case GLFW_KEY_Q:
        glfwSetWindowShouldClose(window, true);
        break;
    case GLFW_KEY_ESCAPE:{
        UIPage page = input_manager->get_active_page();
        if (page == UIPage::None || page == UIPage::PauseMenu){
            input_manager->set_paused(!input_manager->is_paused());
        } break;
        }
    default:
        break;
    }
}