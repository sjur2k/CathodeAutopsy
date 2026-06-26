#pragma once
#include <vector>
using namespace std;

class Grid{
    public:
        int width; // meters
        int height; // meters
        double get_value(int x, int y);
        Grid(int w, int h);
    private:
        vector<double> grid_serialized;
};