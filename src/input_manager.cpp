#include "input_manager.hpp"
#include "camera.hpp"

InputManager::InputManager(GLFWwindow* window, Camera& camera) : window_(window), camera_(camera) {
    glfwSetWindowUserPointer(window_, this);
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window_, mouse_callback);
    glfwSetMouseButtonCallback(window_, mouse_button_callback);
    glfwSetKeyCallback(window_, key_callback);
    //glfwSetScrollCallback(window_, scroll_callback);
}

bool InputManager::is_paused(){return paused_;}

void InputManager::set_paused(bool value){
    paused_ = value;
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
    
    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS){
        current_pose.position += unit_forward*velocity;
    }
    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS){
        current_pose.position -= unit_right*velocity;
    }
    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS){
        current_pose.position -= unit_forward*velocity;
    }
    if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS){
        current_pose.position += unit_right*velocity;
    }
    if (glfwGetKey(window_, GLFW_KEY_SPACE) == GLFW_PRESS){
        current_pose.position += unit_up*velocity;
    }
    if (glfwGetKey(window_, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
        if (current_pose.position.y > 1){
            current_pose.position -= unit_up*velocity;
        }
    }
    camera_.set_pose(current_pose);
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

void InputManager::set_mode(InputMode mode){
    mode_ = mode;
}

InputMode InputManager::get_mode() const {
    return mode_;
}

void InputManager::set_button_box(UIAction action, UIBox box){
    ui_buttons_[action] = box;
}

std::optional<UIAction> InputManager::consume_triggered_action(){
    auto result = triggered_action_;
    triggered_action_.reset();
    return result;
}

bool InputManager::consume_open_file_request(){
    bool was_requested = open_file_requested_;
    open_file_requested_ = false;
    return was_requested;
}

void InputManager::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    InputManager* input_manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (!input_manager) return;
    if (input_manager->orbiting_){
        input_manager->orbit(xpos, ypos);
    }    
}

void InputManager::mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    (void)mods; // Suppress unused-variable-warnings
    InputManager* input_manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (!input_manager) return;
    if (input_manager->is_paused()) return;

    // check different button actions
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS){
        if (input_manager->get_mode() == InputMode::Interactive){
            double x,y;
            glfwGetCursorPos(window,&x,&y);
            auto it = input_manager->page_buttons_.find(input_manager->active_page_);
            if (it == input_manager->page_buttons_.end()){return;}
            for (auto& [action_id, box] : it->second){
                if (box.contains(x,y)){
                    input_manager->triggered_action_ = action_id;
                    break;
                }
            }
        } else if (input_manager->get_mode() == InputMode::Locked){
            input_manager->orbiting_ = true;
            input_manager->first_mouse_ = true;
        }
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
        input_manager->orbiting_ = false;
        input_manager->first_mouse_ = false;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
        if (input_manager->get_mode() == InputMode::Locked){
            input_manager->camera_.reset_pose();
        }
    }    
}

void InputManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    (void)scancode; (void)mods; // Suppress unused-variable-warnings
    InputManager* input_manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if(!input_manager) return;

    if(action == GLFW_PRESS){
        switch(key){
            case GLFW_KEY_T:
                input_manager->camera_.print_pose();
                break;
            case GLFW_KEY_Q:
                glfwSetWindowShouldClose(window, true);
                break;
            case GLFW_KEY_ESCAPE:
                if (input_manager->is_paused()){
                    input_manager->set_paused(false); // UNPAUSE
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                } else {
                    input_manager->set_paused(true); // PAUSE
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                }
        }
    }
}

/* void InputManager::scroll_callback(GLFWwindow* window, double xoffset, double yoffset){

} */