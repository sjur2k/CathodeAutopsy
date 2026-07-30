#pragma once

#include "ui/ui_geometry.hpp"

class PauseScreen {
    public:
        explicit PauseScreen(UIContext ui_ctx) : ui_ctx_(ui_ctx) {}
    
        // Core behavior
        bool update(); // Returns true if redraw is needed
        void draw();

        // Accessors
        bool should_exit();
    
    private:
        UIContext ui_ctx_;
        bool should_exit_ = false;
};