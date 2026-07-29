#include "window.hpp"
#include "glfw_context.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <string>

Window::Window(int width, int height, const std::string& title, GLFWUserContext* context) :
    width_(width), height_(height){
    if (!glfwInit()){
        throw std::runtime_error("Failed to initialize GLFW");
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    int work_x = 0, work_y = 0, work_w = width_, work_h = height_;
    if (primary) {
        glfwGetMonitorWorkarea(primary, &work_x, &work_y, &work_w, &work_h);
    }
    bool fits = (work_w >= width_) && (work_h >= height_);
    if (!fits) {
        glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    }

    window_ = glfwCreateWindow(width_, height_, title.c_str(), nullptr, nullptr);
    if (!window_) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    
    if (fits && primary){
        int window_x = work_x + (work_w - width_ ) / 2;
        int window_y = work_y + (work_h - height_ ) / 2;
        glfwSetWindowPos(window_, window_x, window_y);
    }

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1); // Enables V-sync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLAD");
    }
    context->window = this;
    glfwSetWindowUserPointer(window_, context);
    glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
    glfwSetWindowRefreshCallback(window_, window_refresh_callback);

    glfwGetFramebufferSize(window_, &width_, &height_);
    glViewport(0, 0, width_, height_);
}

Window::~Window() {
    if (window_) {
        glfwDestroyWindow(window_);
    }
    glfwTerminate();
}


// Static callbacks (GLFW C API)
void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height){
    auto* ctx = static_cast<GLFWUserContext*>(glfwGetWindowUserPointer(window));
    if(!ctx || !ctx->window) return;
    Window* self = ctx->window;

    self->width_ = width;
    self->height_ = height;

    if (width == 0 || height == 0) return;

    glViewport(0, 0, width, height);
    if (self->resize_callback_){
        self->resize_callback_(width, height);
    }
}

void Window::window_refresh_callback(GLFWwindow* window){
    auto* ctx = static_cast<GLFWUserContext*>(glfwGetWindowUserPointer(window));
    if(!ctx || !ctx->window) return;
    Window* self = ctx->window;
    if (self->refresh_callback_) self->refresh_callback_();
}