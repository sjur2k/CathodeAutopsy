#pragma once

#include <glm/glm.hpp>

#include <vector>
#include <string>
#include <cstddef>

class PointCloud {
    public:
        PointCloud() = default;

        // Data input handling
        static PointCloud load_CSV(const std::string& path);
        static PointCloud load_E57(const std::string& path);

        // Accessors
        const std::vector<glm::vec3>& points() const {return points_;}
        size_t size() const {return points_.size();}
    
    private:
        std::vector<glm::vec3> points_;
};