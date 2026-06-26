#include <vector>
#include <random>
#include <iostream>

#include "grid.hpp"
using namespace std;

Grid::Grid(int w, int h) : width(w), height(h), grid_serialized(w * h, 0.0) {
    random_device rd;
    mt19937 gen(rd());
    normal_distribution<> dis(0.0, 1.0);
    for(int i = 0; i < w * h; i++){
        this->grid_serialized[i] = dis(gen);
    }
}

double Grid::get_value(int x, int y) {
    return this->grid_serialized[y * width + x];
}