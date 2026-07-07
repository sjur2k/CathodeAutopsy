#pragma once

#include "geometry.hpp"
#include "grid.hpp"
#include <glm/glm.hpp>

class Camera {
    private:
        Pose pose;
        Pose initial_pose;
        float fov;
        float aspect_ratio;
        float near_plane;
        float far_plane;
    public:
        Camera(
            Pose initial_pose = Pose(),
            float field_of_view = 90.0f,
            float aspect_ratio = 1.0f,
            float near_plane = 0.1f,
            float far_plane = 100.0f
        ) : pose(initial_pose), initial_pose(initial_pose), fov(field_of_view), 
        aspect_ratio(aspect_ratio), near_plane(near_plane), far_plane(far_plane) {};
        
        Pose get_pose() const;
        void set_pose(const Pose& new_pose);
        void reset_pose();
        void print_pose();
        glm::mat4 get_view_matrix() const;
        glm::mat4 get_projection_matrix() const;
};