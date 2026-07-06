#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window {
    public:
        Window(int width, int height, const std::string& title);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        bool should_close() const;
        void swap_buffers();

        GLFWwindow* get_handle() const { return window_; }
        int width() const { return width_; }
        int height() const { return height_; }

    private:
        GLFWwindow* window_ = nullptr;
        int width_, height_;
};