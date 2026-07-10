#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "geometry.hpp"

struct Dimensions{
    int rows,cols;
};

class Grid{
    public:
        Grid(int rows, int cols, Pose relative_origin = Pose()) : 
            rows_(rows),
            cols_(cols),
            grid_serialized_(std::vector<float>(rows * cols)),
            relative_origin_(relative_origin) 
        {}
        Dimensions get_dimensions() const;
        float get_value(int i, int j) const;
        void fill_random_smooth();
        void print_grid();
        void write_grid_to_PPM();
        void write_grid_to_csv(float res);
        std::vector<glm::vec3> get_point_cloud_vec3();
    private:
        int rows_, cols_;
        Pose relative_origin_;
        std::vector<float> grid_serialized_;
};