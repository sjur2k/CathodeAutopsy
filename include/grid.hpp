#pragma once
#include "geometry.hpp"
#include <glm/glm.hpp>
#include <vector>

struct Dimensions{
    int rows,cols;
};

class Grid{
    public:
        Grid(int rows, int cols, Pose relative_origin = Pose()) : 
            rows_(rows),
            cols_(cols),
            relative_origin_(relative_origin),
            grid_serialized_(std::vector<float>(rows * cols))
        {}
        
        // Mutating operations
        void fill_random_smooth();
        std::vector<glm::vec3> generate_random_point_cloud();
        
        // Accessors
        Dimensions get_dimensions() const;
        float get_value(int i, int j) const;
        std::vector<glm::vec3> get_point_cloud_vec3() const;
        
        // Exports
        void write_grid_to_PPM() const;
        void write_grid_to_csv(float res) const;

    private:
        int rows_, cols_;
        Pose relative_origin_;
        std::vector<float> grid_serialized_;
};