#pragma once

#include "transform.hpp"

#include <glm/glm.hpp>

class Camera {
    public:
        Camera(
            Pose initial_pose = Pose(),
            float field_of_view = 90.0f,
            float aspect_ratio = 1.0f,
            float near_plane = 0.1f,
            float far_plane = 100.0f
        ) : pose_(initial_pose), initial_pose_(initial_pose), fov_(field_of_view), 
        aspect_ratio_(aspect_ratio), near_plane_(near_plane), far_plane_(far_plane) {};
        
        // Mutating operations
        void set_pose(const Pose& new_pose) {pose_ = new_pose;}
        void reset_pose() {pose_ = initial_pose_;}
        void set_aspect_ratio(float aspect_ratio){aspect_ratio_ = aspect_ratio;}

        // Accessors
        Pose get_pose() const {return pose_;}
        glm::mat4 get_view_matrix() const;
        glm::mat4 get_projection_matrix() const;
    
        // Debug
        void print_pose() const;

    private:
        Pose pose_;
        Pose initial_pose_;
        float fov_;
        float aspect_ratio_;
        float near_plane_;
        float far_plane_;
};