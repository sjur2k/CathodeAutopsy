#pragma once

#include <glm/glm.hpp>

#include <vector>
#include <string>
#include <cstddef>

struct PointCloudBounds {
    glm::vec3 center;
    float radius;
    float min_y;
    float half_extent_x;
    float half_extent_z;
};

class PointCloud {
    public:
        PointCloud() = default;

        // Data input handling
        void load(const std::string& path);
        
        // Accessors
        const std::vector<glm::vec3>& points() const {return points_;}
        PointCloudBounds compute_bounds() const;

        // Mutating operations
        void set_yz_swap(bool yz_swap){ yz_swap_ = yz_swap; }

    private:
        void load_CSV(const std::string& path);
        void load_E57(const std::string& path);
        
        std::vector<glm::vec3> points_;
        bool yz_swap_ = false;
};