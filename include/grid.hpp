#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "geometry.hpp"

class Grid{
    public:
        Grid(int r, int c, Pose origin = Pose());
        float get_value(int i, int j);
        void fill_random_smooth();
        void print_grid();
        void write_grid_to_PPM();
        void write_grid_to_csv(float res);
        std::vector<glm::vec3> get_point_cloud_vec3();
    private:
        int rows, cols;
        Pose relative_origin;
        std::vector<float> grid_serialized;
};