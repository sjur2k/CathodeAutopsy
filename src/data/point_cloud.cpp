#include "data/point_cloud.hpp"
#include "core/paths.hpp"

#include <glm/glm.hpp>
#include <E57Format/E57SimpleData.h>
#include <E57Format/E57SimpleReader.h>
#include <filesystem>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cctype>

namespace {
    constexpr int64_t kChunkSize = 65536;
}

void PointCloud::load(const std::string& path){
    std::string ext = std::filesystem::path(path).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(),
        [](unsigned char c){ return std::tolower(c);});
    
    if (ext == ".csv"){
        return load_CSV(path);
    }
    if (ext == ".e57"){
        return load_E57(path);
    }
    throw std::runtime_error("Unsupported point cloud file type: " + ext);
}

void PointCloud::load_CSV(const std::string& path){
    std::ifstream file(path);
    if (!file.is_open()){
        throw std::runtime_error("CSV file: " + path + " is open in another program.");
    }

    points_.clear();
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

            if(yz_swap_){
                points_.emplace_back(x, z, y);
            } else {
                points_.emplace_back(x, y, z); // OpenGL convention
            }
            
        } catch(const std::exception& e) {
            continue; // Skip malformed rows
        }   
    }
    if (points_.empty()){
        throw std::runtime_error("No valid data found in: " + path);
    }
}

void PointCloud::load_E57(const std::string& path){
    points_.clear();
    try {
        e57::ReaderOptions options;
        options.checksumPolicy = 0;
        e57::Reader reader(path, options);
        
        int64_t scan_count = reader.GetData3DCount();
        
        if (scan_count == 0){
            throw std::runtime_error("No 3D scans found in: " + path);
        }

        for (int64_t scan_index = 0; scan_index < scan_count; ++scan_index){
            e57::Data3D scan_header;
            reader.ReadData3D(scan_index, scan_header);

            if (!scan_header.pointFields.cartesianXField ||
                !scan_header.pointFields.cartesianYField ||
                !scan_header.pointFields.cartesianZField){
                continue;
            }

            bool has_invalid_state = scan_header.pointFields.cartesianInvalidStateField;

            std::vector<double> x_buf(kChunkSize), y_buf(kChunkSize), z_buf(kChunkSize);
            std::vector<int8_t> valid_buf(kChunkSize);

            e57::Data3DPointsDouble buffers;
            buffers.cartesianX = x_buf.data();
            buffers.cartesianY = y_buf.data();
            buffers.cartesianZ = z_buf.data();
            if (has_invalid_state){
                buffers.cartesianInvalidState = valid_buf.data();
            }

            e57::CompressedVectorReader data_reader =
                reader.SetUpData3DPointsData(scan_index, kChunkSize, buffers);

            size_t n_read = 0;
            while ((n_read = data_reader.read()) > 0){
                for (size_t i = 0; i < n_read; ++i){
                    if (has_invalid_state && valid_buf[i] != 0) continue;
                    float x = static_cast<float>(x_buf[i]);
                    float y = static_cast<float>(y_buf[i]);
                    float z = static_cast<float>(z_buf[i]); 
                    if (yz_swap_){
                        points_.emplace_back(x,z,y);
                    } else {
                        points_.emplace_back(x,y,z); // OpenGL standard
                    }
                }
            }
            data_reader.close();
        }

        reader.Close();
    } catch (const e57::E57Exception& ex) {
        throw std::runtime_error("E57 error while reading: " + path + " - " + std::string(ex.errorStr()));
    }

    if (points_.empty()){
        throw std::runtime_error("No valid points found in: " + path);
    }
}

PointCloudBounds PointCloud::compute_bounds() const {
    if (points_.empty()){
        throw std::runtime_error("Cannot compute bounds of empty point cloud.");
    }

    glm::vec3 min_point = points_[0];
    glm::vec3 max_point = points_[0];

    for (const auto& p : points_){
        min_point = glm::min(min_point, p);
        max_point = glm::max(max_point, p);
    }
    glm::vec3 center = (min_point + max_point) * 0.5f;
    float radius = glm::length(max_point - center);
    
    // Degenerate case
    if (radius < 1e-4f){
        radius = 1.0f;
    }
    float half_extent_x = (max_point.x - min_point.x) * 0.5f;
    float half_extent_z = (max_point.z - min_point.z) * 0.5f;

    return {center, radius, min_point.y, half_extent_x, half_extent_z};
}