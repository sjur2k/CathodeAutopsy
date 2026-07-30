#pragma once

#include "rendering/text_renderer.hpp"
#include "rendering/texture.hpp"
#include "ui/ui_geometry.hpp"

#include <string>

class StartupScreen{
    public:
        StartupScreen(
            UIContext ui_ctx,
            Texture& logo_texture,
            Texture& close_btn_texture
        );

        // Core behavior
        bool update();
        void draw();

        // Accessors
        bool finished();
        bool should_load_point_cloud() const { return file_loaded_ && use_file_data_; }
        const std::string& file_path() const { return loaded_file_path_; }

    private:
        UIContext ui_ctx_;
        Texture& logo_texture_;
        Texture& close_btn_texture_;

        bool display_info_ = false;
        bool file_loaded_ = false;
        bool use_file_data_ = true;
        bool startup_finished_ = false;
        std::string loaded_file_path_;
        std::string file_name_;
};