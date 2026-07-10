#pragma once

#include "geometry.hpp"
#include "grid.hpp"
#include <glm/glm.hpp>

class Camera {
    private:
        Pose pose_;
        Pose initial_pose_;
        float fov_;
        float aspect_ratio_;
        float near_plane_;
        float far_plane_;
    public:
        Camera(
            Pose initial_pose = Pose(),
            float field_of_view = 90.0f,
            float aspect_ratio = 1.0f,
            float near_plane = 0.1f,
            float far_plane = 100.0f
        ) : pose_(initial_pose), initial_pose_(initial_pose), fov_(field_of_view), 
        aspect_ratio_(aspect_ratio), near_plane_(near_plane), far_plane_(far_plane) {};
        
        Pose get_pose() const;
        void set_pose(const Pose& new_pose);
        void reset_pose();
        void print_pose();
        glm::mat4 get_view_matrix() const;
        glm::mat4 get_projection_matrix() const;
};