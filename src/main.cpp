#include <stdio.h>
#include <cmath>
#include "grid.hpp"

int main(){
    //Assumed constants (in SI-units):
    float cell_width = 10.0f; //Corresponds to rows (y)
    float cell_length = 4.0f; //Corresponds to columns (x)
    float resolution = 0.05f; // Laser scans every 5cm
    Grid grid(cell_width/resolution, cell_length/resolution);
    grid.fill_random_smooth();
    //grid.print_grid();
    grid.write_grid_to_PPM();
    grid.write_grid_to_csv(resolution);
    return 0;
}