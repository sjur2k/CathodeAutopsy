#pragma once
#include <vector>
using namespace std;

class Grid{
    public:
        Grid(int r, int c) : rows(r), cols(c), grid_serialized(r * c) {}
        float get_value(int i, int j);
        void fill_random_smooth();
        void print_grid();
        void write_grid_to_PPM();
        void write_grid_to_csv(float res);
    private:
        int rows, cols;
        vector<float> grid_serialized;
};