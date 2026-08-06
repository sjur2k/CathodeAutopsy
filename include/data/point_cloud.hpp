#pragma once

#include "data/axis_convention.hpp"

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
        AxisConvention get_axis_convention() {return axis_conv_; }

        // Mutating operations
        void set_axis_convention(AxisConvention conv){ axis_conv_ = conv; }

    private:
        void load_CSV(const std::string& path);
        void load_E57(const std::string& path);
        
        std::vector<glm::vec3> points_;
        AxisConvention axis_conv_ = AxisConvention::XYZ;
};