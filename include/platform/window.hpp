#pragma once

#include <glad.h>
#include <GLFW/glfw3.h>
#include <glfw_context.hpp>

#include <string>
#include <functional>


class Window {
    public:
        Window(int width, int height, const std::string& title, GLFWUserContext* context);
        ~Window();

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;

        // Core behavior
        bool should_close() const { return glfwWindowShouldClose(window_); };
        void swap_buffers() { glfwSwapBuffers(window_); };
        
        // Accessors
        GLFWwindow* get_handle() const {return window_;}
        int width() const {return width_;}
        int height() const {return height_;}

        // Callbacks
        using ResizeCallback = std::function<void(int,int)>;
        void set_resize_callback(ResizeCallback cb) {
            resize_callback_ = std::move(cb);
        }

        using RefreshCallback = std::function<void()>;
        void set_refresh_callback(RefreshCallback cb){
            refresh_callback_ = std::move(cb);
        }

    private:
        // Static callbacks (GLFW C API)
        static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
        static void window_refresh_callback(GLFWwindow* window);

        GLFWwindow* window_ = nullptr;
        int width_, height_;
        
        ResizeCallback resize_callback_;
        RefreshCallback refresh_callback_;
};