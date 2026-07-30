#include "data/point_cloud.hpp"

#include <glm/glm.hpp>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

PointCloud PointCloud::load_CSV(const std::string& path){
    std::ifstream file(path);
    if (!file.is_open()){
        throw std::runtime_error("CSV file: " + path + " is open in another program.");
    }
    PointCloud cloud;
    std::string line;

    while (std::getline(file, line)){
        if (line.empty() || line[0] == '#') continue;
        std::stringstream ss(line);
        std::string x_str, y_str, z_str;
        if (!std::getline(ss, x_str, ',')) continue;
        if (!std::getline(ss, y_str, ',')) continue;
        if (!std::getline(ss, z_str, ',')) continue;
        if (x_str == "x" && y_str == "y" && z_str == "z") continue;
        try{
            float x = std::stof(x_str);
            float y = std::stof(y_str);
            float z = std::stof(z_str);
            // To follow opengl convention, z and y are swapped
            cloud.points_.emplace_back(x, z, y);
        } catch(const std::exception& e) {
            continue; // Skip malformed rows. Uncomment under to see error
            //std::cerr << e.what() << '\n';
        }   
    }
    if (cloud.points_.empty()){
        throw std::runtime_error("No valid data found in: " + path);
    }
    return cloud;
}

PointCloud PointCloud::load_E57(const std::string& path){
    (void)path;
    return PointCloud{};
}