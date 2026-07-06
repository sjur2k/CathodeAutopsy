#include <vector>
#include <random>
#include <cmath>
#include <fstream>
#include <algorithm>
#include <cstdint>
#include <iostream>

#include "grid.hpp"
#include "geometry.hpp"
using namespace std;

float hash2(int x, int y){
    uint32_t h = (uint32_t)(x * 374761393u + (uint32_t)(y * 668265263u));
    h = (h ^ (h >> 13)) * 1274126177u;
    h ^= h >> 16;
    return (h & 0xFFFFFF) / (float)0x1000000;
}

float smooth(float t){
    return t * t * (3.0f - 2.0f * t);
}

float valueNoise(float x, float y){
    int x0 = (int)floor(x);
    int x1 = x0 + 1;
    int y0 = (int)floor(y);
    int y1 = y0 + 1;

    float sx = smooth(x - (float)x0);
    float sy = smooth(y - (float)y0);

    float n0 = hash2(x0, y0);
    float n1 = hash2(x1, y0);
    float ix0 = n0 + sx * (n1 - n0);

    n0 = hash2(x0, y1);
    n1 = hash2(x1, y1);
    float ix1 = n0 + sx * (n1 - n0);

    return ix0 + sy * (ix1 - ix0);
}

float fbm(float x, float y, int octaves, float persistence = 0.5f) {
    float sum = 0, amp = 0.5f, freq = 1.0f, norm = 0;
    for (int k = 0; k < octaves; ++k) {
        sum  += amp * valueNoise(x * freq, y * freq);
        norm += amp;
        freq *= 2.0f;   // lacunarity — finer features each octave
        amp  *= persistence;   // persistence — how much they contribute
    }
    return sum / norm;  // back to ~[0,1]
}

Grid::Grid(int r, int c, Pose origin) {
    rows = r;
    cols = c;
    grid_serialized = std::vector<float>(r * c);
    relative_origin = origin;
}
void Grid::fill_random_smooth(){
    float scale = 1.0f / 32.0f;
    int octaves = 6;
    float persistence = 0.4f;
    float z_scale = 10.0f;
    for (int j = 0; j < rows; j++){
        for (int i = 0; i < cols; i++){
            grid_serialized[j * cols + i] = z_scale*fbm(i*scale, j*scale, octaves, persistence);
        }
    }
}
float Grid::get_value(int i, int j) {
    return grid_serialized[j * cols + i];
}


void Grid::print_grid() {
    for (int j = 0; j < rows; j++){
        for (int i = 0; i < cols; i++){
            printf("%f ", get_value(i, j));
        }
        printf("\n");
    }
}

void Grid::write_grid_to_PPM() {
    ofstream out("data/grid.ppm", ios::binary);
    float minVal = *min_element(grid_serialized.begin(), grid_serialized.end());
    float maxVal = *max_element(grid_serialized.begin(), grid_serialized.end());
    float range = (maxVal - minVal) > 1e-6f ? (maxVal - minVal) : 1.0f;
    out << "P6\n" << cols << " " << rows << "\n255\n";
    int BNW = 1; // Set to 1 for black and white, 0 for color
    for (int k = 0; k < cols * rows; ++k) {
        float t = (grid_serialized[k] - minVal) / range; // normalize
        if (BNW == 1){
            unsigned char gray = static_cast<unsigned char>(t * 255);
            out.put(gray).put(gray).put(gray);
        } else {
            unsigned char r = static_cast<unsigned char>(t * 255);
            unsigned char b = static_cast<unsigned char>((1 - t) * 255);
            unsigned char g = static_cast<unsigned char>(64);
            out.put(r).put(g).put(b);
        }
    }
    out.close();
}

void Grid::write_grid_to_csv(float res){
    ofstream out("data/grid.csv");
    out <<"#"<<"rows"<<","<<rows<<","<<"cols"<<","<<cols<<","<<"resolution"<<","<<res<<"\n";
    out << "x,y,z\n";
    for (int j = 0; j < rows; j++){
        for (int i = 0; i < cols; i++){
            out << i << "," << j << "," << get_value(i,j) << "\n";
        }
    }
    out.close();
}

std::vector<glm::vec3> Grid::get_point_cloud_vec3() {
    std::vector<glm::vec3> point_cloud;
    point_cloud.reserve(rows * cols);
    for (int j = 0; j < rows; j++) {
        for (int i = 0; i < cols; i++) {
            float x = i + relative_origin.position.x;
            float y = j + relative_origin.position.y;
            float z = get_value(i, j) + relative_origin.position.z;
            point_cloud.emplace_back(x, y, z);
        }
    }
    return point_cloud;
}