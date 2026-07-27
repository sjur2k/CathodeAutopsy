#define GLFW_EXPOSE_NATIVE_WIN32
#include "window.hpp"
#include "glfw_context.hpp"
#include <GLFW/glfw3native.h>
#include <windows.h>
#include <stdexcept>
#include <algorithm>

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
    needs_initial_fullscreen_ = !fits;
    
    window_ = glfwCreateWindow(width_, height_, title.c_str(), nullptr, nullptr);
    if (!window_) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    
    if (primary){
        int window_x = work_x + std::max(0, (work_w - width_) / 2);
        int window_y = work_y + std::max(0, (work_h - height_) / 2);
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

Window::~Window() {
    if (window_) {
        glfwDestroyWindow(window_);
    }
    glfwTerminate();
}

bool Window::should_close() const {
    return glfwWindowShouldClose(window_);
}

void Window::swap_buffers() {
    glfwSwapBuffers(window_);
}

void Window::toggle_fullscreen(){
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if(!monitor) return;
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if(!mode) return;

    if(!is_fullscreen_){
        glfwGetWindowPos(window_, &windowed_x_, &windowed_y_);
        glfwGetWindowSize(window_, &windowed_w_, &windowed_h_);
        glfwSetWindowMonitor(
            window_, monitor, 0, 0, mode->width, mode->height, mode->refreshRate
        );
        is_fullscreen_ = true;
    } else {
        glfwSetWindowMonitor(
            window_, nullptr, windowed_x_, windowed_y_, windowed_w_, windowed_h_, 0
        );
        HWND hwnd = glfwGetWin32Window(window_);
        SetWindowPos(hwnd, nullptr, 0, 0, 0, 0,
            SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE
        );
        is_fullscreen_ = false;
    }
}