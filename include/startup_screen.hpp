#pragma once

#include "text_renderer.hpp"
#include "texture.hpp"
#include "ui_geometry.hpp"

#include <string>

class StartupScreen{
    public:
        StartupScreen(
            UIContext ui_ctx,
            TextRenderer& display_text_renderer,
            Texture& logo_texture,
            Texture& close_btn_texture,
            int& window_width,
            int& window_height
        );

        // Core behavior
        bool update();
        void draw();

        // Accessors
        bool finished() const { return startup_finished_; }
        bool should_load_point_cloud() const { return file_loaded_ && use_file_data_; }
        const std::string& file_path() const { return loaded_file_path_; }

    private:
        UIContext ui_ctx_;
        TextRenderer& display_text_renderer_;
        Texture& logo_texture_;
        Texture& close_btn_texture_;
        int& window_width_;
        int& window_height_;

        bool display_info_ = false;
        bool file_loaded_ = false;
        bool use_file_data_ = true;
        bool startup_finished_ = false;
        std::string loaded_file_path_;
        std::string file_name_;
};