#include "input_manager.hpp"
#include "camera.hpp"

InputManager::InputManager(GLFWwindow* window, Camera& camera) : window_(window), camera_(camera) {
    glfwSetWindowUserPointer(window_, this);
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window_, mouse_callback);
    glfwSetMouseButtonCallback(window_, mouse_button_callback);
}

void InputManager::process_input(float delta_time) {
    if (glfwGetKey(window_, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window_, true);
    }
    process_keyboard(delta_time);
}

void InputManager::process_keyboard(float delta_time) {
    float velocity = movement_speed_multiplier_ * delta_time;
    Pose current_pose = camera_.get_pose();
    
    float yaw_rad = glm::radians(current_pose.rotation.yaw);
    float pitch_rad = glm::radians(current_pose.rotation.pitch);
    
    Position unit_forward(
        cos(pitch_rad) * sin(yaw_rad),
        +sin(pitch_rad),
        -cos(pitch_rad) * cos(yaw_rad)
    );

    Position unit_right(
        cos(yaw_rad),
        0.0f,
        -sin(yaw_rad)
    );

    Position unit_up(0.0f,0.0f,1.0f);
    
    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS){
        current_pose.position += unit_forward*velocity;
    }
    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS){
        current_pose.position -= unit_forward*velocity;
    }
    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS){
        current_pose.position -= unit_right*velocity;
    }
    if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS){
        current_pose.position += unit_right*velocity;
    }
    if (glfwGetKey(window_, GLFW_KEY_SPACE) == GLFW_PRESS){
        current_pose.position += unit_up*velocity;
    }
    if (glfwGetKey(window_, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
        current_pose.position -= unit_up*velocity;
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

void InputManager::mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    InputManager* input_manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (!input_manager) return;
    if (input_manager->orbiting_){
        input_manager->orbit(xpos, ypos);
    }    
}

void InputManager::mouse_button_callback(GLFWwindow* window, int button, int action, int mods){
    InputManager* input_manager = static_cast<InputManager*>(glfwGetWindowUserPointer(window));
    if (!input_manager) return;
    if (button == GLFW_MOUSE_BUTTON_LEFT){
        if (action == GLFW_PRESS){
            input_manager->orbiting_ = true;
            input_manager->first_mouse_ = true;
        } else if (action == GLFW_RELEASE){
            input_manager->orbiting_ = false;
            input_manager->first_mouse_ = false;
        }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS){
        input_manager->camera_.reset_pose();
    }
}