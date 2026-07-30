#pragma once

#include "rendering/camera.hpp"
#include "rendering/renderer.hpp"
#include "ui/ui_geometry.hpp"
#include "rendering/text_renderer.hpp"

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

    private:
        UIContext ui_ctx_;
        Camera& camera_;
        Renderer& point_cloud_renderer_;
};