#pragma once

#include "rendering/camera.hpp"
#include "rendering/renderer.hpp"
#include "ui/ui_geometry.hpp"
#include "rendering/text_renderer.hpp"

#include <glm/glm.hpp>

class RunningScreen {
    public:
        RunningScreen(
            UIContext ui_ctx,
            Camera& camera,
            Renderer& point_cloud_renderer_
        );

        // Core behavior
        void update(float delta_time);
        void draw();

        // Mutating operations
        void set_floor_extent(const Position& center, float half_width, float half_length);

        // Helpers
        glm::mat4 build_floor_model() const;
        
    private:
        UIContext ui_ctx_;
        Camera& camera_;
        Renderer& point_cloud_renderer_;
        Position floor_center_;
        float floor_half_width_;
        float floor_half_length_;
};